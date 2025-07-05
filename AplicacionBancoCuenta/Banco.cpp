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

// Constructor y destructor de la clase Banco
Banco::Banco() : listaPersonas(nullptr) {} // Constructor

Banco::~Banco() { // Destructor
	// Liberar memoria de la lista enlazada
	NodoPersona* actual = listaPersonas;
	while (actual) {
		NodoPersona* temp = actual;
		actual = actual->siguiente;
		delete temp;
	}
}

// Metodo para agregar una persona con cuenta
void Banco::agregarPersonaConCuenta() {
	// variables locales

	std::string opciones[] = { "Cuenta de Ahorros", "Cuenta Corriente", "Cancelar" };
	int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
	int seleccion = 0;
	// Creamos la persona dinamicamente
	Persona* persona = new Persona();
	int x = 5; // Posicion X inicial del menu
	int y = 5; // Posicion Y inicial del menu
	// Pasamos el puntero al constructor
	NodoPersona* nuevo = new NodoPersona(persona);
	nuevo->siguiente = listaPersonas;
	listaPersonas = nuevo;

	// Menu con cursor
	while (true) {
		system("cls");
		std::cout << "\n\nSeleccione el tipo de cuenta a crear para la persona:\n\n";
		for (int i = 0; i < numOpciones; i++) {
			if (i == seleccion)
				std::cout << " > " << opciones[i] << std::endl;
			else
				std::cout << "   " << opciones[i] << std::endl;
		}
		// Limpia cualquier linea sobrante si el menu se reduce
		Utilidades::gotoxy(0, y + numOpciones);

		int tecla = _getch();
		if (tecla == 224) { // Teclas especiales
			tecla = _getch();
			if (tecla == 72) // Flecha arriba
				seleccion = (seleccion - 1 + numOpciones) % numOpciones;
			else if (tecla == 80) // Flecha abajo
				seleccion = (seleccion + 1) % numOpciones;
		}
		else if (tecla == 13) { // ENTER
			break;
		}
	}

	if (seleccion == 2) {
		std::cout << "Operacion cancelada.\n";
		return;
	}

	// Ingreso de datos de la persona con cursor
	if (seleccion == 0 || seleccion == 1) {
		std::cout << "\nPresione ENTER para confirmar y crear la cuenta, o ESC para cancelar...\n";
		int tecla = _getch();
		if (tecla == 27) { // ESC
			std::cout << "Operacion cancelada por el usuario.\n";
			return;
		}

		// Selecciona el tipo de cuenta AHORROS
		if (seleccion == 0) {
			CuentaAhorros* nuevaCuentaAhorros = nullptr;

			// Solicitar cedula antes de ingresar todos los datos
			std::string cedulaTemp;
			while (true) {
				system("cls");
				std::cout << "\n\n----- INGRESE SUS DATOS -----\n";
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
				}
				if (!Validar::ValidarCedula(cedulaTemp)) {
					std::cout << "Cedula invalida, presione cualquier tecla para volver a ingresar." << std::endl;
					int teclaCualquiera = _getch();
					(void)teclaCualquiera;
					continue;
				}

				// Verificar si la cedula ya existe
				bool existe = false;
				NodoPersona* actual = listaPersonas;
				while (actual) {
					Persona* p = actual->persona;
					if (p && p->getCedula() == cedulaTemp) {
						existe = true;
						break;
					}
					actual = actual->siguiente;
				}
				if (existe) {
					const char* opciones[2] = { "Si", "No" };
					int seleccion = 1; // Por defecto "No"
					int tecla = 0;

					// Guardar una referencia a la persona existente con la cedula
					Persona* personaExistente = nullptr;
					NodoPersona* nodoActual = listaPersonas;
					while (nodoActual) {
						if (nodoActual->persona && nodoActual->persona->getCedula() == cedulaTemp) {
							personaExistente = nodoActual->persona;
							break;
						}
						nodoActual = nodoActual->siguiente;
					}

					while (true) {
						system("cls");
						if (personaExistente == nullptr) {
							std::cout << "Error: El puntero 'personaExistente' es NULL.\n";
							return;
						}
						std::cout << "La cedula " << cedulaTemp << " ya esta registrada en el sistema.\n";
						std::cout << "Titular: " << personaExistente->getNombres() << " " << personaExistente->getApellidos() << "\n\n";
						std::cout << "Desea agregar una nueva cuenta para este titular?\n\n";
						for (int i = 0; i < 2; ++i) {
							if (i == seleccion) {
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
									BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN |
									FOREGROUND_BLUE | FOREGROUND_INTENSITY);
								std::cout << " > " << opciones[i] << " < ";
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
									FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
							}
							else {
								std::cout << "   " << opciones[i] << "   ";
							}
						}
						std::cout << "\r";
						tecla = _getch();
						if (tecla == 224) {
							tecla = _getch();
							if (tecla == 75 && seleccion > 0) --seleccion; // Izquierda
							else if (tecla == 77 && seleccion < 1) ++seleccion; // Derecha
						}
						else if (tecla == 13) break; // Enter
					}
					std::cout << std::endl;

					if (seleccion == 1) { // No
						std::cout << "Operacion cancelada.\n";
						// Eliminar el nodo y la persona recien creados para evitar memoria no utilizada
						NodoPersona* temp = listaPersonas;
						listaPersonas = listaPersonas->siguiente;
						delete temp;
						return;
					}
					else { // Si - usar la persona existente
						delete persona;
						nuevo->persona = personaExistente;
						persona = personaExistente;
						std::cout << "Usando datos de titular existente.\n";
						system("pause");

						try {
							Fecha fechaActual;
							std::string fechaStr = fechaActual.obtenerFechaFormateada();
							nuevaCuentaAhorros = new CuentaAhorros("0", 0.0, fechaStr, "Activa", 5.0);

							bool resultado = persona->crearSoloCuentaAhorros(nuevaCuentaAhorros, cedulaTemp);
							if (!resultado) {
								delete nuevaCuentaAhorros; // Evitar fugas de memoria
							}
						}
						catch (const std::exception& e) {
							std::cerr << "Error: " << e.what() << std::endl;
							system("pause");
							return;
						}
						//std::cout << "Cuenta de ahorros creada correctamente para el titular existente.\n";

						system("pause");
						return;
					}
				}
				// Si no existe o selecciona "Si" (usando persona existente), continuar
				break;
			}

			// Esta seccion se ejecuta solo para personas nuevas
			persona->setCedula(cedulaTemp);
			try {
				Fecha fechaActual;
				std::string fechaStr = fechaActual.obtenerFechaFormateada();
				nuevaCuentaAhorros = new CuentaAhorros("0", 0.0, fechaStr, "Activa", 5.0);

				bool resultado = persona->crearAgregarCuentaAhorros(nuevaCuentaAhorros, cedulaTemp);
				if (resultado) {
					CuentaAhorros* verificacion = persona->getCabezaAhorros();
					if (verificacion) {
						//std::cout << "Cuenta vinculada correctamente: " << verificacion->getNumeroCuenta() << std::endl;
					}
					else {
						//std::cout << "Error: La cuenta no se vinculo correctamente" << std::endl;
					}
				}
				if (!resultado) {
					delete nuevaCuentaAhorros; // Evitar fugas de memoria
				}
			}
			catch (const std::exception& e) {
				std::cerr << "Error: " << e.what() << std::endl;
				system("pause");
				return;
			}
			std::cout << "Persona y cuenta de ahorros creadas correctamente.\n";
		}

		// Selecciona el tipo de cuenta CORRIENTE
		else if (seleccion == 1)
		{
			CuentaCorriente* nuevaCuentaCorriente = nullptr;

			// Solicitar cedula antes de ingresar todos los datos
			std::string cedulaTemp;
			while (true) {
				system("cls");
				std::cout << "\n\n----- INGRESE SUS DATOS -----\n";
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
				}
				if (!Validar::ValidarCedula(cedulaTemp)) {
					std::cout << "Cedula invalida, presione cualquier tecla para volver a ingresar." << std::endl;
					int teclaCualquiera = _getch();
					(void)teclaCualquiera;
					continue;
				}

				// Verificar si la cedula ya existe
				bool existe = false;
				NodoPersona* actual = listaPersonas;
				while (actual) {
					Persona* p = actual->persona;
					if (p && p->getCedula() == cedulaTemp) {
						existe = true;
						break;
					}
					actual = actual->siguiente;
				}
				if (existe) {
					const char* opciones[2] = { "Si", "No" };
					int seleccion = 1;
					int tecla = 0;

					// Guardar una referencia a la persona existente con la cedula
					Persona* personaExistente = nullptr;
					NodoPersona* nodoActual = listaPersonas;
					while (nodoActual) {
						if (nodoActual->persona && nodoActual->persona->getCedula() == cedulaTemp) {
							personaExistente = nodoActual->persona;
							break;
						}
						nodoActual = nodoActual->siguiente;
					}

					while (true) {
						system("cls");
						std::cout << "La cedula " << cedulaTemp << " ya esta registrada en el sistema.\n";
						std::cout << "Titular: " << personaExistente->getNombres() << " "
							<< personaExistente->getApellidos() << "\n\n";
						std::cout << "Desea crear una nueva cuenta para este titular?\n\n";
						for (int i = 0; i < 2; ++i) {
							if (i == seleccion) {
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
									BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN |
									FOREGROUND_BLUE | FOREGROUND_INTENSITY);
								std::cout << " > " << opciones[i] << " < ";
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
									FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
							}
							else {
								std::cout << "   " << opciones[i] << "   ";
							}
						}
						std::cout << "\r";
						tecla = _getch();
						if (tecla == 224) {
							tecla = _getch();
							if (tecla == 75 && seleccion > 0) --seleccion;
							else if (tecla == 77 && seleccion < 1) ++seleccion;
						}
						else if (tecla == 13) break;
					}
					std::cout << std::endl;

					if (seleccion == 1) {
						std::cout << "Operacion cancelada.\n";
						NodoPersona* temp = listaPersonas;
						listaPersonas = listaPersonas->siguiente;
						delete temp;
						return;
					}
					else {
						delete persona;
						nuevo->persona = personaExistente;
						persona = personaExistente;
						std::cout << "Usando datos de titular existente.\n";
						system("pause");

						try {
							Fecha fechaActual;
							std::string fechaStr = fechaActual.obtenerFechaFormateada();
							nuevaCuentaCorriente = new CuentaCorriente("0", 0.0, fechaStr, "Activa", 0.0);

							bool resultado = persona->crearSoloCuentaCorriente(nuevaCuentaCorriente, cedulaTemp);
							if (resultado) {
								CuentaAhorros* verificacion = persona->getCabezaAhorros();
								if (verificacion) {
									std::cout << "Cuenta vinculada correctamente: " << verificacion->getNumeroCuenta() << std::endl;
								}
								else {
									std::cout << "Error: La cuenta no se vinculo correctamente" << std::endl;
								}
							}
							if (!resultado) {
								delete nuevaCuentaCorriente; // Evitar fugas de memoria
							}
						}
						catch (const std::exception& e) {
							std::cerr << "Error: " << e.what() << std::endl;
							system("pause");
							return;
						}

						//std::cout << "Cuenta corriente creada correctamente para el titular existente.\n";
						system("pause");
						return;
					}
				}
				break;
			}
			persona->setCedula(cedulaTemp);

			try {
				Fecha fechaActual;
				std::string fechaStr = fechaActual.obtenerFechaFormateada();
				nuevaCuentaCorriente = new CuentaCorriente("0", 0.0, fechaStr, "Activa", 0.0);

				bool resultado = persona->crearAgregarCuentaCorriente(nuevaCuentaCorriente, cedulaTemp);
				if (resultado) {
					CuentaAhorros* verificacion = persona->getCabezaAhorros();
					if (verificacion) {
						std::cout << "Cuenta vinculada correctamente: " << verificacion->getNumeroCuenta() << std::endl;
					}
					else {
						std::cout << "Error: La cuenta no se vinculo correctamente" << std::endl;
					}
				}
				if (!resultado) {
					delete nuevaCuentaCorriente; // Evitar fugas de memoria
				}
			}
			catch (const std::exception& e) {
				std::cerr << "Error: " << e.what() << std::endl;
				system("pause");
				return;
			}

			std::cout << "Persona y cuenta corriente creadas correctamente.\n";
		}
		system("pause");
	}
}

