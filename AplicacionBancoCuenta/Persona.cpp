/**
 * @file Persona.cpp
 * @brief Implementación refactorizada de la clase Persona aplicando SOLID y código limpio
 */

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <functional>
#include <iterator>
#include <unordered_map>
#include <memory>
#include "Validar.h"
#include "Fecha.h"
#include "Persona.h"
#include "PersonaValidator.h"
#include "PersonaDataProcessor.h"
#include "Utilidades.h"
#include "_BaseDatosPersona.h"
#include "ConexionMongo.h"

 /**
  * @namespace PersonaUI
  * @brief Proporciona elementos de interfaz de usuario para la clase Persona
  *
  * Este namespace contiene funciones lambda para la interacción con el usuario,
  * como selectores y validadores de entrada.
  */
namespace PersonaUI {
	/**
	 * @brief Lambda para mostrar un selector de opciones Sí/No con navegación por teclado
	 *
	 * @param mensaje Mensaje a mostrar al usuario antes de las opciones
	 * @return bool true si selecciona "Sí", false si selecciona "No"
	 */
	const std::function<bool(const std::string&)> seleccionarSiNo = [](const std::string& mensaje) -> bool {
		int seleccion = 0;
		int tecla = 0;
		// Variable para controlar si ya se mostró el mensaje
		bool mensajeMostrado = false;
		// Variable para guardar la posición de las opciones
		COORD posicionOpciones = { 0, 0 };

		do {
			Utilidades::ocultarCursor(); // Oculta el cursor para una mejor experiencia visual

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

			// Solo mostramos el mensaje la primera vez y guardamos la posición para las opciones
			if (!mensajeMostrado) {
				cout << mensaje << endl;
				mensajeMostrado = true;

				// Guardamos la posición donde aparecerán las opciones (2 líneas más abajo)
				GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
				posicionOpciones = { 0, (SHORT)(csbi.dwCursorPosition.Y + 2) };
			}

			// Siempre usamos la misma posición para las opciones
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), posicionOpciones);

			// Limpiamos solo la línea de opciones
			cout << string(80, ' ');
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), posicionOpciones);

			const char* opciones[2] = { "Si", "No" };
			for (int i = 0; i < 2; ++i) {
				if (i == seleccion) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					cout << " > " << opciones[i] << " < ";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				}
				else {
					cout << "   " << opciones[i] << "   ";
				}
			}

			tecla = _getch();
			if (tecla == 224) {
				tecla = _getch();
				if (tecla == 75 && seleccion > 0) { // Flecha izquierda
					--seleccion;
				}
				else if (tecla == 77 && seleccion < 1) { // Flecha derecha
					++seleccion;
				}
			}
			Utilidades::mostrarCursor(); // Muestra el cursor nuevamente
		} while (tecla != 13); // Enter

		return (seleccion == 0); // Retorna true si selecciona "Si"
	};

	/**
	 * @brief Lambda para ingresar y validar montos numéricos con límites
	 *
	 * Permite ingresar valores numéricos con formato de moneda y validación en tiempo real.
	 *
	 * @param min Valor mínimo permitido
	 * @param max Valor máximo permitido
	 * @param mensaje Mensaje a mostrar al usuario
	 * @return double Valor ingresado dentro de los límites especificados
	 */
	const std::function<double(double, double, const std::string&)> ingresarMonto = [](double min, double max, const std::string& mensaje) -> double {
		std::string entrada;
		bool tienePunto = false;
		int digitosDecimales = 0;
		bool terminado = false;

		system("pause");
		while (!terminado) {
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			std::cout << mensaje << ": ";
			std::cout << entrada;

			char tecla = _getch();

			// Enter - verificar si el valor es valido
			if (tecla == 13 && !entrada.empty()) {
				try {
					double valor = std::stod(entrada);
					if (valor >= min && valor <= max) {
						std::cout << std::endl;
						return valor;
					}
					std::cout << "\nEl valor debe estar entre " << min << " y " << max << ". Presione cualquier tecla...";
					int teclaCualquiera = _getch();
					(void)teclaCualquiera;
					continue;
				}
				catch (...) {
					std::cout << "\nFormato invalido. Presione cualquier tecla...";
					int teclaCualquiera = _getch();
					(void)teclaCualquiera;
					continue;
				}
			}

			// Backspace - borrar un caracter
			if (tecla == 8 && !entrada.empty()) {
				if (entrada.back() == '.') {
					tienePunto = false;
				}
				else if (tienePunto && digitosDecimales > 0) {
					digitosDecimales--;
				}
				entrada.pop_back();
				std::cout << "\r" << mensaje << ": " << std::string(40, ' ');
				std::cout << "\r" << mensaje << ": " << entrada;
				continue;
			}

			// Digitos (0-9)
			if (tecla == 0 || tecla == -32 || tecla == 224) {
				// Es una tecla especial (flechas, etc.), consumir el siguiente valor y no hacer nada
				int teclaEspecial = _getch();
				(void)teclaEspecial;
				continue;
			}
			if (isdigit(tecla)) {
				if (tienePunto && digitosDecimales >= 2) {
					continue; // Maximo 2 decimales
				}
				entrada += tecla;
				if (tienePunto) {
					digitosDecimales++;
				}
				std::cout << tecla;
			}
			// Punto decimal (solo uno y no al inicio)
			else if (tecla == '.' && !tienePunto && !entrada.empty()) {
				tienePunto = true;
				entrada += tecla;
				std::cout << tecla;
			}
		}

		return 0.0; // En caso de error
		};
}

using namespace std;

Persona::Persona() : cabezaAhorros(nullptr), cabezaCorriente(nullptr),
numCuentas(0), numCorrientes(0), isDestroyed(false),
validator(std::make_unique<PersonaValidator>()),
dataProcessor(std::make_unique<PersonaDataProcessor>()) {
}

Persona::Persona(const string& cedula, const string& nombres, const string& apellidos,
	const string& fechaNacimiento, const string& correo, const string& direccion)
	: cedula(cedula), nombres(nombres), apellidos(apellidos),
	fechaNacimiento(fechaNacimiento), correo(correo), direccion(direccion),
	cabezaAhorros(nullptr), cabezaCorriente(nullptr), numCuentas(0), numCorrientes(0),
	isDestroyed(false),
	validator(std::make_unique<PersonaValidator>()),
	dataProcessor(std::make_unique<PersonaDataProcessor>()) {
}

Persona::~Persona() {
	liberarListaRecursivo(cabezaAhorros);
	liberarListaRecursivo(cabezaCorriente);
	isDestroyed = true;
}

template void Persona::liberarListaRecursivo<CuentaAhorros>(CuentaAhorros*);
template void Persona::liberarListaRecursivo<CuentaCorriente>(CuentaCorriente*);
template CuentaAhorros* Persona::buscarEnListaRecursivo<CuentaAhorros>(CuentaAhorros*, const std::function<bool(CuentaAhorros*)>&);
template CuentaCorriente* Persona::buscarEnListaRecursivo<CuentaCorriente>(CuentaCorriente*, const std::function<bool(CuentaCorriente*)>&);

// === MÉTODOS RECURSIVOS PRIVADOS ===

template<typename T>
void Persona::liberarListaRecursivo(T* nodo) {
	if (!nodo) return;

	T* siguiente = nodo->getSiguiente();
	delete nodo;
	liberarListaRecursivo(siguiente);
}

