//
// Created by manuel on 21.06.15.
//

#include "ArgsCommand.h"
#include <iostream>
#include <plog/Log.h>
#include <unistd.h>
#include <string>


ArgsCommand::ArgsCommand(int argc, char  **argv){
    this->argc = argc;
    this->argv = argv;

    char opt;
    this->debugInConsole = false;
    size_t findPosition;

    while ((opt = getopt (argc, argv, "i:o:a:d")) != -1)
    {

        switch (opt)
        {
            default:
          break;
            case 'i': //twee input file
                this->sourceFile = optarg;
            //std::cout   << "Input Source File:" << sourceFile;
                this->isAssembly = false;

                //define standard output file
                this->outputFile = sourceFile;
                findPosition = outputFile.find(".tws");
                if(findPosition == -1)
                {
                    std::cout << "Not a .tws Twee File"; //LOG_ERROR is not working here

                }
                outputFile.replace(findPosition, std::string(".tws").length(), ".z8");


                break;

            case 'o':
                this->outputFile = optarg;
                findPosition = this->outputFile.find(".z8");
                if(findPosition == -1)
                {
                    this->outputFile = this->outputFile + ".z8";
                }
                //std::cout  << "Output File:" << outputFile;

                break;

            case 'd':
                //std::cout << "Debug in Console is activated";
                this->debugInConsole = true;
                break;

            case '?':
                break;

            case 'a': //source file is Z-Assembly
                this->sourceFile = optarg;
                this->isAssembly = true;


                this->outputFile = this->sourceFile;

            //std::cout << "Input file is Z-Assembly";
                findPosition = outputFile.find(".zap");
                if(findPosition == -1)
                {
                    std::cout << "Not a .tws Twee File"; //LOG_ERROR is not working here

                }
                outputFile.replace(findPosition, std::string(".zap").length(), ".z8");

                break;

        }



    }



}


std::string ArgsCommand::getSourceFileName()
{
    return this->sourceFile;

}

std::string ArgsCommand::getOutputFileName()
{
    return this->outputFile;
}
bool ArgsCommand::sourceFileIsTwee()
{
    if(this->isAssembly == true)
        return false;
    else
        return true;

}
bool ArgsCommand::sourceFileIsAssembly()
{

    if(this->isAssembly == true)
        return true;
    else
        return false;
}

bool ArgsCommand::isDebugInConsole()
{
 return this->debugInConsole;
}
