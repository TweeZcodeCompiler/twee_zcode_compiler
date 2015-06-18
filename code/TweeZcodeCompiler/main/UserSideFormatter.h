#pragma once
#include <iomanip>
#include <string>
#include <plog/Util.h>
#include <plog/Log.h>

namespace plog
{
    class UserSideFormatter
    {
    public:
        static util::nstring header()
        {
            return util::nstring();
        }

        static util::nstring format(const Record& record)
        {
            util::nstringstream ss;
            //filters console output to make in user friendly
            if(record.getSeverity() <= plog::warning) {
                ss << record.getMessage().c_str() << "\n"; // Produce a simple string with a log message.
            }

            return ss.str();
        }
    };
}