template<typename T>
T* Persona::buscarEnListaRecursivo(T* nodo, const std::function<bool(T*)>& criterio) {
	if (!nodo) return nullptr;
	if (criterio(nodo)) return nodo;
	return buscarEnListaRecursivo(nodo->getSiguiente(), criterio);
}

// === SETTERS REFACTORIZADOS CON BUILDER PATTERN ===

Persona& Persona::setCedula(const string& cedula) {
	this->cedula = cedula;
	return *this;
}

Persona& Persona::setNombres(const string& nombres) {
	this->nombres = nombres;
	return *this;
}

Persona& Persona::setApellidos(const string& apellidos) {
	this->apellidos = apellidos;
	return *this;
}

Persona& Persona::setFechaNacimiento(const string& fechaNacimiento) {
	this->fechaNacimiento = fechaNacimiento;
	return *this;
}

Persona& Persona::setCorreo(const string& correo) {
	this->correo = correo;
	return *this;
}

Persona& Persona::setDireccion(const string& direccion) {
	this->direccion = direccion;
	return *this;
}

std::string Persona::procesarEntradaConValidacion(const std::string& tipo, const std::string& prompt,
	const std::function<std::string()>& inputFunction) {
	std::string resultado;
	bool valido = false;

	while (!valido) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << msgIngresoDatos() << std::endl;
		std::cout << prompt;
   		resultado = inputFunction();

		if (resultado.empty()) {
			std::cout << "El campo no puede estar vacio. Presione cualquier tecla para continuar.";
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
			continue;
		}

		valido = validator->validarDato(tipo, resultado);
		if (!valido) {
			std::cout << validator->obtenerMensajeError(tipo) << std::endl;
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
		}
	}

	return resultado;
}

/**
 * @brief Inicia el proceso de ingreso de datos para una persona
 *
 * Llama al método sobrecargado utilizando la cédula actual como parámetro.
 */
void Persona::ingresarDatos() {
	ingresarDatos(this->cedula);
}

/**
 * @brief Método para ingresar los datos de la persona, validando contra una cédula esperada
 *
 * Permite el ingreso completo de los datos personales con validación de cada campo.
 * Si los datos son correctos, los guarda en el objeto y en un archivo.
 *
 * @param cedulaEsperada Cédula que se espera que ingrese el usuario para validación
 */
void Persona::ingresarDatos(const std::string& cedulaEsperada) {
	// Aplicando Command Pattern para cada tipo de entrada
	const std::vector<std::function<void()>> comandosEntrada = {
		[this, &cedulaEsperada]() { this->cedula = ingresarCedula(const_cast<std::string&>(const_cast<std::string&>(cedulaEsperada))); },
		[this]() { this->nombres = ingresarNombres(this->nombres); },
		[this]() { this->apellidos = ingresarApellidos(this->apellidos); },
		[this]() { this->fechaNacimiento = ingresarFechaNacimiento(this->fechaNacimiento); },
		[this]() { this->correo = ingresarCorreo(this->correo); },
		[this]() { this->direccion = ingresarDireccion(this->direccion); }
	};

	bool datosCorrectos = false;
	while (!datosCorrectos) {
		// Ejecutar todos los comandos de entrada
		std::for_each(comandosEntrada.begin(), comandosEntrada.end(), [](const auto& comando) {
			comando();
			});

		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		mostrarDatos();

		int seleccion = Utilidades::menuInteractivo("¿Los datos son correctos?", { "Si", "No" }, 10, 10);
		datosCorrectos = (seleccion == 0);

		if (datosCorrectos) {
			guardarEnArchivo();
		}
	}
}

std::vector<CuentaAhorros*> Persona::obtenerCuentasAhorros() const {
	std::vector<CuentaAhorros*> cuentas;
	forEachCuentaAhorros([&cuentas](CuentaAhorros* cuenta) {
		cuentas.push_back(cuenta);
		});
	return cuentas;
}

std::vector<CuentaCorriente*> Persona::obtenerCuentasCorriente() const {
	std::vector<CuentaCorriente*> cuentas;
	forEachCuentaCorriente([&cuentas](CuentaCorriente* cuenta) {
		cuentas.push_back(cuenta);
		});
	return cuentas;
}

void Persona::forEachCuentaAhorros(const std::function<void(CuentaAhorros*)>& accion) const {
	if (dataProcessor) {
		dataProcessor->procesarCuentasAhorros(const_cast<Persona*>(this), accion);
	}
}

void Persona::forEachCuentaCorriente(const std::function<void(CuentaCorriente*)>& accion) const {
	if (dataProcessor) {
		dataProcessor->procesarCuentasCorriente(const_cast<Persona*>(this), accion);
	}
}

std::vector<CuentaAhorros*> Persona::filtrarCuentasAhorros(const std::function<bool(CuentaAhorros*)>& filtro) const {
	std::vector<CuentaAhorros*> resultado;
	forEachCuentaAhorros([&](CuentaAhorros* cuenta) {
		if (filtro(cuenta)) {
			resultado.push_back(cuenta);
		}
		});
	return resultado;
}

std::vector<CuentaCorriente*> Persona::filtrarCuentasCorriente(const std::function<bool(CuentaCorriente*)>& filtro) const {
	std::vector<CuentaCorriente*> resultado;
	forEachCuentaCorriente([&](CuentaCorriente* cuenta) {
		if (filtro(cuenta)) {
			resultado.push_back(cuenta);
		}
		});
	return resultado;
}

/**
 * @brief Solicita y valida el ingreso de la cédula del usuario
 *
 * Verifica que la cédula ingresada sea válida y coincida con la cédula esperada.
 *
 * @param cedulaIngresada Cédula previamente ingresada para comparación
 * @return std::string Cédula validada
 */
std::string Persona::ingresarCedula(std::string& cedulaIngresada) {
	do {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << msgIngresoDatos() << endl;
		cout << "Por seguridad ingrese nuevamente su cedula (10 digitos): ";

		cedula.clear();
		int digitos = 0;
		while (true) {
			char tecla = _getch();
			if ((tecla >= '0' && tecla <= '9') && digitos < 10) {
				cedula += tecla;
				digitos++;
				std::cout << tecla;
			}
			else if (tecla == 8 && !cedula.empty()) {
				cedula.pop_back();
				digitos--;
				std::cout << "\b \b";
			}
			else if (tecla == 13) {
				std::cout << std::endl;
				break;
				char tecla = _getch();
				if (tecla == 0 || tecla == -32 || tecla == 224) {
					// Es una tecla especial (flechas, etc.), consumir el siguiente valor y no hacer nada
					int teclaEspecial = _getch();
					(void)teclaEspecial;
					continue;
				}
			}
		}

		bool cedulaValida = Validar::ValidarCedula(cedula);
		if (!cedulaValida) {
			cout << "Cedula invalida, presione cualquier tecla para volver a ingresar." << endl;
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
		}
		else if (cedula != cedulaIngresada) {
			cout << "La cedula no coincide con la ingresada previamente.\nPresione cualquier tecla para volver a intentarlo." << endl;
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
		}
		else {
			break; // Salir del bucle si la cedula es valida y coincide
		}
	} while (true);

	return cedula; // Retorna la cedula ingresada
}

/**
 * @brief Solicita y valida el ingreso de los nombres de la persona
 *
 * Valida que los nombres solo contengan letras y espacios, no estén vacíos
 * y cumplan con requisitos de formato.
 *
 * @param nombres Referencia a la variable donde se almacenarán los nombres
 * @return std::string Nombres validados
 */
