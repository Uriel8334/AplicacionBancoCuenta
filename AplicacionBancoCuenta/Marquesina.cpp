#include "Marquesina.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <regex>
#include <string>

Marquesina::Marquesina(int x, int y, int ancho, const std::string& archivoHTML, int velocidad)
	: posX(x), posY(y), ancho(ancho), archivoHTML(archivoHTML), velocidad(velocidad),
	ejecutando(false), pausado(false), bloqueado(false), operacionCritica(false),
	bufferActualizado(false)
{
	ZeroMemory(&ultimaModificacion, sizeof(FILETIME));
	cargarDesdeHTML();
	actualizarBuffer();
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
		bloqueado = false;
		operacionCritica = false;
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

void Marquesina::bloquear()
{
	bloqueado = true;
}

void Marquesina::desbloquear()
{
	bloqueado = false;
}

// NUEVOS: Métodos para operaciones críticas
void Marquesina::marcarOperacionCritica()
{
	operacionCritica = true;
}

void Marquesina::finalizarOperacionCritica()
{
	operacionCritica = false;
}

void Marquesina::forzarActualizacion()
{
	std::lock_guard<std::mutex> lock(mtx);
	cargarDesdeHTML();
	actualizarBuffer();
}

void Marquesina::actualizarTexto(const std::string& nuevoTexto)
{
	std::lock_guard<std::mutex> lock(mtx);

	std::ofstream archivo(archivoHTML);
	if (archivo)
	{
		archivo << "<marquesina>\n";
		archivo << "  " << nuevoTexto << "\n";
		archivo << "</marquesina>";
		archivo.close();
		cargarDesdeHTML();
		actualizarBuffer();
	}
}

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

void Marquesina::cargarDesdeHTML()
{
	std::ifstream archivo(archivoHTML);
	if (!archivo)
	{
		std::ofstream crear(archivoHTML);
		if (crear)
		{
			crear << "<marquesina>\n";
			crear << "  <color=white bg=blue>Bienvenido al Sistema Bancario</color> - ";
			crear << "<color=yellow>Desarrolladores: Uriel Andrade, Kerly Chuqui, Abner Proano</color>\n";
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

bool Marquesina::archivoModificado()
{
	FILETIME nuevoTiempo;
	ZeroMemory(&nuevoTiempo, sizeof(FILETIME));

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

		if (CompareFileTime(&nuevoTiempo, &ultimaModificacion) > 0)
		{
			ultimaModificacion = nuevoTiempo;
			return true;
		}
	}

	return false;
}

bool Marquesina::esSafeZone(int fila)
{
	return (fila >= posY && fila <= posY + 1);
}

// NUEVO: Actualizar buffer interno
void Marquesina::actualizarBuffer()
{
	std::lock_guard<std::mutex> lock(mtx);
	bufferMarquesina = "   ";
	for (const auto& elem : elementos)
	{
		bufferMarquesina += elem.texto + "   ";
	}
	bufferActualizado = true;
}

// NUEVO: Renderizado separado y más seguro
void Marquesina::renderizarMarquesina()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	
	// Verificar estado de la consola antes de proceder
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
		return;

	// Crear buffer local para evitar conflictos
	std::string textoBuffer;
	{
		std::lock_guard<std::mutex> lock(mtx);
		textoBuffer = bufferMarquesina;
	}

	if (textoBuffer.empty())
		return;

	// Operación completamente atómica
	COORD marquesinaPos = { (SHORT)posX, (SHORT)posY };
	
	// Usar API de Windows más eficiente
	DWORD caracteresEscritos;
	std::string lineaLimpia(ancho, ' ');
	
	// Escribir línea completa en una sola operación
	WriteConsoleOutputCharacterA(
		hConsole,
		lineaLimpia.c_str(),
		ancho,
		marquesinaPos,
		&caracteresEscritos
	);

	// Preparar texto visible
	static int posicionTexto = 0;
	std::string textoVisible = textoBuffer.substr(posicionTexto, ancho);
	if (textoVisible.length() < ancho)
	{
		textoVisible += textoBuffer.substr(0, ancho - textoVisible.length());
	}

	// Escribir texto con color
	WORD colorTurquesa = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	
	for (size_t i = 0; i < textoVisible.length() && i < ancho; ++i)
	{
		COORD charPos = { (SHORT)(posX + i), (SHORT)posY };
		WriteConsoleOutputCharacterA(hConsole, &textoVisible[i], 1, charPos, &caracteresEscritos);
		WriteConsoleOutputAttribute(hConsole, &colorTurquesa, 1, charPos, &caracteresEscritos);
	}

	posicionTexto = (posicionTexto + 1) % textoBuffer.length();
}

std::vector<ElementoMarquesina> Marquesina::parsearHTML(const std::string& contenido)
{
	std::vector<ElementoMarquesina> resultado;

	std::regex marquesinaRegex("<marquesina>[\\s\\S]*?</marquesina>");
	std::regex colorRegex("<color=([a-z]+)(?:\\s+bg=([a-z]+))?>(.*?)</color>");
	std::regex boldRegex("<b>(.*?)</b>");

	std::smatch matchMarquesina;
	if (std::regex_search(contenido, matchMarquesina, marquesinaRegex))
	{
		std::string textoMarquesina = matchMarquesina.str();
		textoMarquesina = std::regex_replace(textoMarquesina, std::regex("<marquesina>"), "");
		textoMarquesina = std::regex_replace(textoMarquesina, std::regex("</marquesina>"), "");

		std::string::const_iterator searchStart(textoMarquesina.cbegin());
		std::string::const_iterator end(textoMarquesina.cend());
		std::smatch match;

		while (std::regex_search(searchStart, end, match, colorRegex))
		{
			ElementoMarquesina elem;

			std::string colorTexto = match[1].str();
			if (colorTexto == "black") elem.colorTexto = 0;
			else if (colorTexto == "blue") elem.colorTexto = 1;
			else if (colorTexto == "green") elem.colorTexto = 2;
			else if (colorTexto == "cyan") elem.colorTexto = 3;
			else if (colorTexto == "red") elem.colorTexto = 4;
			else if (colorTexto == "magenta") elem.colorTexto = 5;
			else if (colorTexto == "yellow") elem.colorTexto = 6;
			else if (colorTexto == "white") elem.colorTexto = 7;
			else elem.colorTexto = 7;

			elem.colorFondo = 0;
			if (match[2].matched)
			{
				std::string colorFondo = match[2].str();
				if (colorFondo == "black") elem.colorFondo = 0;
				else if (colorFondo == "blue") elem.colorFondo = 1;
				else if (colorFondo == "green") elem.colorFondo = 2;
				else if (colorFondo == "cyan") elem.colorFondo = 3;
				else if (colorFondo == "red") elem.colorFondo = 4;
				else if (colorFondo == "magenta") elem.colorFondo = 5;
				else if (colorFondo == "yellow") elem.colorFondo = 6;
				else if (colorFondo == "white") elem.colorFondo = 7;
			}

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
	while (ejecutando)
	{
		// Verificaciones múltiples antes de renderizar
		if (bloqueado || pausado || operacionCritica || elementos.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		// Verificar actualización de archivo
		if (archivoModificado())
		{
			cargarDesdeHTML();
			actualizarBuffer();
		}

		// Renderizar solo si es completamente seguro
		try 
		{
			renderizarMarquesina();
		}
		catch (...)
		{
			// Si hay cualquier error, pausar brevemente y continuar
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			continue;
		}

		// Velocidad controlada
		std::this_thread::sleep_for(std::chrono::milliseconds(velocidad));
	}
}