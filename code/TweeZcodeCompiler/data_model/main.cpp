#include "include/Passage/Passage.h"
#include "include/Passage/Body.h"
#include "include/Passage/Head.h"
#include "include/Passage/Body/BodyPart.h"
#include "include/Passage/Body/Text.h"
#include "include/Passage/Body/Link.h"


#include <iostream>

int main() {

    Body b;
    Head h("test");
    b += new Text("testText");
    b += new Link("testTarget");
    Passage p(h, b);

    std::cout << p.to_string();
}