#include "include/Passage/Body/Macros/EndIfMacro.h"
#include "include/Passage/Body/IBodyPartsVisitor.h"

EndIfMacro::EndIfMacro() {
}

std::string EndIfMacro::to_string() const {
    return "EndIfMacro";
}

void EndIfMacro::accept(IBodyPartsVisitor & visitor) const {
    visitor.visit(*this);
}