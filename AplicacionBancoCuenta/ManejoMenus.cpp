// @file ManejoMenus.cpp
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Utilidades.h"
#include "ManejoMenus.h"
#include "Persona.h"
#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include "Validar.h"

/**
 * @brief Muestra un menú interactivo para seleccionar el tipo de cuenta a crear
 * @return Entero que representa la opción seleccionada (0 para Ahorros, 1 para Corriente, 2 para Cancelar)
 */
int ManejoMenus::mostrarMenuTipoCuenta() {
	std::vector<std::string> opciones = { "Cuenta de Ahorros", "Cuenta Corriente", "Cancelar" };
	// Puedes ajustar las coordenadas x, y según tu preferencia visual
	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	Utilidades::ocultarCursor();
	int seleccion = Utilidades::menuInteractivo("Seleccione el tipo de cuenta a crear para la persona:", opciones, 0, 0);
	return seleccion;
}

/**
 * @brief Solicita al usuario que ingrese una cédula
 * @return Cédula ingresada como string, o string vacío si se cancela
 */
std::string ManejoMenus::solicitarCedula() {
	std::string cedula;

	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "=== SOLICITAR CEDULA ===" << std::endl;
	std::cout << "Ingrese el numero de cedula: ";

	std::getline(std::cin, cedula);

	// Validar que no esté vacía
	if (cedula.empty()) {
		std::cout << "Cedula no puede estar vacia." << std::endl;
		return "";
	}

	// Validar formato de cédula usando la clase Validar
	if (!Validar::ValidarCedula(cedula)) {
		std::cout << "Formato de cedula invalido." << std::endl;
		return "";
	}

	return cedula;
}

/**
 * @brief Solicita al usuario que ingrese un número de cuenta
 * @param tipo Tipo de cuenta para mostrar en el mensaje (opcional)
 * @return Número de cuenta ingresado como string, o string vacío si se cancela
 */
std::string ManejoMenus::solicitarNumeroCuenta(const std::string& tipo) {
	std::string numeroCuenta;

	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "=== SOLICITAR NUMERO DE CUENTA ===" << std::endl;

	if (!tipo.empty()) {
		std::cout << "Ingrese el numero de cuenta " << tipo << ": ";
	}
	else {
		std::cout << "Ingrese el numero de cuenta: ";
	}

	std::getline(std::cin, numeroCuenta);

	// Validar que no esté vacío
	if (numeroCuenta.empty()) {
		std::cout << "Numero de cuenta no puede estar vacio." << std::endl;
		return "";
	}

	// Validar formato básico del número de cuenta
	if (numeroCuenta.length() < 5) {
		std::cout << "Numero de cuenta debe tener al menos 5 caracteres." << std::endl;
		return "";
	}

	return numeroCuenta;
}

/**
 * @brief Solicita al usuario que ingrese un monto
 * @return Monto ingresado como double, o 0.0 si se cancela o es inválido
 */
double ManejoMenus::solicitarMonto() {
	std::string entrada;
	double monto = 0.0;

	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "=== SOLICITAR MONTO ===" << std::endl;
	std::cout << "Ingrese el monto: $";

	std::getline(std::cin, entrada);

	// Validar que no esté vacío
	if (entrada.empty()) {
		std::cout << "Monto no puede estar vacio." << std::endl;
		return 0.0;
	}

	try {
		monto = std::stod(entrada);

		// Validar que sea positivo
		if (monto <= 0) {
			std::cout << "El monto debe ser mayor que cero." << std::endl;
			return 0.0;
		}

		// Validar que no tenga más de 2 decimales
		if (std::floor(monto * 100) != monto * 100) {
			std::cout << "El monto no puede tener mas de 2 decimales." << std::endl;
			return 0.0;
		}

	}
	catch (const std::exception&) {
		std::cout << "Monto invalido. Ingrese un numero valido." << std::endl;
		return 0.0;
	}

	return monto;
}

/**
 * @brief Permite al usuario seleccionar una fecha
 * @return Fecha seleccionada como string en formato DD/MM/YYYY, o string vacío si se cancela
 */
std::string ManejoMenus::seleccionarFecha() {
	std::string fecha;

	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "=== SELECCIONAR FECHA ===" << std::endl;
	std::cout << "Ingrese la fecha (DD/MM/YYYY): ";

	std::getline(std::cin, fecha);

	// Validar que no esté vacía
	if (fecha.empty()) {
		std::cout << "Fecha no puede estar vacia." << std::endl;
		return "";
	}

	// Validar formato básico de fecha
	if (!Validar::ValidarFecha(fecha)) {
		std::cout << "Formato de fecha invalido. Use DD/MM/YYYY." << std::endl;
		return "";
	}

	return fecha;
}

/**
 * @brief Solicita confirmación del usuario para una operación
 * @param mensaje Mensaje a mostrar al usuario
 * @return true si el usuario confirma, false en caso contrario
 */