// Metodo para guardar cuentas en un archivo
void Banco::guardarCuentasEnArchivo(const std::string& nombreArchivo) const {
	std::string rutaEscritorio = obtenerRutaEscritorio();
	std::string rutaCompleta = rutaEscritorio + nombreArchivo + ".bak";

	std::ofstream archivo(rutaCompleta, std::ios::out | std::ios::trunc);
	if (!archivo.is_open()) {
		std::cout << "No se pudo abrir el archivo para guardar en: " << rutaCompleta << "\n";
		return;
	}

	// Escribir cabecera
	archivo << "BANCO_BACKUP_V1.0\n";

	int contadorPersonas = 0;
	NodoPersona* actual = listaPersonas;
	while (actual) {
		Persona* p = actual->persona;
		if (p && p->isValidInstance()) {
			archivo << "===PERSONA_INICIO===\n";
			archivo << "CEDULA:" << p->getCedula() << "\n";
			archivo << "NOMBRES:" << p->getNombres() << "\n";
			archivo << "APELLIDOS:" << p->getApellidos() << "\n";
			archivo << "FECHA_NACIMIENTO:" << p->getFechaNacimiento() << "\n";
			archivo << "CORREO:" << p->getCorreo() << "\n";
			archivo << "DIRECCION:" << p->getDireccion() << "\n";

			archivo << "===CUENTAS_AHORRO_INICIO===\n";
			int cuentasAhorro = p->guardarCuentas(archivo, "AHORROS");
			archivo << "TOTAL_CUENTAS_AHORRO:" << cuentasAhorro << "\n";
			archivo << "===CUENTAS_AHORRO_FIN===\n";

			archivo << "===CUENTAS_CORRIENTE_INICIO===\n";
			int cuentasCorriente = p->guardarCuentas(archivo, "CORRIENTE");
			archivo << "TOTAL_CUENTAS_CORRIENTE:" << cuentasCorriente << "\n";
			archivo << "===CUENTAS_CORRIENTE_FIN===\n";

			archivo << "===PERSONA_FIN===\n\n";
			contadorPersonas++;
		}
		actual = actual->siguiente;
	}

	archivo << "TOTAL_PERSONAS:" << contadorPersonas << "\n";
	archivo << "FIN_BACKUP\n";
	archivo.close();
	std::cout << "Respaldo guardado correctamente en " << rutaCompleta << "\n";
}

