#ifndef HASHFUNCTIONS_HPP
#define HASHFUNCTIONS_HPP

#include <iostream>
#include <cstdlib>
#include <ctime>

// Daniel Palenzuela Álvarez alu0101140469

// Clase nif, Representa la parte numérica del NIF
// Encapsula un número entero de 8 dígitos y sobrecarga operadores necesarios.
class nif {
private:
    long number; // Almacena el número de 8 dígitos
public:
    // Constructor por defecto
    nif() {
        // Genera un número aleatorio entre 10000000 y 99999999 (8 dig).
        number = 10000000 + rand() % 90000000;
    }
    // Constructor que recibe un valor long para inicializar el nif.
    nif(long n) : number(n) {}

    // Operador de conversión a long, para poder realizar operaciones numéricas.
    operator long() const { return number; }

    // Sobrecarga del operador de igualdad.
    bool operator==(const nif &other) const { return number == other.number; }
    // Sobrecarga del operador de desigualdad.
    bool operator!=(const nif &other) const { return number != other.number; }
    // Sobrecarga del operador menor que.
    bool operator<(const nif &other) const { return number < other.number; }
    // Sobrecarga del operador mayor que.
    bool operator>(const nif &other) const { return number > other.number; }
};

// Funciones de Dispersión
// Clase base abstracta para las funciones de dispersión.
// Define el operador función que con una clave devuelve una posición en la tabla.
template<class Key>
class DispersionFunction {
public:
    // Método virtual puro, cada función de dispersión debe implementarlo.
    virtual unsigned operator()(const Key &key) const = 0;
    virtual ~DispersionFunction() {}
};

// Función de dispersión con el método módulo.
// Calcular h(k) = k % tableSize
template<class Key>
class ModuleHashFunction : public DispersionFunction<Key> {
private:
    unsigned tableSize; // Tamaño de la tabla, para limitar la posición.
public:
    // Constructor que inicializa el tamaño de la tabla.
    ModuleHashFunction(unsigned ts) : tableSize(ts) {}
    
    // Implementación del operador función.
    unsigned operator()(const Key &key) const {
        // Se convierte la clave a long y se calcula el módulo.
        return static_cast<unsigned>(static_cast<long>(key)) % tableSize;
    }
};

// Función de dispersión con la suma de dígitos de la clave.
// Calcular h(k) = sum(ki) % tableSize
template<class Key>
class SumHashFunction : public DispersionFunction<Key> {
private:
    unsigned tableSize; // Tamaño de la tabla.
public:
    // Constructor que recibe el tamaño de la tabla.
    SumHashFunction(unsigned ts) : tableSize(ts) {}
    
    // Implementación del operador función.
    unsigned operator()(const Key &key) const {
        // Se convierte la clave a long.
        long n = static_cast<long>(key);
        unsigned sum = 0;
        // Se suman los dígitos del número.
        while(n > 0) {
            sum += n % 10; // Suma el último dígito.
            n /= 10;       // Elimina el último dígito.
        }
        return sum % tableSize; // Se aplica módulo para obtener la posición.
    }
};

// Función de dispersión pseudoaleatoria.
// Calcular h(k) = {srand(k); rand() % tableSize}
// La semilla se inicializa con el valor de la clave para obtener resultados pseudoaleatorios.
template<class Key>
class PseudoRandomHashFunction : public DispersionFunction<Key> {
private:
    unsigned tableSize; // Tamaño de la tabla.
public:
    // Constructor que recibe el tamaño de la tabla.
    PseudoRandomHashFunction(unsigned ts) : tableSize(ts) {}
    
    // Implementación del operador función.
    unsigned operator()(const Key &key) const {
        // Inicializa el generador de números aleatorios con la clave.
        srand(static_cast<unsigned>(static_cast<long>(key)));
        // Devuelve un número aleatorio en el rango [0, tableSize-1].
        return rand() % tableSize;
    }
};


// Funciones de Exploración
// Clase base abstracta para las estrategias de exploración.
// Define un operador función que recibe la clave y el número de intento, devolviendo un desplazamiento.
template<class Key>
class ExplorationFunction {
public:
    // Método virtual puro.
    virtual unsigned operator()(const Key &key, unsigned i) const = 0;
    virtual ~ExplorationFunction() {}
};

// Exploración lineal: g(k, i) = i
// Devuelve el número de intento.
template<class Key>
class LinearExploration : public ExplorationFunction<Key> {
public:
    unsigned operator()(const Key &key, unsigned i) const {
        (void) key;
        return i;
    }
};

// Exploración cuadrática: g(k, i) = i^2
// Devuelve el cuadrado del número de intento.
template<class Key>
class QuadraticExploration : public ExplorationFunction<Key> {
public:
    unsigned operator()(const Key &key, unsigned i) const {
        (void) key;
        return i * i;
    }
};

// Exploración por doble dispersión: g(k, i) = f(k) * i
// Recibe una función de dispersión auxiliar (f(k)) a través del constructor.
template<class Key>
class DoubleHashExploration : public ExplorationFunction<Key> {
private:
    DispersionFunction<Key>& secondary; // Función auxiliar para calcular f(k)
public:
    // Constructor que recibe la función auxiliar.
    DoubleHashExploration(DispersionFunction<Key>& sec) : secondary(sec) {}
    
    // Devuelve el desplazamiento multiplicando el resultado de f(k) por el intento.
    unsigned operator()(const Key &key, unsigned i) const {
        return secondary(key) * i;
    }
};

// Exploración por redispersión: g(k, i) = f^(i)(k)
// Utiliza el generador pseudoaleatorio: inicializa la semilla con la clave y devuelve el i-ésimo número aleatorio.
template<class Key>
class RedispersionExploration : public ExplorationFunction<Key> {
public:
    unsigned operator()(const Key &key, unsigned i) const {
        // Inicializa la semilla con el valor de la clave.
        srand(static_cast<unsigned>(static_cast<long>(key)));
        unsigned offset = 0;
        // Llama a rand() i+1 veces, devolviendo el último valor.
        for(unsigned j = 0; j <= i; j++){
            offset = rand();
        }
        return offset;
    }
};

#endif