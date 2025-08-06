/**
 * @file Utilidades.cpp
 * @brief Implementación de diversas utilidades y estructuras de datos para el sistema bancario
 *
 * Este archivo contiene la implementación de clases de utilidad para el sistema bancario,
 * incluyendo una implementación didáctica de un Árbol B, funciones para manipulación de
 * texto, control de interfaz de usuario y generación de códigos QR.
 */

#define _CRT_SECURE_NO_WARNINGS


#include <windows.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <regex>
#include <string>
#include <ctime>
#include <chrono>
#include <queue>
#include <limits>
#include <set>
#include <algorithm>
#include <numeric>
#include <mutex>
#include <thread>
#include <vector>
#include <shellapi.h>

#include "CodigoQR.h"
#include "Marquesina.h"
#include "Utilidades.h"
#include "ArbolBGrafico.h"
#include "ArbolB.h"
#include "NodoPersona.h"
#include "Persona.h"
#include "ConexionMongo.h"
#include "_BaseDatosPersona.h"
#include "GestorHashBaseDatos.h"

 // Variable externa para acceso a la marquesina global
extern Marquesina* marquesinaGlobal;
// Variables para control de operaciones críticas y actualizaciones de menú
extern std::atomic<bool> actualizandoMenu;
extern std::mutex mtxActualizacion;

/**
 * @brief Muestra un menú en la consola
 *
 * Esta función muestra un menú de opciones en la consola, resaltando la opción seleccionada
 * y permitiendo al usuario navegar por las opciones con las teclas de flecha.
 *
 * @param seleccion Índice de la opción seleccionada actualmente
 * @param opciones Lista de opciones a mostrar en el menú
 * @param x Coordenada X para la posición del menú
 * @param y Coordenada Y para la posición del menú
 * @param seleccionAnterior Opción seleccionada anteriormente (por defecto -1)
 */
void Utilidades::mostrarMenu(int seleccion, const std::vector<std::string>& opciones, int x, int y, int seleccionAnterior) {
	std::lock_guard<std::mutex> lock(mtxActualizacion);
	actualizandoMenu = true;

	if (seleccionAnterior != -1 && seleccionAnterior != seleccion) {
		Utilidades::gotoxy(x, y + seleccionAnterior);
		std::cout << "    " << opciones[seleccionAnterior] << "    ";
	}

	Utilidades::gotoxy(x, y + seleccion);
	std::cout << " > " << opciones[seleccion] << "    ";
	std::cout.flush();
	actualizandoMenu = false;
}

/**
 * @brief Muestra un menú interactivo y permite al usuario seleccionar una opción
 *
 * Permite al usuario navegar por un menú de opciones utilizando las teclas de flecha
 * y seleccionar una opción con Enter. También maneja la tecla ESC para cancelar.
 *
 * @param titulo Título del menú a mostrar
 * @param opciones Lista de opciones a mostrar en el menú
 * @param x Coordenada X para la posición del menú
 * @param y Coordenada Y para la posición del menú
 * @return Índice de la opción seleccionada o -1 si se presiona ESC
 */
int Utilidades::menuInteractivo(const std::string& titulo, const std::vector<std::string>& opciones, int x, int y) {
	int seleccion = 0;
	int seleccionAnterior = -1;
	int numOpciones = static_cast<int>(opciones.size());

	if (!titulo.empty()) {
		Utilidades::gotoxy(x, y);
		std::cout << titulo << "\n\n";
	}
	int opcionesY = y + (titulo.empty() ? 0 : 2);

	for (int i = 0; i < numOpciones; ++i) {
		Utilidades::gotoxy(x, opcionesY + i);
		if (i == seleccion) {
			std::cout << " > " << opciones[i] << "    ";
		}
		else {
			std::cout << "    " << opciones[i] << "    ";
		}
	}
	std::cout.flush();

	while (true) {
		int tecla = _getch();
		if (tecla == 224) {
			tecla = _getch();
			if (tecla == 72) { // Flecha arriba
				seleccionAnterior = seleccion;
				seleccion = (seleccion - 1 + numOpciones) % numOpciones;
				Utilidades::mostrarMenu(seleccion, opciones, x, opcionesY, seleccionAnterior);
			}
			else if (tecla == 80) { // Flecha abajo
				seleccionAnterior = seleccion;
				seleccion = (seleccion + 1) % numOpciones;
				Utilidades::mostrarMenu(seleccion, opciones, x, opcionesY, seleccionAnterior);
			}
		}
		else if (tecla == 13) { // Enter
			return seleccion;
		}
		else if (tecla == 27) { // ESC
			return -1; // Devuelve -1 para indicar ESC
		}
	}
}

/**
* @brief Marca el inicio de una operación crítica que no debe ser interrumpida
*
* Notifica a la marquesina para pausar actualizaciones durante operaciones críticas
*/
void Utilidades::iniciarOperacionCritica()
{
	if (marquesinaGlobal)
	{
		marquesinaGlobal->marcarOperacionCritica();
		// Pequeña pausa para asegurar que la marquesina se detenga
		Sleep(10);
	}
}

/**
 * @brief Marca el fin de una operación crítica
 *
 * Notifica a la marquesina para reanudar actualizaciones normales
 */
void Utilidades::finalizarOperacionCritica()
{
	if (marquesinaGlobal)
	{
		marquesinaGlobal->finalizarOperacionCritica();
	}
}

/**
 * @brief Posiciona el cursor en coordenadas específicas de la consola
 *
 * Versión mejorada y thread-safe de la función gotoxy tradicional
 *
 * @param x Posición horizontal (columna)
 * @param y Posición vertical (fila)
 */
void Utilidades::gotoxy(int x, int y)
{
	// Marcar operación crítica de cursor
	iniciarOperacionCritica();

	COORD coord{};
	coord.X = x;
	coord.Y = y + 2; // Offset para la marquesina

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsole, coord);

	// Finalizar operación crítica después de una pausa mínima
	Sleep(1);
	finalizarOperacionCritica();
}

/**
 * @brief Limpia la pantalla preservando el área de la marquesina
 *
 * @param lineasMarquesina Número de líneas reservadas para la marquesina
 */
void Utilidades::limpiarPantallaPreservandoMarquesina(int lineasMarquesina)
{
	iniciarOperacionCritica();

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	int ancho = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int alto = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	COORD startCoords = { 0, (SHORT)lineasMarquesina };
	SetConsoleCursorPosition(hConsole, startCoords);

	DWORD caracteresEscritos;
	int espaciosAEscribir = ancho * (alto - lineasMarquesina);

	// Usar API más eficiente
	FillConsoleOutputCharacter(hConsole, ' ', espaciosAEscribir, startCoords, &caracteresEscritos);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, espaciosAEscribir, startCoords, &caracteresEscritos);

	SetConsoleCursorPosition(hConsole, startCoords);

	finalizarOperacionCritica();
}


/**
 * @brief Convierte una cadena a valor numérico double
 *
 * Maneja excepciones y realiza validaciones para evitar errores
 *
 * @param texto Cadena a convertir
 * @return double Valor numérico obtenido, 0.0 en caso de error
 */
double Utilidades::ConvertirADouble(const std::string& texto) {
	if (texto.empty()) {
		std::cout << "Texto vacio, retornando 0.0" << std::endl;
		return 0.0;
	}

	std::string soloNumeros = ExtraerNumerosParaDouble(texto);

	if (soloNumeros.empty() || soloNumeros == "-") {
		std::cout << "No se encontraron digitos validos, retornando 0.0" << std::endl;
		return 0.0;
	}

	try {
		double resultado = std::stod(soloNumeros);
		std::cout << "Conversion exitosa: " << std::fixed << std::setprecision(2) << resultado << std::endl;
		return resultado;
	}
	catch (const std::invalid_argument&) {
		std::cout << "Error: argumento invalido en la conversion" << std::endl;
		return 0.0;
	}
	catch (const std::out_of_range&) {
		std::cout << "Error: valor fuera de rango" << std::endl;
		return 0.0;
	}
	catch (const std::exception& e) {
		std::cout << "Error desconocido: " << e.what() << std::endl;
		return 0.0;
	}
}

/**
 * @brief Extrae los caracteres válidos para un double de una cadena
 * @param texto Cadena a procesar
 * @return Cadena con solo los caracteres válidos para un double
 */
std::string Utilidades::ExtraerNumerosParaDouble(const std::string& texto) {
	std::string soloNumeros;
	bool puntoEncontrado = false;
	for (const char& c : texto) {
		if (std::isdigit(c)) {
			soloNumeros += c;
		}
		else if (c == '.' && !puntoEncontrado) {
			soloNumeros += c;
			puntoEncontrado = true;
		}
		else if (c == '-' && soloNumeros.empty()) {
			soloNumeros += c;
		}
	}
	return soloNumeros;
}

/**
 * @brief Formatea un valor numérico con separadores de miles y decimales
 *
 * @param monto Valor a formatear
 * @param decimales Número de decimales a mostrar
 * @return std::string Valor formateado como texto
 */
