#ifndef CELL_H
#define CELL_H

#include <string>

class Cell {
private:
    std::string content; // Contenido de la celda

public:
    Cell();
    void setContent(const std::string& value);
    std::string getContent() const;
    double getNumericValue() const; // Convierte a número si aplica
};

#endif // CELL_H
