#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <conio.h> // Para _getch()
#include <windows.h>
#include <functional>
#include <iterator>
#include "Validar.h"
#include "Fecha.h"
#include "Persona.h"

namespace PersonaUI {
	// Lambda para cursor de seleccion de si o no
	const std::function<bool(const std::string&)> seleccionarSiNo = [](const std::string& mensaje) -> bool {
		int seleccion = 0;
		int tecla = 0;
		system("pause");
		do {
			system("cls");
			cout << mensaje << endl;
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
			cout << "\r"; // Retorna el cursor al inicio de la linea
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
		} while (tecla != 13); // Enter
		return (seleccion == 0); // Retorna true si selecciona "Si"
		};

	// Lambda para ingresar montos con validación en tiempo real
	const std::function<double(double, double, const std::string&)> ingresarMonto = [](double min, double max, const std::string& mensaje) -> double {
		std::string entrada;
		bool tienePunto = false;
		int digitosDecimales = 0;
		bool terminado = false;

		system("pause");
		while (!terminado) {
			system("cls");
			std::cout << mensaje << ": ";
			std::cout << entrada;

			char tecla = _getch();

			// Enter - verificar si el valor es válido
			if (tecla == 13 && !entrada.empty()) {
				try {
					double valor = std::stod(entrada);
					if (valor >= min && valor <= max) {
						std::cout << std::endl;
						return valor;
					}
					std::cout << "\nEl valor debe estar entre " << min << " y " << max << ". Presione cualquier tecla...";
					_getch();
					continue;
				}
				catch (...) {
					std::cout << "\nFormato inválido. Presione cualquier tecla...";
					_getch();
					continue;
				}
			}

			// Backspace - borrar un carácter
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

			// Dígitos (0-9)
			if (isdigit(tecla)) {
				if (tienePunto && digitosDecimales >= 2) {
					continue; // Máximo 2 decimales
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

// Metodo para ingresar Datos de la persona
void Persona::ingresarDatos() {
	ingresarDatos(this->cedula); // O puedes pasar "" si prefieres no validar
}

/// <summary>
/// Metodo para ingresar los datos de la persona, validando la cedula ingresada previamente.
/// </summary>
/// <param name="cedulaEsperada"></param>
void Persona::ingresarDatos(const std::string& cedulaEsperada) {
	do {
		std::string cedulaTemp = cedulaEsperada; // Cedula esperada para validar
		std::string cedula = ingresarCedula(cedulaTemp); // Llamar al metodo para ingresar la cedula
		std::string nombres = ingresarNombres(this->nombres); // Llamar al metodo para ingresar los nombres
		std::string apellidos = ingresarApellidos(this->apellidos); // Llamar al metodo para ingresar los apellidos
		std::string fechaNacimiento = ingresarFechaNacimiento(this->fechaNacimiento); // Llamar al metodo para ingresar la fecha de nacimiento
		std::string correo = ingresarCorreo(this->correo); // Llamar al metodo para ingresar el correo
		std::string direccion = ingresarDireccion(this->direccion); // Llamar al metodo para ingresar la direccion
		std::system("cls"); // Limpiar la pantalla
		// Si el usuario considera que los datos son correctos, 
		// selecciona "Si" para setear los datos o "No" para repetir
		// Mostrar los datos ingresados y preguntar si desea corregirlos
		system("cls");
		mostrarDatos();
		cout << endl;

		bool seleccion = corregirDatos(); // Llamar al metodo para corregir los datos`
		// Si selecciona "Si", repetir el ingreso de datos
		if (!(seleccion == 0)) { // Si selecciona "Si"
			continue;
		}
		else { // Si selecciona "No"
			// Setear los datos
			setCedula(cedula);
			setNombres(nombres);
			setApellidos(apellidos);
			setFechaNacimiento(fechaNacimiento);
			setCorreo(correo);
			setDireccion(direccion);
			// Guardar los datos en el archivo
			guardarEnArchivo();
			break;
		}
	} while (true);
}

/// <summary>
/// Metodo para ingresar la cedula de la persona, validando que sea correcta y que coincida con la cedula esperada.
/// </summary>
/// <param name="cedulaIngresada"></param>
/// <returns></returns>
std::string Persona::ingresarCedula(std::string& cedulaIngresada) {
	do {
		system("cls");
		cout << "----- INGRESE SUS DATOS -----\n";
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

/// <summary>
/// Metodo para ingresar los nombres de la persona, validando que no esten vacios y que sean correctos.
/// </summary>
/// <param name="nombres"></param>
/// <returns></returns>
std::string Persona::ingresarNombres(std::string& nombres) {
	// Ingreso nombres y validacion
	do {
		system("cls");
		cout << "----- INGRESE SUS DATOS -----\n";
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
			// Letras (mayusculas o minusculas) - permitir solo si no está vacío o es el primer carácter
			else if (((tecla >= 'A' && tecla <= 'Z') || (tecla >= 'a' && tecla <= 'z'))) {
				nombres += tecla;
				espacioPermitido = true; // Permitir espacio después de una letra
				cout << tecla;
			}
			// Un solo espacio entre palabras - NO permitir como primer carácter
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

/// <summary>
/// Metodo para ingresar los apellidos de la persona, validando que no esten vacios y que sean correctos.
/// </summary>
/// <param name="apellidos"></param>
/// <returns></returns>
std::string Persona::ingresarApellidos(std::string& apellidos) {
	// Ingreso apellidos y validacion
	do {
		system("cls");
		cout << "----- INGRESE SUS DATOS -----\n";
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
			// Letras (mayusculas o minusculas) - permitir solo si no está vacío o es el primer carácter
			else if (((tecla >= 'A' && tecla <= 'Z') || (tecla >= 'a' && tecla <= 'z'))) {
				apellidos += tecla;
				espacioPermitido = true; // Permitir espacio después de una letra
				cout << tecla;
			}
			// Un solo espacio entre palabras - No permite como primer carácter
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

/// <summary>
/// Metodo para ingresar la fecha de nacimiento de la persona, validando que sea correcta y no futura.
/// </summary>
/// <param name="fechaNacimiento"></param>
/// <returns></returns>
std::string Persona::ingresarFechaNacimiento(std::string& fechaNacimiento) {
	SYSTEMTIME st;
	GetLocalTime(&st);

	int anioActual = st.wYear;
	int mesActual = st.wMonth;
	int diaActual = st.wDay;

	int dia = 1, mes = 1, anio = 2000;
	bool fechaSeleccionada = false;
	const int anioMaximo = anioActual + 100;

	// campo = 0 (Día), 1 (Mes), 2 (Año)
	int campo = 0;

	while (!fechaSeleccionada) {
		system("cls");
		std::cout << "----- INGRESE SUS DATOS -----\n";
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

/// <summary>
/// Metodo para ingresar el correo de la persona, validando que sea correcto y cumpla con los requisitos.
/// </summary>
/// <param name="correo"></param>
/// <returns></returns>
std::string Persona::ingresarCorreo(std::string& correo)
{
	// Ingreso correo y validacion con getch, simbolos validos, sin espacios ni dos puntos seguidos, y limites de longitud
	do {
		system("cls");
		cout << "----- INGRESE SUS DATOS -----\n";
		cout << "Ingrese el correo: ";
		correo.clear();
		int atCount = 0;
		int localLen = 0;
		int domainLen = 0;
		bool enDominio = false;
		char prevChar = '\0';
		while (true) {
			char tecla = _getch();

			// Ignorar teclas especiales (flechas, etc.)
			if (tecla == 0 || tecla == -32 || tecla == 224) {
				int teclaCualquiera = _getch();
				(void)teclaCualquiera; // Consumir la tecla especial
				continue;
			}

			// Enter
			if (tecla == 13 && !correo.empty()) {
				cout << endl;
				break;
			}
			// Backspace
			else if (tecla == 8 && !correo.empty()) {
				if (correo.back() == '@') {
					atCount--;
					enDominio = false;
					domainLen = 0;
				}
				else if (!enDominio) {
					localLen--;
				}
				else {
					domainLen--;
				}
				correo.pop_back();
				cout << "\b \b";
				prevChar = correo.empty() ? '\0' : correo.back();
			}
			// No permitir espacios
			else if (tecla == ' ') {
				continue;
			}
			// No permitir dos puntos seguidos
			else if (tecla == '.' && prevChar == '.') {
				continue;
			}
			// Solo un '@' y no al inicio
			else if (tecla == '@') {
				if (atCount == 0 && !correo.empty() && !enDominio && localLen > 0) {
					correo += tecla;
					atCount++;
					enDominio = true;
					cout << tecla;
					prevChar = tecla;
				}
			}
			// Permitir simbolos validos internacionalmente en la parte local
			else if (!enDominio &&
				localLen < 64 &&
				((tecla >= 'A' && tecla <= 'Z') || (tecla >= 'a' && tecla <= 'z') ||
					(tecla >= '0' && tecla <= '9') ||
					std::string("!#$%&'*+/=?^_`{|}~.-").find(tecla) != std::string::npos)) {
				correo += tecla;
				localLen++;
				cout << tecla;
				prevChar = tecla;
			}
			// Permitir simbolos validos en el dominio (solo letras, numeros, guion y punto)
			else if (enDominio &&
				domainLen < 255 &&
				((tecla >= 'A' && tecla <= 'Z') || (tecla >= 'a' && tecla <= 'z') ||
					(tecla >= '0' && tecla <= '9') ||
					tecla == '-' || tecla == '.')) {
				correo += tecla;
				domainLen++;
				cout << tecla;
				prevChar = tecla;
			}
			// Ignorar cualquier otro caracter
		}
		// Validar formato general y restricciones
		// No permitir dos puntos seguidos, ni al inicio/final de local o dominio
		bool formatoValido = false;
		std::regex pattern(R"(^[A-Za-z0-9!#$%&'*+/=?^_`{|}~.-]{1,64}@[A-Za-z0-9.-]{1,255}\.[A-Za-z]{2,}$)");
		if (correo.find("..") == std::string::npos &&
			correo.find(' ') == std::string::npos &&
			std::regex_match(correo, pattern)) {
			// No punto al inicio/final de local o dominio
			auto atPos = correo.find('@');
			if (atPos != std::string::npos && atPos > 0 && atPos < correo.length() - 1) {
				string local = correo.substr(0, atPos);
				string domain = correo.substr(atPos + 1);
				if (local.front() != '.' && local.back() != '.' &&
					domain.front() != '.' && domain.back() != '.') {
					formatoValido = true;
				}
			}
		}
		if (!correo.empty()) { // Si el correo no esta vacio
			if (!formatoValido) {
				cout << "Correo invalido, presione cualquier tecla para volver a ingresar." << endl;
				int teclaCualquiera = _getch();
				(void)teclaCualquiera;
			}
			else {
				break; // Salir del bucle si el correo es valido
			}
		}
		else { // Si el correo esta vacio
			cout << "El correo no puede estar vacio, presione cualquier tecla para volver a ingresar." << endl;
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
		}
	} while (true);

	return correo; // Retorna el correo ingresado
}

/// <summary>
/// Metodo para ingresar la direccion de la persona, validando que no este vacia y sea correcta.
/// </summary>
/// <param name="direccion"></param>
/// <returns></returns>
std::string Persona::ingresarDireccion(std::string& direccion)
{
	// Ingreso de direccion con getch, maximo 100 caracteres, permitiendo letras, numeros, espacios y simbolos comunes en Ecuador
	do {
		system("cls");
		cout << "----- INGRESE SUS DATOS -----\n";
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

/// <summary>
/// Metodo para corregir los datos de la persona.
/// </summary>
/// <returns></returns>
bool Persona::corregirDatos()
{
	if (PersonaUI::seleccionarSiNo("¿Los datos ingresados son correctos?")) {
		return false; // Si selecciona "Si", retorna false para no repetir el ingreso de datos
	}
	else {
		return true; // Si selecciona "No", retorna true para repetir el ingreso de datos
	}
}

/// <summary>
/// Metodo para mostrar los datos de la persona.
/// </summary>
void Persona::mostrarDatos() const {
	cout << "\n----- DATOS DEL USUARIO -----\n";
	cout << "Cedula: " << cedula << endl;
	cout << "Nombres: " << nombres << endl;
	cout << "Apellidos: " << apellidos << endl;
	cout << "Fecha de nacimiento: " << fechaNacimiento << endl;
	cout << "Correo: " << correo << endl;
	cout << "Direccion: " << direccion << endl;
}

/// <summary>
/// Metodo para mostrar las cuentas de la persona, filtrando por tipo de cuenta.
/// </summary>
/// <param name="tipoCuenta"></param>
/// <returns></returns>
int Persona::mostrarCuentas(const std::string& tipoCuenta) const
{
	int cuentasEncontradas = 0;
	bool mostrarDatosTitular = false;

	// Lambda para buscar en cada tipo de cuenta
	auto buscarEnCuentas = [&](Cuenta<double>* actual, const std::string& tipo) -> void {
		while (actual) {
			bool encontrado = false;

			if (tipoCuenta == "Ahorros" && dynamic_cast<CuentaAhorros*>(actual)) {
				encontrado = true;
			}
			else if (tipoCuenta == "Corriente" && dynamic_cast<CuentaCorriente*>(actual)) {
				encontrado = true;
			}
			else if (tipoCuenta == "Ambas") {
				encontrado = true;
			}

			if (encontrado) {
				// Mostrar datos del titular solo la primera vez
				if (!mostrarDatosTitular) { // Si es la primera cuenta encontrada
					std::cout << "\n ===== DATOS DEL TITULAR =====\n";
					std::cout << "Cédula: " << this->cedula << std::endl;
					std::cout << "Nombre: " << this->nombres << " " << this->apellidos << std::endl;
					std::cout << "Correo: " << this->correo << std::endl;
					std::cout << "Dirección: " << this->direccion << std::endl;
					std::cout << std::string(30, '-') << std::endl;
					mostrarDatosTitular = true;
				}
				std::cout << "\nCUENTA DE " << tipo << ":\n";
				actual->mostrarInformacion(this->cedula, false); // false para no borrar la pantalla
				cuentasEncontradas++;
			}
			actual = actual->getSiguiente(); // Avanzar al siguiente nodo
		}
		};

	// Buscar en ambas listas
	buscarEnCuentas(cabezaAhorros, "Ahorros");
	buscarEnCuentas(cabezaCorriente, "Corriente");

	return cuentasEncontradas; // Retorna el numero de cuentas encontradas
}

/// <summary>
/// Metodo para guardar los datos de la persona en un archivo.
/// </summary>
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

/// <summary>
/// Metodo para buscar una persona por un criterio especifico.
/// </summary>
/// <param name="criterioBusqueda"></param>
/// <param name="numeroCuenta"></param>
/// <param name="fechaApertura"></param>
/// <param name="saldo"></param>
/// <returns></returns>
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
					std::cout << "Cédula: " << this->cedula << std::endl;
					std::cout << "Nombre: " << this->nombres << " " << this->apellidos << std::endl;
					std::cout << "Correo: " << this->correo << std::endl;
					std::cout << "Dirección: " << this->direccion << std::endl;
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

/// <summary>
/// Metodo para buscar una persona por fecha de apertura de cuentas.
/// </summary>
/// <param name="fecha"></param>
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

/// <summary>
/// Metodo para buscar una persona por numero de cuenta.
/// </summary>
/// <param name="numeroCuenta"></param>
/// <returns></returns>
int Persona::buscarPersonaPorCuentas(const string& numeroCuenta) const {
	// Evita acceso a un objeto destruido
	if (!isValidInstance()) {
		//std::cerr << "Error: objeto 'Persona' invalido (destruido o no inicializado).\n";
		return 0;
	}
	int encontrados = 0;
	auto buscarCuenta = [&](Cuenta<double>* actual, const std::string& tipo) -> void {
		while (actual) {
			if (!actual) { // Si el puntero es nulo, continuar con el siguiente nodo
				actual = actual->getSiguiente();
				continue;
			}
			if (actual->getNumeroCuenta() == numeroCuenta) {
				actual->mostrarInformacion(this->cedula, false); // false para no borrar la pantalla
				encontrados++;
			}
			actual = actual->getSiguiente();
		}
		};
	return encontrados;
}

/// <summary>
/// Metodo para guardar las cuentas de la persona en un archivo.
/// </summary>
/// <param name="archivo"></param>
/// <returns></returns>
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
				// Aqui se podrian añadir otros campos especificos de cada tipo de cuenta
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

/// <summary>
/// Metodo para crear y agregar una cuenta de ahorros a la persona.
/// </summary>
/// <param name="nuevaCuenta"></param>
/// <param name="cedulaEsperada"></param>
/// <returns></returns>
bool Persona::crearAgregarCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada)
{
	const int MAX_CUENTAS = 5; // Definir el maximo de cuentas permitidas

	// Verificar si la cedula esperada coincide con la cedula de la persona
	ingresarDatos(cedulaEsperada);

	// Verificación de puntero nulo primero
	if (!nuevaCuenta) {
		cout << "Error: Cuenta nula pasada como parámetro." << endl;
		return false;
	}

	// Verificar límite de cuentas
	if (this->numCuentas >= MAX_CUENTAS) {
		cout << "No se pueden agregar más cuentas, límite alcanzado." << endl;
		return false;
	}

	try {
		// Generar número de cuenta
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

		std::string numeroCuenta = crearNumeroCuenta(nuevaCuenta);
		if (numeroCuenta.empty()) {
			cout << "Error generando número de cuenta." << endl;
			return false;
		}

		// Mostrar información
		nuevaCuenta->mostrarInformacion(cedulaEsperada, true);

		//std::cout << "Antes de vincular: cabezaAhorros = " << (cabezaAhorros ? cabezaAhorros->getNumeroCuenta() : "NULL") << std::endl; // Depuración para verificar la vinculación
		// Agregar a la lista enlazada
		nuevaCuenta->setSiguiente(cabezaAhorros);
		nuevaCuenta->setAnterior(nullptr);
		if (cabezaAhorros != nullptr) {
			cabezaAhorros->setAnterior(nuevaCuenta);
		}
		this->cabezaAhorros = nuevaCuenta;
		this->numCuentas++;

		//std::cout << "Después de vincular: cabezaAhorros = " << (cabezaAhorros ? cabezaAhorros->getNumeroCuenta() : "NULL") << std::endl; // Depuración para verificar la vinculación

		std::cout << "---- Cuenta de Ahorros creada correctamente ----" << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al crear cuenta: " << e.what() << std::endl;
		return false;
	}
}

/// <summary>
/// Metodo para crear y agregar una cuenta corriente a la persona, validando que no se supere el maximo de cuentas permitidas.
/// </summary>
/// <param name="nuevaCuenta"></param>
/// <param name="cedulaEsperada"></param>
/// <returns></returns>
bool Persona::crearAgregarCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada) {

	ingresarDatos(cedulaEsperada);
	// Verificación de puntero nulo primero
	if (!nuevaCuenta) {
		cout << "Error: Cuenta nula pasada como parámetro." << endl;
		return false;
	}

	try {
		// Generar número de cuenta
		Fecha fechaActual;
		std::string fechaStr = fechaActual.obtenerFechaFormateada();
		nuevaCuenta->setFechaApertura(fechaStr);
		nuevaCuenta->setEstadoCuenta("ACTIVA");

		// Obligatorio ingresar un monto inicial minimo de 250.00 USD
		double montoInicial = 0.0;
		do {
			montoInicial = PersonaUI::ingresarMonto(250.00, 15000.00, "Ingrese el monto inicial (minimo 250.00 USD, maximo 15000.00 USD): ");
		} while (montoInicial < 250.00);
		nuevaCuenta->depositar(montoInicial); // Depositar el monto inicial
		// No es necesario setear el saldo, ya que depositar lo hace automaticamente

		std::string numeroCuenta = crearNumeroCuenta(nuevaCuenta);
		if (numeroCuenta.empty()) {
			cout << "Error generando número de cuenta." << endl;
			return false;
		}

		// Mostrar información
		nuevaCuenta->mostrarInformacion(cedulaEsperada, true);

		//std::cout << "Antes de vincular: cabezaAhorros = " << (cabezaCorriente ? cabezaCorriente->getNumeroCuenta() : "NULL") << std::endl; // Depuración para verificar la vinculación

		// Agregar a la lista enlazada
		nuevaCuenta->setSiguiente(cabezaCorriente);
		nuevaCuenta->setAnterior(nullptr);
		if (cabezaCorriente != nullptr) {
			cabezaCorriente->setAnterior(nuevaCuenta);
		}
		this->cabezaCorriente = nuevaCuenta;
		this->numCuentas++;

		//std::cout << "Después de vincular: cabezaAhorros = " << (cabezaCorriente ? cabezaCorriente->getNumeroCuenta() : "NULL") << std::endl; // Depuración para verificar la vinculación

		std::cout << "---- Cuenta de Ahorros creada correctamente ----" << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al crear cuenta: " << e.what() << std::endl;
		return false;
	}

}

bool Persona::crearSoloCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada)
{
	const int MAX_CUENTAS = 5;

	// Verificación de puntero nulo primero
	if (!nuevaCuenta) {
		cout << "Error: Cuenta nula pasada como parámetro." << endl;
		return false;
	}

	// Verificar límite de cuentas
	if (this->numCuentas >= MAX_CUENTAS) {
		cout << "No se pueden agregar más cuentas, límite alcanzado." << endl;
		return false;
	}

	try {
		// Generar número de cuenta
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

		// Generar número de cuenta
		std::string numeroCuenta = crearNumeroCuenta(nuevaCuenta);
		if (numeroCuenta.empty()) {
			cout << "Error generando número de cuenta." << endl;
			return false;
		}

		// Mostrar información
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

bool Persona::crearSoloCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada)
{
	// Verificación de puntero nulo primero
	if (!nuevaCuenta) {
		cout << "Error: Cuenta nula pasada como parámetro." << endl;
		return false;
	}

	try {
		// Generar número de cuenta
		Fecha fechaActual;
		std::string fechaStr = fechaActual.obtenerFechaFormateada();
		nuevaCuenta->setFechaApertura(fechaStr);
		nuevaCuenta->setEstadoCuenta("ACTIVA");

		// Obligatorio ingresar un monto inicial minimo de 250.00 USD
		double montoInicial = 0.0;
		do {
			montoInicial = PersonaUI::ingresarMonto(250.00, 15000.00, "Ingrese el monto inicial (minimo 250.00 USD, maximo 15000.00 USD): ");
		} while (montoInicial < 250.00);
		nuevaCuenta->depositar(montoInicial); // Depositar el monto inicial
		// No es necesario setear el saldo, ya que depositar lo hace automaticamente

		std::string numeroCuenta = crearNumeroCuenta(nuevaCuenta);
		if (numeroCuenta.empty()) {
			cout << "Error generando número de cuenta." << endl;
			return false;
		}

		// Mostrar información
		nuevaCuenta->mostrarInformacion(cedulaEsperada, true);

		// Agregar a la lista enlazada
		nuevaCuenta->setSiguiente(cabezaCorriente);
		nuevaCuenta->setAnterior(nullptr);
		if (cabezaCorriente != nullptr) {
			cabezaCorriente->setAnterior(nuevaCuenta);
		}
		this->cabezaCorriente = nuevaCuenta;
		this->numCuentas++;

		std::cout << "---- Cuenta de Ahorros creada correctamente ----" << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al crear cuenta: " << e.what() << std::endl;
		return false;
	}
}

/// <summary>
/// Metodo para crear un numero de cuenta unico para una nueva cuenta, asegurando que no se repita con las cuentas existentes.
/// </summary>
/// <param name="nuevaCuenta"></param>
/// <returns></returns>
std::string Persona::crearNumeroCuenta(Cuenta<double>* nuevaCuenta) {
	std::vector<Cuenta<double>*> cuentas;
	bool numeroValido = false;
	std::string numeroCuentaStr;

	// Agregar cuentas de ahorro a la lista
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
	do {
		int mayorCentral = 0;

		for (auto cuenta : cuentas) {
			std::string numeroStr = cuenta->getNumeroCuenta();
			if (numeroStr.size() < 10) {
				numeroStr.insert(0, 10 - numeroStr.size(), '0');
			}

			if (numeroStr.substr(0, 3) == "090") {
				try {
					int parteCentral = std::stoi(numeroStr.substr(3, 6));
					if (parteCentral > mayorCentral) {
						mayorCentral = parteCentral;
					}
				}
				catch (const std::exception& e) {
					std::cerr << "Error en la generación del número de cuenta: " << e.what() << std::endl;
					return ""; // O un valor por defecto seguro
				}

			}
		}

		++mayorCentral;

		std::ostringstream oss;
		oss << std::setw(6) << std::setfill('0') << mayorCentral;
		std::string parteCuenta = oss.str();
		std::string base = "090" + parteCuenta;

		int suma = 0;
		for (char c : base) {
			suma += (c - '0');
		}
		int digitoControl = suma % 10;

		numeroCuentaStr = base + std::to_string(digitoControl);

		if (!Validar::ValidarNumeroCuenta(numeroCuentaStr)) {
			return ""; // Retornar cadena vacía indica error
		}
		// El código continúa si la validación es exitosa
		nuevaCuenta->setNumeroCuenta(numeroCuentaStr);
		numeroValido = true;

	} while (!numeroValido); // Si el numero no es valido, se genera otro

	return numeroCuentaStr;
}



