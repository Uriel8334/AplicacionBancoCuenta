
/**
 * @file main.cpp
 * @brief Sistema de gestión bancaria con interfaz de consola
 *
 * Este archivo implementa la aplicación principal del sistema bancario, proporcionando
 * un menú interactivo para gestionar cuentas, realizar transferencias, buscar información,
 * y generar reportes. Incluye una marquesina en la parte superior para mensajes importantes.
 *
 * @author Uriel Andrade
 * @author Kerly Chuqui
 * @author Abner Proano
 */
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include "Persona.h"
#include <conio.h> // getch()
#include <windows.h> // SetConsoleCursorPosition
#include "Banco.h"
#include "Utilidades.h"
#include "Cifrado.h" 
#include <algorithm>
#include <vector>
#include <shellapi.h>
#include "Marquesina.h"
#include "CodigoQR.h"
#include "_ExportadorArchivo.h"
#include "_BaseDatosPersona.h"
#include "_BaseDatosArchivos.h"
#include "ConexionMongo.h"
#include "AdministradorChatRedLocal.h"
#include "AdministradorChatSocket.h"


 /** @brief Puntero global a la marquesina utilizada en la aplicación */
Marquesina* marquesinaGlobal = nullptr;

/** @brief Bandera atómica para coordinar actualizaciones entre menú y marquesina */
std::atomic<bool> actualizandoMenu(false);

/** @brief Mutex para sincronización entre hilos */
std::mutex mtxActualizacion;

/**
 * @brief Pausa temporalmente la marquesina durante operaciones críticas
 */
static void pausarMarquesina() {
	if (marquesinaGlobal) {
		marquesinaGlobal->pausar();
	}
}

/**
 * @brief Reanuda la marquesina después de una operación crítica
 */
static void reanudarMarquesina() {
	if (marquesinaGlobal) {
		marquesinaGlobal->reanudar();
	}
}


/**
 * @brief Muestra información de personas en consola
 *
 * Imprime los datos básicos de las personas del vector proporcionado.
 *
 * @param personas Vector de punteros a objetos Persona a mostrar
 */
static void mostrarPersonas(const std::vector<Persona*>& personas) {
	for (auto p : personas) {
		std::cout << p->getNombres() << " " << p->getApellidos() << " - " << p->getFechaNacimiento() << "\n";
	}
}

void configurarConsolaUTF8() {
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
}
  
/**
 * @brief Permite al usuario seleccionar el modo de conexión a MongoDB
 * @return true si se seleccionó correctamente, false si se canceló
 */
bool seleccionarModoConexion() {
	std::vector<std::string> opcionesConexion = {
		"SERVIDOR (Local - localhost:27017)",
		"CLIENTE (Remoto - 192.168.1.10:27017)"
	};

	std::cout << "=== CONFIGURACIÓN DE CONEXIÓN MONGODB ===" << std::endl;
	std::cout << std::endl;
	std::cout << "Seleccione el modo de conexión:" << std::endl;
	std::cout << std::endl;
	std::cout << "• SERVIDOR: Para ejecutar en la máquina que tiene MongoDB instalado" << std::endl;
	std::cout << "• CLIENTE: Para ejecutar en máquinas remotas conectadas al servidor" << std::endl;
	std::cout << std::endl;

	int seleccionConexion = Utilidades::menuInteractivo("Modo de Conexión MongoDB", opcionesConexion, 0, 0);

	if (seleccionConexion == -1) {
		// Usuario presionó ESC
		std::cout << "Configuración cancelada. Saliendo del sistema..." << std::endl;
		return false;
	}

	// Configurar el modo según la selección
	if (seleccionConexion == 0) {
		ConexionMongo::setModoConexion(ConexionMongo::SERVIDOR);
		std::cout << std::endl;
		std::cout << "=== MODO SERVIDOR SELECCIONADO ===" << std::endl;
		std::cout << "• Conectará a: mongodb://localhost:27017" << std::endl;
		std::cout << "• Asegúrese de que MongoDB esté ejecutándose localmente" << std::endl;
	}
	else {
		ConexionMongo::setModoConexion(ConexionMongo::CLIENTE);
		std::cout << std::endl;
		std::cout << "=== MODO CLIENTE SELECCIONADO ===" << std::endl;
		std::cout << "• Conectará a: mongodb://192.168.1.10:27017" << std::endl;
		std::cout << "• Asegúrese de que pcServidor esté accesible en la red" << std::endl;
	}

	std::cout << std::endl;
	return true;
}

