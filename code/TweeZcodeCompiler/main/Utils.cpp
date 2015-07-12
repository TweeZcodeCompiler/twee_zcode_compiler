//
// Created by philip on 07.06.15.
//

#include <cstdint>
#include "Utils.h"

using std::bitset;
using std::vector;

std::shared_ptr<ZCodeContainer> Utils::dynamicMemory = NULL;

void Utils::addTwoBytes(uint16_t val, vector<bitset<8>> &bitvector) {
    bitset<16> shortVal(val);
    bitset<8> firstHalf, secondHalf;

    for (size_t i = 0; i < 8; i++) {
        secondHalf.set(i, shortVal[i]);
    }

    for (size_t i = 8; i < 16; i++) {
        firstHalf.set(i - 8, shortVal[i]);
    }

    bitvector.push_back(firstHalf);
    bitvector.push_back(secondHalf);
}

void Utils::fillWithBytes(std::vector<std::bitset<8>> &bitVector, uint8_t value, unsigned long amountOfBytes) {
    for (size_t i = 0; i < amountOfBytes; i++) {
        bitVector.insert(bitVector.end(), value);
    }
}

size_t Utils::calculateNextPackageAddress(size_t currentOffset) {
    return ((currentOffset + 8) / 8) * 8;
}

size_t Utils::paddingToNextPackageAddress(size_t vector_size, size_t offset) {
    size_t pkgAdrr = Utils::calculateNextPackageAddress(offset + vector_size + 3);
    size_t empty = (pkgAdrr - vector_size - offset) % 8;
    return (empty > 0) ? empty : 0;
}

void Utils::append(std::vector<std::bitset<8>> &head, std::vector<std::bitset<8>> &tail) {
    head.insert(head.end(), tail.begin(), tail.end());
}

void Utils::insertPaddingToNextRoutine(vector<bitset<8>> &bitsets, size_t routineOffset) {
    size_t padding = Utils::paddingToNextPackageAddress(bitsets.size(), routineOffset);

    for (size_t i = 0; i < padding; i++) {
        bitsets.push_back(bitset<8>(0));
    }
}

