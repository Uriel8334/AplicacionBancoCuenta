#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>
#include <conio.h> // Para _getch()
#include <windows.h>
#include <functional>
#include "Validar.h"
#include "Fecha.h"
#include "Persona.h"
#include <string>

using namespace std;

// Constructor por defecto
Persona::Persona()
	: cedula(""), nombres(""), apellidos(""), fechaNacimiento(""), correo(""), direccion(""),
	cabezaAhorros(nullptr), cabezaCorriente(nullptr), numAhorros(0), isDestroyed(false) {
}

// Destructor para liberar memoria de las listas
Persona::~Persona() {
	// Marcamos el objeto como destruido
	isDestroyed = true;

	// Verificamos si aun existen nodos en las listas antes de liberar
	while (cabezaAhorros) {
		NodoCuentaAhorros* temp = cabezaAhorros;
		cabezaAhorros = cabezaAhorros->siguiente;
		delete temp;
	}
	while (cabezaCorriente) {
		NodoCuentaCorriente* temp = cabezaCorriente;
		cabezaCorriente = cabezaCorriente->siguiente;
		delete temp;
	}
}

// Metodos de la clase Persona

// Metodo para establecer la cedula de la persona
bool Persona::setCedula(const string& cedula) {
	if (cedula.length() == 10 && all_of(cedula.begin(), cedula.end(), ::isdigit)) {
		this->cedula = cedula;
		return true;
	}
	else {
		cout << "Cuidado: La cedula debe tener exactamente 10 digitos numericos.\n";
		return false;
	}
}

// Metodo para establecer los nombres de la persona
bool Persona::setNombres(const string& nombres) {
	if (!nombres.empty()) {
		this->nombres = nombres;
		return true;
	}
	else {
		cout << "Cuidado: Los nombres no pueden estar vacios.\n";
		return false;
	}
}

// Metodo para establecer los apellidos de la persona
bool Persona::setApellidos(const string& apellidos) {
	if (!apellidos.empty()) {
		this->apellidos = apellidos;
		return true;
	}
	else {
		cout << "Cuidado: Los apellidos no pueden estar vacios.\n";
		return false;
	}
}

// Metodo para establecer la fecha de nacimiento de la persona
bool Persona::setFechaNacimiento(const string& fechaNacimiento) {
	regex formato("\\d{2}/\\d{2}/\\d{4}");
	if (regex_match(fechaNacimiento, formato)) {
		this->fechaNacimiento = fechaNacimiento;
		return true;
	}
	else {
		cout << "Cuidado: La fecha debe tener el formato dd/mm/aaaa.\n";
		return false;
	}
}


// Metodo para establecer el correo de la persona
bool Persona::setCorreo(const string& correo) {
	if (!correo.empty()) {
		this->correo = correo;
		return true;
	}
	else {
		cout << "Cuidado: El correo no puede estar vacio.\n";
		return false;
	}
}

// Metodo para establecer la direccion de la persona
bool Persona::setDireccion(const string& direccion) {
	if (!direccion.empty()) {
		this->direccion = direccion;
		return true;
	}
	else {
		cout << "Cuidado: La direccion no puede estar vacia.\n";
		return false;
	}
}

// Getters
string Persona::getCedula() const { return cedula; }
string Persona::getNombres() const { return nombres; }
string Persona::getApellidos() const { return apellidos; }
string Persona::getFechaNacimiento() const { return fechaNacimiento; }
string Persona::getCorreo() const { return correo; }
string Persona::getDireccion() const { return direccion; }


// Metodo para ingresar Datos de la persona
void Persona::ingresarDatos() {
	ingresarDatos(this->cedula); // O puedes pasar "" si prefieres no validar
}

