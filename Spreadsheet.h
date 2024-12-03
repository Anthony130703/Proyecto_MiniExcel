#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "Cell.h"
#include <vector>
#include <string>
#include <stdexcept>

class Spreadsheet {
private:
    std::vector<std::vector<Cell>> grid;

    // Genera etiquetas para las filas (A, B, C, ..., Z, AA, AB, ...)
    std::string generateRowLabel(int index) const;

    // Evalúa una expresión matemática (por ejemplo, "=A1+B2")
    double evaluateExpression(const std::string& expression) const;

    // Convierte una referencia de celda (e.g., "A1") a índices de fila y columna
    bool parseCellReference(const std::string& ref, int& row, int& col) const;

    // Convierte una expresión infija a postfija para evaluar matemáticamente
    std::string infixToPostfix(const std::string& expression) const;

    // Evalúa una expresión postfija
    double evaluatePostfix(const std::string& postfix) const;

    // Valida rangos de celdas
    bool isCellInRange(int row, int col) const;

    // Soporte para funciones como SUM
    double evaluateFunction(const std::string& function, const std::string& range) const;

public:
    Spreadsheet(int rows, int cols);

    // Asigna contenido (valor o fórmula) a una celda
    void setCellContent(int row, int col, const std::string& value);

    // Obtiene el contenido textual de una celda
    std::string getCellContent(int row, int col) const;

    // Obtiene el valor numérico evaluado de una celda
    double getCellValue(int row, int col) const;

    // Muestra la hoja de cálculo en consola
    void display() const;
};

#endif // SPREADSHEET_H
