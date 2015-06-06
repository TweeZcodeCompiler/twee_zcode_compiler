//
// Created by manuel on 06.06.15.
//

#include "Logger.h"

 void Logger::init(std::string fileName)
 {
     plog::init(plog::debug, fileName.c_str()); // Step2: initialize the logger.
 }