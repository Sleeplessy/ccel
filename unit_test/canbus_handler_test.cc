#include <ccel/canbus.hpp>


int main() {
  boost::asio::io_service io;
  ccel::canbus::canbus_handler can("vcan0", io);
  bool dont_stop_until = true;
  while (can.opened()) {
    can_frame _a = can.read_sock();
    std::cout << "can data:" << std::hex;
    for (auto i = 0; i < _a.can_dlc; i++)
      std::cout << static_cast<unsigned>(_a.data[i]) << " ";
    std::cout << " " << std::endl;
    if (static_cast<unsigned>(_a.can_id) == 0x177) {
      can.close();
      std::cout << "END FRAME RECEIVED!" << std::endl;
      return 0;
    }
  }
}
