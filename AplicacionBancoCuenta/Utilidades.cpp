#include "Utilidades.h"
#include "NodoPersona.h"
#include "Persona.h"
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <regex>
#include <string>
#include <ctime>
#include <chrono>
#include <queue>
#include <algorithm>
#include "GeneradorQRBanco.h"
#include "Marquesina.h"

// Nodo para el Arbol B
template<typename T>
class NodoArbolB {
public:
	bool esHoja;                       // Indica si es un nodo hoja
	std::vector<T*> claves;            // Claves del nodo (punteros a objetos)
	std::vector<NodoArbolB<T>*> hijos; // Punteros a los hijos

	// Constructor
	NodoArbolB(bool hoja = true) : esHoja(hoja) {}

	// Destructor
	~NodoArbolB() {
		for (auto& hijo : hijos) {
			delete hijo;
		}
	}
};

// ImplementaciOn didActica de Arbol B
template<typename T>
class ArbolB {
private:
	NodoArbolB<T>* raiz;
	int grado; // Grado mInimo (determina nUmero de claves por nodo)

	// Busca una clave en el nodo y sus subArboles
	T* buscarEnNodo(NodoArbolB<T>* nodo, const std::string& valor,
		std::function<bool(const T*, const std::string&)> comparador) {
		if (!nodo) return nullptr;

		// Buscar la clave en el nodo actual
		for (auto& clave : nodo->claves) {
			if (comparador(clave, valor)) {
				return clave;
			}
		}

		// Si es hoja y no se encontrO, no existe
		if (nodo->esHoja) {
			return nullptr;
		}

		// Determinar subArbol donde podrIa estar la clave
		int i = 0;
		while (i < nodo->claves.size() && !comparador(nodo->claves[i], valor)) {
			i++;
		}

		// Buscar en el hijo correspondiente
		return buscarEnNodo(nodo->hijos[i < nodo->hijos.size() ? i : nodo->hijos.size() - 1], valor, comparador);
	}

public:
	// Constructor
	ArbolB(int _grado) : raiz(nullptr), grado(_grado) {
		if (grado < 2) grado = 2; // MInimo grado 2
	}

	// Destructor
	~ArbolB() {
		if (raiz) delete raiz;
	}

	// Construir Arbol desde un vector de elementos
	void construirDesdeVector(std::vector<T*>& elementos) {
		// Crear raIz
		raiz = new NodoArbolB<T>();

		// Si hay pocos elementos, los ponemos en la raIz
		if (elementos.size() <= 2 * grado - 1) {
			for (auto& elem : elementos) {
				raiz->claves.push_back(elem);
			}
			return;
		}

		// Si hay muchos elementos, crear estructura de Arbol
		raiz->esHoja = false;

		int elementosPorNodo = grado;
		int totalElementos = elementos.size();
		int posActual = 0;

		// Crear nodos hoja y distribuir elementos
		while (posActual < totalElementos) {
			NodoArbolB<T>* nodoHoja = new NodoArbolB<T>(true);

			// Llenar nodo con elementos
			for (int i = 0; i < elementosPorNodo && posActual < totalElementos; i++) {
				nodoHoja->claves.push_back(elementos[posActual++]);
			}

			// Agregar nodo como hijo de la raIz
			raiz->hijos.push_back(nodoHoja);

			// Si no es el primer hijo, promover una clave a la raIz
			if (raiz->hijos.size() > 1 && !nodoHoja->claves.empty()) {
				raiz->claves.push_back(nodoHoja->claves.front());
			}
		}
	}

	// Buscar elemento en el Arbol
	T* buscar(const std::string& valor, std::function<bool(const T*, const std::string&)> comparador) {
		return buscarEnNodo(raiz, valor, comparador);
	}

	// Mostrar estructura del Arbol
	void mostrar() {
		if (!raiz) {
			std::cout << "Arbol vacIo" << std::endl;
			return;
		}

		// Recorrer por niveles
		std::queue<NodoArbolB<T>*> cola;
		cola.push(raiz);

		int nivel = 0;
		while (!cola.empty()) {
			int nodosEnNivel = cola.size();
			std::cout << "Nivel " << nivel << ": ";

			for (int i = 0; i < nodosEnNivel; i++) {
				NodoArbolB<T>* nodoActual = cola.front();
				cola.pop();

				// Mostrar claves del nodo
				std::cout << "[ ";
				for (auto& clave : nodoActual->claves) {
					if (auto persona = dynamic_cast<Persona*>(clave)) {
						std::cout << persona->getNombres() << " ";
					}
					else {
						std::cout << "objeto ";
					}
				}
				std::cout << "] ";

				// Encolar hijos
				for (auto& hijo : nodoActual->hijos) {
					cola.push(hijo);
				}
			}
			std::cout << std::endl;
			nivel++;
		}
	}

