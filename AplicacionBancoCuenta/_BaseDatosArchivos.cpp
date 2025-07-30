// @file _BaseDatosArchivos.cpp
#include "_BaseDatosArchivos.h"
#include "_BaseDatosPersona.h"
#include "Banco.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <regex>
#include <ctime>
#include <conio.h>
#include <shlobj.h>
#include <vector>
#include "Utilidades.h"

/**
 * @brief Selecciona la colección a exportar desde la base de datos
 *
 * Presenta un menú interactivo para que el usuario seleccione entre las colecciones disponibles:
 * "personas" o "archivos". Si se selecciona "Cancelar", se retorna una cadena vacía.
 *
 * @return Nombre de la colección seleccionada o una cadena vacía si se cancela
 */
std::string _BaseDatosArchivos::seleccionarColeccionParaExportar() {
	std::vector<std::string> opciones = { "personas", "archivos", "Cancelar"};
	int seleccion = Utilidades::menuInteractivo("Seleccione la colección a exportar:", opciones, 0, 0);

	// Aquí, si seleccion es 0 (personas) o 1 (archivos), se devuelve la opción.
	// Si seleccion es 2 ("Cancelar") o -1 (ESC), se devuelve una cadena vacía.
	// Esto es un buen manejo para este sub-menú.
	if (seleccion >= 0 && seleccion < opciones.size() - 1) { // 0 o 1
		return opciones[seleccion];
	}
	return ""; // Devuelve vacío si es "Cancelar" o ESC
}

/**
 * @brief Muestra el menú de opciones para la base de datos
 *
 * Permite al usuario seleccionar entre exportar o importar la base de datos.
 */
void _BaseDatosArchivos::mostrarMenuBaseDatos() {
	std::vector<std::string> opciones = { "Exportar base de datos", "Importar base de datos", "Volver" };
	while (true) {
		Utilidades::limpiarPantallaPreservandoMarquesina(0); // Limpia la pantalla antes de mostrar el menú principal
		int seleccion = Utilidades::menuInteractivo("Menú de Base de Datos", opciones, 0, 0);

		switch (seleccion) {
		case 0: { // Exportar
			Utilidades::limpiarPantallaPreservandoMarquesina(0); // Limpia antes de mostrar el sub-menú
			std::string coleccion = seleccionarColeccionParaExportar();
			if (!coleccion.empty()) { // Solo exporta si se seleccionó una colección válida
				std::string uri = "mongodb://localhost:27017/";
				std::string db = "Banco";
				exportarBackupMongoDB(uri, db, coleccion);
			}
			// Si coleccion.empty() (por Cancelar o ESC), simplemente vuelve al bucle
			// y se redibuja el "Menú de Base de Datos".
			break;
		}
		case 1: { // Importar
			Utilidades::limpiarPantallaPreservandoMarquesina(0); // Limpia antes de la entrada de texto
			std::cout << "Ingrese el nombre completo del archivo de backup (incluya la extension .json): ";
			std::string nombreArchivo;
			// Usar getline para leer la línea completa, especialmente si puedes tener espacios
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpiar el buffer de entrada
			std::getline(std::cin, nombreArchivo);
			// Si la cadena está vacía (por ejemplo, si el usuario solo presiona Enter), podrías querer manejarlo
			if (!nombreArchivo.empty()) {
				std::string uri = "mongodb://localhost:27017/";
				std::string db = "Banco";
				restaurarBackupMongoDB(uri, db, nombreArchivo);
			}
			break;
		}
		case 2: // Opción "Volver"
			return; // Sale de la función mostrarMenuBaseDatos, regresando al menú anterior
		case -1: // Se presionó ESC en "Menú de Base de Datos"
			return; // También sale de la función, comportándose como "Volver"
		default:
			// Esto no debería pasar con la lógica de menuInteractivo que devuelve 0 a numOpciones-1 o -1.
			break;
		}
	}
}

/**
 * @brief Exporta un backup de la base de datos MongoDB a un archivo
 *
 * Este método crea un archivo JSON con los datos de una colección específica de MongoDB.
 * El archivo se guarda en una carpeta llamada "BancoApp" en el escritorio del usuario.
 *
 * @param uri URI de conexión a MongoDB (ejemplo: mongodb+srv://<usuario>:<contraseña>@<cluster-url>)
 * @param db Nombre de la base de datos
 * @param coleccion Nombre de la colección a exportar
 */
