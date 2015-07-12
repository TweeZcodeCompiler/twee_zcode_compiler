#include "include/Passage/Body/Newline.h"
#include "include/Passage/Body/IBodyPartsVisitor.h"

#include <string>


Newline::Newline() {
}

std::string Newline::to_string() const {
    return "Newline: \\n";
}

void Newline::accept(const IBodyPartsVisitor & visitor) {
    visitor.visit(*this);
}