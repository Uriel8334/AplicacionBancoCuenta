#include "Banco.h"  
#include "Utilidades.h"  
#include "Validar.h"  
#include <iostream>  
#include <conio.h>  
#include <thread>  
#include <chrono>  
#include "ManejoMenus.h"  
#include "ConexionMongo.h"  
#include "Cuenta.h"  
#include "BancoManejoPersona.h"  
#include "BancoManejoCuenta.h"  
#include "_BaseDatosPersona.h"  
#include "CreadorCuentas.h"  
#include "BuscadorCuentas.h"  
#include "ValidadorBaseDatos.h" 
#include "BancoManejaRegistro.h"
#include "Persona.h"
#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include <memory>
#include <iomanip>

Banco::Banco() :
	manejoPersonas(std::make_unique<BancoManejoPersona>()),
	manejoCuentas(std::make_unique<BancoManejoCuenta>(*manejoPersonas)),
	manejoRegistros(std::make_unique<BancoManejaRegistro>()),
	baseDatosPersona(ConexionMongo::obtenerClienteBaseDatos()),
	buscadorCuentas(std::make_unique<BuscadorCuentas>(baseDatosPersona)),
	validadorBaseDatos(std::make_unique<ValidadorBaseDatos>(baseDatosPersona)) {
}

Banco::~Banco() = default;

bool Banco::validarOperacion(const std::string& operacion) {
	// Crear validador especializado
	ValidadorBaseDatos validador(*validadorBaseDatos);

	// Lógica de validación centralizada usando MongoDB
	if (operacion == "buscar") {
		bool tienePersonas = validador.tienePersonasRegistradas();
		if (!tienePersonas) {
			std::cout << "\n[INFORMACIÓN] No hay personas registradas en la base de datos.\n";
			std::cout << "Para buscar cuentas, primero debe registrar al menos una persona.\n";
			validador.mostrarEstadoBaseDatos();
		}
		return tienePersonas;
	}
	else if (operacion == "transferir") {
		bool tieneCuentas = validador.tieneCuentasRegistradas();
		if (!tieneCuentas) {
			std::cout << "\n[INFORMACIÓN] No hay cuentas registradas en la base de datos.\n";
			std::cout << "Para realizar transferencias, debe crear al menos dos cuentas.\n";
			validador.mostrarEstadoBaseDatos();
		}
		return tieneCuentas;
	}
	else if (operacion == "cuentas") {
		// Para operaciones de cuentas, verificar que existan cuentas
		bool tieneCuentas = validador.tieneCuentasRegistradas();
		if (!tieneCuentas) {
			std::cout << "\n[INFORMACIÓN] No hay cuentas bancarias registradas en el sistema.\n";
			std::cout << "Para realizar operaciones bancarias, primero debe crear al menos una cuenta.\n";
			validador.mostrarEstadoBaseDatos();
		}
		return tieneCuentas;
	}

	return true; // Para otras operaciones, permitir por defecto
}

void Banco::registrarOperacion(const std::string& operacion) {
	// Log de operaciones si es necesario
	// std::cout << "Operación realizada: " << operacion << std::endl;
}

void Banco::agregarPersonaConCuenta() {
	if (!validarOperacion("agregar")) return;

	// Obtener datos iniciales
	auto [tipoCuenta, cedula, tipoCuentaInt] = obtenerDatosIniciales();
	if (tipoCuenta == "Cancelar" || tipoCuentaInt == -1) return;

	// Buscar persona existente
	Persona* personaExistente = baseDatosPersona.obtenerPersonaPorCedula(cedula);

	bool operacionExitosa = false;
	std::string tipoOperacion;

	// Aplicar estrategia según el caso
	if (personaExistente != nullptr) { // Persona ya existe
		std::cout << "DEBUG: ENTRANDO A CREAR CUENTA " + tipoCuenta + " CON PERSONA EXISTENTE\n";
		system("pause");
		operacionExitosa = procesarPersonaExistente(personaExistente, tipoCuenta, cedula);
		tipoOperacion = "agregar_cuenta";
	}
	else { // Persona no existe, crear nueva
		std::cout << "DEBUG: ENTRANDO A CREAR CUENTA " + tipoCuenta + " CON PERSONA NUEVA\n";
		system("pause");
		operacionExitosa = procesarPersonaNueva(tipoCuenta, cedula);
		tipoOperacion = "crear_persona_cuenta";
	}

	// Finalizar operación con registros y mensajes
	finalizarOperacion(operacionExitosa, cedula, tipoOperacion);
}