bool ManejoMenus::confirmarOperacion(const std::string& mensaje) {
	std::string respuesta;

	std::cout << std::endl << mensaje << std::endl;
	std::cout << "¿Confirma esta operacion? (S/N): ";

	std::getline(std::cin, respuesta);

	// Convertir a minúsculas para comparación
	std::transform(respuesta.begin(), respuesta.end(), respuesta.begin(), ::tolower);

	return (respuesta == "s" || respuesta == "si" || respuesta == "y" || respuesta == "yes");
}

/**
 * @brief Solicita confirmación para agregar cuenta a persona existente
 * @param cedula Cédula de la persona existente
 * @param nombre Nombre completo de la persona
 * @return true si el usuario confirma, false en caso contrario
 */
bool ManejoMenus::confirmarAgregarCuentaExistente(const std::string& cedula, const std::string& nombre) {
	std::cout << std::endl << "=== PERSONA EXISTENTE ENCONTRADA ===" << std::endl;
	std::cout << "Cedula: " << cedula << std::endl;
	std::cout << "Nombre: " << nombre << std::endl;
	std::cout << std::endl;

	return confirmarOperacion("¿Desea agregar una nueva cuenta a esta persona existente?");
}

/**
 * @brief Muestra un mensaje de error con formato consistente
 * @param mensaje Mensaje de error a mostrar
 */
void ManejoMenus::mostrarMensajeError(const std::string& mensaje) {
	std::cout << std::endl << "*** ERROR ***" << std::endl;
	std::cout << mensaje << std::endl;
	std::cout << "************" << std::endl << std::endl;
}

/**
 * @brief Muestra un mensaje de éxito con formato consistente
 * @param mensaje Mensaje de éxito a mostrar
 */
void ManejoMenus::mostrarMensajeExito(const std::string& mensaje) {
	std::cout << std::endl << "*** EXITO ***" << std::endl;
	std::cout << mensaje << std::endl;
	std::cout << "*************" << std::endl << std::endl;
}

/**
 * @brief Muestra un menú para seleccionar tipo de búsqueda
 * @return Entero que representa la opción seleccionada
 */
int ManejoMenus::mostrarMenuBusqueda() {
	std::vector<std::string> opciones = {
		"Por fecha de creacion",
		"Por criterio de usuario",
		"Por numero de cuenta",
		"Por cedula",
		"Cancelar"
	};

	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	Utilidades::ocultarCursor();
	int seleccion = Utilidades::menuInteractivo("Seleccione el tipo de busqueda:", opciones, 0, 0);
	return seleccion;
}

/**
 * @brief Muestra los datos de una persona en los resultados de búsqueda
 * @param persona Puntero a la persona a mostrar
 */
void ManejoMenus::mostrarResultadoBusquedaPersona(Persona* persona) {
	if (persona) {
		std::cout << "Persona: " << persona->getNombres() << " " << persona->getApellidos() << std::endl;
	}
}

/**
 * @brief Muestra la información de una cuenta en los resultados de búsqueda
 * @param cuenta Puntero void a la cuenta (CuentaAhorros o CuentaCorriente)
 */
void ManejoMenus::mostrarResultadoBusquedaCuenta(void* cuenta) {
	if (!cuenta) {
		return;
	}

	// Intentar casting a CuentaAhorros
	auto* cuentaAhorros = static_cast<CuentaAhorros*>(cuenta);
	if (cuentaAhorros) {
		// Verificar si realmente es una CuentaAhorros mediante un método específico
		try {
			std::cout << "Cuenta de Ahorros: " << cuentaAhorros->getNumeroCuenta()
				<< ", Saldo: $" << std::fixed << std::setprecision(2) << cuentaAhorros->getSaldo() << std::endl;
			return;
		}
		catch (...) {
			// Si falla, intentar con CuentaCorriente
		}
	}

	// Intentar casting a CuentaCorriente
	auto* cuentaCorriente = static_cast<CuentaCorriente*>(cuenta);
	if (cuentaCorriente) {
		try {
			std::cout << "Cuenta Corriente: " << cuentaCorriente->getNumeroCuenta()
				<< ", Saldo: $" << std::fixed << std::setprecision(2) << cuentaCorriente->getSaldo() << std::endl;
		}
		catch (...) {
			std::cout << "Error al mostrar información de la cuenta" << std::endl;
		}
	}
}

/**
 * @brief Muestra los resultados de búsqueda aplicando SRP
 * @param resultados Vector de pares con persona y cuenta
 */
void ManejoMenus::mostrarResultadosBusqueda(const std::vector<std::pair<Persona*, void*>>& resultados) {
	if (resultados.empty()) {
		mostrarMensajeError("No se encontraron resultados.");
		return;
	}

	std::cout << std::endl << "=== RESULTADOS DE BUSQUEDA ===" << std::endl;
	std::cout << "Se encontraron " << resultados.size() << " resultado(s):" << std::endl << std::endl;

	for (size_t i = 0; i < resultados.size(); ++i) {
		std::cout << "Resultado " << (i + 1) << ":" << std::endl;
		mostrarResultadoBusquedaPersona(resultados[i].first);
		mostrarResultadoBusquedaCuenta(resultados[i].second);
		std::cout << "------------------------" << std::endl;
	}
}