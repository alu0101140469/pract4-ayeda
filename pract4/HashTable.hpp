#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <vector>
#include "HashFunctions.hpp" // Ahora incluye la definición de persona y funciones de dispersión/exploración
#include <list>

// ----------------------------
// Clases de Secuencias
// ----------------------------

// Clase base abstracta para una secuencia (celda de la tabla hash)
template<class Key>
class Sequence {
public:
    virtual bool search(const Key &key) const = 0;
    virtual bool insert(const Key &key) = 0;
    virtual ~Sequence() {}
};

// Secuencia dinámica para dispersión abierta (usa std::list)
template<class Key>
class dynamicSequence : public Sequence<Key> {
private:
    std::list<Key> data;
public:
    bool search(const Key &key) const override {
        for(const auto &elem : data) {
            if(elem == key) return true;
        }
        return false;
    }
    bool insert(const Key &key) override {
        data.push_back(key);
        return true;
    }
};

// Secuencia estática para dispersión cerrada (usa std::vector)
template<class Key>
class staticSequence : public Sequence<Key> {
private:
    std::vector<Key> data;
    unsigned blockSize;
public:
    staticSequence(unsigned bs) : blockSize(bs) {}
    bool search(const Key &key) const override {
        for(const auto &elem : data) {
            if(elem == key) return true;
        }
        return false;
    }
    bool insert(const Key &key) override {
        if(isFull()) return false;
        data.push_back(key);
        return true;
    }
    bool isFull() const {
        return data.size() >= blockSize;
    }
};

// ----------------------------
// Tabla Hash Genérica
// ----------------------------

// Versión general para dispersión cerrada (usa staticSequence)
template<class Key, class Container = staticSequence<Key> >
class HashTable : public Sequence<Key> {
private:
    unsigned tableSize;
    unsigned blockSize;
    std::vector<Container*> table;
    DispersionFunction<Key>& fd;
    ExplorationFunction<Key>& fe;
public:
    HashTable(unsigned ts, DispersionFunction<Key>& dispFunc, ExplorationFunction<Key>& explFunc, unsigned bs)
    : tableSize(ts), blockSize(bs), fd(dispFunc), fe(explFunc) {
        table.resize(tableSize, nullptr);
        for(unsigned i = 0; i < tableSize; i++){
            table[i] = new Container(blockSize);
        }
    }
    ~HashTable(){
        for(auto ptr : table)
            delete ptr;
    }
    bool search(const Key &key) const override {
        unsigned h = fd(key);
        unsigned maxAttempts = tableSize;
        for(unsigned i = 0; i < maxAttempts; i++){
            unsigned pos = (h + fe(key, i)) % tableSize;
            if(table[pos]->search(key))
                return true;
        }
        return false;
    }
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

// Especialización parcial para dispersión abierta (usa dynamicSequence)
// No se usan función de exploración ni blockSize.
template<class Key>
class HashTable<Key, dynamicSequence<Key> > : public Sequence<Key> {
private:
    unsigned tableSize;
    std::vector<dynamicSequence<Key>*> table;
    DispersionFunction<Key>& fd;
public:
    HashTable(unsigned ts, DispersionFunction<Key>& dispFunc)
    : tableSize(ts), fd(dispFunc) {
        table.resize(tableSize, nullptr);
        for(unsigned i = 0; i < tableSize; i++){
            table[i] = new dynamicSequence<Key>();
        }
    }
    ~HashTable(){
        for(auto ptr : table)
            delete ptr;
    }
    bool search(const Key &key) const override {
        unsigned pos = fd(key);
        return table[pos]->search(key);
    }
    bool insert(const Key &key) override {
        unsigned pos = fd(key);
        return table[pos]->insert(key);
    }
};

#endif // HASHTABLE_HPP