std::string Persona::ingresarNombres(std::string& nombres) const {
	// Ingreso nombres y validacion
	do {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << msgIngresoDatos() << endl;
		cout << "Ingrese los nombres: ";
		nombres.clear();
		bool espacioPermitido = false; // Solo permite un espacio entre palabras
		while (true) {
			char tecla = _getch();

			// Ignorar teclas especiales (flechas, etc.)
			if (tecla == 0 || tecla == -32 || tecla == 224) {
				// Es una tecla especial (flechas, etc.), consumir el siguiente valor y no hacer nada
				int teclaEspecial = _getch();
				(void)teclaEspecial;
				continue;
			}
			// Enter
			if (tecla == 13 && !nombres.empty()) {
				cout << endl;
				break;
			}
			// Backspace
			else if (tecla == 8 && !nombres.empty()) {
				if (nombres.back() == ' ') espacioPermitido = false;
				nombres.pop_back();
				cout << "\b \b";
			}
			// Letras (mayusculas o minusculas) - permitir solo si no esta vacio o es el primer caracter
			else if (((tecla >= 'A' && tecla <= 'Z') || (tecla >= 'a' && tecla <= 'z'))) {
				nombres += tecla;
				espacioPermitido = true; // Permitir espacio despues de una letra
				cout << tecla;
			}
			// Un solo espacio entre palabras - NO permitir como primer caracter
			else if (tecla == ' ' && espacioPermitido && !nombres.empty()) {
				nombres += tecla;
				espacioPermitido = false;
				cout << tecla;
			}
			// Ignorar cualquier otro caracter
		}
		bool nombresValidos = Validar::ValidarNombrePersona(nombres); // Validar los nombres
		if (!nombres.empty()) { // Si los nombres no estan vacios
			if (!nombresValidos) {
				cout << "Nombre invalido, presione cualquier tecla para volver a ingresar." << endl;
				int teclaCualquiera = _getch();
				(void)teclaCualquiera;
			}
			else {
				break; // Salir del bucle si los nombres son validos
			}
		}
		else {
			cout << "Los nombres no pueden estar vacios, presione cualquier tecla para volver a ingresar." << endl;
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
		}
	} while (true);
	return nombres; // Retorna los nombres ingresados
}

/**
 * @brief Solicita y valida el ingreso de los apellidos de la persona
 *
 * Valida que los apellidos solo contengan letras y espacios, no estén vacíos
 * y cumplan con requisitos de formato.
 *
 * @param apellidos Referencia a la variable donde se almacenarán los apellidos
 * @return std::string Apellidos validados
 */
std::string Persona::ingresarApellidos(std::string& apellidos) const {
	// Ingreso apellidos y validacion
	do {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << msgIngresoDatos() << endl;
		cout << "Ingrese los apellidos: ";
		apellidos.clear();
		bool espacioPermitido = false; // Solo permite un espacio entre palabras
		while (true) {
			char tecla = _getch();

			// Ignorar teclas especiales (flechas, etc.)
			if (tecla == 0 || tecla == -32 || tecla == 224) {
				// Es una tecla especial (flechas, etc.), consumir el siguiente valor y no hacer nada
				int teclaEspecial = _getch();
				(void)teclaEspecial;
				continue;
			}
			// Enter
			if (tecla == 13 && !apellidos.empty()) {
				cout << endl;
				break;
			}
			// Backspace
			else if (tecla == 8 && !apellidos.empty()) {
				if (apellidos.back() == ' ') espacioPermitido = false;
				apellidos.pop_back();
				cout << "\b \b";
			}
			// Letras (mayusculas o minusculas) - permitir solo si no esta vacio o es el primer caracter
			else if (((tecla >= 'A' && tecla <= 'Z') || (tecla >= 'a' && tecla <= 'z'))) {
				apellidos += tecla;
				espacioPermitido = true; // Permitir espacio despues de una letra
				cout << tecla;
			}
			// Un solo espacio entre palabras - No permite como primer caracter
			else if (tecla == ' ' && espacioPermitido && !apellidos.empty()) {
				apellidos += tecla;
				espacioPermitido = false;
				cout << tecla;
			}
			// Ignorar cualquier otro caracter
		}
		bool apellidosValidos = Validar::ValidarNombrePersona(apellidos); // Validar los nombres
		if (!apellidos.empty()) { // Si los apellidos no estan vacios
			if (!apellidosValidos) {
				cout << "Nombre invalido, presione cualquier tecla para volver a ingresar." << endl;
				int teclaCualquiera = _getch();
				(void)teclaCualquiera;
			}
			else {
				break; // Salir del bucle si los nombres son validos
			}
		}
		else { // Si los apellidos estan vacios
			cout << "Los apellidos no pueden estar vacios, presione cualquier tecla para volver a ingresar." << endl;
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
		}
	} while (true);
	return apellidos; // Retorna los apellidos ingresados
}

/**
 * @brief Solicita y valida el ingreso de la fecha de nacimiento
 *
 * Implementa un selector interactivo de fecha y valida que no sea una fecha futura.
 *
 * @param fechaNacimiento Referencia a la variable donde se almacenará la fecha
 * @return std::string Fecha de nacimiento validada en formato DD/MM/AAAA
 */
std::string Persona::ingresarFechaNacimiento(std::string& fechaNacimiento) {
	SYSTEMTIME st;
	GetLocalTime(&st);

	int anioActual = st.wYear;
	int mesActual = st.wMonth;
	int diaActual = st.wDay;

	int dia = 1, mes = 1, anio = 2000;
	bool fechaSeleccionada = false;
	const int anioMaximo = anioActual + 100;

	// campo = 0 (Dia), 1 (Mes), 2 (Año)
	int campo = 0;

	while (!fechaSeleccionada) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << msgIngresoDatos() << endl;
		std::cout << "Use Flechas Izquierda/Derecha para cambiar [DIA|MES|AÑO]\n";
		std::cout << "Flechas Arriba/Abajo para aumentar/disminuir\n";
		std::cout << "Enter para confirmar\n\n";

		// Mostrar en formato horizontal resaltando el campo actual
		std::cout << (campo == 0 ? "-> " : "   ");
		std::cout << (dia < 10 ? "0" : "") << dia << " / ";
		std::cout << (campo == 1 ? "-> " : "   ");
		std::cout << (mes < 10 ? "0" : "") << mes << " / ";
		std::cout << (campo == 2 ? "-> " : "   ");
		std::cout << anio << "\n\n";

		int tecla = _getch();
		if (tecla == 224) {
			tecla = _getch();
			switch (tecla) {
			case 75: // Flecha izquierda
				campo = (campo + 2) % 3;
				break;
			case 77: // Flecha derecha
				campo = (campo + 1) % 3;
				break;
			case 72: // Flecha arriba
				if (campo == 0) {
					dia = (dia == 31) ? 1 : dia + 1;
				}
				else if (campo == 1) {
					mes = (mes == 12) ? 1 : mes + 1;
				}
				else {
					if (anio < anioMaximo) anio++;
				}
				break;
			case 80: // Flecha abajo
				if (campo == 0) {
					dia = (dia == 1) ? 31 : dia - 1;
				}
				else if (campo == 1) {
					mes = (mes == 1) ? 12 : mes - 1;
				}
				else {
					if (anio > 1900) anio--;
				}
				break;
			}
		}
		else if (tecla == 13) { // Enter
			// Formar fecha y validarla
			char buffer[11];
			snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", dia, mes, anio);
			std::string fechaStr(buffer);

			if (!Validar::ValidarFecha(fechaStr)) {
				std::cout << "Fecha invalida. Presione una tecla para continuar.\n";
				int teclaCualquiera = _getch();
				(void)teclaCualquiera;
				continue;
			}

			// Verificar si no es fecha futura
			bool fechaNoFutura = (anio < anioActual) ||
				(anio == anioActual && mes < mesActual) ||
				(anio == anioActual && mes == mesActual && dia <= diaActual);

			if (fechaNoFutura) {
				this->fechaNacimiento = fechaStr;
				fechaSeleccionada = true;
			}
			else {
				std::cout << "Fecha futura. Presione una tecla para continuar.\n";
				int teclaCualquiera = _getch();
				(void)teclaCualquiera;
			}
		}
	}
	return this->fechaNacimiento; // Retorna la fecha de nacimiento ingresada
}

