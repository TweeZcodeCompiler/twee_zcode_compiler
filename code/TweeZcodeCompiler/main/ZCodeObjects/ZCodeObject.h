//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODEOBJECT_H
#define PROJECT_ZCODEOBJECT_H


#include <glob.h>
#include <vector>
#include <bitset>
#include <bits/unique_ptr.h>

class ZCodeObject {
protected:
    std::unique_ptr<ZCodeObject> parrent;
    std::vector<ZCodeObject> children = std::vector<ZCodeObject>();
    size_t  offset = 0;
    size_t  size = 0;

public:
    virtual std::vector<std::bitset<8>> &print();
    //returns true if needs to be revalidate again
    virtual bool revalidate();

    void add(ZCodeObject &Child);
    void setSize(size_t size);
    size_t getSize();
    void setOffset(size_t offset);
    size_t getOffset();
};


#endif //PROJECT_ZCODEOBJECT_H
