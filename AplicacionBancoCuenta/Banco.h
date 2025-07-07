#pragma once
#ifndef BANCO_H
#define BANCO_H

#include "Persona.h"
#include "NodoPersona.h"
#include <Shlobj.h>  // Para SHGetKnownFolderPath
#include <comdef.h>  // Para _bstr_t
#include <KnownFolders.h> // Para FOLDERID_Desktop
#pragma comment(lib, "shell32.lib")  // Enlazar con shell32.lib

/**
 * @class Banco
 * @brief Clase que representa un banco con sus operaciones principales
 *
 * Esta clase gestiona la lista de personas (clientes) del banco y proporciona
 * funcionalidades para crear, buscar y administrar cuentas bancarias. También
 * permite realizar operaciones como transferencias y manejo de archivos para
 * persistencia de datos.
 */
class Banco {
private:
    /** @brief Lista enlazada de personas (clientes) del banco */
    NodoPersona* listaPersonas;

public:
    /**
     * @brief Constructor por defecto
     *
     * Inicializa un banco sin clientes (listaPersonas = nullptr)
     */
    Banco();

    /**
     * @brief Destructor
     *
     * Libera la memoria de todos los nodos de la lista de personas
     */
    ~Banco();

    /**
     * @brief Obtiene la lista de personas del banco
     * @return Puntero al primer nodo de la lista de personas
     */
    NodoPersona* getListaPersonas() const { return listaPersonas; }

    /**
     * @brief Establece una nueva lista de personas
     * @param nuevaLista Puntero al primer nodo de la nueva lista
     */
    void setListaPersonas(NodoPersona* nuevaLista) { listaPersonas = nuevaLista; }

    /**
     * @brief Agrega una nueva persona con cuenta al banco
     *
     * Solicita los datos de la persona y crea una cuenta asociada a ella
     */
    void agregarPersonaConCuenta();

    /**
     * @brief Guarda las cuentas en un archivo con nombre específico
     * @param nombreArchivo Nombre del archivo donde se guardarán los datos
     */
    void guardarCuentasEnArchivo(const std::string& nombreArchivo) const;

    /**
     * @brief Guarda las cuentas en un archivo con nombre predeterminado
     *
     * Utiliza la ruta del escritorio y un nombre por defecto
     */
    void guardarCuentasEnArchivo();

    /**
     * @brief Carga las cuentas desde un archivo existente
     * @param nombreArchivo Nombre del archivo desde donde se cargarán los datos
     */
    void cargarCuentasDesdeArchivo(const std::string& nombreArchivo);

    /**
     * @brief Busca una cuenta en el sistema
     *
     * Permite buscar cuentas por diferentes criterios como número o titular
     */
    void buscarCuenta();

    /**
     * @brief Busca cuentas por fecha de apertura
     * @param fecha Fecha de apertura en formato DD/MM/AAAA
     */
    void buscarCuentasPorFecha(const std::string& fecha) const;

    /**
     * @brief Busca cuentas por diversos criterios
     *
     * Interfaz para buscar cuentas por número, saldo, titular, etc.
     */
    void buscarCuentasPorCriterio();

    /**
     * @brief Realiza transferencias entre cuentas
     *
     * Permite mover fondos entre distintas cuentas bancarias
     */
    void realizarTransferencia();

    /**
     * @brief Formatea un valor monetario con separadores de miles
     * @param valorEnCentavos Valor monetario en centavos
     * @return String formateado con el valor monetario y separadores
     */
    std::string formatearConComas(double valorEnCentavos) const;

    /**
     * @brief Obtiene la ruta del escritorio del usuario actual
     * @return String con la ruta completa al escritorio
     *
     * Utiliza funciones del API de Windows para obtener la ruta
     */
    std::string obtenerRutaEscritorio() const;
};

#endif // BANCO_H