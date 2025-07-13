/**
 * @file Banco.cpp
 * @brief Implementación de la clase Banco que gestiona el sistema bancario
 * @author Sistema Bancario, Uriel Andrade, Kerly Chuqui, Abner Proano
 * @date 2025
 */
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
 * @brief Agrega una nueva persona con una cuenta asociada al sistema bancario
 *
 * Este método presenta un menú para seleccionar el tipo de cuenta (ahorros o corriente),
 * solicita los datos personales necesarios y crea tanto la persona como su cuenta asociada.
 * También maneja el caso de cédulas existentes, permitiendo agregar una nueva cuenta a un
 * cliente que ya existe en el sistema.
 */
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
		Utilidades::ocultarCursor();
		Utilidades::limpiarPantallaPreservandoMarquesina(2);
		std::cout << "\nSeleccione el tipo de cuenta a crear para la persona:\n\n";
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
		Utilidades::mostrarCursor();
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
				Utilidades::limpiarPantallaPreservandoMarquesina(2);
				std::cout << "----- INGRESE SUS DATOS -----\n";
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
						Utilidades::limpiarPantallaPreservandoMarquesina(3);
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
				Utilidades::limpiarPantallaPreservandoMarquesina(2);
				std::cout << "----- INGRESE SUS DATOS -----\n";
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
						Utilidades::limpiarPantallaPreservandoMarquesina(3);
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
									//std::cout << "Error: La cuenta no se vinculo correctamente" << std::endl;
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
						//std::cout << "Error: La cuenta no se vinculo correctamente" << std::endl;
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

/**
 * @brief Guarda todas las cuentas en un archivo con nombre específico
 *
 * @param nombreArchivo Nombre del archivo donde se guardarán los datos
 */
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

/**
 * @brief Guarda todas las cuentas en un archivo con nombre generado automáticamente
 *
 * Crea un respaldo con la fecha actual en el nombre y verifica que la fecha
 * del sistema no haya sido manipulada antes de realizar el respaldo.
 */

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

/**
 * @brief Convierte un archivo de respaldo .bak a formato PDF
 *
 * Este método lee un archivo de respaldo previamente generado, crea un
 * archivo HTML con formato mejorado y lo convierte a PDF usando wkhtmltopdf.
 *
 * @param nombreArchivo Nombre del archivo de respaldo (sin extensión)
 * @return bool true si la conversión fue exitosa, false en caso contrario
 */