std::string Utilidades::FormatearMonto(double monto, int decimales) {
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(decimales) << monto;
	std::string resultado = oss.str();

	size_t posDecimal = resultado.find('.');
	if (posDecimal == std::string::npos) {
		posDecimal = resultado.length();
	}

	// Refactor: Separar la inserción de comas en una función auxiliar
	resultado = InsertarSeparadoresMiles(resultado, posDecimal);

	return resultado;
}

/**
 * @brief Inserta separadores de miles en una cadena numérica
 * @param numero Cadena numérica
 * @param posDecimal Posición del punto decimal
 * @return Cadena con separadores de miles
 */
std::string Utilidades::InsertarSeparadoresMiles(const std::string& numero, size_t posDecimal) {
	std::string resultado = numero;
	if (posDecimal > 3) {
		for (size_t i = posDecimal - 3; i > 0; i -= 3) {
			resultado.insert(i, ",");
			if (i <= 3) break;
		}
	}
	return resultado;
}

/**
 * @brief Formatea una fecha a partir de sus componentes
 *
 * @param dia Día del mes
 * @param mes Mes del año
 * @param anio Año
 * @return std::string Fecha formateada como "DD/MM/AAAA"
 */
std::string Utilidades::FormatearFecha(int dia, int mes, int anio) {
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(2) << dia << "/"
		<< std::setfill('0') << std::setw(2) << mes << "/"
		<< anio;

	return oss.str();
}

/**
 * @brief Verifica si una cadena representa un valor numérico válido
 *
 * @param texto Cadena a verificar
 * @return bool true si es numérico, false en caso contrario
 */
bool Utilidades::EsNumerico(const std::string& texto) {
	if (texto.empty()) {
		return false;
	}

	bool puntoDecimal = false;
	bool tieneDigito = false;

	for (size_t i = 0; i < texto.length(); i++) {
		if (i == 0 && texto[i] == '-') {
			// Permitir signo negativo al inicio
			continue;
		}

		if (texto[i] == '.') {
			if (puntoDecimal) {
				// Mas de un punto decimal
				return false;
			}
			puntoDecimal = true;
		}
		else if (!std::isdigit(texto[i])) {
			return false;
		}
		else {
			tieneDigito = true;
		}
	}

	return tieneDigito;
}

/**
 * @brief Valida si una cadena es un correo electrónico válido
 *
 * @param correo Cadena a validar
 * @return bool true si es un correo válido, false en caso contrario
 */
bool Utilidades::EsCorreoValido(const std::string& correo) {
	// Expresion regular simple para validar correos electronicos
	const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
	return std::regex_match(correo, pattern);
}

/**
 * @brief Elimina todos los espacios de una cadena
 *
 * @param texto Cadena original
 * @return std::string Cadena sin espacios
 */
std::string Utilidades::EliminarEspacios(const std::string& texto) {
	std::string resultado = texto;
	resultado.erase(std::remove_if(resultado.begin(), resultado.end(),
		[](unsigned char c) { return std::isspace(c); }),
		resultado.end());
	return resultado;
}

/**
 * @brief Convierte todos los caracteres de una cadena a mayúsculas
 *
 * @param texto Cadena original
 * @return std::string Cadena en mayúsculas
 */
std::string Utilidades::ConvertirAMayusculas(const std::string& texto) {
	std::string resultado = texto;
	std::transform(resultado.begin(), resultado.end(), resultado.begin(),
		[](unsigned char c) { return std::toupper(c); });
	return resultado;
}

/**
 * @brief Convierte todos los caracteres de una cadena a minúsculas
 *
 * @param texto Cadena original
 * @return std::string Cadena en minúsculas
 */
std::string Utilidades::ConvertirAMinusculas(const std::string& texto) {
	std::string resultado = texto;
	std::transform(resultado.begin(), resultado.end(), resultado.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return resultado;
}

/**
 * @brief Devuelve un mensaje para regresar al menú principal
 *
 * @return std::string Mensaje estándar
 */
std::string Utilidades::Regresar() {
	return "Regresar al menu principal";
}

/**
 * @brief Muestra el menú de ayuda del sistema
 *
 * Ejecuta la aplicación externa de ayuda desde la misma ruta del programa
 */
void Utilidades::mostrarMenuAyuda() {
	char path[MAX_PATH];

	// Obtiene la ruta completa del ejecutable actual
	GetModuleFileNameA(NULL, path, MAX_PATH);

	// Convertimos a string para manipularlo
	std::string ruta(path);

	// Eliminamos el nombre del ejecutable para obtener solo la carpeta
	size_t pos = ruta.find_last_of("\\/");
	if (pos != std::string::npos) {
		ruta = ruta.substr(0, pos);
	}

	// Construimos la ruta del ejecutable de ayuda
	ruta += "\\ayuda\\AyudaSistemaBancario.exe";

	// Ejecutamos el menu de ayuda
	HINSTANCE resultado = ShellExecuteA(NULL, "open", ruta.c_str(), NULL, NULL, SW_SHOWNORMAL);

	if (reinterpret_cast<INT_PTR>(resultado) <= 32) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cerr << "Error al abrir el menu de ayuda. Codigo: " << reinterpret_cast<INT_PTR>(resultado) << std::endl;
	}


}

/**
 * @brief Calcula un hash simplificado de un archivo
 *
 * Implementación didáctica de un algoritmo de hash para archivos
 *
 * @param rutaArchivo Ruta al archivo a procesar
 * @return std::string Hash generado
 */
std::string Utilidades::calcularSHA1(const std::string& rutaArchivo) {
	std::ifstream archivo(rutaArchivo, std::ios::binary);
	if (!archivo) {
		return "ERROR_ARCHIVO_NO_ENCONTRADO";
	}

	// --- Principios basicos del hashing ---
	// 1. Inicializacion de valores semilla
	// Usamos valores primos como semilla para mejor distribucion
	std::size_t h1 = 0x01234567;
	std::size_t h2 = 0x89ABCDEF;
	std::size_t h3 = 0xFEDCBA98;
	std::size_t h4 = 0x76543210;

	// 2. Variables para el procesamiento por bloques
	unsigned char buffer[64]{}; // Tamaño de bloque tipico: 64 bytes
	std::size_t totalBytes = 0;
	std::size_t bytesLeidos = 0;

	// 3. Lectura y procesamiento por bloques (simulando hash real)
	while ((bytesLeidos = archivo.read(reinterpret_cast<char*>(buffer),
		sizeof(buffer)).gcount()) > 0) {
		// Procesamos cada byte con operaciones de hash
		for (std::size_t i = 0; i < bytesLeidos; ++i) {
			// Aplicamos funcion de mezcla a cada byte
			h1 = ((h1 << 5) | (h1 >> 27)) ^ buffer[i];  // Rotacion circular y XOR
			h2 = ((h2 << 7) | (h2 >> 25)) + buffer[i];  // Rotacion y suma
			h3 = h3 * 33 + ~buffer[i];                  // Multiplicacion y negacion
			h4 = ((h4 >> 3) | (h4 << 29)) ^ buffer[i];  // Rotacion inversa y XOR

			// Mezclamos los estados del hash periodicamente
			if (i % 16 == 15) {
				std::size_t temp = h1;
				h1 = h2 ^ h3;
				h2 = h3 + h4;
				h3 = h4 ^ temp;
				h4 = temp + h1;
			}
		}

		totalBytes += bytesLeidos;
	}

	// 4. Finalizacion: incorporamos el tamaño al hash (importante en hashes criptograficos)
	h1 ^= totalBytes;
	h2 += totalBytes;
	h3 ^= (h1 ^ h2);
	h4 += (h2 ^ h3);

	// 5. Mezclado final para garantizar avalancha (pequeños cambios → grandes diferencias)
	for (int i = 0; i < 3; ++i) {
		h1 = ((h1 << 13) | (h1 >> 19)) + h4;
		h2 = ((h2 << 17) | (h2 >> 15)) ^ h1;
		h3 = ((h3 << 7) | (h3 >> 25)) + h2;
		h4 = ((h4 << 11) | (h4 >> 21)) ^ h3;
	}

	// 6. Convertir a representacion hexadecimal (32 caracteres)
	std::stringstream ss;
	ss << std::hex << std::setfill('0')
		<< std::setw(8) << h1
		<< std::setw(8) << h2
		<< std::setw(8) << h3
		<< std::setw(8) << h4;

	// 7. Añadir el tamaño del archivo como informacion extra
	return ss.str() + "-" + std::to_string(totalBytes);
}

/**
 * @brief Verifica si el hash de un archivo coincide con uno esperado
 *
 * @param rutaArchivo Ruta al archivo a verificar
 * @param hashEsperado Hash esperado para comparación
 * @return bool true si los hashes coinciden, false en caso contrario
 */
