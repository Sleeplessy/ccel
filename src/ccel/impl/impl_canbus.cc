#include <ccel/impl/impl_canbus.hpp>

namespace ccel {
namespace canbus {

const bool test_id_bit(canid_t can_id, std::size_t index) {
  std::bitset<32> id_holder(can_id);
  return id_holder.test(index);
}

const bool ext_frame_test(can_frame &frame) {
  return test_id_bit(frame.can_id, 31);
}

boost::asio::posix::stream_descriptor::native_handle_type
open_cansock(const char *bus_name,const int protocol=CAN_RAW) {
  sockaddr_can addr;
  ifreq ifr;
  int can_sock = socket(PF_CAN, SOCK_RAW, protocol);
  std::strcpy(ifr.ifr_name, bus_name);
  if (ioctl(can_sock, SIOCGIFINDEX, &ifr) < 0)
    throw std::runtime_error(
        "[CANBus_Handler]Error while syncing io with system.");
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  std::string interface_name{ifr.ifr_name};
  if (bind(can_sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
    throw std::runtime_error("[CANBus_Handler]Error while opening cansock.");
  }
  return can_sock;
}

canbus_handler::canbus_handler(const char *interface_name,
                               boost::asio::io_service &io)
  : base_handler(io, sizeof(can_frame)), _loopback(false),_stream(__handler_io),
     _frame(can_frame{}), _interface_name{interface_name} {
  this->open(interface_name);
}

canbus_handler::canbus_handler(cansock_type &can_sock,
                               boost::asio::io_service &io)
    : base_handler(io, sizeof(can_frame)), _loopback(false),
      _stream(__handler_io), _frame(can_frame{}) {
  bind_sock(can_sock);
}

int canbus_handler::open() {
  if (!_stream.is_open()) {
    open(_interface_name.c_str());
  }
  return 0;
}

int canbus_handler::bind_sock(cansock_type &cansock) {
  boost::system::error_code __tmp_ec_holder;
  _stream.assign(cansock, __tmp_ec_holder);
  if (__tmp_ec_holder) {
    return -1;
  }
  return 0;
}

int canbus_handler::open(const char *interface_name) {
  try {
    auto __tmp_sock_holder = open_cansock(interface_name);
    boost::system::error_code __tmp_ec_holder;
    _stream.assign(__tmp_sock_holder, __tmp_ec_holder);
    if (__tmp_ec_holder)
      throw std::logic_error(
          "[CANBUS_Handler]boost_basic_stream assign error happend.");
    _open = true;
    _interface_name = interface_name;
    return 0;
  } catch (std::runtime_error &_e) {
    return -1;
  } catch (...) {
    throw;
  }
}

can_frame &canbus_handler::read_sock() {
  _stream.read_some(boost::asio::buffer(&_frame, sizeof(_frame)));
  return _frame;
}

int canbus_handler::close() {
  if (_open) {
    boost::system::error_code __tmp_ec_holder;
    _stream.close(__tmp_ec_holder);
    if (__tmp_ec_holder)
      return -1;
    _open = false;
  }
  return 0;
}

int canbus_handler::refresh() {
  _frame = can_frame{};
  return 0;
}

const bool canbus_handler::loopback() noexcept { return _loopback; }

const bool canbus_handler::loopback(const bool turn) {
  if(setsockopt(_stream.native_handle(), SOL_CAN_RAW, CAN_RAW_LOOPBACK, &turn,
                sizeof(turn)) < 0)
    throw std::runtime_error("[CANBus Handler]Enabling loopback failed.");
  _loopback = turn;
  return _loopback;
}

std::string canbus_handler::interface_name() { return _interface_name; }


void canbus_handler::write_sock(can_frame &frame) {
  boost::system::error_code ec;
  _stream.write_some(boost::asio::buffer(&frame, sizeof(frame)), ec);
  if (ec)
    throw std::runtime_error("[CANBUS_Handler] write_sock error.");
}

canbus_stream &canbus_handler::stream() { return _stream; }
}
}
