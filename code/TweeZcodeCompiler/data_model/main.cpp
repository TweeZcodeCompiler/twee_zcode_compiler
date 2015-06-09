#include "include/Passage/Passage.h"
#include "include/Passage/Body.h"
#include "include/Passage/Head.h"
#include "include/Passage/Body/BodyPart.h"
#include "include/Passage/Body/Text.h"


#include <iostream>

int main() {

    Text t = Text("asdasd");
    //BodyPart &bp = t;
    std::cout << t.to_string();
}