std::tuple<std::string, std::string, int> Banco::obtenerDatosIniciales() {
	int tipoCuenta = ManejoMenus::mostrarMenuTipoCuenta();
	if (tipoCuenta == -1) {
		std::cout << "Operacion cancelada.\n";
		return { "Cancelar" , "" , -1 };
	}

	std::string strTipoCuenta;
	if (tipoCuenta == 0) {
		strTipoCuenta = "ahorros";
	}
	else {
		strTipoCuenta = "corriente";
	}

	std::string cedula = ManejoMenus::solicitarCedula();
	if (cedula.empty()) {
		std::cout << "Operacion cancelada por el usuario.\n";
		return { "Cancelar", "" , -1 };
	}

	return { strTipoCuenta, cedula, tipoCuenta };
}

bool Banco::procesarPersonaExistente(Persona* persona, const std::string& tipoCuenta, const std::string& cedula) {
	std::string nombreCompleto = persona->getNombres() + " " + persona->getApellidos();

	if (ManejoMenus::confirmarAgregarCuentaExistente(cedula, nombreCompleto)) {
		return crearCuentaPorTipo(tipoCuenta, cedula, nombreCompleto);
	}
	else {
		std::cout << "Operacion cancelada. No se agrego nueva cuenta.\n";
		return false;
	}
}

// REFACTORIZADA: Aplicando principios SOLID
bool Banco::procesarPersonaNueva(const std::string& tipoCuenta, const std::string& cedula) {
	// Mostrar mensaje y preparar persona
	mostrarMensajeCreacionCliente();

	auto nuevaPersona = prepararNuevaPersona(cedula);
	if (!nuevaPersona) {
		return false;
	}
	
	Utilidades::mostrarCursor();
	nuevaPersona->ingresarDatos();

	// Persistir persona en base de datos
	if (!persistirPersonaEnBaseDatos(*nuevaPersona)) {
		return false;
	}

	// Crear cuenta usando el patrón Strategy
	auto resultadoCreacion = crearCuentaSegunTipo(tipoCuenta, nuevaPersona.get(), cedula);
	Utilidades::ocultarCursor();

	if (!resultadoCreacion.first) {
		std::cout << "Error al crear la cuenta para la nueva persona.\n";
		return false;
	}

	

	// Mostrar resultado final
	std::string nombreCompleto = nuevaPersona->getNombres() + "" + nuevaPersona->getApellidos();
	mostrarResultadoCreacion(true, tipoCuenta, nombreCompleto);

	// Liberamos la referencia del unique_ptr para que no se destruya automáticamente
	nuevaPersona.release();

	return true;
}

// REFACTORIZADA: Template Method Pattern
bool Banco::crearCuentaPorTipo(const std::string& tipoCuenta,
	const std::string& cedula,
	const std::string& nombreCompleto) {
	try {
		// Template Method Pattern
		auto persona = obtenerPersonaExistente(cedula);
		if (!persona) {
			return false;
		}

		CreadorCuentas creador(baseDatosPersona);
		auto resultado = crearCuentaParaPersonaExistente(creador, tipoCuenta, persona.get(), cedula);

		mostrarResultadoCreacion(resultado.first, tipoCuenta, nombreCompleto);
		return resultado.first;
	}
	catch (const std::exception& e) {
		std::cerr << "Error en crearCuentaPorTipo: " << e.what() << std::endl;
		return false;
	}
}

// === MÉTODOS PRIVADOS EXTRAÍDOS (Extract Method) ===

void Banco::mostrarMensajeCreacionCliente() {
	std::cout << "Cedula no encontrada en el sistema. \nCreando nuevo cliente..."
		<< "\nPresione cualquier tecla para continuar";
	int teclaCualquiera = _getch();
	(void)teclaCualquiera;
}

std::unique_ptr<Persona> Banco::prepararNuevaPersona(const std::string& cedula) {
	auto nuevaPersona = std::make_unique<Persona>();
	nuevaPersona->setCedula(cedula);

	// Asegurar que personaActual esté establecido ANTES de cualquier operación
	manejoPersonas->personaActual = nuevaPersona.get();
	manejoPersonas->agregarPersona(nuevaPersona.get());

	return nuevaPersona;
}