	int altura() const {
		if (!raiz) return 0;

		// Aproximamos la altura basada en el número de nodos
		int nodosEstimados = 0;
		for (auto& hijo : raiz->hijos) {
			nodosEstimados++;
			if (!hijo->esHoja) {
				nodosEstimados += hijo->hijos.size();
			}
		}

		// La altura del árbol es aproximadamente log(n)
		return raiz->hijos.empty() ? 1 : 2 + (nodosEstimados > 0 ? 1 : 0);
	}
};


// Variable externa para acceso a la marquesina global
extern Marquesina* marquesinaGlobal;

// Control de operaciones críticas
void Utilidades::iniciarOperacionCritica()
{
    if (marquesinaGlobal)
    {
        marquesinaGlobal->marcarOperacionCritica();
        // Pequeña pausa para asegurar que la marquesina se detenga
        Sleep(10);
    }
}

void Utilidades::finalizarOperacionCritica()
{
    if (marquesinaGlobal)
    {
        marquesinaGlobal->finalizarOperacionCritica();
    }
}

// Función gotoxy mejorada y thread-safe
void Utilidades::gotoxy(int x, int y)
{
    // Marcar operación crítica de cursor
    iniciarOperacionCritica();
    
    COORD coord;
    coord.X = x; 
    coord.Y = y + 2; // Offset para la marquesina
    
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, coord);
    
    // Finalizar operación crítica después de una pausa mínima
    Sleep(1);
    finalizarOperacionCritica();
}

// Función mejorada para limpiar pantalla
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

Utilidades::Utilidades() {
	// Constructor vacio
}

Utilidades::~Utilidades() {
	// Destructor vacio
}

// Funcion para formatear valores monetarios con formato americano ($1,000.23)
std::string formatearValorMonetario(double valor) {
	std::stringstream ss;
	ss.imbue(std::locale("en_US.UTF-8")); // Usar locale americano (comas para miles, punto para decimales)
	ss << std::fixed << std::setprecision(2) << valor;
	return ss.str();
}

// Funcion para formatear valores en centavos a formato monetario
std::string formatearCentavosAMonetario(int valorEnCentavos) {
	return formatearValorMonetario(valorEnCentavos / 100.0);
}

// Metodo para convertir un string a double
double Utilidades::ConvertirADouble(const std::string& texto) {
	// Verificar si el texto esta vacio
	if (texto.empty()) {
		std::cout << "Texto vacio, retornando 0.0" << std::endl;
		return 0.0;
	}

	// Eliminar caracteres no numericos excepto el punto decimal y signo negativo
	std::string soloNumeros;
	bool puntoEncontrado = false;

	for (char c : texto) {
		if (std::isdigit(c)) {
			soloNumeros += c;
		}
		else if (c == '.' && !puntoEncontrado) {
			soloNumeros += c;
			puntoEncontrado = true;
		}
		else if (c == '-' && soloNumeros.empty()) {
			// Solo permitir el signo negativo al principio
			soloNumeros += c;
		}
	}

	// Si la cadena resultante esta vacia o solo tiene un signo negativo
	if (soloNumeros.empty() || soloNumeros == "-") {
		std::cout << "No se encontraron digitos validos, retornando 0.0" << std::endl;
		return 0.0;
	}

	// Bloque para manejar excepciones
	try {
		// Convertir la cadena limpia a double
		double resultado = std::stod(soloNumeros);
		std::cout << "Conversion exitosa: " << std::fixed << std::setprecision(2) << resultado << std::endl; // Para depuracion
		return resultado;
	}
	catch (const std::invalid_argument&) {
		std::cout << "Error: argumento invalido en la conversion" << std::endl; // Para depuracion
		return 0.0;
	}
	catch (const std::out_of_range&) {
		std::cout << "Error: valor fuera de rango" << std::endl; // Para depuracion
		return 0.0;
	}
	catch (const std::exception& e) {
		std::cout << "Error desconocido: " << e.what() << std::endl; // Para depuracion
		return 0.0;
	}
}

