// -----------------------------------------------------------------------------
// main.cpp
//
// DISCLAIMER:
// Feabhas is furnishing this item "as is". Feabhas does not provide any
// warranty of the item whatsoever, whether express, implied, or statutory,
// including, but not limited to, any warranty of merchantability or fitness
// for a particular purpose or any warranty that the contents of the item will
// be error-free.
// In no respect shall Feabhas incur any liability for any damages, including,
// but limited to, direct, indirect, special, or consequential damages arising
// out of, resulting from, or any way connected to the use of the item, whether
// or not based upon warranty, contract, tort, or otherwise; whether or not
// injury was sustained by persons or property or otherwise; and whether or not
// loss was sustained from, or arose out of, the results of, the item, or any
// services that may be provided by Feabhas.
// -----------------------------------------------------------------------------

#include <cstdint> // C99
#include <iostream>
#include <string>  // std::to_string

enum class House : std::uint8_t {
	INVALID, A, B, C, D, E, F, G, H
};

const char* HouseName = "-ABCDEFGH";       // classic C technique


using Unit = std::uint8_t; // C++11

struct Device {
	House house;
	Unit  unit;
};

struct Lamp
{
	Device device;
	bool   state;
};

int main()
{
	// Using initialisation list
	//
	Lamp desk_lamp{ { House::A, 1 }, false };

	// Using member-by-member initialisation
	//
	Lamp bed_main{};
	bed_main.device.house = House::B;
	bed_main.device.unit = 1;
	bed_main.state       = true;

	std::cout << "Desk lamp(" 
		<< HouseName[static_cast<int>(desk_lamp.device.house)] 
		<< std::to_string(desk_lamp.device.unit) << ") is "
		<< (desk_lamp.state ? "on" : "off") << std::endl;

	std::cout << "Bedroom main lamp(" 
		<< HouseName[static_cast<int>(desk_lamp.device.house)]   
		<< std::to_string(bed_main.device.unit) << ") is "
		<< (bed_main.state ? "on" : "off") << std::endl;
}
