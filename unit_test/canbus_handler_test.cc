#include <ccel/canbus.hpp>
#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  ccel::io_service io;
  ccel::canbus::canbus_handler can("can0", io);
  unsigned loop_time = 50;
  if (argc > 1)
    loop_time = std::stoi(argv[1]);
  for (std::size_t i = 0; i < loop_time; i++) {
    can_frame _a = can.read_sock();
    bool ext = ccel::canbus::ext_frame_test(_a);
    std::cout << std::hex << std::uppercase << std::setfill('0')
              << std::setw(8);
    if (!ext)
      std::cout << std::setfill(' ');
    std::cout << static_cast<unsigned>(_a.can_id & CAN_EFF_MASK) << " "
              << (ext ? "EXT" : "STD");
    std::cout << "[" << static_cast<unsigned>(_a.can_dlc) << "]"
              << "\t";
    for (auto i = 0; i < _a.can_dlc; i++)
      std::cout << std::setw(2) << static_cast<unsigned>(_a.data[i]) << " ";
    std::cout << std::endl;
    if (static_cast<unsigned>(_a.can_id) == 0x177) {
      can.close();
      std::cout << "END FRAME RECEIVED!" << std::endl;
      return 0;
    }
  }

  std::cout << "vcan0 test finished." << std::endl;
  return 0;
}
