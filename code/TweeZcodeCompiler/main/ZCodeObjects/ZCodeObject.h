//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODEOBJECT_H
#define PROJECT_ZCODEOBJECT_H


#include <glob.h>
#include <vector>
#include <bitset>
#include <memory>
#include <bits/unique_ptr.h>

class ZCodeObject {
protected:
    std::shared_ptr<ZCodeObject> parrent = NULL;
    std::vector<std::shared_ptr<ZCodeObject>> children;

public:
    size_t  offset = 0;
    size_t  size = 0;
    std::string displayName = "UNKNOWN";
    virtual void print(std::vector<std::bitset<8>> &code) {};
    //returns true if needs to be revalidate again
    virtual bool revalidate(){};

    void printMemory();
    void add(std::shared_ptr<ZCodeObject> Child);
    void setSize(size_t size);
    size_t getSize();
    void setOffset(size_t offset);
    size_t getOffset();
    ZCodeObject(){
    }
    ZCodeObject(std::string displayName){
        this->displayName = displayName;
    }
    ~ZCodeObject(){
        parrent = NULL;
        children.clear();
    }
};


#endif //PROJECT_ZCODEOBJECT_H
