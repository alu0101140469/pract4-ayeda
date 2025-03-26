#include <iostream>
#include <cstring>
#include <cstdlib>
#include "HashFunctions.hpp"  // Incluye la definición de persona y las funciones de dispersión/exploración
#include "HashTable.hpp"      // Incluye las secuencias y la tabla hash

using namespace std;

// Función que muestra información detallada sobre el uso del programa.
void printUsage(char* progName) {
    cout << "========================================\n";
    cout << "  Programa de Tabla Hash - Búsqueda por Dispersión\n";
    cout << "========================================\n\n";
    cout << "Descripción:\n";
    cout << "  Este programa implementa una tabla hash configurable para almacenar\n";
    cout << "  registros de personas. Cada persona tiene un ID con formato:\n";
    cout << "    alu<7 dígitos>, prof<7 dígitos> o pas<7 dígitos>\n";
    cout << "  y además guarda su nombre, primer apellido y segundo apellido.\n\n";
    cout << "Uso:\n";
    cout << "  " << progName << " -ts <tableSize> -fd <fdCode> -hash <open|close> [-bs <blockSize>] [-fe <feCode>]\n\n";
    cout << "Opciones:\n";
    cout << "  -ts <tableSize>     Número de celdas de la tabla hash.\n";
    cout << "  -fd <fdCode>        Código de la función de dispersión:\n";
    cout << "                         1  -> Módulo (h(k) = valor_numerico % tableSize)\n";
    cout << "                         2  -> Suma de dígitos (h(k) = suma(dígitos) % tableSize)\n";
    cout << "                         3  -> Pseudoaleatoria (h(k) = {srand(valor_numerico); rand()} % tableSize)\n";
    cout << "  -hash <open|close>  Tipo de dispersión:\n";
    cout << "                         open  -> Dispersión abierta (usa listas dinámicas).\n";
    cout << "                         close -> Dispersión cerrada (usa arrays estáticos).\n";
    cout << "  -bs <blockSize>     Tamaño máximo de registros por celda (solo para 'close').\n";
    cout << "  -fe <feCode>        Código de la función de exploración (solo para 'close'):\n";
    cout << "                         1  -> Exploración lineal (g(k,i) = i)\n";
    cout << "                         2  -> Exploración cuadrática (g(k,i) = i^2)\n";
    cout << "                         3  -> Doble dispersión (g(k,i) = f(k) * i)\n";
    cout << "                         4  -> Redispersión (g(k,i) = f(i)(k))\n\n";
    cout << "Ejemplos:\n";
    cout << "  Dispersión cerrada con exploración lineal:\n";
    cout << "    " << progName << " -ts 100 -fd 1 -hash close -bs 5 -fe 1\n";
    cout << "  Dispersión abierta usando función de suma de dígitos:\n";
    cout << "    " << progName << " -ts 50 -fd 2 -hash open\n";
    cout << "  Para ver esta ayuda:\n";
    cout << "    " << progName << " --help\n";
    cout << "========================================\n";
}