/**
 * @brief Solicita y valida el ingreso del correo electrónico
 *
 * Verifica que el formato del correo sea válido según estándares internacionales.
 *
 * @param correo Referencia a la variable donde se almacenará el correo
 * @return std::string Correo electrónico validado
 */
std::string Persona::ingresarCorreo(std::string& correo) const
{
	std::vector<std::string> dominios = { "gmail.com", "hotmail.com", "outlook.com", "espe.edu.ec", "<Otro dominio>" };

	do {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << msgIngresoDatos() << std::endl;
		std::cout << "Ingrese la parte local del correo (antes del @): ";
		correo.clear();

		std::string parteLocal;
		char c;
		// Entrada de la parte local
		while (true) {
			c = _getch();
			if (c == 13 && !parteLocal.empty()) { // Enter
				break;
			}
			if (c == 8 && !parteLocal.empty()) { // Backspace
				parteLocal.pop_back();
				std::cout << "\b \b";
			}
			else if (isprint(c) && c != ' ' && c != '@') {
				parteLocal += c;
				std::cout << c;
			}
			else if (c == '@' && !parteLocal.empty()) {
				std::cout << '@';
				break;
			}
		}

		if (parteLocal.empty()) {
			std::cout << "\nLa parte local no puede estar vacía. Presione una tecla para reintentar.\n";
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
			continue;
		}

		// Selección de dominio
		int seleccion = Utilidades::menuInteractivo("Seleccione el dominio del correo:", dominios, 5, 5);
		std::string dominioSeleccionado;
		if (seleccion == dominios.size() - 1) { // "<Otro dominio>"
			std::cout << "Ingrese el dominio manualmente: ";
			std::getline(std::cin, dominioSeleccionado);
			if (dominioSeleccionado.empty()) {
				std::cout << "\nEl dominio no puede estar vacío. Presione una tecla para reintentar.\n";
				int teclaCualquiera = _getch();
				(void)teclaCualquiera;
				continue;
			}
		}
		else if (seleccion >= 0 && seleccion < dominios.size()) {
			dominioSeleccionado = dominios[seleccion];
		}
		else {
			std::cout << "\nSelección inválida. Presione una tecla para reintentar.\n";
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
			continue;
		}

		correo = parteLocal + "@" + dominioSeleccionado;

		// Validación de formato
		size_t atPos = correo.find('@');
		bool formatoValido = false;
		if (!correo.empty() && atPos != std::string::npos && atPos > 0 && atPos < correo.length() - 1) {
			std::regex regexCorreo(R"(^[A-Za-z0-9]+([._-]?[A-Za-z0-9]+)*@[A-Za-z0-9]+([.-]?[A-Za-z0-9]+)*\.[A-Za-z]{2,}$)");
			formatoValido = std::regex_match(correo, regexCorreo);

			std::string usuario = correo.substr(0, atPos);
			std::string dominio = correo.substr(atPos + 1);

			if (dominio.find('@') != std::string::npos) formatoValido = false;
			if (!dominio.empty() && (dominio[0] == '.' || dominio[0] == '-' || dominio[0] == '_')) formatoValido = false;
			if (dominio.find("..") != std::string::npos) formatoValido = false;
			if (dominio.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-") != std::string::npos) formatoValido = false;
			if (!usuario.empty() && usuario.back() == '.') formatoValido = false;
			if (!usuario.empty() && usuario.front() == '.') formatoValido = false;
			if (usuario.find("..") != std::string::npos) formatoValido = false;
		}

		if (!formatoValido) {
			std::cout << "\nCorreo invalido, presione cualquier tecla para volver a ingresar." << std::endl;
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
			continue;
		}

		break; // Correo válido

	} while (true);

	return correo;
}

/**
 * @brief Solicita y valida el ingreso de la dirección
 *
 * Permite el ingreso de una dirección con caracteres especiales comunes en direcciones.
 *
 * @param direccion Referencia a la variable donde se almacenará la dirección
 * @return std::string Dirección validada
 */
std::string Persona::ingresarDireccion(std::string& direccion) const
{
	// Ingreso de direccion con getch, maximo 100 caracteres, permitiendo letras, numeros, espacios y simbolos comunes en Ecuador
	do {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << msgIngresoDatos() << endl;
		cout << "Ingrese la direccion: ";
		direccion.clear();
		const int maxDireccion = 100;
		while (true) {
			char tecla = _getch();
			// Ignorar teclas especiales (flechas, etc.)
			if (tecla == 0 || tecla == -32 || tecla == 224) {
				int teclaCualquiera = _getch();
				(void)teclaCualquiera; // Consumir la tecla especial
				continue;
			}

			// Enter
			if (tecla == 13 && !direccion.empty()) {
				cout << endl;
				break;
			}
			// Backspace
			else if (tecla == 8 && !direccion.empty()) {
				direccion.pop_back();
				cout << "\b \b";
			}
			// Permitir letras, numeros, espacios y simbolos comunes en direcciones ecuatorianas
			else if (
				direccion.length() < maxDireccion &&
				(
					(tecla >= 'A' && tecla <= 'Z') ||
					(tecla >= 'a' && tecla <= 'z') ||
					(tecla >= '0' && tecla <= '9') ||
					tecla == ' ' || tecla == '.' || tecla == '-' ||
					tecla == '#' || tecla == ',' || tecla == 'N' || tecla == 'n' || tecla == 'º'
					)
				) {
				direccion += tecla;
				cout << tecla;
			}
			// Ignorar cualquier otro caracter
		}
		if (direccion.empty()) {
			cout << "La direccion no puede estar vacia. Presione cualquier tecla para volver a ingresar." << endl;
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
		}
		else {
			break;
		}
	} while (true);
	return direccion; // Retorna la direccion ingresada
}

/**
 * @brief Muestra un selector para confirmar o corregir los datos ingresados
 *
 * @return bool true si se deben corregir los datos, false si son correctos
 */
bool Persona::corregirDatos()
{
	if (PersonaUI::seleccionarSiNo("\n¿Los datos ingresados son correctos?")) {
		return false; // Si selecciona "Si", retorna false para no repetir el ingreso de datos
	}
	else {
		return true; // Si selecciona "No", retorna true para repetir el ingreso de datos
	}
}

/**
 * @brief Muestra en pantalla los datos actuales de la persona
 */
void Persona::mostrarDatos() const {
	cout << "\n----- DATOS DEL USUARIO -----\n";
	cout << "Cedula: " << cedula << endl;
	cout << "Nombres: " << nombres << endl;
	cout << "Apellidos: " << apellidos << endl;
	cout << "Fecha de nacimiento: " << fechaNacimiento << endl;
	cout << "Correo: " << correo << endl;
	cout << "Direccion: " << direccion << endl;
}

/**
 * @brief Muestra las cuentas asociadas a la persona según su tipo
 *
 * @param tipoCuenta Tipo de cuenta a mostrar: "Ahorros", "Corriente" o "Ambas"
 * @return int Número de cuentas encontradas
 */
int Persona::mostrarCuentas(const std::string& tipoCuenta) const {
	int cuentasEncontradas = 0;
	bool datosTitularMostrados = false;

	auto mostrarDatosTitular = [&]() {
		if (!datosTitularMostrados) {
			mostrarDatos();
			datosTitularMostrados = true;
		}
		};

	if (tipoCuenta == "Ahorros" || tipoCuenta == "Ambas") {
		forEachCuentaAhorros([&](CuentaAhorros* cuenta) {
			mostrarDatosTitular();
			cuenta->mostrarInformacion(this->cedula, false);
			++cuentasEncontradas;
			});
	}

	if (tipoCuenta == "Corriente" || tipoCuenta == "Ambas") {
		forEachCuentaCorriente([&](CuentaCorriente* cuenta) {
			mostrarDatosTitular();
			cuenta->mostrarInformacion(this->cedula, false);
			++cuentasEncontradas;
			});
	}

	return cuentasEncontradas;
}

/**
 * @brief Guarda los datos de la persona en un archivo de texto
 *
 * Almacena todos los datos personales en el archivo "personas.txt".
 */
void Persona::guardarEnArchivo() const {
	ofstream archivo("personas.txt", ios::app);
	if (archivo.is_open()) {
		archivo << "Cedula: " << cedula << endl;
		archivo << "Nombres: " << nombres << endl;
		archivo << "Apellidos: " << apellidos << endl;
		archivo << "Fecha de nacimiento: " << fechaNacimiento << endl;
		archivo << "Correo: " << correo << endl;
		archivo << "Direccion: " << direccion << endl;
		archivo << "-----------------------------" << endl;
		archivo.close();
	}
	else {
		cerr << "Error al abrir el archivo.\n";
	}
}

/**
 * @brief Busca cuentas asociadas a la persona según un criterio específico
 *
 * @param criterioBusqueda Criterio de búsqueda: "Numero de cuenta", "Fecha de apertura", "Saldo mayor a" o "Tipo de cuenta"
 * @param numeroCuenta Número de cuenta a buscar (o tipo de cuenta si el criterio es "Tipo de cuenta")
 * @param fechaApertura Fecha de apertura a buscar
 * @param saldo Valor de saldo mínimo para la búsqueda
 * @return int Número de cuentas que cumplen con el criterio
 */
int Persona::buscarPersonaPorCriterio(const std::string& criterioBusqueda, const std::string& numeroCuenta, const std::string& fechaApertura, double saldo) const {

	int cuentasEncontradas = 0;
	bool mostrarDatosTitular = false;

	// Lambda para buscar en cada tipo de cuenta
	auto buscarEnLista = [&](Cuenta<double>* actual, const std::string& tipo) -> void {
		while (actual) {
			bool encontrado = false;

			if (criterioBusqueda == "Numero de cuenta" &&
				actual->getNumeroCuenta() == numeroCuenta) {
				encontrado = true;
			}
			else if (criterioBusqueda == "Fecha de apertura" &&
				actual->getFechaApertura().toString()
				== fechaApertura) {
				encontrado = true;
			}
			else if (criterioBusqueda == "Saldo mayor a" &&
				actual->consultarSaldo() > saldo) {
				encontrado = true;
			}
			else if (criterioBusqueda == "Tipo de cuenta" &&
				(numeroCuenta == tipo || numeroCuenta == "Ambas")) {
				encontrado = true;
			}

			if (encontrado) {
				// Mostrar datos del titular solo la primera vez
				if (!mostrarDatosTitular) { // Si es la primera cuenta encontrada
					std::cout << "\n===== DATOS DEL TITULAR =====\n";
					std::cout << "Cedula: " << this->cedula << std::endl;
					std::cout << "Nombre: " << this->nombres << " " << this->apellidos << std::endl;
					std::cout << "Correo: " << this->correo << std::endl;
					std::cout << "Direccion: " << this->direccion << std::endl;
					std::cout << std::string(30, '-') << std::endl;
					mostrarDatosTitular = true;
				}

				std::cout << "\nCUENTA DE " << tipo << ":\n";
				actual->mostrarInformacion(this->cedula, false); // false para no borrar la pantalla
				cuentasEncontradas++;
			}

			actual = actual->getSiguiente();
		}
		};

	// Buscar en ambas listas
	buscarEnLista(cabezaAhorros, "Ahorros");
	buscarEnLista(cabezaCorriente, "Corriente");

	return cuentasEncontradas;
}

/**
 * @brief Busca cuentas con una fecha de apertura específica
 *
 * @param fecha Fecha de apertura a buscar en formato DD/MM/AAAA
 */
void Persona::buscarPersonaPorFecha(const std::string& fecha) const {
	// Agregar validacion de instancia destruida
	if (!isValidInstance()) {
		return;
	}

	int encontrados = 0;
	bool datosPersonalesMostrados = false;

	// Funcion para mostrar datos personales solo una vez
	auto buscarFecha = [&](Cuenta<double>* actual, const std::string& tipo) -> void {
		while (actual) {
			// Verificar puntero a cuenta
			if (!actual) { // Si el puntero es nulo, continuar con el siguiente nodo
				actual = actual->getSiguiente();
				continue;
			}
			// Verificar coincidencia de fecha
			if (actual->getFechaApertura().toString() == fecha) { // Si la fecha coincide
				// Mostrar datos personales antes de la primera cuenta
				if (!datosPersonalesMostrados) { // Si no se han mostrado los datos personales
					std::cout << "\n----- DATOS DEL TITULAR -----\n";
					std::cout << "Cedula: " << this->cedula << std::endl;
					std::cout << "Nombres: " << this->nombres << " " << this->apellidos << std::endl;
					std::cout << "Correo: " << this->correo << std::endl;
					std::cout << "Direccion: " << this->direccion << std::endl;
					std::cout << "Fecha de nacimiento: " << this->fechaNacimiento << std::endl;
					std::cout << std::string(30, '-') << std::endl;
					datosPersonalesMostrados = true;
				}
				std::cout << "\nCUENTA DE " << tipo << ":\n";
				actual->mostrarInformacion(this->cedula, false); // false para no borrar la pantalla
				encontrados++;
			}
			actual = actual->getSiguiente();
		}
		};

	//buscar en ambas listas
	buscarFecha(cabezaAhorros, "Ahorros");
	buscarFecha(cabezaCorriente, "Corriente");
}

/**
 * @brief Busca cuentas por número de cuenta
 *
 * @param numeroCuenta Número de cuenta a buscar
 * @return int Número de cuentas encontradas
 */
int Persona::buscarPersonaPorCuentas(const string& numeroCuenta) const {
	if (!isValidInstance()) return 0;

	int encontrados = 0;

	// Buscar usando programación funcional
	auto buscarEnTipo = [&](const auto& lista) {
		std::for_each(lista.begin(), lista.end(), [&](auto* cuenta) {
			if (cuenta && cuenta->getNumeroCuenta() == numeroCuenta) {
				cuenta->mostrarInformacion(this->cedula, false);
				++encontrados;
			}
			});
		};

	buscarEnTipo(obtenerCuentasAhorros());
	buscarEnTipo(obtenerCuentasCorriente());

	return encontrados;
}

/**
 * @brief Guarda las cuentas asociadas a la persona en un archivo
 *
 * @param archivo Referencia al archivo de salida abierto
 * @param tipo Tipo de cuentas a guardar: "AHORROS" o "CORRIENTE"
 * @return int Número de cuentas guardadas
 */
int Persona::guardarCuentas(std::ofstream& archivo, std::string tipo) const {
	if (!archivo.is_open() || !isValidInstance()) {
		return 0;
	}
	int contador = 0;

	auto guardarLista = [&](Cuenta<double>* actual, const std::string& tipo) -> void {
		while (actual) {
			if (actual->getNumeroCuenta() != "") { // Asegurarse de que la cuenta no sea nula
				archivo << tipo << "_INICIO\n";
				archivo << "NUMERO_CUENTA:" << actual->getNumeroCuenta() << "\n";
				archivo << "SALDO:" << actual->consultarSaldo() << "\n";
				archivo << "FECHA_APERTURA:" << actual->getFechaApertura().toString() << "\n";
				archivo << "ESTADO:" << actual->consultarEstado() << "\n";
				// Aqui se podrian añadirse otros campos especificos de cada tipo de cuenta
				archivo << "CUENTA_" << tipo << "_FIN\n";
				contador++;
			}
			actual = actual->getSiguiente();
		}
		};

	if (tipo == "AHORROS") { // Si es tipo Ahorros
		guardarLista(cabezaAhorros, "AHORROS"); // Guardar cuentas de ahorro
	}
	else if (tipo == "CORRIENTE") // Si es tipo Corriente
	{
		guardarLista(cabezaCorriente, "CORRIENTE"); // Guardar cuentas corrientes
	}

	return contador;
}

/**
 * @brief Crea y agrega una cuenta de ahorros para la persona
 *
 * Valida los datos de la persona y configura una nueva cuenta de ahorros
 * con todos los parámetros necesarios, incluyendo depósito inicial opcional.
 *
 * @param nuevaCuenta Puntero a la nueva cuenta de ahorros
 * @param cedulaEsperada Cédula esperada para validación
 * @return bool true si la cuenta fue creada exitosamente, false en caso contrario
 */
bool Persona::crearAgregarCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada)
{
	const int MAX_CUENTAS = 5;

	// Verificar si la cedula esperada coincide con la cedula de la persona
	this->ingresarDatos(cedulaEsperada);

	// Verificacion de puntero nulo primero
	if (!nuevaCuenta) {
		cout << "Error: Cuenta nula pasada como parametro." << endl;
		return false;
	}

	// Verificar limite de cuentas
	if (this->numCuentas >= MAX_CUENTAS) {
		cout << "No se pueden agregar mas cuentas, limite alcanzado." << endl;
		return false;
	}

	try {
		// Crear instancia de base de datos
		_BaseDatosPersona baseDatos(ConexionMongo::obtenerClienteBaseDatos());

		// Generar fecha actual
		Fecha fechaActual;
		std::string fechaStr = fechaActual.obtenerFechaFormateada();

		Utilidades::limpiarPantallaPreservandoMarquesina(1);

		// Configurar monto inicial
		std::cout << "\nAhora puede configurar un monto inicial para su cuenta de ahorros.\n" << std::endl;

		double montoInicial = 0.0;
		bool siIngresoMonto = PersonaUI::seleccionarSiNo("Desea ingresar un monto inicial? (maximo 15000.00 USD)\n");

		if (siIngresoMonto) {
			do {
				montoInicial = PersonaUI::ingresarMonto(0.0, 15000.00, "\nIngrese el monto inicial (maximo 15000.00 USD): ");
			} while (montoInicial < 0 || montoInicial > 15000.00);
		}

		// Seleccionar sucursal
		std::string sucursal = seleccionSucursal();

		// Generar número de cuenta
		std::string numeroCuenta = crearNumeroCuenta(nuevaCuenta, sucursal);
		if (numeroCuenta.empty()) {
			cout << "Error generando numero de cuenta." << endl;
			return false;
		}

		// Configurar datos de la cuenta
		nuevaCuenta->setFechaApertura(fechaStr);
		nuevaCuenta->setEstadoCuenta("ACTIVA");
		nuevaCuenta->depositar(montoInicial);
		nuevaCuenta->setNumeroCuenta(numeroCuenta);

		// **CREAR DOCUMENTO BSON PARA MONGODB**
		using bsoncxx::builder::basic::kvp;
		using bsoncxx::builder::basic::make_document;

		auto cuentaDoc = make_document(
			kvp("numeroCuenta", numeroCuenta),
			kvp("tipo", "ahorros"),
			kvp("saldo", montoInicial),
			kvp("fechaApertura", fechaStr),
			kvp("estado", "ACTIVA"),
			kvp("sucursal", sucursal),
			kvp("fechaCreacion", bsoncxx::types::b_date{ std::chrono::system_clock::now() })
		);

		// **GUARDAR EN MONGODB**
		bool exitoBaseDatos = baseDatos.agregarCuentaPersona(cedulaEsperada, cuentaDoc);
		if (!exitoBaseDatos) {
			std::cout << "Error al guardar la cuenta en la base de datos.\n";
			return false;
		}

		// Agregar a la lista enlazada en memoria
		nuevaCuenta->setSiguiente(cabezaAhorros);
		nuevaCuenta->setAnterior(nullptr);
		if (cabezaAhorros != nullptr) {
			cabezaAhorros->setAnterior(nuevaCuenta);
		}
		this->cabezaAhorros = nuevaCuenta;
		this->numCuentas++;

		// Mostrar información
		nuevaCuenta->mostrarInformacion(cedulaEsperada, true);

		std::cout << "---- DEBUG: Cuenta de Ahorros creada y guardada correctamente ----" << std::endl;
		std::cout << "Número de cuenta: " << numeroCuenta << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al crear cuenta: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Crea y agrega una cuenta corriente para la persona
 *
 * Valida los datos de la persona y configura una nueva cuenta corriente
 * con todos los parámetros necesarios, incluyendo depósito inicial mínimo.
 *
 * @param nuevaCuenta Puntero a la nueva cuenta corriente
 * @param cedulaEsperada Cédula esperada para validación
 * @return bool true si la cuenta fue creada exitosamente, false en caso contrario
 */
bool Persona::crearAgregarCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada) {

	ingresarDatos(cedulaEsperada);

	// Verificacion de puntero nulo primero
	if (!nuevaCuenta) {
		cout << "Error: Cuenta nula pasada como parametro." << endl;
		return false;
	}

	try {
		// Crear instancia de base de datos
		_BaseDatosPersona baseDatos(ConexionMongo::obtenerClienteBaseDatos());

		// Generar fecha actual
		Fecha fechaActual;
		std::string fechaStr = fechaActual.obtenerFechaFormateada();

		// Obligatorio ingresar un monto inicial minimo de 250.00 USD
		double montoInicial = 0.0;
		do {
			montoInicial = PersonaUI::ingresarMonto(250.00, 15000.00, "\nIngrese el monto inicial (minimo 250.00 USD, maximo 15000.00 USD): ");
		} while (montoInicial < 250.00);

		// Seleccionar sucursal
		std::string sucursal = seleccionSucursal();

		// Generar número de cuenta
		std::string numeroCuenta = crearNumeroCuenta(nuevaCuenta, sucursal);
		if (numeroCuenta.empty()) {
			cout << "Error generando numero de cuenta." << endl;
			return false;
		}

		// Configurar datos de la cuenta
		nuevaCuenta->setFechaApertura(fechaStr);
		nuevaCuenta->setEstadoCuenta("ACTIVA");
		nuevaCuenta->depositar(montoInicial);
		nuevaCuenta->setNumeroCuenta(numeroCuenta);

		// **CREAR DOCUMENTO BSON PARA MONGODB**
		using bsoncxx::builder::basic::kvp;
		using bsoncxx::builder::basic::make_document;

		auto cuentaDoc = make_document(
			kvp("numeroCuenta", numeroCuenta),
			kvp("tipo", "corriente"),
			kvp("saldo", montoInicial),
			kvp("fechaApertura", fechaStr),
			kvp("estado", "ACTIVA"),
			kvp("sucursal", sucursal),
			kvp("fechaCreacion", bsoncxx::types::b_date{ std::chrono::system_clock::now() })
		);

		// **GUARDAR EN MONGODB**
		bool exitoBaseDatos = baseDatos.agregarCuentaPersona(cedulaEsperada, cuentaDoc);
		if (!exitoBaseDatos) {
			std::cout << "Error al guardar la cuenta en la base de datos.\n";
			return false;
		}

		// Agregar a la lista enlazada en memoria
		nuevaCuenta->setSiguiente(cabezaCorriente);
		nuevaCuenta->setAnterior(nullptr);
		if (cabezaCorriente != nullptr) {
			cabezaCorriente->setAnterior(nuevaCuenta);
		}
		this->cabezaCorriente = nuevaCuenta;
		this->numCuentas++;

		// Mostrar información
		nuevaCuenta->mostrarInformacion(cedulaEsperada, true);

		std::cout << "---- DEBUG: Cuenta Corriente creada y guardada correctamente ----" << std::endl;
		std::cout << "Número de cuenta: " << numeroCuenta << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al crear cuenta: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Crea una cuenta de ahorros sin solicitar datos personales adicionales
 *
 * @param nuevaCuenta Puntero a la nueva cuenta de ahorros
 * @param cedulaEsperada Cédula esperada para validación
 * @return bool true si la cuenta fue creada exitosamente, false en caso contrario
 */
bool Persona::crearSoloCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada)
{
	const int MAX_CUENTAS = 5;

	// Verificacion de puntero nulo primero
	if (!nuevaCuenta) {
		cout << "Error: Cuenta nula pasada como parametro." << endl;
		return false;
	}

	// Verificar limite de cuentas
	if (this->numCuentas >= MAX_CUENTAS) {
		cout << "No se pueden agregar mas cuentas, limite alcanzado." << endl;
		return false;
	}

	try {
		// Generar numero de cuenta
		Fecha fechaActual;
		std::string fechaStr = fechaActual.obtenerFechaFormateada();
		nuevaCuenta->setFechaApertura(fechaStr);
		nuevaCuenta->setEstadoCuenta("ACTIVA");

		// Desea ingresar un saldo inicial, si o no? maximo 15000.00 USD
		double montoInicial = 0.0;
		if (PersonaUI::seleccionarSiNo("Desea ingresar un monto inicial? (maximo 15000.00 USD)"))
		{ // Si desea ingresar un monto inicial 
			do {
				montoInicial = PersonaUI::ingresarMonto(0.0, 15000.00, "Ingrese el monto inicial (maximo 15000.00 USD): ");
			} while (montoInicial < 0 || montoInicial > 15000.00);
			nuevaCuenta->depositar(montoInicial); // Depositar el monto inicial
		}
		else { // Si no desea ingresar un monto inicial, se considera 0 
			montoInicial = 0.0; // Si no se ingresa monto, se considera 0
		}
		// No es necesario setear el saldo, ya que depositar lo hace automaticamente

		// Generar numero de cuenta
		// Pedir al usuario que seleccione una sucursal
		std::string sucursal = seleccionSucursal();

		std::string numeroCuenta = crearNumeroCuenta(nuevaCuenta, sucursal);
		if (numeroCuenta.empty()) {
			cout << "Error generando numero de cuenta." << endl;
			return false;
		}
		if (numeroCuenta.empty()) {
			cout << "Error generando numero de cuenta." << endl;
			return false;
		}

		// Mostrar informacion
		nuevaCuenta->mostrarInformacion(cedulaEsperada, true);

		// Agregar a la lista enlazada
		nuevaCuenta->setSiguiente(cabezaAhorros);
		nuevaCuenta->setAnterior(nullptr);
		if (cabezaAhorros != nullptr) {
			cabezaAhorros->setAnterior(nuevaCuenta);
		}
		this->cabezaAhorros = nuevaCuenta;
		this->numCuentas++;

		std::cout << "---- Cuenta de Ahorros creada correctamente ----" << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al crear cuenta: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Crea una cuenta corriente sin solicitar datos personales adicionales
 *
 * @param nuevaCuenta Puntero a la nueva cuenta corriente
 * @param cedulaEsperada Cédula esperada para validación
 * @return bool true si la cuenta fue creada exitosamente, false en caso contrario
 */
bool Persona::crearSoloCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada)
{
	// Verificacion de puntero nulo primero
	if (!nuevaCuenta) {
		cout << "Error: Cuenta nula pasada como parametro." << endl;
		return false;
	}

	try {
		// Generar numero de cuenta
		Fecha fechaActual;
		std::string fechaStr = fechaActual.obtenerFechaFormateada();
		nuevaCuenta->setFechaApertura(fechaStr);
		nuevaCuenta->setEstadoCuenta("ACTIVA");

		// Obligatorio ingresar un monto inicial minimo de 250.00 USD
		double montoInicial = 0.0;
		do {
			montoInicial = PersonaUI::ingresarMonto(250.00, 15000.00, "\nIngrese el monto inicial (minimo 250.00 USD, maximo 15000.00 USD): ");
		} while (montoInicial < 250.00);
		nuevaCuenta->depositar(montoInicial); // Depositar el monto inicial
		// No es necesario setear el saldo, ya que depositar lo hace automaticamente

		// Pedir al usuario que seleccione una sucursal
		std::string sucursal = seleccionSucursal();

		std::string numeroCuenta = crearNumeroCuenta(nuevaCuenta, sucursal);
		if (numeroCuenta.empty()) {
			cout << "Error generando numero de cuenta." << endl;
			return false;
		}
		if (numeroCuenta.empty()) {
			cout << "Error generando numero de cuenta." << endl;
			return false;
		}

		// Mostrar informacion
		nuevaCuenta->mostrarInformacion(cedulaEsperada, true);

		// Agregar a la lista enlazada
		nuevaCuenta->setSiguiente(cabezaCorriente);
		nuevaCuenta->setAnterior(nullptr);
		if (cabezaCorriente != nullptr) {
			cabezaCorriente->setAnterior(nuevaCuenta);
		}
		this->cabezaCorriente = nuevaCuenta;
		this->numCuentas++;

		std::cout << "---- Cuenta de Corriente creada correctamente ----" << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al crear cuenta: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Genera un número de cuenta único para una nueva cuenta
 *
 * Crea un número de cuenta basado en el código de sucursal, un secuencial
 * y un dígito verificador calculado.
 *
 * @param nuevaCuenta Puntero a la nueva cuenta
 * @param sucursal Código de la sucursal seleccionada
 * @return std::string Número de cuenta generado
 */
std::string Persona::crearNumeroCuenta(Cuenta<double>* nuevaCuenta, const std::string& sucursal) {
	// Validar que la sucursal esté entre las permitidas
	if (sucursal != "210" && sucursal != "220" && sucursal != "480" && sucursal != "560") {
		std::cerr << "Error: Código de sucursal no válido. Use 210, 220, 480 o 560." << std::endl;
		return "";
	}

	try {
		// Crear instancia de base de datos usando la conexión actual
		_BaseDatosPersona baseDatos(ConexionMongo::obtenerClienteBaseDatos());

		// Obtener el último secuencial desde MongoDB
		int ultimoSecuencialBD = baseDatos.obtenerUltimoSecuencial(sucursal);

		// Obtener el mayor número de cuenta existente en la BD por si hay inconsistencias
		int mayorSecuencialCuentas = baseDatos.obtenerMayorNumeroCuentaPorSucursal(sucursal);

		// También verificar en memoria (cuentas locales no guardadas aún)
		int mayorSecuencialMemoria = obtenerMayorSecuencialEnMemoria(sucursal);

		// Usar el máximo de todos los valores encontrados
		int mayorSecuencial = std::max({ ultimoSecuencialBD, mayorSecuencialCuentas, mayorSecuencialMemoria });

		// Incrementar para obtener el siguiente número secuencial
		int nuevoSecuencial = mayorSecuencial + 1;

		// Formatear el número de cuenta con ceros a la izquierda
		std::ostringstream oss;
		oss << sucursal << std::setw(6) << std::setfill('0') << nuevoSecuencial;
		std::string base = oss.str();

		// Calcular dígito verificador: suma de todos los dígitos módulo 10
		int suma = 0;
		for (char c : base) {
			suma += (c - '0');
		}
		int digitoVerificador = suma % 10;

		// Formar el número de cuenta completo
		std::string numeroCuentaStr = base + std::to_string(digitoVerificador);

		// Verificar que el número de cuenta sea válido
		if (!Validar::ValidarNumeroCuenta(numeroCuentaStr)) {
			std::cerr << "Error: El número de cuenta generado no es válido." << std::endl;
			return "";
		}

		// Actualizar el secuencial en la base de datos
		if (!baseDatos.actualizarSecuencial(sucursal, nuevoSecuencial)) {
			std::cerr << "Advertencia: No se pudo actualizar el secuencial en la base de datos." << std::endl;
		}

		// Asignar el número de cuenta al objeto
		nuevaCuenta->setNumeroCuenta(numeroCuentaStr);

		std::cout << "Número de cuenta generado: " << numeroCuentaStr << std::endl;
		return numeroCuentaStr;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al crear número de cuenta: " << e.what() << std::endl;
		return "";
	}
}

/**
 * @brief Obtiene el mayor número secuencial de cuentas en memoria para una sucursal
 * @param sucursal Código de sucursal
 * @return Mayor número secuencial encontrado en memoria
 */
int Persona::obtenerMayorSecuencialEnMemoria(const std::string& sucursal) {
	int mayorSecuencial = 0;
	std::vector<Cuenta<double>*> cuentas;

	// Recolectar todas las cuentas en memoria
	CuentaAhorros* actualAhorros = cabezaAhorros;
	while (actualAhorros) {
		cuentas.push_back(actualAhorros);
		actualAhorros = actualAhorros->getSiguiente();
	}

	CuentaCorriente* actualCorriente = cabezaCorriente;
	while (actualCorriente) {
		cuentas.push_back(actualCorriente);
		actualCorriente = actualCorriente->getSiguiente();
	}

	// Buscar el mayor número secuencial para la sucursal especificada
	for (auto cuenta : cuentas) {
		std::string numCuenta = cuenta->getNumeroCuenta();

		if (numCuenta.length() == 10 && numCuenta.substr(0, 3) == sucursal) {
			try {
				int numSecuencial = std::stoi(numCuenta.substr(3, 6));
				if (numSecuencial > mayorSecuencial) {
					mayorSecuencial = numSecuencial;
				}
			}
			catch (const std::exception& e) {
				std::cerr << "Error al procesar número de cuenta en memoria: " << e.what() << std::endl;
			}
		}
	}

	return mayorSecuencial;
}

/**
 * @brief Presenta un selector de sucursal bancaria
 *
 * Muestra un menú interactivo para seleccionar la sucursal donde se aperturará la cuenta.
 *
 * @return std::string Código de la sucursal seleccionada
 */
std::string Persona::seleccionSucursal() {
	std::string sucursales[] = {
		"Sucursal Quicentro Shopping",
		"Sucursal Centro Plaza Grande",
		"Sucursal Quicentro Sur",
		"Sucursal Valle de los Chillos"
	};

	std::string codigosSucursales[] = { "210", "220", "480", "560" };
	int numOpciones = sizeof(sucursales) / sizeof(sucursales[0]);
	int seleccion = 0;

	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "\nSeleccione la sucursal donde se encuentra:\n\n";

	// Mostrar opciones inicialmente
	for (int i = 0; i < numOpciones; i++) {
		if (i == seleccion)
			std::cout << " > " << sucursales[i] << std::endl;
		else
			std::cout << "   " << sucursales[i] << std::endl;
	}

	// Navegación del menú
	while (true) {
		Utilidades::ocultarCursor();
		int tecla = _getch();

		if (tecla == 224) { // Tecla especial
			tecla = _getch();
			if (tecla == 72) { // Flecha arriba
				// Guardar selección anterior
				int selAnterior = seleccion;
				seleccion = (seleccion - 1 + numOpciones) % numOpciones;

				// Actualizar solo las líneas que cambian
				Utilidades::gotoxy(0, 2 + selAnterior);
				std::cout << "   " << sucursales[selAnterior] << std::string(20, ' ');

				Utilidades::gotoxy(0, 2 + seleccion);
				std::cout << " > " << sucursales[seleccion] << std::string(20, ' ');
			}
			else if (tecla == 80) { // Flecha abajo
				// Guardar selección anterior
				int selAnterior = seleccion;
				seleccion = (seleccion + 1) % numOpciones;

				// Actualizar solo las líneas que cambian
				Utilidades::gotoxy(0, 2 + selAnterior);
				std::cout << "   " << sucursales[selAnterior] << std::string(20, ' ');

				Utilidades::gotoxy(0, 2 + seleccion);
				std::cout << " > " << sucursales[seleccion] << std::string(20, ' ');
			}
		}
		else if (tecla == 13) { // ENTER
			return codigosSucursales[seleccion];
		}
		else if (tecla == 27) { // ESC - valor por defecto
			return "210"; // Retorna código de sucursal por defecto
		}
		Utilidades::mostrarCursor();
	}
}

/**
 * @brief Genera un mensaje estándar para el ingreso de datos
 *
 * @return std::string Mensaje formateado
 */
std::string Persona::msgIngresoDatos() const {
	return "\n----- INGRESO DE DATOS -----\n";
}

template<typename T>
T* Persona::configurarCabezaLista(T* nuevaCabeza, T*& cabezaActual) {
	if (!nuevaCabeza) return cabezaActual;

	// Aplicando Command Pattern para encapsular la operación
	auto comando = [&cabezaActual](T* nodo) {
		nodo->setSiguiente(cabezaActual);
		nodo->setAnterior(nullptr);

		if (cabezaActual) {
			cabezaActual->setAnterior(nodo);
		}
		};

	comando(nuevaCabeza);
	cabezaActual = nuevaCabeza;

	return cabezaActual;
}

CuentaAhorros* Persona::setCabezaAhorros(CuentaAhorros* nuevaCabeza) {
	return configurarCabezaLista(nuevaCabeza, cabezaAhorros);
}

CuentaCorriente* Persona::setCabezaCorriente(CuentaCorriente* nuevaCabeza) {
	return configurarCabezaLista(nuevaCabeza, cabezaCorriente);
}