// Metodo para formatear un monto a string con separadores de miles y decimales
std::string Utilidades::FormatearMonto(double monto, int decimales) {
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(decimales) << monto;

	std::string resultado = oss.str();

	// Agregar separadores de miles
	size_t posDecimal = resultado.find('.');
	if (posDecimal == std::string::npos) {
		posDecimal = resultado.length();
	}

	// Usar size_t para el indice del bucle para evitar la conversion
	if (posDecimal > 3) { // Verificar que hay suficientes digitos para insertar comas
		for (size_t i = posDecimal - 3; i > 0; i -= 3) {
			resultado.insert(i, ",");
			// Evitar el problema del unsigned underflow cuando i es menor que 3
			if (i <= 3) break;
		}
	}

	return resultado;
}

// Metodo para formatear la fecha 
std::string Utilidades::FormatearFecha(int dia, int mes, int anio) {
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(2) << dia << "/"
		<< std::setfill('0') << std::setw(2) << mes << "/"
		<< anio;

	return oss.str();
}

// metodo para comprobar si es numerico
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

// metodo para validar el correo 
bool Utilidades::EsCorreoValido(const std::string& correo) {
	// Expresion regular simple para validar correos electronicos
	const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
	return std::regex_match(correo, pattern);
}

// Metodo para eliminar espacios 
std::string Utilidades::EliminarEspacios(const std::string& texto) {
	std::string resultado = texto;
	resultado.erase(std::remove_if(resultado.begin(), resultado.end(),
		[](unsigned char c) { return std::isspace(c); }),
		resultado.end());
	return resultado;
}

// Metodo para convertir a mayusculas 
std::string Utilidades::ConvertirAMayusculas(const std::string& texto) {
	std::string resultado = texto;
	std::transform(resultado.begin(), resultado.end(), resultado.begin(),
		[](unsigned char c) { return std::toupper(c); });
	return resultado;
}

// Metodo para convertir a minusculas 
std::string Utilidades::ConvertirAMinusculas(const std::string& texto) {
	std::string resultado = texto;
	std::transform(resultado.begin(), resultado.end(), resultado.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return resultado;
}


// metodo para regresar al menu
std::string Utilidades::Regresar() {
	return "Regresar al menu principal";
}

// Funcion para abrir la aplicacion de menu de ayuda 
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

	if ((int)resultado <= 32) {
		std::cerr << "Error al abrir el menu de ayuda. Codigo: " << (int)resultado << std::endl;
	}
}

// Implementacion didactica de hash para archivos
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
	unsigned char buffer[64]; // Tamaño de bloque tipico: 64 bytes
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

// Verificar si el hash coincide con el del archivo
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

// Guardar hash en un archivo separado con metadatos educativos
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

// Leer hash desde un archivo, saltando lineas de comentarios
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

