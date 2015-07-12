//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODEOBJECT_H
#define PROJECT_ZCODEOBJECT_H


#include <glob.h>
#include <vector>
#include <bitset>
#include <memory>
#include <iostream>

class ZCodeObject : public std::enable_shared_from_this<ZCodeObject> {
protected:
    std::shared_ptr<ZCodeObject> parrent;
    std::vector<std::shared_ptr<ZCodeObject>> children;

public:
    std::shared_ptr<ZCodeObject> share() {
        return shared_from_this();
    }

    size_t offset = 0;
    size_t size = 0;
    std::string displayName = "UNKNOWN";
    void checkOffset(std::vector<std::bitset<8>> &code);
    virtual void print(std::vector<std::bitset<8>> &code) = 0;

    //returns true if needs to be revalidate again
    virtual bool revalidate() = 0;

    void printMemory();

    void add(std::shared_ptr<ZCodeObject> Child);

    void setSize(size_t size);

    size_t getSize();

    void setOffset(size_t offset);

    size_t getOffset();

    ZCodeObject() {
    }

    ZCodeObject(std::string displayName) {
        this->displayName = displayName;
    }

    void cleanup() {
        for (size_t i = 0; i < children.size(); i++) {
            children.at(i)->cleanup();
        }
        children.clear();
        parrent = NULL;
    }
};


#endif //PROJECT_ZCODEOBJECT_H
