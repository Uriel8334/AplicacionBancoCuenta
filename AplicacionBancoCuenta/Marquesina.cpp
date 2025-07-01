#include "Marquesina.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <regex>
#include <string>

// Namespace para utilidades de la consola
namespace Utilidades
{
	void gotoxy(int x, int y)
	{
		COORD coord;
		coord.X = x;
		coord.Y = y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}
}

Marquesina::Marquesina(int x, int y, int ancho, const std::string& archivoHTML, int velocidad)
	: posX(x), posY(y), ancho(ancho), archivoHTML(archivoHTML), velocidad(velocidad),
	ejecutando(false), pausado(false)
{

	// Mostrar la ruta del archivo para depuraci�n
	//mostrarRutaArchivo();

	// Inicializar la �ltima modificaci�n a 0
	ZeroMemory(&ultimaModificacion, sizeof(FILETIME));
	cargarDesdeHTML();
}

Marquesina::~Marquesina()
{
	detener();
}

void Marquesina::iniciar()
{
	if (!ejecutando)
	{
		ejecutando = true;
		pausado = false;
		hiloMarquesina = std::thread(&Marquesina::ejecutarMarquesina, this);
	}
}

void Marquesina::detener()
{
	if (ejecutando)
	{
		ejecutando = false;
		if (hiloMarquesina.joinable())
		{
			hiloMarquesina.join();
		}
	}
}

void Marquesina::pausar()
{
	pausado = true;
}

void Marquesina::reanudar()
{
	pausado = false;
}

void Marquesina::actualizarTexto(const std::string& nuevoTexto)
{
	std::lock_guard<std::mutex> lock(mtx);

	// Escribir el nuevo texto en el archivo HTML
	std::ofstream archivo(archivoHTML);
	if (archivo)
	{
		archivo << "<marquesina>\n";
		archivo << "  " << nuevoTexto << "\n";
		archivo << "</marquesina>";
		archivo.close();

		// Recargar el contenido
		cargarDesdeHTML();
	}
}

//funci�n auxiliar antes de la implementaci�n de la clase
std::wstring stringToWideString(const std::string& str)
{
	if (str.empty())
		return L"";

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
	return wstr;
}

void Marquesina::mostrarRutaArchivo()
{
	char path[MAX_PATH];
	GetFullPathNameA(archivoHTML.c_str(), MAX_PATH, path, NULL);
	std::cout << "Ruta completa del archivo HTML: " << path << std::endl;
}

// Modificar la funcion cargarDesdeHTML
void Marquesina::cargarDesdeHTML()
{
	std::lock_guard<std::mutex> lock(mtx);

	std::ifstream archivo(archivoHTML);
	if (!archivo)
	{
		// Si el archivo no existe, crear uno predeterminado
		std::ofstream crear(archivoHTML);
		if (crear)
		{
			crear << "<marquesina>\n";
			crear << "  <color=white bg=blue>Bienvenido al Sistema Bancario</color> - <b> ";
			crear << "      <color=yellow>Hola Mundo</color>";
			crear << "          <color=yellow>Desarrolladores: Uriel Andrade, Kerly Chuqui, Abner Proano</color>\n";
			crear << "</marquesina>";
			crear.close();
			archivo.open(archivoHTML);
		}
	}

	if (archivo)
	{
		std::string contenido;
		std::string linea;
		while (std::getline(archivo, linea))
		{
			contenido += linea + "\n";
		}
		elementos = parsearHTML(contenido);

		// Actualizar la marca de tiempo usando API de Windows con soporte Unicode
		std::wstring wArchivoHTML = stringToWideString(archivoHTML);
		HANDLE hFile = CreateFileW(
			wArchivoHTML.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			GetFileTime(hFile, NULL, NULL, &ultimaModificacion);
			CloseHandle(hFile);
		}
	}
}
// Modificar la función archivoModificado
bool Marquesina::archivoModificado()
{
	FILETIME nuevoTiempo;
	ZeroMemory(&nuevoTiempo, sizeof(FILETIME));

	// Usar versión Unicode de CreateFile
	std::wstring wArchivoHTML = stringToWideString(archivoHTML);
	HANDLE hFile = CreateFileW(
		wArchivoHTML.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		GetFileTime(hFile, NULL, NULL, &nuevoTiempo);
		CloseHandle(hFile);

		// Comparar los tiempos
		if (CompareFileTime(&nuevoTiempo, &ultimaModificacion) > 0)
		{
			ultimaModificacion = nuevoTiempo;
			return true;
		}
	}

	return false;
}

