#include "include/Passage/Body/Macros/EndIfMacro.h"

EndIfMacro::EndIfMacro() {
}

std::string EndIfMacro::to_string() const {
    return "EndIfMacro";
}

void EndIfMacro::accept(BodyInspector bodyInspector) { }