std::string Utils::getMallocLib() {
    return ";one data set contains:\n"
            ";\t1 byte:\t\ttype of stored data\n"
            ";\t2 byte:\t\tsize of stored data\n"
            ";\tn bit:\t\tdata\n"
            "\n"
            ";the types are:\n"
            ";\t0:\tfree\n"
            ";\t1:\treservated\n"
            ";\t2:\tnumber\n"
            ";\t3:\tbool\n"
            ";\t4:\tstring\n"
            ";\t5:\tconcatinated types\n"
            ";\t255:\tEOM(End Of Memory)\n"
            "\n"
            ";--------------------------------------------------------------------\n"
            ";function sys_malloc(mem, size):\n"
            ";\tmem:\tbyte-adress of an array (or some other memory space)\n"
            ";\tsize:\tsize of the memory to allociate\n"
            ";\treturn:\tbyte adress of the begin of the free memory\n"
            "\n"
            ".FUNCT sys_malloc mem, size, currentPointer, currentSize, lastPointer, help\n"
            "\t; initial set currentPointer = lastPointer = mem\n"
            "\t; all pointers points to the begin of the memory space\n"
            "\t; size will be calculated with the 3 byte of storeage information\n"
            "\tstore currentPointer mem\n"
            "\tstore currentSize 0\n"
            "\tstore lastPointer currentPointer\n"
            "\tadd size 3 -> size \n"
            "\t\n"
            "\t;increment the currentPointer until currentSize == size\n"
            "\t;or found allready allociated memory\n"
            "SEARCH:\n"
            "\tloadb currentPointer 0 -> help\n"
            "\tje help 0 ?FREE_MEM\n"
            "\tje help 255 ?EOM\n"
            "\t;jump over the allociated memory\n"
            "\tadd currentPointer 1 -> currentPointer\n"
            "\tloadw currentPointer 0 -> sp\n"
            "\tadd currentPointer sp -> currentPointer\n"
            "\tsub currentPointer 1 -> currentPointer\n"
            "\tstore lastPointer currentPointer\n"
            "\tstore currentSize 0\n"
            "\tjump ?SEARCH\n"
            "FREE_MEM:\n"
            "\tadd currentSize 1 -> currentSize\n"
            "\tadd currentPointer 1 -> currentPointer\n"
            "\tje currentSize size ?FOUND\n"
            "\tjump ?SEARCH\n"
            "\t\n"
            "\t;if the algorithm found a free memory, it will be allociated\n"
            "FOUND:\n"
            "\tstoreb lastPointer 0 1\n"
            "\tadd lastPointer 1 -> lastPointer\n"
            "\tstorew lastPointer 0 size\n"
            "\tadd lastPointer 2 -> lastPointer\n"
            "\tret lastPointer\n"
            "EOM:\t\n"
            "\tnew_line\n"
            "\tprint \"ERROR: running out of memory :(\"\n"
            "\tnew_line\n"
            "\tquit\t\n"
            "\t\n"
            ";--------------------------------------------------------------------\n"
            ";function sys_free(pointer)\n"
            ";\tpointer:\tbyte adress\n"
            "\n"
            ".FUNCT sys_free pointer, size, i\n"
            "\n"
            "\t;calculate the size of the pice of memory\n"
            "\tsub pointer 2 -> pointer\n"
            "\tloadw pointer 0 -> size\n"
            "\tsub pointer 1 -> pointer\n"
            "\tstore i 0\n"
            "\t\n"
            "\t;fill with 0\n"
            "FREE:\n"
            "\tje i size ?FINISH\n"
            "\tstoreb pointer 0 0\n"
            "\tadd pointer 1 -> pointer\n"
            "\tadd i 1 -> i\n"
            "\tjump ?FREE\n"
            "FINISH:\n"
            "\trtrue\n"
            "\n"
            ";--------------------------------------------------------------------\n"
            ";function sys_number(mem, initialValue):\n"
            ";\tmem:\t\tbyte-adress of an array (or some other memory space)\n"
            ";\tinitialValue:\tinitial value of number\n"
            ";\treturn:\t\tbyte-adress of the number\n"
            "\n"
            ".FUNCT sys_number mem, initialValue, pointer\n"
            "\tcall sys_malloc mem 2 -> pointer\n"
            "\tsub pointer 3 -> pointer\n"
            "\tstoreb pointer 0 2\n"
            "\tadd pointer 3 -> pointer\n"
            "\tstorew pointer 0 initialValue\n"
            "\tret pointer\n"
            "\t\n"
            ";--------------------------------------------------------------------\n"
            ";function sys_bool(mem, initialValue):\n"
            ";\tmem:\t\tbyte-adress of an array (or some other memory space)\n"
            ";\tinitialValue:\tinitial value of bool\n"
            ";\treturn:\t\tbyte-adress of the bool\n"
            "\n"
            ".FUNCT sys_bool mem, initialValue, pointer\n"
            "\tcall sys_malloc mem 1 -> pointer\n"
            "\tsub pointer 3 -> pointer\n"
            "\tstoreb pointer 0 3\n"
            "\tadd pointer 3 -> pointer\n"
            "\tstoreb pointer 0 initialValue\n"
            "\tret pointer\n"
            "\n"
            ";--------------------------------------------------------------------\n"
            ";function sys_string(mem, initialValue):\n"
            ";\tmem:\t\tbyte-adress of an array (or some other memory space)\n"
            ";\tinitialValue:\tinitial value of string (byte-adress)\n"
            ";\treturn:\t\tbyte-adress of the string\n"
            "\n"
            ".FUNCT sys_string mem, initialValue, pointer\n"
            "\tcall sys_malloc mem 2 -> pointer\n"
            "\tsub pointer 3 -> pointer\n"
            "\tstoreb pointer 0 4\n"
            "\tadd pointer 3 -> pointer\n"
            "\tstorew pointer 0 initialValue\n"
            "\tret pointer\n"
            "\n"
            ";--------------------------------------------------------------------\n"
            ";function sys_concat(mem, head, tail):\n"
            ";\tmem:\t\tbyte-adress of an array (or some other memory space)\n"
            ";\theat:\t\tbyte-adress of the first part\n"
            ";\ttail:\t\tbyte-adress of the second part\n"
            ";\treturn:\t\tbyte-adress of the concatination\n"
            "\n"
            ".FUNCT sys_concat mem, head, tail, pointer\n"
            "\tcall sys_malloc mem 4 -> pointer\n"
            "\tsub pointer 3 -> pointer\n"
            "\tstoreb pointer 0 5\n"
            "\tadd pointer 3 -> pointer\n"
            "\tstorew pointer 0 head\n"
            "\tstorew pointer 1 tail\n"
            "\tret pointer\n"
            "\n"
            ";--------------------------------------------------------------------\n"
            ";function sys_instanceof(pointer)\n"
            ";\tpointer:\tbyte-adress of the stored type\n"
            ";\treturn:\t\ttype code (see top of page)\n"
            "\n"
            ".FUNCT sys_instanceof pointer\n"
            "\tsub pointer 3 -> pointer\n"
            "\tloadb pointer 0 -> sp\n"
            "\tret sp\n"
            "\n"
            ";--------------------------------------------------------------------\n"
            ";function sys_isa(pointer,type)\n"
            ";\tpointer:\tbyte-adress of the stored type\n"
            ";\ttype:\ttype to compare with\n"
            ";\treturn:\t\t1:true or 0:false\n"
            "\n"
            ".FUNCT sys_isa pointer, type\n"
            "\tcall sys_instanceof pointer -> sp\n"
            "\tje sp type ?YES\n"
            "\trfalse\n"
            "YES:\n"
            "rtrue\n"
            "\n"
            ";--------------------------------------------------------------------\n"
            ";function sys_print(pointer)\n"
            ";\tpointer:\tbyte-adress of the stored type\n"
            "\n"
            ".FUNCT sys_print pointer, type\n"
            "\n"
            "\tcall sys_instanceof pointer -> type\n"
            "\tje type 2 ?NUMBER\n"
            "\tje type 3 ?BOOL\n"
            "\tje type 4 ?STRING\n"
            "\tje type 5 ?CONCAT\n"
            "\trtrue\n"
            "\t\n"
            "NUMBER:\n"
            "\tloadw pointer 0 -> sp\n"
            "\tprint_num sp\n"
            "\trtrue\n"
            "\t\n"
            "BOOL:\n"
            "\tloadb pointer 0 -> sp\n"
            "\tje sp 0 ?FALSE\n"
            "\tprint \"true\"\n"
            "\trtrue\n"
            "\tFALSE:\n"
            "\tprint \"false\"\n"
            "\trtrue\n"
            "\t\n"
            "STRING:\n"
            "\tloadw pointer 0 -> sp\n"
            "\tprint_addr sp\n"
            "\trtrue\n"
            "\n"
            "CONCAT:\n"
            "\tloadw pointer 1 -> sp\n"
            "\tloadw pointer 0 -> sp\n"
            "\tcall_vn sys_print sp\n"
            "\tcall_vn sys_print sp\n"
            "\trtrue ";
}