bool Utilidades::verificarSHA1(const std::string& rutaArchivo, const std::string& hashEsperado) {
	std::string hashActual = calcularSHA1(rutaArchivo);

	// Informe detallado para fines educativos
	if (hashActual == hashEsperado) {
		std::cout << "Hash verificado exitosamente." << std::endl;
		std::cout << "  • Hash esperado/recibido: " << hashEsperado << std::endl;
		std::cout << "  • Hash actual/calculado: " << hashActual << std::endl;
		return true;
	}
	else {
		std::cout << "¡ADVERTENCIA! Hash no coincide." << std::endl;
		std::cout << "  • Hash esperado/recibido: " << hashEsperado << std::endl;
		std::cout << "  • Hash actual/calculado: " << hashActual << std::endl;
		return false;
	}
}

/**
 * @brief Guarda el hash de un archivo en un archivo separado
 *
 * @param rutaArchivo Ruta al archivo original
 * @param hash Hash a guardar
 */
void Utilidades::guardarHashArchivo(const std::string& rutaArchivo, const std::string& hash) {
	std::string rutaHash = rutaArchivo + ".hash";
	std::ofstream archivoHash(rutaHash);

	if (archivoHash) {
		// Añadimos cabecera informativa con fecha
		time_t tiempoActual = time(nullptr);
		struct tm timeinfo;
		localtime_s(&timeinfo, &tiempoActual);  // Version segura
		char buffer[128];
		std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);

		archivoHash << "# Hash de integridad del archivo: " << rutaArchivo << std::endl;
		archivoHash << "# Generado: " << buffer << std::endl;
		archivoHash << "# Formato: [hash-tamaño_bytes]" << std::endl;
		archivoHash << hash;
		archivoHash.close();
		std::cout << "Hash guardado en: " << rutaHash << std::endl;
	}
	else {
		std::cerr << "Error al guardar el hash" << std::endl;
	}
}

/**
 * @brief Lee el hash desde un archivo de hash
 *
 * @param rutaHashArchivo Ruta al archivo de hash
 * @return std::string Hash leído o cadena vacía en caso de error
 */
std::string Utilidades::leerHashArchivo(const std::string& rutaHashArchivo) {
	std::ifstream archivoHash(rutaHashArchivo);
	if (!archivoHash) {
		return "";
	}

	std::string linea;
	std::string hash;

	// Leer linea por linea hasta encontrar una que no sea comentario
	while (std::getline(archivoHash, linea)) {
		// Saltamos lineas que empiezan con # (comentarios)
		if (!linea.empty() && linea[0] != '#') {
			hash = linea;
			break;
		}
	}

	return hash;
}


/**
 * @brief Presenta datos usando una estructura de árbol B para visualización y manipulación
 *
 * Construye un árbol B a partir de una lista de personas y permite buscar y eliminar en él
 * con visualización animada similar a la de una herramienta didáctica de estructuras de datos.
 *
 * @param cabeza Puntero al primer nodo de la lista de personas
 */
