#include "include/Passage/Body/Macros/EndIfMacro.h"
#include "include/Passage/Body/BodyPartsVisitor.h"

EndIfMacro::EndIfMacro() {
}

std::string EndIfMacro::to_string() const {
    return "EndIfMacro";
}

void EndIfMacro::accept(BodyPartsVisitor visitor) {
    visitor.visit(*this);
}