#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <vector>
#include "HashFunctions.hpp"
#include <list>

// Daniel Palenzuela Álvarez alu0101140469

// modif
// Clases de Secuencias
// Clase base abstracta para una secuencia de claves (una celda de la tabla hash).
// Define las operaciones básicas búsqueda e insert.
template<class Key>
class Sequence {
public:
    // Busca una clave en la secuencia.
    virtual bool search(const Key &key) const = 0;
    // Inserta una clave en la secuencia.
    virtual bool insert(const Key &key) = 0;
    virtual ~Sequence() {}
};

// Implementación de secuencia dinámica para dispersión abierta.
// Usa una lista dinámica para almacenar claves sinónimas.
template<class Key>
class dynamicSequence : public Sequence<Key> {
private:
    std::list<Key> data; // Lista para almacenar las claves
public:
    // Busca la clave recorriendo la lista.
    bool search(const Key &key) const override {
        for(const auto &elem : data) {
            if(elem == key)
                return true;
        }
        return false;
    }
    // Inserta la clave al final de la lista.
    bool insert(const Key &key) override {
        data.push_back(key);
        return true;
    }
};

// Implementación de secuencia estática para dispersión cerrada.
// Usa un vector con un número máximo de claves definido por blockSize.
template<class Key>
class staticSequence : public Sequence<Key> {
private:
    std::vector<Key> data; // Vector para almacenar las claves
    unsigned blockSize;    // Tamaño máximo de la celda
public:
    // Constructor que inicializa el tamaño máximo de la celda.
    staticSequence(unsigned bs) : blockSize(bs) {}

    // Busca la clave recorriendo el vector.
    bool search(const Key &key) const override {
        for(const auto &elem : data) {
            if(elem == key)
                return true;
        }
        return false;
    }
    // Inserta la clave si la celda no está llena.
    bool insert(const Key &key) override {
        if(isFull())
            return false;
        data.push_back(key);
        return true;
    }
    // Devuelve true si la celda ha alcanzado el tamaño máximo.
    bool isFull() const {
        return data.size() >= blockSize;
    }
};

// Tabla Hash Genérica
// Versión general para dispersión cerrada.
// Usa una secuencia estática en cada posición de la tabla.
template<class Key, class Container = staticSequence<Key> >
class HashTable : public Sequence<Key> {
private:
    unsigned tableSize;  // Número de celdas de la tabla
    unsigned blockSize;  // Tamaño máximo de cada celda (para staticSequence)
    std::vector<Container*> table; // Vector de punteros a celdas (secuencias)
    DispersionFunction<Key>& fd;   // Función de dispersión para mapear claves a posiciones
    ExplorationFunction<Key>& fe;  // Función de exploración para resolver colisiones
public:
    // Constructor
    // ts: tamaño de la tabla, dispFunc: función de dispersión, explFunc: función de exploración,
    // bs: tamaño del bloque (celda) para staticSequence
    HashTable(unsigned ts, DispersionFunction<Key>& dispFunc, ExplorationFunction<Key>& explFunc, unsigned bs)
    : tableSize(ts), blockSize(bs), fd(dispFunc), fe(explFunc) {
        // Se redimensiona el vector para tener tableSize posiciones.
        table.resize(tableSize, nullptr);
        // Se crea dinámicamente un objeto Container para cada celda de la tabla.
        for(unsigned i = 0; i < tableSize; i++){
            table[i] = new Container(blockSize);
        }
    }
    // Destructor, libera la memoria asignada a cada celda.
    ~HashTable(){
        for(auto ptr : table)
            delete ptr;
    }
    
    // Búsqueda en la tabla hash.
    // Aplica la función de dispersión para obtener la posición inicial,
    // y utiliza la función de exploración para manejar colisiones.
    bool search(const Key &key) const override {
        unsigned h = fd(key); // Posición inicial usando la función de dispersión
        unsigned maxAttempts = tableSize; // Se limita el número de intentos para evitar bucles infinitos
        for(unsigned i = 0; i < maxAttempts; i++){
            // Calcula la nueva posición aplicando la exploración.
            unsigned pos = (h + fe(key, i)) % tableSize;
            if(table[pos]->search(key))
                return true;
        }
        return false;
    }
    
    // Inserción en la tabla hash.
    // Utiliza el mismo mecanismo que en la búsqueda para encontrar una celda donde insertar.
    bool insert(const Key &key) override {
        unsigned h = fd(key);
        unsigned maxAttempts = tableSize;
        for(unsigned i = 0; i < maxAttempts; i++){
            unsigned pos = (h + fe(key, i)) % tableSize;
            if(table[pos]->insert(key))
                return true;
        }
        return false;
    }
};

// Especialización parcial para dispersión abierta.
// En este caso se utiliza dynamicSequence en cada celda y no se requiere función de exploración ni blockSize.
template<class Key>
class HashTable<Key, dynamicSequence<Key> > : public Sequence<Key> {
private:
    unsigned tableSize;  // Tamaño de la tabla
    std::vector<dynamicSequence<Key>*> table; // Vector de punteros a dynamicSequence
    DispersionFunction<Key>& fd;  // Función de dispersión
public:
    // Constructor, recibe el tamaño de la tabla y la función de dispersión.
    HashTable(unsigned ts, DispersionFunction<Key>& dispFunc)
    : tableSize(ts), fd(dispFunc) {
        table.resize(tableSize, nullptr);
        // Se crea una dynamicSequence para cada celda.
        for(unsigned i = 0; i < tableSize; i++){
            table[i] = new dynamicSequence<Key>();
        }
    }
    // Destructor, libera la memoria asignada a cada dynamicSequence.
    ~HashTable(){
        for(auto ptr : table)
            delete ptr;
    }
    
    // Búsqueda en la tabla
    // Utiliza únicamente la función de dispersión para determinar la celda.
    bool search(const Key &key) const override {
        unsigned pos = fd(key);
        return table[pos]->search(key);
    }
    
    // Inserción en la tabla
    // Inserta la clave en la celda determinada por la función de dispersión.
    bool insert(const Key &key) override {
        unsigned pos = fd(key);
        return table[pos]->insert(key);
    }
};

#endif