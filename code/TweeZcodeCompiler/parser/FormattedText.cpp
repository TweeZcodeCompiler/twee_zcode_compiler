//
// Created by tobias on 6/30/15.
//

#include <plog/Log.h>
#include "include/Passage/Body/FormattedText.h"

FormattedText::FormattedText(std::string type) {
    if (type.compare("//") == 0) {
        this->format = ITALIC;
    } else if (type.compare("\"") == 0) {
        this->format = BOLD;
    } else if (type.compare("__") == 0) {
        this->format = UNDERLINED;
    } else if (type.compare("{{{") == 0 || type.compare("}}}") == 0) {
        this->format = MONOSPACE;
    } else {
        LOG_DEBUG << "Unsupported text formatting";
        throw;
    }
}

Format FormattedText::getFormat() {
    return format;
}

std::string FormattedText::to_string() const {
    return std::string("FormattedText:  " + format);
}