// Metodo para ingresar los datos de la persona
void Persona::ingresarDatos(const std::string& cedulaEsperada) {
	do {
#pragma region INGRESO CEDULA
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
			else if (cedula != cedulaEsperada) {
				cout << "La cedula no coincide con la ingresada previamente.\nPresione cualquier tecla para volver a intentarlo." << endl;
				int teclaCualquiera = _getch();
				(void)teclaCualquiera;
			}
			else {
				break; // Salir del bucle si la cedula es valida y coincide
			}
		} while (true);
#pragma endregion

#pragma region INGRESO NOMBRES
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
				// Letras (mayusculas o minusculas)
				else if (((tecla >= 'A' && tecla <= 'Z') || (tecla >= 'a' && tecla <= 'z'))) {
					nombres += tecla;
					espacioPermitido = true;
					cout << tecla;
				}
				// Un solo espacio entre palabras
				else if (tecla == ' ' && espacioPermitido) {
					nombres += tecla;
					espacioPermitido = false;
					cout << tecla;
				}
				// Ignorar cualquier otro caracter
			}
			bool nombresValidos = Validar::ValidarNombrePersona(nombres); // Validar los nombres
			if (!nombresValidos) {
				cout << "Nombre invalido, presione cualquier tecla para volver a ingresar." << endl;
				int teclaCualquiera = _getch();
				(void)teclaCualquiera;
			}
			else {
				break; // Salir del bucle si los nombres son validos
			}
		} while (true);
#pragma endregion

#pragma region INGRESO APELLIDOS
		// Ingreso apellidos y validacion
		do {
			system("cls");
			cout << "----- INGRESE SUS DATOS -----\n";
			cout << "Ingrese los apellidos: ";
			apellidos.clear();
			bool espacioPermitido = false; // Solo permite un espacio entre palabras
			while (true) {
				char tecla = _getch();

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
				// Letras (mayusculas o minusculas)
				else if (((tecla >= 'A' && tecla <= 'Z') || (tecla >= 'a' && tecla <= 'z'))) {
					apellidos += tecla;
					espacioPermitido = true;
					cout << tecla;
				}
				// Un solo espacio entre palabras
				else if (tecla == ' ' && espacioPermitido) {
					apellidos += tecla;
					espacioPermitido = false;
					cout << tecla;
				}
				// Ignorar cualquier otro caracter
			}
			bool apellidosValidos = Validar::ValidarNombrePersona(apellidos); // Validar los nombres
			if (!apellidosValidos) {
				cout << "Nombre invalido, presione cualquier tecla para volver a ingresar." << endl;
				int teclaCualquiera = _getch();
				(void)teclaCualquiera;
			}
			else {
				break; // Salir del bucle si los nombres son validos
			}
		} while (true);
#pragma endregion

