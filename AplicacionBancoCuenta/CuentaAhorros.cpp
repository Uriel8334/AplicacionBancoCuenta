#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <locale>
#include <windows.h>
#include <sstream>
#include <conio.h>
#include <climits> // For INT_MAX
#include <cmath>
#include "Validar.h" // Include validation class
#include "Cifrado.h" // Include encryption class
#include "Fecha.h"   // Include date class
#include "Utilidades.h" // Include utilities class
#include "NodoCuentaAhorros.h" // Add this include for the full node definition
#include "CuentaAhorros.h" // Include the header for CuentaAhorros class

using namespace std;

CuentaAhorros::CuentaAhorros() : saldo(0.0), numeroCuenta(""), fechaApertura("") {}

CuentaAhorros::CuentaAhorros(double saldo, const string& numeroCuenta, const string& fechaApertura)
	: saldo(saldo), numeroCuenta(numeroCuenta), fechaApertura(fechaApertura) {
}

	CuentaCorriente() : Cuenta<int>(), montoMinimo(0) {}
	CuentaCorriente(int numCuenta, int sal, std::string fecha, std::string estado, double monto)
		: Cuenta<int>(numCuenta, sal, fecha, estado), montoMinimo(monto) {}

	void depositar(int cantidad) override {
		this->saldo += cantidad;
	}

	void retirar(int cantidad) override {
		if (cantidad <= this->saldo) {
			this->saldo -= cantidad;
		}
		else {
			std::cout << "Fondos insuficientes." << std::endl;
		}
	}

	int consultarSaldo() const override {
		return this->saldo;
	}

	std::string consultarEstado() const override {
		return this->estadoCuenta;
	}

	// En la clase CuentaCorriente, implementamos nuestro metodo para formatear el saldo
	std::string formatearSaldo() const override {
		return formatearConComas(this->saldo);
	}

	// Metodo para formatear con comas
	std::string formatearConComas(int valorEnCentavos) const {
		// Convertir de centavos a valor decimal
		double valorReal = valorEnCentavos / 100.0;
		std::ostringstream oss;
		oss.imbue(std::locale("en_US.UTF-8")); // Usar formato americano: 1,234.56
		oss << std::fixed << std::setprecision(2) << valorReal;
		return oss.str();
	}

	// Añadimos el segundo método formatearConComas para valores double, igual que en CuentaAhorros
	std::string formatearConComas(double valor) const {
		std::ostringstream oss;
		oss.imbue(std::locale(""));
		oss << std::fixed << std::setprecision(2) << valor;
		return oss.str();
	}

	// Modificamos mostrarInformacion para usar el método de la clase en lugar de una lambda
	void mostrarInformacion(const std::string& cedula = "", bool limpiarPantalla = true) const override {
		// Validacion basica
		if (this == nullptr) {
			return;
		}

		// Limpieza de pantalla solo si se solicita
		if (limpiarPantalla) {
			system("cls");
		}

		// Título con formato específico para CuentaCorriente
		std::cout << "\n" << std::string(50, '=') << std::endl;
		std::cout << "          INFORMACION DE CUENTA CORRIENTE" << std::endl;
		std::cout << std::string(50, '=') << "\n" << std::endl;

		// Información del titular si está disponible
		if (!cedula.empty()) {
			std::cout << "Cedula del titular: " << cedula << std::endl;
			std::cout << std::string(30, '-') << std::endl;
		}

		// Información común
		std::cout << "Tipo de cuenta: CORRIENTE" << std::endl;
		std::cout << "Numero de cuenta: " << this->numeroCuenta << std::endl;
		std::cout << "Fecha de apertura: " << this->fechaApertura << std::endl;
		std::cout << "Estado: " << this->estadoCuenta << std::endl;

		// Usando el método de clase para formatear, eliminamos la lambda
		std::cout << "Saldo actual: $" << formatearConComas(this->saldo) << std::endl;

		// Pie de página
		std::cout << "\n" << std::string(50, '-') << std::endl;
		std::cout << "Presione cualquier tecla para continuar..." << std::endl;
		int tecla = _getch();
		(void)tecla;
		// Limpieza de pantalla
		system("cls");
	}

	void guardarEnArchivo(const std::string& nombreArchivo) const override {
		std::ofstream archivo(nombreArchivo, std::ios::binary);
		if (archivo.is_open()) {
			archivo.write(reinterpret_cast<const char*>(&this->numeroCuenta), sizeof(this->numeroCuenta));
			archivo.write(reinterpret_cast<const char*>(&this->saldo), sizeof(this->saldo));
			archivo.write(this->fechaApertura.c_str(), this->fechaApertura.size() + 1);
			archivo.write(this->estadoCuenta.c_str(), this->estadoCuenta.size() + 1);
			archivo.write(reinterpret_cast<const char*>(&montoMinimo), sizeof(montoMinimo));
			archivo.close();
		}
	}

	void cargarDesdeArchivo(const std::string& nombreArchivo) override {
		std::ifstream archivo(nombreArchivo, std::ios::binary);
		if (archivo.is_open()) {
			archivo.read(reinterpret_cast<char*>(&this->numeroCuenta), sizeof(this->numeroCuenta));
			archivo.read(reinterpret_cast<char*>(&this->saldo), sizeof(this->saldo));
			char buffer[100];
			archivo.getline(buffer, 100, '\0');
			this->fechaApertura = buffer;
			archivo.getline(buffer, 100, '\0');
			this->estadoCuenta = buffer;
			archivo.read(reinterpret_cast<char*>(&montoMinimo), sizeof(montoMinimo));
			archivo.close();
		}
	}

	// Metodo para crear una cuenta corriente
	void crearCuenta(Persona* persona) {
		system("cls");
		std::cout << "\n----- DATOS DE LA CUENTA -----\n" << std::endl;

		// Generar un numero de cuenta unico que se pueda almacenar en un int (maximo 9 digitos)
		// El valor maximo de int es aproximadamente 2.147.483.647 (unos 10 digitos)
		std::string numeroCuentaStr;
		numeroCuentaStr.clear();

		// Generamos un numero de 9 digitos para evitar overflow en int
		for (int i = 0; i < 9; ++i) {
			char digito = '0' + (rand() % 10);
			// El primer digito no debe ser cero
			if (i == 0 && digito == '0') {
				digito = '1' + (rand() % 9);
			}
			numeroCuentaStr += digito;
		}

		try {
			// Convertimos usando stoi que guarda directamente como int
			// stoll convierte a long long y luego el static_cast puede causar overflow
			this->numeroCuenta = std::stoi(numeroCuentaStr);

			// Verificamos que el numero sea positivo
			if (this->numeroCuenta <= 0) {
				// Si por alguna razon el numero es negativo o cero, usamos un valor predeterminado
				this->numeroCuenta = 100000000 + (rand() % 899999999); // Entre 100M y 999M
			}

			// Actualizamos la cadena para mostrar el numero real almacenado
			numeroCuentaStr = std::to_string(this->numeroCuenta);
		}
		catch (const std::exception& e) {
			std::cerr << "Error al convertir el numero de cuenta: " << e.what() << "\n";
			// Generamos un numero seguro en caso de error
			this->numeroCuenta = 100000000 + (rand() % 899999999); // Entre 100M y 999M
			numeroCuentaStr = std::to_string(this->numeroCuenta);
		}

		std::cout << "Numero de cuenta: " << numeroCuentaStr << std::endl;

		// Saldo inicial de la cuenta
		this->saldo = 0;
		// Usar el método de clase en lugar de una lambda
		std::cout << "Saldo inicial: $" << formatearConComas(this->saldo) << std::endl;

		// Fecha de apertura de la cuenta
		Fecha fechaSistema;
		this->fechaApertura = fechaSistema.obtenerFechaFormateada();
		std::cout << "Fecha de apertura: " << this->fechaApertura << std::endl;

		// Estado de la cuenta
		this->estadoCuenta = "Activa";
		std::cout << "Estado de la cuenta: " << this->estadoCuenta << std::endl;

		// Preguntar si desea ingresar un monto inicial a la cuenta
		std::cout << "\nDesea ingresar un monto inicial a su cuenta? " << std::endl;

		// Opciones para el menu
		int opcionSeleccionada = 0;
		bool continuar = true;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hConsole, &csbi);

		while (continuar) {
			// Mostrar las opciones
			for (int i = 0; i < 2; i++) {
				COORD pos = { 5, static_cast<SHORT>(csbi.dwCursorPosition.Y + i + 1) };
				SetConsoleCursorPosition(hConsole, pos);

				if (i == opcionSeleccionada) {
					SetConsoleTextAttribute(hConsole, 0x70); //
				}
				else {
					SetConsoleTextAttribute(hConsole, 0x07); //
				}

				if (i == 0) {
					std::cout << "1. Si" << std::endl;
				}
				else {
					std::cout << "2. No" << std::endl;
				}
			}

			// Obtener la tecla presionada
			int tecla = _getch();

			switch (tecla) {
			case 72: // Flecha arriba
				opcionSeleccionada = (opcionSeleccionada == 0) ? 1 : 0;
				break;
			case 80: // Flecha abajo
				opcionSeleccionada = (opcionSeleccionada == 1) ? 0 : 1;
				break;
			case 13: // Enter
				continuar = false;
				break;
			default:
				break;
			}
		}

		// Limpiar la pantalla
		system("cls");

		// Si el usuario selecciona "Si"
		if (opcionSeleccionada == 0) {
			std::string entrada;
			bool entradaValida = false;
			bool tienePunto = false;

			while (!entradaValida) {
				std::cout << "Ingrese el monto inicial (en dolares): ";
				std::getline(std::cin, entrada);

				// Validar que la entrada sea un número válido
				entradaValida = true;
				tienePunto = false;
				for (char c : entrada) {
					if (!isdigit(c)) {
						if (c == '.' && !tienePunto) {
							tienePunto = true;
						}
						else {
							entradaValida = false;
							break;
						}
					}
				}

				if (!entradaValida) {
					std::cout << "Entrada invalida. Por favor, ingrese un numero valido." << std::endl;
				}
			}

			// Realizar el depósito
			// Corregimos la conversión de dólares a centavos
			int centavos = 0;

			if (tienePunto) {
				// Si el valor tiene punto decimal, procesamos por separado parte entera y decimal
				size_t puntoPos = entrada.find('.');
				std::string parteEntera = entrada.substr(0, puntoPos);
				std::string parteDecimal = entrada.substr(puntoPos + 1);
				// Ajustamos la parte decimal a 2 dígitos
				if (parteDecimal.length() == 1) parteDecimal += "0";
				else if (parteDecimal.length() > 2) parteDecimal = parteDecimal.substr(0, 2);

				// Convertimos a centavos
				centavos = std::stoi(parteEntera) * 100;
				if (!parteDecimal.empty()) {
					centavos += std::stoi(parteDecimal);
				}
			}
			else {
				// Si es un número entero, multiplicamos por 100
				centavos = std::stoi(entrada) * 100;
			}

			// Añadimos debug para verificar el valor
			/*std::cout << "DEBUG - Valor ingresado: " << entrada << std::endl;
			std::cout << "DEBUG - Centavos calculados: " << centavos << std::endl;*/

			// Asignamos los centavos al saldo
			this->saldo += centavos;
			std::cout << "Depósito realizado con éxito." << std::endl;
			std::cout << "Nuevo saldo: $" << formatearConComas(this->saldo) << std::endl;
		}

		// Pie de página
		std::cout << "\n" << std::string(50, '-') << std::endl;
		std::cout << "Presione cualquier tecla para continuar..." << std::endl;
		int tecla = _getch();
		(void)tecla;
		// Limpieza de pantalla
		system("cls");
	}

