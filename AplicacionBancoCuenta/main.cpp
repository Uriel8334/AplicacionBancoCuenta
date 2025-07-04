#include <iostream>
#include <string>
#include "Persona.h"
#include <conio.h> // getch()
#include <windows.h> // SetConsoleCursorPosition
#include "Banco.h"
#include "Utilidades.h"
#include "Cifrado.h" // Asegurate de incluir este archivo de cabecera
#include <algorithm>
#include "Marquesina.h"
#include "GeneradorQRBanco.h"
// Funcion para mostrar el menu sin parpadeo y limpiar toda la linea
static void mostrarMenu(int seleccion, std::string opciones[], int numOpciones, int x, int y) {
	Utilidades::limpiarPantallaPreservandoMarquesina(); // Ya maneja las operaciones críticas

	const int anchoLinea = 80;
	for (int i = 0; i < numOpciones; i++) {
		Utilidades::gotoxy(0, y + i); // Ya thread-safe
		std::cout << std::string(anchoLinea, ' ');
		Utilidades::gotoxy(x, y + i);
		if (i == seleccion)
			std::cout << " > " << opciones[i] << "   ";
		else
			std::cout << "   " << opciones[i] << "   ";
	}
	Utilidades::gotoxy(0, y + numOpciones);
	std::cout << std::string(anchoLinea, ' ');
}

