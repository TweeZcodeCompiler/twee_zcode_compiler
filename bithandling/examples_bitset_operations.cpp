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

	std::bitset<8> stringBitsetA (std::string("11001111")); //  build a bitset using the string, the bitset looks like 00001111
	std::bitset<8> stringBitsetB (std::string("11110011")); //  build a bitset using the string, the bitset looks like 11110000

/*
 *  ===========================================================Manipulate_the_string_bitsets==========================================
 */

	std::cout << std::endl << "Manipulate the string constructed bitsets" << std::endl;
	std::cout << "Note: every bitset operation must be encapsuled in () !" << std::endl;
	std::cout << "Strings: A = " << stringBitsetA << " B = " << stringBitsetB << std::endl << std::endl;

//=============================================================AND_OR_XOR=============================================================

	std::cout << "Following logical operators with two operands are supported:" << std::endl << std::endl;
	std::cout << "AND - &, AND with Assign - &= : (A&B) = " << (stringBitsetA & stringBitsetB) << std::endl;
	std::cout << "OR - |,  OR  with Assign - |= : (A|B) = " << (stringBitsetA | stringBitsetB) << std::endl;
	std::cout << "XOR - ^, XOR with Assign - ^= : (A^B) = " << (stringBitsetA ^ stringBitsetB) << std::endl << std::endl;

//=============================================================NEG====================================================================
	std::cout << "Use ~ to negate a complete bitset:"<< std::endl << std::endl;
	std::cout << "Original: " << stringBitsetA  << std::endl;
	std::cout << "Negation: " << (~stringBitsetA) << std::endl << std::endl;

//=============================================================Shifting===============================================================
	
	std::cout << "Following shifting operators are supported:" << std::endl << std::endl;
	std::cout << "Original: " << stringBitsetA  << std::endl;
	std::cout << "SHL - <<, SHL with Assign - <<= : (A>>2) = " << (stringBitsetA>>2) << std::endl;
	std::cout << "SHR - >>, SHR with Assign - >>= : (A<<2) = " << (stringBitsetA<<2) << std::endl << std::endl;


//=============================================================Comparison=============================================================
	std::cout << "Following compare operators are supported:" << std::endl << std::endl;
	std::cout << "EQ  == : (A==B) = " << (stringBitsetA == stringBitsetB) << std::endl;
	std::cout << "NEQ != : (A!=B) = " << (stringBitsetA != stringBitsetB) << std::endl << std::endl;

	return 0;
}