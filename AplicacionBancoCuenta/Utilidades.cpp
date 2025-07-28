/**
 * @file Utilidades.cpp
 * @brief Implementación de diversas utilidades y estructuras de datos para el sistema bancario
 *
 * Este archivo contiene la implementación de clases de utilidad para el sistema bancario,
 * incluyendo una implementación didáctica de un Árbol B, funciones para manipulación de
 * texto, control de interfaz de usuario y generación de códigos QR.
 */

#define NOMINMAX
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
#include "Marquesina.h"
#include "Utilidades.h"
#include "ArbolBGrafico.h"
#include "ArbolB.h"
#include "NodoPersona.h"
#include "Persona.h"
#include <mutex>
#include <thread>
#include <vector>

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
	// Sleep(2); // Considera si realmente lo necesitas
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
	int numOpciones = opciones.size();

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
		std::cerr << "Error al abrir el menu de ayuda. Codigo: " << (int)resultado << std::endl;
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
				selCriterio = (static_cast<unsigned long long>(selCriterio) - 1 + criterios.size()) % criterios.size();
			else if (tecla == 80) // Flecha abajo
				selCriterio = (static_cast<unsigned long long>(selCriterio) + 1) % criterios.size();

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
				selOpcion = (static_cast<unsigned long long>(selOpcion) - 1 + opcionesArbol.size()) % opcionesArbol.size();
			else if (tecla == 80) // Flecha abajo
				selOpcion = (static_cast<unsigned long long>(selOpcion) + 1) % opcionesArbol.size();
		}
		else if (tecla == 13) { // Enter
			if (selOpcion == 0) { // Buscar persona
				// Coordenadas para el área de entrada
				int baseY = static_cast<unsigned long long>(arbol.altura()) + 8 + opcionesArbol.size();
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
				int baseY = static_cast<unsigned long long>(arbol.altura()) + 8 + opcionesArbol.size();
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
		// Pausar la marquesina y limpiar la pantalla
		if (marquesinaGlobal) {
			marquesinaGlobal->marcarOperacionCritica();
		}
		limpiarPantallaPreservandoMarquesina(1);

		// Crear y generar QR
		CodigoQR::GeneradorQRTextoPlano qr(
			persona.getNombres() + " " + persona.getApellidos(),
			numeroCuenta
		);
		qr.generarQR();

		// Mostrar información y QR
		std::cout << "\n=== CODIGO QR GENERADO ===\n\n";
		qr.imprimirEnConsola();

		// Opciones para el QR
		std::string opcionesQRGen[] = {
			"Generar PDF",
			"Volver al menu principal"
		};
		int numOpcionesQRGen = sizeof(opcionesQRGen) / sizeof(opcionesQRGen[0]);
		int seleccionQRGen = 0;

		// Mostrar título de opciones
		std::cout << "\n=== OPCIONES ===\n\n";

		// Posición inicial del menú - se mantiene fija para redibujarlo
		int menuY = 0;
		bool opcionesDibujadas = false;

		// Bucle de navegación del menú
		while (true) {
			// Dibujar opciones solo la primera vez
			if (!opcionesDibujadas) {
				// Guardar la posición Y de la primera opción
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
				menuY = csbi.dwCursorPosition.Y - 2; // -2 por offset de marquesina

				// Dibujar todas las opciones
				for (int i = 0; i < numOpcionesQRGen; i++) {
					if (i == seleccionQRGen)
						std::cout << " > " << opcionesQRGen[i] << std::endl;
					else
						std::cout << "   " << opcionesQRGen[i] << std::endl;
				}

				opcionesDibujadas = true; // Marcar que ya se dibujaron las opciones
			}

			int teclaQRGen = _getch();
			if (teclaQRGen == 224) {
				teclaQRGen = _getch();
				if (teclaQRGen == 72 || teclaQRGen == 80) { // Arriba o Abajo
					// Guardar la selección anterior
					int seleccionAnterior = seleccionQRGen;

					// Actualizar selección según la tecla presionada
					if (teclaQRGen == 72) // Arriba
						seleccionQRGen = (seleccionQRGen - 1 + numOpcionesQRGen) % numOpcionesQRGen;
					else // Abajo (80)
						seleccionQRGen = (seleccionQRGen + 1) % numOpcionesQRGen;

					// Actualizar opción anterior (quitar cursor)
					gotoxy(0, menuY + seleccionAnterior);
					std::cout << "   " << opcionesQRGen[seleccionAnterior] << "   ";

					// Actualizar nueva opción seleccionada (poner cursor)
					gotoxy(0, menuY + seleccionQRGen);
					std::cout << " > " << opcionesQRGen[seleccionQRGen] << "   ";
				}
			}
			else if (teclaQRGen == 13) { // ENTER
				if (seleccionQRGen == 0) { // Generar PDF
					// Crear nombre del archivo
					std::string nombreArchivo = "QR_" +
						persona.getNombres() + "_" +
						persona.getApellidos();

					// Eliminar caracteres no válidos para nombre de archivo
					nombreArchivo = Utilidades::EliminarEspacios(nombreArchivo);

					// Añadir extensión y ruta temporal
					char tempPath[MAX_PATH];
					GetTempPathA(MAX_PATH, tempPath);
					std::string rutaCompleta = std::string(tempPath) + nombreArchivo + ".pdf";

					try {
						// Mostrar proceso de generación
						gotoxy(0, menuY + numOpcionesQRGen);
						std::cout << "\nGenerando PDF del código QR..." << std::endl;

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
				}
				else { // Volver al menu
					if (marquesinaGlobal) {
						marquesinaGlobal->finalizarOperacionCritica();
					}
					limpiarPantallaPreservandoMarquesina(1);
					return true;
				}
			}
			else if (teclaQRGen == 27) { // ESC
				if (marquesinaGlobal) {
					marquesinaGlobal->finalizarOperacionCritica();
				}
				limpiarPantallaPreservandoMarquesina(1);
				return false;
			}
		}
	}
	catch (const std::exception& e) {
		// Manejo de excepciones
		if (marquesinaGlobal) {
			marquesinaGlobal->finalizarOperacionCritica();
		}
		limpiarPantallaPreservandoMarquesina(1);
		std::cout << "Error generando QR: " << e.what() << std::endl;
		system("pause");
		return false;
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
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

/**
 * @brief Muestra el cursor de la consola
 */
void Utilidades::mostrarCursor() {
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 25; // Tamaño normal
	cursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}
