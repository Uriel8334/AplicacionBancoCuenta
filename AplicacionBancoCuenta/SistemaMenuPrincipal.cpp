#include "SistemaMenuPrincipal.h"
#include "ConexionMongo.h"
#include "_ExportadorArchivo.h"

SistemaMenuPrincipal::SistemaMenuPrincipal(Banco& bancoRef) : banco(bancoRef), baseDatosPersona(ConexionMongo::obtenerClienteBaseDatos()) {
	inicializarOpciones();
	inicializarAcciones();
}

void SistemaMenuPrincipal::inicializarOpciones() {
	opcionesMenu = {
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
}

void SistemaMenuPrincipal::inicializarAcciones() {
	mapaAcciones = {
		{ 0, [this]() { ejecutarCrearCuenta(); }}, // LISTO
		{ 1, [this]() { ejecutarBuscarCuenta(); }}, // LISTO
		{ 2, [this]() { ejecutarOperacionesCuenta(); }}, // LISTO
		{ 3, [this]() { ejecutarTransferencias(); }}, // LISTO
		{ 4, [this]() { ejecutarGuardarArchivo(); }}, 
		{ 5, [this]() { ejecutarRecuperarArchivo(); }}, 
		{ 6, [this]() { ejecutarDescifrarArchivo(); }}, 
		{ 7, [this]() { ejecutarMenuAyuda(); }}, // LISTO
		{ 8, [this]() { ejecutarExploradorArchivos(); }}, // LISTO
		{ 9, [this]() { ejecutarGestionHash(); }}, // LISTO
		{ 10, [this]() { ejecutarArbolB(); }}, // LISTO
		{ 11, [this]() { ejecutarGenerarQR(); }}, // LISTO
		{ 12, [this]() { ejecutarDocumentacion(); }}, // LISTO
		{ 13, [this]() { ejecutarBaseDatos(); }}, // LISTO
		{ 14, [this]() { ejecutarChat(); }}, // LISTO
		{ 15, [this]() { ejecutarSalir(); }} // LISTO
	};
}

void SistemaMenuPrincipal::mostrarMenu() {
	for (int i = 0; i < static_cast<int>(opcionesMenu.size()); i++) {
		std::cout << std::endl;
	}
}

void SistemaMenuPrincipal::procesarSeleccion(int seleccion, bool& necesitaRedibujado) {
	if (seleccion == 15) {
		ejecutarSalir();
		return;
	}

	ejecutarOpcionMenu(seleccion, necesitaRedibujado);
}

void SistemaMenuPrincipal::ejecutarOpcionMenu(int seleccion, bool& necesitaRedibujado) {
	Utilidades::gotoxy(0, static_cast<int>(opcionesMenu.size()) + 1);
	Utilidades::ocultarCursor();

	auto it = mapaAcciones.find(seleccion);
	if (it != mapaAcciones.end()) {
		it->second();
		Utilidades::ocultarCursor();
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		necesitaRedibujado = true;
	}
}

// === IMPLEMENTACIÓN DE ACCIONES ESPECÍFICAS ===

void SistemaMenuPrincipal::ejecutarCrearCuenta() {
	banco.agregarPersonaConCuenta();
}

void SistemaMenuPrincipal::ejecutarBuscarCuenta() {
	banco.buscarCuenta();
}

void SistemaMenuPrincipal::ejecutarOperacionesCuenta() {
	if (banco.verificarCuentasBanco()) {
		Utilidades::ocultarCursor();
		banco.subMenuCuentasBancarias();
	}
}

void SistemaMenuPrincipal::ejecutarTransferencias() {
	banco.realizarTransferencia();
}

void SistemaMenuPrincipal::ejecutarMenuAyuda() {
	Utilidades::mostrarMenuAyuda();
	system("pause");
}

void SistemaMenuPrincipal::ejecutarDocumentacion() {
	documentoDoxygen.abrirDocumentacion();
}

void SistemaMenuPrincipal::ejecutarBaseDatos() {
	baseDatosArchivos.mostrarMenuBaseDatos();
}

void SistemaMenuPrincipal::ejecutarChat() {
	AdministradorChatSocket::iniciarChatInteractivo();
}

void SistemaMenuPrincipal::ejecutarSalir() {
	Utilidades::ocultarCursor();
	Utilidades::limpiarPantallaPreservandoMarquesina(0);
	std::cout << "Saliendo del sistema...\n";
	exit(0);
}

void SistemaMenuPrincipal::ejecutarGenerarQR() {
	Utilidades::generarQR();
}

void SistemaMenuPrincipal::ejecutarExploradorArchivos()
{
	Utilidades::exploradorArchivosInteractivo(banco);
}

void SistemaMenuPrincipal::ejecutarGestionHash()
{
	Utilidades::gestionHashInteractiva();
}

void SistemaMenuPrincipal::ejecutarArbolB() {
	baseDatosPersona.iniciarBaseDatosArbolB();
}

/**
 * @brief Implementación en SistemaMenuPrincipal para usar la nueva funcionalidad
 */
void SistemaMenuPrincipal::ejecutarGuardarArchivo() {
	ExportadorArchivo::procesarSolicitudGuardado(baseDatosPersona);
}

void SistemaMenuPrincipal::ejecutarRecuperarArchivo()
{
	ExportadorArchivo::procesarSolicitudRecuperacion(baseDatosPersona);
}

void SistemaMenuPrincipal::ejecutarDescifrarArchivo()
{
	bool resultado = Cifrado::iniciarProcesoDescifrado(baseDatosPersona);
	(void)resultado; // Evitar warning de variable no utilizada si es necesario
}

const std::vector<std::string>& SistemaMenuPrincipal::getOpciones() const {
	return opcionesMenu;
}