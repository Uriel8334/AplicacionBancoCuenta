//
///**
// * @file main.cpp
// * @brief Sistema de gestión bancaria con interfaz de consola
// *
// * Este archivo implementa la aplicación principal del sistema bancario, proporcionando
// * un menú interactivo para gestionar cuentas, realizar transferencias, buscar información,
// * y generar reportes. Incluye una marquesina en la parte superior para mensajes importantes.
// *
// * @author Uriel Andrade
// * @author Kerly Chuqui
// * @author Abner Proano
// */
//#define _CRT_SECURE_NO_WARNINGS
//
//
//#include "Marquesina.h"
//#include "CodigoQR.h"
//#include "_ExportadorArchivo.h"
//#include "_BaseDatosPersona.h"
//#include "_BaseDatosArchivos.h"
//#include "ConexionMongo.h"
//#include "AdministradorChatSocket.h"
//#include "DocumentacionDoxygen.h"
//#include "Persona.h"
//#include "Banco.h"
//#include "Utilidades.h"
//#include "Cifrado.h" 
//
//#include <iostream>
//#include <string>
//#include <conio.h> // getch()
//#include <windows.h> // SetConsoleCursorPosition
//#include <algorithm>
//#include <vector>
//#include <shellapi.h>
//
// /** @brief Puntero global a la marquesina utilizada en la aplicación */
//Marquesina* marquesinaGlobal = nullptr;
//
///** @brief Bandera atómica para coordinar actualizaciones entre menú y marquesina */
//std::atomic<bool> actualizandoMenu(false);
//
///** @brief Mutex para sincronización entre hilos */
//std::mutex mtxActualizacion;
//
///**
// * @brief Pausa temporalmente la marquesina durante operaciones críticas
// */
//static void pausarMarquesina() {
//	if (marquesinaGlobal) {
//		marquesinaGlobal->pausar();
//	}
//}
//
///**
// * @brief Reanuda la marquesina después de una operación crítica
// */
//static void reanudarMarquesina() {
//	if (marquesinaGlobal) {
//		marquesinaGlobal->reanudar();
//	}
//}
//
///**
// * @brief Muestra información de personas en consola
// *
// * Imprime los datos básicos de las personas del vector proporcionado.
// *
// * @param personas Vector de punteros a objetos Persona a mostrar
// */
//static void mostrarPersonas(const std::vector<Persona*>& personas) {
//	for (auto p : personas) {
//		std::cout << p->getNombres() << " " << p->getApellidos() << " - " << p->getFechaNacimiento() << "\n";
//	}
//}
//
///**
// * @brief Configura la consola para usar codificación UTF-8
// *
// * Esta función establece la consola de Windows para que utilice la codificación UTF-8,
// * permitiendo mostrar correctamente caracteres especiales y acentuados.
// */
//static void configurarConsolaUTF8() {
//	SetConsoleOutputCP(CP_UTF8);
//	SetConsoleCP(CP_UTF8);
//}
//  
///**
// * @brief Permite al usuario seleccionar el modo de conexión a MongoDB
// * @return true si se seleccionó correctamente, false si se canceló
// */
//static bool seleccionarModoConexion() {
//	std::vector<std::string> opcionesConexion = {
//		"SERVIDOR (Local - localhost:27017)",
//		"CLIENTE (Remoto - Red Local Automática)",
//		"INTERNET (MongoDB Atlas - Nube)"
//	};
//
//	std::cout << "=== CONFIGURACIÓN DE CONEXIÓN MONGODB ===" << std::endl;
//	std::cout << std::endl;
//	std::cout << "Seleccione el modo de conexión:" << std::endl;
//	std::cout << std::endl;
//	std::cout << "• SERVIDOR: Para ejecutar en la máquina que tiene MongoDB instalado localmente" << std::endl;
//	std::cout << "• CLIENTE: Para ejecutar en máquinas remotas conectadas al servidor (detección automática)" << std::endl;
//	std::cout << "• INTERNET: Para conectar a MongoDB Atlas en la nube (acceso desde cualquier lugar)" << std::endl;
//	std::cout << std::endl;
//
//	int seleccionConexion = Utilidades::menuInteractivo("Modo de Conexión MongoDB", opcionesConexion, 0, 0);
//
//	system("cls");
//	system("pause");
//
//	if (seleccionConexion == -1) {
//		// Usuario presionó ESC
//		std::cout << "Configuración cancelada. Saliendo del sistema..." << std::endl;
//		return false;
//	}
//
//	// Configurar el modo según la selección
//	if (seleccionConexion == 0) {
//		ConexionMongo::setModoConexion(ConexionMongo::SERVIDOR);
//		std::cout << std::endl;
//		std::cout << "=== MODO SERVIDOR SELECCIONADO ===" << std::endl;
//		std::cout << "• Conectará a: mongodb://localhost:27017" << std::endl;
//		std::cout << "• Asegúrese de que MongoDB esté ejecutándose localmente" << std::endl;
//	}
//	else if (seleccionConexion == 1) {
//		ConexionMongo::setModoConexion(ConexionMongo::CLIENTE);
//		std::cout << std::endl;
//		std::cout << "=== MODO CLIENTE SELECCIONADO ===" << std::endl;
//		std::cout << "• Detectando configuración de red automáticamente..." << std::endl;
//		std::cout << "• Se escaneará la red local para encontrar servidores MongoDB" << std::endl;
//		std::cout << "• Asegúrese de estar conectado a la misma red que el servidor" << std::endl;
//	}
//	else {
//		ConexionMongo::setModoConexion(ConexionMongo::INTERNET);
//		std::cout << std::endl;
//		std::cout << "=== MODO INTERNET SELECCIONADO ===" << std::endl;
//		std::cout << "• Conectará a: MongoDB Atlas (sistemaaplicacioncuenta.qnzmjnz.mongodb.net)" << std::endl;
//		std::cout << "• Base de datos compartida en la nube - ideal para desarrollo colaborativo" << std::endl;
//		std::cout << "• Asegúrese de tener conexión a Internet estable" << std::endl;
//		std::cout << "• Perfecto para trabajo remoto y control de versiones con Git" << std::endl;
//	}
//
//	system("pause");
//	system("cls");
//
//	std::cout << std::endl;
//	return true;
//}
//
///**
// * @brief Opciones del menú principal del sistema bancario
// *
// * Contiene las diferentes opciones que el usuario puede seleccionar
// * para interactuar con el sistema bancario.
// */
//std::vector<std::string> opcionesMenuPrincipal = {
//	"Crear Cuenta",
//	"Buscar Cuenta",
//	"Operaciones de Cuenta",
//	"Realizar Transferencias",
//	"Guardar Archivo",
//	"Recuperar Archivo",
//	"Descifrar Archivo",
//	"Menu de ayuda",
//	"Explorador de archivos",
//	"Gestion de Hash",
//	"Arbol B",
//	"Generar QR",
//	"Abrir documentacion",
//	"Operaciones Base de Datos",
//	"Chat de mensajeria",
//	"Salir"
//};
//
//// Este archivo contiene solo la porción refactorizada del main() y su ciclo principal
//// usando principios SOLID sin modificar nombres de funciones existentes.
//static void manejarSeleccionMenuPrincipal(int seleccion, Banco& banco, bool& necesitaRedibujado) {
//	Utilidades::gotoxy(0, static_cast<int>(opcionesMenuPrincipal.size()) + 1);
//	_BaseDatosArchivos baseDatosArchivos;
//	DocumentacionDoxygen documentoDoxygen;
//
//
//	if (seleccion == 15) {
//		Utilidades::ocultarCursor();
//		Utilidades::limpiarPantallaPreservandoMarquesina(0);
//		std::cout << "Saliendo del sistema...\n";
//		exit(0);
//	}
//	Utilidades::ocultarCursor();
//
//	static const std::map<int, std::function<void()>> acciones = {
//		{ 0, [&]() { banco.agregarPersonaConCuenta(); }},
//		{ 1, [&]() { banco.buscarCuenta(); }},
//		{ 2, [&]() {
//			if (banco.verificarCuentasBanco()) {
//				Utilidades::ocultarCursor();
//				banco.subMenuCuentasBancarias();
//			}
//		}},
//		{ 3, [&]() { banco.realizarTransferencia(); }},
//		//{ 4, [&]() { Utilidades::guardarArchivoInteractivo(banco); }},
//		//{ 5, [&]() { Utilidades::recuperarArchivoInteractivo(banco); }},
//		//{ 6, [&]() { Utilidades::descifrarArchivoInteractivo(banco); }},
//		{ 7, [&]() { Utilidades::mostrarMenuAyuda(); system("pause"); }},
//		//{ 8, [&]() { Utilidades::exploradorArchivosInteractivo(banco); }},
//		//{ 9, [&]() { Utilidades::gestionHashInteractiva(); }},
//		//{ 10, [&]() { Utilidades::arbolBInteractivo(); }},
//		//{ 11, [&]() { CodigoQR::generarCodigoQRInteractivo(); }},
//		{ 12, [&]() { documentoDoxygen.abrirDocumentacion(); }},
//		{ 13, [&]() { baseDatosArchivos.mostrarMenuBaseDatos(); }},
//		{ 14, [&]() { AdministradorChatSocket::iniciarChatInteractivo(); }}
//	};
//	auto it = acciones.find(seleccion);
//	if (it != acciones.end()) {
//		it->second();
//		Utilidades::ocultarCursor();
//		Utilidades::limpiarPantallaPreservandoMarquesina(1);
//		necesitaRedibujado = true;
//	}
//}
//
///**
// * @brief Función principal del programa
// *
// * Configura la consola, establece la conexión a MongoDB y muestra el menú principal.
// * Permite al usuario interactuar con el sistema bancario educativo.
// *
// * @return Código de salida del programa
// */
//int main() {
//	configurarConsolaUTF8();
//	std::cout << "SISTEMA BANCARIO EDUCATIVO" << std::endl << "===================================\n" << std::endl;
//	if (!seleccionarModoConexion()) return 0;
//	try {
//		ConexionMongo::getCliente();
//		std::cout << "Conexión MongoDB establecida correctamente\n";
//	}
//	catch (const std::exception& e) {
//		std::cerr << "Error al conectar con MongoDB: " << e.what() << "\n";
//		system("pause");
//		return -1;
//	}
//
//	system("pause");
//	Utilidades::limpiarPantallaPreservandoMarquesina(0);
//	Banco banco;
//	int x = 0, y = 0;
//	CONSOLE_SCREEN_BUFFER_INFO csbi;
//	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
//	int anchoConsola = csbi.srWindow.Right - csbi.srWindow.Left + 1;
//	marquesinaGlobal = new Marquesina(0, 0, anchoConsola, "marquesina.html", 200);
//	marquesinaGlobal->iniciar();
//	Utilidades::centrarVentanaConsola();
//	std::cout << std::endl << std::endl;
//	Utilidades::ocultarCursor();
//	std::vector<std::string> opciones = opcionesMenuPrincipal;
//	for (int i = 0; i < static_cast<int>(opciones.size()); i++) std::cout << std::endl;
//	while (true) {
//		bool necesitaRedibujado = false;
//		int seleccion = Utilidades::menuInteractivo("SISTEMA BANCARIO-EDUCATIVO", opciones, x, y);
//		manejarSeleccionMenuPrincipal(seleccion, banco, necesitaRedibujado);
//	}
//}


/**
 * @file main.cpp
 * @brief Sistema de gestión bancaria con interfaz de consola
 *
 * Este archivo implementa la aplicación principal del sistema bancario, proporcionando
 * un menú interactivo para gestionar cuentas, realizar transferencias, buscar información,
 * y generar reportes. Refactorizado aplicando principios SOLID y código limpio.
 *
 * @author Uriel Andrade
 * @author Kerly Chuqui
 * @author Abner Proano
 */
#define _CRT_SECURE_NO_WARNINGS

#include "AplicacionPrincipal.h"
#include "Marquesina.h"
#include <atomic>
#include <mutex>

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
 * @brief Función principal del programa
 *
 * Punto de entrada principal que utiliza el patrón Application Controller
 * para coordinar toda la aplicación del sistema bancario.
 *
 * @return Código de salida del programa
 */
int main() {
	try {
		AplicacionPrincipal aplicacion;
		return aplicacion.ejecutar();
	}
	catch (const std::exception& e) {
		std::cerr << "Error crítico en la aplicación: " << e.what() << std::endl;
		system("pause");
		return -1;
	}
	catch (...) {
		std::cerr << "Error desconocido en la aplicación" << std::endl;
		system("pause");
		return -1;
	}
}

