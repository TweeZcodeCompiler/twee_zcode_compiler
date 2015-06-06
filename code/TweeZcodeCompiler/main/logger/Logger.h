//
// Created by manuel on 06.06.15.
//

#ifndef PROJECT_LOGGER_H
#define PROJECT_LOGGER_H


#include<iostream>
#include<plog/Log.h>
class Logger {

public:
    static void init(std::string fileName);

};

/*
 * how du use
 *
    LOG_VERBOSE << "verbose";
    LOG_DEBUG << "debug";
    LOG_INFO << "info";
    LOG_WARNING << "warning";
    LOG_ERROR << "error";
    LOG_FATAL << "fatal";

    short forms

    LOGV << "verbose";
    LOGD << "debug";
    LOGI << "info";
    LOGW << "warning";
    LOGE << "error";
    LOGF << "fatal";
 */



#endif //PROJECT_LOGGER_H
