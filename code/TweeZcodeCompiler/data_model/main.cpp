//
// Created by george on 6/10/15.
//

#include "include/TweeFile.h"

int main(int argc, char *argv[]) {

    TweeFile* tweeStructure = new TweeFile();

    //start passage
    Head* h1 = new Head("start");
    Body* b1 = new Body();
    Text* t1_1 = new Text("Starttext 1");
    Link* t1_2 = new Link("psg1","psg1");
    *b1 += *t1_1;
    *b1 += *t1_2;
    Passage p1 = new Passage(*h1, *b1);

    *tweeStructure += *p1;
}