void Utilidades::PorArbolB(NodoPersona* cabeza) {
	if (!cabeza) {
		std::cout << "No hay datos para mostrar." << std::endl;
		system("pause");
		return;
	}

	// Opciones de criterios de ordenamiento (sin tildes)
	std::vector<std::string> criterios = { "Cedula", "Nombre", "Apellido", "Fecha de nacimiento" };
	int selCriterio = 0;

	// Recolectar personas de la lista
	std::vector<Persona*> personas;
	NodoPersona* actual = cabeza;
	while (actual) {
		if (actual->persona && actual->persona->isValidInstance()) {
			personas.push_back(actual->persona);
		}
		actual = actual->siguiente;
	}

	if (personas.empty()) {
		std::cout << "No hay personas validas para procesar." << std::endl;
		system("pause");
		return;
	}

	// Función local para mostrar menú sin parpadeo
	auto mostrarMenuCriterios = [&criterios, &selCriterio]() {
		limpiarPantallaPreservandoMarquesina(2);
		std::cout << "=== ARBOL B DIDACTICO ===" << std::endl;
		std::cout << "Seleccione criterio de ordenamiento:" << std::endl;

		for (size_t i = 0; i < criterios.size(); i++) {
			if (i == selCriterio)
				std::cout << " > " << criterios[i] << std::endl;
			else
				std::cout << "   " << criterios[i] << std::endl;
		}
		};

	// Menú inicial
	mostrarMenuCriterios();

	// Navegación del menú
	while (true) {
		int tecla = _getch();

		if (tecla == 224) {
			tecla = _getch();
			if (tecla == 72) // Flecha arriba
				selCriterio = static_cast<int>((static_cast<unsigned long long>(selCriterio) - 1 + criterios.size()) % criterios.size());
			else if (tecla == 80) // Flecha abajo
				selCriterio = static_cast<int>((static_cast<unsigned long long>(selCriterio) + 1) % criterios.size());

			// Redibujar todo el menú
			mostrarMenuCriterios();
		}
		else if (tecla == 13) break; // Enter
		else if (tecla == 27) return; // ESC
	}

	// Definir comparadores para criterios
	std::function<bool(const Persona*, const Persona*)> criterioOrdenamiento;
	std::function<bool(const Persona*, const std::string&)> criterioBusqueda;

	switch (selCriterio) {
	case 0: // Cedula
		criterioOrdenamiento = [](const Persona* a, const Persona* b) {
			return a->getCedula() < b->getCedula();
			};
		criterioBusqueda = [](const Persona* p, const std::string& cedula) {
			return p->getCedula() == cedula;
			};
		break;
	case 1: // Nombre
		criterioOrdenamiento = [](const Persona* a, const Persona* b) {
			return ConvertirAMinusculas(a->getNombres()) <
				ConvertirAMinusculas(b->getNombres());
			};
		criterioBusqueda = [](const Persona* p, const std::string& nombre) {
			return ConvertirAMinusculas(p->getNombres()).find(
				ConvertirAMinusculas(nombre)) != std::string::npos;
			};
		break;
	case 2: // Apellido
		criterioOrdenamiento = [](const Persona* a, const Persona* b) {
			return ConvertirAMinusculas(a->getApellidos()) <
				ConvertirAMinusculas(b->getApellidos());
			};
		criterioBusqueda = [](const Persona* p, const std::string& apellido) {
			return ConvertirAMinusculas(p->getApellidos()).find(
				ConvertirAMinusculas(apellido)) != std::string::npos;
			};
		break;
	case 3: // Fecha de nacimiento
		criterioOrdenamiento = [](const Persona* a, const Persona* b) {
			return a->getFechaNacimiento() < b->getFechaNacimiento();
			};
		criterioBusqueda = [](const Persona* p, const std::string& fecha) {
			return p->getFechaNacimiento() == fecha;
			};
		break;
	}

	// Ordenar y crear árbol
	std::sort(personas.begin(), personas.end(), criterioOrdenamiento);

	auto inicio = std::chrono::high_resolution_clock::now();
	ArbolB<Persona> arbol(3); // Grado 3 para el árbol B
	arbol.construirDesdeVector(personas);
	auto fin = std::chrono::high_resolution_clock::now();
	auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

	// Colores para la visualización
	const int COLOR_NORMAL = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	const int COLOR_RESALTADO = (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Amarillo
	const int COLOR_DELETE = FOREGROUND_RED | FOREGROUND_INTENSITY; // Rojo brillante
	const int COLOR_SUCCESS = FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Verde brillante

	// Función para mostrar el árbol con nodos resaltados
	auto mostrarArbolConResaltado = [&](const std::string& elementoResaltado, int colorResaltado, bool animado = false) {
		limpiarPantallaPreservandoMarquesina(3);
		std::cout << "=== ARBOL B DIDACTICO ===" << std::endl;
		std::cout << "Ordenado por: " << criterios[selCriterio] << std::endl;

		// Verificar si el árbol está vacío
		if (arbol.estaVacio()) {
			std::cout << "\nEl árbol está vacío." << std::endl;
			return;
		}

		// Mostrar árbol con nodos resaltados
		//arbol.mostrarAnimado(elementoResaltado, colorResaltado, animado, selCriterio);

		std::cout << "\nPresione Enter para mostrar de manera grafica ..." << std::endl;
		int teclaCualquiera = _getch();
		(void)teclaCualquiera;

		ArbolBGrafico::mostrarAnimadoSFMLGrado3(&arbol, "", 0);

		std::cout << "\nTiempo de construcción: " << duracion << " milisegundos." << std::endl;
		};

	// Submenú de operaciones con árbol
	std::vector<std::string> opcionesArbol = { "Buscar persona", "Eliminar persona", "Volver" };
	int selOpcion = 0;

	while (true) {
		// Mostrar árbol normal sin resaltados
		mostrarArbolConResaltado("", COLOR_NORMAL, false);

		std::cout << "\nSeleccione operación:" << std::endl;
		for (size_t i = 0; i < opcionesArbol.size(); i++) {
			if (i == selOpcion)
				std::cout << " > " << opcionesArbol[i] << std::endl;
			else
				std::cout << "   " << opcionesArbol[i] << std::endl;
		}

		int tecla = _getch();
		if (tecla == 224) {
			tecla = _getch();
			if (tecla == 72) // Flecha arriba
				selOpcion = static_cast<int>((static_cast<unsigned long long>(selOpcion) - 1 + opcionesArbol.size()) % opcionesArbol.size());
			else if (tecla == 80) // Flecha abajo
				selOpcion = static_cast<int>((static_cast<unsigned long long>(selOpcion) + 1) % opcionesArbol.size());
		}
		else if (tecla == 13) { // Enter
			if (selOpcion == 0) { // Buscar persona
				// Coordenadas para el área de entrada
				int baseY = static_cast<int>(static_cast<unsigned long long>(arbol.altura()) + 8 + opcionesArbol.size());
				gotoxy(0, baseY);
				std::cout << std::string(80, ' '); // Limpiar línea
				gotoxy(0, baseY);

				std::string criterioBusquedaStr;
				mostrarCursor();

				// Instrucciones según criterio seleccionado
				if (selCriterio == 3) {
					std::cout << "Usar el formato DD/MM/AAAA para buscar por fecha: ";
				}
				else {
					std::cout << "Ingrese " << criterios[selCriterio] << " a buscar: ";
				}

				std::cin >> criterioBusquedaStr;
				ocultarCursor();

				// Búsqueda con animación
				auto inicioBusqueda = std::chrono::high_resolution_clock::now();

				// Mostrar primero la búsqueda en curso
				mostrarArbolConResaltado(criterioBusquedaStr, COLOR_RESALTADO, true);

				// Realizar la búsqueda real
				Persona* encontrado = arbol.buscar(criterioBusquedaStr, criterioBusqueda);

				auto finBusqueda = std::chrono::high_resolution_clock::now();
				auto duracionBusqueda = std::chrono::duration_cast<std::chrono::milliseconds>
					(finBusqueda - inicioBusqueda).count();

				// Mostrar resultados
				gotoxy(0, baseY + 1);
				if (encontrado) {
					std::cout << "Persona encontrada:" << std::endl;
					std::cout << "Cédula: " << encontrado->getCedula() << std::endl;
					std::cout << "Nombre: " << encontrado->getNombres() << std::endl;
					std::cout << "Apellidos: " << encontrado->getApellidos() << std::endl;
					std::cout << "Fecha de nacimiento: " << encontrado->getFechaNacimiento() << std::endl;
					std::cout << "Correo: " << encontrado->getCorreo() << std::endl;
				}
				else {
					std::cout << "Persona no encontrada." << std::endl;
				}

				std::cout << "Tiempo de búsqueda: " << duracionBusqueda << " milisegundos." << std::endl;
				std::cout << "\nPresione Enter para continuar...";

				std::cin.clear();
				std::cin.ignore(1000, '\n');
				std::cout << "\nPresione Enter para continuar...";
				int teclaCualquiera = _getch();
				(void)teclaCualquiera; // Ignorar tecla presionada

				std::cin.get();
			}
			else if (selOpcion == 1) { // Eliminar persona
				// Coordenadas para el área de entrada
				int baseY = static_cast<int>(static_cast<unsigned long long>(arbol.altura()) + 8 + opcionesArbol.size());
				gotoxy(0, baseY);
				std::cout << std::string(80, ' '); // Limpiar línea
				gotoxy(0, baseY);

				std::string criterioEliminarStr;
				mostrarCursor();

				if (selCriterio == 3) {
					std::cout << "Usar el formato DD/MM/AAAA para eliminar por fecha: ";
				}
				else {
					std::cout << "Ingrese " << criterios[selCriterio] << " a eliminar: ";
				}

				std::cin >> criterioEliminarStr;
				ocultarCursor();

				// Búsqueda para confirmar
				gotoxy(0, baseY + 1);
				std::cout << "Buscando elemento a eliminar..." << std::endl;

				// Mostrar animación de búsqueda
				mostrarArbolConResaltado(criterioEliminarStr, COLOR_DELETE, true);

				// Realizar la búsqueda real
				Persona* aEliminar = arbol.buscar(criterioEliminarStr, criterioBusqueda);

				gotoxy(0, baseY + 1);
				if (aEliminar) {
					std::cout << "¿Confirma eliminar a " << aEliminar->getNombres() << " "
						<< aEliminar->getApellidos() << "? (S/N): ";
					char confirmar = _getch();
					std::cout << confirmar << std::endl;

					if (confirmar == 'S' || confirmar == 's') {
						// Marcar en rojo para eliminar
						mostrarArbolConResaltado(criterioEliminarStr, COLOR_DELETE, true);

						// Eliminar del árbol
						bool eliminado = arbol.eliminar(criterioEliminarStr, criterioBusqueda);

						// Eliminar de la lista de personas original
						if (eliminado) {
							NodoPersona* anterior = nullptr;
							NodoPersona* actual = cabeza;

							while (actual) {
								if (actual->persona == aEliminar) {
									if (anterior) {
										anterior->siguiente = actual->siguiente;
									}
									else {
										cabeza = actual->siguiente;
									}
									// No eliminar el objeto persona, solo el nodo
									actual->persona = nullptr;
									delete actual;
									break;
								}
								anterior = actual;
								actual = actual->siguiente;
							}

							// Actualizar vector de personas
							personas.erase(std::remove(personas.begin(), personas.end(), aEliminar), personas.end());

							// Mensaje de éxito
							HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
							SetConsoleTextAttribute(hConsole, COLOR_SUCCESS);
							std::cout << "Persona eliminada exitosamente." << std::endl;
							SetConsoleTextAttribute(hConsole, COLOR_NORMAL);
						}
						else {
							std::cout << "Error al eliminar la persona." << std::endl;
						}
					}
					else {
						std::cout << "Eliminación cancelada." << std::endl;
					}
				}
				else {
					std::cout << "No se encontró ninguna persona con ese criterio." << std::endl;
				}

				std::cout << "\nPresione Enter para continuar...";

				std::cin.clear();
				std::cin.ignore(1000, '\n');
				std::cout << "\nPresione Enter para continuar...";
				int teclaCualquiera = _getch();
				(void)teclaCualquiera; // Ignorar tecla presionada

				std::cin.get();
			}
			else {
				return; // Volver
			}
		}
		else if (tecla == 27) { // ESC
			return;
		}
	}
}
/**
 * @brief Genera un código QR para una persona y su cuenta bancaria
 *
 * Esta función toma los datos de una persona y un número de cuenta para generar
 * un código QR que puede ser mostrado en pantalla o guardado como PDF.
 *
 * @param persona Referencia al objeto Persona cuyos datos se incluirán en el QR
 * @param numeroCuenta Número de cuenta a incluir en el QR
 * @return bool true si la generación fue exitosa, false si se canceló o hubo error
 */
bool Utilidades::generarQR(const Persona& persona, const std::string& numeroCuenta) {
	try {
		limpiarPantallaPreservandoMarquesina(1);

		// Crear y generar QR
		CodigoQR::GeneradorQRTextoPlano qr(
			persona.getNombres() + " " + persona.getApellidos(),
			numeroCuenta
		);
		qr.generarQR();

		// Mostrar información y QR
		std::cout << "\n=== CODIGO QR GENERADO EXISTOSAMENTE ===";
		//qr.imprimirEnConsola();

		// Opciones para el QR
		std::vector<std::string> opcionesQRGen = {
			"Generar PDF",
			"Mostrar QR tecnico en pantalla",
			"Volver al menu principal"
		};
		std::cout << "\n=== OPCIONES ===";

		int seleccionQR = Utilidades::menuInteractivo("Seleccione una opcion para el QR", opcionesQRGen, 2, 4);

		if (seleccionQR == -1 || seleccionQR == 2) { // Volver al menu principal
			limpiarPantallaPreservandoMarquesina(1);
			return false; // Cancelado
		}
		switch (seleccionQR) {
		case 0:  // Generar PDF
		{
			// Crear nombre del archivo
			std::string nombreArchivo = "QR_" +
				persona.getNombres() + "_" +
				persona.getApellidos();

			// Eliminar caracteres no válidos para nombre de archivo
			nombreArchivo = Utilidades::EliminarEspacios(nombreArchivo);

			// Construir ruta hacia la carpeta BancoApp en el Escritorio
			char desktopPath[MAX_PATH];
			HRESULT hr = SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, desktopPath);

			std::string rutaCompleta;
			if (SUCCEEDED(hr)) {
				// Construir ruta: Escritorio\BancoApp\nombreArchivo.pdf
				rutaCompleta = std::string(desktopPath) + "\\BancoApp\\" + nombreArchivo + ".pdf";

				// Crear la carpeta BancoApp si no existe
				std::string carpetaBancoApp = std::string(desktopPath) + "\\BancoApp";
				CreateDirectoryA(carpetaBancoApp.c_str(), NULL);
			}
			else {
				// Fallback a carpeta temporal si no se puede obtener el escritorio
				char tempPath[MAX_PATH];
				GetTempPathA(MAX_PATH, tempPath);
				rutaCompleta = std::string(tempPath) + nombreArchivo + ".pdf";
				std::cerr << "No se pudo acceder al Escritorio, usando carpeta temporal." << std::endl;
			}

			try {
				std::cout << "\nGenerando PDF del código QR..." << std::endl;
				std::cout << "Guardando en: " << rutaCompleta << std::endl;

				// Llamar a la función de generación de PDF
				qr.generarPDFQR(qr.qr, rutaCompleta);

				// Abrir el PDF automáticamente
				HINSTANCE resultado = ShellExecuteA(NULL, "open", rutaCompleta.c_str(), NULL, NULL, SW_SHOWNORMAL);

				if (reinterpret_cast<INT_PTR>(resultado) <= 32) {
					std::cerr << "Error al abrir el PDF. Se guardó en: " << rutaCompleta << std::endl;
					Sleep(2000);
				}
			}
			catch (const std::exception& e) {
				std::cerr << "Error generando el PDF: " << e.what() << std::endl;
				Sleep(2000);
			}
			break;
		}
		case 1: // Mostrar QR técnico en pantalla
		{
			// Mostrar QR en consola
			std::cout << "\n=== CÓDIGO QR TÉCNICO ===" << std::endl;
			qr.imprimirEnConsola();
			std::cout << "\nPresione Enter para continuar...";
			std::cin.get();
			break;
		}
		}
	}	
	catch (const std::exception& e) {
		limpiarPantallaPreservandoMarquesina(1);
		std::cout << "Error generando QR: " << e.what() << std::endl;
		system("pause");
		return false;
	}
}

