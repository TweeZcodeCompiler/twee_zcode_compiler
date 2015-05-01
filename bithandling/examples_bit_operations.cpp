/*
 *  This file shows how to use the C++ bitset
 *  Version: 1.0
 *  Author Lars Hochstetter - lars.hochstetter@fu-berlin.de
 */

#include <iostream> //  std::cout, std::endl;
#include <string>   //  std::string
#include <bitset>   //  std::bitset



int main(int argc, char const *argv[])
{
//=============================================================Construct_bit_sets=====================================================

	std::bitset<8> defaultBitset;                          //  initialize a bitset with 8 bits, every bit is set to 0
	std::bitset<8> hexBitset (0xEf);                       //  build a bitset using the hexvalues, the bitset looks like 11101111
	std::bitset<8> stringBitset (std::string("00001111")); //  build a bitset using the string, the bitset looks like 00001111

//===========================================================Print_the_bitsets========================================================

	std::cout << std::endl << "Print all bitsets" << std::endl << std::endl;
	std::cout << "Default constructed: " << defaultBitset << std::endl;
	std::cout << "Hex constructed:     " << hexBitset << std::endl;
	std::cout << "String constructed:  " << stringBitset << std::endl;
	std::cout << std::endl;

/*
 *  ===========================================================Manipulate_the_default_bitsets_bits====================================
 */
	std::cout << "Manipulate the bits of the default constructed bitset" << std::endl << std::endl;

//=============================================================Set_all_bits===========================================================
	defaultBitset.set();
	std::cout << "All bits were set to 1 using the set() function:               " << defaultBitset << std::endl;
	std::cout << "set() sets all bits to 1" << std::endl << std::endl;

//=============================================================Set_the_first_bit_to_0=================================================	
/*
 *  Note: the first bit is the rightmost bit!
 */
	defaultBitset.set(0,0);
	std::cout << "The first bit was set to 0 using the set(pos, value) function: " << defaultBitset << std::endl;
	std::cout << "Note that the first bit is the rightmost bit!" << std::endl << std::endl;

//=============================================================Set_the_last_bit_to_0==================================================
/*
 *  Note: the last bit is the leftmost bit!
 */
	defaultBitset.set(defaultBitset.size()-1,0);
	std::cout << "The last bit was set to 0 using the set(pos, value) function:  " << defaultBitset << std::endl;
	std::cout << "Note that the last bit is the left bit!" << std::endl << std::endl;

//=============================================================Set_the_last_bit_to_1==================================================
/*
 *  Note: the last bit is the leftmost bit!
 */
	defaultBitset.set(defaultBitset.size()-1);
	std::cout << "The last bit was set to 1 using the set(pos) function:         " << defaultBitset << std::endl;
	std::cout << "set(pos) sets the bit at pos to 1" << std::endl << std::endl;

//=============================================================Flip_the_second_last_bit===============================================
	defaultBitset.flip(defaultBitset.size()-2);
	std::cout << "The second last bit was flipped using the flip(pos) function:  " << defaultBitset << std::endl << std::endl;

//=============================================================Flip_all_bits==========================================================
	defaultBitset.flip();
	std::cout << "All bits were flipped using the flip() function:               " << defaultBitset << std::endl << std::endl;


//=============================================================Reset_the_second_last_bit==============================================
	defaultBitset.reset(defaultBitset.size()-2);
	std::cout << "The second last bit was reset using the reset(pos) function:   " << defaultBitset << std::endl;
	std::cout << "reset(pos) sets the bit at pos to 0" << std::endl << std::endl;

//=============================================================Reset_all_bits=========================================================
	defaultBitset.reset();
	std::cout << "All bits were reset using the reset() function:                " << defaultBitset << std::endl;
	std::cout << "reset() sets all bits to 0" << std::endl << std::endl;

	return 0;
}