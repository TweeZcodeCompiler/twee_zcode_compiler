//
// Created by tobias on 6/30/15.
//

#include <plog/Log.h>
#include "include/Passage/Body/Formatting.h"

Formatting::Formatting(std::string format) {
    if (format == "//") {
        this->format = Format:: ITALIC;
    } else if (format == "''") {
        this->format = Format::BOLD;
    } else if (format == "__") {
        this->format = Format::UNDERLINED;
    } else if ((format == "{{{") || (format == "}}}")) {
        this->format = Format::MONOSPACE;
    }
}

Format Formatting::getFormat() const {
    return this->format;
}

std::string Formatting::getFormatString() const {
    switch (format) {
        case Format:: ITALIC:
            return "Italic";
        case Format:: BOLD:
            return "Bold";
        case Format:: UNDERLINED:
            return "Underlined";
        case Format:: MONOSPACE:
            return "Monospace";
        default:
            break;
    };
}

std::string Formatting::to_string() const {
    return std::string("Formatting:  " + this->getFormatString());
}

void Formatting::accept(IBodyPartsVisitor &visitor) const{
    visitor.visit(*this);
}