void Banco::guardarCuentasEnArchivo() {
	Fecha fechaActual;
	if ((fechaActual.getEsFechaSistemaManipulada())) {
		std::cout << "Error: la fecha del sistema parece haber sido manipulada. No se guardara el respaldo.\n";
		return;
	}

	std::string fechaFormateada = fechaActual.obtenerFechaFormateada();
	for (auto& c : fechaFormateada) {
		if (c == '/') c = '_';
	}

	std::string nombreArchivo = "Respaldo_" + fechaFormateada;
	std::string rutaEscritorio = obtenerRutaEscritorio();
	std::string nombreArchivoUnico = Validar::generarNombreConIndice(rutaEscritorio, fechaFormateada);
	std::string rutaCompleta = rutaEscritorio + nombreArchivoUnico + ".bak";

	std::ofstream archivo(rutaCompleta, std::ios::out | std::ios::trunc);
	if (!archivo.is_open()) {
		std::cout << "No se pudo abrir el archivo para guardar en: " << rutaCompleta << "\n";
		return;
	}

	archivo << "BANCO_BACKUP_V1.0\n";

	int contadorPersonas = 0;
	NodoPersona* actual = listaPersonas;
	while (actual) {
		Persona* p = actual->persona;
		if (p && p->isValidInstance()) {
			archivo << "===PERSONA_INICIO===\n";
			archivo << "CEDULA:" << p->getCedula() << "\n";
			archivo << "NOMBRES:" << p->getNombres() << "\n";
			archivo << "APELLIDOS:" << p->getApellidos() << "\n";
			archivo << "FECHA_NACIMIENTO:" << p->getFechaNacimiento() << "\n";
			archivo << "CORREO:" << p->getCorreo() << "\n";
			archivo << "DIRECCION:" << p->getDireccion() << "\n";

			archivo << "===CUENTAS_AHORRO_INICIO===\n";
			int cuentasAhorro = p->guardarCuentas(archivo, "AHORROS");
			archivo << "TOTAL_CUENTAS_AHORRO:" << cuentasAhorro << "\n";
			archivo << "===CUENTAS_AHORRO_FIN===\n";

			archivo << "===CUENTAS_CORRIENTE_INICIO===\n";
			int cuentasCorriente = p->guardarCuentas(archivo, "CORRIENTE");
			archivo << "TOTAL_CUENTAS_CORRIENTE:" << cuentasCorriente << "\n";
			archivo << "===CUENTAS_CORRIENTE_FIN===\n";

			archivo << "===PERSONA_FIN===\n\n";
			contadorPersonas++;
		}
		actual = actual->siguiente;
	}

	archivo << "TOTAL_PERSONAS:" << contadorPersonas << "\n";
	archivo << "FIN_BACKUP\n";
	archivo.close();
	std::cout << "Respaldo guardado correctamente en " << rutaCompleta << "\n";
}

