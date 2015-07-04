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
    enum ErrorType {
        INVALID_ROUTINE,
        INVALID_DIRECTIVE,
        INVALID_GLOBAL,
        INVALID_LOCAL,
        INVALID_VARIABLE,
        INVALID_INSTRUCTION,
        INVALID_LABEL
    };


    AssemblyException(ErrorType error) : errorType(error), lineNumber(0) {}

    ErrorType errorType;
    unsigned lineNumber;
    std::string line;
};

class InvalidLabelException : public AssemblyException {
public:
    InvalidLabelException(std::string label) : AssemblyException(INVALID_LABEL), labelName(label) {}


    const std::string& getLabelName() const {
        return labelName;
    }
private:
    std::string labelName;
};


// Other exceptions

class TweeDocumentException : public TweeCompilerException {
public:
    TweeDocumentException(const std::string what) : _what(what) {}
    const char* what() const noexcept {return _what.c_str();}
private:
    std::string _what;
};


#endif //PROJECT_HEADER_FILE_H