// Funcion para buscar cuenta por cedula o numero de cuenta (retorna la cuenta y tipo)
static bool buscarCuentaParaOperacion(Banco& banco, CuentaAhorros*& cuentaAhorros, CuentaCorriente*& cuentaCorriente, std::string& cedula) {
	system("cls");
	std::cout << "\n===== OPERACIONES DE CUENTA =====\n\n";

	// Menu de seleccion: buscar por cedula o numero
	std::string opciones[] = { "Buscar por cedula", "Buscar por numero de cuenta", "Cancelar" };
	int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
	int seleccion = 0;

	while (true) {
		system("cls");
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
		system("cls");
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
				system("cls");
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
				auto& cuentaSelec = cuentas[selCuenta - 1];
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
		system("cls");
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

// Funcion para mostrar personas
void mostrarPersonas(const std::vector<Persona*>& personas) {
	for (auto p : personas) {
		std::cout << p->getNombres() << " " << p->getApellidos() << " - " << p->getFechaNacimiento() << "\n";
	}
}

Marquesina* marquesinaGlobal = nullptr;

int main() {
	std::string opciones[] = {
		"Crear Cuenta",
		"Buscar Cuenta",
		"Operaciones de Cuenta",
		"Realizar Transferencias",
		"Guardar Archivo",
		"Recuperar Archivo",
		"Descifrar Archivo",
		"Menu de ayuda",
		"Explorador de archivos",
		"Gestion de Hash",
		"Arbol B",
		"Generar QR",
		"Salir"
	};

	int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
	int seleccion = 0;
	int x = 0, y = 0;

	Banco banco;

	// Imprime el menu una vez (para reservar espacio)
	for (int i = 0; i < numOpciones; i++)
		std::cout << std::endl;

	// Configuración de la consola
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	int anchoConsola = csbi.srWindow.Right - csbi.srWindow.Left + 1;

	// Crear la marquesina en la parte superior de la consola
	marquesinaGlobal = new Marquesina(0, 0, anchoConsola, "marquesina.html", 150);
	marquesinaGlobal->iniciar();

	// Dejar espacio para la marquesina
	std::cout << std::endl << std::endl; // 2 líneas para la marquesina




	while (true) {
		mostrarMenu(seleccion, opciones, numOpciones, x, y);
		int tecla = _getch();

		if (tecla == 224) { // Tecla especial
			tecla = _getch();
			if (tecla == 72) // Flecha arriba
				seleccion = (seleccion - 1 + numOpciones) % numOpciones;
			else if (tecla == 80) // Flecha abajo
				seleccion = (seleccion + 1) % numOpciones;
		}
		else if (tecla == 13) { // Enter
			Utilidades::gotoxy(0, y + numOpciones + 1);
			std::cout << "Has seleccionado: " << opciones[seleccion] << std::endl;

			// Switch para manejar la opcion seleccionada
			switch (seleccion) {
			case 0: // Crear Cuenta
				Utilidades::iniciarOperacionCritica();
				banco.agregarPersonaConCuenta();
				Utilidades::finalizarOperacionCritica();
				break;
			case 1: // Buscar Cuenta
				banco.buscarCuenta();
				break;
			case 2: // Cuenta (nueva opcion)
			{
				// Verificar si hay cuentas
				if (banco.getListaPersonas() == nullptr) {
					system("cls");
					std::cout << "No hay cuentas registradas. Cree una cuenta primero.\n";
					system("pause");
					break;
				}

				// Submenu para operaciones de cuenta
				std::string opcionesCuenta[] = { "Depositar", "Retirar", "Consultar saldo", "Cancelar" };
				int numOpcionesCuenta = sizeof(opcionesCuenta) / sizeof(opcionesCuenta[0]);
				int selCuenta = 0;

				while (true) {
					system("cls");
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

				if (selCuenta == 3) { // Cancelar
					break;
				}

				// Buscar cuenta para realizar la operacion
				CuentaAhorros* cuentaAhorros = nullptr;
				CuentaCorriente* cuentaCorriente = nullptr;
				std::string cedula;

				if (!buscarCuentaParaOperacion(banco, cuentaAhorros, cuentaCorriente, cedula)) {
					break; // Si no se encontro ninguna cuenta o se cancelo la operacion
				}

				// Realizar la operacion seleccionada
				system("cls");
				if (cuentaAhorros != nullptr) {
					std::cout << "CUENTA DE AHORROS: " << cuentaAhorros->getNumeroCuenta() << "\n";
				}
				else {
					std::cout << "CUENTA CORRIENTE: " << cuentaCorriente->getNumeroCuenta() << "\n";
				}

				if (selCuenta == 0) { // Depositar
					std::cout << "\nDEPOSITO\n\n";
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
						// Cualquier otra tecla se ignora
					}

					// Convertir la entrada a double con manejo de excepciones
					double monto = 0.0;
					try {
						monto = std::stod(entrada);

						if (monto <= 0) {
							std::cout << "El monto debe ser mayor a cero.\n";
						}
						else {
							int montoEnCentavos = static_cast<int>(monto * 100);
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
					}
					catch (const std::exception& e) {
						cout << e.what() << endl;
						//std::cout << "Error al procesar el monto: formato invalido.\n " << endl;
					}
				}
				else if (selCuenta == 1) { // Retirar
					std::cout << "\nRETIRO\n\n";

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
				}
				else if (selCuenta == 2) { // Consultar saldo
					std::cout << "\nCONSULTA DE SALDO\n\n";

					if (cuentaAhorros != nullptr) {
						cuentaAhorros->mostrarInformacion(cedula, false);
					}
					else {
						cuentaCorriente->mostrarInformacion(cedula, false);
					}
				}

				if (selCuenta != 2) { // Si no es consulta (que ya tiene su propio system("pause"))
					system("pause");
				}
			}
			break;
			case 3: // Transferencias
				banco.realizarTransferencia();
				break;
			case 4: // Guardar Archivo
			{
				// Verificar si hay datos para guardar
				if (banco.getListaPersonas() == nullptr) {
					system("cls");
					std::cout << "No hay datos para guardar. Cree al menos una cuenta primero.\n";
					system("pause");
					break;
				}

				// Submenu para tipo de guardado
				std::string opcionesGuardado[] = { "Respaldo (.bak)", "Archivo cifrado (.bin)", "Cancelar" };
				int numOpcionesGuardado = sizeof(opcionesGuardado) / sizeof(opcionesGuardado[0]);
				int selGuardado = 0;

				while (true) {
					system("cls");
					std::cout << "Seleccione el tipo de guardado:\n\n";
					for (int i = 0; i < numOpcionesGuardado; i++) {
						if (i == selGuardado)
							std::cout << " > " << opcionesGuardado[i] << std::endl;
						else
							std::cout << "   " << opcionesGuardado[i] << std::endl;
					}

					int teclaGuardado = _getch();
					if (teclaGuardado == 224) {
						teclaGuardado = _getch();
						if (teclaGuardado == 72) // Flecha arriba
							selGuardado = (selGuardado - 1 + numOpcionesGuardado) % numOpcionesGuardado;
						else if (teclaGuardado == 80) // Flecha abajo
							selGuardado = (selGuardado + 1) % numOpcionesGuardado;
					}
					else if (teclaGuardado == 13) // Enter
						break;
				}

				if (selGuardado == 2) { // Cancelar
					break;
				}

				// Pedir nombre del archivo
				system("cls");


				if (selGuardado == 0) { // Respaldo (.bak)
					std::cout << "Guardando respaldo en archivo .bak\n";
					std::cout << "Presione cualquier tecla para continuar...\n";
					int tecla = _getch();
					(void)tecla; // Ignorar la tecla presionada
					banco.guardarCuentasEnArchivo();
				}
				else if (selGuardado == 1) { // Archivo cifrado (.bin)
					std::cout << "Ingrese el nombre del archivo (sin extension): ";
					std::string nombreArchivo;
					std::cin >> nombreArchivo;
					std::cout << "Ingrese una clave para cifrar (un solo caracter): ";
					char clave = _getch();
					std::cout << "*\n"; // Ocultar la clave
					Cifrado::cifrarYGuardarDatos(banco, nombreArchivo, clave);
				}
			}
			system("pause");
			break;
			case 5: // Recuperar Archivo
			{
				// Submenu para tipo de carga
				std::string opcionesCarga[] = { "Recuparar de Respaldo (.bak)", "Recuperar de Archivo cifrado (.bin)", "Cancelar" };
				int numOpcionesCarga = sizeof(opcionesCarga) / sizeof(opcionesCarga[0]);
				int selCarga = 0;
				Utilidades::limpiarPantallaPreservandoMarquesina();
				while (true) {
					system("cls");
					std::cout << "\n\nSeleccione el tipo de archivo a cargar:\n\n";
					for (int i = 0; i < numOpcionesCarga; i++) {
						if (i == selCarga)
							std::cout << " > " << opcionesCarga[i] << std::endl;
						else
							std::cout << "   " << opcionesCarga[i] << std::endl;
					}

					int teclaCarga = _getch();
					if (teclaCarga == 224) {
						teclaCarga = _getch();
						if (teclaCarga == 72)      // Flecha arriba
							selCarga = (selCarga - 1 + numOpcionesCarga) % numOpcionesCarga;
						else if (teclaCarga == 80) // Flecha abajo
							selCarga = (selCarga + 1) % numOpcionesCarga;
					}
					else if (teclaCarga == 13) // Enter
						break;
				}

				// Si se selecciona la opcion "Cancelar" (indice 2), finaliza
				if (selCarga == 2) {
					break;
				}

				system("cls");
				std::cout << "\n\nIngrese el nombre del archivo (sin extension): ";
				std::string nombreArchivo;
				std::cin >> nombreArchivo;

				switch (selCarga) {
				case 0: { // Respaldo (.bak)
					banco.cargarCuentasDesdeArchivo(nombreArchivo);
					break;
				}
				case 1: { // Archivo cifrado (.bin)
					std::cout << "Ingrese la clave de descifrado (un solo caracter): ";
					char clave = _getch();
					std::cout << "*\n"; // Ocultar la clave
					if (!Cifrado::descifrarYCargarDatos(banco, nombreArchivo, clave)) {
						std::cout << "No se pudo cargar el archivo cifrado. Verifique la clave.\n";
					}
					break;
				}
				}
			}
			system("pause");
			break;
			case 6:  // Descifrar Archivo
			{
				// Submenu principal para descifrar archivo
				std::string opcionesDescifrado[] = { "Descifrar Archivo (.bin)", "Cancelar" };
				int numOpcionesDescifrado = sizeof(opcionesDescifrado) / sizeof(opcionesDescifrado[0]);
				int selDescifrado = 0;

				while (true) {
					system("cls");
					std::cout << "Seleccione a que descifrar:\n\n";
					for (int i = 0; i < numOpcionesDescifrado; i++) {
						if (i == selDescifrado)
							std::cout << " > " << opcionesDescifrado[i] << std::endl;
						else
							std::cout << "   " << opcionesDescifrado[i] << std::endl;
					}

					int teclaDescifrado = _getch();
					if (teclaDescifrado == 224) {
						teclaDescifrado = _getch();
						if (teclaDescifrado == 72) // Flecha arriba
							selDescifrado = (selDescifrado - 1 + numOpcionesDescifrado) % numOpcionesDescifrado;
						else if (teclaDescifrado == 80) // Flecha abajo
							selDescifrado = (selDescifrado + 1) % numOpcionesDescifrado;
					}
					else if (teclaDescifrado == 13) {
						break;
					}
				}

				if (selDescifrado == 1) { // Cancelar
					break;
				}

				// Submenu secundario para tipo de descifrado
				std::string subOpcionesDescifrado[] = {
					"Binario(.bin) -> Backup(.bak)",
					"Binario(.bin) -> Texto(.txt)",
					"Cancelar"
				};
				int numSubOpcionesDescifrado = sizeof(subOpcionesDescifrado) / sizeof(subOpcionesDescifrado[0]);
				int selSubDescifrado = 0;

				while (true) {
					system("cls");
					std::cout << "Seleccione el tipo de descifrado:\n\n";
					for (int i = 0; i < numSubOpcionesDescifrado; i++) {
						if (i == selSubDescifrado)
							std::cout << " > " << subOpcionesDescifrado[i] << std::endl;
						else
							std::cout << "   " << subOpcionesDescifrado[i] << std::endl;
					}

					int teclaSubDescifrado = _getch();
					if (teclaSubDescifrado == 224) {
						teclaSubDescifrado = _getch();
						if (teclaSubDescifrado == 72) // Flecha arriba
							selSubDescifrado = (selSubDescifrado - 1 + numSubOpcionesDescifrado) % numSubOpcionesDescifrado;
						else if (teclaSubDescifrado == 80) // Flecha abajo
							selSubDescifrado = (selSubDescifrado + 1) % numSubOpcionesDescifrado;
					}
					else if (teclaSubDescifrado == 13) {
						break;
					}
				}

				// Manejar opcion "Cancelar" del segundo submenu
				if (selSubDescifrado == 2) {
					break;
				}

				system("cls");
				std::cout << "Ingrese el nombre del archivo (sin extension): ";
				std::string nombreArchivo;
				std::cin >> nombreArchivo;

				switch (selSubDescifrado) {
				case 0: { // Archivo cifrado (.bin) -> Respaldo (.bak)
					std::cout << "Ingrese la clave de descifrado (un solo caracter): ";
					char clave = _getch();
					std::cout << "*\n"; // Ocultar la clave
					if (!Cifrado::descifrarSinCargarDatos(banco, nombreArchivo, clave, 0)) {
						std::cout << "No se pudo cargar el archivo cifrado. Verifique la clave.\n";
					}
					break;
				}
				case 1: { // Archivo cifrado (.bin) -> Texto (.txt)
					std::cout << "Ingrese la clave de descifrado (un solo caracter): ";
					char clave = _getch();
					std::cout << "*\n"; // Ocultar la clave
					if (!Cifrado::descifrarSinCargarDatos(banco, nombreArchivo, clave, 1)) {
						std::cout << "No se pudo cargar el archivo cifrado. Verifique la clave.\n";
					}
					break;
				}
				}
			}
			break;
			case 7: // Menu de ayuda
			{
				system("cls");
				// se llama a la aplicacion de ayuda
				Utilidades::mostrarMenuAyuda();
				system("pause");
				break;
			}
			case 8: // Explorador de archivos
			{
				NodoPersona* cabeza = banco.getListaPersonas();

				std::vector<std::string> opcionesPersona = { "Nombre", "Apellido", "Fecha de nacimiento" };

				// Criterios de ordenamiento para personas
				std::vector<std::function<bool(const Persona*, const Persona*)>> criteriosPersona = {
					[](const Persona* a, const Persona* b) {
						std::string na = a->getNombres(), nb = b->getNombres();
						std::transform(na.begin(), na.end(), na.begin(), ::tolower);
						std::transform(nb.begin(), nb.end(), nb.begin(), ::tolower);
						return na < nb;
					},
					[](const Persona* a, const Persona* b) {
						std::string aa = a->getApellidos(), ab = b->getApellidos();
						std::transform(aa.begin(), aa.end(), aa.begin(), ::tolower);
						std::transform(ab.begin(), ab.end(), ab.begin(), ::tolower);
						return aa < ab;
					},
					[](const Persona* a, const Persona* b) {
						return a->getFechaNacimiento() < b->getFechaNacimiento();
					}
				};
				std::vector<std::function<bool(const Persona*, const Persona*)>> criteriosPersonaDesc = {
					[](const Persona* a, const Persona* b) {
						std::string na = a->getNombres(), nb = b->getNombres();
						std::transform(na.begin(), na.end(), na.begin(), ::tolower);
						std::transform(nb.begin(), nb.end(), nb.begin(), ::tolower);
						return na > nb;
					},
					[](const Persona* a, const Persona* b) {
						std::string aa = a->getApellidos(), ab = b->getApellidos();
						std::transform(aa.begin(), aa.end(), aa.begin(), ::tolower);
						std::transform(ab.begin(), ab.end(), ab.begin(), ::tolower);
						return aa > ab;
					},
					[](const Persona* a, const Persona* b) {
						return a->getFechaNacimiento() > b->getFechaNacimiento();
					}
				};


				int seleccion = 0;
				bool ascendente = true;
				while (true) {
					system("cls");
					// Encabezado horizontal con cursor
					for (size_t i = 0; i < opcionesPersona.size(); ++i) {
						if (i == seleccion)
							std::cout << " >" << opcionesPersona[i] << "< ";
						else
							std::cout << "  " << opcionesPersona[i] << "  ";
						if (i < opcionesPersona.size() - 1) std::cout << "|";
					}
					std::cout << "\n";
					std::cout << "-------------------------------------------------------------\n";
					// Ordenar la lista enlazada
					if (ascendente)
						Utilidades::burbujaLista(cabeza, criteriosPersona[seleccion]);
					else
						Utilidades::burbujaLista(cabeza, criteriosPersonaDesc[seleccion]);
					// Mostrar datos en formato horizontal
					NodoPersona* actual = cabeza;
					while (actual) {
						Persona* p = actual->persona;
						std::cout << p->getNombres() << " | " << p->getApellidos() << " | " << p->getFechaNacimiento() << "\n";
						actual = actual->siguiente;
					}
					std::cout << "\nUse IZQUIERDA/DERECHA para cambiar criterio, ARRIBA para ascendente, ABAJO para descendente, ESC para salir.\n";

					int tecla = _getch();
					if (tecla == 224) {
						tecla = _getch();
						if (tecla == 75) // Izquierda
							seleccion = (seleccion - 1 + opcionesPersona.size()) % opcionesPersona.size();
						else if (tecla == 77) // Derecha
							seleccion = (seleccion + 1) % opcionesPersona.size();
						else if (tecla == 72) // Arriba
							ascendente = true;
						else if (tecla == 80) // Abajo
							ascendente = false;
					}
					else if (tecla == 27) { // ESC
						break;
					}
				}

				break;
			}
			case 9: // Gestion de Hash
			{
				// Submenu para gestion de hash
				std::string opcionesHash[] = { "Crear Hash", "Verificar Hash", "Cancelar" };
				int numOpcionesHash = sizeof(opcionesHash) / sizeof(opcionesHash[0]);
				int seleccionHash = 0;

				while (true) {
					system("cls");
					std::cout << "GESTIoN DE HASH DE ARCHIVOS\n\n";
					std::cout << "Seleccione una operacion:\n\n";
					for (int i = 0; i < numOpcionesHash; i++) {
						if (i == seleccionHash)
							std::cout << " > " << opcionesHash[i] << std::endl;
						else
							std::cout << "   " << opcionesHash[i] << std::endl;
					}

					int teclaHash = _getch();
					if (teclaHash == 224) {
						teclaHash = _getch();
						if (teclaHash == 72) // Flecha arriba
							seleccionHash = (seleccionHash - 1 + numOpcionesHash) % numOpcionesHash;
						else if (teclaHash == 80) // Flecha abajo
							seleccionHash = (seleccionHash + 1) % numOpcionesHash;
					}
					else if (teclaHash == 13) // Enter
						break;
					else if (teclaHash == 27) { // ESC
						seleccionHash = 2; // Cancelar
						break;
					}
				}

				if (seleccionHash == 2) { // Cancelar
					break;
				}

				system("cls");
				std::cout << "Ingrese el nombre del archivo (sin extension): ";
				std::string nombreArchivo;
				std::cin >> nombreArchivo;

				// Formar la ruta completa al archivo
				std::string rutaEscritorio = banco.obtenerRutaEscritorio();
				std::string rutaArchivo = rutaEscritorio + nombreArchivo + ".bak";

				if (seleccionHash == 0) { // Crear Hash
					std::string hash = Utilidades::calcularSHA1(rutaArchivo);
					if (hash == "ERROR_ARCHIVO_NO_ENCONTRADO") {
						std::cout << "Error: El archivo no existe." << std::endl;
					}
					else {
						std::cout << "Hash calculado: " << hash << std::endl;
						Utilidades::guardarHashArchivo(rutaArchivo, hash);
					}
				}
				else if (seleccionHash == 1) { // Verificar Hash
					std::string rutaHash = rutaArchivo + ".hash";
					std::string hashGuardado = Utilidades::leerHashArchivo(rutaHash);

					if (hashGuardado.empty()) {
						std::cout << "Error: No se encontro un archivo de hash para este backup." << std::endl;
					}
					else {
						bool valido = Utilidades::verificarSHA1(rutaArchivo, hashGuardado);
						if (valido) {
							std::cout << "¡Verificacion exitosa! El archivo es legitimo y no ha sido modificado." << std::endl;
						}
						else {
							std::cout << "¡ADVERTENCIA! El hash no coincide. El archivo puede haber sido modificado." << std::endl;
						}
					}
				}
				system("pause");
				break;
			}
			case 10: // Arbol B
			{
				if (banco.getListaPersonas() == nullptr) {
					system("cls");
					std::cout << "No hay personas registradas. Cree una cuenta primero.\n";
					system("pause");
					break;
				}

				Utilidades::PorArbolB(banco.getListaPersonas());
				break;
			}
			case 11: { // Generar QR 
				// Verificar si hay personas registradas
				if (banco.getListaPersonas() == nullptr) {
					Utilidades::limpiarPantallaPreservandoMarquesina();
					std::cout << "No hay personas registradas. Cree una cuenta primero.\n";
					system("pause");
					break;
				}

				NodoPersona* cabeza = banco.getListaPersonas();
				std::vector<std::string> opcionesPersona = { "Nombre", "Apellido" }; // Omitimos fecha de nacimiento

				// Criterios de ordenamiento para personas (sin fecha de nacimiento)
				std::vector<std::function<bool(const Persona*, const Persona*)>> criteriosPersona = {
					[](const Persona* a, const Persona* b) {
						std::string na = a->getNombres(), nb = b->getNombres();
						std::transform(na.begin(), na.end(), na.begin(), ::tolower);
						std::transform(nb.begin(), nb.end(), nb.begin(), ::tolower);
						return na < nb;
					},
					[](const Persona* a, const Persona* b) {
						std::string aa = a->getApellidos(), ab = b->getApellidos();
						std::transform(aa.begin(), aa.end(), aa.begin(), ::tolower);
						std::transform(ab.begin(), ab.end(), ab.begin(), ::tolower);
						return aa < ab;
					}
				};

				std::vector<std::function<bool(const Persona*, const Persona*)>> criteriosPersonaDesc = {
					[](const Persona* a, const Persona* b) {
						std::string na = a->getNombres(), nb = b->getNombres();
						std::transform(na.begin(), na.end(), na.begin(), ::tolower);
						std::transform(nb.begin(), nb.end(), nb.begin(), ::tolower);
						return na > nb;
					},
					[](const Persona* a, const Persona* b) {
						std::string aa = a->getApellidos(), ab = b->getApellidos();
						std::transform(aa.begin(), aa.end(), aa.begin(), ::tolower);
						std::transform(ab.begin(), ab.end(), ab.begin(), ::tolower);
						return aa > ab;
					}
				};

				// Variables para navegación
				int seleccionCriterio = 0;
				bool ascendente = true;
				int seleccionPersona = 0;
				std::vector<Persona*> personasOrdenadas;

				// Función para recolectar y ordenar personas
				auto actualizarListaPersonas = [&]() {
					personasOrdenadas.clear();
					NodoPersona* actual = cabeza;
					while (actual) {
						if (actual->persona) {
							personasOrdenadas.push_back(actual->persona);
						}
						actual = actual->siguiente;
					}

					if (!personasOrdenadas.empty()) {
						if (ascendente) {
							Utilidades::burbuja<Persona>(personasOrdenadas, criteriosPersona[seleccionCriterio]);
						}
						else {
							Utilidades::burbuja<Persona>(personasOrdenadas, criteriosPersonaDesc[seleccionCriterio]);
						}
					}

					// Asegurar que la selección esté dentro del rango
					if (seleccionPersona >= static_cast<int>(personasOrdenadas.size())) {
						seleccionPersona = static_cast<int>(personasOrdenadas.size()) - 1;
					}
					if (seleccionPersona < 0) {
						seleccionPersona = 0;
					}
					};

				// Bucle principal del explorador
				while (true) {
					actualizarListaPersonas();

					if (personasOrdenadas.empty()) {
						Utilidades::limpiarPantallaPreservandoMarquesina();
						std::cout << "No hay personas válidas para mostrar.\n";
						system("pause");
						break;
					}

					Utilidades::limpiarPantallaPreservandoMarquesina();

					// Mostrar encabezado con criterios de ordenamiento
					std::cout << "=== GENERADOR QR - SELECCIONAR PERSONA ===\n\n";
					std::cout << "Ordenar por: ";
					for (size_t i = 0; i < opcionesPersona.size(); ++i) {
						if (i == seleccionCriterio)
							std::cout << " >" << opcionesPersona[i] << "< ";
						else
							std::cout << "  " << opcionesPersona[i] << "  ";
						if (i < opcionesPersona.size() - 1) std::cout << "|";
					}
					std::cout << " (" << (ascendente ? "Ascendente" : "Descendente") << ")\n";
					std::cout << std::string(60, '-') << "\n";

					// Mostrar lista de personas con selección
					for (size_t i = 0; i < personasOrdenadas.size(); ++i) {
						Persona* p = personasOrdenadas[i];
						if (static_cast<int>(i) == seleccionPersona) {
							std::cout << " > " << p->getNombres() << " " << p->getApellidos() << " <\n";
						}
						else {
							std::cout << "   " << p->getNombres() << " " << p->getApellidos() << "\n";
						}
					}

					std::cout << "\nControles:\n";
					std::cout << "IZQUIERDA/DERECHA: cambiar criterio | ARRIBA/ABAJO: cambiar orden\n";
					std::cout << "W/S: navegar personas | ENTER: generar QR | ESC: salir\n";

					int tecla = _getch();
					if (tecla == 224) {
						tecla = _getch();
						if (tecla == 75) { // Izquierda
							seleccionCriterio = (seleccionCriterio - 1 + opcionesPersona.size()) % opcionesPersona.size();
						}
						else if (tecla == 77) { // Derecha
							seleccionCriterio = (seleccionCriterio + 1) % opcionesPersona.size();
						}
						else if (tecla == 72) { // Arriba
							ascendente = true;
						}
						else if (tecla == 80) { // Abajo
							ascendente = false;
						}
					}
					else if (tecla == 'w' || tecla == 'W') {
						seleccionPersona = (seleccionPersona - 1 + personasOrdenadas.size()) % personasOrdenadas.size();
					}
					else if (tecla == 's' || tecla == 'S') {
						seleccionPersona = (seleccionPersona + 1) % personasOrdenadas.size();
					}
					else if (tecla == 13) { // Enter - Generar QR
						Persona* personaSeleccionada = personasOrdenadas[seleccionPersona];

						// Buscar cuenta de la persona seleccionada
						std::string numeroCuentaQR = "";

						// Buscar primera cuenta disponible (ahorros o corriente)
						CuentaAhorros* cuentaAhorros = personaSeleccionada->getCabezaAhorros();
						if (cuentaAhorros && cuentaAhorros->getCuentaAhorros()) {
							numeroCuentaQR = cuentaAhorros->getCuentaAhorros()->getNumeroCuenta();
						}
						else {
							CuentaCorriente* cuentaCorriente = personaSeleccionada->getCabezaCorriente();
							if (cuentaCorriente && cuentaCorriente->getCuentaCorriente()) {
								numeroCuentaQR = cuentaCorriente->getCuentaCorriente()->getNumeroCuenta();
							}
						}

						if (numeroCuentaQR.empty()) {
							Utilidades::limpiarPantallaPreservandoMarquesina();
							std::cout << "Error: La persona seleccionada no tiene cuentas asociadas.\n";
							system("pause");
							continue;
						}

						// Limpiar pantalla y generar QR
						Utilidades::limpiarPantallaPreservandoMarquesina();

						try {
							// Crear y generar QR (sin fecha de nacimiento)
							GeneradorQRBanco qr(*personaSeleccionada, numeroCuentaQR);
							qr.generar();

							int teclaQR = 0;

							std::cout << "=== CODIGO QR GENERADO EXITOSAMENTE ===\n\n";
							qr.imprimir();

							// Menú de opciones para el QR generado
							std::string opcionesQR[] = { 
								"Mostrar QR nuevamente", 
								"Cambiar a formato JSON",
								"Cambiar a formato estructurado",
								"Guardar como SVG", 
								"Volver al explorador" 
							};
							int numOpcionesQR = sizeof(opcionesQR) / sizeof(opcionesQR[0]);
							int seleccionQR = 0;

							while (true) {
								std::cout << "\n=== OPCIONES DEL QR ===\n";
								for (int i = 0; i < numOpcionesQR; i++) {
									if (i == seleccionQR)
										std::cout << " > " << opcionesQR[i] << std::endl;
									else
										std::cout << "   " << opcionesQR[i] << std::endl;
								}

								int teclaQR = _getch();
								if (teclaQR == 224) {
									teclaQR = _getch();
									if (teclaQR == 72) // Flecha arriba
										seleccionQR = (seleccionQR - 1 + numOpcionesQR) % numOpcionesQR;
									else if (teclaQR == 80) // Flecha abajo
										seleccionQR = (seleccionQR + 1) % numOpcionesQR;
								}
								else if (teclaQR == 13) { // Enter
									if (seleccionQR == 0) { // Mostrar QR nuevamente
										Utilidades::limpiarPantallaPreservandoMarquesina();
										std::cout << "=== CODIGO QR ===\n\n";
										qr.imprimir();
									}
									else if (seleccionQR == 1) { // Cambiar a JSON
										qr.setFormatoSalida(true);
										qr.generar();
										Utilidades::limpiarPantallaPreservandoMarquesina();
										std::cout << "=== QR REGENERADO EN FORMATO JSON ===\n\n";
										qr.imprimir();
									}
									else if (seleccionQR == 2) { // Cambiar a estructurado
										qr.setFormatoSalida(false);
										qr.generar();
										Utilidades::limpiarPantallaPreservandoMarquesina();
										std::cout << "=== QR REGENERADO EN FORMATO ESTRUCTURADO ===\n\n";
										qr.imprimir();
									}
									else if (seleccionQR == 3) { // Guardar como SVG
										std::string archivoBase = "QR_" + numeroCuentaQR + "_" +
											personaSeleccionada->getNombres() + "_" +
											personaSeleccionada->getApellidos();

										// Reemplazar espacios con guiones bajos
										std::replace(archivoBase.begin(), archivoBase.end(), ' ', '_');

										qr.guardarComoSVG(archivoBase + ".svg");
										qr.guardarInformacionCuenta(archivoBase + "_info.txt");

										std::cout << "\nArchivos guardados exitosamente:\n";
										std::cout << "- " << archivoBase << ".svg\n";
										std::cout << "- " << archivoBase << "_info.txt\n";
										system("pause");
									}
									else if (seleccionQR == 4) { // Volver al explorador
										break;
									}
								}
								else if (teclaQR == 27) { // ESC
									break;
								}
							}

							if (seleccionQR == 4 || teclaQR == 27) {
								continue; // Volver al explorador
							}

						}
						catch (const std::exception& e) {
							Utilidades::limpiarPantallaPreservandoMarquesina();
							std::cout << "Error al generar QR: " << e.what() << std::endl;
							system("pause");
							continue;
						}
					}
					else if (tecla == 27) { // ESC
						break;
					}
				}
				break;
			}
			case 12: // Salir
			{
				system("cls");
				std::cout << "Saliendo del sistema...\n";
				return 0;
			}

			if (seleccion == numOpciones - 1) {
				// Si selecciono "Salir"
				break;
			}
			// Limpiar la linea
			Utilidades::gotoxy(0, y + numOpciones + 1);
			std::cout << std::string(40, ' ') << std::endl;
			}
		}
		else if (tecla == 27) // ESC
		{
			Utilidades::gotoxy(0, y + numOpciones + 1);
			std::cout << "Saliendo con ESC..." << std::endl;
			break;
		}
	}

	if (marquesinaGlobal) {
		marquesinaGlobal->detener();
		delete marquesinaGlobal;
	}
	return 0;
}