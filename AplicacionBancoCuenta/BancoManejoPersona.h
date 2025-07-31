#pragma once
#ifndef BANCOMANEJOPERSONA_H
#define BANCOMANEJOPERSONA_H

#include "Persona.h"
#include "NodoPersona.h"
#include "_BaseDatosPersona.h"
#include <functional>
#include <string>

/**
 * @class BancoManejoPersona
 * @brief Responsable únicamente del manejo de personas en el banco
 *
 * Aplicando SRP: Una sola responsabilidad - gestionar la lista de personas
 */
class BancoManejoPersona {
private:
    NodoPersona* listaPersonas;

    // Métodos recursivos para operaciones en lista
    void eliminarPersonasRecursivo(NodoPersona* nodo);
    NodoPersona* buscarPersonaRecursivo(NodoPersona* nodo, const std::string& cedula);
    void aplicarFuncionRecursivo(NodoPersona* nodo, const std::function<void(Persona*)>& funcion);

public:
    BancoManejoPersona();
    ~BancoManejoPersona();

    // Operaciones CRUD de personas
    void agregarPersona(Persona* persona);
    Persona* buscarPersonaPorCedula(const std::string& cedula);
    bool existePersona(const std::string& cedula);

    // Iteradores funcionales
    void forEachPersona(const std::function<void(Persona*)>& funcion);
    void forEachNodoPersona(const std::function<void(NodoPersona*)>& funcion);

    // Getters/Setters
    NodoPersona* getListaPersonas() const { return listaPersonas; }
    void setListaPersonas(NodoPersona* nuevaLista) { listaPersonas = nuevaLista; }
    bool tienePersonas() const { return listaPersonas != nullptr; }
};

#endif // BANCOMANEJOPERSONA_H