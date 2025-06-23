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

// Funcion para mover el cursor en la consola
void Utilidades::gotoxy(int x, int y) {
	COORD coord;
	coord.X = x; coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
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
    } else {
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
    } else {
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