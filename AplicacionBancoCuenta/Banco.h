#pragma once
#ifndef BANCO_H
#define BANCO_H

#include "Persona.h"
#include "NodoPersona.h"
#include <Shlobj.h>  // Para SHGetKnownFolderPath
#include <comdef.h>  // Para _bstr_t
#include <KnownFolders.h> // Para FOLDERID_Desktop
#pragma comment(lib, "shell32.lib")  // Enlazar con shell32.lib

class Banco {
private:
	NodoPersona* listaPersonas; // Lista enlazada de personas

public:
	Banco(); // Constructor por defecto
	~Banco(); // Destructor

	// Metodos para manipular la lista enlazada
	NodoPersona* getListaPersonas() const { return listaPersonas; }
	void setListaPersonas(NodoPersona* nuevaLista) { listaPersonas = nuevaLista; }

	void agregarPersonaConCuenta(); // Agrega una persona con cuenta
	void guardarCuentasEnArchivo(const std::string& nombreArchivo) const; // Guarda las cuentas en un archivo
	void guardarCuentasEnArchivo(); // Guarda las cuentas en un archivo con nombre por defecto
	void cargarCuentasDesdeArchivo(const std::string& nombreArchivo); // Carga las cuentas desde un archivo
	void buscarCuenta(); // Busca cuentas

	void buscarCuentasPorFecha(const std::string& fecha) const;
	void buscarCuentasPorCriterio(); // Busca cuentas por criterio
	void realizarTransferencia(); // Metodo para realizar transferencias entre cuentas
	std::string formatearConComas(int valorEnCentavos)const;

	// Obtiene la ruta del escritorio del usuario
	std::string obtenerRutaEscritorio() const;
};

#endif // BANCO_H