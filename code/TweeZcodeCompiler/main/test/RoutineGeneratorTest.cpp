//
// Created by philip on 17.05.15.
//

#include "RoutineGeneratorTest.h"
#include "../RoutineGenerator.h"
#include <vector>
#include <bitset>
#include <iostream>
using std::bitset;

void RoutineGeneratorTest::runTest() {
    RoutineGenerator rg = RoutineGenerator();
    std::vector<std::bitset<8>> res = rg.printPrintRoutine("hallo");

    //createVerifier
    std::vector<std::bitset<8>> ver = std::vector<std::bitset<8>>();
    //0 arguments
    ver.push_back(bitset<8>(0));
    //Opcode for print
    ver.push_back(bitset<8>(178));
    //"hallo" in Z-Characters
    ver.push_back(bitset<8>(52));
    ver.push_back(bitset<8>(209));
    ver.push_back(bitset<8>(198));
    ver.push_back(bitset<8>(128));
    //Opcode for quit
    ver.push_back(bitset<8>(186));
    assertsEquals(ver,res);
}