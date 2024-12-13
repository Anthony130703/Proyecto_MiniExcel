#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cctype>
#include <cstdlib>

namespace utils {

// Convierte una cadena a un número double
inline double stringToDouble(const std::string& str) {
    char* end;
    double value = std::strtod(str.c_str(), &end);
    if (*end != '\0') {
        throw std::invalid_argument("Invalid double conversion");
    }
    return value;
}

// Convierte una cadena a un número entero
inline bool stringToInt(const std::string& str, int& result) {
    char* end;
    long value = std::strtol(str.c_str(), &end, 10);
    if (*end == '\0') {
        result = static_cast<int>(value);
        return true;
    }
    return false;
}

// Verifica si un carácter es un dígito
inline bool isDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c));
}

// Verifica si un carácter es una letra
inline bool isAlpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c));
}

// Convierte un carácter a minúscula
inline char toLower(char c) {
    return std::tolower(static_cast<unsigned char>(c));
}

} // namespace utils

#endif // UTILS_H