/**
 * @brief Menú interactivo para generar QR desde datos de MongoDB
 *
 * Muestra una tabla interactiva con todas las personas y sus cuentas,
 * permitiendo ordenamiento y búsqueda para generar códigos QR.
 */
void Utilidades::generarQR() {
	// Datos para el manejo de la tabla
	struct PersonaCuenta {
		std::string cedula;
		std::string nombreCompleto;
		std::string numeroCuenta;
		std::string tipoCuenta;
		double saldo;
		int totalCuentas;

		PersonaCuenta(const std::string& ced, const std::string& nombre,
			const std::string& cuenta, const std::string& tipo,
			double sal, int total)
			: cedula(ced), nombreCompleto(nombre), numeroCuenta(cuenta),
			tipoCuenta(tipo), saldo(sal), totalCuentas(total) {
		}
	};

	try {
		//iniciarOperacionCritica();
		limpiarPantallaPreservandoMarquesina(1);

		// Obtener datos desde MongoDB usando ConexionMongo
		auto& clienteDB = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona baseDatos(clienteDB);

		auto personas = baseDatos.mostrarTodasPersonas();

		if (personas.empty()) {
			std::cout << "\n[INFORMACIÓN] No hay personas registradas en la base de datos.\n";
			std::cout << "Para generar códigos QR, primero debe registrar personas con cuentas.\n";
			system("pause");
			return;
		}

		// Procesar datos para la tabla
		std::vector<PersonaCuenta> datosTabla;
		datosTabla.reserve(personas.size() * 5); // Estimación de cuentas promedio

		std::for_each(personas.begin(), personas.end(), [&datosTabla](const auto& doc) {
			auto view = doc.view();

			std::string cedula = std::string(view["cedula"].get_string().value);
			std::string nombreCompleto = std::string(view["nombre"].get_string().value) +
				" " + std::string(view["apellido"].get_string().value);
			int totalCuentas = view.find("totalCuentasExistentes") != view.end() ?
				view["totalCuentasExistentes"].get_int32().value : 0;

			// Procesar cuentas usando for_each
			if (view.find("cuentas") != view.end() &&
				view["cuentas"].type() == bsoncxx::type::k_array) {

				auto cuentasArray = view["cuentas"].get_array().value;
				std::for_each(cuentasArray.begin(), cuentasArray.end(),
					[&datosTabla, &cedula, &nombreCompleto, totalCuentas](const auto& cuentaElement) {
						if (cuentaElement.type() == bsoncxx::type::k_document) {
							auto cuenta = cuentaElement.get_document().value;

							std::string numeroCuenta = std::string(cuenta["numeroCuenta"].get_string().value);
							std::string tipoCuenta = std::string(cuenta["tipo"].get_string().value);
							double saldo = cuenta["saldo"].get_double().value;

							datosTabla.emplace_back(cedula, nombreCompleto, numeroCuenta,
								tipoCuenta, saldo, totalCuentas);
						}
					});
			}
			});

		if (datosTabla.empty()) {
			std::cout << "\n[INFORMACIÓN] No hay cuentas registradas para generar códigos QR.\n";
			system("pause");
			//finalizarOperacionCritica();
			return;
		}

		// Variables de control de la tabla
		enum class ColumnaOrden { NOMBRE, CUENTA, BUSCAR };
		ColumnaOrden columnaActual = ColumnaOrden::NOMBRE;
		bool ordenAscendente = true;
		int filaSeleccionada = 0;
		std::string textoBusqueda = "";
		std::vector<int> indicesFiltrados;

		// Funciones auxiliares usando principios SOLID
		auto aplicarFiltro = [&datosTabla, &indicesFiltrados](const std::string& busqueda) {
			indicesFiltrados.clear();
			if (busqueda.empty()) {
				indicesFiltrados.resize(datosTabla.size());
				std::iota(indicesFiltrados.begin(), indicesFiltrados.end(), 0);
			}
			else {
				std::string busquedaLower = ConvertirAMinusculas(busqueda);
				for (size_t i = 0; i < datosTabla.size(); ++i) {
					std::string nombreLower = ConvertirAMinusculas(datosTabla[i].nombreCompleto);
					if (nombreLower.find(busquedaLower) != std::string::npos) {
						indicesFiltrados.push_back(static_cast<int>(i));
					}
				}
			}
			};

		auto ordenarDatos = [&datosTabla, &indicesFiltrados](ColumnaOrden columna, bool ascendente) {
			std::function<bool(int, int)> comparador;

			switch (columna) {
			case ColumnaOrden::NOMBRE:
				comparador = [&datosTabla, ascendente](int a, int b) {
					auto resultado = datosTabla[a].nombreCompleto.compare(datosTabla[b].nombreCompleto);
					return ascendente ? resultado < 0 : resultado > 0;
					};
				break;
			case ColumnaOrden::CUENTA:
				comparador = [&datosTabla, ascendente](int a, int b) {
					auto resultado = datosTabla[a].numeroCuenta.compare(datosTabla[b].numeroCuenta);
					return ascendente ? resultado < 0 : resultado > 0;
					};
				break;
			default:
				return;
			}

			std::sort(indicesFiltrados.begin(), indicesFiltrados.end(), comparador);
			};

		auto mostrarTabla = [&]() {
			limpiarPantallaPreservandoMarquesina(1);

			std::cout << "=== GENERADOR DE CÓDIGOS QR ===\n\n";
			std::cout << "Total de cuentas: " << indicesFiltrados.size() << "\n";
			if (!textoBusqueda.empty()) {
				std::cout << "Filtro activo: \"" << textoBusqueda << "\"\n";
			}
			std::cout << "\n";

			// Encabezados con indicadores de ordenamiento
			std::string headerNombre = "Nombre-Apellido";
			std::string headerCuenta = "N. Cuenta";
			std::string headerBuscar = "Buscar específico";

			if (columnaActual == ColumnaOrden::NOMBRE) {
				headerNombre += (ordenAscendente ? " ↑" : " ↓");
			}
			else if (columnaActual == ColumnaOrden::CUENTA) {
				headerCuenta += (ordenAscendente ? " ↑" : " ↓");
			}

			std::cout << std::setw(35) << std::left << headerNombre
				<< std::setw(20) << std::left << headerCuenta
				<< std::setw(15) << std::left << "Tipo"
				<< std::setw(15) << std::left << "Saldo"
				<< std::setw(20) << std::left << headerBuscar << "\n";
			std::cout << std::string(105, '-') << "\n";

			// Mostrar datos paginados (máximo 15 filas por página)
			const int filasPorPagina = 15;
			int inicioFila = (filaSeleccionada / filasPorPagina) * filasPorPagina;
			int finFila = std::min(inicioFila + filasPorPagina, static_cast<int>(indicesFiltrados.size()));

			for (int i = inicioFila; i < finFila; ++i) {
				const auto& item = datosTabla[indicesFiltrados[i]];

				if (i == filaSeleccionada) {
					std::cout << "> ";
				}
				else {
					std::cout << "  ";
				}

				std::cout << std::setw(33) << std::left << item.nombreCompleto
					<< std::setw(18) << std::left << item.numeroCuenta
					<< std::setw(13) << std::left << item.tipoCuenta
					<< "$" << std::setw(12) << std::right << std::fixed << std::setprecision(2) << item.saldo
					<< "\n";
			}

			std::cout << "\n=== CONTROLES ===\n";
			std::cout << "↑↓: Navegar | ←→: Cambiar columna | ENTER: Generar QR | F: Buscar | ESC: Salir\n";
			std::cout << "Columna actual: ";
			switch (columnaActual) {
			case ColumnaOrden::NOMBRE: std::cout << "Nombre-Apellido"; break;
			case ColumnaOrden::CUENTA: std::cout << "N. Cuenta"; break;
			case ColumnaOrden::BUSCAR: std::cout << "Búsqueda"; break;
			}
			std::cout << " (" << (ordenAscendente ? "ASC" : "DESC") << ")\n";
			};

		// Inicializar filtro y ordenamiento
		aplicarFiltro("");
		ordenarDatos(columnaActual, ordenAscendente);

		// Bucle principal de interacción
		while (true) {
			mostrarTabla();

			int tecla = _getch();

			if (tecla == 224) { // Teclas especiales
				tecla = _getch();
				switch (tecla) {
				case 72: // Flecha arriba
					filaSeleccionada = std::max(0, filaSeleccionada - 1);
					break;
				case 80: // Flecha abajo
					filaSeleccionada = std::min(static_cast<int>(indicesFiltrados.size()) - 1,
						filaSeleccionada + 1);
					break;
				case 75: // Flecha izquierda
					columnaActual = static_cast<ColumnaOrden>((static_cast<int>(columnaActual) - 1 + 3) % 3);
					if (columnaActual != ColumnaOrden::BUSCAR) {
						ordenarDatos(columnaActual, ordenAscendente);
					}
					break;
				case 77: // Flecha derecha
					columnaActual = static_cast<ColumnaOrden>((static_cast<int>(columnaActual) + 1) % 3);
					if (columnaActual != ColumnaOrden::BUSCAR) {
						ordenarDatos(columnaActual, ordenAscendente);
					}
					break;
				}
			}
			else if (tecla == 13) { // ENTER - Generar QR
				if (!indicesFiltrados.empty() && filaSeleccionada < indicesFiltrados.size()) {
					const auto& seleccionado = datosTabla[indicesFiltrados[filaSeleccionada]];

					// Crear objeto Persona temporal para generar QR
					Persona personaTemp;
					size_t espacioPos = seleccionado.nombreCompleto.find(' ');
					if (espacioPos != std::string::npos) {
						personaTemp.setNombres(seleccionado.nombreCompleto.substr(0, espacioPos));
						personaTemp.setApellidos(seleccionado.nombreCompleto.substr(espacioPos + 1));
					}
					else {
						personaTemp.setNombres(seleccionado.nombreCompleto);
						personaTemp.setApellidos("");
					}
					personaTemp.setCedula(seleccionado.cedula);

					// Generar QR usando la función existente
					bool qrGenerado = generarQR(personaTemp, seleccionado.numeroCuenta);

					if (qrGenerado) {
						std::cout << "\nCódigo QR generado exitosamente para "
							<< seleccionado.nombreCompleto << "\n";
					}

					system("pause");
				}
				break;
			}
			else if (tecla == 'f' || tecla == 'F') { // Búsqueda
				std::cout << "\nIngrese texto a buscar: ";
				mostrarCursor();
				std::cin >> textoBusqueda;
				ocultarCursor();

				aplicarFiltro(textoBusqueda);
				filaSeleccionada = 0;

				if (columnaActual != ColumnaOrden::BUSCAR) {
					ordenarDatos(columnaActual, ordenAscendente);
				}
			}
			else if (tecla == ' ') { // Espacio - Cambiar orden ASC/DESC
				if (columnaActual != ColumnaOrden::BUSCAR) {
					ordenAscendente = !ordenAscendente;
					ordenarDatos(columnaActual, ordenAscendente);
				}
			}
			else if (tecla == 27) { // ESC - Salir
				break;
			}
		}

	}
	catch (const std::exception& e) {
		std::cerr << "\nError en generación de QR: " << e.what() << std::endl;
		system("pause");
	}

	//finalizarOperacionCritica();
	limpiarPantallaPreservandoMarquesina(1);
}

