#pragma once
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <conio.h>
#include <memory>
#include "Validar.h"

/**
 * @enum TipoInput
 * @brief Tipos de entrada soportados por el sistema
 */
enum class TipoInput {
	NUMERICO_SOLO,           // Solo números (0-9)
	CEDULA,                  // Números con validación de cédula (10 dígitos)
	ALFABETICO_SOLO,         // Solo letras (a-z, A-Z)
	ALFABETICO_ESPACIOS,     // Letras + espacios
	ALFANUMERICO,            // Letras + números
	ALFANUMERICO_ESPACIOS,   // Letras + números + espacios
	DECIMAL,                 // Números + punto decimal
	CORREO_LOCAL,            // Parte local del correo (antes del @)
	CORREO_COMPLETO,         // Correo electrónico completo con validación
	DIRECCION,               // Caracteres especiales para direcciones
	NUMERO_CUENTA,           // Validación específica para números de cuenta
	TELEFONO,                // Números + caracteres telefónicos
	FECHA_COMPONENT,         // Componentes de fecha (día, mes, año)
	FECHA_NACIMIENTO,        // Fecha de nacimiento con validación
	MONTO,                   // Montos con formato decimal
	TEXTO_LIBRE              // Cualquier carácter imprimible
};

/**
 * @struct ConfiguracionInput
 * @brief Configuración para un tipo específico de entrada
 */
struct ConfiguracionInput {
	int longitudMaxima = 0;
	int longitudMinima = 0;
	bool permitirVacio = false;
	std::string prompt;
	std::string mensajeError;
	std::function<bool(char)> validadorCaracter;
	std::function<bool(const std::string&)> validadorCompleto;
	std::function<void(std::string&)> formateador;
};

/**
 * @class InputManager
 * @brief Gestor centralizado para entradas en tiempo real por consola
 *
 * Proporciona una interfaz unificada para manejar diferentes tipos de entrada
 * con validación en tiempo real, aplicando principios SOLID.
 */
class InputManager {
private:
	std::unordered_map<TipoInput, ConfiguracionInput> configuraciones;

	// Métodos privados de configuración
	void inicializarConfiguraciones();
	//void configurarNumerico();
	void configurarCedula();
	//void configurarAlfabetico();
	void configurarAlfabetiroEspacios();
	//void configurarAlfanumerico();
	//void configurarDecimal();
	void configurarCorreoLocal();
	void configurarDireccion();
	//void configurarNumeroCuenta();
	void configurarMonto();
	//void configurarTextoLibre();

	// Métodos de procesamiento
	bool procesarTeclaEspecial(char tecla, std::string& entrada, int& posicionCursor);
	//void actualizarPantalla(const std::string& prompt, const std::string& entrada, int posicionCursor);
public: bool validarEntradaCompleta(const std::string& entrada, const ConfiguracionInput& config);

public:
	InputManager();
	~InputManager() = default;

	/**
	 * @brief Entrada principal para cualquier tipo de dato
	 * @param tipo Tipo de entrada requerida
	 * @param prompt Mensaje a mostrar al usuario
	 * @param valorInicial Valor inicial (opcional)
	 * @return String ingresado y validado
	 */
	std::string obtenerEntrada(TipoInput tipo, const std::string& prompt = "",
		const std::string& valorInicial = "");

	/**
	 * @brief Entrada con configuración personalizada
	 * @param config Configuración específica para esta entrada
	 * @param prompt Mensaje a mostrar al usuario
	 * @return String ingresado y validado
	 */
	//std::string obtenerEntradaPersonalizada(const ConfiguracionInput& config,
	//	const std::string& prompt);

	/**
	 * @brief Entrada de rango numérico con límites
	 * @param min Valor mínimo
	 * @param max Valor máximo
	 * @param prompt Mensaje a mostrar
	 * @return Valor numérico dentro del rango
	 */
	double obtenerMonto(double min, double max, const std::string& prompt);

	/**
	 * @brief Entrada multi-línea
	 * @param prompt Mensaje a mostrar
	 * @param maxLineas Máximo número de líneas permitidas
	 * @return Vector de strings, una por línea
	 */
	//std::vector<std::string> obtenerEntradaMultiLinea(const std::string& prompt, int maxLineas = 5);

	/**
	 * @brief Selector interactivo de opciones
	 * @param prompt Mensaje a mostrar
	 * @param opciones Vector de opciones disponibles
	 * @return Índice de la opción seleccionada
	 */
	//int obtenerSeleccion(const std::string& prompt, const std::vector<std::string>& opciones);

	/**
	 * @brief Entrada con auto-completado
	 * @param tipo Tipo de entrada
	 * @param prompt Mensaje a mostrar
	 * @param sugerencias Vector de sugerencias disponibles
	 * @return String ingresado
	 */
	//std::string obtenerEntradaConAutoCompletado(TipoInput tipo, const std::string& prompt,
	//	const std::vector<std::string>& sugerencias);

	// Métodos de utilidad
	//void registrarTipoPersonalizado(TipoInput tipo, const ConfiguracionInput& config);
/**
	 * @brief Obtiene la configuración para un tipo de entrada específico
	 * @param tipo Tipo de entrada del cual obtener la configuración
	 * @return Referencia a la configuración del tipo especificado
	 * @throws std::runtime_error si el tipo no está configurado
	 */
	ConfiguracionInput& obtenerConfiguracion(TipoInput tipo);

	/**
	 * @brief Obtiene la configuración para un tipo de entrada específico (versión const)
	 * @param tipo Tipo de entrada del cual obtener la configuración
	 * @return Referencia constante a la configuración del tipo especificado
	 * @throws std::runtime_error si el tipo no está configurado
	 */
	const ConfiguracionInput& obtenerConfiguracion(TipoInput tipo) const;
	//bool validarCaracter(char c, TipoInput tipo);

	void configurarCorreoCompleto();
	void configurarFechaNacimiento();



};

/**
 * @namespace InputUI
 * @brief Funciones lambda especializadas para casos de uso comunes
 */
namespace InputUI {
	extern std::unique_ptr<InputManager> manager;

	// Inicialización del manager
	void inicializar();

	// Funciones lambda especializadas
	extern const std::function<std::string(const std::string&)> ingresarCedula;
	extern const std::function<std::string(const std::string&)> ingresarNombres;
	extern const std::function<std::string(const std::string&)> ingresarApellidos;
	extern const std::function<std::string(const std::string&)> ingresarCorreoLocal;
	extern const std::function<std::string(const std::string&)> ingresarCorreoCompleto;
	extern const std::function<std::string(const std::string&)> ingresarFechaNacimiento;
	extern const std::function<std::string(const std::string&)> ingresarDireccion;
	extern const std::function<std::string(const std::string&)> ingresarNumeroCuenta;
	extern const std::function<double(double, double, const std::string&)> ingresarMonto;
	extern const std::function<bool(const std::string&)> seleccionarSiNo;
}

#endif // INPUT_MANAGER_H