// Metodo para cargar cuentas desde un archivo
void Banco::cargarCuentasDesdeArchivo(const std::string& nombreArchivo) {
	std::string rutaEscritorio = obtenerRutaEscritorio();
	std::string rutaCompleta = rutaEscritorio + nombreArchivo + ".bak";

	std::ifstream archivo(rutaCompleta);
	if (!archivo.is_open()) {
		std::cout << "No se pudo abrir el archivo para cargar desde: " << rutaCompleta << "\n";
		return;
	}

	while (listaPersonas) {
		NodoPersona* temp = listaPersonas;
		listaPersonas = listaPersonas->siguiente;
		delete temp;
	}

	std::string linea;
	std::getline(archivo, linea);

	if (linea != "BANCO_BACKUP_V1.0") {
		std::cout << "Formato de archivo invalido.\n";
		archivo.close();
		return;
	}

	Persona* personaActual = nullptr;
	bool enPersona = false;
	bool enCuentasAhorro = false;
	bool enCuentasCorriente = false;
	bool enCuentaAhorro = false;
	bool enCuentaCorriente = false;
	int contadorPersonas = 0;

	std::string numCuenta = "";
	int saldo = 0;
	std::string fechaApertura, estado;

	while (std::getline(archivo, linea)) {
		if (linea == "===PERSONA_INICIO===") {
			personaActual = new Persona();
			enPersona = true;
			continue;
		}
		else if (linea == "===PERSONA_FIN===") {
			if (personaActual && enPersona) {
				NodoPersona* nuevo = new NodoPersona(personaActual);
				nuevo->siguiente = listaPersonas;
				listaPersonas = nuevo;
				contadorPersonas++;
			}
			enPersona = false;
			personaActual = nullptr;
			continue;
		}
		else if (linea == "===CUENTAS_AHORRO_INICIO===") {
			enCuentasAhorro = true;
			continue;
		}
		else if (linea == "===CUENTAS_AHORRO_FIN===") {
			enCuentasAhorro = false;
			continue;
		}
		else if (linea == "===CUENTAS_CORRIENTE_INICIO===") {
			enCuentasCorriente = true;
			continue;
		}
		else if (linea == "===CUENTAS_CORRIENTE_FIN===") {
			enCuentasCorriente = false;
			continue;
		}
		else if (linea == "CUENTA_AHORRO_INICIO") {
			enCuentaAhorro = true;
			numCuenta = ""; // Cambiado a string para evitar problemas con std::stoi
			saldo = 0;
			fechaApertura = "";
			estado = "";
			continue;
		}
		else if (linea == "CUENTA_AHORRO_FIN" && enCuentaAhorro && personaActual) {
			CuentaAhorros* nuevaCuenta = new CuentaAhorros(numCuenta, saldo, fechaApertura, estado, 5);
			personaActual->setCabezaAhorros(nuevaCuenta);
			enCuentaAhorro = false;
			continue;
		}
		else if (linea == "CUENTA_CORRIENTE_INICIO") {
			enCuentaCorriente = true;
			numCuenta = ""; // Cambiado a string para evitar problemas con std::stoi
			saldo = 0;
			fechaApertura = "";
			estado = "";
			continue;
		}
		else if (linea == "CUENTA_CORRIENTE_FIN" && enCuentaCorriente && personaActual) {
			CuentaCorriente* nuevaCuenta = new CuentaCorriente(numCuenta, saldo, fechaApertura, estado, 0);
			personaActual->setCabezaCorriente(nuevaCuenta);
			enCuentaCorriente = false;
			continue;
		}

		if (enPersona && personaActual) {
			if (linea.substr(0, 7) == "CEDULA:")
				personaActual->setCedula(linea.substr(7));
			else if (linea.substr(0, 8) == "NOMBRES:")
				personaActual->setNombres(linea.substr(8));
			else if (linea.substr(0, 10) == "APELLIDOS:")
				personaActual->setApellidos(linea.substr(10));
			else if (linea.substr(0, 17) == "FECHA_NACIMIENTO:")
				personaActual->setFechaNacimiento(linea.substr(17));
			else if (linea.substr(0, 7) == "CORREO:")
				personaActual->setCorreo(linea.substr(7));
			else if (linea.substr(0, 10) == "DIRECCION:")
				personaActual->setDireccion(linea.substr(10));
		}

		if ((enCuentaAhorro || enCuentaCorriente) && (enCuentasAhorro || enCuentasCorriente)) {
			if (linea.substr(0, 13) == "NUMERO_CUENTA:")
				numCuenta = linea.substr(13);
			else if (linea.substr(0, 6) == "SALDO:")
				saldo = std::stoi(linea.substr(6));
			else if (linea.substr(0, 14) == "FECHA_APERTURA:")
				fechaApertura = linea.substr(14);
			else if (linea.substr(0, 7) == "ESTADO:")
				estado = linea.substr(7);
		}
	}

	archivo.close();
	std::cout << "Se cargaron " << contadorPersonas << " personas desde el archivo.\n";
}

