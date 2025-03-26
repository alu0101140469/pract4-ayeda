#ifndef HASHFUNCTIONS_HPP
#define HASHFUNCTIONS_HPP

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

// ----------------------------
// Clase persona
// ----------------------------
// Representa a una persona con un ID, nombre y dos apellidos.
// El ID tiene el formato: <tipo><7 dígitos>, donde <tipo> es "alu", "prof" o "pas".
// La conversión a long se realiza extrayendo la parte numérica y sumando un offset
// según el tipo, de modo que "prof" y "pas" se diferencian de "alu".
class persona {
private:
    std::string id;       // Ejemplos: "alu0001345", "prof0001234", "pas0001234"
    std::string nombre;
    std::string apellido1;
    std::string apellido2;
public:
    // Constructor que recibe todos los datos
    persona(const std::string &id, const std::string &nombre,
            const std::string &apellido1, const std::string &apellido2)
      : id(id), nombre(nombre), apellido1(apellido1), apellido2(apellido2) {}

    // Constructor por defecto (vacío, aunque podría generar datos aleatorios)
    persona() : id("alu0000000"), nombre(""), apellido1(""), apellido2("") {}

    // Operador de conversión a long:
    // Extrae la parte numérica (7 dígitos) y le suma un offset según el prefijo.
    // Offset: "alu" -> 0, "prof" -> 10000000, "pas" -> 20000000.
    operator long() const {
        long offset = 0;
        std::string numPart;
        if(id.substr(0,4) == "prof") {
            offset = 10000000;
            numPart = id.substr(4); // Desde el 5º carácter
        } else if(id.substr(0,3) == "pas") {
            offset = 20000000;
            numPart = id.substr(3);
        } else if(id.substr(0,3) == "alu") {
            offset = 0;
            numPart = id.substr(3);
        } else {
            // Si el formato es desconocido, se usa 0
            numPart = "0";
        }
        return offset + std::stol(numPart);
    }

    // Operadores de comparación: se comparan los ID (se asume que son únicos).
    bool operator==(const persona &other) const { return id == other.id; }
    bool operator!=(const persona &other) const { return id != other.id; }
    bool operator<(const persona &other) const { return id < other.id; }
    bool operator>(const persona &other) const { return id > other.id; }

    // Métodos para obtener los datos (opcional para mostrar)
    std::string getId() const { return id; }
    std::string getNombre() const { return nombre; }
    std::string getApellido1() const { return apellido1; }
    std::string getApellido2() const { return apellido2; }
};

// ----------------------------
// Funciones de Dispersión
// ----------------------------

// Clase base abstracta para las funciones de dispersión.
// Dado un objeto de tipo Key (ahora persona), retorna una posición en la tabla.
template<class Key>
class DispersionFunction {
public:
    virtual unsigned operator()(const Key &key) const = 0;
    virtual ~DispersionFunction() {}
};

// Función de dispersión usando el método módulo.
// Calcula: h(k) = (valor numérico de key) % tableSize.
template<class Key>
class ModuleHashFunction : public DispersionFunction<Key> {
private:
    unsigned tableSize;
public:
    ModuleHashFunction(unsigned ts) : tableSize(ts) {}
    unsigned operator()(const Key &key) const {
        return static_cast<unsigned>(static_cast<long>(key)) % tableSize;
    }
};

// Función de dispersión basada en la suma de dígitos de la parte numérica de key.
// Calcula: h(k) = (suma de dígitos del valor numérico) % tableSize.
template<class Key>
class SumHashFunction : public DispersionFunction<Key> {
private:
    unsigned tableSize;
public:
    SumHashFunction(unsigned ts) : tableSize(ts) {}
    unsigned operator()(const Key &key) const {
        long n = static_cast<long>(key);
        unsigned sum = 0;
        while(n > 0) {
            sum += n % 10;
            n /= 10;
        }
        return sum % tableSize;
    }
};

// Función de dispersión pseudoaleatoria.
// Inicializa la semilla con el valor numérico de key y retorna un número aleatorio en [0, tableSize-1].
template<class Key>
class PseudoRandomHashFunction : public DispersionFunction<Key> {
private:
    unsigned tableSize;
public:
    PseudoRandomHashFunction(unsigned ts) : tableSize(ts) {}
    unsigned operator()(const Key &key) const {
        srand(static_cast<unsigned>(static_cast<long>(key)));
        return rand() % tableSize;
    }
};

// ----------------------------
// Funciones de Exploración
// ----------------------------

// Clase base abstracta para las estrategias de exploración.
// Recibe la clave y el número de intento, y retorna un desplazamiento.
template<class Key>
class ExplorationFunction {
public:
    virtual unsigned operator()(const Key &key, unsigned i) const = 0;
    virtual ~ExplorationFunction() {}
};

// Exploración lineal: g(k, i) = i.
template<class Key>
class LinearExploration : public ExplorationFunction<Key> {
public:
    unsigned operator()(const Key &key, unsigned i) const {
        (void) key; // No se usa la clave
        return i;
    }
};

// Exploración cuadrática: g(k, i) = i^2.
template<class Key>
class QuadraticExploration : public ExplorationFunction<Key> {
public:
    unsigned operator()(const Key &key, unsigned i) const {
        (void) key;
        return i * i;
    }
};

// Exploración por doble dispersión: g(k, i) = f(k) * i,
// donde f(k) es una función de dispersión auxiliar.
template<class Key>
class DoubleHashExploration : public ExplorationFunction<Key> {
private:
    DispersionFunction<Key>& secondary;
public:
    DoubleHashExploration(DispersionFunction<Key>& sec) : secondary(sec) {}
    unsigned operator()(const Key &key, unsigned i) const {
        return secondary(key) * i;
    }
};

// Exploración por redispersión: g(k, i) = f(i)(k),
// se utiliza el generador pseudoaleatorio inicializado con key.
template<class Key>
class RedispersionExploration : public ExplorationFunction<Key> {
public:
    unsigned operator()(const Key &key, unsigned i) const {
        srand(static_cast<unsigned>(static_cast<long>(key)));
        unsigned offset = 0;
        for(unsigned j = 0; j <= i; j++){
            offset = rand();
        }
        return offset;
    }
};

#endif // HASHFUNCTIONS_HPP