bool Banco::archivoGuardadoHaciaPDF(const std::string& nombreArchivo) {
	std::string rutaEscritorio = obtenerRutaEscritorio();
	std::string rutaBak = rutaEscritorio + nombreArchivo + ".bak";
	std::string rutaHtml = rutaEscritorio + nombreArchivo + "_temp.html";
	std::string rutaPdf = rutaEscritorio + nombreArchivo + ".pdf";

	// Verificar si existe el archivo de respaldo
	std::ifstream archivoEntrada(rutaBak);
	if (!archivoEntrada.is_open()) {
		std::cout << "No se pudo abrir el archivo de respaldo: " << rutaBak << std::endl;
		return false;
	}

	// Crear archivo HTML temporal con estilo
	std::ofstream archivoHtml(rutaHtml);
	if (!archivoHtml.is_open()) {
		std::cout << "No se pudo crear el archivo HTML temporal" << std::endl;
		archivoEntrada.close();
		return false;
	}

	// Escribir encabezado HTML con estilos CSS
	archivoHtml << "<!DOCTYPE html>\n";
	archivoHtml << "<html>\n<head>\n";
	archivoHtml << "<meta charset=\"UTF-8\">\n";
	archivoHtml << "<title>Informe de Cuentas Bancarias</title>\n";
	archivoHtml << "<style>\n";
	archivoHtml << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
	archivoHtml << "h1, h2 { color: #003366; }\n";
	archivoHtml << "h1 { text-align: center; border-bottom: 2px solid #003366; padding-bottom: 10px; }\n";
	archivoHtml << "h2 { margin-top: 20px; border-bottom: 1px solid #ccc; }\n";
	archivoHtml << ".persona { background-color: #f9f9f9; border: 1px solid #ddd; margin: 15px 0; padding: 10px; border-radius: 5px; }\n";
	archivoHtml << ".cuenta { background-color: #eef6ff; margin: 10px 0; padding: 8px; border-left: 4px solid #003366; }\n";
	archivoHtml << ".cuenta-ahorro { border-left-color: #007700; }\n";
	archivoHtml << ".cuenta-corriente { border-left-color: #770000; }\n";
	archivoHtml << ".label { font-weight: bold; color: #555; min-width: 150px; display: inline-block; }\n";
	archivoHtml << ".total { font-weight: bold; margin-top: 20px; color: #003366; }\n";
	archivoHtml << "footer { text-align: center; margin-top: 30px; font-size: 0.8em; color: #777; }\n";
	archivoHtml << "</style>\n</head>\n<body>\n";

	// Añadir encabezado del informe
	Fecha fechaActual;
	archivoHtml << "<h1>Informe de Cuentas Bancarias</h1>\n";
	archivoHtml << "<p style='text-align: center;'>Generado el " << fechaActual.obtenerFechaFormateada() << "</p>\n";

	// Variables para leer y procesar el archivo
	std::string linea;
	bool enPersona = false;
	bool enCuentasAhorro = false;
	bool enCuentasCorriente = false;
	bool enCuentaAhorro = false;
	bool enCuentaCorriente = false;
	int contadorPersonas = 0;
	int totalCuentasAhorro = 0;
	int totalCuentasCorriente = 0;

	// Leer la primera línea que debería ser la versión del archivo
	std::getline(archivoEntrada, linea);
	if (linea != "BANCO_BACKUP_V1.0") {
		archivoHtml << "<p style='color: red;'>Formato de archivo inválido.</p>\n";
		archivoHtml << "</body>\n</html>";
		archivoHtml.close();
		archivoEntrada.close();
		return false;
	}

	// Leer y procesar el resto del archivo
	std::map<std::string, std::string> datosPersona;

	while (std::getline(archivoEntrada, linea)) {
		// Procesamiento de secciones principales
		if (linea == "===PERSONA_INICIO===") {
			enPersona = true;
			datosPersona.clear();
			continue;
		}
		else if (linea == "===PERSONA_FIN===") {
			if (enPersona) {
				// Escribir los datos de la persona en el HTML
				archivoHtml << "<div class='persona'>\n";
				archivoHtml << "  <h2>Cliente: " << datosPersona["NOMBRES"] << " " << datosPersona["APELLIDOS"] << "</h2>\n";
				archivoHtml << "  <p><span class='label'>Cédula:</span> " << datosPersona["CEDULA"] << "</p>\n";
				archivoHtml << "  <p><span class='label'>Fecha Nacimiento:</span> " << datosPersona["FECHA_NACIMIENTO"] << "</p>\n";
				archivoHtml << "  <p><span class='label'>Correo:</span> " << datosPersona["CORREO"] << "</p>\n";
				archivoHtml << "  <p><span class='label'>Dirección:</span> " << datosPersona["DIRECCION"] << "</p>\n";
				archivoHtml << "</div>\n";

				contadorPersonas++;
			}
			enPersona = false;
			continue;
		}
		else if (linea == "===CUENTAS_AHORRO_INICIO===") {
			enCuentasAhorro = true;
			archivoHtml << "<h3>Cuentas de Ahorro</h3>\n";
			continue;
		}
		else if (linea == "===CUENTAS_AHORRO_FIN===") {
			enCuentasAhorro = false;
			continue;
		}
		else if (linea == "===CUENTAS_CORRIENTE_INICIO===") {
			enCuentasCorriente = true;
			archivoHtml << "<h3>Cuentas Corrientes</h3>\n";
			continue;
		}
		else if (linea == "===CUENTAS_CORRIENTE_FIN===") {
			enCuentasCorriente = false;
			continue;
		}
		else if (linea == "CUENTA_AHORROS_INICIO") {
			enCuentaAhorro = true;
			archivoHtml << "<div class='cuenta cuenta-ahorro'>\n";
			continue;
		}
		else if (linea == "CUENTA_AHORROS_FIN") {
			archivoHtml << "</div>\n";
			enCuentaAhorro = false;
			totalCuentasAhorro++;
			continue;
		}
		else if (linea == "CUENTA_CORRIENTE_INICIO") {
			enCuentaCorriente = true;
			archivoHtml << "<div class='cuenta cuenta-corriente'>\n";
			continue;
		}
		else if (linea == "CUENTA_CORRIENTE_FIN") {
			archivoHtml << "</div>\n";
			enCuentaCorriente = false;
			totalCuentasCorriente++;
			continue;
		}
		else if (linea.substr(0, 14) == "TOTAL_PERSONAS:") {
			archivoHtml << "<div class='total'>Total de Clientes: " << contadorPersonas << "</div>\n";
			continue;
		}
		else if (linea.substr(0, 21) == "TOTAL_CUENTAS_AHORRO:") {
			archivoHtml << "<div class='total'>Total de Cuentas de Ahorro: " << linea.substr(21) << "</div>\n";
			continue;
		}
		else if (linea.substr(0, 24) == "TOTAL_CUENTAS_CORRIENTE:") {
			archivoHtml << "<div class='total'>Total de Cuentas Corrientes: " << linea.substr(24) << "</div>\n";
			continue;
		}
		else if (linea == "FIN_BACKUP") {
			break;
		}

		// Procesar datos individuales
		if (enPersona) {
			size_t pos = linea.find(':');
			if (pos != std::string::npos) {
				std::string clave = linea.substr(0, pos);
				std::string valor = linea.substr(pos + 1);
				datosPersona[clave] = valor;
			}
		}

		// Procesar detalles de cuentas
		if (enCuentaAhorro || enCuentaCorriente) {
			size_t pos = linea.find(':');
			if (pos != std::string::npos) {
				std::string clave = linea.substr(0, pos);
				std::string valor = linea.substr(pos + 1);

				if (clave == "NUMERO_CUENTA") {
					archivoHtml << "  <p><span class='label'>Número de Cuenta:</span> " << valor << "</p>\n";
				}
				else if (clave == "SALDO") {
					double saldo = std::stod(valor);
					archivoHtml << "  <p><span class='label'>Saldo:</span> $" << std::fixed << std::setprecision(2) << saldo << "</p>\n";
				}
				else if (clave == "FECHA_APERTURA") {
					archivoHtml << "  <p><span class='label'>Fecha de Apertura:</span> " << valor << "</p>\n";
				}
				else if (clave == "ESTADO") {
					archivoHtml << "  <p><span class='label'>Estado:</span> " << valor << "</p>\n";
				}
			}
		}
	}

	// Cerrar archivo HTML
	archivoHtml << "<footer>Este documento fue generado automáticamente por el sistema bancario.</footer>\n";
	archivoHtml << "</body>\n</html>";
	archivoHtml.close();
	archivoEntrada.close();

	// Convertir HTML a PDF usando wkhtmltopdf
	std::string comando = "wkhtmltopdf \"" + rutaHtml + "\" \"" + rutaPdf + "\"";
	int resultado = system(comando.c_str());

	if (resultado != 0) {
		std::cout << "Error al convertir a PDF. Asegúrese de que wkhtmltopdf esté instalado." << std::endl;
		std::cout << "Puede descargar wkhtmltopdf desde: https://wkhtmltopdf.org/downloads.html" << std::endl;
		std::cout << "Se ha generado un archivo HTML en: " << rutaHtml << std::endl;
		return false;
	}

	// Eliminar archivo HTML temporal
	if (remove(rutaHtml.c_str()) != 0) {
		std::cout << "Advertencia: No se pudo eliminar el archivo HTML temporal." << std::endl;
	}

	std::cout << "PDF generado correctamente: " << rutaPdf << std::endl;
	return true;
}

