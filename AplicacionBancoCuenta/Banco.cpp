/**
 * @file Banco.cpp
 * @brief Implementación de la clase Banco que gestiona el sistema bancario
 * @author Sistema Bancario, Uriel Andrade, Kerly Chuqui, Abner Proano
 * @date 2025
 */
#define NOMINMAX
#include "Banco.h"
#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include <iostream>
#include <locale>
#include <sstream>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include "Utilidades.h"
#include "NodoPersona.h"
#include <iomanip>
#include <map>
#include <functional>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include "_BaseDatosPersona.h"
#include "ConexionMongo.h"

 /**
  * @brief Constructor por defecto de la clase Banco
  *
  * Inicializa una instancia de Banco con una lista vacía de personas
  */
Banco::Banco() : listaPersonas(nullptr) {} // Constructor

/**
 * @brief Destructor de la clase Banco
 *
 * Libera toda la memoria ocupada por la lista enlazada de personas
 */
Banco::~Banco() { // Destructor
	// Liberar memoria de la lista enlazada
	NodoPersona* actual = listaPersonas;
	while (actual) {
		NodoPersona* temp = actual;
		actual = actual->siguiente;
		delete temp;
	}
}

/**
 * @brief Guarda una cuenta de ahorros en la base de datos MongoDB
 * @param cedula Cédula del titular de la cuenta
 * @param cuenta Puntero a la cuenta de ahorros a guardar
 */
