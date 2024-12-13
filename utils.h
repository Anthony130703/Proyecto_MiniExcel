#ifndef UTILS_H
#define UTILS_H

namespace utils {
    inline bool isDigit(char c) {
        return (c >= '0' && c <= '9');
    }

    inline bool isAlpha(char c) {
        return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
    }

    inline bool stringToInt(const std::string& str, int& value) {
        try {
            value = std::stoi(str);
            return true;
        } catch (...) {
            return false;
        }
    }

    inline double stringToDouble(const std::string& str) {
        try {
            return std::stod(str);
        } catch (...) {
            return 0.0;
        }
    }

    inline char toLower(char c) {
        return (c >= 'A' && c <= 'Z') ? c + 32 : c;
    }
}

#endif // UTILS_H