// Metodo para buscar cuentas
void Banco::buscarCuenta() {
	CuentaAhorros* cuentaAhorros = nullptr;
	CuentaCorriente* cuentaCorriente = nullptr;
	if (listaPersonas == nullptr || reinterpret_cast<uintptr_t>(listaPersonas) > 0xFFFFFFFF00000000) {
		system("cls");
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
		system("cls");
		std::cout << "Seleccione el tipo de busqueda:\n\n";
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
			system("cls");
			std::cout << "Seleccione la fecha de creacion, usando las flechas del teclado. ENTER para aceptar.\n";
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
		std::cout << "Ingrese el numero de cuenta: ";
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
					system("cls");
					cuentasEncontradas += actual->persona->buscarPersonaPorCuentas(numCuentaBuscar);
				}
				catch (...) {
					std::cerr << "Error al buscar en persona." << std::endl;
				}
			}
			actual = actual->siguiente;
		}

		if (cuentasEncontradas == 0) {
			std::cout << "No se encontraron cuentas con el numero: " << numCuentaBuscar << std::endl;
		}
		else {
			std::cout << "Se encontraron " << cuentasEncontradas << " cuenta(s)." << std::endl;
		}

		system("pause");
		return;
	}

	// Nuevo bloque para busqueda por cedula
	if (seleccionBusqueda == 3) { // Por cedula
		std::string cedulaBuscar;
		std::cout << "Ingrese la cedula (10 digitos): ";

		// Control de entrada para la cedula (solo numeros y maximo 10 digitos)
		int digitos = 0;
		while (true) {
			char tecla = _getch();
			if ((tecla >= '0' && tecla <= '9') && digitos < 10) {
				cedulaBuscar += tecla;
				digitos++;
				std::cout << tecla;
			}
			else if (tecla == 8 && !cedulaBuscar.empty()) { // Backspace
				cedulaBuscar.pop_back();
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

		if (!Validar::ValidarCedula(cedulaBuscar)) {
			std::cout << "Cedula invalida.\n";
			system("pause");
			return;
		}

		// Buscar la persona con esa cedula
		bool encontrado = false;
		NodoPersona* actual = listaPersonas;
		while (actual) {
			if (actual->persona && actual->persona->isValidInstance() && actual->persona->getCedula() == cedulaBuscar) 
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
						cuentaAhorros->getCuentaAhorros()->mostrarInformacion(cedulaBuscar, false);
					}
					cuentaAhorros = cuentaAhorros->getSiguiente();
				}

				// Mostrar cuentas corrientes
				int contadorCorrientes = 0;
				while (cuentaCorriente) {
					if (cuentaCorriente->getCuentaCorriente()) {
						std::cout << "\nCUENTA CORRIENTE #" << ++contadorCorrientes << std::endl;
						// Pasar false para no limpiar pantalla entre cuentas
						cuentaCorriente->getCuentaCorriente()->mostrarInformacion(cedulaBuscar, false);
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
			std::cout << "No se encontro ninguna persona con la cedula: " << cedulaBuscar << std::endl;
		}

		system("pause");
		return;
	}
}

// Busqueda recursiva por fecha de creacion
static void buscarCuentasPorFechaRec(NodoPersona* nodo, const std::string& fecha, int& encontrados) {
	if (!nodo) return;
	nodo->persona->buscarPersonaPorFecha(fecha);
	// Puedes incrementar encontrados dentro de buscarPersonaPorFecha si lo deseas
	buscarCuentasPorFechaRec(nodo->siguiente, fecha, encontrados);
}

// Metodo para buscar cuentas por fecha de creacion
void Banco::buscarCuentasPorFecha(const std::string& fecha) const {
	int encontrados = 0;
	buscarCuentasPorFechaRec(listaPersonas, fecha, encontrados);
	if (encontrados == 0) {
		std::cout << "No se encontraron cuentas con esa fecha en el banco.\n";
	}
}

// Metodo para buscar cuentas por criterio
void Banco::buscarCuentasPorCriterio() {
	std::string criterios[] = {
		"Numero de cuenta",
		"Fecha de apertura",
		"Saldo mayor a",
		"Tipo de cuenta",
		"Cedula de titular", // Nueva opcion
		"Cancelar"
	};
	CuentaAhorros* cuentaAhorros = nullptr;
	CuentaCorriente* cuentaCorriente = nullptr;

	int numCriterios = sizeof(criterios) / sizeof(criterios[0]);
	int seleccion = 0;

	// Menu de seleccion con cursor
	while (true) {
		system("cls");
		std::cout << "Seleccione el criterio de busqueda de cuentas:\n\n";
		for (int i = 0; i < numCriterios; i++) {
			if (i == seleccion)
				std::cout << " > " << criterios[i] << std::endl;
			else
				std::cout << "   " << criterios[i] << std::endl;
		}
		int tecla = _getch();
		if (tecla == 224) {
			tecla = _getch();
			if (tecla == 72) seleccion = (seleccion - 1 + numCriterios) % numCriterios;
			else if (tecla == 80) seleccion = (seleccion + 1) % numCriterios;
		}
		else if (tecla == 13) break;
	}
	if (seleccion == numCriterios - 1) return; // Cancelar

	std::string fechaIngresada;
	std::string numCuentaIngresada;
	double valorNum = 0.0;

	// Solicitar el valor segun el criterio
	if (seleccion == 0) { // Numero de cuenta
		std::cout << "Ingrese el numero de cuenta: ";
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
			system("cls");
			std::cout << "Seleccione la fecha de apertura, usando las flechas del teclado. ENTER para aceptar.\n";
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
							valorNum = std::stod(entrada);
							if (valorNum >= 0) {
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

	// El resto de criterios esta bien implementado

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
				int cuentasEncontradas = actual->persona->buscarPersonaPorCriterio(criterios[seleccion], numCuentaIngresada, fechaIngresada, valorNum);
				totalCuentasEncontradas += cuentasEncontradas;
			}
			actual = actual->siguiente;
		}

		if (totalCuentasEncontradas == 0) {
			std::cout << "\nNo se encontraron cuentas que cumplan con el criterio seleccionado." << std::endl;
		}
		else {
			std::cout << "\nTotal de cuentas encontradas: " << totalCuentasEncontradas << std::endl;
		}
	}

	system("pause");
}

