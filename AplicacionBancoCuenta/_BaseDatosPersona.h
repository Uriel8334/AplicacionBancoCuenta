#pragma once  
#ifndef _BASEDATOSPERSONA_H  
#define _BASEDATOSPERSONA_H  
#define NOMINMAX
#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include "Persona.h"  
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

class _BaseDatosPersona {
public:
	_BaseDatosPersona(mongocxx::client& client);

	/**
	* @brief Inserta una nueva persona en la base de datos
	*
	* @param persona La persona a insertar
	* @return true si la inserción fue exitosa, false en caso contrario
	*/
	bool insertarPersona(const Persona& persona);

	/**
	* @brief Inserta una nueva persona en la base de datos
	*
	* @param persona La persona a insertar
	* @param cuentaInicial La cuenta inicial de la persona (opcional)
	* @return true si la inserción fue exitosa, false en caso contrario
	*/
	bool insertarPersona(const Persona& persona, const bsoncxx::document::value* cuentaInicial = nullptr);
	// Puedes agregar métodos como:  
	// bool actualizarPersona(const Persona& persona);  
	// Persona buscarPersonaPorCedula(const std::string& cedula);  
	// bool eliminarPersona(const std::string& cedula); 

	/**
	* @brief Prueba la conexión a una base de datos MongoDB
	*
	* @param uri_str La cadena de conexión URI de MongoDB
	* @return true si la conexión fue exitosa, false en caso contrario
	*/
	bool probarConexionMongoDB(const std::string& uri_str);

	/**
	* @brief Inserta una nueva persona en la base de datos MongoDB
	*
	* @param persona La persona a insertar
	* @return true si la inserción fue exitosa, false en caso contrario
	*/
	bool agregarCuentaPersona(const std::string& cedula, const bsoncxx::document::value& cuentaDoc);

	/**
	 * @brief Verifica si existe una persona con la cédula dada en la base de datos MongoDB
	 * @param cedula Cédula a buscar
	 * @return true si existe, false si no existe
	 */
	bool existePersonaPorCedula(const std::string& cedula);

	/**
	 * @brief Busca y retorna una persona por cédula desde MongoDB
	 * @param cedula Cédula a buscar
	 * @return Objeto Persona* si existe, nullptr si no existe
	 */
	Persona* obtenerPersonaPorCedula(const std::string& cedula);

private:
	mongocxx::client& _client;
};

#endif // _BASEDATOSPERSONA_H