std::pair<bool, std::string> Banco::crearCuentaSegunTipo(const std::string& tipoCuenta, Persona* persona, const std::string& cedula)
{

	CreadorCuentas creador(baseDatosPersona);

	if (tipoCuenta == "ahorros") {
		auto cuenta = std::make_unique<CuentaAhorros>();
		manejoCuentas->cuentaAhorrosActual = cuenta.get();
		return creador.crearCuentaAhorros(persona, cedula, cuenta.release());
	}
	else if (tipoCuenta == "corriente") {
		auto cuenta = std::make_unique<CuentaCorriente>();
		manejoCuentas->cuentaCorrienteActual = cuenta.get();
		return creador.crearCuentaCorriente(persona, cedula, cuenta.release());
	}

	return { false, "Tipo de cuenta no válido" };
}

bool Banco::persistirPersonaEnBaseDatos(const Persona& persona) {
	bool exitoBaseDatos = baseDatosPersona.insertarNuevaPersona(persona);
	if (!exitoBaseDatos) {
		std::cout << "Error al insertar nueva persona en la base de datos.\n";
		return false;
	}
	return true;
}

std::unique_ptr<Persona> Banco::obtenerPersonaExistente(const std::string& cedula) {
	auto persona = baseDatosPersona.obtenerPersonaPorCedula(cedula);
	return persona ? std::unique_ptr<Persona>(persona) : nullptr;
}

std::pair<bool, std::string> Banco::crearCuentaParaPersonaExistente(
	CreadorCuentas& creador,
	const std::string& tipoCuenta,
	Persona* persona,
	const std::string& cedula) {

	if (tipoCuenta == "ahorros") {
		auto cuenta = std::make_unique<CuentaAhorros>();
		return creador.crearCuentaAhorros(persona, cedula, cuenta.release());
	}
	else if (tipoCuenta == "corriente") {
		auto cuenta = std::make_unique<CuentaCorriente>();
		return creador.crearCuentaCorriente(persona, cedula, cuenta.release());
	}

	return { false, "Tipo de cuenta no válido" };
}

// === MÉTODOS QUE PERMANECEN IGUAL ===

void Banco::mostrarResultadoCreacion(bool exito, const std::string& tipoCuenta, const std::string& nombreCompleto) {
	if (exito) {
		std::string mensaje = nombreCompleto.empty()
			? "Cliente y cuenta de " + tipoCuenta + " creados correctamente.\n"
			: "Nueva cuenta de " + tipoCuenta + " agregada correctamente a " + nombreCompleto + ".\n";
		std::cout << mensaje;
	}
	else {
		std::string mensaje = nombreCompleto.empty()
			? "Error al crear la cuenta de " + tipoCuenta + ".\n"
			: "Error al crear la cuenta de " + tipoCuenta + " para " + nombreCompleto + ".\n";
		std::cout << mensaje;
	}
}

void Banco::finalizarOperacion(bool exitoso, const std::string& cedula, const std::string& tipoOperacion) {
	if (exitoso) {
		// Registrar la operación en la base de datos
		if (manejoRegistros->agregarRegistroBaseDatos(tipoOperacion, cedula)) {
			std::cout << "\n[SISTEMA] Operacion registrada correctamente en el sistema.\n";
		}
		else {
			std::cout << "\n[ADVERTENCIA] La operacion fue exitosa pero no se pudo registrar en el log.\n";
		}

		// Registrar en el sistema interno
		registrarOperacion("agregar_persona_cuenta");
	}
	else {
		std::cout << "\n[ERROR] La operacion no se pudo completar.\n";
	}

	// Mensaje de salida con realismo
	std::cout << "\nPresione cualquier tecla para salir";
	int teclaCualquiera = _getch();
	(void)teclaCualquiera;

	Utilidades::mensajeAnimado("Saliendo de la creacion de cuenta");
}

void Banco::buscarCuenta() {
	if (!validarOperacion("buscar")) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "No hay personas registradas todavia.\n";
		system("pause");
		return;
	}

	// Delegar la búsqueda al manejador especializado
	std::vector<std::string> opcionesBusqueda = {
		"Por fecha de creacion",
		"Por criterio de usuario",
		"Por numero de cuenta",
		"Por cedula",
		"Cancelar"
	};
	
	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	int seleccion = Utilidades::menuInteractivo("Seleccione el tipo de busqueda:", opcionesBusqueda, 0, 0);

	if (seleccion >= 0 && seleccion <= 3) {
		buscadorCuentas->ejecutarBusqueda(seleccion);
	}
	else if (seleccion == 4 || seleccion == -1) {
		std::cout << "Búsqueda cancelada.\n";
	}

	registrarOperacion("buscar_cuenta");
}

