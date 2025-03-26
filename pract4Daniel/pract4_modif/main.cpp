#include <iostream>
#include <cstring>
#include <cstdlib>
#include "HashFunctions.hpp"
#include "HashTable.hpp"

using namespace std;

// Daniel Palenzuela Álvarez alu0101140469

// modif: Cambiar objeto key de nif a persona con 3 tipos de ID (alu, prof, pas) seguido de 7 números y la key debe guardar nombre, apellido1 y apellido2
// alu/prof/pas pueden repetirse en numeros, los diferenciará las letras (alu, prof y pas los clasifica)
// Ejemplo: Key { ID: alu0001345, prof0001234, pas0001234 }

// Función para mostrar el uso correcto del programa por línea de comandos.
void printUsage(char* progName) {
    cout << "  Programa de Tabla Hash - Búsqueda por Dispersión\n";
    cout << "Instrucciones:\n";
    cout << "  " << progName << " -ts <tableSize> -fd <fdCode> -hash <open|close> [-bs <blockSize>] [-fe <feCode>]\n\n";
    cout << "Opciones:\n";
    cout << "  -ts <tableSize>   Tamaño de la tabla hash (número de celdas).\n";
    cout << "  -fd <fdCode>      Función de dispersión:\n";
    cout << "                      1  -> Módulo\n";
    cout << "                      2  -> Suma de dígitos\n";
    cout << "                      3  -> Pseudoaleatoria\n";
    cout << "  -hash <open|close>  Tipo de dispersión:\n";
    cout << "                      open  -> Dispersión abierta (usa listas dinámicas).\n";
    cout << "                      close -> Dispersión cerrada (usa arrays estáticos).\n";
    cout << "  -bs <blockSize>   Tamaño del bloque (exclusivo de close, número máximo de registros por celda).\n";
    cout << "  -fe <feCode>      Función de exploración (exclusivo de close):\n";
    cout << "                      1  -> Exploración lineal\n";
    cout << "                      2  -> Exploración cuadrática\n";
    cout << "                      3  -> Doble dispersión\n";
    cout << "                      4  -> Redispersión\n\n";
}

