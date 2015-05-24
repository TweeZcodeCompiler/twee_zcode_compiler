//
// Created by tobias on 10/10/15.
//

#include <iostream>
#include "ZCodeHeaderTest.h"
#include "../ZCodeHeader.h"

void ZCodeHeaderTest::testHeader() {
    ZCodeHeader header;
    header.setFileLength(800, 99);
    header.setRoutinesOffset(100);
    header.setStaticStringsOffset(200);

    std::vector<std::bitset<8>> bits = header.getHeaderBits();

    for (size_t i = 0; i < bits.size(); i++) {
        std::cout << bits.at(i) << std::endl;
    }
}