bool Banco::verificarCuentasBanco() const {
	// Crear validador especializado para verificación de base de datos
	ValidadorBaseDatos validador(const_cast<_BaseDatosPersona&>(baseDatosPersona));

	// Verificar directamente en MongoDB si existen cuentas
	bool tieneCuentas = validador.tieneCuentasRegistradas();

	if (!tieneCuentas) {
		// Mostrar información detallada del estado
		validador.mostrarEstadoBaseDatos();
		std::cout << "\n[INFORMACIÓN] No se encontraron cuentas en la base de datos MongoDB.\n";
		std::cout << "Para realizar operaciones bancarias, primero debe crear al menos una cuenta.\n";
	}

	return tieneCuentas;
}

void Banco::realizarTransferencia() {
	if (!validarOperacion("transferir")) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "No hay cuentas registradas en el sistema.\n";
		system("pause");
		return;
	}

	// 1. Obtener cuenta origen
	std::string cuentaOrigen = ManejoMenus::solicitarNumeroCuenta("origen");
	if (cuentaOrigen.empty()) return;

	// 2. Obtener cuenta destino
	std::string cuentaDestino = ManejoMenus::solicitarNumeroCuenta("destino");
	if (cuentaDestino.empty() || cuentaDestino == cuentaOrigen) {
		std::cout << "Cuenta de destino inválida.\n";
		return;
	}

	// 3. Verificar que ambas cuentas existen
	if (!manejoCuentas->existeCuenta(cuentaOrigen) || !manejoCuentas->existeCuenta(cuentaDestino)) {
		std::cout << "Una o ambas cuentas no existen.\n";
		return;
	}

	// 4. Mostrar saldos actuales
	double saldoOrigen = manejoCuentas->consultarSaldo(cuentaOrigen);
	std::cout << "Saldo actual cuenta origen: $" << std::fixed << std::setprecision(2) << saldoOrigen << std::endl;

	// 5. Obtener monto
	double monto = ManejoMenus::solicitarMonto();
	if (monto <= 0) return;

	// 6. Validar fondos
	if (!manejoCuentas->validarFondosSuficientes(cuentaOrigen, monto)) {
		std::cout << "Fondos insuficientes.\n";
		return;
	}

	// 7. Confirmar transferencia
	if (!ManejoMenus::confirmarOperacion("¿Confirmar la transferencia de $" + std::to_string(monto) + "?")) {
		std::cout << "Transferencia cancelada.\n";
		return;
	}

	// 8. Realizar transferencia
	if (manejoCuentas->transferir(cuentaOrigen, cuentaDestino, monto)) {
		std::cout << "Transferencia realizada exitosamente.\n";
		std::cout << "Nuevo saldo cuenta origen: $" << std::fixed << std::setprecision(2)
			<< manejoCuentas->consultarSaldo(cuentaOrigen) << std::endl;
	}
	else {
		std::cout << "Error al realizar la transferencia.\n";
	}

	registrarOperacion("transferencia");
	system("pause");
}

void Banco::subMenuCuentasBancarias() {
	if (!validarOperacion("cuentas")) {
		system("pause");
		return;
	}

	std::vector<std::string> opcionesCuentas = {
		"Realizar Deposito",
		"Realizar Retiro",
		"Consultar Saldo",
		"Mostrar Informacion de Cuenta",
		"Volver al Menu Principal"
	};

	int seleccion;
	do {

		// Mostrar estadísticas actuales de la base de datos
		ValidadorBaseDatos validador(baseDatosPersona);
		validador.mostrarEstadoBaseDatos();

		Utilidades::limpiarPantallaPreservandoMarquesina(1);

		seleccion = Utilidades::menuInteractivo("=== SUBMENU CUENTAS BANCARIAS ===", opcionesCuentas, 0, 0);

		switch (seleccion) {
		case 0: { // Realizar Depósito
			realizarDeposito();
			break;
		}
		case 1: { // Realizar Retiro
			realizarRetiro();
			break;
		}
		case 2: { // Consultar Saldo
			consultarSaldo();
			break;
		}
		case 3: { // Mostrar Información Completa
			mostrarInformacionCuenta();
			break;
		}
		case 4: // Volver al menú principal
			break;
		}
	} while (seleccion != 4);

	registrarOperacion("Operaciones_Cuentas");
}

// === MÉTODOS AUXILIARES EXTRAÍDOS (Apply Extract Method) ===

