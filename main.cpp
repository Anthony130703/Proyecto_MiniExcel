// PROYECTO MINI EXCEL
#include "Spreadsheet.h"
#include <iostream>
#include <string>

void showMenu() {
    std::cout << "\n--- Mini-Excel Interactivo ---\n";
    std::cout << "1. Mostrar hoja de calculo\n";
    std::cout << "2. Editar celda\n";
    std::cout << "3. Salir\n";
    std::cout << "Selecciona una opcion: ";
}

void editCell(Spreadsheet& sheet) {
    std::string rowLabel;
    int col;
    std::string content;

    std::cout << "Ingresa la fila (A, B, C, ...): ";
    std::cin >> rowLabel;
    std::cout << "Ingresa la columna (1, 2, 3, ...): ";
    std::cin >> col;
    std::cin.ignore(); // Ignorar el salto de línea
    std::cout << "Ingresa el contenido de la celda (e.g., '=A1+B2' o '10'): ";
    std::getline(std::cin, content);

    // Convertir etiquetas a índices
    int row = 0;
    for (char c : rowLabel) {
        row = row * 26 + (c - 'A' + 1);
    }
    row -= 1; // Ajustar a índice 0

    // Asignar contenido a la celda
    sheet.setCellContent(row, col - 1, content);
    std::cout << "Celda actualizada correctamente.\n";
}

int main() {
    int rows = 10, cols = 10; // Número inicial de filas y columnas
    Spreadsheet sheet(rows, cols);

    bool running = true;
    while (running) {
        showMenu();

        int option;
        std::cin >> option;

        switch (option) {
        case 1:
            std::cout << "\nContenido de la hoja:\n";
            sheet.display();
            break;

        case 2:
            editCell(sheet);
            break;

        case 3:
            running = false;
            std::cout << "Saliendo del programa...\n";
            break;

        default:
            std::cout << "Opcion no valida. Intentalo de nuevo.\n";
        }
    }

    return 0;
}
