#include "PersonaDataProcessor.h"
#include "Persona.h"
#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include <iostream>
#include <algorithm>

// === Implementaciones de procesadores ===

void BuscadorCuentasProcessor::procesar(Persona* persona, const std::string& numeroCuenta) const {
    if (!persona || numeroCuenta.empty()) return;

    int encontrados = persona->buscarPersonaPorCuentas(numeroCuenta);
    if (encontrados == 0) {
        std::cout << "No se encontraron cuentas con el numero: " << numeroCuenta << std::endl;
    }
}

void MostradorCuentasProcessor::procesar(Persona* persona, const std::string& tipoCuenta) const {
    if (!persona) return;

    int cuentasEncontradas = persona->mostrarCuentas(tipoCuenta);
    std::cout << "Total de cuentas " << tipoCuenta << " encontradas: " << cuentasEncontradas << std::endl;
}

// === Implementación del procesador principal ===

PersonaDataProcessor::PersonaDataProcessor() {
    procesadores.push_back(std::make_unique<BuscadorCuentasProcessor>());
    procesadores.push_back(std::make_unique<MostradorCuentasProcessor>());
}

void PersonaDataProcessor::ejecutarProcesamiento(const std::string& tipo, Persona* persona, const std::string& parametro) const {
    if (tipo == "buscar" && !procesadores.empty()) {
        procesadores[0]->procesar(persona, parametro);
    }
    else if (tipo == "mostrar" && procesadores.size() > 1) {
        procesadores[1]->procesar(persona, parametro);
    }
}

// === Métodos recursivos ===

template<typename T>
void PersonaDataProcessor::procesarListaRecursivo(T* nodo, const std::function<void(T*)>& accion) const {
    if (!nodo) return;

    accion(nodo);
    procesarListaRecursivo(nodo->getSiguiente(), accion);
}

template<typename T>
int PersonaDataProcessor::contarElementosRecursivo(T* nodo) const {
    if (!nodo) return 0;
    return 1 + contarElementosRecursivo(nodo->getSiguiente());
}

void PersonaDataProcessor::procesarCuentasAhorros(Persona* persona, const std::function<void(CuentaAhorros*)>& accion) const {
    if (persona && persona->getCabezaAhorros()) {
        procesarListaRecursivo(persona->getCabezaAhorros(), accion);
    }
}

void PersonaDataProcessor::procesarCuentasCorriente(Persona* persona, const std::function<void(CuentaCorriente*)>& accion) const {
    if (persona && persona->getCabezaCorriente()) {
        procesarListaRecursivo(persona->getCabezaCorriente(), accion);
    }
}

int PersonaDataProcessor::contarCuentasAhorros(Persona* persona) const {
    return persona ? contarElementosRecursivo(persona->getCabezaAhorros()) : 0;
}

int PersonaDataProcessor::contarCuentasCorriente(Persona* persona) const {
    return persona ? contarElementosRecursivo(persona->getCabezaCorriente()) : 0;
}

// Instanciaciones explícitas de templates
template void PersonaDataProcessor::procesarListaRecursivo<CuentaAhorros>(CuentaAhorros*, const std::function<void(CuentaAhorros*)>&) const;
template void PersonaDataProcessor::procesarListaRecursivo<CuentaCorriente>(CuentaCorriente*, const std::function<void(CuentaCorriente*)>&) const;
template int PersonaDataProcessor::contarElementosRecursivo<CuentaAhorros>(CuentaAhorros*) const;
template int PersonaDataProcessor::contarElementosRecursivo<CuentaCorriente>(CuentaCorriente*) const;