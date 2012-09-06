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
#include "openaccessory.hpp"
#include <iostream>
bool OpenAccessory::lib_load = false;

namespace Until{
  void sleep_ms(int ms){
#ifdef WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
    }
}


void OpenAccessory::init(void){
  int ret;
  ret = libusb_init(NULL);
  if(ret != 0){
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  lib_load = true;
}

OpenAccessory::OpenAccessory(string manufacture, string model_name,
                             string description, string version, string uri,
                             string serial_number, uint32_t vid, uint32_t pid)
  : man(manufacture), model(model_name), desc(description), ver(version),
    url(uri), serial(serial_number), dev_vid(vid), dev_pid(pid)
{
  if(!lib_load){
    throw(OpenAccessoryException("Lib not initzialised"));
  }
  try{
    load_accessory();
  }
  catch(OpenAccessoryException& e){
    find_device();
    change_device();
	Until::sleep_ms(2000);
    load_accessory();
  }
}


bool OpenAccessory::check_for_accessory(uint16_t vid, uint16_t pid){
  if(!lib_load){
    throw(OpenAccessoryException("Lib not initzialised"));
  }
  libusb_device_handle* handle;
  if((handle = libusb_open_device_with_vid_pid(NULL, vid, pid)) != NULL){
    return false;
  }
  libusb_close(handle);
  return true;
}

void OpenAccessory::load_accessory(void){
  bool set = false;
#ifndef _MSC_VER
  for(auto& p: _pid){
	  
#else
  for(int i = 0; i < _pid_size; i++){
	  uint32_t p = _pid[i];
#endif
	
    if(set) continue;
    if((dev = libusb_open_device_with_vid_pid(NULL, 0x18D1, p)) != NULL){
      set = true;
    }
  }
  
  if(!set){
    throw(OpenAccessoryException("No Device Found"));
  }
  int ret;
  ret = libusb_claim_interface(dev, 0);
  if(ret != 0){
	   throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  Until::sleep_ms(10);
}

void OpenAccessory::change_device(void){
  uint8_t buffer[2];
  int ret = libusb_control_transfer(dev, 0xC0, 51, 0, 0, buffer, 2, 0);
  if(ret < 0){
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  Until::sleep_ms(20);
  if((ret = libusb_control_transfer(dev, 0x40, 52, 0, 0,
                                    (uint8_t*)man.c_str(),
                                    man.size(), 0)) < 0){
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }


  if((ret = libusb_control_transfer(dev, 0x40, 52, 0, 1,
                                    (uint8_t*) model.c_str(),
                                    model.size() + 1, 0)) < 0){
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  if((ret = libusb_control_transfer(dev, 0x40, 52, 0, 2,
                                    (uint8_t*)desc.c_str(),
                                    desc.size() + 1,0)) < 0){
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  if((ret = libusb_control_transfer(dev, 0x40, 52, 0, 3,
                                   (uint8_t*) ver.c_str(),
                                    ver.size() + 1,0)) < 0){
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  if((ret = libusb_control_transfer(dev, 0x40, 52, 0, 4,
                                    (uint8_t*)url.c_str(),
                                    url.size() + 1, 0)) < 0){
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  if((ret = libusb_control_transfer(dev, 0x40, 52, 0, 5,
                                    (uint8_t*)serial.c_str(),
                                    serial.size() + 1, 0)) < 0){
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  if((ret = libusb_control_transfer(dev, 0x40, 53, 0, 0, NULL, 0, 0)) < 0){
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
#ifdef __linux__
  if(dev != NULL){
    libusb_release_interface(dev, 0);
  }
#endif
  Until::sleep_ms(20);
}

void OpenAccessory::find_device(void){
  if((dev = libusb_open_device_with_vid_pid(NULL, dev_vid, dev_pid))
     == NULL){
    throw(OpenAccessoryException("No Device Found"));
  }
#ifdef __linux__
  int ret;
  libusb_detach_kernel_driver(dev, 0);
  ret  = libusb_claim_interface(dev, 0);
  if(ret != 0){
    libusb_close(dev);
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
#endif
}

void OpenAccessory::send_data(vector<uint8_t> data){
  int ret = 0;
  int sent = 0;
  ret = libusb_bulk_transfer(dev, out_addr, data.data(), data.size(), &sent, 0);
  if(ret < 0 || (unsigned int)ret != data.size()){
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
}

void OpenAccessory::read_data(vector<uint8_t>& data){
  int ret = 0;
  int size = 0;
  uint8_t buff[USB_READ_MAX_SIZE];
  data.clear();
  ret = libusb_bulk_transfer(dev, in_addr, buff, USB_READ_MAX_SIZE, &size, 0);
  cout << size << endl;
  if(ret < 0){
    throw(OpenAccessoryException(libusb_error_name(ret)));
  }
  data.assign(buff, buff + size);
}

OpenAccessory::~OpenAccessory(){
  libusb_release_interface(dev, 0);
  libusb_close(dev);
}

void OpenAccessory::de_init(void){
  if(lib_load){
    libusb_exit(NULL);
    lib_load = false;
  }
}
const uint32_t OpenAccessory::_pid[] = {0x2D00, 0x2D01, 0x2D04, 0x2D05};