void Banco::guardarCuentaEnMongoDB(const std::string& cedula, const CuentaAhorros* cuenta) {
	try {
		_BaseDatosPersona dbPersona = ConexionMongo::getCliente();

		auto cuentaDoc = bsoncxx::builder::basic::document{};
		cuentaDoc.append(
			bsoncxx::builder::basic::kvp("tipo", "ahorros"),
			bsoncxx::builder::basic::kvp("numeroCuenta", cuenta->getNumeroCuenta()),
			bsoncxx::builder::basic::kvp("saldo", cuenta->getSaldo()),
			bsoncxx::builder::basic::kvp("fechaApertura", cuenta->getFechaApertura()),
			bsoncxx::builder::basic::kvp("estado", cuenta->getEstadoCuenta())
		);

		if (dbPersona.agregarCuentaPersona(cedula, cuentaDoc.extract())) {
			std::cout << "Cuenta agregada correctamente en MongoDB.\n";
		}
		else {
			std::cout << "Error al agregar la cuenta en MongoDB.\n";
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error de conexión a MongoDB: " << e.what() << std::endl;
	}
}

/**
 * @brief Guarda una cuenta corriente en la base de datos MongoDB
 * @param cedula Cédula del titular de la cuenta
 * @param cuenta Puntero a la cuenta corriente a guardar
 */
void Banco::guardarCuentaEnMongoDB(const std::string& cedula, const CuentaCorriente* cuenta) {
	try {
		mongocxx::client& client = ConexionMongo::getCliente();
		_BaseDatosPersona dbPersona(client);

		auto cuentaDoc = bsoncxx::builder::basic::document{};
		cuentaDoc.append(
			bsoncxx::builder::basic::kvp("tipo", "corriente"),
			bsoncxx::builder::basic::kvp("numeroCuenta", cuenta->getNumeroCuenta()),
			bsoncxx::builder::basic::kvp("saldo", cuenta->getSaldo()),
			bsoncxx::builder::basic::kvp("fechaApertura", cuenta->getFechaApertura()),
			bsoncxx::builder::basic::kvp("estado", cuenta->getEstadoCuenta())
		);

		if (dbPersona.agregarCuentaPersona(cedula, cuentaDoc.extract())) {
			std::cout << "Cuenta agregada correctamente en MongoDB.\n";
		}
		else {
			std::cout << "Error al agregar la cuenta en MongoDB.\n";
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error de conexión a MongoDB: " << e.what() << std::endl;
	}
}

Persona* Banco::obtenerOIngresarPersona(const std::string& cedula, _BaseDatosPersona& dbPersona) {
	Persona* personaExistente = dbPersona.obtenerPersonaPorCedula(cedula);
	if (personaExistente) {
		personaExistente->mostrarDatos();
		if (!confirmarAgregarCuentaExistente(personaExistente, cedula)) {
			delete personaExistente; // Evita fugas de memoria
			return nullptr;
		}
		return personaExistente;
	}
	else { // Se crea una nueva persona
		Persona* nuevaPersona = new Persona();
		nuevaPersona->setCedula(cedula);
		nuevaPersona->ingresarDatos(cedula);
		return nuevaPersona;
	}
}

void Banco::agregarCuentaAPersona(Persona* persona, int tipoCuenta, const std::string& cedula, _BaseDatosPersona& dbPersona) {
	if (tipoCuenta == 0) {
		CuentaAhorros* nuevaCuenta = new CuentaAhorros();
		if (persona->crearSoloCuentaAhorros(nuevaCuenta, cedula)) {
			guardarCuentaEnMongoDB(cedula, nuevaCuenta);
		}
		else {
			delete nuevaCuenta;
		}
	}
	else if (tipoCuenta == 1) {
		CuentaCorriente* nuevaCuenta = new CuentaCorriente();
		if (persona->crearSoloCuentaCorriente(nuevaCuenta, cedula)) {
			guardarCuentaEnMongoDB(cedula, nuevaCuenta);
		}
		else {
			delete nuevaCuenta;
		}
	}
}

/**
 * @brief Agrega una nueva persona con una cuenta asociada al sistema bancario
 *
 * Este método presenta un menú para seleccionar el tipo de cuenta (ahorros o corriente),
 * solicita los datos personales necesarios y crea tanto la persona como su cuenta asociada.
 * También maneja el caso de cédulas existentes, permitiendo agregar una nueva cuenta a un
 * cliente que ya existe en el sistema.
 */
void Banco::agregarPersonaConCuenta() {
	int seleccion = mostrarMenuTipoCuenta();
	if (seleccion == 2) {
		std::cout << "Operacion cancelada.\n";
		return;
	}

	Utilidades::mostrarCursor();
	std::string cedula = solicitarCedula();
	if (cedula.empty()) {
		std::cout << "Operacion cancelada por el usuario.\n";
		return;
	}

	mongocxx::client& client = ConexionMongo::getCliente();
	_BaseDatosPersona dbPersona(client);

	bool personaExiste = dbPersona.existePersonaPorCedula(cedula);

	// 1. Preparar persona
	Persona* persona = obtenerOIngresarPersona(cedula, dbPersona);
	if (!persona) return;

	// 2. Preparar documento de cuenta
	bsoncxx::document::value cuentaDoc = crearDocumentoCuenta(persona, seleccion, cedula);
	if (cuentaDoc.view().empty()) {
		delete persona;
		return;
	}

	// 3. Persistir en base de datos
	if (!personaExiste) {
		insertarPersonaConCuenta(*persona, cuentaDoc, dbPersona);
		agregarPersonaEnMemoria(persona);
	}
	else {
		agregarCuentaPersonaExistente(cedula, cuentaDoc, dbPersona);
	}

	system("pause");
}

bsoncxx::document::value Banco::crearDocumentoCuenta(Persona* persona, int tipoCuenta, const std::string& cedula) {
	if (tipoCuenta == 0) { // Ahorros
		CuentaAhorros* nuevaCuenta = new CuentaAhorros();
		if (persona->crearSoloCuentaAhorros(nuevaCuenta, cedula)) {
			auto doc = bsoncxx::builder::basic::document{};
			doc.append(
				bsoncxx::builder::basic::kvp("tipo", "ahorros"),
				bsoncxx::builder::basic::kvp("numeroCuenta", nuevaCuenta->getNumeroCuenta()),
				bsoncxx::builder::basic::kvp("saldo", nuevaCuenta->getSaldo()),
				bsoncxx::builder::basic::kvp("fechaApertura", nuevaCuenta->getFechaApertura()),
				bsoncxx::builder::basic::kvp("estado", nuevaCuenta->getEstadoCuenta())
			);
			delete nuevaCuenta;
			return doc.extract();
		}
		delete nuevaCuenta;
	}
	else if (tipoCuenta == 1) { // Corriente
		CuentaCorriente* nuevaCuenta = new CuentaCorriente();
		if (persona->crearSoloCuentaCorriente(nuevaCuenta, cedula)) {
			auto doc = bsoncxx::builder::basic::document{};
			doc.append(
				bsoncxx::builder::basic::kvp("tipo", "corriente"),
				bsoncxx::builder::basic::kvp("numeroCuenta", nuevaCuenta->getNumeroCuenta()),
				bsoncxx::builder::basic::kvp("saldo", nuevaCuenta->getSaldo()),
				bsoncxx::builder::basic::kvp("fechaApertura", nuevaCuenta->getFechaApertura()),
				bsoncxx::builder::basic::kvp("estado", nuevaCuenta->getEstadoCuenta())
			);
			delete nuevaCuenta;
			return doc.extract();
		}
		delete nuevaCuenta;
	}
	return bsoncxx::builder::basic::document{}.extract(); // Devuelve vacío si falla
}

void Banco::insertarPersonaConCuenta(const Persona& persona, const bsoncxx::document::value& cuentaDoc, _BaseDatosPersona& dbPersona) {
	if (dbPersona.insertarPersona(persona, &cuentaDoc)) {
		std::cout << "Persona y cuenta insertadas correctamente en MongoDB.\n";
	}
	else {
		std::cout << "Error al insertar persona y cuenta en MongoDB.\n";
	}
}

void Banco::agregarCuentaPersonaExistente(const std::string& cedula, const bsoncxx::document::value& cuentaDoc, _BaseDatosPersona& dbPersona) {
	if (dbPersona.agregarCuentaPersona(cedula, cuentaDoc)) {
		std::cout << "Cuenta agregada correctamente en MongoDB.\n";
	}
	else {
		std::cout << "Error al agregar la cuenta en MongoDB.\n";
	}
}

void Banco::agregarPersonaEnMemoria(Persona* persona) {
	NodoPersona* nuevo = new NodoPersona(persona);
	nuevo->siguiente = listaPersonas;
	listaPersonas = nuevo;
}

/**
 * @brief Muestra un menú interactivo para seleccionar el tipo de cuenta a crear
 * @return Entero que representa la opción seleccionada (0 para Ahorros, 1 para Corriente, 2 para Cancelar)
 */
int Banco::mostrarMenuTipoCuenta() {
	std::vector<std::string> opciones = { "Cuenta de Ahorros", "Cuenta Corriente", "Cancelar" };
	// Puedes ajustar las coordenadas x, y según tu preferencia visual
	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	Utilidades::ocultarCursor();
	int seleccion = Utilidades::menuInteractivo("Seleccione el tipo de cuenta a crear para la persona:", opciones, 0, 0);
	return seleccion;
}

std::string Banco::solicitarCedula() {
	std::string cedulaTemp;
	while (true) {
		system("pause");
		Utilidades::mostrarCursor();
		Utilidades::limpiarPantallaPreservandoMarquesina(0);
		std::cout << "\n----- INGRESE SUS DATOS -----\n";
		std::cout << "Ingrese su cedula (10 digitos): ";
		cedulaTemp.clear();
		int digitos = 0;
		while (true) {
			char tecla = _getch();
			if ((tecla >= '0' && tecla <= '9') && digitos < 10) {
				cedulaTemp += tecla;
				digitos++;
				std::cout << tecla;
			}
			else if (tecla == 8 && !cedulaTemp.empty()) {
				cedulaTemp.pop_back();
				digitos--;
				std::cout << "\b \b";
			}
			else if (tecla == 13) {
				std::cout << std::endl;
				break;
			}
			else if (tecla == 27) {
				return "";
			}
		}
		if (!Validar::ValidarCedula(cedulaTemp)) {
			std::cout << "Cedula invalida, presione cualquier tecla para volver a ingresar." << std::endl;
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
			continue;
		}
		else
		{
			std::cout << "\nCargando, por favor espere...\n";
		}
		break;
	}
	return cedulaTemp;
}

Persona* Banco::buscarPersonaPorCedula(const std::string& cedula) {
	Persona* resultado = nullptr;
	forEachPersona([&](Persona* p) {
		if (p->getCedula() == cedula) {
			resultado = p;
		}
		});
	return resultado;
}

/**
 * @brief Confirma si se desea agregar una cuenta a un titular existente
 * @param personaExistente Puntero al objeto Persona existente
 * @param cedula Cédula del titular
 * @return true si se desea agregar una nueva cuenta, false si se cancela la operación
 */
bool Banco::confirmarAgregarCuentaExistente(Persona* personaExistente, const std::string& cedula) {
	Utilidades::limpiarPantallaPreservandoMarquesina(0);
	Utilidades::ocultarCursor();

	// Mensaje informativo
	std::cout << "La cedula " << cedula << " ya esta registrada en el sistema.\n";
	std::cout << "Titular: " << personaExistente->getNombres() << " " << personaExistente->getApellidos() << "\n\n";
	std::cout << "Desea agregar una nueva cuenta para este titular?\n" << std::endl;

	// Opciones del menú
	std::vector<std::string> opciones = { "Si", "No" };
	int seleccion = Utilidades::menuInteractivo("Seleccione una opcion:", opciones, 0, 0);

	if (seleccion == 1 || seleccion == -1) {
		std::cout << "Operacion cancelada.\n";
		system("pause");
		return false;
	}
	std::cout << "Usando datos de titular existente.\n";
	system("pause");
	Utilidades::mostrarCursor();
	return true;
}

void Banco::crearCuentaAhorrosParaPersona(Persona* persona, const std::string& cedula) {
	try {
		Fecha fechaActual;
		std::string fechaStr = fechaActual.obtenerFechaFormateada();
		CuentaAhorros* nuevaCuentaAhorros = new CuentaAhorros("0", 0.0, fechaStr, "Activa", 5.0);
		bool resultado = persona->crearAgregarCuentaAhorros(nuevaCuentaAhorros, cedula);
		if (!resultado) {
			delete nuevaCuentaAhorros;
		}
		else {
			std::cout << "Cuenta de ahorros creada correctamente.\n";

			// --- INICIO BLOQUE MONGODB ---
			try {
				mongocxx::client& client = ConexionMongo::getCliente();
				_BaseDatosPersona dbPersona(client);

				auto cuentaDoc = bsoncxx::builder::basic::document{};
				cuentaDoc.append(
					bsoncxx::builder::basic::kvp("tipo", "ahorros"),
					bsoncxx::builder::basic::kvp("numeroCuenta", nuevaCuentaAhorros->getNumeroCuenta()),
					bsoncxx::builder::basic::kvp("saldo", nuevaCuentaAhorros->consultarSaldo()),
					bsoncxx::builder::basic::kvp("fechaApertura", nuevaCuentaAhorros->getFechaApertura()),
					bsoncxx::builder::basic::kvp("estado", nuevaCuentaAhorros->getEstadoCuenta())
				);

				if (dbPersona.agregarCuentaPersona(cedula, cuentaDoc.extract())) {
					std::cout << "Cuenta agregada correctamente en MongoDB.\n";
				}
				else {
					std::cout << "Error al agregar la cuenta en MongoDB.\n";
				}
			}
			catch (const std::exception& e) {
				std::cerr << "Error de conexión a MongoDB: " << e.what() << std::endl;
			}
			// --- FIN BLOQUE MONGODB ---
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		system("pause");
	}
}

void Banco::crearCuentaCorrienteParaPersona(Persona* persona, const std::string& cedula) {
	try {
		Fecha fechaActual;
		std::string fechaStr = fechaActual.obtenerFechaFormateada();
		CuentaCorriente* nuevaCuentaCorriente = new CuentaCorriente("0", 0.0, fechaStr, "Activa", 0.0);
		bool resultado = persona->crearAgregarCuentaCorriente(nuevaCuentaCorriente, cedula);
		if (!resultado) {
			delete nuevaCuentaCorriente;
		}
		else {
			std::cout << "Cuenta de ahorros creada correctamente.\n";

			// --- INICIO BLOQUE MONGODB ---
			try {
				mongocxx::client& client = ConexionMongo::getCliente();
				_BaseDatosPersona dbPersona(client);

				auto cuentaDoc = bsoncxx::builder::basic::document{};
				cuentaDoc.append(
					bsoncxx::builder::basic::kvp("tipo", "corriente"),
					bsoncxx::builder::basic::kvp("numeroCuenta", nuevaCuentaCorriente->getNumeroCuenta()),
					bsoncxx::builder::basic::kvp("saldo", nuevaCuentaCorriente->consultarSaldo()),
					bsoncxx::builder::basic::kvp("fechaApertura", nuevaCuentaCorriente->getFechaApertura()),
					bsoncxx::builder::basic::kvp("estado", nuevaCuentaCorriente->getEstadoCuenta())
				);

				if (dbPersona.agregarCuentaPersona(cedula, cuentaDoc.extract())) {
					std::cout << "Cuenta agregada correctamente en MongoDB.\n";
				}
				else {
					std::cout << "Error al agregar la cuenta en MongoDB.\n";
				}
			}
			catch (const std::exception& e) {
				std::cerr << "Error de conexión a MongoDB: " << e.what() << std::endl;
			}
			// --- FIN BLOQUE MONGODB ---
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		system("pause");
	}
}

/**
 * @brief Presenta un menú para buscar cuentas según diferentes criterios
 *
 * Permite buscar por fecha de creación, por criterio personalizado,
 * por número de cuenta, o por cédula del titular.
 */
void Banco::buscarCuenta() {
	CuentaAhorros* cuentaAhorros = nullptr;
	CuentaCorriente* cuentaCorriente = nullptr;
	if (listaPersonas == nullptr || reinterpret_cast<uintptr_t>(listaPersonas) > 0xFFFFFFFF00000000) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "No hay personas registradas todavia.\n";
		std::cout << "Presione cualquier tecla para continuar";
		int tecla = _getch();
		(void)tecla;
		return;
	}

	std::string opcionesBusqueda[] = { "Por fecha de creacion", "Por criterio de usuario",
									   "Por numero de cuenta", "Por cedula", "Cancelar" };
	int numOpcionesBusqueda = sizeof(opcionesBusqueda) / sizeof(opcionesBusqueda[0]);
	int seleccionBusqueda = 0;

	while (true) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "\nSeleccione el tipo de busqueda:\n\n";
		for (int i = 0; i < numOpcionesBusqueda; i++) {
			if (i == seleccionBusqueda)
				std::cout << " > " << opcionesBusqueda[i] << std::endl;
			else
				std::cout << "   " << opcionesBusqueda[i] << std::endl;
		}
		int tecla = _getch();
		if (tecla == 224) {
			tecla = _getch();
			if (tecla == 72) seleccionBusqueda = (seleccionBusqueda - 1 + numOpcionesBusqueda) % numOpcionesBusqueda;
			else if (tecla == 80) seleccionBusqueda = (seleccionBusqueda + 1) % numOpcionesBusqueda;
		}
		else if (tecla == 13) break;
	}
	if (seleccionBusqueda == 4) return;

	// Busqueda por fecha de creacion
	if (seleccionBusqueda == 0) {
		int dia = 1, mes = 1, anio = 2000, campo = 0;
		bool fechaSeleccionada = false;
		SYSTEMTIME st;
		GetLocalTime(&st);
		int anioActual = st.wYear, mesActual = st.wMonth, diaActual = st.wDay;

		auto esBisiesto = [](int anio) {
			return (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0));
			};
		auto diasEnMes = [&](int mes, int anio) {
			switch (mes) {
			case 2: return esBisiesto(anio) ? 29 : 28;
			case 4: case 6: case 9: case 11: return 30;
			default: return 31;
			}
			};

		while (!fechaSeleccionada) {
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			std::cout << "\nSeleccione la fecha de creacion, usando las flechas del teclado. ENTER para aceptar.\n";
			for (int i = 0; i < 3; ++i) {
				if (i == campo)
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				if (i == 0) std::cout << (dia < 10 ? "0" : "") << dia;
				if (i == 1) std::cout << "/" << (mes < 10 ? "0" : "") << mes;
				if (i == 2) std::cout << "/" << anio;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			}
			std::cout << std::endl;

			int tecla = _getch();
			if (tecla == 224) {
				tecla = _getch();
				switch (tecla) {
				case 75: if (campo > 0) campo--; break; // Izquierda
				case 77: if (campo < 2) campo++; break; // Derecha
				case 72: // Arriba
					if (campo == 0) { int maxDia = diasEnMes(mes, anio); dia++; if (dia > maxDia) dia = 1; }
					else if (campo == 1) { mes++; if (mes > 12) mes = 1; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
					else if (campo == 2) { anio++; if (anio > anioActual) anio = 1900; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
					break;
				case 80: // Abajo
					if (campo == 0) { int maxDia = diasEnMes(mes, anio); dia--; if (dia < 1) dia = maxDia; }
					else if (campo == 1) { mes--; if (mes < 1) mes = 12; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
					else if (campo == 2) { anio--; if (anio < 1900) anio = anioActual; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
					break;
				}
			}
			else if (tecla == 13) { // Enter
				char buffer[11];
				snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", dia, mes, anio);
				std::string fechaBuscar(buffer);
				// Aqui puedes validar la fecha si lo deseas
				fechaSeleccionada = true;

				// Realizar la busqueda usando la lista enlazada
				NodoPersona* actual = listaPersonas;
				int encontrados = 0;
				while (actual) {
					actual->persona->buscarPersonaPorFecha(fechaBuscar);
					actual = actual->siguiente;
				}
				system("pause");
				return;
			}
		}
	}

	// Busqueda por criterio de usuario
	if (seleccionBusqueda == 1) {
		buscarCuentasPorCriterio();
		return;
	}

	// Busqueda por numero de cuenta
	if (seleccionBusqueda == 2) {
		std::string numCuentaBuscar;
		std::cout << "\nIngrese el numero de cuenta: ";
		// Ingreso solo numerico con getch
		while (true) {
			char tecla = _getch();
			if (tecla >= '0' && tecla <= '9') {
				numCuentaBuscar += tecla;
				std::cout << tecla;
			}
			else if (tecla == 8 && !numCuentaBuscar.empty()) { // Backspace
				numCuentaBuscar.pop_back();
				std::cout << "\b \b";
			}
			else if (tecla == 13 && !numCuentaBuscar.empty()) { // Enter
				std::cout << std::endl;
				break;
			}
			// Ignora cualquier otra tecla
		}

		int cuentasEncontradas = 0;
		NodoPersona* actual = listaPersonas;
		while (actual) {
			if (actual->persona && actual->persona->isValidInstance()) {
				try {
					// El metodo muestra la informacion y devuelve cuantas cuentas encontro
					Utilidades::limpiarPantallaPreservandoMarquesina(1);
					cuentasEncontradas += actual->persona->buscarPersonaPorCuentas(numCuentaBuscar);
				}
				catch (...) {
					std::cerr << "Error al buscar en persona." << std::endl;
				}
			}
			actual = actual->siguiente;
		}

		if (cuentasEncontradas == 0) {
			std::cout << "\nNo se encontraron cuentas con el numero: " << numCuentaBuscar << std::endl;
		}
		else {
			std::cout << "Se encontraron " << cuentasEncontradas << " cuenta(s)." << std::endl;
		}

		system("pause");
		return;
	}

	// Nuevo bloque para busqueda por cedula
	if (seleccionBusqueda == 3) { // Por cedula
		std::string cedula;
		std::cout << "Ingrese la cedula (10 digitos): ";

		// Control de entrada para la cedula (solo numeros y maximo 10 digitos)
		int digitos = 0;
		while (true) {
			char tecla = _getch();
			if ((tecla >= '0' && tecla <= '9') && digitos < 10) {
				cedula += tecla;
				digitos++;
				std::cout << tecla;
			}
			else if (tecla == 8 && !cedula.empty()) { // Backspace
				cedula.pop_back();
				digitos--;
				std::cout << "\b \b";
			}
			else if (tecla == 13 && digitos == 10) { // Enter y cedula completa
				std::cout << std::endl;
				break;
			}
			else if (tecla == 27) { // ESC para cancelar
				std::cout << "\nBusqueda cancelada.\n";
				system("pause");
				return;
			}
		}

		if (!Validar::ValidarCedula(cedula)) {
			std::cout << "Cedula invalida.\n";
			system("pause");
			return;
		}

		// Buscar la persona con esa cedula
		bool encontrado = false;
		NodoPersona* actual = listaPersonas;
		while (actual) {
			if (actual->persona && actual->persona->isValidInstance() && actual->persona->getCedula() == cedula)
			{

				// Encontramos la persona, mostrar sus datos y cuentas
				cuentaAhorros = actual->persona->getCabezaAhorros();
				cuentaCorriente = actual->persona->getCabezaCorriente();

				// Mostrar datos basicos de la persona
				std::cout << "\n----- DATOS DEL TITULAR -----\n";
				std::cout << "Cedula: " << actual->persona->getCedula() << std::endl;
				std::cout << "Nombre: " << actual->persona->getNombres() << " "
					<< actual->persona->getApellidos() << std::endl;
				std::cout << "Correo: " << actual->persona->getCorreo() << std::endl;

				// Mostrar todas las cuentas de esta persona
				std::cout << "\n----- CUENTAS ASOCIADAS -----\n";

				// Mostrar cuentas de ahorro
				int contadorAhorros = 0;
				while (cuentaAhorros) {
					if (cuentaAhorros->getCuentaAhorros()) {
						std::cout << "\nCUENTA DE AHORROS #" << ++contadorAhorros << std::endl;
						// Pasar false para no limpiar pantalla entre cuentas
						cuentaAhorros->getCuentaAhorros()->mostrarInformacion(cedula, false);
					}
					cuentaAhorros = cuentaAhorros->getSiguiente();
				}

				// Mostrar cuentas corrientes
				int contadorCorrientes = 0;
				while (cuentaCorriente) {
					if (cuentaCorriente->getCuentaCorriente()) {
						std::cout << "\nCUENTA CORRIENTE #" << ++contadorCorrientes << std::endl;
						// Pasar false para no limpiar pantalla entre cuentas
						cuentaCorriente->getCuentaCorriente()->mostrarInformacion(cedula, false);
					}
					cuentaCorriente = cuentaCorriente->getSiguiente();
				}

				if (contadorAhorros == 0 && contadorCorrientes == 0) {
					std::cout << "Esta persona no tiene cuentas asociadas.\n";
				}
				else {
					std::cout << "\nTotal de cuentas: " << (contadorAhorros + contadorCorrientes) << std::endl;
				}

				encontrado = true;
				break;
			}
			actual = actual->siguiente;
		}

		if (!encontrado) {
			std::cout << "No se encontro ninguna persona con la cedula: " << cedula << std::endl;
		}

		system("pause");
		return;
	}
}

/**
 * @brief Función auxiliar para buscar cuentas por fecha de forma recursiva
 *
 * @param nodo Nodo actual en la recursión
 * @param fecha Fecha a buscar
 * @param encontrados Referencia a contador de resultados encontrados
 */
static void buscarCuentasPorFechaRec(NodoPersona* nodo, const std::string& fecha, int& encontrados) {
	if (!nodo) return;
	nodo->persona->buscarPersonaPorFecha(fecha);
	// Puedes incrementar encontrados dentro de buscarPersonaPorFecha si lo deseas
	buscarCuentasPorFechaRec(nodo->siguiente, fecha, encontrados);
}

/**
 * @brief Busca cuentas por fecha de creación
 *
 * @param fecha Fecha de apertura a buscar
 */
void Banco::buscarCuentasPorFecha(const std::string& fecha) const {
	int encontrados = 0;
	forEachPersona([&](Persona* p) {
		p->buscarPersonaPorFecha(fecha);
		// Si buscarPersonaPorFecha retorna cantidad, puedes sumarla aquí
		});
	if (encontrados == 0) {
		std::cout << "No se encontraron cuentas con esa fecha en el banco.\n";
	}
}

/**
 * @brief Busca cuentas según criterios personalizados del usuario
 *
 * Permite buscar por número de cuenta, fecha de apertura, saldo,
 * tipo de cuenta o cédula del titular.
 */
void Banco::buscarCuentasPorCriterio() {
	std::string opciones[] = {
		"Numero de cuenta",
		"Fecha de apertura",
		"Saldo mayor a",
		"Tipo de cuenta",
		"Cedula de titular", // Nueva opcion
		"Cancelar"
	};
	CuentaAhorros* cuentaAhorros = nullptr;
	CuentaCorriente* cuentaCorriente = nullptr;

	int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
	int seleccion = 0;

	bool continuarBusqueda = true;

	while (continuarBusqueda) {
		// Menu de seleccion con cursor
		while (true) {
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			std::cout << "\nSeleccione el criterio de busqueda de cuentas:\n\n";
			for (int i = 0; i < numOpciones; i++) {
				if (i == seleccion)
					std::cout << " > " << opciones[i] << std::endl;
				else
					std::cout << "   " << opciones[i] << std::endl;
			}
			int tecla = _getch();
			if (tecla == 224) {
				tecla = _getch();
				if (tecla == 72) seleccion = (seleccion - 1 + numOpciones) % numOpciones;
				else if (tecla == 80) seleccion = (seleccion + 1) % numOpciones;
			}
			else if (tecla == 13) break;
		}
		if (seleccion == numOpciones - 1) return; // Cancelar

		std::string fechaIngresada;
		std::string numCuentaIngresada;
		double valorNum = 0.0;

		// Solicitar el valor segun el criterio
		if (seleccion == 0) { // Numero de cuenta
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			std::cout << "\nIngrese el numero de cuenta: ";
			numCuentaIngresada.clear();

			// Control de entrada numerica similar a otros lugares
			while (true) {
				char tecla = _getch();
				if (tecla >= '0' && tecla <= '9') {
					numCuentaIngresada += tecla;
					std::cout << tecla;
				}
				else if (tecla == 8 && !numCuentaIngresada.empty()) { // Backspace
					numCuentaIngresada.pop_back();
					std::cout << "\b \b";
				}
				else if (tecla == 13 && !numCuentaIngresada.empty()) { // Enter
					std::cout << std::endl;
					break;
				}
				else if (tecla == 27) { // ESC para cancelar
					return;
				}
			}
		}
		else if (seleccion == 1) { // Fecha de apertura
			// Implementar seleccion de fecha con cursor como en otras partes del codigo
			int dia = 1, mes = 1, anio = 2000, campo = 0;
			bool fechaSeleccionada = false;
			SYSTEMTIME st;
			GetLocalTime(&st);
			int anioActual = st.wYear, mesActual = st.wMonth, diaActual = st.wDay;

			auto esBisiesto = [](int anio) {
				return (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0));
				};
			auto diasEnMes = [&](int mes, int anio) {
				switch (mes) {
				case 2: return esBisiesto(anio) ? 29 : 28;
				case 4: case 6: case 9: case 11: return 30;
				default: return 31;
				}
				};

			while (!fechaSeleccionada) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				std::cout << "\nSeleccione la fecha de apertura, usando las flechas del teclado. ENTER para aceptar.\n";
				for (int i = 0; i < 3; ++i) {
					if (i == campo)
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					if (i == 0) std::cout << (dia < 10 ? "0" : "") << dia;
					if (i == 1) std::cout << "/" << (mes < 10 ? "0" : "") << mes;
					if (i == 2) std::cout << "/" << anio;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				}
				std::cout << std::endl;

				int tecla = _getch();
				if (tecla == 224) {
					tecla = _getch();
					switch (tecla) {
					case 75: if (campo > 0) campo--; break; // Izquierda
					case 77: if (campo < 2) campo++; break; // Derecha
					case 72: // Arriba
						if (campo == 0) { int maxDia = diasEnMes(mes, anio); dia++; if (dia > maxDia) dia = 1; }
						else if (campo == 1) { mes++; if (mes > 12) mes = 1; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
						else if (campo == 2) { anio++; if (anio > anioActual) anio = 1900; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
						break;
					case 80: // Abajo
						if (campo == 0) { int maxDia = diasEnMes(mes, anio); dia--; if (dia < 1) dia = maxDia; }
						else if (campo == 1) { mes--; if (mes < 1) mes = 12; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
						else if (campo == 2) { anio--; if (anio < 1900) anio = anioActual; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
						break;
					}
				}
				else if (tecla == 13) { // Enter
					char buffer[11];
					snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", dia, mes, anio);
					fechaIngresada = buffer;
					fechaSeleccionada = true;
				}
				else if (tecla == 27) { // ESC
					return;
				}
			}
		}
		else if (seleccion == 2) { // Saldo mayor a
			std::cout << "Ingrese el saldo minimo: ";

			// Implementar validacion similar a otras partes
			std::string entrada;
			bool tienePunto = false;
			int digitosDecimales = 0;

			while (true) {
				char tecla = _getch();

				// ESC para cancelar
				if (tecla == 27) {
					return;
				}

				// ENTER
				if (tecla == 13) {
					if (!entrada.empty()) {
						if (Validar::ValidarNumeroConDosDecimales(entrada)) {
							try {
								double valor = std::stod(entrada);
								if (valor >= 0) {
									std::cout << std::endl;
									break;
								}
							}
							catch (...) {}
						}
					}
					std::cout << "\nFormato invalido. Ingrese nuevamente: ";
					entrada.clear();
					tienePunto = false;
					digitosDecimales = 0;
					continue;
				}

				// BACKSPACE
				if (tecla == 8 && !entrada.empty()) {
					if (entrada.back() == '.') {
						tienePunto = false;
					}
					else if (tienePunto && digitosDecimales > 0) {
						--digitosDecimales;
					}
					entrada.pop_back();
					std::cout << "\b \b";
					continue;
				}

				// Ignoramos teclas especiales
				if (tecla == 0 || tecla == -32) {
					int teclaEspecial = _getch();
					(void)teclaEspecial;
					continue;
				}

				// Digitos
				if (isdigit(tecla)) {
					if (tienePunto && digitosDecimales == 2) continue;
					if (tienePunto) ++digitosDecimales;
					entrada += tecla;
					std::cout << tecla;
				}
				// Punto decimal
				else if (tecla == '.' && !tienePunto && !entrada.empty()) {
					tienePunto = true;
					entrada += tecla;
					std::cout << tecla;
				}
			}
		}

		// A continuacion, modificamos como se muestran los resultados de busqueda:

		int totalCuentasEncontradas = 0;

		if (seleccion == 4) { // Si es busqueda por cedula, ya esta implementado correctamente
			bool encontrado = false;
			NodoPersona* actual = listaPersonas;

			while (actual) {
				if (actual->persona && actual->persona->isValidInstance() &&
					actual->persona->getCedula() == numCuentaIngresada) {

					// Encontramos la persona, mostrar sus datos y cuentas
					cuentaAhorros = actual->persona->getCabezaAhorros();
					cuentaCorriente = actual->persona->getCabezaCorriente();

					// Mostrar informacion basica de la persona
					std::cout << "\n===== DATOS DEL TITULAR =====\n";
					std::cout << "Cedula: " << actual->persona->getCedula() << std::endl;
					std::cout << "Nombre: " << actual->persona->getNombres() << " "
						<< actual->persona->getApellidos() << std::endl;
					std::cout << "Fecha de nacimiento: " << actual->persona->getFechaNacimiento() << std::endl;
					std::cout << "Correo: " << actual->persona->getCorreo() << std::endl;
					std::cout << "Direccion: " << actual->persona->getDireccion() << std::endl;

					// Mostrar todas sus cuentas
					std::cout << "\n===== CUENTAS DE AHORRO =====\n";
					CuentaAhorros* cuentaAhorros = actual->persona->getCabezaAhorros();
					int contadorAhorros = 0;
					if (!cuentaAhorros) {
						std::cout << "  No tiene cuentas de ahorro.\n";
					}
					while (cuentaAhorros) {
						if (cuentaAhorros->getCuentaAhorros()) {
							std::cout << "\nCUENTA DE AHORROS #" << ++contadorAhorros << std::endl;
							cuentaAhorros->getCuentaAhorros()->mostrarInformacion(actual->persona->getCedula(), false);
							totalCuentasEncontradas++;
						}
						cuentaAhorros = cuentaAhorros->getSiguiente();
					}

					std::cout << "\n===== CUENTAS CORRIENTES =====\n";
					CuentaCorriente* cuentaCorriente = actual->persona->getCabezaCorriente();
					int contadorCorrientes = 0;
					if (!cuentaCorriente) {
						std::cout << "  No tiene cuentas corrientes.\n";
					}
					while (cuentaCorriente) {
						if (cuentaCorriente->getCuentaCorriente()) {
							std::cout << "\nCUENTA CORRIENTE #" << ++contadorCorrientes << std::endl;
							cuentaCorriente->getCuentaCorriente()->mostrarInformacion(actual->persona->getCedula(), false);
							totalCuentasEncontradas++;
						}
						cuentaCorriente = cuentaCorriente->getSiguiente();
					}

					encontrado = true;
					break;
				}
				actual = actual->siguiente;
			}

			if (!encontrado) {
				std::cout << "No se encontro ninguna persona con la cedula: " << numCuentaIngresada << std::endl;
			}
			else {
				std::cout << "\nTotal de cuentas encontradas: " << totalCuentasEncontradas << std::endl;
			}
		}
		else {
			// Para otros criterios, modificamos el metodo buscarPersonaPorCriterio en la clase Persona
			// para que muestre los datos del titular junto con cada cuenta.

			// Recorremos todas las personas y buscamos segun criterio
			NodoPersona* actual = listaPersonas;
			while (actual) {
				if (actual->persona && actual->persona->isValidInstance()) {
					// Contar las cuentas encontradas por persona
					int cuentasEncontradas = actual->persona->buscarPersonaPorCriterio(opciones[seleccion], numCuentaIngresada, fechaIngresada, valorNum);
					totalCuentasEncontradas += cuentasEncontradas;
				}
				actual = actual->siguiente;
			}

			if (totalCuentasEncontradas == 0) {
				std::cout << "\nNo se encontraron cuentas que cumplan con el criterio seleccionado." << std::endl;
				system("pause");
				continuarBusqueda = true; // Continuar buscando
			}
			else {
				std::cout << "\nTotal de cuentas encontradas: " << totalCuentasEncontradas << std::endl;
				system("pause");
				continuarBusqueda = false; // Salir del bucle de busqueda
			}
		}
	}
}

/**
 * @brief Realiza una transferencia de fondos entre dos cuentas
 *
 * Gestiona la búsqueda de cuentas origen y destino, validación de fondos,
 * y ejecución de la transferencia con sus respectivas actualizaciones de saldo.
 */
void Banco::realizarTransferencia() {
	// Verificar que existan personas con cuentas
	if (!listaPersonas) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "No hay cuentas registradas en el sistema.\n";
		system("pause");
		return;
	}

	// Variables para almacenar referencias a las cuentas

	CuentaAhorros* cuentaAhorrosOrigen = nullptr;
	CuentaCorriente* cuentaCorrienteOrigen = nullptr;

	CuentaAhorros* cuentaAhorrosDestino = nullptr;
	CuentaCorriente* cuentaCorrienteDestino = nullptr;

	Persona* personaOrigen = nullptr;
	Persona* personaDestino = nullptr;

	std::string numCuentaOrigen, numCuentaDestino;
	bool esAhorrosOrigen = false, esAhorrosDestino = false;
	bool cuentaOrigenEncontrada = false, cuentaDestinoEncontrada = false;

	// 1. Obtener la cuenta de origen
	while (!cuentaOrigenEncontrada) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "\n=== TRANSFERENCIA BANCARIA ===\n\n";
		std::cout << "Ingrese el numero de cuenta de origen (o ESC para cancelar): ";

		numCuentaOrigen.clear();
		while (true) {
			char tecla = _getch();
			if (tecla == 27) { // ESC
				std::cout << "\nOperacion cancelada por el usuario.\n";
				system("pause");
				return;
			}
			else if (tecla >= '0' && tecla <= '9') {
				numCuentaOrigen += tecla;
				std::cout << tecla;
			}
			else if (tecla == 8 && !numCuentaOrigen.empty()) { // Backspace
				numCuentaOrigen.pop_back();
				std::cout << "\b \b";
			}
			else if (tecla == 13 && !numCuentaOrigen.empty()) { // Enter
				std::cout << std::endl;
				break;
			}
		}

		// Buscar la cuenta de origen en todas las personas
		NodoPersona* actual = listaPersonas;
		while (actual && !cuentaOrigenEncontrada) {
			if (!actual->persona) {
				actual = actual->siguiente;
				continue;
			}

			// Buscar en cuentas de ahorro
			cuentaAhorrosOrigen = actual->persona->getCabezaAhorros();
			while (cuentaAhorrosOrigen && !cuentaOrigenEncontrada) {
				if (cuentaAhorrosOrigen->getCuentaAhorros() && cuentaAhorrosOrigen->getNumeroCuenta() == numCuentaOrigen) {
					cuentaAhorrosOrigen = cuentaAhorrosOrigen->getCuentaAhorros();
					personaOrigen = actual->persona;
					cuentaOrigenEncontrada = true;
					esAhorrosOrigen = true;
					break;
				}
				cuentaAhorrosOrigen = cuentaAhorrosOrigen->getSiguiente();
			}

			// Si no se encontro en ahorro, buscar en corriente
			if (!cuentaOrigenEncontrada) {
				cuentaCorrienteOrigen = actual->persona->getCabezaCorriente();
				while (cuentaCorrienteOrigen && !cuentaOrigenEncontrada) {
					if (cuentaCorrienteOrigen->getCuentaCorriente() && cuentaCorrienteOrigen->getNumeroCuenta() == numCuentaOrigen) {
						cuentaCorrienteOrigen = cuentaCorrienteOrigen->getCuentaCorriente();
						personaOrigen = actual->persona;
						cuentaOrigenEncontrada = true;
						esAhorrosOrigen = false;
						break;
					}
					cuentaCorrienteOrigen = cuentaCorrienteOrigen->getSiguiente();
				}
			}

			actual = actual->siguiente;
		}

		if (!cuentaOrigenEncontrada) {
			// Configuracion de opciones para el menu de confirmacion
			const char* opciones[] = { "Si", "No" };
			int seleccion = 1; // Por defecto seleccionar "Si"
			int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
			bool continuar = true;

			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			std::cout << "Cuenta de origen no encontrada.\n";
			std::cout << "Desea intentar con otro numero?\n\n";

			// Obtener handle para manipular la consola
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(hConsole, &csbi);
			int posY = csbi.dwCursorPosition.Y;

			// Bucle de manejo del menu
			while (continuar) {
				// Mostrar las opciones
				for (int i = 0; i < numOpciones; i++) {
					// Posicionar el cursor
					COORD pos = { 5, static_cast<SHORT>(posY + i) };
					SetConsoleCursorPosition(hConsole, pos);

					// Resaltar la opcion seleccionada
					if (i == seleccion) {
						SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
						std::cout << " > " << opciones[i] << " < ";
					}
					else {
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
						std::cout << "   " << opciones[i] << "   ";
					}
				}

				// Restaurar atributos por defecto
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

				// Capturar tecla
				int tecla = _getch();

				// Procesar la tecla
				if (tecla == 224) {
					tecla = _getch();
					if (tecla == 75 && seleccion > 0) --seleccion;
					else if (tecla == 77 && seleccion < numOpciones - 1) ++seleccion;
				}
				else if (tecla == 13) { // Enter
					continuar = false;
				}
				else if (tecla == 27) { // ESC - cancelar
					seleccion = 1; // Seleccionar "No"
					continuar = false;
				}
			}

			// Procesar la seleccion
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << "\n\n";

			if (seleccion == 1) { // "No" seleccionado
				std::cout << "Operacion cancelada.\n";
				system("pause");
				return;
			}
			// Si selecciono "Si", continuara el flujo del programa
		}
	}

	// Mostrar informacion de la cuenta origen
	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "\n=== TRANSFERENCIA BANCARIA ===\n\n";
	std::cout << "CUENTA ORIGEN:\n";
	if (esAhorrosOrigen) {
		cuentaAhorrosOrigen->mostrarInformacion(personaOrigen->getCedula(), false); // Pasar false para no limpiar pantalla
	}
	else {
		cuentaCorrienteOrigen->mostrarInformacion(personaOrigen->getCedula(), false); // Pasar false para no limpiar pantalla
	}
	std::cout << "\nTitular: " << personaOrigen->getNombres() << " " << personaOrigen->getApellidos() << "\n\n";

	// 2. Obtener la cuenta de destino
	while (!cuentaDestinoEncontrada) {
		std::cout << "\nIngrese el numero de cuenta de destino (o ESC para cancelar): ";

		numCuentaDestino.clear();
		while (true) {
			char tecla = _getch();
			if (tecla == 27) { // ESC
				std::cout << "\nOperacion cancelada por el usuario.\n";
				system("pause");
				return;
			}
			else if (tecla >= '0' && tecla <= '9') {
				numCuentaDestino += tecla;
				std::cout << tecla;
			}
			else if (tecla == 8 && !numCuentaDestino.empty()) { // Backspace
				numCuentaDestino.pop_back();
				std::cout << "\b \b";
			}
			else if (tecla == 13 && !numCuentaDestino.empty()) { // Enter
				std::cout << std::endl;
				break;
			}
		}

		// No permitir transferir a la misma cuenta
		if (numCuentaDestino == numCuentaOrigen) {
			std::cout << "No se puede transferir a la misma cuenta. Intente con otra.\n";
			continue;
		}

		// Buscar la cuenta de destino en todas las personas
		NodoPersona* actual = listaPersonas;
		while (actual && !cuentaDestinoEncontrada) {
			if (!actual->persona) {
				actual = actual->siguiente;
				continue;
			}

			// Buscar en cuentas de ahorro
			cuentaAhorrosDestino = actual->persona->getCabezaAhorros();
			while (cuentaAhorrosDestino && !cuentaDestinoEncontrada) {
				if (cuentaAhorrosDestino->getCuentaAhorros() && cuentaAhorrosDestino->getNumeroCuenta() == numCuentaDestino) {
					cuentaAhorrosDestino = cuentaAhorrosDestino->getCuentaAhorros();
					personaDestino = actual->persona;
					cuentaDestinoEncontrada = true;
					esAhorrosDestino = true;
					break;
				}
				cuentaAhorrosDestino = cuentaAhorrosDestino->getSiguiente();
			}

			// Si no se encontro en ahorro, buscar en corriente
			if (!cuentaDestinoEncontrada) {
				cuentaCorrienteDestino = actual->persona->getCabezaCorriente();
				while (cuentaCorrienteDestino && !cuentaDestinoEncontrada) {
					if (cuentaCorrienteDestino->getCuentaCorriente() && cuentaCorrienteDestino->getNumeroCuenta() == numCuentaDestino) {
						cuentaCorrienteDestino = cuentaCorrienteDestino->getCuentaCorriente();
						personaDestino = actual->persona;
						cuentaDestinoEncontrada = true;
						esAhorrosDestino = false;
						break;
					}
					cuentaCorrienteDestino = cuentaCorrienteDestino->getSiguiente();
				}
			}

			actual = actual->siguiente;
		}

		if (!cuentaDestinoEncontrada) {
			std::cout << "Cuenta de destino no encontrada. \nDesea intentar con otro numero? (S/N): ";
			char respuesta = _getch();
			if (respuesta == 'N' || respuesta == 'n') {
				std::cout << "\nOperacion cancelada.\n";
				system("pause");
				return;
			}
		}
	}

	// Mostrar informacion de la cuenta destino
	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "\n=== TRANSFERENCIA BANCARIA ===\n\n";
	std::cout << "CUENTA ORIGEN:\n";
	if (esAhorrosOrigen) {
		cuentaAhorrosOrigen->mostrarInformacion(personaOrigen->getCedula(), false);
	}
	else {
		cuentaCorrienteOrigen->mostrarInformacion(personaOrigen->getCedula(), false);
	}
	std::cout << "\nTitular: " << personaOrigen->getNombres() << " " << personaOrigen->getApellidos() << "\n\n";

	std::cout << "\nCUENTA DESTINO:\n";
	if (esAhorrosDestino) {
		cuentaAhorrosDestino->mostrarInformacion(personaDestino->getCedula(), false);
	}
	else {
		cuentaCorrienteDestino->mostrarInformacion(personaDestino->getCedula(), false);
	}
	std::cout << "\nTitular: " << personaDestino->getNombres() << " " << personaDestino->getApellidos() << "\n\n";

	// 3. Solicitar monto a transferir
	double montoEnCentavos = 0;
	std::string entrada;
	bool tienePunto = false;
	int digitosDecimales = 0;

	std::cout << "Ingrese el monto a transferir (ejemplo: 100.50): ";
	while (true) {
		char tecla = _getch();

		// ESC para cancelar
		if (tecla == 27) {
			std::cout << "\nOperacion cancelada por el usuario.\n";
			system("pause");
			return;
		}

		// Detecta Ctrl+V (para pegar)
		if (tecla == 22) {
			std::string pegado = Validar::leerDesdePortapapeles();
			if (Validar::ValidarNumeroConDosDecimales(pegado)) {
				try {
					double valor = std::stod(pegado);
					if (valor > 0) {
						entrada = pegado;
						std::cout << "\rIngrese el monto a transferir (ejemplo: 100.50): " << entrada;
						std::cout << std::endl;
						montoEnCentavos = static_cast<double>(valor);
						break;
					}
				}
				catch (...) {}
			}
			std::cout << "\nFormato invalido. Ingrese nuevamente (ejemplo: 100.50): ";
			entrada.clear();
			tienePunto = false;
			digitosDecimales = 0;
			continue;
		}

		// ENTER
		if (tecla == 13) {
			if (!entrada.empty()) {
				if (Validar::ValidarNumeroConDosDecimales(entrada)) {
					try {
						double valor = std::stod(entrada);
						if (valor > 0) {
							std::cout << std::endl;
							montoEnCentavos = static_cast<double>(valor);
							break;
						}
					}
					catch (...) {}
				}
			}
			std::cout << "\nFormato invalido o monto menor o igual a cero. \nIngrese nuevamente (ejemplo: 100.50): ";
			entrada.clear();
			tienePunto = false;
			digitosDecimales = 0;
			continue;
		}

		// BACKSPACE
		if (tecla == 8 && !entrada.empty()) {
			if (entrada.back() == '.') {
				tienePunto = false;
			}
			else if (tienePunto && digitosDecimales > 0) {
				--digitosDecimales;
			}
			entrada.pop_back();
			std::cout << "\rIngrese el monto a transferir (ejemplo: 100.50): " << std::string(40, ' ') << "\r";
			std::cout << "Ingrese el monto a transferir (ejemplo: 100.50): " << entrada;
			continue;
		}

		// Ignora teclas especiales (como flechas, etc.)


		// Digitos
		if (isdigit(tecla)) {
			if (tienePunto && digitosDecimales == 2) continue;
			if (tienePunto) ++digitosDecimales;
			entrada += tecla;
			std::cout << tecla;
		}
		// Punto decimal
		else if (tecla == '.' && !tienePunto && !entrada.empty()) {
			tienePunto = true;
			entrada += tecla;
			std::cout << tecla;
		}
	}

	// 4. Verificar fondos suficientes
	double saldoOrigen = esAhorrosOrigen ? cuentaAhorrosOrigen->consultarSaldo() : cuentaCorrienteOrigen->consultarSaldo();

	if (montoEnCentavos > saldoOrigen) { // Comparar directamente sin convertir a centavos
		std::cout << "Fondos insuficientes para realizar la transferencia.\n";
		system("pause");
		return;
	}

	// 5. Confirmar transferencia
	std::cout << "Confirmar transferencia de $"
		<< (esAhorrosOrigen ? cuentaAhorrosOrigen->formatearConComas(montoEnCentavos) :
			formatearConComas(montoEnCentavos))
		<< "? (S/N): ";

	char confirmacion = _getch();
	if (confirmacion != 'S' && confirmacion != 's') {
		std::cout << "\nOperacion cancelada.\n";
		system("pause");
		return;
	}

	// 6. Realizar la transferencia
	if (esAhorrosOrigen) {
		cuentaAhorrosOrigen->retirar(montoEnCentavos); // Usar el valor directamente
	}
	else {
		cuentaCorrienteOrigen->retirar(montoEnCentavos); // Usar el valor directamente
	}

	if (esAhorrosDestino) {
		cuentaAhorrosDestino->depositar(montoEnCentavos); // Usar el valor directamente
	}
	else {
		cuentaCorrienteDestino->depositar(montoEnCentavos); // Usar el valor directamente
	}

	// 7. Mostrar confirmacion
	std::cout << "\nTransferencia realizada con exito!\n\n";
	std::cout << "NUEVO SALDO EN SU CUENTA: $"
		<< (esAhorrosOrigen ? cuentaAhorrosOrigen->formatearConComas(cuentaAhorrosOrigen->consultarSaldo()) :
			formatearConComas(cuentaCorrienteOrigen->consultarSaldo()))
		<< "\n";

	system("pause");
}

/**
 * @brief Formatea un valor monetario con separadores de miles
 *
 * @param valorEnCentavos Valor monetario a formatear
 * @return Cadena formateada con separadores de miles y dos decimales
 */
std::string Banco::formatearConComas(double valorEnCentavos) const {
	std::ostringstream oss;
	oss.imbue(std::locale(""));
	oss << std::fixed << std::setprecision(2) << valorEnCentavos;
	return oss.str();
}

/**
 * @brief Obtiene la ruta del directorio del escritorio del usuario
 *
 * Crea una carpeta "BancoApp" en el escritorio si no existe, para
 * almacenar los archivos generados por la aplicación.
 *
 * @return Ruta completa de la carpeta de la aplicación
 */
std::string Banco::obtenerRutaEscritorio() const {
	PWSTR path = NULL;
	std::string rutaEscritorio = "";

	// Obtener la ruta del escritorio
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &path))) {
		// Convertir de PWSTR a std::string
		_bstr_t b(path);
		rutaEscritorio = (char*)b;

		// Liberar la memoria asignada por SHGetKnownFolderPath
		CoTaskMemFree(path);
	}
	else {
		// Si falla, usar una ubicacion alternativa como respaldo
		rutaEscritorio = "."; // Directorio actual como respaldo
	}

	// Asegurar que la ruta termine con una barra diagonal
	if (!rutaEscritorio.empty() && rutaEscritorio.back() != '\\') {
		rutaEscritorio += '\\';
	}

	// Crear una subcarpeta especifica para la aplicacion
	rutaEscritorio += "BancoApp\\";

	// Asegurar que la carpeta existe
	std::string comando = "if not exist \"" + rutaEscritorio + "\" mkdir \"" + rutaEscritorio + "\"";
	system(comando.c_str());

	return rutaEscritorio;
}

/**
* @brief Verifica las cuentas del banco
*/
bool Banco::verificarCuentasBanco() const
{
	// Verificar si hay cuentas
	if (this->getListaPersonas() == nullptr) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "No hay cuentas registradas en el banco. Cree una cuenta primero.\n";
		system("pause");
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		return false;
	}
	return true;
}

/**
 * @brief Muestra el submenú de cuentas bancarias
 *
 * Submenú de cuentas bancarias para separar las opciones de gestión de cuentas
 */
void Banco::subMenuCuentasBancarias()
{
	// Submenu para operaciones de cuenta
	std::string opcionesCuenta[] = { "Depositar", "Retirar", "Consultar saldo", "Cancelar" };
	int numOpcionesCuenta = sizeof(opcionesCuenta) / sizeof(opcionesCuenta[0]);
	int selCuenta = 0;

	while (true) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "Seleccione la operacion a realizar:\n\n";
		for (int i = 0; i < numOpcionesCuenta; i++) {
			if (i == selCuenta)
				std::cout << " > " << opcionesCuenta[i] << std::endl;
			else
				std::cout << "   " << opcionesCuenta[i] << std::endl;
		}
		int teclaCuenta = _getch();
		if (teclaCuenta == 224) {
			teclaCuenta = _getch();
			if (teclaCuenta == 72) // Flecha arriba
				selCuenta = (selCuenta - 1 + numOpcionesCuenta) % numOpcionesCuenta;
			else if (teclaCuenta == 80) // Flecha abajo
				selCuenta = (selCuenta + 1) % numOpcionesCuenta;
		}
		else if (teclaCuenta == 13) // Enter
			break;
		else if (teclaCuenta == 27) { // ESC
			selCuenta = 3; // Cancelar
			break;
		}
	}

	// Buscar cuenta para realizar la operacion
	CuentaAhorros* cuentaAhorros = nullptr;
	CuentaCorriente* cuentaCorriente = nullptr;
	std::string cedula;

	if (!buscarCuentaParaOperacion(*this, cuentaAhorros, cuentaCorriente, cedula)) {
		return; // Si no se encontró ninguna cuenta o se canceló la operación
	}

	// Mostrar información de la cuenta seleccionada
	if (cuentaAhorros != nullptr) {
		std::cout << "CUENTA DE AHORROS: " << cuentaAhorros->getNumeroCuenta() << "\n";
	}
	else {
		std::cout << "CUENTA CORRIENTE: " << cuentaCorriente->getNumeroCuenta() << "\n";
	}

	// Realizar la operación seleccionada usando switch-case
	switch (selCuenta) {
	case 0: // Depositar
	{
		Utilidades::mostrarCursor();
		std::cout << "\n\nDEPOSITO\n";
		std::cout << "Ingrese el monto a depositar: ";

		// Variables para la entrada manual controlada
		std::string entrada;
		bool tienePunto = false;
		int digitosDecimales = 0;

		while (true) {
			char tecla = _getch();

			// Permitir solo digitos
			if (tecla >= '0' && tecla <= '9') {
				// Limitar a 2 decimales despues del punto
				if (tienePunto && digitosDecimales >= 2) continue;

				entrada += tecla;
				std::cout << tecla;

				// Contar digitos despues del punto decimal
				if (tienePunto) digitosDecimales++;
			}
			// Permitir solo un punto decimal
			else if (tecla == '.' && !tienePunto && !entrada.empty()) {
				tienePunto = true;
				entrada += tecla;
				std::cout << tecla;
			}
			// Permitir borrar
			else if (tecla == 8 && !entrada.empty()) { // Backspace
				if (entrada.back() == '.') {
					tienePunto = false;
				}
				else if (tienePunto && digitosDecimales > 0) {
					--digitosDecimales;
				}
				entrada.pop_back();
				std::cout << "\b \b"; // Retrocede, imprime espacio y retrocede
			}
			// Finalizar con Enter si hay algo ingresado
			else if (tecla == 13 && !entrada.empty()) { // Enter
				std::cout << std::endl;
				break;
			}
		}

		// Convertir la entrada a double con manejo de excepciones
		try {
			double monto = std::stod(entrada);

			if (monto <= 0) {
				std::cout << "El monto debe ser mayor a cero.\n";
			}
			else {
				double montoEnCentavos = static_cast<double>(monto);
				if (cuentaAhorros != nullptr) {
					cuentaAhorros->depositar(montoEnCentavos);
					std::cout << "\nDeposito realizado con exito.\n";
					std::cout << "Nuevo saldo: $" << cuentaAhorros->formatearSaldo() << std::endl;
				}
				else {
					cuentaCorriente->depositar(montoEnCentavos);
					std::cout << "\nDeposito realizado con exito.\n";
					std::cout << "Nuevo saldo: $" << cuentaCorriente->formatearSaldo() << std::endl;
				}
			}
			Utilidades::ocultarCursor();
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		system("pause");
		break;
	}

	case 1: // Retirar
	{
		std::cout << "\n\nRETIRO\n\n";
		Utilidades::mostrarCursor();
		double saldoActual = 0;

		if (cuentaAhorros != nullptr) {
			saldoActual = cuentaAhorros->consultarSaldo();
			std::cout << "Saldo disponible: $" << cuentaAhorros->formatearSaldo() << std::endl;
		}
		else {
			saldoActual = cuentaCorriente->consultarSaldo();
			std::cout << "Saldo disponible: $" << cuentaCorriente->formatearSaldo() << std::endl;
		}

		std::cout << "Ingrese el monto a retirar: ";
		double monto;
		std::cin >> monto;

		if (monto <= 0) {
			std::cout << "El monto debe ser mayor a cero.\n";
		}
		else {
			double montoEnCentavos = static_cast<double>(monto);
			if (cuentaAhorros != nullptr) {
				cuentaAhorros->retirar(montoEnCentavos);
				std::cout << "Retiro realizado con exito.\n";
				std::cout << "Nuevo saldo: $" << cuentaAhorros->formatearSaldo() << std::endl;
			}
			else {
				cuentaCorriente->retirar(montoEnCentavos);
				std::cout << "Retiro realizado con exito.\n";
				std::cout << "Nuevo saldo: $" << cuentaCorriente->formatearSaldo() << std::endl;
			}
		}
		Utilidades::ocultarCursor();
		system("pause");
		break;
	}

	case 2: // Consultar saldo
	{
		std::cout << "\nCONSULTA DE SALDO\n\n";

		if (cuentaAhorros != nullptr) {
			cuentaAhorros->mostrarInformacion(cedula, false);
		}
		else {
			cuentaCorriente->mostrarInformacion(cedula, false);
		}
		// No necesitamos system("pause") aquí ya que mostrarInformacion() lo incluye
		break;
	}
	}
}

/**
 * @brief Busca una cuenta bancaria para realizar operaciones
 *
 * Permite al usuario buscar una cuenta por cédula o número de cuenta,
 * y devuelve la cuenta encontrada para realizar operaciones sobre ella.
 *
 * @param banco Referencia al objeto Banco donde buscar la cuenta
 * @param cuentaAhorros Referencia a puntero que se actualizará con la cuenta de ahorros encontrada
 * @param cuentaCorriente Referencia a puntero que se actualizará with la cuenta corriente encontrada
 * @param cedula Referencia a string que se actualizará con la cédula del titular
 * @return bool true si se encontró una cuenta válida, false en caso contrario
 */
bool Banco::buscarCuentaParaOperacion(Banco& banco, CuentaAhorros*& cuentaAhorros, CuentaCorriente*& cuentaCorriente, std::string& cedula) {
	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "\n===== OPERACIONES DE CUENTA =====\n\n";

	// Menu de seleccion: buscar por cedula o numero
	std::string opciones[] = { "Buscar por cedula", "Buscar por numero de cuenta", "Cancelar" };
	int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
	int seleccion = 0;

	while (true) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "Seleccione metodo de busqueda:\n\n";
		for (int i = 0; i < numOpciones; i++) {
			if (i == seleccion)
				std::cout << " > " << opciones[i] << std::endl;
			else
				std::cout << "   " << opciones[i] << std::endl;
		}
		int tecla = _getch();
		if (tecla == 224) {
			tecla = _getch();
			if (tecla == 72) seleccion = (seleccion - 1 + numOpciones) % numOpciones;
			else if (tecla == 80) seleccion = (seleccion + 1) % numOpciones;
		}
		else if (tecla == 13) break; // ENTER
		else if (tecla == 27) return false; // ESC
	}

	if (seleccion == 2) return false; // Cancelar

	if (seleccion == 0) { // Buscar por cedula
		cedula.clear();
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "Ingrese la cedula (10 digitos): ";
		int digitos = 0;
		while (true) {
			char tecla = _getch();
			if ((tecla >= '0' && tecla <= '9') && digitos < 10) {
				cedula += tecla;
				digitos++;
				std::cout << tecla;
			}
			else if (tecla == 8 && !cedula.empty()) { // Backspace
				cedula.pop_back();
				digitos--;
				std::cout << "\b \b";
			}
			else if (tecla == 13 && digitos == 10) { // Enter y cedula completa
				std::cout << std::endl;
				break;
			}
			else if (tecla == 27) return false; // ESC
		}

		// Buscar por cedula usando la clase Banco
		NodoPersona* actual = banco.getListaPersonas();
		bool encontrado = false;

		while (actual) {
			if (actual->persona && actual->persona->getCedula() == cedula) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				std::cout << "Titular: " << actual->persona->getNombres() << " " << actual->persona->getApellidos() << "\n\n";

				// Listar cuentas disponibles
				std::vector<std::pair<bool, void*>> cuentas; // true=ahorro, false=corriente

				// Cuentas de ahorro
				cuentaAhorros = actual->persona->getCabezaAhorros(); // Obtener cuenta de ahorro principal
				int contador = 1;
				while (cuentaAhorros) {
					if (cuentaAhorros->getCuentaAhorros()) {
						std::cout << contador << ". Cuenta de Ahorro: "
							<< cuentaAhorros->getCuentaAhorros()->getNumeroCuenta() << "\n";
						cuentas.push_back({ true, cuentaAhorros->getCuentaAhorros() });
						contador++;
					}
					cuentaAhorros = cuentaAhorros->getSiguiente();
				}

				// Cuentas corrientes
				cuentaCorriente = actual->persona->getCabezaCorriente(); // Obtener cuenta corriente principal
				while (cuentaCorriente) {
					if (cuentaCorriente->getCuentaCorriente()) {
						std::cout << contador << ". Cuenta Corriente: "
							<< cuentaCorriente->getCuentaCorriente()->getNumeroCuenta() << "\n";
						cuentas.push_back({ false, cuentaCorriente->getCuentaCorriente() });
						contador++;
					}
					cuentaCorriente = cuentaCorriente->getSiguiente();
				}

				if (cuentas.empty()) {
					std::cout << "El titular no tiene cuentas asociadas.\n";
					system("pause");
					return false;
				}

				// Seleccionar cuenta
				std::cout << "\nSeleccione una cuenta (1-" << cuentas.size() << "): ";
				int selCuenta;
				std::cin >> selCuenta;

				if (selCuenta < 1 || selCuenta > static_cast<int>(cuentas.size())) {
					std::cout << "Opcion invalida.\n";
					system("pause");
					return false;
				}

				// Obtener cuenta seleccionada
				auto& cuentaSelec = cuentas[static_cast<std::vector<std::pair<bool, void*>, std::allocator<std::pair<bool, void*>>>::size_type>(selCuenta) - 1];
				if (cuentaSelec.first) { // Cuenta de ahorro
					cuentaAhorros = static_cast<CuentaAhorros*>(cuentaSelec.second);
					cuentaCorriente = nullptr;
				}
				else { // Cuenta corriente
					cuentaAhorros = nullptr;
					cuentaCorriente = static_cast<CuentaCorriente*>(cuentaSelec.second);
				}

				encontrado = true;
				break;
			}
			actual = actual->siguiente;
		}

		if (!encontrado) {
			std::cout << "No se encontro ninguna persona con esa cedula.\n";
			system("pause");
			return false;
		}
	}
	else { // Buscar por numero de cuenta
		std::string numCuenta;
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "\nIngrese el numero de cuenta: ";
		while (true) {
			char tecla = _getch();
			if (tecla >= '0' && tecla <= '9') {
				numCuenta += tecla;
				std::cout << tecla;
			}
			else if (tecla == 8 && !numCuenta.empty()) { // Backspace
				numCuenta.pop_back();
				std::cout << "\b \b";
			}
			else if (tecla == 13 && !numCuenta.empty()) { // Enter
				std::cout << std::endl;
				break;
			}
			else if (tecla == 27) return false; // ESC
		}

		// Buscar la cuenta por numero
		NodoPersona* actual = banco.getListaPersonas();
		bool encontrado = false;

		while (actual && !encontrado) {
			if (actual->persona) {
				// Buscar en cuentas de ahorro
				cuentaAhorros = actual->persona->getCabezaAhorros(); // Obtener cuenta de ahorro principal
				while (cuentaAhorros && !encontrado) {
					if (cuentaAhorros->getCuentaAhorros() && cuentaAhorros->getCuentaAhorros()->getNumeroCuenta() == numCuenta) {
						cuentaAhorros = cuentaAhorros->getCuentaAhorros();
						cuentaCorriente = nullptr;
						cedula = actual->persona->getCedula();
						encontrado = true;
						break;
					}
					cuentaAhorros = cuentaAhorros->getSiguiente();
				}

				// Si no se encontro, buscar en cuentas corrientes
				if (!encontrado) {
					cuentaCorriente = actual->persona->getCabezaCorriente(); // Obtener cuenta corriente principal
					while (cuentaCorriente && !encontrado) {
						if (cuentaCorriente->getCuentaCorriente() && cuentaCorriente->getCuentaCorriente()->getNumeroCuenta() == numCuenta) {
							cuentaAhorros = nullptr;
							cuentaCorriente = cuentaCorriente->getCuentaCorriente();
							cedula = actual->persona->getCedula();
							encontrado = true;
							break;
						}
						cuentaCorriente = cuentaCorriente->getSiguiente();
					}
				}
			}
			if (!encontrado) {
				actual = actual->siguiente;
			}
		}

		if (!encontrado) {
			std::cout << "No se encontro ninguna cuenta con ese numero.\n";
			system("pause");
			return false;
		}
	}

	return true;
}

/**
* @brief Itera sobre todas las personas en el banco y aplica una funcion a cada una
* @param funcion Funcion a aplicar a cada Persona
*/
void Banco::forEachPersona(const std::function<void(Persona*)>& funcion) const {
	NodoPersona* actual = listaPersonas;
	while (actual) {
		if (actual->persona) {
			funcion(actual->persona);
		}
		actual = actual->siguiente;
	}
}

/**
 * @brief Itera sobre todos los nodos de Persona y aplica una funcion a cada NodoPersona
 * @param funcion Funcion a aplicar a cada NodoPersona
 */
void Banco::forEachNodoPersona(const std::function<void(NodoPersona*)>& funcion) const {
	NodoPersona* actual = listaPersonas;
	while (actual) {
		funcion(actual);
		actual = actual->siguiente;
	}
}

