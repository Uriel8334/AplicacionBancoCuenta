#pragma once
#ifndef _EXPORTADORARCHIVO_H
#define _EXPORTADORARCHIVO_H

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <fstream>
#include <map>
#include <functional>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/document/element.hpp>
#include "_BaseDatosPersona.h"
#include <memory>

// Forward declarations
class Banco;
class Persona;
class _BaseDatosPersona;

/**
 * @brief Interfaz Strategy para diferentes tipos de guardado
 * Aplicando Open/Closed Principle
 */
class IEstrategiaGuardado {
public:
	virtual ~IEstrategiaGuardado() = default;
	virtual bool ejecutar(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo) = 0;
	virtual std::string obtenerExtension() const = 0;
	virtual std::string obtenerDescripcion() const = 0;
};

/**
 * @brief Factory para crear estrategias de guardado
 * Aplicando Factory Pattern y Dependency Inversion Principle
 */
class FabricaEstrategiasGuardado {
public:
	static std::unique_ptr<IEstrategiaGuardado> crear(int tipoGuardado, char claveCifrado = '\0');
	static std::vector<std::string> obtenerOpcionesDisponibles();
};

/**
 * @brief Gestor principal de guardado de archivos
 * Aplicando Single Responsibility Principle
 */
class GestorGuardadoArchivos {
private:
	const _BaseDatosPersona& baseDatos;
	std::unique_ptr<IEstrategiaGuardado> estrategia;

public:
	explicit GestorGuardadoArchivos(const _BaseDatosPersona& bd);

	bool configurarEstrategia(int tipoGuardado, char claveCifrado = '\0');
	bool ejecutarGuardado(const std::string& nombreArchivo);
	std::vector<std::string> obtenerOpcionesGuardado() const;
	bool validarDatosDisponibles() const;
};

/**
 * @brief Estructura para mantener el estado durante el procesamiento de PDF
 */
struct EstadoProcesamiento {
	bool enPersona = false;
	bool enCuentasAhorro = false;
	bool enCuentasCorriente = false;
	bool enCuentaAhorro = false;
	bool enCuentaCorriente = false;
	int contadorPersonas = 0;
	int totalCuentasAhorro = 0;
	int totalCuentasCorriente = 0;
};

/**
 * @class ExportadorArchivo
 * @brief Clase responsable de exportar e importar datos del banco
 *
 * Esta clase maneja la exportación e importación de datos del banco
 * hacia diferentes formatos de archivo (.bak, .bin, .pdf, .txt).
 * Implementa principios SOLID y programación funcional.
 */
class ExportadorArchivo {
private:
	// === MÉTODOS AUXILIARES PARA MONGODB ===
	static bool guardarDesdeBaseDatos(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo, const std::string& extension);
	static void procesarPersonaRecursivamente(const bsoncxx::document::value& personaDoc, std::ofstream& archivo);
	static void escribirCampoPersona(std::ofstream& archivo, const std::string& nombreCampo, const bsoncxx::document::element& elemento);
	static bool guardarArchivoConCifrado(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo, char claveCifrado);
	static bool generarPDFDesdeBaseDatos(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo);
	static bool archivoGuardadoHaciaPDFConQR(const std::string& nombreArchivo, const _BaseDatosPersona& baseDatos);
	static void procesarLineasPDFRecursivamente(std::ifstream& archivo, std::ofstream& archivoHtml,
		std::map<std::string, std::string>& datosPersona, EstadoProcesamiento& estado);
	static void escribirPersonaConQR(std::ofstream& archivoHtml, const std::map<std::string, std::string>& datosPersona);
	static void procesarCuentasBSON(const bsoncxx::array::view& cuentasArray, std::ofstream& archivo);
	static void procesarLineaEspecifica(const std::string& linea, std::ofstream& archivoHtml,
		std::map<std::string, std::string>& datosPersona, EstadoProcesamiento& estado);
	static void escribirCabeceraHTML(std::ofstream& archivoHtml);
	static void finalizarHTML(std::ofstream& archivoHtml);
	static bool convertirHTMLaPDF(const std::string& rutaHtml, const std::string& rutaPdf);
	static void procesarPersonaTradicional(Banco& banco, std::ifstream& archivo, std::string& linea);

public:
	class EstrategiaRespaldoBD;
	class EstrategiaCifrado;
	class EstrategiaPDFConQR;
	class EstrategiaRecuperacionBAK;
	class EstrategiaRecuperacionBIN;

	// === MÉTODOS DE RECUPERACIÓN ===
	static bool procesarSolicitudRecuperacion(const _BaseDatosPersona& baseDatos);
	static bool recuperarDesdeRespaldo(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo);
	static bool recuperarDesdeCifrado(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo, char claveDescifrado);
	static bool procesarArchivoRecuperacion(std::ifstream& archivo, _BaseDatosPersona& baseDatos);
	static bool cargarPersonaEnMongoDB(const std::map<std::string, std::string>& datosPersona,
		const std::vector<std::map<std::string, std::string>>& cuentasAhorro,
		const std::vector<std::map<std::string, std::string>>& cuentasCorriente,
		_BaseDatosPersona& baseDatos);
	static bool agregarCuentaDesdeBackup(const std::string& cedula,
		const std::map<std::string, std::string>& datosCuenta,
		const std::string& tipoCuenta,
		_BaseDatosPersona& baseDatos);

	// === MÉTODOS DE INTERFAZ DE USUARIO PARA RECUPERACIÓN ===
	static int solicitarTipoRecuperacion();
	static std::string solicitarNombreArchivoRecuperacion();
	static char solicitarClaveDescifrado();
	static void mostrarResultadoRecuperacion(bool exito, const std::string& tipoOperacion);

	// === MÉTODOS DE INTERFAZ DE USUARIO ===
	static int solicitarTipoGuardado();
	static std::string solicitarNombreArchivo();
	static char solicitarClaveParaCifrado();
	static void mostrarResultado(bool exito, const std::string& tipoOperacion);
	static bool validarDatosEnBaseDatos(const _BaseDatosPersona& baseDatos);

	// === MÉTODOS PRINCIPALES REFACTORIZADOS ===
	/**
	 * @brief Método principal simplificado que delega a GestorGuardadoArchivos
	 */
	static bool procesarSolicitudGuardado(const _BaseDatosPersona& baseDatos);

	// === MÉTODOS EXISTENTES ===
	static void guardarCuentasEnArchivo(const Banco& banco, const std::string& nombreArchivo);
	static void guardarCuentasEnArchivo(const Banco& banco);
	static void guardarPersonaEnArchivo(std::ofstream& archivo, Persona* p);
	static void cargarCuentasDesdeArchivo(Banco& banco, const std::string& nombreArchivo);
	static void limpiarBanco(Banco& banco);
	static void procesarPersona(Banco& banco, std::ifstream& archivo, std::string& linea);
	static bool archivoGuardadoHaciaPDF(const std::string& nombreArchivo);
	static std::string obtenerRutaEscritorio();

	// === NUEVOS MÉTODOS PARA MONGODB ===
	static bool guardarArchivosVarios(const _BaseDatosPersona& baseDatos, int tipoArchivo,
		const std::string& nombreArchivo, char claveCifrado = '\0');
	static bool procesarPersonaDesdeBSON(const bsoncxx::document::value& personaDoc, std::ofstream& archivo);
	static std::string generarQRPersona(const std::string& cedula, const std::string& nombres,
		const std::string& apellidos, const std::string& numeroCuenta);
};

#endif // _EXPORTADORARCHIVO_H