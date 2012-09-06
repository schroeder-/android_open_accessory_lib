/*
    Android Open Accessory Lib: a library to talk to your android device over USB.
    Copyright (C) 2012  Alexander Schrode (Midix01[at]googlemail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef OPENACCESSORY_HPP
#define OPENACCESSORY_HPP

#include "libusb.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <array>
#include <stdexcept>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define USB_READ_MAX_SIZE 1024

#define USB_VENDOR_GOOGLE 0x18d1
#define USB_PRODUCT_NEXUS_S 0x4e22

using namespace std;
class OpenAccessoryException : public runtime_error{
public:
  OpenAccessoryException(const string& message) : runtime_error(message){};
};


class OpenAccessory{

private:
  libusb_device_handle* dev;
  const static uint8_t in_addr = 0x85;
  const static uint8_t out_addr = 0x07;
#ifndef _MSC_VER
  const static array<uint32_t, 4> _pid {{0x2D00, 0x2D01, 0x2D04, 0x2D05}};
#else
  const static uint32_t _pid[];
  const static uint32_t _pid_size = 4;
#endif
  static bool lib_load;

  string man;
  string model;
  string desc;
  string ver;
  string url;
  string serial;
  uint32_t dev_vid;
  uint32_t dev_pid;

  void load_accessory(void);
  void change_device(void);
  void find_device(void);
  OpenAccessory(const OpenAccessory& rhs){};
public:
  static bool check_for_accessory(uint16_t vid, uint16_t pid);
  static void init(void);
  static void de_init(void);
  OpenAccessory(string manufacture, string model_name, string description,
      string version, string uri, string serial_number, uint32_t vid,
                uint32_t pid);
  void send_data(vector<uint8_t> data);
  void read_data(vector<uint8_t>& data);
  ~OpenAccessory();
};

#endif