/**
 * @brief Genera código QR para una persona y retorna la representación en string
 *
 * Esta función crea un código QR con los datos de la persona y cuenta,
 * pero en lugar de mostrarlo, retorna una representación que puede ser
 * utilizada para integrar en otros documentos como HTML o PDF.
 *
 * @param persona Referencia al objeto Persona
 * @param numeroCuenta Número de cuenta para incluir en el QR
 * @return String con la representación del código QR, vacío si hay error
 */
std::string Utilidades::generarQRSoloMostrar(const Persona& persona, const std::string& numeroCuenta) {
	try {
		// Crear datos para el QR igual que en la función original
		std::string datosQR = "BANCO_CUENTA\n";
		datosQR += "CEDULA:" + persona.getCedula() + "\n";
		datosQR += "NOMBRE:" + persona.getNombres() + " " + persona.getApellidos() + "\n";
		datosQR += "CUENTA:" + numeroCuenta + "\n";
		datosQR += "FECHA:" + Fecha().obtenerFechaFormateada();

		// Generar el código QR usando la biblioteca existente
		CodigoQR::QrCode qr = CodigoQR::QrCode::encodeText(datosQR.c_str(), CodigoQR::QrCode::Ecc::MEDIUM);

		// Convertir a representación de texto usando caracteres ASCII
		std::ostringstream resultado;
		int size = qr.getSize();

		// Usar caracteres diferentes para mejor visualización en HTML
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				resultado << (qr.getModule(x, y) ? "██" : "  ");
			}
			resultado << "\n";
		}

		return resultado.str();
	}
	catch (const std::exception& e) {
		std::cerr << "Error generando QR: " << e.what() << std::endl;
		return "";
	}
}

/**
 * @brief Formatea un valor monetario con formato americano ($1,000.23)
 *
 * @param valor Valor a formatear
 * @return std::string Valor formateado con separador de miles y dos decimales
 */
static std::string formatearValorMonetario(double valor) {
	std::stringstream ss;
	ss.imbue(std::locale("en_US.UTF-8")); // Usar locale americano (comas para miles, punto para decimales)
	ss << std::fixed << std::setprecision(2) << valor;
	return ss.str();
}

/**
 * @brief Formatea un valor en centavos a formato monetario
 *
 * @param valorEnCentavos Valor en centavos a formatear
 * @return std::string Valor formateado como moneda
 */
static std::string formatearCentavosAMonetario(int valorEnCentavos) {
	return formatearValorMonetario(valorEnCentavos / 100.0);
}

/**
 * @brief Presenta un menú para ordenar elementos con diferentes criterios
 *
 * @tparam T Tipo de datos a ordenar
 * @param vec Vector de elementos a ordenar
 * @param opciones Vector de nombres de criterios de ordenamiento
 * @param criterios Vector de funciones para comparar elementos según cada criterio
 * @param mostrarDatos Función para mostrar los datos ordenados
 */
template<typename T>
void mostrarMenuOrdenar(std::vector<T*>& vec, const std::vector<std::string>& opciones, const std::vector<std::function<bool(const T*, const T*)>>& criterios, std::function<void(const std::vector<T*>&)> mostrarDatos)
{
	int seleccion = 0;
	while (true) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "Ordenar por:\n";
		for (size_t i = 0; i < opciones.size(); ++i) {
			if (i == seleccion)
				std::cout << " > " << opciones[i] << "\n";
			else
				std::cout << "   " << opciones[i] << "\n";
		}
		std::cout << "\nESC para salir\n";

		int tecla = _getch();
		if (tecla == 224) {
			tecla = _getch();
			if (tecla == 72) seleccion = (static_cast<unsigned long long>(seleccion) - 1 + opciones.size()) % opciones.size();
			else if (tecla == 80) seleccion = (static_cast<unsigned long long>(seleccion) + 1) % opciones.size();
		}
		else if (tecla == 13) {
			Utilidades::burbuja<T>(vec, criterios[seleccion]);
			mostrarDatos(vec);
			system("pause");
		}
		else if (tecla == 27) {
			break;
		}
	}
}

/**
 * @brief Oculta el cursor de la consola
 */
