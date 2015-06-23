//
// Created by Danni on 23.06.15.
//

#ifndef PROJECT_HEADER_FILE_H
#define PROJECT_HEADER_FILE_H

#include <exception>
#include <string>

class TweeCompilerException : public std::exception {

};


// Assembly parsing exceptions

class AssemblyException : public TweeCompilerException {
public:
    unsigned lineNumber;
    std::string line;
};

class InvalidRoutineException : public AssemblyException {

};

class InvalidDirectiveException : public AssemblyException {

};

class InvalidVariableException : public AssemblyException {

};

class InvalidLabelException : public AssemblyException {

};


// Other exceptions

class TweeDocumentException : public TweeCompilerException {

};


#endif //PROJECT_HEADER_FILE_H
