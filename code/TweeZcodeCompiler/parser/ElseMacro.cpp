#include "include/Passage/Body/Macros/ElseMacro.h"
#include "include/Passage/Body/BodyPartsVisitor.h"

ElseMacro::ElseMacro() {
}

std::string ElseMacro::to_string() const {
    return "ElseMacro";
}

void ElseMacro::accept(BodyPartsVisitor& visitor) {
    visitor.visit(*this);
}