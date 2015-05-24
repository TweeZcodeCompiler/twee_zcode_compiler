//
// Created by philip on 17.05.15.
//

#include "RoutineGeneratorTest.h"
#include "../RoutineGenerator.h"
#include <vector>
#include <bitset>
#include <iostream>
using std::bitset;

void RoutineGeneratorTest::testRoutineGenerator() {
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

    for(int i = 0; i < 7; i++){
        if(ver[i] != res[i]){
            std::cout << "ERROR in RoutineGenerator at byte "<<i<<" : expected " << ver[i] <<". Found: " << res[i] <<".\n";
            return;
        }
    }
    std::cout << "Test for RoutineGenerator ran SUCCESSFULLY\n";

}