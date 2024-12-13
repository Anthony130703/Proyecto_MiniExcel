#include "Spreadsheet.h"
#include "utils.h"
#include <iostream>
#include <cmath>

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
    return utils::stringToDouble(content);
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
    while (pos < ref.size() && utils::isAlpha(ref[pos])) {
        pos++;
    }

    std::string rowLabel = ref.substr(0, pos);
    std::string colLabel = ref.substr(pos);

    // Convertir fila (letras) a índice
    row = 0;
    for (char c : rowLabel) {
        row = row * 26 + ((utils::toLower(c) - 'a') + 1);
    }
    row -= 1; // Ajustar a índice 0

    // Convertir columna (números) a índice
    if (!utils::stringToInt(colLabel, col)) {
        return false;
    }
    col -= 1; // Ajustar a índice 0

    return row >= 0 && col >= 0;
}

// Convierte una expresión infija a postfija
std::string Spreadsheet::infixToPostfix(const std::string& expression) const {
    char ops[100];
    int top = -1;
    std::string output;

    auto precedence = [](char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        return 0;
    };

    for (size_t i = 0; i < expression.size(); ++i) {
        char token = expression[i];

        if (utils::isDigit(token) || token == '.') {
            while (i < expression.size() && (utils::isDigit(expression[i]) || expression[i] == '.')) {
                output += expression[i++];
            }
            output += " ";
            --i;
        } else if (utils::isAlpha(token)) {
            std::string cellRef;
            while (i < expression.size() && (utils::isAlpha(expression[i]) || utils::isDigit(expression[i]))) {
                cellRef += expression[i++];
            }
            output += cellRef + " ";
            --i;
        } else if (token == '(') {
            ops[++top] = token;
        } else if (token == ')') {
            while (top >= 0 && ops[top] != '(') {
                output += ops[top--];
                output += " ";
            }
            --top;
        } else {
            while (top >= 0 && precedence(ops[top]) >= precedence(token)) {
                output += ops[top--];
                output += " ";
            }
            ops[++top] = token;
        }
    }

    while (top >= 0) {
        output += ops[top--];
        output += " ";
    }

    return output;
}

// Evalúa una expresión postfija
double Spreadsheet::evaluatePostfix(const std::string& postfix) const {
    double values[100];
    int top = -1;
    std::string token;

    for (size_t i = 0; i < postfix.size(); ++i) {
        if (postfix[i] == ' ') continue;

        if (utils::isDigit(postfix[i]) || (postfix[i] == '-' && i + 1 < postfix.size() && utils::isDigit(postfix[i + 1]))) {
            size_t start = i;
            while (i < postfix.size() && (utils::isDigit(postfix[i]) || postfix[i] == '.')) {
                ++i;
            }
            token = postfix.substr(start, i - start);
            values[++top] = utils::stringToDouble(token);
            --i;
        } else if (utils::isAlpha(postfix[i])) {
            token.clear();
            while (i < postfix.size() && (utils::isAlpha(postfix[i]) || utils::isDigit(postfix[i]))) {
                token += postfix[i++];
            }
            int row, col;
            if (parseCellReference(token, row, col)) {
                values[++top] = getCellValue(row, col);
            } else {
                return 0.0; // Referencia inválida
            }
            --i;
        } else {
            double b = values[top--];
            double a = values[top--];
            switch (postfix[i]) {
                case '+': values[++top] = a + b; break;
                case '-': values[++top] = a - b; break;
                case '*': values[++top] = a * b; break;
                case '/': values[++top] = b != 0 ? a / b : 0.0; break;
            }
        }
    }

    return values[top];
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
            return 0.0;
        }
    }
    return 0.0;
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
    std::cout << "      ";
    for (int col = 1; col <= cols; ++col) {
        std::cout << "    " << col << " | ";
    }
    std::cout << "\n";

    // Mostrar separador
    for (int i = 0; i < 6 + (cols * 8); ++i) std::cout << "-";
    std::cout << "\n";

    // Mostrar filas con etiquetas
    for (int row = 0; row < rows; ++row) {
        std::cout << generateRowLabel(row) << " | ";
        for (int col = 0; col < cols; ++col) {
            try {
                // Evaluar contenido si es fórmula
                std::string content = grid[row][col].getContent();
                if (!content.empty() && content[0] == '=') {
                    std::cout << evaluateExpression(content.substr(1)) << " | ";
                } else {
                    std::cout << content << " | ";
                }
            } catch (...) {
                std::cout << "ERR | ";
            }
        }
        std::cout << "\n";
    }
}
