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

	std::bitset<8> hexBitset (0xef);                       //  build a bitset using the hexvalues, the bitset looks like 11101111
	std::cout << std::endl << "Hex constructed: " << hexBitset << std::endl;

/*
 *  =============================================================Access_the_hex_bitset================================================
 */

	std::cout << std::endl << "Access the bits of the hex constructed bitset" << std::endl << std::endl;

//=============================================================Return_the_size========================================================

	std::cout << "The size of a bitset is aquired by size()" << std::endl;
	std::cout << "Size of the hex bitset: " << hexBitset.size() << std::endl << std::endl;

//=============================================================Return_the_set_bits====================================================

	std::cout << "The count of set bits in a bitset is aquired by count()" << std::endl;
	std::cout << "Count of the set bits in the hex bitset: " << hexBitset.count() << std::endl << std::endl;

//=============================================================Access_the_third_bit===================================================

	std::cout << "A bitset can be accessed like an array: bitset[2] will return the third bit" << std::endl;
	std::cout << "Third bit of the hex bitset: " << hexBitset[2] << std::endl << std::endl;

//=============================================================Test_the_third_bit=====================================================

	std::cout << "Use test(pos) to test if a bit is set" << std::endl;
	std::cout << "Is the third bit of the hex bitset set? " << hexBitset.test(2) << std::endl << std::endl;

//=============================================================Any====================================================================

	std::cout << "any() will check if any bit of the bitset is set" << std::endl;
	std::cout << "Is any bit of the hex bitset set? " << hexBitset.any() << std::endl << std::endl;

//=============================================================All====================================================================

	std::cout << "all() will check if all bits of the bitset are set" << std::endl;
	std::cout << "Are all bits of the hex bitset set? " << hexBitset.all() << std::endl << std::endl;

//=============================================================None===================================================================

	std::cout << "none() will check if no bits of the bitset are set" << std::endl;
	std::cout << "Are no bits of the hex bitset set? " << hexBitset.none() << std::endl << std::endl;

	return 0;
}