std::vector<ElementoMarquesina> Marquesina::parsearHTML(const std::string& contenido)
{
	std::vector<ElementoMarquesina> resultado;

	// En C++14 no existe regex::dotall, usamos una t�cnica alternativa
	// para hacer que el punto coincida con nuevas l�neas
	std::regex marquesinaRegex("<marquesina>[\\s\\S]*?</marquesina>");
	std::regex colorRegex("<color=([a-z]+)(?:\\s+bg=([a-z]+))?>(.*?)</color>");
	std::regex boldRegex("<b>(.*?)</b>");

	// Extraer contenido de la etiqueta <marquesina>
	std::smatch matchMarquesina;
	if (std::regex_search(contenido, matchMarquesina, marquesinaRegex))
	{
		std::string textoMarquesina = matchMarquesina.str();

		// Eliminar las etiquetas de apertura y cierre
		textoMarquesina = std::regex_replace(textoMarquesina, std::regex("<marquesina>"), "");
		textoMarquesina = std::regex_replace(textoMarquesina, std::regex("</marquesina>"), "");

		// Procesar colores
		std::string::const_iterator searchStart(textoMarquesina.cbegin());
		std::string::const_iterator end(textoMarquesina.cend());
		std::smatch match;

		while (std::regex_search(searchStart, end, match, colorRegex))
		{
			ElementoMarquesina elem;

			// Mapear nombre de color a c�digo Windows
			std::string colorTexto = match[1].str();
			if (colorTexto == "black")
				elem.colorTexto = 0;
			else if (colorTexto == "blue")
				elem.colorTexto = 1;
			else if (colorTexto == "green")
				elem.colorTexto = 2;
			else if (colorTexto == "cyan")
				elem.colorTexto = 3;
			else if (colorTexto == "red")
				elem.colorTexto = 4;
			else if (colorTexto == "magenta")
				elem.colorTexto = 5;
			else if (colorTexto == "yellow")
				elem.colorTexto = 6;
			else if (colorTexto == "white")
				elem.colorTexto = 7;
			else
				elem.colorTexto = 7; // Blanco por defecto

			// Color de fondo si est� presente
			elem.colorFondo = 0; // Negro por defecto
			if (match[2].matched)
			{
				std::string colorFondo = match[2].str();
				if (colorFondo == "black")
					elem.colorFondo = 0;
				else if (colorFondo == "blue")
					elem.colorFondo = 1;
				else if (colorFondo == "green")
					elem.colorFondo = 2;
				else if (colorFondo == "cyan")
					elem.colorFondo = 3;
				else if (colorFondo == "red")
					elem.colorFondo = 4;
				else if (colorFondo == "magenta")
					elem.colorFondo = 5;
				else if (colorFondo == "yellow")
					elem.colorFondo = 6;
				else if (colorFondo == "white")
					elem.colorFondo = 7;
			}

			// Procesar negrita dentro de color
			std::string contenidoColor = match[3].str();
			std::smatch boldMatch;
			if (std::regex_search(contenidoColor, boldMatch, boldRegex))
			{
				elem.negrita = true;
				elem.texto = boldMatch[1].str();
			}
			else
			{
				elem.negrita = false;
				elem.texto = contenidoColor;
			}

			resultado.push_back(elem);
			searchStart = match.suffix().first;
		}

		// Procesar texto sin formato (no capturado por las regex anteriores)
		if (resultado.empty())
		{
			ElementoMarquesina elem;
			elem.colorTexto = 7;
			elem.colorFondo = 0;
			elem.negrita = false;
			elem.texto = textoMarquesina;
			resultado.push_back(elem);
		}
	}

	return resultado;
}

void Marquesina::ejecutarMarquesina()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// Generar texto completo con espacios
	std::string textoCompleto = "   "; // Espacio inicial
	for (const auto& elem : elementos)
	{
		textoCompleto += elem.texto + "   "; // A�adir espacios entre elementos
	}

	// Array de colores para alternar (colores brillantes de Windows Console)
	WORD colores[] = {
		FOREGROUND_RED | FOREGROUND_INTENSITY,                                      // Rojo brillante
	};
	int numColores = sizeof(colores) / sizeof(colores[0]);

	int posicionTexto = 0;

	while (ejecutando)
	{
		if (!pausado && !elementos.empty())
		{
			if (archivoModificado())
			{
				cargarDesdeHTML();
				// Regenerar texto completo
				textoCompleto = "   ";
				for (const auto& elem : elementos)
				{
					textoCompleto += elem.texto + "   ";
				}
				posicionTexto = 0;
			}

			// Guardar la posici�n actual del cursor
			GetConsoleScreenBufferInfo(hConsole, &csbi);
			COORD posicionOriginal = csbi.dwCursorPosition;

			// Establecer la posici�n para la marquesina
			COORD marquesinaPos = { (SHORT)posX, (SHORT)posY };
			SetConsoleCursorPosition(hConsole, marquesinaPos);

			// Dibujar la marquesina
			std::string textoVisible = textoCompleto.substr(posicionTexto, ancho);
			if (textoVisible.length() < ancho)
			{
				textoVisible += textoCompleto.substr(0, ancho - textoVisible.length());
			}

			// Limpiar l�nea
			std::cout << std::string(ancho, ' ');
			SetConsoleCursorPosition(hConsole, marquesinaPos);

			// Imprimir cada letra con un color diferente
			for (size_t i = 0; i < textoVisible.length() && i < ancho; ++i)
			{
				// Calcular el �ndice de color basado en la posici�n del texto y la letra
				int indiceColor = (posicionTexto + i) % numColores;

				// Si es un espacio, usar un color m�s tenue para que no sea tan visible
				if (textoVisible[i] == ' ')
				{
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				}
				else
				{
					SetConsoleTextAttribute(hConsole, colores[indiceColor]);
				}

				std::cout << textoVisible[i];
			}

			// Restaurar color por defecto
			SetConsoleTextAttribute(hConsole, csbi.wAttributes);

			// Restaurar la posici�n original del cursor
			SetConsoleCursorPosition(hConsole, posicionOriginal);

			// Avanzar posici�n para el desplazamiento
			posicionTexto = (posicionTexto + 1) % textoCompleto.length();

			// Pausar para controlar la velocidad
			std::this_thread::sleep_for(std::chrono::milliseconds(velocidad));
		}
		else
		{
			// Si est� pausado, solo esperar
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}