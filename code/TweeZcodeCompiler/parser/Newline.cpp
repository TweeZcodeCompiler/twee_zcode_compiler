#include "include/Passage/Body/Newline.h"
#include "include/Passage/Body/BodyPartsVisitor.h"

#include <string>


Newline::Newline() {
}

std::string Newline::to_string() const {
    return "Newline: \\n";
}

void Newline::accept(BodyPartsVisitor& visitor) {
    visitor.visit(*this);
}