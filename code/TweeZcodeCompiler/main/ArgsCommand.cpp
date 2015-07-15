//
// Created by manuel on 21.06.15.
//

#include "ArgsCommand.h"
#include <iostream>
#include <plog/Log.h>
#include <unistd.h>
#include <string>
#include <libgen.h>
#include <stdexcept>

using namespace std;

static const string ZMACHINE_EXT_V8 = ".z8";
static const string ZAP_EXT = ".zap";
static const string DEFAULT_PROGRAM_NAME = "twzcc";


string GetProperFileName(string fullPath) {
    string fileName;

    char *base;
    char *fileCStr = new char[fullPath.length() +
                              1]; // need to copy because c_str return const val (on linux but apparently not darwin???)
    copy(fullPath.begin(), fullPath.end(), fileCStr);
    fileCStr[fullPath.length()] = '\0';

    if ((base = basename(fileCStr)) != NULL) {
        char *pointPos = strrchr(base, '.');
        if (pointPos)
            pointPos[0] = '\0';

        fileName = string(base);
    }

    delete[] fileCStr;

    if (base == NULL) {
        throw invalid_argument("unable to resolve basename for " + fullPath);
    }

    return fileName;
}

void ArgsCommand::printHelpMessage(string invocation) {
    string properInvocationName = "";
    try {
        properInvocationName = GetProperFileName(invocation);
    } catch (const invalid_argument &e) { }

    cout << "Usage: " <<
    (properInvocationName.empty() ? DEFAULT_PROGRAM_NAME : properInvocationName) << " " <<
    "[-d] [-o outputfile] [-a] <inputfile>" << endl << endl <<
    "  -d"                  << "\t\t\t\t" << "print debug logs to console" << endl <<
    "  -o <outputfile>"     << "\t"       << "use specified output instead of default (name of input file with .z8 extension)" << endl <<
    "  -a"                  << "\t\t\t\t" << "treat input file as Z-machine assembly instead of Twee source (cannot be used in conjunction with -s)" << endl <<
    "  -s"                  << "\t\t\t\t" << "generate readable Z-machine assembly instead of Z-program (cannot be used in conjunction with -a)" << endl;
}

ArgsCommand::ArgsCommand(int argc, char **argv) {
    int opt;
    debugInConsole = false;
    inputIsAssembly = false;
    valid = true;

    size_t findPosition;

    int longIndex = 0;

    while ((opt = getopt(argc, argv, "o:ads")) != -1) {
        switch (opt) {
            default:
                cerr << "unknown argument: " << (char) opt << endl;
                valid = false;
                return;
            case 'd':
                debugInConsole = true;
                break;
            case 'a': //source file is Z-Assembly
                inputIsAssembly = true;
                break;
            case 's':
                outputIsAssembly = true;
                break;
            case 'o':
                outputFile = optarg;
                break;
        }
    }

    if (inputIsAssembly && outputIsAssembly) {
        cerr << "Both -a and -s flag provided." << endl;
        valid = false;
        return;
    }

    if (optind == argc - 1) {
        sourceFile = argv[optind];

        int status;
        struct stat st_buf;

        status = stat(sourceFile.c_str(), &st_buf);
        if(status != 0) {
            cerr << "error checking file status: " << strerror(errno) << endl;
            valid = false;
        }

        if (S_ISDIR(st_buf.st_mode)) {
            cerr << "error: specified input is a directory" << endl;
            valid = false;
        }

        // define standard output file if necessary
        if (outputFile.empty()) { // assuming empty string means not set yet
            try {
                outputFile = GetProperFileName(sourceFile) + (outputIsAssembly ? ZAP_EXT : ZMACHINE_EXT_V8);
            } catch (const invalid_argument &e) {
                valid = false;
            }
        }
    } else {
        cerr << "invalid number of non-option arguments" << endl;
        valid = false;
    }
}


string ArgsCommand::getSourceFileName() const {
    return sourceFile;
}

string ArgsCommand::getOutputFileName() const {
    return outputFile;
}

bool ArgsCommand::sourceFileIsTwee() const {
    return !inputIsAssembly;
}

bool ArgsCommand::sourceFileIsAssembly() const {
    return inputIsAssembly;
}

bool ArgsCommand::isDebugInConsole() const {
    return debugInConsole;
}

bool ArgsCommand::isValid() const {
    return valid;
}

bool ArgsCommand::outputToAssembly() const {
    return outputIsAssembly;
}