template<typename T>
void mostrarMenuOrdenar(std::vector<T*>& vec,
	const std::vector<std::string>& opciones,
	const std::vector<std::function<bool(const T*, const T*)>>& criterios,
	std::function<void(const std::vector<T*>&)> mostrarDatos)
{
	int seleccion = 0;
	while (true) {
		system("cls");
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
			if (tecla == 72) seleccion = (seleccion - 1 + opciones.size()) % opciones.size();
			else if (tecla == 80) seleccion = (seleccion + 1) % opciones.size();
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

// Implementación de la función PorArbolB mejorada
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
		system("cls");
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
				selCriterio = (selCriterio - 1 + criterios.size()) % criterios.size();
			else if (tecla == 80) // Flecha abajo
				selCriterio = (selCriterio + 1) % criterios.size();

			// Redibujar todo el menú
			mostrarMenuCriterios();
		}
		else if (tecla == 13) break; // Enter
		else if (tecla == 27) return; // ESC
	}

	// Resto del código para definir comparadores, construir árbol, etc...
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
			return Utilidades::ConvertirAMinusculas(a->getNombres()) <
				Utilidades::ConvertirAMinusculas(b->getNombres());
			};
		criterioBusqueda = [](const Persona* p, const std::string& nombre) {
			return Utilidades::ConvertirAMinusculas(p->getNombres()).find(
				Utilidades::ConvertirAMinusculas(nombre)) != std::string::npos;
			};
		break;
	case 2: // Apellido
		criterioOrdenamiento = [](const Persona* a, const Persona* b) {
			return Utilidades::ConvertirAMinusculas(a->getApellidos()) <
				Utilidades::ConvertirAMinusculas(b->getApellidos());
			};
		criterioBusqueda = [](const Persona* p, const std::string& apellido) {
			return Utilidades::ConvertirAMinusculas(p->getApellidos()).find(
				Utilidades::ConvertirAMinusculas(apellido)) != std::string::npos;
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
	ArbolB<Persona> arbol(3);
	arbol.construirDesdeVector(personas);
	auto fin = std::chrono::high_resolution_clock::now();
	auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

	// Submenú de operaciones con mismo enfoque que el menú principal
	std::vector<std::string> opcionesArbol = { "Buscar persona", "Volver" };
	int selOpcion = 0;

	// Función local para mostrar menú completo
	auto mostrarMenuCompleto = [&criterios, &selCriterio, &arbol, &duracion, &opcionesArbol, &selOpcion]() {
		system("cls");
		std::cout << "=== ARBOL B DIDACTICO ===" << std::endl;
		std::cout << "Ordenado por: " << criterios[selCriterio] << std::endl;
		arbol.mostrar();
		std::cout << "\nTiempo de construccion: " << duracion << " milisegundos." << std::endl;

		std::cout << "\nSeleccione operacion:" << std::endl;
		for (size_t i = 0; i < opcionesArbol.size(); i++) {
			if (i == selOpcion)
				std::cout << " > " << opcionesArbol[i] << std::endl;
			else
				std::cout << "   " << opcionesArbol[i] << std::endl;
		}
		};

	// Mostrar menú inicial
	mostrarMenuCompleto();

	while (true) {
		int tecla = _getch();
		if (tecla == 224) {
			tecla = _getch();
			if (tecla == 72) // Flecha arriba
				selOpcion = (selOpcion - 1 + opcionesArbol.size()) % opcionesArbol.size();
			else if (tecla == 80) // Flecha abajo
				selOpcion = (selOpcion + 1) % opcionesArbol.size();

			// Redibujamos el menú completo con la nueva selección
			mostrarMenuCompleto();
		}
		else if (tecla == 13) { // Enter
			if (selOpcion == 0) { // Buscar persona
				// Coordenadas para el área de entrada
				int baseY = arbol.altura() + 8 + opcionesArbol.size();
				Utilidades::gotoxy(0, baseY);
				std::cout << std::string(80, ' '); // Limpiar línea
				Utilidades::gotoxy(0, baseY);

				std::string criterioBusquedaStr;
				// Si criterios es igual a Fecha de nacimiento imprimir "DD/MM/AAAA"
				if (selCriterio == 3) {
					std::cout << "Usar el siguiente formato para buscar por fecha DD/MM/AAAA: ";
				}
				std::cout << "Ingrese " << criterios[selCriterio] << " a buscar: ";

				std::cin >> criterioBusquedaStr;

				// Búsqueda y medición de tiempo
				auto inicioBusqueda = std::chrono::high_resolution_clock::now();
				Persona* encontrado = arbol.buscar(criterioBusquedaStr, criterioBusqueda);
				auto finBusqueda = std::chrono::high_resolution_clock::now();
				auto duracionBusqueda = std::chrono::duration_cast<std::chrono::milliseconds>
					(finBusqueda - inicioBusqueda).count();

				// Mostrar resultados
				Utilidades::gotoxy(0, baseY + 1);
				if (encontrado) {
					std::cout << "Persona encontrada:" << std::endl;
					std::cout << "Cedula: " << encontrado->getCedula() << std::endl;
					std::cout << "Nombre: " << encontrado->getNombres() << std::endl;
					std::cout << "Apellidos: " << encontrado->getApellidos() << std::endl;
					std::cout << "Fecha de nacimiento: " << encontrado->getFechaNacimiento() << std::endl;
					std::cout << "Correo: " << encontrado->getCorreo() << std::endl;
				}
				else {
					std::cout << "Persona no encontrada." << std::endl;
				}

				std::cout << "Tiempo de busqueda: " << duracionBusqueda << " milisegundos." << std::endl;
				std::cout << "\nPresione Enter para continuar...";
				std::cin.ignore();
				std::cin.get();

				// Redibujar menú completo
				mostrarMenuCompleto();
			}
			else {
				break; // Volver
			}
		}
		else if (tecla == 27) { // ESC
			break;
		}
	}
}