/**
 * @brief Carga las cuentas desde un archivo de respaldo
 *
 * @param nombreArchivo Nombre del archivo desde donde cargar los datos
 */
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
		Utilidades::limpiarPantallaPreservandoMarquesina(2);
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
		Utilidades::limpiarPantallaPreservandoMarquesina(2);
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
			Utilidades::limpiarPantallaPreservandoMarquesina(2);
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
					Utilidades::limpiarPantallaPreservandoMarquesina(3);
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
	buscarCuentasPorFechaRec(listaPersonas, fecha, encontrados);
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

	bool continuarBusqueda = true;

	while (continuarBusqueda) {
		// Menu de seleccion con cursor
		while (true) {
			Utilidades::limpiarPantallaPreservandoMarquesina(2);
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
			Utilidades::limpiarPantallaPreservandoMarquesina(2);
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
					continue;
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
				Utilidades::limpiarPantallaPreservandoMarquesina(3);
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
		Utilidades::limpiarPantallaPreservandoMarquesina(3);
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
		Utilidades::limpiarPantallaPreservandoMarquesina(3);
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

			Utilidades::limpiarPantallaPreservandoMarquesina(3);
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
	Utilidades::limpiarPantallaPreservandoMarquesina(3);
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
	Utilidades::limpiarPantallaPreservandoMarquesina(3);
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
		Utilidades::limpiarPantallaPreservandoMarquesina(2);
		std::cout << "No hay cuentas registradas en el banco. Cree una cuenta primero.\n";
		system("pause");
		Utilidades::limpiarPantallaPreservandoMarquesina(2);
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
		Utilidades::limpiarPantallaPreservandoMarquesina(2);
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
					digitosDecimales--;
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
				int montoEnCentavos = static_cast<int>(monto);
				if (cuentaAhorros != nullptr) {
					cuentaAhorros->depositar(montoEnCentavos);
					std::cout << "Deposito realizado con exito.\n";
					std::cout << "Nuevo saldo: $" << cuentaAhorros->formatearSaldo() << std::endl;
				}
				else {
					cuentaCorriente->depositar(montoEnCentavos);
					std::cout << "Deposito realizado con exito.\n";
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
			int montoEnCentavos = static_cast<int>(monto * 100);
			if (montoEnCentavos > saldoActual) {
				std::cout << "Fondos insuficientes.\n";
			}
			else {
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
 * @param cuentaCorriente Referencia a puntero que se actualizará con la cuenta corriente encontrada
 * @param cedula Referencia a string que se actualizará con la cédula del titular
 * @return bool true si se encontró una cuenta válida, false en caso contrario
 */
bool Banco::buscarCuentaParaOperacion(Banco& banco, CuentaAhorros*& cuentaAhorros, CuentaCorriente*& cuentaCorriente, std::string& cedula) {
	Utilidades::limpiarPantallaPreservandoMarquesina(2);
	std::cout << "\n===== OPERACIONES DE CUENTA =====\n\n";

	// Menu de seleccion: buscar por cedula o numero
	std::string opciones[] = { "Buscar por cedula", "Buscar por numero de cuenta", "Cancelar" };
	int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
	int seleccion = 0;

	while (true) {
		Utilidades::limpiarPantallaPreservandoMarquesina(3);
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
		Utilidades::limpiarPantallaPreservandoMarquesina(3);
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
			else if (tecla == 13 && digitos == 10) { // Enter
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
				Utilidades::limpiarPantallaPreservandoMarquesina(3);
				std::cout << "Titular: " << actual->persona->getNombres() << " "
					<< actual->persona->getApellidos() << "\n\n";

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
		Utilidades::limpiarPantallaPreservandoMarquesina(3);
		std::cout << "Ingrese el numero de cuenta: ";
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

