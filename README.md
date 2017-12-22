# CCEL - Common Communication Event Layer [![Build Status](https://travis-ci.org/Sleeplessy/ccel.svg?branch=master)](https://travis-ci.org/Sleeplessy/ccel)

CCEL provides a variant set of effective network-apis and udev binding implemented under Boost.It uses a managed handler pool to achieve memory management(using Boost.object_pool),and suppords function hooking through handlers. 

## Protocols that supported

+ TCP
+ UDP
+ HTTP
+ CANBus (SocketCAN,CAN over TCP/IP Protocols,CAN over Serial)
+ ModBus (Through TCP/UDP,Serial)

## Usage

Include "ccel/ccel.hpp" for all protocols or includes sperated headers for exact functions.
You can also use ccel/udev.hpp to handle user-space devices (like uinput)

## Progress

Works need to be done,and some needs to refatrory

- [x] udev keyboard handler
  - [x] virtual keyboard
    - [x] pretend press&release
    - [x] click emulated
  - [ ] virtual mouse
  - [ ] virtual joypad
  - [ ] callback hooking
- [ ] TCP handler
- [ ] UDP handler
- [ ] HTTP handler
- [ ] Serial handler
- [x] CANBus Protocol Family Implementation 
  - [x] SocketCAN handler
    - [x] callback hooking
    - [x] blocked&asynchronous data reading
    - [ ] blocked&asynchronous data transfer
- [ ] ModBus Protocol Family Implementation
  - [ ] over TCP
  - [ ] over UDP
  - [ ] over HTTP
  - [ ] over Serial

## LICENSE

Copyright [2017] [Sleeplessy]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
or the project contained LICENSE file

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