void Utilidades::ocultarCursor() {
	CONSOLE_CURSOR_INFO cursorInfo{};
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

/**
 * @brief Muestra el cursor de la consola
 */
void Utilidades::mostrarCursor() {
	CONSOLE_CURSOR_INFO cursorInfo{};
	cursorInfo.dwSize = 25; // Tamaño normal
	cursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

/**
 * @brief Centra la ventana de la consola en la pantalla
 *
 * Utiliza las funciones de Windows API para mover la ventana de la consola
 * al centro de la pantalla principal.
 */
void Utilidades::centrarVentanaConsola() {
	HWND hConsole = GetConsoleWindow(); // Obtiene el handle de la ventana de la consola
	RECT r;
	GetWindowRect(hConsole, &r); // Obtiene las coordenadas actuales de la ventana

	// Obtiene las dimensiones de la pantalla principal
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Calcula las nuevas coordenadas para centrar la ventana
	// Ancho de la ventana
	int windowWidth = r.right - r.left;
	// Alto de la ventana
	int windowHeight = r.bottom - r.top;

	int newX = (screenWidth - windowWidth) / 2;
	int newY = (screenHeight - windowHeight) / 2;

	// Mueve la ventana a la nueva posición.
	// Los últimos dos parámetros (width, height) son el tamaño actual de la ventana,
	// el penúltimo es si debe redibujar la ventana (TRUE).
	MoveWindow(hConsole, newX, newY, windowWidth, windowHeight, TRUE);
}

/**
 * @brief Restaura la barra de título de la consola
 *
 * Esta función añade los estilos necesarios para que la consola tenga una barra de título,
 * un menú del sistema y un marco que permita redimensionarla manualmente.
 */
void Utilidades::restaurarBarraTituloConsola() {
	HWND hConsole = GetConsoleWindow();
	LONG style = GetWindowLong(hConsole, GWL_STYLE);

	// Añade los estilos para la barra de título, menú del sistema (para minimizar/maximizar/cerrar)
	// y el marco de la ventana (para poder redimensionarla manualmente).
	style |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;

	SetWindowLong(hConsole, GWL_STYLE, style);

	// Necesario para que los cambios de estilo surtan efecto inmediatamente
	SetWindowPos(hConsole, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

/**
 * @brief Explorador interactivo de archivos de personas desde MongoDB
 *
 * Muestra una tabla interactiva con todas las personas y sus cuentas,
 * permitiendo ordenamiento por nombre, apellido y número de cuenta,
 * además de funciones de búsqueda avanzada.
 */
void Utilidades::exploradorArchivosInteractivo(Banco& banco) {
	// Estructura para manejar datos de la tabla
	struct PersonaArchivo {
		std::string cedula;
		std::string nombres;
		std::string apellidos;
		std::string nombreCompleto;
		std::string correo;
		std::string direccion;
		std::string fechaNacimiento;
		std::string numeroCuenta;
		std::string tipoCuenta;
		double saldo;
		int totalCuentas;

		PersonaArchivo(const std::string& ced, const std::string& nom, const std::string& ape,
			const std::string& email, const std::string& dir, const std::string& fecha,
			const std::string& cuenta, const std::string& tipo, double sal, int total)
			: cedula(ced), nombres(nom), apellidos(ape), correo(email), direccion(dir),
			fechaNacimiento(fecha), numeroCuenta(cuenta), tipoCuenta(tipo),
			saldo(sal), totalCuentas(total) {
			nombreCompleto = nombres + " " + apellidos;
		}
	};

	try {
		//iniciarOperacionCritica();
		limpiarPantallaPreservandoMarquesina(1);

		// Obtener datos desde MongoDB
		auto& clienteDB = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona baseDatos(clienteDB);

		auto personas = baseDatos.mostrarTodasPersonas();

		if (personas.empty()) {
			std::cout << "\n[INFORMACIÓN] No hay personas registradas en la base de datos.\n";
			std::cout << "Para explorar archivos, primero debe registrar personas.\n";
			system("pause");
			//finalizarOperacionCritica();
			return;
		}

		// Procesar datos para la tabla
		std::vector<PersonaArchivo> datosTabla;
		datosTabla.reserve(personas.size() * 5); // Estimación de cuentas promedio

		// Usar for_each para procesar personas aplicando principios SOLID
		std::for_each(personas.begin(), personas.end(), [&datosTabla](const auto& doc) {
			auto view = doc.view();

			std::string cedula = std::string(view["cedula"].get_string().value);
			std::string nombres = std::string(view["nombre"].get_string().value);
			std::string apellidos = std::string(view["apellido"].get_string().value);
			std::string correo = view.find("correo") != view.end() ?
				std::string(view["correo"].get_string().value) : "No especificado";
			std::string direccion = view.find("direccion") != view.end() ?
				std::string(view["direccion"].get_string().value) : "No especificada";
			std::string fechaNacimiento = view.find("fechaNacimiento") != view.end() ?
				std::string(view["fechaNacimiento"].get_string().value) : "No especificada";

			int totalCuentas = view.find("totalCuentasExistentes") != view.end() ?
				view["totalCuentasExistentes"].get_int32().value : 0;

			// Si la persona no tiene cuentas, agregar registro sin cuenta
			if (totalCuentas == 0) {
				datosTabla.emplace_back(cedula, nombres, apellidos, correo, direccion,
					fechaNacimiento, "Sin cuentas", "N/A", 0.0, 0);
			}
			else {
				// Procesar cuentas usando for_each
				if (view.find("cuentas") != view.end() &&
					view["cuentas"].type() == bsoncxx::type::k_array) {

					auto cuentasArray = view["cuentas"].get_array().value;
					std::for_each(cuentasArray.begin(), cuentasArray.end(),
						[&](const auto& cuentaElement) {
							if (cuentaElement.type() == bsoncxx::type::k_document) {
								auto cuenta = cuentaElement.get_document().value;

								std::string numeroCuenta = std::string(cuenta["numeroCuenta"].get_string().value);
								std::string tipoCuenta = std::string(cuenta["tipo"].get_string().value);
								double saldo = cuenta["saldo"].get_double().value;

								datosTabla.emplace_back(cedula, nombres, apellidos, correo, direccion,
									fechaNacimiento, numeroCuenta, tipoCuenta,
									saldo, totalCuentas);
							}
						});
				}
			}
			});

		if (datosTabla.empty()) {
			std::cout << "\n[INFORMACIÓN] No hay datos para explorar.\n";
			system("pause");
			//finalizarOperacionCritica();
			return;
		}

		// Variables de control de la tabla aplicando principios SOLID
		enum class ColumnaOrden { NOMBRE, APELLIDO, NUMERO_CUENTA, BUSCAR };
		ColumnaOrden columnaActual = ColumnaOrden::NOMBRE;
		bool ordenAscendente = true;
		int filaSeleccionada = 0;
		std::string textoBusqueda = "";
		std::vector<int> indicesFiltrados;

		// Función lambda para aplicar filtros (SRP: responsabilidad única de filtrado)
		auto aplicarFiltro = [&datosTabla, &indicesFiltrados](const std::string& busqueda) {
			indicesFiltrados.clear();
			if (busqueda.empty()) {
				indicesFiltrados.resize(datosTabla.size());
				std::iota(indicesFiltrados.begin(), indicesFiltrados.end(), 0);
			}
			else {
				std::string busquedaLower = ConvertirAMinusculas(busqueda);

				// Buscar en múltiples campos
				for (size_t i = 0; i < datosTabla.size(); ++i) {
					const auto& item = datosTabla[i];

					std::string textoCompleto = ConvertirAMinusculas(
						item.nombreCompleto + " " + item.cedula + " " +
						item.numeroCuenta + " " + item.correo + " " + item.direccion
					);

					if (textoCompleto.find(busquedaLower) != std::string::npos) {
						indicesFiltrados.push_back(static_cast<int>(i));
					}
				}
			}
			};

		// Función lambda para ordenar datos (SRP: responsabilidad única de ordenamiento)
		auto ordenarDatos = [&datosTabla, &indicesFiltrados](ColumnaOrden columna, bool ascendente) {
			std::function<bool(int, int)> comparador;

			switch (columna) {
			case ColumnaOrden::NOMBRE:
				comparador = [&datosTabla, ascendente](int a, int b) {
					auto resultado = datosTabla[a].nombres.compare(datosTabla[b].nombres);
					return ascendente ? resultado < 0 : resultado > 0;
					};
				break;
			case ColumnaOrden::APELLIDO:
				comparador = [&datosTabla, ascendente](int a, int b) {
					auto resultado = datosTabla[a].apellidos.compare(datosTabla[b].apellidos);
					return ascendente ? resultado < 0 : resultado > 0;
					};
				break;
			case ColumnaOrden::NUMERO_CUENTA:
				comparador = [&datosTabla, ascendente](int a, int b) {
					auto resultado = datosTabla[a].numeroCuenta.compare(datosTabla[b].numeroCuenta);
					return ascendente ? resultado < 0 : resultado > 0;
					};
				break;
			default:
				return;
			}

			std::sort(indicesFiltrados.begin(), indicesFiltrados.end(), comparador);
			};

		// Función lambda para mostrar la tabla (SRP: responsabilidad única de visualización)
		auto mostrarTabla = [&]() {
			limpiarPantallaPreservandoMarquesina(1);

			std::cout << "=== EXPLORADOR DE ARCHIVOS DE PERSONAS ===\n\n";
			std::cout << "Total de registros: " << indicesFiltrados.size() << "\n";
			if (!textoBusqueda.empty()) {
				std::cout << "Filtro activo: \"" << textoBusqueda << "\"\n";
			}
			std::cout << "\n";

			// Encabezados con indicadores de ordenamiento
			std::string headerNombre = "Nombre";
			std::string headerApellido = "Apellido";
			std::string headerNumeroCuenta = "N. Cuenta";
			std::string headerBuscar = "Buscar específico";

			if (columnaActual == ColumnaOrden::NOMBRE) {
				headerNombre += (ordenAscendente ? " ↑" : " ↓");
			}
			else if (columnaActual == ColumnaOrden::APELLIDO) {
				headerApellido += (ordenAscendente ? " ↑" : " ↓");
			}
			else if (columnaActual == ColumnaOrden::NUMERO_CUENTA) {
				headerNumeroCuenta += (ordenAscendente ? " ↑" : " ↓");
			}

			std::cout << std::setw(4) << std::left << "Sel"
				<< std::setw(20) << std::left << headerNombre
				<< std::setw(20) << std::left << headerApellido
				<< std::setw(15) << std::left << "Cédula"
				<< std::setw(18) << std::left << headerNumeroCuenta
				<< std::setw(12) << std::left << "Tipo"
				<< std::setw(15) << std::left << "Saldo"
				<< std::setw(20) << std::left << headerBuscar << "\n";
			std::cout << std::string(124, '-') << "\n";

			// Mostrar datos paginados (máximo 12 filas por página)
			const int filasPorPagina = 12;
			int inicioFila = (filaSeleccionada / filasPorPagina) * filasPorPagina;
			int finFila = std::min(inicioFila + filasPorPagina, static_cast<int>(indicesFiltrados.size()));

			for (int i = inicioFila; i < finFila; ++i) {
				const auto& item = datosTabla[indicesFiltrados[i]];

				std::cout << std::setw(4) << std::left << (i == filaSeleccionada ? ">>> " : "   ")
					<< std::setw(20) << std::left << item.nombres.substr(0, 19)
					<< std::setw(20) << std::left << item.apellidos.substr(0, 19)
					<< std::setw(15) << std::left << item.cedula
					<< std::setw(18) << std::left << item.numeroCuenta.substr(0, 17)
					<< std::setw(12) << std::left << item.tipoCuenta.substr(0, 11)
					<< "$" << std::setw(12) << std::right << std::fixed << std::setprecision(2) << item.saldo
					<< "\n";
			}

			std::cout << "\n=== CONTROLES ===\n";
			std::cout << "↑↓: Navegar | ←→: Cambiar columna | ENTER: Ver detalles | F: Buscar | ESPACIO: Orden ASC/DESC | ESC: Salir\n";
			std::cout << "Columna actual: ";
			switch (columnaActual) {
			case ColumnaOrden::NOMBRE: std::cout << "Nombre"; break;
			case ColumnaOrden::APELLIDO: std::cout << "Apellido"; break;
			case ColumnaOrden::NUMERO_CUENTA: std::cout << "N. Cuenta"; break;
			case ColumnaOrden::BUSCAR: std::cout << "Búsqueda"; break;
			}
			std::cout << " (" << (ordenAscendente ? "ASC" : "DESC") << ")\n";
			};

		// Función lambda para mostrar detalles (SRP: responsabilidad única de mostrar detalles)
		auto mostrarDetallesPersona = [&](const PersonaArchivo& persona) {
			limpiarPantallaPreservandoMarquesina(1);

			std::cout << "=== DETALLES DE LA PERSONA ===\n\n";
			std::cout << "Cédula: " << persona.cedula << "\n";
			std::cout << "Nombres: " << persona.nombres << "\n";
			std::cout << "Apellidos: " << persona.apellidos << "\n";
			std::cout << "Fecha de Nacimiento: " << persona.fechaNacimiento << "\n";
			std::cout << "Correo: " << persona.correo << "\n";
			std::cout << "Dirección: " << persona.direccion << "\n";
			std::cout << "Total de Cuentas: " << persona.totalCuentas << "\n\n";

			if (persona.numeroCuenta != "Sin cuentas") {
				std::cout << "=== INFORMACIÓN DE CUENTA SELECCIONADA ===\n";
				std::cout << "Número de Cuenta: " << persona.numeroCuenta << "\n";
				std::cout << "Tipo de Cuenta: " << persona.tipoCuenta << "\n";
				std::cout << "Saldo: $" << std::fixed << std::setprecision(2) << persona.saldo << "\n\n";
			}

			// Obtener todas las cuentas de la persona desde la base de datos
			auto personaCompleta = baseDatos.buscarPersonaCompletaPorCedula(persona.cedula);
			if (!personaCompleta.view().empty()) {
				auto view = personaCompleta.view();

				if (view.find("cuentas") != view.end() &&
					view["cuentas"].type() == bsoncxx::type::k_array) {

					auto cuentasArray = view["cuentas"].get_array().value;
					if (std::distance(cuentasArray.begin(), cuentasArray.end()) > 0) {
						std::cout << "=== TODAS LAS CUENTAS ===\n";
						std::cout << std::setw(18) << std::left << "Número"
							<< std::setw(12) << std::left << "Tipo"
							<< std::setw(15) << std::left << "Saldo"
							<< std::setw(15) << std::left << "F. Apertura" << "\n";
						std::cout << std::string(60, '-') << "\n";

						std::for_each(cuentasArray.begin(), cuentasArray.end(),
							[](const auto& cuentaElement) {
								if (cuentaElement.type() == bsoncxx::type::k_document) {
									auto cuenta = cuentaElement.get_document().value;

									std::string numCuenta = std::string(cuenta["numeroCuenta"].get_string().value);
									std::string tipo = std::string(cuenta["tipo"].get_string().value);
									double saldo = cuenta["saldo"].get_double().value;
									std::string fecha = cuenta.find("fechaApertura") != cuenta.end() ?
										std::string(cuenta["fechaApertura"].get_string().value) : "N/A";

									std::cout << std::setw(18) << std::left << numCuenta
										<< std::setw(12) << std::left << tipo
										<< "$" << std::setw(12) << std::right << std::fixed << std::setprecision(2) << saldo
										<< std::setw(15) << std::left << fecha << "\n";
								}
							});
					}
				}
			}

			std::cout << "\nPresione cualquier tecla para regresar...";
			int teclaCualquiera = _getch();
			(void)teclaCualquiera;
			};

		// Inicializar filtro y ordenamiento
		aplicarFiltro("");
		ordenarDatos(columnaActual, ordenAscendente);

		// Bucle principal de interacción
		while (true) {
			mostrarTabla();

			int tecla = _getch();

			if (tecla == 224) { // Teclas especiales
				tecla = _getch();
				switch (tecla) {
				case 72: // Flecha arriba
					filaSeleccionada = std::max(0, filaSeleccionada - 1);
					break;
				case 80: // Flecha abajo
					filaSeleccionada = std::min(static_cast<int>(indicesFiltrados.size()) - 1,
						filaSeleccionada + 1);
					break;
				case 75: // Flecha izquierda
					columnaActual = static_cast<ColumnaOrden>((static_cast<int>(columnaActual) - 1 + 4) % 4);
					if (columnaActual != ColumnaOrden::BUSCAR) {
						ordenarDatos(columnaActual, ordenAscendente);
					}
					break;
				case 77: // Flecha derecha
					columnaActual = static_cast<ColumnaOrden>((static_cast<int>(columnaActual) + 1) % 4);
					if (columnaActual != ColumnaOrden::BUSCAR) {
						ordenarDatos(columnaActual, ordenAscendente);
					}
					break;
				}
			}
			else if (tecla == 13) { // ENTER - Ver detalles
				if (!indicesFiltrados.empty() && filaSeleccionada < indicesFiltrados.size()) {
					const auto& seleccionado = datosTabla[indicesFiltrados[filaSeleccionada]];
					mostrarDetallesPersona(seleccionado);
				}
			}
			else if (tecla == 'f' || tecla == 'F') { // Búsqueda
				std::cout << "\nIngrese texto a buscar (nombre, apellido, cédula, cuenta, correo): ";
				mostrarCursor();
				std::cin >> textoBusqueda;
				ocultarCursor();

				aplicarFiltro(textoBusqueda);
				filaSeleccionada = 0;

				if (columnaActual != ColumnaOrden::BUSCAR) {
					ordenarDatos(columnaActual, ordenAscendente);
				}
			}
			else if (tecla == ' ') { // Espacio - Cambiar orden ASC/DESC
				if (columnaActual != ColumnaOrden::BUSCAR) {
					ordenAscendente = !ordenAscendente;
					ordenarDatos(columnaActual, ordenAscendente);
				}
			}
			else if (tecla == 27) { // ESC - Salir
				break;
			}
		}

	}
	catch (const std::exception& e) {
		std::cerr << "\nError en explorador de archivos: " << e.what() << std::endl;
		system("pause");
	}
	//finalizarOperacionCritica();
	limpiarPantallaPreservandoMarquesina(1);
}

/**
 * @brief Función principal de gestión de hash interactiva
 *
 * Implementa el patrón Facade y utiliza los principios SOLID
 */
void Utilidades::gestionHashInteractiva() {
	try {
		// Crear conexión MongoDB (DIP - Dependency Injection)
		ConexionMongo conexion;

		// Crear gestor hash con factory method
		auto gestorHash = GestorHashBaseDatos::crear(conexion);

		// Crear interfaz con inyección de dependencias
		InterfazGestionHash interfaz(std::move(gestorHash));

		// Mostrar interfaz principal
		interfaz.mostrarMenuPrincipal();

	}
	catch (const std::exception& e) {
		std::cerr << "Error en gestión de hash: " << e.what() << std::endl;
		std::cout << "Presione Enter para continuar...";
		std::cin.get();
	}
}

void Utilidades::mensajeAnimado(const std::string& mensaje)
{
	std::cout << mensaje;

	// Animación de puntos para mayor realismo
	for (int i = 0; i < 3; ++i) {
		std::this_thread::sleep_for(std::chrono::milliseconds(333));  // Pausa de 333 ms 
		std::cout << ".";
		std::cout.flush();
	}

	std::cout << "\n\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Pausa adicional
}
