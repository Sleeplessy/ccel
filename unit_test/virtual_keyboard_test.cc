#include <ccel/udev.hpp>
#include <unistd.h>
int main(){
  ccel::io_service _io;
  ccel::udev::virtual_keyboard_handler vkey(_io);
  vkey.set_click_time(233,20);
  for(auto i=0;i<100;i++){
  vkey.click(233);
  }
  return 0;

}