void _BaseDatosArchivos::exportarBackupMongoDB(const std::string& uri, const std::string& db, const std::string& coleccion) {
	// Obtener la ruta del escritorio del usuario
	char pathEscritorio[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, pathEscritorio))) {
		std::string rutaCarpeta = std::string(pathEscritorio) + "\\BancoApp";
		// Crear la carpeta BancoApp si no existe
		std::filesystem::create_directories(rutaCarpeta);

		// Obtener fecha y hora actual
		auto now = std::chrono::system_clock::now();
		std::time_t t = std::chrono::system_clock::to_time_t(now);
		std::tm tm;
#ifdef _WIN32
		localtime_s(&tm, &t);
#else
		localtime_r(&t, &tm);
#endif

		// Construir base del nombre de archivo
		std::ostringstream nombreBase;
		nombreBase << "Respaldo_BaseDatos_Banco_" << coleccion << "_N";

		// Buscar el siguiente índice disponible
		int indice = 1;
		std::string nombreArchivo;
		do {
			std::ostringstream nombreCompleto;
			nombreCompleto << rutaCarpeta << "\\" << nombreBase.str() << indice << "_"
				<< std::setfill('0') << std::setw(2) << tm.tm_mday << "_"
				<< std::setfill('0') << std::setw(2) << (tm.tm_mon + 1) << "_"
				<< (tm.tm_year + 1900) << "_"
				<< std::setfill('0') << std::setw(2) << tm.tm_hour << "_"
				<< std::setfill('0') << std::setw(2) << tm.tm_min << "_"
				<< std::setfill('0') << std::setw(2) << tm.tm_sec << ".json";
			nombreArchivo = nombreCompleto.str();
			indice++;
		} while (std::filesystem::exists(nombreArchivo));

		// Exportar la colección a archivo
		mongocxx::client conn{ mongocxx::uri{uri} };
		auto collection = conn[db][coleccion];

		std::ofstream archivo(nombreArchivo);
		for (auto&& doc : collection.find({})) {
			archivo << bsoncxx::to_json(doc) << std::endl;
		}
		archivo.close();

		std::cout << "Backup exportado exitosamente a: " << nombreArchivo << std::endl;
	}
	else {
		std::cerr << "No se pudo obtener la ruta del escritorio del usuario." << std::endl;
	}
}

/**
 * @brief Restaura un backup de MongoDB desde un archivo JSON
 *
 * Este método valida el nombre del archivo, extrae el nombre de la colección
 * y restaura los documentos en la colección correspondiente de la base de datos.
 *
 * @param uri URI de conexión a MongoDB
 * @param db Nombre de la base de datos
 * @param nombreArchivo Nombre del archivo JSON que contiene el backup
 */
void _BaseDatosArchivos::restaurarBackupMongoDB(const std::string& uri, const std::string& db, const std::string& nombreArchivo) {
	// Validar y extraer el nombre de la colección usando regex
	std::regex patron(
		R"(Respaldo_BaseDatos_Banco_(personas|archivos)_N\d+_(0[1-9]|[12][0-9]|3[01])_(0[1-9]|1[0-2])_(\d{4})_([01][0-9]|2[0-3])_([0-5][0-9])_([0-5][0-9])\.json)"
	);
	std::smatch match;
	if (!std::regex_match(nombreArchivo, match, patron)) {
		std::cout << "Nombre de archivo inválido o colección no permitida.\n";
		return;
	}

	// Extraer el nombre de la colección
	std::string coleccion = match[1];

	mongocxx::client conn{ mongocxx::uri{uri} };
	auto collection = conn[db][coleccion];

	std::ifstream archivo(nombreArchivo);
	if (!archivo.is_open()) {
		std::cout << "No se pudo abrir el archivo de backup: " << nombreArchivo << std::endl;
		return;
	}

	std::string linea;
	int insertados = 0;
	while (std::getline(archivo, linea)) {
		if (linea.empty()) continue;
		try {
			auto doc = bsoncxx::from_json(linea);
			collection.insert_one(doc.view());
			insertados++;
		}
		catch (const std::exception& e) {
			std::cout << "Error al insertar documento: " << e.what() << std::endl;
		}
	}
	archivo.close();

	std::cout << "Restauración completada en colección '" << coleccion << "'. Documentos insertados: " << insertados << std::endl;
}