#pragma region INGRESO FECHA NACIMIENTO
		// Ingreso fecha de nacimiento y validacion con cursor
		// Llamar al metodo para seleccionar la fecha de nacimiento
		seleccionarFecha();



		/*do {
			system("cls");
			cout << "----- INGRESE SUS DATOS -----\n";
			cout << "Seleccione la fecha de nacimiento usando las flechas. ENTER para aceptar." << endl;

			SYSTEMTIME st;
			GetLocalTime(&st);
			int anioActual = st.wYear;
			int mesActual = st.wMonth;
			int diaActual = st.wDay;

			int dia = 1, mes = 1, anio = 2000;
			int campo = 0; // 0: dia, 1: mes, 2: año
			bool fechaSeleccionada = false;

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
				// Limpiar la pantalla antes de mostrar la fecha y el mensaje
				system("cls");
				cout << "----- INGRESE SUS DATOS -----\n";
				cout << "Seleccione la fecha de nacimiento usando las flechas. ENTER para aceptar." << endl;

				// Mostrar la fecha con el campo seleccionado resaltado
				for (int i = 0; i < 3; ++i) {
					if (i == campo) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					}
					if (i == 0) cout << (dia < 10 ? "0" : "") << dia;
					if (i == 1) cout << "/" << (mes < 10 ? "0" : "") << mes;
					if (i == 2) cout << "/" << anio;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				}
				cout << endl;
				cout.flush();

				//int tecla = _getch();
				//if (tecla == 224) { // Tecla especial
				//	tecla = _getch();
				//	switch (tecla) {
				//	case 75: // Izquierda
				//		if (campo > 0) campo--;
				//		break;
				//	case 77: // Derecha
				//		if (campo < 2) campo++;
				//		break;
				//	case 72: // Arriba
				//		if (campo == 0) { // Dia
				//			int maxDia = diasEnMes(mes, anio);
				//			dia++;
				//			if (dia > maxDia) dia = 1;
				//		}
				//		else if (campo == 1) { // Mes
				//			mes++;
				//			if (mes > 12) mes = 1;
				//			int maxDia = diasEnMes(mes, anio);
				//			if (dia > maxDia) dia = maxDia;
				//		}
				//		else if (campo == 2) { // Año
				//			anio++;
				//			if (anio > anioActual) anio = 1900;
				//			int maxDia = diasEnMes(mes, anio);
				//			if (dia > maxDia) dia = maxDia;
				//			if (anio == anioActual && mes > mesActual) mes = mesActual;
				//			if (anio == anioActual && mes == mesActual && dia > diaActual) dia = diaActual;
				//		}
				//		break;
				//	case 80: // Abajo
				//		if (campo == 0) {
				//			int maxDia = diasEnMes(mes, anio);
				//			dia--;
				//			if (dia < 1) dia = maxDia;
				//		}
				//		else if (campo == 1) {
				//			mes--;
				//			if (mes < 1) mes = 12;
				//			int maxDia = diasEnMes(mes, anio);
				//			if (dia > maxDia) dia = maxDia;
				//			if (anio == anioActual && mes > mesActual) mes = mesActual;
				//			if (anio == anioActual && mes == mesActual && dia > diaActual) dia = diaActual;
				//		}
				//		else if (campo == 2) {
				//			anio--;
				//			if (anio < 1900) anio = anioActual;
				//			int maxDia = diasEnMes(mes, anio);
				//			if (dia > maxDia) dia = maxDia;
				//			if (anio == anioActual && mes > mesActual) mes = mesActual;
				//			if (anio == anioActual && mes == mesActual && dia > diaActual) dia = diaActual;
				//		}
				//		break;
				//	}
				//}
				else if (tecla == 13) { // Enter
					// Validar la fecha y que no sea mayor a la del sistema
					char buffer[11];
					snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", dia, mes, anio);
					string fechaStr(buffer);
					bool fechaValida = Validar::ValidarFecha(fechaStr);
					bool fechaNoFutura = (anio < anioActual) ||
						(anio == anioActual && mes < mesActual) ||
						(anio == anioActual && mes == mesActual && dia <= diaActual);
					if (fechaValida && fechaNoFutura) {
						fechaNacimiento = fechaStr;
						fechaSeleccionada = true;
					}
					else {
						// Limpiar pantalla antes de mostrar el mensaje de error
						system("cls");
						cout << "----- INGRESE SUS DATOS -----\n";
						cout << "Seleccione la fecha de nacimiento usando las flechas. ENTER para aceptar." << endl;
						cout << "\nFecha invalida o futura. Presione cualquier tecla para volver a seleccionar.";
						int teclaCualquiera = _getch();
						(void)teclaCualquiera;
					}
				}
			}
			break;
		} while (true);*/

#pragma endregion

#pragma region INGRESO CORREO
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
					_getch();
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
			if (!formatoValido) {
				cout << "Correo invalido, presione cualquier tecla para volver a ingresar." << endl;
				int teclaCualquiera = _getch();
				(void)teclaCualquiera;
			}
			else {
				break;
			}
		} while (true);
#pragma endregion

#pragma region INGRESO DIRECCION
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
					_getch();
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
#pragma endregion

