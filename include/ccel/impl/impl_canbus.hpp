#ifndef __CCEL_IMPL_CANBUS_HPP__
#define __CCEL_IMPL_CANBUS_HPP__
#include "common.hpp"
#include <boost/asio.hpp>

// Bring SocketCAN header in
#include <linux/can.h>
#include <linux/can/bcm.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace ccel {
namespace canbus {
typedef boost::asio::posix::stream_descriptor canbus_stream;
typedef boost::asio::posix::stream_descriptor::native_handle_type cansock_type;
cansock_type open_cansock(const char *bus_name,
                          const int protocol); // Open a SocketCAN socket
const static can_frame __EMPTY_CAN_FRAME = {};
const bool test_id_bit(canid_t can_id, std::size_t index);
const bool ext_frame_test(can_frame &frame); // Test whether a frame is EXTENDED

class canbus_handler : public base_handler {
public:
  canbus_handler(const char *interface_name, boost::asio::io_service &io);
  canbus_handler(cansock_type &can_sock, boost::asio::io_service &io);
  int bind_sock(cansock_type &cansock);
  int open();
  int open(const char *interface_name); // Open with a new interface
  int refresh();                        // Clear the frame buffer
  const bool loopback() noexcept;
  const bool loopback(const bool turn);
  can_frame &read_sock();            // Do read from sock
  void write_sock(can_frame &frame); // Do write to sock;
  template <typename _T_Func, typename... Args>
  can_frame &
  async_read_sock(_T_Func &__callback,
                  Args &&... __args) { // Read from the sock asynchronous
    if (opened()) {
      _stream.async_read_some(
          boost::asio::buffer(&_frame, sizeof(_frame)),
          boost::bind(__callback, std::forward<Args>(__args)...));
      __handler_io.poll();
      return _frame;
    } else {
      throw std::runtime_error("[CANBUS_Handler]Handler's socket not open.");
    }
  }
  int close();
  // void refresh();               // Clear the buffer
  std::string interface_name(); // Get interface_name
  canbus_stream &stream();      // Get the stream
private:
  bool _loopback;
  canbus_stream _stream;
  can_frame _frame; // the canbus frame buffer
  std::string _interface_name;
};
}
}

#endif
