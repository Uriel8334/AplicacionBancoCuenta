#include "CreadorCuentas.h"
#include "Utilidades.h"
#include "Fecha.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <chrono>
#include <iostream>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

std::pair<bool, std::string> CreadorCuentas::crearCuentaAhorros(
	Persona* persona,
	const std::string& cedula,
	CuentaAhorros* cuenta) {

	if (!cuenta || !persona) {
		return { false, "Parámetros inválidos" };
	}

	try {
		// Configurar datos básicos
		configurarDatosBasicosCuenta(cuenta);

		// Solicitar monto inicial
		double montoInicial = solicitarMontoInicialAhorros();

		// Seleccionar sucursal y generar número de cuenta
		std::string sucursal = persona->seleccionSucursal();
		std::string numeroCuenta = persona->crearNumeroCuenta(cuenta, sucursal);

		if (numeroCuenta.empty()) {
			return { false, "Error generando número de cuenta" };
		}

		// Configurar cuenta completa
		finalizarConfiguracionCuenta(cuenta, numeroCuenta, montoInicial);

		// Crear documento para MongoDB
		auto cuentaDoc = crearDocumentoCuenta(numeroCuenta, "ahorros", montoInicial, cuenta->getFechaApertura(), sucursal);

		// Persistir en MongoDB
		if (!baseDatos.agregarCuentaPersona(cedula, cuentaDoc)) {
			return { false, "Error al guardar en base de datos" };
		}

		// Agregar a memoria
		agregarCuentaAMemoria(persona, cuenta);

		// Mostrar información
		cuenta->mostrarInformacion(cedula, true);

		std::cout << "---- DEBUG: Cuenta de Ahorros creada y guardada correctamente ----" << std::endl;
		std::cout << "Número de cuenta: " << numeroCuenta << std::endl;

		return { true, numeroCuenta };
	}
	catch (const std::exception& e) {
		return { false, std::string("Error: ") + e.what() };
	}
}

std::pair<bool, std::string> CreadorCuentas::crearCuentaCorriente(
	Persona* persona,
	const std::string& cedula,
	CuentaCorriente* cuenta) {

	if (!cuenta || !persona) {
		return { false, "Parámetros inválidos" };
	}

	try {
		// Configurar datos básicos
		configurarDatosBasicosCuenta(cuenta);

		// Solicitar monto inicial (obligatorio para corriente)
		double montoInicial = solicitarMontoInicialCorriente();

		// Seleccionar sucursal y generar número de cuenta
		std::string sucursal = persona->seleccionSucursal();
		std::string numeroCuenta = persona->crearNumeroCuenta(cuenta, sucursal);

		if (numeroCuenta.empty()) {
			return { false, "Error generando número de cuenta" };
		}

		// Configurar cuenta completa
		finalizarConfiguracionCuenta(cuenta, numeroCuenta, montoInicial);

		// Crear documento para MongoDB
		auto cuentaDoc = crearDocumentoCuenta(numeroCuenta, "corriente", montoInicial, cuenta->getFechaApertura(), sucursal);

		// Persistir en MongoDB
		if (!baseDatos.agregarCuentaPersona(cedula, cuentaDoc)) {
			return { false, "Error al guardar en base de datos" };
		}

		// Agregar a memoria
		agregarCuentaAMemoria(persona, cuenta);

		// Mostrar información
		cuenta->mostrarInformacion(cedula, true);

		std::cout << "---- DEBUG: Cuenta Corriente creada y guardada correctamente ----" << std::endl;
		std::cout << "Número de cuenta: " << numeroCuenta << std::endl;

		return { true, numeroCuenta };
	}
	catch (const std::exception& e) {
		return { false, std::string("Error: ") + e.what() };
	}
}

double CreadorCuentas::solicitarMontoInicialAhorros() {
	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "\nAhora puede configurar un monto inicial para su cuenta de ahorros.\n";

	bool deseaIngreso = PersonaUI::seleccionarSiNo(
		"¿Desea ingresar un monto inicial? (máximo 15000.00 USD)\n"
	);

	if (!deseaIngreso) {
		return 0.0;
	}

	double monto;
	do {
		monto = PersonaUI::ingresarMonto(0.0, 15000.00,
			"\nIngrese el monto inicial (máximo 15000.00 USD): ");
	} while (monto < 0 || monto > 15000.00);

	return monto;
}

double CreadorCuentas::solicitarMontoInicialCorriente() {
	double monto;
	do {
		monto = PersonaUI::ingresarMonto(250.00, 15000.00,
			"\nIngrese el monto inicial (mínimo 250.00 USD, máximo 15000.00 USD): ");
	} while (monto < 250.00);

	return monto;
}

bsoncxx::document::value CreadorCuentas::crearDocumentoCuenta(
	const std::string& numeroCuenta,
	const std::string& tipo,
	double saldo,
	const std::string& fecha,
	const std::string& sucursal) {

	return make_document(
		kvp("numeroCuenta", numeroCuenta),
		kvp("tipo", tipo),
		kvp("saldo", saldo),
		kvp("fechaApertura", fecha),
		kvp("estado", "ACTIVA"),
		kvp("sucursal", sucursal),
		kvp("fechaCreacion", bsoncxx::types::b_date{ std::chrono::system_clock::now() })
	);
}

void CreadorCuentas::configurarDatosBasicosCuenta(Cuenta<double>* cuenta) {
	Fecha fechaActual;
	std::string fechaStr = fechaActual.obtenerFechaFormateada();
	cuenta->setFechaApertura(fechaStr);
	cuenta->setEstadoCuenta("ACTIVA");
}

void CreadorCuentas::finalizarConfiguracionCuenta(Cuenta<double>* cuenta, const std::string& numeroCuenta, double monto) {
	cuenta->setNumeroCuenta(numeroCuenta);
	cuenta->depositar(monto);
}

void CreadorCuentas::agregarCuentaAMemoria(Persona* persona, CuentaAhorros* cuenta) {
	cuenta->setSiguiente(persona->getCabezaAhorros());
	cuenta->setAnterior(nullptr);
	if (persona->getCabezaAhorros() != nullptr) {
		persona->getCabezaAhorros()->setAnterior(cuenta);
	}
	persona->setCabezaAhorros(cuenta);
}

void CreadorCuentas::agregarCuentaAMemoria(Persona* persona, CuentaCorriente* cuenta) {
	cuenta->setSiguiente(persona->getCabezaCorriente());
	cuenta->setAnterior(nullptr);
	if (persona->getCabezaCorriente() != nullptr) {
		persona->getCabezaCorriente()->setAnterior(cuenta);
	}
	persona->setCabezaCorriente(cuenta);
}