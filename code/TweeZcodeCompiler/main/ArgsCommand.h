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
    bool isAssembly;
    bool debugInConsole;

    int argc;
    char **argv;

public:
    ArgsCommand(int argc, char **argv);
    std::string getSourceFileName() const;
    std::string getOutputFileName() const;
    bool sourceFileIsTwee() const;
    bool sourceFileIsAssembly() const;
    bool isDebugInConsole() const;
};


#endif //PROJECT_ARGSCOMMAND_H
