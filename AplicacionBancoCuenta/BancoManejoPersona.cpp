#include "BancoManejoPersona.h"
#include <algorithm>

BancoManejoPersona::BancoManejoPersona() : listaPersonas(nullptr) {}

BancoManejoPersona::~BancoManejoPersona() {
    eliminarPersonasRecursivo(listaPersonas);
}

// Implementación recursiva para eliminar toda la lista
void BancoManejoPersona::eliminarPersonasRecursivo(NodoPersona* nodo) {
    if (!nodo) return;

    eliminarPersonasRecursivo(nodo->siguiente);
    delete nodo;
}

// Búsqueda recursiva más eficiente
NodoPersona* BancoManejoPersona::buscarPersonaRecursivo(NodoPersona* nodo, const std::string& cedula) {
    if (!nodo) return nullptr;
    if (nodo->persona && nodo->persona->getCedula() == cedula) return nodo;

    return buscarPersonaRecursivo(nodo->siguiente, cedula);
}

void BancoManejoPersona::agregarPersona(Persona* persona) {
    if (!persona) return;

    NodoPersona* nuevo = new NodoPersona(persona);
    nuevo->siguiente = listaPersonas;
    listaPersonas = nuevo;
}

Persona* BancoManejoPersona::buscarPersonaPorCedula(const std::string& cedula) {
    NodoPersona* nodo = buscarPersonaRecursivo(listaPersonas, cedula);
    return nodo ? nodo->persona : nullptr;
}

bool BancoManejoPersona::existePersona(const std::string& cedula) {
    return buscarPersonaPorCedula(cedula) != nullptr;
}

// Aplicar función recursivamente
void BancoManejoPersona::aplicarFuncionRecursivo(NodoPersona* nodo, const std::function<void(Persona*)>& funcion) {
    if (!nodo) return;

    if (nodo->persona) {
        funcion(nodo->persona);
    }
    aplicarFuncionRecursivo(nodo->siguiente, funcion);
}

void BancoManejoPersona::forEachPersona(const std::function<void(Persona*)>& funcion) {
    aplicarFuncionRecursivo(listaPersonas, funcion);
}

void BancoManejoPersona::forEachNodoPersona(const std::function<void(NodoPersona*)>& funcion) {
    std::function<void(NodoPersona*)> aplicarRecursivo = [&](NodoPersona* nodo) {
        if (!nodo) return;
        funcion(nodo);
        aplicarRecursivo(nodo->siguiente);
        };

    aplicarRecursivo(listaPersonas);
}