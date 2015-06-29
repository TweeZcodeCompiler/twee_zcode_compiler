//
// Created by manuel on 21.06.15.
//

#ifndef PROJECT_ARGSCOMMAND_H
#define PROJECT_ARGSCOMMAND_H

#include<iostream>

/*
 *
 * Following calls are supported
 *
 *
 *
 * TweeZCodeCompiler [-i] source.twee [-o zcode.z8] [-a assembly.zap] [-d]
 *
 *
 *  -i is always interpreted as twee file
 *
 *
 * it is not possible two use -a and -i at the same time
 *
 *
 * sample calls
 * TweeZCodeCompiler source.twee
 * TweeZCodeCompiler -a source.zap
 * TweeZCodeCompiler -
 */

class ArgsCommand {
private:
    std::string sourceFile;
    std::string outputFile;
    bool inputIsAssembly;
    bool outputIsAssembly;
    bool debugInConsole;
    bool valid;

public:
    ArgsCommand(int argc, char **argv);

    static void printHelpMessage(std::string invocation = "");

    explicit operator bool() const {
        return valid;
    }

    std::string getSourceFileName() const;

    std::string getOutputFileName() const;

    bool isValid() const;

    bool sourceFileIsTwee() const;

    bool sourceFileIsAssembly() const;

    bool outputToAssembly() const;

    bool isDebugInConsole() const;
};


#endif //PROJECT_ARGSCOMMAND_H