// Generar QR para Persona y numero de cuenta 
bool Utilidades::generarQRPersona(const Persona& persona, const std::string& numeroCuenta) {
	try {
		// Validar datos
		if (!GeneradorQRBanco::esNumeroCuentaValido(numeroCuenta)) {
			std::cout << "Numero de cuenta invalido." << std::endl;
			return false;
		}

		std::string nombreCompleto = persona.getNombres() + " " + persona.getApellidos();
		if (!GeneradorQRBanco::esNombreValido(nombreCompleto)) {
			std::cout << "Nombre invalido para QR." << std::endl;
			return false;
		}

		// Crear y generar QR
		GeneradorQRBanco qr(persona, numeroCuenta);
		qr.generar();
		qr.imprimir();

		// Preguntar si desea guardar
		std::cout << "\nDesea guardar el QR? (s/n): ";
		char opcion = _getch();
		std::cout << opcion << std::endl;

		if (opcion == 's' || opcion == 'S') {
			std::string archivoBase = "QR_" + numeroCuenta;
			qr.guardarComoSVG(archivoBase + ".svg");
			qr.guardarInformacionCuenta(archivoBase + "_info.txt");
		}

		return true;
	}
	catch (const std::exception& e) {
		std::cout << "Error generando QR: " << e.what() << std::endl;
		return false;
	}
}

// Método para generar QR con datos manuales
bool Utilidades::generarQRManual() {
	limpiarPantallaPreservandoMarquesina();

	std::cout << "=== GENERADOR DE QR BANCARIO ===" << std::endl;
	std::cout << "NOTA: Solo se incluyen datos basicos (nombre y cuenta)" << std::endl;
	std::cout << std::string(50, '=') << std::endl;

	std::string numeroCuenta, nombreCompleto;

	// Solicitar número de cuenta - CORREGIDO para 10 dígitos exactos
	do {
		std::cout << "\nIngrese el numero de cuenta (exactamente 10 digitos): ";
		std::getline(std::cin, numeroCuenta);

		if (!GeneradorQRBanco::esNumeroCuentaValido(numeroCuenta)) {
			std::cout << "Numero de cuenta invalido. Debe contener exactamente 10 digitos numericos." << std::endl;
		}
	} while (!GeneradorQRBanco::esNumeroCuentaValido(numeroCuenta));

	// Solicitar nombre del usuario
	do {
		std::cout << "Ingrese el nombre completo (2-60 caracteres): ";
		std::getline(std::cin, nombreCompleto);

		if (!GeneradorQRBanco::esNombreValido(nombreCompleto)) {
			std::cout << "Nombre invalido. Solo letras, espacios y guiones (2-60 caracteres)." << std::endl;
		}
	} while (!GeneradorQRBanco::esNombreValido(nombreCompleto));

	try {
		// Generar QR
		std::cout << "\nGenerando codigo QR..." << std::endl;

		GeneradorQRBanco qr(nombreCompleto, numeroCuenta);
		qr.generar();
		qr.imprimir();

		// Opciones de guardado
		std::cout << "\nDesea guardar el QR? (s/n): ";
		char opcion = _getch();
		std::cout << opcion << std::endl;

		if (opcion == 's' || opcion == 'S') {
			std::string archivoBase = "QR_" + numeroCuenta;
			qr.guardarComoSVG(archivoBase + ".svg");
			qr.guardarInformacionCuenta(archivoBase + "_info.txt");

			std::cout << "\nArchivos generados exitosamente:" << std::endl;
			std::cout << "   - " << archivoBase << ".svg (imagen del QR)" << std::endl;
			std::cout << "   - " << archivoBase << "_info.txt (informacion de cuenta)" << std::endl;
		}

		std::cout << "\nProceso completado. El QR contiene unicamente:" << std::endl;
		std::cout << "   - Nombre: " << nombreCompleto << std::endl;
		std::cout << "   - Numero de cuenta: " << numeroCuenta << std::endl;
		std::cout << "\nSin datos sensibles incluidos." << std::endl;

		return true;
	}
	catch (const std::exception& e) {
		std::cout << "Error generando QR: " << e.what() << std::endl;
		return false;
	}
}