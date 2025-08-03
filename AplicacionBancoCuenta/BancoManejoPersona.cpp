#include "BancoManejoPersona.h"
#include "ConexionMongo.h"
#include <algorithm>
#include <conio.h>

BancoManejoPersona::BancoManejoPersona() : listaPersonas(nullptr), personaActual(nullptr) {}

BancoManejoPersona::~BancoManejoPersona() {
    eliminarPersonasRecursivo(listaPersonas);
}

// Implementación recursiva para eliminar toda la lista
void BancoManejoPersona::eliminarPersonasRecursivo(NodoPersona* nodo) {
    if (!nodo) return;

    eliminarPersonasRecursivo(nodo->siguiente);
    delete nodo;
}

// Búsqueda recursiva más eficiente integrada con base de datos
NodoPersona* BancoManejoPersona::buscarPersonaRecursivo(NodoPersona* nodo, const std::string& cedula) {
    if (!nodo) {
        // Si no se encontró en la lista enlazada, buscar en la base de datos
        try {
            std::cout << " Cargando... Por favor espere." << std::endl;

            mongocxx::client& client = ConexionMongo::obtenerClienteBaseDatos();
            _BaseDatosPersona dbPersona(client);

            // Buscar en la base de datos MongoDB
            Persona* personaBD = dbPersona.obtenerPersonaPorCedula(cedula);

            if (personaBD) {
                // Si se encontró en la base de datos, agregarla a la lista enlazada
                agregarPersona(personaBD);

                // Retornar el nodo recién agregado (que será la cabeza de la lista)
                return listaPersonas;
            }
            std::cout << "Presione cualquier tecla para continuar..." << std::endl;
            int teclaCualquiera = _getch();
			(void)teclaCualquiera; 

			Utilidades::limpiarPantallaPreservandoMarquesina(0);
        }
        catch (const std::exception& e) {
            std::cerr << "Error al buscar en base de datos: " << e.what() << std::endl;
            system("pause");
            Utilidades::limpiarPantallaPreservandoMarquesina(0);
        }

        return nullptr; // No se encontró ni en memoria ni en base de datos
    }

    if (nodo->persona && nodo->persona->getCedula() == cedula) {
        return nodo;
    }

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