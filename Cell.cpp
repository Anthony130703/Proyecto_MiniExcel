#include "Cell.h"
#include <stdexcept>
#include <cstdlib>

Cell::Cell() : content("") {}

void Cell::setContent(const std::string& value) {
    content = value;
}

std::string Cell::getContent() const {
    return content;
}

double Cell::getNumericValue() const {
    try {
        return std::stod(content);
    } catch (const std::invalid_argument&) {
        return 0.0; // Si no es un número, retorna 0
    }
}