int main(int argc, char* argv[]) {
    // Usando --help se muestra el mensaje de ayuda y se sale.
    if(argc == 2 && (strcmp(argv[1], "--help") == 0)) {
        printUsage(argv[0]);
        return 0;
    }

    // Verifica que se hayan pasado al menos los parámetros mínimos.
    if(argc < 5) {
        printUsage(argv[0]);
        return 1;
    }

    // Variables para almacenar los parámetros de la línea de comandos.
    unsigned tableSize = 0;  // Tamaño de la tabla hash
    unsigned blockSize = 0;  // Tamaño de bloque para dispersión cerrada
    int fdCode = 0;          // Código para elegir la función de dispersión
    int feCode = 0;          // Código para elegir la función de exploración (dispersión cerrada)
    string hashType = "";    // Tipo de dispersión open o close
    
    // Procesa los argumentos de línea de comandos.
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-ts") == 0 && i + 1 < argc) {
            tableSize = atoi(argv[++i]); // Asigna el tamaño de la tabla
        } else if(strcmp(argv[i], "-fd") == 0 && i + 1 < argc) {
            fdCode = atoi(argv[++i]);    // Asigna el código de función de dispersión
        } else if(strcmp(argv[i], "-hash") == 0 && i + 1 < argc) {
            hashType = argv[++i];         // Asigna el tipo de dispersión (open o close)
        } else if(strcmp(argv[i], "-bs") == 0 && i + 1 < argc) {
            blockSize = atoi(argv[++i]);  // Asigna el tamaño de bloque (solo para dispersión cerrada)
        } else if(strcmp(argv[i], "-fe") == 0 && i + 1 < argc) {
            feCode = atoi(argv[++i]);     // Asigna el código de función de exploración (solo para dispersión cerrada)
        }
    }
    
    // Verifica que se hayan establecido los parámetros mínimos requeridos.
    if(tableSize == 0 || fdCode == 0 || hashType == "") {
        printUsage(argv[0]);
        return 1;
    }
    
    // Inicializa la semilla para números aleatorios.
    srand(time(NULL));
    
    // Crear la función de dispersión según el código especificado.
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
    
    // Si se selecciona dispersión abierta, se utiliza dynamicSequence.
    if(hashType == "open") {
        // Crea la tabla hash con dynamicSequence. No se usa blockSize ni exploración.
        HashTable<persona, dynamicSequence<persona> > table(tableSize, *df);
        
        int option;
        persona p;
        // Menú interactivo para añadir y buscar personas.
        do {
            cout << "\nMenú principal:\n1. Insertar\n2. Buscar\n0. Salir\nOpción: ";
            cin >> option;
            if(option == 1) {
                std::string id, nombre, apel1, apel2;
                cout << "Introduce el ID (alu/prof/pas seguido de 7 num): ";
                cin >> id;
                cout << "Introduce el nombre: ";
                cin >> nombre;
                cout << "Introduce el primer apellido: ";
                cin >> apel1;
                cout << "Introduce el segundo apellido: ";
                cin >> apel2;

                p = persona(id, nombre, apel1, apel2);

                if(table.insert(p))
                    cout << "Añadido correctamente." << endl;
                else
                    cout << "Error al insertar." << endl;

            } else if(option == 2) {
                std::string id;
                cout << "Introduce el ID (alu/prof/pas seguido de 7 num): ";
                cin >> id;

                p = persona(id, "", "", "");

                if(table.search(p))
                    cout << "Encontrado." << endl;
                else
                    cout << "No encontrado." << endl;
            }
        } while(option != 0);
    } 
    // Si se selecciona dispersión cerrada, se utiliza staticSequence.
    else if(hashType == "close") {
        // Verifica que se hayan especificado blockSize y código de función de exploración.
        if(blockSize == 0 || feCode == 0) {
            cout << "Para dispersión cerrada se deben proporcionar blockSize y código de función de exploración." << endl;
            return 1;
        }
        ExplorationFunction<persona>* ef = nullptr;
        // Selecciona la función de exploración según el código.
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
        // Crea la tabla hash con staticSequence, utilizando blockSize y la función de exploración.
        HashTable<persona, staticSequence<persona> > table(tableSize, *df, *ef, blockSize);
        
        int option;
        persona p;
        // Menú interactivo para insertar y buscar claves.
        do {
            cout << "\nMenú principal:\n1. Insertar\n2. Buscar\n0. Salir\nOpción: ";
            cin >> option;
            if(option == 1) {
                std::string id, nombre, apel1, apel2;
                cout << "Introduce el ID (alu/prof/pas seguido de 7 num): ";
                cin >> id;
                cout << "Introduce el nombre: ";
                cin >> nombre;
                cout << "Introduce el primer apellido: ";
                cin >> apel1;
                cout << "Introduce el segundo apellido: ";
                cin >> apel2;

                p = persona(id, nombre, apel1, apel2);

                if(table.insert(p))
                    cout << "Añadido correctamente." << endl;
                else
                    cout << "Error al insertar (posible saturación en el bloque o tabla)." << endl;
            
            } else if(option == 2) {
                std::string id;
                cout << "Introduce el ID (alu/prof/pas seguido de 7 num): ";
                cin >> id;

                p = persona(id, "", "", "");

                if(table.search(p))
                    cout << "Encontrado." << endl;
                else
                    cout << "No encontrado." << endl;
            }
        } while(option != 0);
        // Libera la memoria asignada a la función de exploración.
        delete ef;
    } else {
        cout << "Tipo de hash inválido. Usa open o close." << endl;
        return 1;
    }
    
    // Libera la memoria asignada a la función de dispersión.
    delete df;
    return 0;
}