#include "include/Passage/Body/Newline.h"

#include <string>


Newline::Newline() {
}

std::string Newline::to_string() const {
    return "Newline: \\n";
}

void Newline::accept(BodyInspector bodyInspector) const {
    bodyInspector.visit(*this);
}