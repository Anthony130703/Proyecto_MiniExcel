#include "Spreadsheet.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <algorithm>

// Constructor
Spreadsheet::Spreadsheet(int rows, int cols)
    : grid(rows, std::vector<Cell>(cols)) {}

// Asigna contenido a una celda
void Spreadsheet::setCellContent(int row, int col, const std::string& value) {
    if (row >= 0 && col >= 0 && row < grid.size() && col < grid[0].size()) {
        grid[row][col].setContent(value);
    }
}

// Obtiene contenido de una celda
std::string Spreadsheet::getCellContent(int row, int col) const {
    if (row >= 0 && col >= 0 && row < grid.size() && col < grid[0].size()) {
        return grid[row][col].getContent();
    }
    return "";
}

// Obtiene el valor numérico de una celda
double Spreadsheet::getCellValue(int row, int col) const {
    std::string content = getCellContent(row, col);

    // Si es una fórmula, evalúala
    if (!content.empty() && content[0] == '=') {
        return evaluateExpression(content.substr(1)); // Quitar '='
    }

    // Si no es fórmula, intenta convertirla a número
    try {
        return std::stod(content);
    } catch (...) {
        return 0.0; // Retorna 0 si no es un número válido
    }
}

// Genera etiquetas para las filas (A, B, C, ..., Z, AA, AB, ...)
std::string Spreadsheet::generateRowLabel(int index) const {
    std::string label;
    while (index >= 0) {
        label = static_cast<char>('A' + (index % 26)) + label;
        index = (index / 26) - 1;
    }
    return label;
}

// Convierte una referencia de celda (e.g., "A1") a índices de fila y columna
bool Spreadsheet::parseCellReference(const std::string& ref, int& row, int& col) const {
    int pos = 0;

    // Leer letras para la fila
    while (pos < ref.size() && std::isalpha(ref[pos])) {
        pos++;
    }

    std::string rowLabel = ref.substr(0, pos);
    std::string colLabel = ref.substr(pos);

    // Convertir fila (letras) a índice
    row = 0;
    for (char c : rowLabel) {
        row = row * 26 + (std::toupper(c) - 'A' + 1);
    }
    row -= 1; // Ajustar a índice 0

    // Convertir columna (números) a índice
    try {
        col = std::stoi(colLabel) - 1; // Ajustar a índice 0
    } catch (...) {
        return false;
    }

    return row >= 0 && col >= 0;
}

// Convierte una expresión infija a postfija
std::string Spreadsheet::infixToPostfix(const std::string& expression) const {
    std::stack<char> ops;
    std::string output;

    auto precedence = [](char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        return 0;
    };

    for (size_t i = 0; i < expression.size(); ++i) {
        char token = expression[i];

        if (std::isdigit(token) || token == '.') {
            while (i < expression.size() && (std::isdigit(expression[i]) || expression[i] == '.')) {
                output += expression[i++];
            }
            output += " ";
            --i;
        } else if (std::isalpha(token)) {
            std::string cellRef;
            while (i < expression.size() && (std::isalpha(expression[i]) || std::isdigit(expression[i]))) {
                cellRef += expression[i++];
            }
            output += cellRef + " ";
            --i;
        } else if (token == '(') {
            ops.push(token);
        } else if (token == ')') {
            while (!ops.empty() && ops.top() != '(') {
                output += ops.top();
                output += " ";
                ops.pop();
            }
            ops.pop();
        } else {
            while (!ops.empty() && precedence(ops.top()) >= precedence(token)) {
                output += ops.top();
                output += " ";
                ops.pop();
            }
            ops.push(token);
        }
    }

    while (!ops.empty()) {
        output += ops.top();
        output += " ";
        ops.pop();
    }

    return output;
}

// Evalúa una expresión postfija
double Spreadsheet::evaluatePostfix(const std::string& postfix) const {
    std::istringstream iss(postfix);
    std::stack<double> values;
    std::string token;

    while (iss >> token) {
        if (std::isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) {
            values.push(std::stod(token));
        } else if (std::isalpha(token[0])) {
            int row, col;
            if (parseCellReference(token, row, col)) {
                values.push(getCellValue(row, col));
            } else {
                throw std::runtime_error("Invalid cell reference: " + token);
            }
        } else {
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            switch (token[0]) {
                case '+': values.push(a + b); break;
                case '-': values.push(a - b); break;
                case '*': values.push(a * b); break;
                case '/': values.push(b != 0 ? a / b : throw std::runtime_error("Division by zero")); break;
                default: throw std::runtime_error("Unknown operator: " + token);
            }
        }
    }

    return values.top();
}

// Método principal para evaluar expresiones
double Spreadsheet::evaluateExpression(const std::string& expression) const {
    // Si es una función como SUM(A1:B2), evalúala primero
    if (expression.rfind("SUM(", 0) == 0) {
        size_t start = expression.find('(') + 1;
        size_t end = expression.find(')');
        std::string range = expression.substr(start, end - start);
        return evaluateFunction("SUM", range);
    }
    // Si no es una función, evalúa la expresión normal
    std::string postfix = infixToPostfix(expression);
    return evaluatePostfix(postfix);
}

// Soporte para funciones como SUM
double Spreadsheet::evaluateFunction(const std::string& function, const std::string& range) const {
    if (function == "SUM") {
        int startRow, startCol, endRow, endCol;
        std::string startCell = range.substr(0, range.find(':'));
        std::string endCell = range.substr(range.find(':') + 1);

        if (parseCellReference(startCell, startRow, startCol) && parseCellReference(endCell, endRow, endCol)) {
            double sum = 0.0;
            for (int r = startRow; r <= endRow; ++r) {
                for (int c = startCol; c <= endCol; ++c) {
                    sum += getCellValue(r, c);
                }
            }
            return sum;
        } else {
            throw std::runtime_error("Invalid range in SUM: " + range);
        }
    }
    throw std::runtime_error("Unsupported function: " + function);
}

// Valida si una celda está dentro del rango de la hoja de cálculo
bool Spreadsheet::isCellInRange(int row, int col) const {
    return row >= 0 && row < grid.size() && col >= 0 && col < grid[0].size();
}

// Mostrar hoja de cálculo con etiquetas
void Spreadsheet::display() const {
    int cols = grid[0].size();
    int rows = grid.size();

    // Mostrar encabezados de columnas
    std::cout << std::setw(6) << " "; // Espacio para etiquetas de filas
    for (int col = 1; col <= cols; ++col) {
        std::cout << std::setw(10) << col << " | ";
    }
    std::cout << "\n";

    // Mostrar separador
    std::cout << std::string(6 + (cols * 13), '-') << "\n";

    // Mostrar filas con etiquetas
    for (int row = 0; row < rows; ++row) {
        std::cout << std::setw(6) << generateRowLabel(row) << " | ";
        for (int col = 0; col < cols; ++col) {
            try {
                // Evaluar contenido si es fórmula
                std::string content = grid[row][col].getContent();
                if (!content.empty() && content[0] == '=') {
                    // Asegurar que los números se impriman con decimales
                    std::cout << std::setw(10) << std::fixed << std::setprecision(2) 
                              << evaluateExpression(content.substr(1)) << " | ";
                } else {
                    std::cout << std::setw(10) << content << " | ";
                }
            } catch (const std::exception& e) {
                std::cout << std::setw(10) << "ERR" << " | ";
            }
        }
        std::cout << "\n";
    }
}