void Banco::realizarDeposito() {
	std::string numeroCuenta = ManejoMenus::solicitarNumeroCuenta("para depósito");
	if (!numeroCuenta.empty()) {
		// Verificar existencia usando MongoDB directamente
		double saldoActual = baseDatosPersona.obtenerSaldoCuenta(numeroCuenta);
		if (saldoActual >= 0) { // Cuenta existe
			double monto = ManejoMenus::solicitarMonto();
			if (monto > 0) {
				if (baseDatosPersona.depositarEnCuenta(numeroCuenta, monto)) {
					ManejoMenus::mostrarMensajeExito("Depósito realizado exitosamente.");
					std::cout << "Nuevo saldo: $" << std::fixed << std::setprecision(2)
						<< baseDatosPersona.obtenerSaldoCuenta(numeroCuenta) << std::endl;
					registrarOperacion("deposito");
				}
				else {
					ManejoMenus::mostrarMensajeError("Error al realizar el depósito.");
				}
			}
		}
		else {
			ManejoMenus::mostrarMensajeError("Cuenta no encontrada en la base de datos.");
		}
	}
	system("pause");
}

void Banco::realizarRetiro() {
	std::string numeroCuenta = ManejoMenus::solicitarNumeroCuenta("para retiro");
	if (!numeroCuenta.empty()) {
		double saldoActual = baseDatosPersona.obtenerSaldoCuenta(numeroCuenta);
		if (saldoActual >= 0) { // Cuenta existe
			std::cout << "Saldo actual: $" << std::fixed << std::setprecision(2) << saldoActual << std::endl;

			double monto = ManejoMenus::solicitarMonto();
			if (monto > 0 && baseDatosPersona.verificarFondosSuficientes(numeroCuenta, monto)) {
				if (baseDatosPersona.retirarDeCuenta(numeroCuenta, monto)) {
					ManejoMenus::mostrarMensajeExito("Retiro realizado exitosamente.");
					std::cout << "Nuevo saldo: $" << std::fixed << std::setprecision(2)
						<< baseDatosPersona.obtenerSaldoCuenta(numeroCuenta) << std::endl;
					registrarOperacion("retiro");
				}
				else {
					ManejoMenus::mostrarMensajeError("Error al realizar el retiro.");
				}
			}
			else {
				ManejoMenus::mostrarMensajeError("Fondos insuficientes o monto inválido.");
			}
		}
		else {
			ManejoMenus::mostrarMensajeError("Cuenta no encontrada en la base de datos.");
		}
	}
	system("pause");
}

void Banco::consultarSaldo() {
	std::string numeroCuenta = ManejoMenus::solicitarNumeroCuenta("para consulta");
	if (!numeroCuenta.empty()) {
		double saldo = baseDatosPersona.obtenerSaldoCuenta(numeroCuenta);
		if (saldo >= 0) {
			std::cout << "Saldo actual de la cuenta " << numeroCuenta << ": $"
				<< std::fixed << std::setprecision(2) << saldo << std::endl;
			registrarOperacion("consulta_saldo");
		}
		else {
			ManejoMenus::mostrarMensajeError("Cuenta no encontrada en la base de datos.");
		}
	}
	system("pause");
}

void Banco::mostrarInformacionCuenta() {
	std::string numeroCuenta = ManejoMenus::solicitarNumeroCuenta("para información");
	if (!numeroCuenta.empty()) {
		auto infoCuenta = baseDatosPersona.obtenerInformacionCuenta(numeroCuenta);
		auto view = infoCuenta.view();

		if (!view.empty() && view.find("numeroCuenta") != view.end()) {
			// Mostrar información usando la funcionalidad existente del buscador
			buscadorCuentas->buscarPorNumeroCuenta(); // Reutilizar la lógica existente
			registrarOperacion("mostrar_informacion_cuenta");
		}
		else {
			ManejoMenus::mostrarMensajeError("Cuenta no encontrada en la base de datos.");
		}
	}
	system("pause");
}

// === MÉTODOS DE ACCESO ===

BancoManejoPersona& Banco::getManejoPersonas() {
	return *manejoPersonas;
}

BancoManejoCuenta& Banco::getManejoCuentas() {
	return *manejoCuentas;
}

// Compatibilidad con código existente
NodoPersona* Banco::getListaPersonas() const {
	return manejoPersonas->getListaPersonas();
}

void Banco::setListaPersonas(NodoPersona* lista) {
	manejoPersonas->setListaPersonas(lista);
}

void Banco::forEachPersona(const std::function<void(Persona*)>& funcion) const {
	manejoPersonas->forEachPersona(funcion);
}