/**
 * @brief Opciones del menú principal del sistema bancario
 *
 * Contiene las diferentes opciones que el usuario puede seleccionar
 * para interactuar con el sistema bancario.
 */
std::vector<std::string> opcionesMenuPrincipal = {
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
	"Abrir documentacion",
	"Operaciones Base de Datos",
	"Chat de mensajeria",
	"Salir"
};

/**
 * @brief Función principal que inicia la aplicación bancaria
 *
 * Configura la interfaz de usuario, crea la marquesina, y gestiona el
 * menú principal y sus submenús para realizar todas las operaciones bancarias.
 *
 * @return int Código de salida del programa (0 si termina correctamente)
 */
int main() {
	configurarConsolaUTF8();
	// === SELECCIÓN DEL MODO DE CONEXIÓN ===
	std::cout << "SISTEMA BANCARIO EDUCATIVO" << std::endl;
	std::cout << "===================================" << std::endl;
	std::cout << std::endl;

	// Seleccionar modo de conexión
	if (!seleccionarModoConexion()) {
		return 0; // Salir si se canceló la configuración
	}

	// Inicializar el cliente de MongoDB con el modo seleccionado
	std::cout << "Inicializando conexión MongoDB..." << std::endl;
	try {
		ConexionMongo::getCliente();
		std::cout << "✓ Conexión MongoDB establecida correctamente" << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "✗ Error crítico al conectar con MongoDB: " << e.what() << std::endl;
		std::cout << "La aplicación no puede continuar sin conexión a la base de datos." << std::endl;
		system("pause");
		return -1;
	}
	system("pause");

	Utilidades::limpiarPantallaPreservandoMarquesina(0);

	//variables globales
	int seleccion = 0;
	int seleccionAnterior = -1; // Para evitar parpadeo al actualizar el menú
	int x = 0, y = 0;
	Banco banco;

	// Configuración de la consola
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	int anchoConsola = csbi.srWindow.Right - csbi.srWindow.Left + 1;

	// Crear la marquesina en la parte superior de la consola
	marquesinaGlobal = new Marquesina(0, 0, anchoConsola, "marquesina.html", 200);
	marquesinaGlobal->iniciar();

	Utilidades::centrarVentanaConsola();

	// Dejar espacio para la marquesina
	std::cout << std::endl << std::endl; // 2 líneas para la marquesina
	Utilidades::ocultarCursor(); // Ocultar el cursor al inicio
	std::vector<std::string> opciones = opcionesMenuPrincipal; // Opciones Menu principal 

	// Imprime el menu una vez (para reservar espacio)
	for (int i = 0; i < static_cast<int>(opciones.size()); i++)
		std::cout << std::endl;
	
	while (true) {
		int seleccion = Utilidades::menuInteractivo("SISTEMA BANCARIO-EDUCATIVO", opciones, x, y);
		bool necesitaRedibujado = false;

		// Si el usuario presiona ESC o selecciona "Salir"
		if (seleccion == 15) {
			Utilidades::ocultarCursor();
			Utilidades::limpiarPantallaPreservandoMarquesina(0);
			std::cout << "Saliendo del sistema...\n";
			break;
		}

		Utilidades::gotoxy(0, y + static_cast<int>(opciones.size()) + 1);

		// Switch para manejar la opcion seleccionada
		switch (seleccion)
		{
		case 0: // Crear Cuenta
		{
			Utilidades::mostrarCursor();
			banco.agregarPersonaConCuenta();
			Utilidades::ocultarCursor();
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 1: // Buscar Cuenta
		{
			Utilidades::mostrarCursor();
			banco.buscarCuenta();
			Utilidades::ocultarCursor();
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 2: // Operaciones de Cuenta
		{

			if (!banco.verificarCuentasBanco())
			{
				necesitaRedibujado = true;
				break; /*Si no hay cuentas, no se puede entrar al submenu*/
			}

			Utilidades::ocultarCursor();
			banco.subMenuCuentasBancarias();
			Utilidades::ocultarCursor();
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 3: // Realizar Transferencias
		{
			banco.realizarTransferencia();
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 4: // Guardar Archivo
		{
			// Verificar si hay datos para guardar
			if (banco.getListaPersonas() == nullptr) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				std::cout << "No hay datos para guardar. Cree al menos una cuenta primero.\n";
				system("pause");
				necesitaRedibujado = true;
				break;
			}

			// Submenu para tipo de guardado
			std::string opcionesGuardado[] = { "Respaldo (.bak)", "Archivo cifrado (.bin)", "Archivo PDF de (.bak) o (.txt)", "Cancelar" };
			int numOpcionesGuardado = sizeof(opcionesGuardado) / sizeof(opcionesGuardado[0]);
			int selGuardado = 0;

			while (true) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
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

			if (selGuardado == 3) { // Cancelar
				break;
			}

			// Pedir nombre del archivo
			Utilidades::limpiarPantallaPreservandoMarquesina(1);

			if (selGuardado == 0) { // Respaldo (.bak)
				std::cout << "Guardando respaldo en archivo .bak\n";
				std::cout << "Presione cualquier tecla para continuar...\n";
				int tecla = _getch();
				(void)tecla; // Ignorar la tecla presionada
				ExportadorArchivo::guardarCuentasEnArchivo(banco);

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
			else if (selGuardado == 2) { // Archivo PDF de (.bak) o (.txt)
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				std::cout << "GENERACIÓN DE PDF\n\n";

				// Solicitar el nombre del archivo de respaldo a convertir
				std::cout << "Ingrese el nombre del archivo .bak (sin extensión): ";
				std::string nombreArchivo;
				std::cin >> nombreArchivo;

				// Verificar que el archivo exista
				std::string rutaEscritorio = banco.obtenerRutaEscritorio();
				std::string rutaCompleta = rutaEscritorio + nombreArchivo + ".bak";

				std::ifstream verificarArchivo(rutaCompleta);
				if (!verificarArchivo.is_open()) {
					std::cout << "\nError: No se encontró el archivo " << rutaCompleta << "\n";
					std::cout << "Asegúrese de que el archivo exista antes de convertirlo a PDF.\n";
					system("pause");
				}
				else {
					verificarArchivo.close();

					// Intentar convertir a PDF
					std::cout << "\nConvirtiendo archivo a PDF, por favor espere...\n";
					if (ExportadorArchivo::archivoGuardadoHaciaPDF(nombreArchivo)) {
						std::cout << "\nConversión exitosa. El PDF se guardó en: "
							<< rutaEscritorio << nombreArchivo << ".pdf\n";
					}
					else {
						std::cout << "\nError al convertir a PDF. Verifique que wkhtmltopdf esté instalado.\n";
					}
					system("pause");
				}
			}
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 5: // Recuperar Archivo
		{
			// Submenu para tipo de carga
			std::string opcionesCarga[] = { "Recuperar de Respaldo (.bak)", "Recuperar de Archivo cifrado (.bin)", "Cancelar" };
			int numOpcionesCarga = sizeof(opcionesCarga) / sizeof(opcionesCarga[0]);
			int selCarga = 0;
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			while (true) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
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

			// Si se selecciona la opcion "Cancelar" (indice 2), finaliza
			if (selCarga == 2) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				necesitaRedibujado = true;
				break;
			}

			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			std::cout << "\n\nIngrese el nombre del archivo (sin extension): ";
			std::string nombreArchivo;
			std::cin >> nombreArchivo;

			switch (selCarga) {
			case 0: { // Respaldo (.bak)
				ExportadorArchivo::cargarCuentasDesdeArchivo(banco, nombreArchivo);
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
			system("pause");
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 6:  // Descifrar Archivo
		{
			// Submenu principal para descifrar archivo
			std::string opcionesDescifrado[] = { "Descifrar Archivo (.bin)", "Cancelar" };
			int numOpcionesDescifrado = sizeof(opcionesDescifrado) / sizeof(opcionesDescifrado[0]);
			int selDescifrado = 0;

			while (true) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
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
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				necesitaRedibujado = true;
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
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
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

			Utilidades::limpiarPantallaPreservandoMarquesina(1);
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

			necesitaRedibujado = true;
			break;
		}
		case 7: // Menu de ayuda
		{
			// se llama a la aplicacion de ayuda
			Utilidades::mostrarMenuAyuda();
			system("pause");
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
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
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
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
						seleccion = (seleccion - 1 + static_cast<int>(opcionesPersona.size())) % static_cast<int>(opcionesPersona.size());
					else if (tecla == 77) // Derecha
						seleccion = (static_cast<unsigned long long>(seleccion) + 1) % opcionesPersona.size();
					else if (tecla == 72) // Arriba
						ascendente = true;
					else if (tecla == 80) // Abajo
						ascendente = false;
				}
				else if (tecla == 27) { // ESC
					break;
				}
			}
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 9: // Gestion de Hash
		{
			// Submenu para gestion de hash
			std::string opcionesHash[] = { "Crear Hash", "Verificar Hash", "Cancelar" };
			int numOpcionesHash = sizeof(opcionesHash) / sizeof(opcionesHash[0]);
			int seleccionHash = 0;

			while (true) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				std::cout << "GESTIÓN DE HASH DE ARCHIVOS\n\n";
				std::cout << "Seleccione una operación:\n\n";
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
					Utilidades::limpiarPantallaPreservandoMarquesina(1);
					necesitaRedibujado = true;
					break;
				}
			}

			if (seleccionHash == 2) { // Cancelar
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				necesitaRedibujado = true;
				break;
			}

			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			std::cout << "Ingrese el nombre del archivo (sin extensión): ";
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
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 10: // Arbol B
		{
			if (banco.getListaPersonas() == nullptr) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				std::cout << "No hay personas registradas. Cree una cuenta primero.\n";
				system("pause");
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				necesitaRedibujado = true;
				break;
			}

			Utilidades::PorArbolB(banco.getListaPersonas());
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 11: // Generar QR
		{
			// Verificar que haya personas en la base de datos
			if (banco.getListaPersonas() == nullptr) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				std::cout << "No hay personas registradas en el sistema.\n";
				system("pause");
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				necesitaRedibujado = true;
				break;
			}

			// Recolectar todas las personas validas
			std::vector<Persona*> personasOrdenadas;
			NodoPersona* nodoActual = banco.getListaPersonas();

			while (nodoActual) {
				if (nodoActual->persona && nodoActual->persona->isValidInstance()) {
					personasOrdenadas.push_back(nodoActual->persona);
				}
				nodoActual = nodoActual->siguiente;
			}

			if (personasOrdenadas.empty()) {
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				std::cout << "No hay personas validas para mostrar.\n";
				system("pause");
				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				necesitaRedibujado = true;
				break;
			}

			// Opciones simplificadas: solo Nombres y Apellidos
			std::vector<std::string> opcionesPersona = {
				"Nombres", "Apellidos"
			};

			int seleccionCriterio = 0;
			int seleccionPersona = 0;
			bool ascendente = true;

			// Criterios de ordenamiento solo para nombres y apellidos
			std::vector<std::function<bool(const Persona*, const Persona*)>> criteriosPersona = {
				[](const Persona* a, const Persona* b) { return a->getNombres() < b->getNombres(); },
				[](const Persona* a, const Persona* b) { return a->getApellidos() < b->getApellidos(); }
			};

			std::vector<std::function<bool(const Persona*, const Persona*)>> criteriosPersonaDesc = {
				[](const Persona* a, const Persona* b) { return a->getNombres() > b->getNombres(); },
				[](const Persona* a, const Persona* b) { return a->getApellidos() > b->getApellidos(); }
			};

			// Funcion para actualizar la lista ordenada
			auto actualizarListaPersonas = [&]() {
				if (ascendente) {
					Utilidades::burbuja<Persona>(personasOrdenadas, criteriosPersona[seleccionCriterio]);
				}
				else {
					Utilidades::burbuja<Persona>(personasOrdenadas, criteriosPersonaDesc[seleccionCriterio]);
				}

				// Asegurar seleccion dentro del rango
				if (seleccionPersona >= static_cast<int>(personasOrdenadas.size())) {
					seleccionPersona = static_cast<int>(personasOrdenadas.size()) - 1;
				}
				if (seleccionPersona < 0) {
					seleccionPersona = 0;
				}
				};

			// Bucle para explorador de personas
			while (true) {
				actualizarListaPersonas();

				Utilidades::limpiarPantallaPreservandoMarquesina(1);
				std::cout << "=== EXPLORADOR DE PERSONAS - SELECCIONAR PERSONA ===\n\n";
				std::cout << "Ordenar por: ";

				for (size_t i = 0; i < opcionesPersona.size(); ++i) {
					if (i == seleccionCriterio)
						std::cout << " >" << opcionesPersona[i] << "< ";
					else
						std::cout << "  " << opcionesPersona[i] << "  ";
					if (i < opcionesPersona.size() - 1) std::cout << "|";
				}

				std::cout << " (" << (ascendente ? "Ascendente" : "Descendente") << ")\n";
				std::cout << std::string(50, '-') << "\n";

				// Mostrar SOLO nombres y apellidos
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
				std::cout << "Tecla izq, dere: cambiar criterio | Tecla arriba/abajo: navegar personas\n";
				std::cout << "ESPACIO: cambiar orden | ENTER: seleccionar | ESC: cancelar\n";

				int tecla = _getch();
				if (tecla == 224) {
					tecla = _getch();
					if (tecla == 75) { // Izquierda
						seleccionCriterio = (seleccionCriterio - 1 + static_cast<int>(opcionesPersona.size())) % static_cast<int>(opcionesPersona.size());
					}
					else if (tecla == 77) { // Derecha
						seleccionCriterio = (static_cast<unsigned long long>(seleccionCriterio) + 1) % opcionesPersona.size();
					}
					else if (tecla == 72) { // Arriba
						seleccionPersona = (seleccionPersona - 1 + static_cast<int>(personasOrdenadas.size())) % static_cast<int>(personasOrdenadas.size());
					}
					else if (tecla == 80) { // Abajo
						seleccionPersona = (static_cast<unsigned long long>(seleccionPersona) + 1) % personasOrdenadas.size();
					}
				}
				else if (tecla == 13) { // ENTER
					// Seleccionamos la persona
					Persona* personaSeleccionada = personasOrdenadas[seleccionPersona];

					// Recopilar cuentas disponibles - mostrando solo nombres y numeros
					std::vector<std::string> cuentasDisponibles;

					// Recopilar cuentas de ahorro
					CuentaAhorros* cuentaAhorros = personaSeleccionada->getCabezaAhorros();
					while (cuentaAhorros) {
						if (cuentaAhorros->getCuentaAhorros()) {
							cuentasDisponibles.push_back("Ahorro: " + cuentaAhorros->getCuentaAhorros()->getNumeroCuenta());
						}
						cuentaAhorros = cuentaAhorros->getSiguiente();
					}

					// Recopilar cuentas corrientes
					CuentaCorriente* cuentaCorriente = personaSeleccionada->getCabezaCorriente();
					while (cuentaCorriente) {
						if (cuentaCorriente->getCuentaCorriente()) {
							cuentasDisponibles.push_back("Corriente: " + cuentaCorriente->getCuentaCorriente()->getNumeroCuenta());
						}
						cuentaCorriente = cuentaCorriente->getSiguiente();
					}

					int debugCuentas = 0;
					CuentaAhorros* ca = personaSeleccionada->getCabezaAhorros();
					while (ca) { debugCuentas++; ca = ca->getSiguiente(); }
					std::cout << "[DEBUG] Cuentas de ahorro: " << debugCuentas << std::endl;


					// Si no hay cuentas, mostrar mensaje y volver
					if (cuentasDisponibles.empty()) {
						//Utilidades::limpiarPantallaPreservandoMarquesina(1);
						std::cout << "La persona seleccionada no tiene cuentas asociadas.\n";
						system("pause");
						continue;
					}

					// Menu para seleccionar cuenta
					int seleccionCuenta = 0;
					std::string numeroCuentaQR;

					while (true) {
						Utilidades::limpiarPantallaPreservandoMarquesina(1);
						std::cout << "=== SELECCIONAR CUENTA PARA QR ===\n\n";
						std::cout << "Titular: " << personaSeleccionada->getNombres() << " "
							<< personaSeleccionada->getApellidos() << "\n\n";
						std::cout << "Seleccione una cuenta:\n\n";

						for (size_t i = 0; i < cuentasDisponibles.size(); i++) {
							if (i == seleccionCuenta)
								std::cout << " > " << cuentasDisponibles[i] << "\n";
							else
								std::cout << "   " << cuentasDisponibles[i] << "\n";
						}

						int teclaCuenta = _getch();
						if (teclaCuenta == 224) {
							teclaCuenta = _getch();
							if (teclaCuenta == 72) // Arriba
								seleccionCuenta = (seleccionCuenta - 1 + static_cast<int>(cuentasDisponibles.size())) % static_cast<int>(cuentasDisponibles.size());
							else if (teclaCuenta == 80) // Abajo
								seleccionCuenta = (static_cast<unsigned long long>(seleccionCuenta) + 1) % cuentasDisponibles.size();
						}
						else if (teclaCuenta == 13) { // ENTER
							// Extraer numero de cuenta de la seleccion
							std::string seleccion = cuentasDisponibles[seleccionCuenta];
							numeroCuentaQR = seleccion.substr(seleccion.find(": ") + 2);
							break;
						}
						else if (teclaCuenta == 27) { // ESC
							numeroCuentaQR = "";
							break;
						}
					}

					if (numeroCuentaQR.empty()) {
						continue; // Volver a seleccion de persona
					}

					// Generar QR directamente aqui en main.cpp
					Utilidades::limpiarPantallaPreservandoMarquesina(1);

					// Reemplazar el bloque de código seleccionado por:
					try {
						// Generar QR usando la función de Utilidades
						if (!Utilidades::generarQR(*personaSeleccionada, numeroCuentaQR)) {
							// Si se canceló con ESC, volvemos al explorador de personas
							continue;
						}
						break; // Si terminó correctamente o se seleccionó "Volver al menú principal"
					}
					catch (const std::exception& e) {
						Utilidades::limpiarPantallaPreservandoMarquesina(1);
						std::cout << "\n\nError generando QR: " << e.what() << std::endl;
						system("pause");
					}

					break;
				}
				else if (tecla == 27) { // ESC
					Utilidades::limpiarPantallaPreservandoMarquesina(1);
					necesitaRedibujado = true;
					break; // Cancelar
				}
				else if (tecla == 32) { // ESPACIO - cambiar orden
					ascendente = !ascendente;
				}
			}
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		//case 12: // Abrir documentacion
		//{
		//	Utilidades::limpiarPantallaPreservandoMarquesina(1);
		//	std::cout << "Abriendo documentación en el navegador...\n";
		//	// Definir la ruta a la documentación HTML
		//	std::string rutaBase = "html\\index.html";
		//	// Usar ShellExecute para abrir el archivo en el navegador por defecto
		//	// Esto es más robusto que system() porque:
		//	// 1. Usa el navegador predeterminado del sistema
		//	// 2. Maneja correctamente rutas con espacios
		//	// 3. No muestra ventanas de comandos
		//	HINSTANCE resultado = ShellExecuteA(
		//		NULL,           // Handle del padre (null = escritorio)
		//		"open",         // Operación (abrir)
		//		rutaBase.c_str(), // Archivo a abrir
		//		NULL,           // Parámetros (ninguno)
		//		NULL,           // Directorio de trabajo (usar actual)
		//		SW_SHOWNORMAL   // Mostrar ventana normalmente
		//	);
		//	// Verificar si se pudo abrir
		//	if ((int)resultado <= 32) { // ShellExecute devuelve valores <= 32 en caso de error
		//		std::cout << "No se pudo abrir la documentación. Código de error: " << (int)resultado << "\n";
		//		std::cout << "Intentando método alternativo...\n";
		//		// Método alternativo usando system
		//		std::string comando = "start " + rutaBase;
		//		int resultadoSystem = system(comando.c_str());
		//		if (resultadoSystem != 0) {
		//			std::cout << "Error al abrir la documentación.\n";
		//			std::cout << "Por favor, abra manualmente el archivo: " << rutaBase << "\n";
		//		}
		//		else {
		//			std::cout << "Documentación abierta mediante método alternativo.\n";
		//		}
		//	}
		//	else {
		//		std::cout << "Documentación abierta correctamente.\n";
		//	}
		//	std::cout << "\nPresione cualquier tecla para continuar...\n";
		//	int teclaCualquiera = _getch();
		//	(void)teclaCualquiera;
		//	Utilidades::limpiarPantallaPreservandoMarquesina(1);
		//	necesitaRedibujado = true;
		//	break;
		//}			
		case 12: // Abrir documentacion
		{
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			std::cout << "Abriendo documentación en el navegador...\n";

			// Determinar rutas relativas y absolutas para la documentación
			std::string rutaRelativa = "html\\index.html";
			char rutaEjecutableCompleta[MAX_PATH];
			GetModuleFileNameA(NULL, rutaEjecutableCompleta, MAX_PATH);

			// Extraer directorio base del ejecutable
			std::string rutaEjecutable(rutaEjecutableCompleta);
			std::string directorioBase = rutaEjecutable.substr(0, rutaEjecutable.find_last_of("\\/"));
			std::string rutaAbsoluta = directorioBase + "\\" + rutaRelativa;

			// Verificar existencia del archivo antes de intentar abrirlo
			bool archivoExiste = false;
			std::ifstream verificarArchivo(rutaAbsoluta);
			if (verificarArchivo) {
				archivoExiste = true;
				verificarArchivo.close();
			}

			if (!archivoExiste) {
				std::cout << "No se encontró la documentación en la ruta:\n" << rutaAbsoluta << "\n\n";
				std::cout << "Buscando en directorio de instalación...\n";

				// Intentar buscar en una ubicación alternativa común para instalaciones
				std::string rutaAlternativa = "C:\\Program Files\\SistemaBancario\\html\\index.html";
				std::ifstream verificarAlternativa(rutaAlternativa);
				if (verificarAlternativa) {
					rutaAbsoluta = rutaAlternativa;
					archivoExiste = true;
					verificarAlternativa.close();
				}
				else {
					std::cout << "No se encontró la documentación. Verifique la instalación.\n";
				}
			}

			if (archivoExiste) {
				// Usar ShellExecute para abrir el archivo en el navegador predeterminado
				HINSTANCE resultado = ShellExecuteA(
					NULL,                 // Handle del padre (null = escritorio)
					"open",               // Operación (abrir)
					rutaAbsoluta.c_str(), // Archivo a abrir (ruta absoluta)
					NULL,                 // Parámetros (ninguno)
					NULL,                 // Directorio de trabajo (usar actual)
					SW_SHOWNORMAL         // Mostrar ventana normalmente
				);

				// Verificar si se pudo abrir
				if (reinterpret_cast<intptr_t>(resultado) <= 32) { // ShellExecute devuelve valores <= 32 en caso de error
					std::cout << "No se pudo abrir la documentación. Código de error: " << reinterpret_cast<intptr_t>(resultado) << "\n";
					std::cout << "Intentando método alternativo...\n";

					// Método alternativo usando system
					std::string comando = "start \"\" \"" + rutaAbsoluta + "\"";
					int resultadoSystem = system(comando.c_str());

					if (resultadoSystem != 0) {
						std::cout << "Error al abrir la documentación.\n";
						std::cout << "Por favor, abra manualmente el archivo:\n" << rutaAbsoluta << "\n";
					}
					else {
						std::cout << "Documentación abierta mediante método alternativo.\n";
					}
				}
				else {
					std::cout << "Documentación abierta correctamente.\n";
				}
			}

			std::cout << "\nPresione cualquier tecla para continuar...\n";
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 13: // operaciones de la base de datos MongoDB
		{
			Utilidades::limpiarPantallaPreservandoMarquesina(1);

			_BaseDatosArchivos baseDatos;
			baseDatos.mostrarMenuBaseDatos();

			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		case 14: // Chat de mensajeria
		{
			Utilidades::limpiarPantallaPreservandoMarquesina(1);

			std::cout << "=== SISTEMA DE CHAT LOCAL ===" << std::endl;
			std::cout << std::endl;
			std::cout << "Este chat utiliza la misma configuración de red que MongoDB:" << std::endl;

			if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
				std::cout << "• Actuando como SERVIDOR (esperará conexiones)" << std::endl;
			}
			else {
				std::cout << "• Actuando como CLIENTE (se conectará al servidor)" << std::endl;
			}

			std::cout << std::endl;
			std::cout << "¿Desea continuar? (s/n): ";
			char respuesta;
			std::cin >> respuesta;
			std::cin.ignore(); // Limpiar el buffer

			if (respuesta == 's' || respuesta == 'S') {
				try {
					AdministradorChatSocket chat;
					chat.iniciarChat();
				}
				catch (const std::exception& e) {
					std::cout << "Error en el chat: " << e.what() << std::endl;
					system("pause");
				}
			}

			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			necesitaRedibujado = true;
			break;
		}
		if (necesitaRedibujado) {
			// Forzar redibujado completo del menú si es necesario
		}
		}
	}
	if (marquesinaGlobal) {
		marquesinaGlobal->detener();
		delete marquesinaGlobal;
	}
	return 0;
}