int main(int argc, char* argv[]) {
    // Muestra ayuda si se usa '--help'
    if(argc == 2 && (strcmp(argv[1], "--help") == 0)) {
        printUsage(argv[0]);
        return 0;
    }
    
    // Verifica que se hayan pasado al menos los parámetros mínimos.
    if(argc < 5) {
        printUsage(argv[0]);
        return 1;
    }

    // Variables para los parámetros.
    unsigned tableSize = 0;
    unsigned blockSize = 0;
    int fdCode = 0;
    int feCode = 0;
    string hashType = "";
    
    // Procesa los argumentos de línea de comandos.
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-ts") == 0 && i + 1 < argc) {
            tableSize = atoi(argv[++i]);
        } else if(strcmp(argv[i], "-fd") == 0 && i + 1 < argc) {
            fdCode = atoi(argv[++i]);
        } else if(strcmp(argv[i], "-hash") == 0 && i + 1 < argc) {
            hashType = argv[++i];
        } else if(strcmp(argv[i], "-bs") == 0 && i + 1 < argc) {
            blockSize = atoi(argv[++i]);
        } else if(strcmp(argv[i], "-fe") == 0 && i + 1 < argc) {
            feCode = atoi(argv[++i]);
        }
    }
    
    if(tableSize == 0 || fdCode == 0 || hashType == "") {
        printUsage(argv[0]);
        return 1;
    }
    
    srand(time(NULL));
    
    // Crea la función de dispersión según el código.
    DispersionFunction<persona>* df = nullptr;
    switch(fdCode) {
        case 1:
            df = new ModuleHashFunction<persona>(tableSize);
            break;
        case 2:
            df = new SumHashFunction<persona>(tableSize);
            break;
        case 3:
            df = new PseudoRandomHashFunction<persona>(tableSize);
            break;
        default:
            cout << "Código de función de dispersión inválido." << endl;
            return 1;
    }
    
    // Si se usa dispersión abierta.
    if(hashType == "open") {
        HashTable<persona, dynamicSequence<persona> > table(tableSize, *df);
        
        int option;
        persona p;
        do {
            cout << "\nMenú:\n1. Insertar\n2. Buscar\n0. Salir\nOpción: ";
            cin >> option;
            if(option == 1) {
                // Se solicita introducir los datos de la persona.
                std::string id, nombre, ape1, ape2;
                cout << "Introduce el ID (formato alu/prof/pas seguido de 7 dígitos): ";
                cin >> id;
                cout << "Introduce el nombre: ";
                cin >> nombre;
                cout << "Introduce el primer apellido: ";
                cin >> ape1;
                cout << "Introduce el segundo apellido: ";
                cin >> ape2;
                p = persona(id, nombre, ape1, ape2);
                if(table.insert(p))
                    cout << "Insertado correctamente." << endl;
                else
                    cout << "Error al insertar." << endl;
            } else if(option == 2) {
                std::string id, nombre, ape1, ape2;
                cout << "Introduce el ID (formato alu/prof/pas seguido de 7 dígitos): ";
                cin >> id;
                cout << "Introduce el nombre: ";
                cin >> nombre;
                cout << "Introduce el primer apellido: ";
                cin >> ape1;
                cout << "Introduce el segundo apellido: ";
                cin >> ape2;
                p = persona(id, nombre, ape1, ape2);
                if(table.search(p))
                    cout << "Encontrado." << endl;
                else
                    cout << "No encontrado." << endl;
            }
        } while(option != 0);
    }
    // Si se usa dispersión cerrada.
    else if(hashType == "close") {
        if(blockSize == 0 || feCode == 0) {
            cout << "Para dispersión cerrada se deben proporcionar blockSize y código de función de exploración." << endl;
            return 1;
        }
        ExplorationFunction<persona>* ef = nullptr;
        switch(feCode) {
            case 1:
                ef = new LinearExploration<persona>();
                break;
            case 2:
                ef = new QuadraticExploration<persona>();
                break;
            case 3:
                ef = new DoubleHashExploration<persona>(*df);
                break;
            case 4:
                ef = new RedispersionExploration<persona>();
                break;
            default:
                cout << "Código de función de exploración inválido." << endl;
                return 1;
        }
        HashTable<persona, staticSequence<persona> > table(tableSize, *df, *ef, blockSize);
        
        int option;
        persona p;
        do {
            cout << "\nMenú:\n1. Insertar\n2. Buscar\n0. Salir\nOpción: ";
            cin >> option;
            if(option == 1) {
                std::string id, nombre, ape1, ape2;
                cout << "Introduce el ID (formato alu/prof/pas seguido de 7 dígitos): ";
                cin >> id;
                cout << "Introduce el nombre: ";
                cin >> nombre;
                cout << "Introduce el primer apellido: ";
                cin >> ape1;
                cout << "Introduce el segundo apellido: ";
                cin >> ape2;
                p = persona(id, nombre, ape1, ape2);
                if(table.insert(p))
                    cout << "Insertado correctamente." << endl;
                else
                    cout << "Error al insertar (posible saturación en la celda o tabla)." << endl;
            } else if(option == 2) {
                std::string id, nombre, ape1, ape2;
                cout << "Introduce el ID (formato alu/prof/pas seguido de 7 dígitos): ";
                cin >> id;
                cout << "Introduce el nombre: ";
                cin >> nombre;
                cout << "Introduce el primer apellido: ";
                cin >> ape1;
                cout << "Introduce el segundo apellido: ";
                cin >> ape2;
                p = persona(id, nombre, ape1, ape2);
                if(table.search(p))
                    cout << "Encontrado." << endl;
                else
                    cout << "No encontrado." << endl;
            }
        } while(option != 0);
        delete ef;
    } else {
        cout << "Tipo de hash inválido. Usa 'open' o 'close'." << endl;
        return 1;
    }
    
    delete df;
    return 0;
}
