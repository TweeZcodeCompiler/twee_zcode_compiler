#include "include/Passage/Body/Macros/ElseMacro.h"
#include "include/Passage/Body/IBodyPartsVisitor.h"

ElseMacro::ElseMacro() {
}

std::string ElseMacro::to_string() const {
    return "ElseMacro";
}

void ElseMacro::accept(const IBodyPartsVisitor & visitor) {
    visitor.visit(*this);
}