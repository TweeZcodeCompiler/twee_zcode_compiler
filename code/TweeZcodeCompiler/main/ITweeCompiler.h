//
// Created by Danni on 19.06.15.
//

#ifndef PROJECT_ITWEECOMPILER_H
#define PROJECT_ITWEECOMPILER_H

#include <ostream>
#include <TweeFile.h>

class ITweeCompiler {
public:
    virtual void compile(TweeFile& tweeDoc, std::ostream& out) = 0;
};


#endif //PROJECT_ITWEECOMPILER_H
