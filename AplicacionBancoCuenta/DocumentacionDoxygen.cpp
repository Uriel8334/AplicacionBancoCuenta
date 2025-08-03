#include "DocumentacionDoxygen.h"
#include "Utilidades.h"
#include <conio.h>
#include <windows.h>
#include <shellapi.h>


void DocumentacionDoxygen::abrirDocumentacion()
{
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
	}
}