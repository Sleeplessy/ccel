#include <ccel/udev.hpp>

namespace ccel {
namespace udev {

virtual_keyboard_handler::virtual_keyboard_handler(boost::asio::io_service &io)
  : base_handler(io,sizeof(virtual_keyboard_handler)), _udev{}, _dev_name{"ccel_vkey"}, _open(true) {
  // Init the udev handler
  // Open the uinput file descriptor
  _fd = ::open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  // NOTICE THAT THIS IS NOT ccel::base_handler::open()
  ioctl(_fd, UI_SET_KEYBIT, KEY_SPACE);
  for (auto i = 0; i < 248; i++) {
    ioctl(_fd, UI_SET_KEYBIT, i); // Bind and allow keycodes from 0 to 248
    __key_proprety[i] =
        boost::make_tuple(false, 20); // Default click time in  20 ms
  }
  ioctl(_fd, UI_SET_EVBIT, EV_KEY);
  ioctl(_fd, UI_SET_EVBIT, EV_REL);

  for (auto i = 0; i < _dev_name.length(); i++) {
    _udev.name[i] = _dev_name[i];
  }
  _udev.id.bustype = BUS_USB;
  _udev.id.vendor = 0x1;
  _udev.id.product = 0x1;
  _udev.id.version = 1;
  write(_fd, &_udev, sizeof(_udev));
  if (ioctl(_fd, UI_DEV_CREATE) != 0) {
    std::cout << "CREATE FAILED!\n";
  };
}

virtual_keyboard_handler::~virtual_keyboard_handler() {
  ioctl(_fd, UI_DEV_DESTROY);
  ::close(_fd); // NOTICE THAT THIS IS NOT ccel::base_handler::close
}

int virtual_keyboard_handler::open() {
  if (!this->opened()) { // If is not opened
    ioctl(_fd, UI_SET_KEYBIT, KEY_SPACE);
    for (auto i = 0; i < 248; i++) { // Bind and allow keycodes from 0 to 248
      ioctl(_fd, UI_SET_KEYBIT, i);
    }
    ioctl(_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(_fd, UI_SET_EVBIT, EV_REL);
    _udev = uinput_user_dev{};
    try {
      write(_fd, &_udev, sizeof(_udev));
    } catch (...) {
      return -1;
    }
    ioctl(_fd, UI_DEV_CREATE);
  }
  return 0;
}
int virtual_keyboard_handler::close() {
  if (this->opened()) {
    ioctl(_fd, UI_DEV_DESTROY);
    _open = false;
    return 0;
  } else
    return 0;
}

void virtual_keyboard_handler::press(int key_code) {
  if (key_code >= 0 && key_code <= 248 && !__key_proprety[key_code].get<0>()) {
    input_event __ev{};
    __ev.type = EV_KEY;
    __ev.code = key_code;
    __ev.value = KEY_PRESS;
    if (write(_fd, &__ev, sizeof(input_event)) <
        0) // In fact,if error happened, only -1 would be returned
      throw std::runtime_error(
          "Error while syncing keypress with kernel events.");
    else
      __key_proprety[key_code].get<0>() = true;
  }
}

void virtual_keyboard_handler::release(int key_code) {
  if (key_code >= 0 && key_code <= 248 && __key_proprety[key_code].get<0>()) {
    input_event __ev{};
    __ev.type = EV_KEY;
    __ev.code = key_code;
    __ev.value = KEY_RELEASE;
    if (write(_fd, &__ev, sizeof(input_event)) < 0)
      throw std::runtime_error(
          "Error while syncing keyrelease with kernel events.");
    else
      __key_proprety[key_code].get<0>() = false;
  }
}

void virtual_keyboard_handler::click(int key_code) {
  if (key_code >= 0 && key_code <= 248) {
    input_event __ev{};
    __ev.type = EV_KEY;
    __ev.code = key_code;
    __ev.value = KEY_PRESS;
    write(_fd, &__ev, sizeof(input_event));
    boost::asio::deadline_timer __timer(
        __handler_io,
        boost::posix_time::millisec(__key_proprety[key_code].get<1>()));
    __ev.value = KEY_RELEASE;
    // wait a time here
    __timer.wait();
    write(_fd, &__ev, sizeof(input_event));
    __handler_io.run();
  }
}

void virtual_keyboard_handler::set_click_time(int key_code,
                                              unsigned click_time) {
  __key_proprety[key_code].get<1>() = click_time;
}

const unsigned virtual_keyboard_handler::get_click_time(int key_code) {
  return __key_proprety[key_code].get<1>();
}
}
}
