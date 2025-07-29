#pragma once
#ifndef BANCO_H
#define BANCO_H

#define NOMINMAX
#include "Persona.h"
#include "NodoPersona.h"
#include <Shlobj.h>  // Para SHGetKnownFolderPath
#include <comdef.h>  // Para _bstr_t
#include <KnownFolders.h> // Para FOLDERID_Desktop
#include "_BaseDatosPersona.h"
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
	 * @brief Solicita la cédula de una persona
	 * @return La cédula ingresada por el usuario
	 */
	std::string solicitarCedula();

	/** @brief Solicita los datos de una persona
	 * @param cedula Cédula de la persona (opcional)
	 * @return Puntero a la nueva persona creada
	 */
	Persona* buscarPersonaPorCedula(const std::string& cedula);

	/**
	* @brief Confirma si se debe agregar una cuenta existente a una persona
	* @param personaExistente Puntero a la persona existente
	* @param cedula Cédula de la persona a agregar
	* @return true si se confirma la adición, false en caso contrario
	*/
	bool confirmarAgregarCuentaExistente(Persona* personaExistente, const std::string& cedula);

	/**
	 * @brief Crea una cuenta de ahorros para una persona
	 * @param persona Puntero a la persona a la que se le creará la cuenta
	 * @param cedula Cédula de la persona
	 */
	void crearCuentaAhorrosParaPersona(Persona* persona, const std::string& cedula);

	/**
	 * @brief Crea una cuenta corriente para una persona
	 * @param persona Puntero a la persona a la que se le creará la cuenta
	 * @param cedula Cédula de la persona
	 */
	void crearCuentaCorrienteParaPersona(Persona* persona, const std::string& cedula);

	/**
	 * @brief Guarda una cuenta de ahorros en la base de datos MongoDB
	 * @param cedula Cédula del titular de la cuenta
	 * @param cuenta Puntero a la cuenta de ahorros a guardar
	 */
	void guardarCuentaEnMongoDB(const std::string& cedula, const CuentaAhorros* cuenta);

	/**
	 * @brief Guarda una cuenta corriente en la base de datos MongoDB
	 * @param cedula Cédula del titular de la cuenta
	 * @param cuenta Puntero a la cuenta corriente a guardar
	 */
	void guardarCuentaEnMongoDB(const std::string& cedula, const CuentaCorriente* cuenta);

	/**
	 * @brief Obtiene o prepara una persona a partir de su cédula
	 * @param cedula Cédula de la persona a buscar o crear
	 * @param dbPersona Referencia a la base de datos de personas
	 * @return Puntero a la persona encontrada o creada
	 */
	Persona* obtenerOIngresarPersona(const std::string& cedula, _BaseDatosPersona& dbPersona);

	/**
	 * @brief Agrega una cuenta a una persona existente
	 * @param persona Puntero a la persona a la que se le agregará la cuenta
	 * @param tipoCuenta Tipo de cuenta a agregar (0 para ahorros, 1 para corriente)
	 * @param cedula Cédula de la persona
	 * @param dbPersona Referencia a la base de datos de personas
	 */
	void agregarCuentaAPersona(Persona* persona, int tipoCuenta, const std::string& cedula, _BaseDatosPersona& dbPersona);

	/**
	 * @brief Crea un documento BSON para una cuenta bancaria
	 * @param persona Puntero a la persona asociada a la cuenta
	 * @param tipoCuenta Tipo de cuenta (0 para ahorros, 1 para corriente)
	 * @param cedula Cédula del titular de la cuenta
	 * @return Documento BSON con los datos de la cuenta
	 */
	bsoncxx::document::value crearDocumentoCuenta(Persona* persona, int tipoCuenta, const std::string& cedula);

	/**
	 * @brief Inserta una persona con su cuenta en la base de datos MongoDB
	 * @param persona Puntero a la persona a insertar
	 * @param cuentaDoc Documento BSON de la cuenta asociada
	 * @param dbPersona Referencia a la base de datos de personas
	 */
	void insertarPersonaConCuenta(const Persona& persona, const bsoncxx::document::value& cuentaDoc, _BaseDatosPersona& dbPersona);

	/**
	 * @brief Agrega una cuenta a una persona ya existente en la base de datos
	 * @param cedula Cédula de la persona a la que se le agregará la cuenta
	 * @param cuentaDoc Documento BSON de la cuenta a agregar
	 * @param dbPersona Referencia a la base de datos de personas
	 */
	void agregarCuentaPersonaExistente(const std::string& cedula, const bsoncxx::document::value& cuentaDoc, _BaseDatosPersona& dbPersona);

	/**
	 * @brief Agrega una persona a la lista en memoria
	 * @param persona Puntero a la persona a agregar
	 *
	 * Esta función se encarga de crear un nuevo nodo y vincularlo a la lista de personas.
	 */
	void agregarPersonaEnMemoria(Persona* persona);

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
	 * @brief Agrega una cuenta a una persona existente
	 *
	 * Permite agregar una cuenta de ahorros o corriente a una persona ya registrada
	 */
	int mostrarMenuTipoCuenta();

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
	bool verificarCuentasBanco() const;

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