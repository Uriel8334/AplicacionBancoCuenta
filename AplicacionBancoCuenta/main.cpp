#include <iostream>
#include <string>
#include "Persona.h"
#include <conio.h> // getch()
#include <windows.h> // SetConsoleCursorPosition
#include "Banco.h"
#include "Utilidades.h"
#include "Cifrado.h" // Asegurate de incluir este archivo de cabecera

// Funcion para mostrar el menu sin parpadeo y limpiar toda la linea
void mostrarMenu(int seleccion, std::string opciones[], int numOpciones, int x, int y) {
	system("cls"); // Limpia la pantalla
	const int anchoLinea = 80; // Ajusta segun el ancho de tu consola
	for (int i = 0; i < numOpciones; i++) {
		Utilidades::gotoxy(0, y + i);
		std::cout << std::string(anchoLinea, ' '); // Borra toda la linea
		Utilidades::gotoxy(x, y + i);
		if (i == seleccion)
			std::cout << " > " << opciones[i] << "   ";
		else
			std::cout << "   " << opciones[i] << "   ";
	}
	// Limpia cualquier linea sobrante si el menu se reduce
	Utilidades::gotoxy(0, y + numOpciones);
	std::cout << std::string(anchoLinea, ' ');
}

// Funcion para buscar cuenta por cedula o numero de cuenta (retorna la cuenta y tipo)
bool buscarCuentaParaOperacion(Banco& banco, CuentaAhorros*& cuentaAhorros, CuentaCorriente*& cuentaCorriente, std::string& cedula) {
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
				NodoCuentaAhorros* nodoAhorro = actual->persona->getListaCuentasAhorros();
				int contador = 1;
				while (nodoAhorro) {
					if (nodoAhorro->cuenta) {
						std::cout << contador << ". Cuenta de Ahorro: "
							<< nodoAhorro->cuenta->getNumeroCuenta() << "\n";
						cuentas.push_back({ true, nodoAhorro->cuenta });
						contador++;
					}
					nodoAhorro = nodoAhorro->siguiente;
				}

				// Cuentas corrientes
				NodoCuentaCorriente* nodoCorriente = actual->persona->getListaCuentasCorriente();
				while (nodoCorriente) {
					if (nodoCorriente->cuenta) {
						std::cout << contador << ". Cuenta Corriente: "
							<< nodoCorriente->cuenta->getNumeroCuenta() << "\n";
						cuentas.push_back({ false, nodoCorriente->cuenta });
						contador++;
					}
					nodoCorriente = nodoCorriente->siguiente;
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
				NodoCuentaAhorros* nodoAhorro = actual->persona->getListaCuentasAhorros();
				while (nodoAhorro && !encontrado) {
					if (nodoAhorro->cuenta && std::to_string(nodoAhorro->cuenta->getNumeroCuenta()) == numCuenta) {
						cuentaAhorros = nodoAhorro->cuenta;
						cuentaCorriente = nullptr;
						cedula = actual->persona->getCedula();
						encontrado = true;
						break;
					}
					nodoAhorro = nodoAhorro->siguiente;
				}

				// Si no se encontro, buscar en cuentas corrientes
				if (!encontrado) {
					NodoCuentaCorriente* nodoCorriente = actual->persona->getListaCuentasCorriente();
					while (nodoCorriente && !encontrado) {
						if (nodoCorriente->cuenta && std::to_string(nodoCorriente->cuenta->getNumeroCuenta()) == numCuenta) {
							cuentaAhorros = nullptr;
							cuentaCorriente = nodoCorriente->cuenta;
							cedula = actual->persona->getCedula();
							encontrado = true;
							break;
						}
						nodoCorriente = nodoCorriente->siguiente;
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

int main() {
	std::string opciones[] = {
		"Crear Cuenta",
		"Buscar Cuenta",
		"Cuenta",
		"Transferencias",
		"Guardar Archivo",
		"Recuperar Archivo",
		"Descifrar Archivo",
		"Menu de ayuda",
		"Salir"
	};

	int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
	int seleccion = 0;
	int x = 0, y = 0;

	Banco banco;

	// Imprime el menu una vez (para reservar espacio)
	for (int i = 0; i < numOpciones; i++)
		std::cout << std::endl;

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
				banco.agregarPersonaConCuenta();
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

					int saldoActual = 0;
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
						cuentaAhorros->mostrarInformacion(cedula);
					}
					else {
						cuentaCorriente->mostrarInformacion(cedula);
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
				// Submenú para tipo de carga
				std::string opcionesCarga[] = { "Recuparar de Respaldo (.bak)", "Recuperar de Archivo cifrado (.bin)", "Cancelar" };
				int numOpcionesCarga = sizeof(opcionesCarga) / sizeof(opcionesCarga[0]);
				int selCarga = 0;

				while (true) {
					system("cls");
					std::cout << "Seleccione el tipo de archivo a cargar:\n\n";
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

				// Si se selecciona la opción "Cancelar" (índice 2), finaliza
				if (selCarga == 2) {
					break;
				}

				system("cls");
				std::cout << "Ingrese el nombre del archivo (sin extension): ";
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
				// Submenú principal para descifrar archivo
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

				// Submenú secundario para tipo de descifrado
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

				// Manejar opción "Cancelar" del segundo submenú
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
			case 8: // Salir
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
	return 0;
}