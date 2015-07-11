//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODECONTAINER_H
#define PROJECT_ZCODECONTAINER_H


#include "ZCodeObject.h"
#include "ZCodeLabel.h"
#include <map>

class ZCodeContainer : public ZCodeObject {
private:
    std::map<std::string, std::shared_ptr<ZCodeLabel>> labels;
public:
    static unsigned int mouseXClicked;
    static unsigned int mouseYClicked;

    size_t containerOffset = 0;

    ZCodeContainer() { }

    ZCodeContainer(std::string name) : ZCodeObject(name) {

    }

    std::shared_ptr<ZCodeLabel> getOrCreateLabel(std::string name);

    void print(std::vector<std::bitset<8>> &code);

    bool revalidate();

    virtual ~ZCodeContainer() {
        labels.clear();
    }
};


#endif //PROJECT_ZCODECONTAINER_H