#pragma region MOSTRAR DATOS
		// Si el usuario considera que los datos son correctos, 
		// selecciona "Si" para setear los datos o "No" para repetir

		// Mostrar los datos ingresados y preguntar si desea corregirlos
		system("cls");
		mostrarDatos();
		cout << endl << "Desea corregir los datos ingresados?" << endl;

		const char* opciones[2] = { "Si", "No" };
		int seleccion = 0;
		int tecla = 0;

		do {
			// Mostrar opciones
			for (int i = 0; i < 2; ++i) {
				if (i == seleccion) {
					// Resalta la opcion seleccionada
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
			// Flecha izquierda o derecha
			if (tecla == 224) {
				tecla = _getch();
				if (tecla == 75 && seleccion > 0) { // Izquierda
					--seleccion;
				}
				else if (tecla == 77 && seleccion < 1) { // Derecha
					++seleccion;
				}
			}
		} while (tecla != 13); // Enter

		cout << endl;
		// Si selecciona "Si", repetir el ingreso de datos
		if (seleccion == 0) { // Si selecciona "Si"
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
#pragma endregion
}

// Metodos para mostrar los datos de la persona
void Persona::mostrarDatos() const {
	cout << "\n----- DATOS DEL USUARIO -----\n";
	cout << "Cedula: " << cedula << endl;
	cout << "Nombres: " << nombres << endl;
	cout << "Apellidos: " << apellidos << endl;
	cout << "Fecha de nacimiento: " << fechaNacimiento << endl;
	cout << "Correo: " << correo << endl;
	cout << "Direccion: " << direccion << endl;
}

// Metodo para guardar los datos de la persona en un archivo
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

int Persona::buscarPersonaPorCriterio(const std::string& criterio, const std::string& valorStr, double valorNum) const {
    int cuentasEncontradas = 0;
    bool mostrarDatosTitular = false;
	Persona persona;

    
    // Buscar en cuentas de ahorro
    NodoCuentaAhorros* actualAhorros = getListaCuentasAhorros();
    while (actualAhorros) {
        bool encontrado = false;
        
        if (criterio == "Numero de cuenta" && std::to_string(actualAhorros->cuenta->getNumeroCuenta()) == valorStr)
            encontrado = true;
        else if (criterio == "Fecha de apertura" && actualAhorros->cuenta->getFechaApertura() == valorStr)
            encontrado = true;
        else if (criterio == "Saldo mayor a" && 
                 (actualAhorros->cuenta->consultarSaldo() / 100.0) > valorNum)
            encontrado = true;
        else if ((criterio == "Tipo de cuenta" && valorStr == "Ahorros") ||
                 (criterio == "Tipo de cuenta" && valorStr == "Ambas"))
            encontrado = true;
        
        if (encontrado) {
            // Si es la primera cuenta encontrada, mostrar datos del titular
            if (!mostrarDatosTitular) {
                std::cout << "\n===== DATOS DEL TITULAR =====\n";
                std::cout << "Cédula: " << this->cedula << std::endl;
                std::cout << "Nombre: " << this->nombres << " " << this->apellidos << std::endl;
                std::cout << "Correo: " << this->correo << std::endl;
                std::cout << "Dirección: " << this->direccion << std::endl;
                std::cout << std::string(30, '-') << std::endl;
                mostrarDatosTitular = true;
            }
            
            // Mostrar información de la cuenta
            std::cout << "\nCUENTA DE AHORROS:\n";
            actualAhorros->cuenta->mostrarInformacion(this->cedula);
            cuentasEncontradas++;
        }
        actualAhorros = actualAhorros->siguiente;
    }
    
    // Buscar en cuentas corrientes
	Persona* personaPtr = &persona; // Asegurarse de que personaPtr no sea nulo
	NodoCuentaCorriente* actualCorriente = personaPtr->getListaCuentasCorriente();
    while (actualCorriente) {
        bool encontrado = false;
        
        if (criterio == "Numero de cuenta" && std::to_string(actualCorriente->cuenta->getNumeroCuenta()) == valorStr)
            encontrado = true;
        else if (criterio == "Fecha de apertura" && actualCorriente->cuenta->getFechaApertura() == valorStr)
            encontrado = true;
        else if (criterio == "Saldo mayor a" && 
                 (actualCorriente->cuenta->consultarSaldo() / 100.0) > valorNum)
            encontrado = true;
        else if ((criterio == "Tipo de cuenta" && valorStr == "Corriente") ||
                 (criterio == "Tipo de cuenta" && valorStr == "Ambas"))
            encontrado = true;
        
        if (encontrado) {
            // Si es la primera cuenta encontrada, mostrar datos del titular
            if (!mostrarDatosTitular) {
                std::cout << "\n===== DATOS DEL TITULAR =====\n";
                std::cout << "Cédula: " << this->cedula << std::endl;
                std::cout << "Nombre: " << this->nombres << " " << this->apellidos << std::endl;
                std::cout << "Correo: " << this->correo << std::endl;
                std::cout << "Dirección: " << this->direccion << std::endl;
                std::cout << std::string(30, '-') << std::endl;
                mostrarDatosTitular = true;
            }
            
            // Mostrar información de la cuenta
            std::cout << "\nCUENTA CORRIENTE:\n";
            actualCorriente->cuenta->mostrarInformacion(this->cedula);
            cuentasEncontradas++;
        }
        actualCorriente = actualCorriente->siguiente;
    }
    
    return cuentasEncontradas;
}

void Persona::buscarPersonaPorFecha(const std::string& fecha) const {
	// Agregar validacion de instancia destruida
	if (!isValidInstance()) {
		return;
	}

	int encontrados = 0;
	bool datosPersonalesMostrados = false;

	// Funcion para mostrar datos personales solo una vez
	auto mostrarDatosPersonales = [&]() {
		if (!datosPersonalesMostrados) {
			std::cout << "\n----- DATOS DEL TITULAR -----\n";
			std::cout << "Cedula: " << this->cedula << std::endl;
			std::cout << "Nombres: " << this->nombres << " " << this->apellidos << std::endl;
			std::cout << "Correo: " << this->correo << std::endl;
			std::cout << "Direccion: " << this->direccion << std::endl;
			std::cout << "Fecha de nacimiento: " << this->fechaNacimiento << std::endl;
			std::cout << std::string(30, '-') << std::endl;

			datosPersonalesMostrados = true;
		}
		};
	Persona* persona; // Asegurarse de que persona no sea nulo
	NodoCuentaAhorros* actualAhorros = persona->getListaCuentasAhorros();
	while (actualAhorros) {
		// Verificar puntero a cuenta
		if (!actualAhorros->cuenta) {
			actualAhorros = actualAhorros->siguiente;
			continue;
		}
		// Verificar coincidencia de fecha
		if (actualAhorros->cuenta->getFechaApertura() == fecha) {
			// Mostrar datos personales antes de la primera cuenta
			mostrarDatosPersonales();

			std::cout << "\nCUENTA DE AHORROS #" << (encontrados + 1) << ":\n";
			actualAhorros->cuenta->mostrarInformacion(this->cedula);
			encontrados++;
		}
		actualAhorros = actualAhorros->siguiente;
	}
	Persona* persona;
	NodoCuentaCorriente* actualCorriente = persona->getListaCuentasCorriente();
	while (actualCorriente) {
		if (!actualCorriente->cuenta) {
			actualCorriente = actualCorriente->siguiente;
			continue;
		}
		if (actualCorriente->cuenta->getFechaApertura() == fecha) {
			// Mostrar datos personales antes de la primera cuenta
			mostrarDatosPersonales();

			std::cout << "\nCUENTA CORRIENTE #" << (encontrados + 1) << ":\n";
			actualCorriente->cuenta->mostrarInformacion(this->cedula);
			encontrados++;
		}
		actualCorriente = actualCorriente->siguiente;
	}

	if (encontrados == 0) {
		std::cout << "No se encontraron cuentas con esa fecha para esta persona.\n";
	}
	else {
		std::cout << "\nTotal de cuentas encontradas: " << encontrados << std::endl;
	}
}

int Persona::buscarPersonaPorCuentas(const string& numeroCuenta) const {
    // Evita acceso a un objeto destruido
    if (!isValidInstance()) {
        //std::cerr << "Error: objeto 'Persona' invalido (destruido o no inicializado).\n";
        return 0;
    }

    // Validar longitud y tipo de caracteres
    if (numeroCuenta.size() > 10 || !std::all_of(numeroCuenta.begin(), numeroCuenta.end(), ::isdigit)) {
        std::cerr << "Numero de cuenta invalido (maximo 10 digitos, solo numeros).\n";
        return 0;
    }

    int encontrados = 0;

    // Recorrer lista de ahorro con verificaciones robustas
	Persona* persona; // Asegurarse de que persona no sea nulo
    NodoCuentaAhorros* actualA = persona->getListaCuentasAhorros();
    while (actualA) {
        if (!actualA->cuenta || 
            reinterpret_cast<uintptr_t>(actualA->cuenta) > 0xFFFFFFFF00000000) {
			//std::cout << "[DEBUG] Referencia invalida a cuenta" << std::endl; // Depuracion
            actualA = actualA->siguiente;
            continue;
        }
        
        try {
            int numCuentaTemp = actualA->cuenta->getNumeroCuenta();
            if (std::to_string(numCuentaTemp) == numeroCuenta) {
                actualA->cuenta->mostrarInformacion();
                encontrados++;
            }
        } catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl; // Depuracion
        }
        
        actualA = actualA->siguiente;
    }

    // Recorrer lista corriente con verificaciones robustas
	Persona* persona;
	NodoCuentaCorriente* actualC = persona->getListaCuentasCorriente();
    while (actualC) {
        if (!actualC->cuenta || reinterpret_cast<uintptr_t>(actualC->cuenta) == 0xFFFFFFFFFFFFFFFF) {
            actualC = actualC->siguiente;
            continue;
        }
        
        try {
            int numCuentaTemp = actualC->cuenta->getNumeroCuenta();
            if (std::to_string(numCuentaTemp) == numeroCuenta) {
                actualC->cuenta->mostrarInformacion();
                encontrados++;
            }
        } catch (...) {
            std::cerr << "Error al acceder a cuenta corriente\n";
        }
        
        actualC = actualC->siguiente;
    }

    if (encontrados == 0) {
        // El mensaje se muestra ahora en el metodo que llama a este
    }
    
    return encontrados;
}

// Metodo para guardar las cuentas de ahorro en el archivo
int Persona::guardarCuentasAhorro(std::ofstream& archivo) const {
	if (!archivo.is_open() || !isValidInstance()) {
		return 0;
	}
	
	int contador = 0;
	Persona* persona; // Asegurarse de que persona no sea nulo
	NodoCuentaAhorros* actual = persona->getListaCuentasAhorros();

	while (actual) {
		if (actual->cuenta) {
			archivo << "CUENTA_AHORRO_INICIO\n";
			archivo << "NUMERO_CUENTA:" << actual->cuenta->getNumeroCuenta() << "\n";
			archivo << "SALDO:" << actual->cuenta->consultarSaldo() << "\n";
			archivo << "FECHA_APERTURA:" << actual->cuenta->getFechaApertura() << "\n";
			archivo << "ESTADO:" << actual->cuenta->consultarEstado() << "\n";
			// Aqui se podrian añadir otros campos especificos de CuentaAhorros
			archivo << "CUENTA_AHORRO_FIN\n";
			contador++;
		}
		actual = actual->siguiente;
	}

	return contador;
}

// Metodo para guardar las cuentas corrientes en el archivo
int Persona::guardarCuentasCorriente(std::ofstream& archivo) const {
	if (!archivo.is_open() || !isValidInstance()) {
		return 0;
	}

	int contador = 0;
	Persona* persona; // Asegurarse de que persona no sea nulo
	NodoCuentaCorriente* actual = persona->getListaCuentasCorriente();

	while (actual) {
		if (actual->cuenta) {
			archivo << "CUENTA_CORRIENTE_INICIO\n";
			archivo << "NUMERO_CUENTA:" << actual->cuenta->getNumeroCuenta() << "\n";
			archivo << "SALDO:" << actual->cuenta->consultarSaldo() << "\n";
			archivo << "FECHA_APERTURA:" << actual->cuenta->getFechaApertura() << "\n";
			archivo << "ESTADO:" << actual->cuenta->consultarEstado() << "\n";
			// Aqui se podrian añadir otros campos especificos de CuentaCorriente
			archivo << "CUENTA_CORRIENTE_FIN\n";
			contador++;
		}
		actual = actual->siguiente;
	}

	return contador;
}

void Persona::seleccionarFecha() {
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
}

