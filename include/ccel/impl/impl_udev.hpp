#ifndef __CCEL_IMPL_UDEV_HPP__
#define __CCEL_IMPL_UDEV_HPP__
#include "common.hpp"

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/unordered_map.hpp>

#include <fcntl.h>
#include <linux/uinput.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <string>

namespace ccel {
namespace udev {
const static bool KEY_PRESS = true; // IN FACT IT IS 1,just for human readable
const static bool KEY_RELEASE = false; // 0

// TODO: ADD QtKey Bindings

class virtual_keyboard_handler : base_handler {
public:
  virtual_keyboard_handler(boost::asio::io_service &io,
                           const std::string dev_name = "ccel_vkey",
                           const int flags = (O_WRONLY | O_NDELAY));
  ~virtual_keyboard_handler();
  int open();
  int close();
  void press(int key_code);
  void release(int key_code);
  void click(int key_code);
  void set_click_time(
      int key_code,
      unsigned click_time); // Set the time holding button using click in ms
  const unsigned get_click_time(int key_code);

private:
  void sync(input_event &__ev);
  boost::unordered_map<int, boost::tuple<bool, unsigned>> __key_proprety;
  // [KEY_VALUE] = <PRESSED, CLICK_TIME_MS>
  uinput_user_dev _udev; // The raw handler
  const std::string _dev_name;
  int _fd; // Raw file descriptor
  bool _open;
};
}
}
#endif
