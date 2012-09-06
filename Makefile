test:  src/libtest.cpp src/openaccessory.cpp src/openaccessory.hpp
	g++ -o test_usb src/libtest.cpp src/openaccessory.cpp -std=c++11 -Wall  \
	-llibusb-1.0

test-win:  src/libtest.cpp src/openaccessory.cpp src/openaccessory.hpp
	g++ -o test_usb src/libtest.cpp src/openaccessory.cpp -std=c++11 -Wall  \
	-I include/libusbx-1.0/ libusb-1.0.dll 