// Metodo para realizar transferencias entre cuentas
void Banco::realizarTransferencia() {
	// Verificar que existan personas con cuentas
	if (!listaPersonas) {
		system("cls");
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
		system("cls");
		std::cout << "=== TRANSFERENCIA BANCARIA ===\n\n";
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
			int seleccion = 0; // Por defecto seleccionar "Si"
			int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
			bool continuar = true;

			system("cls");
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
				if (tecla == 224 || tecla == 0) { // Teclas especiales
					tecla = _getch();
					if (tecla == 72) { // Flecha arriba
						seleccion = (seleccion > 0) ? seleccion - 1 : 0;
					}
					else if (tecla == 80) { // Flecha abajo
						seleccion = (seleccion < numOpciones - 1) ? seleccion + 1 : numOpciones - 1;
					}
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
	system("cls");
	std::cout << "=== TRANSFERENCIA BANCARIA ===\n\n";
	std::cout << "CUENTA ORIGEN:\n";
	if (esAhorrosOrigen) {
		cuentaAhorrosOrigen->mostrarInformacion(numCuentaOrigen, false); // Pasar false para no limpiar pantalla
	}
	else {
		cuentaCorrienteOrigen->mostrarInformacion(numCuentaOrigen, false); // Pasar false para no limpiar pantalla
	}
	std::cout << "\nTitular: " << personaOrigen->getNombres() << " " << personaOrigen->getApellidos() << "\n\n";

	// 2. Obtener la cuenta de destino
	while (!cuentaDestinoEncontrada) {
		std::cout << "Ingrese el numero de cuenta de destino (o ESC para cancelar): ";

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
			std::cout << "Cuenta de destino no encontrada. Desea intentar con otro numero? (S/N): ";
			char respuesta = _getch();
			if (respuesta != 'S' && respuesta != 's') {
				std::cout << "\nOperacion cancelada.\n";
				system("pause");
				return;
			}
		}
	}

	// Mostrar informacion de la cuenta destino
	system("cls");
	std::cout << "=== TRANSFERENCIA BANCARIA ===\n\n";
	std::cout << "CUENTA ORIGEN:\n";
	if (esAhorrosOrigen) {
		cuentaAhorrosOrigen->mostrarInformacion(numCuentaOrigen, false);
	}
	else {
		cuentaCorrienteOrigen->mostrarInformacion(numCuentaOrigen, false);
	}
	std::cout << "\nTitular: " << personaOrigen->getNombres() << " " << personaOrigen->getApellidos() << "\n\n";

	std::cout << "CUENTA DESTINO:\n";
	if (esAhorrosDestino) {
		cuentaAhorrosDestino->mostrarInformacion(numCuentaDestino, false);
	}
	else {
		cuentaCorrienteDestino->mostrarInformacion(numCuentaDestino, false);
	}
	std::cout << "\nTitular: " << personaDestino->getNombres() << " " << personaDestino->getApellidos() << "\n\n";

	// 3. Solicitar monto a transferir
	int montoEnCentavos = 0;
	std::string entrada;
	bool tienePunto = false;
	double digitosDecimales = 0;

	std::cout << "Ingrese el monto a transferir (ejemplo: 1000.50): ";
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
						std::cout << "\rIngrese el monto a transferir (ejemplo: 1000.50): " << entrada;
						std::cout << std::endl;
						montoEnCentavos = static_cast<int>(valor * 100);
						break;
					}
				}
				catch (...) {}
			}
			std::cout << "\nFormato invalido. Ingrese nuevamente (ejemplo: 1000.50): ";
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
							montoEnCentavos = static_cast<int>(valor * 100);
							break;
						}
					}
					catch (...) {}
				}
			}
			std::cout << "\nFormato invalido o monto menor o igual a cero. \nIngrese nuevamente (ejemplo: 1000.50): ";
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
			std::cout << "\rIngrese el monto a transferir (ejemplo: 1000.50): " << std::string(40, ' ') << "\r";
			std::cout << "Ingrese el monto a transferir (ejemplo: 1000.50): " << entrada;
			continue;
		}

		// Ignora teclas especiales (como flechas, etc.)
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

	// 4. Verificar fondos suficientes
	double saldoOrigen = esAhorrosOrigen ? cuentaAhorrosOrigen->consultarSaldo() : cuentaCorrienteOrigen->consultarSaldo();

	if (montoEnCentavos > saldoOrigen) {
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
		cuentaAhorrosOrigen->retirar(montoEnCentavos);
	}
	else {
		cuentaCorrienteOrigen->retirar(montoEnCentavos);
	}

	if (esAhorrosDestino) {
		cuentaAhorrosDestino->depositar(montoEnCentavos);
	}
	else {
		cuentaCorrienteDestino->depositar(montoEnCentavos);
	}

	// 7. Mostrar confirmacion
	std::cout << "\nTransferencia realizada con exito!\n\n";
	std::cout << "NUEVO SALDO CUENTA ORIGEN: $"
		<< (esAhorrosOrigen ? cuentaAhorrosOrigen->formatearConComas(cuentaAhorrosOrigen->consultarSaldo()) :
			formatearConComas(cuentaCorrienteOrigen->consultarSaldo()))
		<< "\n";

	system("pause");
}

// Metodo helper para formatear valores con comas (para cuentas corrientes)
std::string Banco::formatearConComas(double valorEnCentavos) const {
	std::ostringstream oss;
	oss.imbue(std::locale(""));
	oss << std::fixed << std::setprecision(2) << valorEnCentavos;
	return oss.str();
}

// Implementacion de la funcion para obtener la ruta del escritorio
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