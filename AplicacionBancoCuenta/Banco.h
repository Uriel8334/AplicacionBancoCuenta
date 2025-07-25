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

    /**
     * @brief Aplica una función a cada persona en la lista del banco (tipo foreach)
     * @param funcion Función lambda o std::function<void(Persona*)>
     */
    void forEachPersona(const std::function<void(Persona*)>& funcion) const;

    /**
     * @brief Aplica una función a cada nodo de persona en la lista del banco (tipo foreach)
     * @param funcion Función lambda o std::function<void(NodoPersona*)>
     */
    void forEachNodoPersona(const std::function<void(NodoPersona*)>& funcion) const;

    /**
     * @brief Guarda los datos de una persona en el archivo de respaldo
     * @param archivo Referencia al archivo de salida
     * @param p Puntero a la persona a guardar
     */
    void guardarPersonaEnArchivo(std::ofstream& archivo, Persona* p) const;

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
     * @brief Convierte un archivo de respaldo .bak a formato PDF
     *
     * @param nombreArchivo Nombre del archivo de respaldo (sin extensión)
     * @return bool true si la conversión fue exitosa, false en caso contrario
     */
    bool archivoGuardadoHaciaPDF(const std::string& nombreArchivo);

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

    /**
	 * @brief Verifica si existen cuentas bancarias registradas
     */
	bool verificarCuentasBanco() const ;

    /**
	 * @brief Submenú para gestionar cuentas bancarias
     */
    void subMenuCuentasBancarias();

    /**
     * @brief Busca una cuenta bancaria para realizar operaciones
     *
     * Permite al usuario buscar una cuenta por cédula o número de cuenta,
     * y devuelve la cuenta encontrada para realizar operaciones sobre ella.
     *
     * @param banco Referencia al objeto Banco donde buscar la cuenta
     * @param cuentaAhorros Referencia a puntero que se actualizará con la cuenta de ahorros encontrada
     * @param cuentaCorriente Referencia a puntero que se actualizará con la cuenta corriente encontrada
     * @param cedula Referencia a string que se actualizará con la cédula del titular
     * @return bool true si se encontró una cuenta válida, false en caso contrario
     */
    bool buscarCuentaParaOperacion(Banco& banco, CuentaAhorros*& cuentaAhorros, CuentaCorriente*& cuentaCorriente, std::string& cedula);


};

#endif // BANCO_H