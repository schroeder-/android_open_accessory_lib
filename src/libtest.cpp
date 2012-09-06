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
#include <vector>
#include <stdint.h>

int main(){
  OpenAccessory* oa;
  vector<uint8_t> in;
  cout << "Start Test" << endl;
  try{
  OpenAccessory::init();
  cout << "Lib Loaded" << endl;
  oa = new OpenAccessory("RASPBERRY PI", "RASPBERRY PI",
                         "Test OpenDevice Lib", "1.0",
                         "https://github.com/schroeder-",
                         "1337", USB_VENDOR_GOOGLE,
                         USB_PRODUCT_NEXUS_S);
  cout << "Dev Loaded" << endl;
  }
  catch(OpenAccessoryException& e){
	cerr << e.what() << endl;
	cerr << "Exit Test with Failure 1" << endl;
	OpenAccessory::de_init();
	exit(-1);
  }
  try{
    while(1){
      oa->read_data(in);
      cout << in.data() << endl;
    }
  }
  catch(OpenAccessoryException& e){
    cerr << e.what() << endl;
    cerr << "Exit Test with Failure" << endl;
  }
  delete oa;
  OpenAccessory::de_init();
}
