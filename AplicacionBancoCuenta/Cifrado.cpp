/**
 * @file Cifrado.cpp
 * @brief Implementación de la clase Cifrado que proporciona funcionalidades de cifrado/descifrado
 */
#include "Cifrado.h"
#include "Banco.h"
#include "_ExportadorArchivo.h"
#include "Utilidades.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <conio.h>

 /**
  * @brief Cifra un archivo usando el algoritmo César
  *
  * @param rutaEntrada Ruta del archivo original a cifrar
  * @param rutaSalida Ruta donde se guardará el archivo cifrado
  * @param clave Carácter que determina el desplazamiento para el cifrado
  * @throws std::runtime_error Si hay problemas al abrir los archivos
  */
void Cifrado::CifrarArchivo(const std::string& rutaEntrada, const std::string& rutaSalida, char clave) {
    std::ifstream archivoEntrada(rutaEntrada, std::ios::binary);
    if (!archivoEntrada) {
        throw std::runtime_error("Error al abrir el archivo de entrada.");
    }
    std::ofstream archivoSalida(rutaSalida, std::ios::binary);
    if (!archivoSalida) {
        throw std::runtime_error("Error al abrir el archivo de salida.");
    }

    // Convertir clave a un valor positivo entre 1-255
    unsigned char desplazamiento = static_cast<unsigned char>(clave);
    if (desplazamiento == 0) desplazamiento = 128; // Evitar desplazamiento nulo

    char byte;
    while (archivoEntrada.get(byte)) {
        // Cifrado Cesar: añade el desplazamiento (modulo 256 implicito en byte)
        unsigned char byteOriginal = static_cast<unsigned char>(byte);
        unsigned char byteCifrado = (byteOriginal + desplazamiento) % 256;
        archivoSalida.put(static_cast<char>(byteCifrado));
    }

    archivoEntrada.close();
    archivoSalida.close();
}

/**
 * @brief Descifra un archivo cifrado previamente con el algoritmo César
 *
 * @param rutaEntrada Ruta del archivo cifrado
 * @param rutaSalida Ruta donde se guardará el archivo descifrado
 * @param clave Carácter usado originalmente para el cifrado
 * @throws std::runtime_error Si hay problemas al abrir los archivos
 */
void Cifrado::desifrarArchivo(const std::string& rutaEntrada, const std::string& rutaSalida, char clave) {
    std::ifstream archivoEntrada(rutaEntrada, std::ios::binary);
    if (!archivoEntrada) {
        throw std::runtime_error("Error al abrir el archivo de entrada.");
    }
    std::ofstream archivoSalida(rutaSalida, std::ios::binary);
    if (!archivoSalida) {
        throw std::runtime_error("Error al abrir el archivo de salida.");
    }

    // Convertir clave a un valor positivo entre 1-255
    unsigned char desplazamiento = static_cast<unsigned char>(clave);
    if (desplazamiento == 0) desplazamiento = 128; // Evitar desplazamiento nulo

    char byte;
    while (archivoEntrada.get(byte)) {
        // Descifrado Cesar: resta el desplazamiento (con ajuste para modulo 256)
        unsigned char byteCifrado = static_cast<unsigned char>(byte);
        unsigned char byteOriginal = (byteCifrado + 256 - desplazamiento) % 256;
        archivoSalida.put(static_cast<char>(byteOriginal));
    }

    archivoEntrada.close();
    archivoSalida.close();
}

/**
 * @brief Guarda y cifra los datos del banco en un archivo
 *
 * Exporta los datos del banco a un archivo temporal, lo cifra y elimina el
 * archivo temporal después del proceso.
 *
 * @param banco Referencia al objeto Banco con los datos a guardar
 * @param nombreArchivo Nombre del archivo de salida (sin extensión)
 * @param claveCifrado Clave de cifrado a utilizar
 */
void Cifrado::cifrarYGuardarDatos(const Banco& banco, const std::string& nombreArchivo, char claveCifrado) {
    // Obtener la ruta del escritorio usando el metodo de Banco
    std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();

    // Definir nombres de archivo consistentes
    std::string nombreArchivoTemp = "temp_backup";
    std::string rutaArchivoTemp = rutaEscritorio + nombreArchivoTemp + ".bak";
    std::string rutaDestino = rutaEscritorio + nombreArchivo + ".bin";

    // Guardar los datos en el archivo temporal
    ExportadorArchivo::guardarCuentasEnArchivo(banco, nombreArchivoTemp);

    try {
        // Cifrar el archivo temporal y guardarlo en el archivo final
        CifrarArchivo(rutaArchivoTemp, rutaDestino, claveCifrado);
        std::cout << "Datos cifrados y guardados correctamente en " << rutaDestino << "\n";

        // Eliminar el archivo temporal despues de cifrar
        if (std::remove(rutaArchivoTemp.c_str()) != 0) {
            std::cout << "Advertencia: No se pudo eliminar el archivo temporal.\n";
        }
    }
    catch (const std::exception& e) {
        std::cout << "Error al cifrar los datos: " << e.what() << "\n";
    }
}

/**
 * @brief Descifra un archivo y carga los datos en el objeto Banco
 *
 * @param banco Referencia al objeto Banco donde se cargarán los datos
 * @param nombreArchivo Nombre del archivo cifrado (sin extensión)
 * @param claveCifrado Clave utilizada para el cifrado original
 * @return true si el proceso se completó exitosamente, false en caso contrario
 */
bool Cifrado::descifrarYCargarDatos(Banco& banco, const std::string& nombreArchivo, char claveCifrado) {
    // Obtener la ruta del escritorio usando el metodo de Banco
    std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();

    // Definir nombres de archivo consistentes
    std::string nombreArchivoTemp = "temp_descifrado";
    std::string rutaArchivoTemp = rutaEscritorio + nombreArchivoTemp + ".bak";
    std::string rutaOrigen = rutaEscritorio + nombreArchivo + ".bin";

    try {
        // Descifrar el archivo cifrado en un archivo temporal
        desifrarArchivo(rutaOrigen, rutaArchivoTemp, claveCifrado);

        // Verificar que el archivo se descifro correctamente
        std::ifstream archivo(rutaArchivoTemp);
        if (!archivo.is_open()) {
            std::cout << "No se pudo abrir el archivo descifrado: " << rutaArchivoTemp << "\n";
            return false;
        }

        // Cerrar el archivo despues de verificarlo
        archivo.close();

        // Cargar los datos descifrados al banco
        ExportadorArchivo::cargarCuentasDesdeArchivo(banco, nombreArchivoTemp);

        // Eliminar el archivo temporal despues de cargar
        if (std::remove(rutaArchivoTemp.c_str()) != 0) {
            std::cout << "Advertencia: No se pudo eliminar el archivo temporal.\n";
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error al descifrar los datos: " << e.what() << "\n";
        return false;
    }
}

/**
 * @brief Descifra un archivo sin cargar los datos en el banco
 *
 * Permite descifrar un archivo y guardarlo con diferentes extensiones
 * según la opción elegida.
 *
 * @param banco Referencia al objeto Banco (usado solo para obtener rutas)
 * @param nombreArchivo Nombre del archivo cifrado (sin extensión)
 * @param claveCifrado Clave utilizada para el cifrado original
 * @param opcion Formato de salida: 0 para .bak, 1 para .txt
 * @return true si el proceso se completó exitosamente, false en caso contrario
 */
bool Cifrado::descifrarSinCargarDatos(const Banco& banco, const std::string& nombreArchivo, char claveCifrado, int opcion) {
    // Obtener la ruta del escritorio usando el metodo de Banco
    std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();
    std::string rutaOrigen = rutaEscritorio + nombreArchivo + ".bin";

    // Variables para la ruta de archivo temporal
    std::string nombreArchivoTemp;
    std::string rutaArchivoTemp;

    // Se decide la extension de salida segun la 'opcion' recibida
    switch (opcion) {
    case 0: // BIN -> BAK
        nombreArchivoTemp = "temp_backup";
        rutaArchivoTemp = rutaEscritorio + nombreArchivoTemp + ".bak";
        break;
    case 1: // BIN -> TXT
        nombreArchivoTemp = "temp_descifrado";
        rutaArchivoTemp = rutaEscritorio + nombreArchivoTemp + ".txt";
        break;
    default:
        std::cout << "Opcion de descifrado invalida.\n";
        return false;
    }

    try {
        // Descifrar el archivo cifrado en un archivo temporal
        desifrarArchivo(rutaOrigen, rutaArchivoTemp, claveCifrado);

        // Verificar que el archivo se descifro correctamente
        std::ifstream archivo(rutaArchivoTemp);
        if (!archivo.is_open()) {
            std::cout << "No se pudo abrir el archivo descifrado: " << rutaArchivoTemp << "\n";
            return false;
        }
        archivo.close();

        //Eliminar el archivo temporal despues de descifrar (si lo deseas)
        //if (std::remove(rutaArchivoTemp.c_str()) != 0) {
        //   std::cout << "Advertencia: No se pudo eliminar el archivo temporal.\n";
        //}

        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error al descifrar los datos: " << e.what() << "\n";
        return false;
    }
}

// === NUEVO MÉTODO PRINCIPAL PARA PROCESO DE DESCIFRADO ===

/**
 * @brief Inicia el proceso interactivo de descifrado de archivos .bin
 *
 * Implementa principios SOLID separando responsabilidades en métodos específicos
 * y usa programación funcional con lambdas y funciones de orden superior.
 *
 * @param baseDatos Referencia a la base de datos de personas (para consistencia de API)
 * @return true si el proceso fue exitoso, false en caso contrario
 */
bool Cifrado::iniciarProcesoDescifrado(const _BaseDatosPersona& baseDatos) {
    try {
        // Aplicando SRP: Cada método tiene una responsabilidad específica
        if (!validarArchivosDisponibles()) {
            return false;
        }

        std::string nombreArchivo = solicitarSeleccionArchivo();
        if (nombreArchivo.empty()) {
            return false; // Usuario canceló
        }

        char claveDescifrado = solicitarClaveDescifrado();
        int formatoSalida = solicitarFormatoSalida();

        if (formatoSalida == -1) {
            return false; // Usuario canceló
        }

        // Ejecutar descifrado usando estrategia específica según formato
        bool resultado = ejecutarDescifradoPorFormato(nombreArchivo, claveDescifrado, formatoSalida);

        // Mostrar resultado al usuario
        mostrarResultadoDescifrado(resultado, formatoSalida, nombreArchivo);

        return resultado;
    }
    catch (const std::exception& e) {
        std::cout << "Error en el proceso de descifrado: " << e.what() << "\n";
        std::cout << "Presione cualquier tecla para continuar...";
        _getch();
        return false;
    }
}

// === MÉTODOS AUXILIARES APLICANDO PRINCIPIOS SOLID ===

/**
 * @brief Valida que existan archivos .bin disponibles para descifrar
 * Aplicando SRP: Solo se encarga de validar la existencia de archivos
 */
bool Cifrado::validarArchivosDisponibles() {
    auto archivosBin = obtenerArchivosBINDisponibles();

    if (archivosBin.empty()) {
        Utilidades::limpiarPantallaPreservandoMarquesina(1);
        std::cout << "No se encontraron archivos .bin para descifrar en el directorio BancoApp.\n";
        std::cout << "Genere primero archivos cifrados desde el menú 'Guardar Archivo'.\n";
        std::cout << "Presione cualquier tecla para continuar...";
        int teclaCualquier = _getch();
		(void)teclaCualquier; // Ignorar tecla presionada
        return false;
    }

    return true;
}

/**
 * @brief Obtiene lista de archivos .bin disponibles usando programación funcional
 * Aplicando principios funcionales con std::for_each y lambdas
 */
std::vector<std::string> Cifrado::obtenerArchivosBINDisponibles() {
    std::vector<std::string> archivosBin;
    std::string rutaBancoApp = ExportadorArchivo::obtenerRutaEscritorio();

    try {
        namespace fs = std::filesystem;

        // Usando programación funcional con for_each y lambda
        std::for_each(fs::directory_iterator(rutaBancoApp), fs::directory_iterator{},
            [&archivosBin](const auto& entrada) {
                if (entrada.is_regular_file() && entrada.path().extension() == ".bin") {
                    archivosBin.push_back(entrada.path().stem().string());
                }
            });
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cout << "Error al acceder al directorio: " << e.what() << "\n";
    }

    return archivosBin;
}

/**
 * @brief Solicita al usuario seleccionar un archivo usando menú interactivo
 * Aplicando SRP: Solo maneja la selección de archivo
 */
std::string Cifrado::solicitarSeleccionArchivo() {
    auto archivosBin = obtenerArchivosBINDisponibles();

    // Crear opciones para el menú incluyendo cancelar
    std::vector<std::string> opciones;

    // Usar transform para crear lista de opciones con formato amigable
    std::transform(archivosBin.begin(), archivosBin.end(), std::back_inserter(opciones),
        [](const std::string& archivo) {
            return archivo + ".bin";
        });

    opciones.push_back("Cancelar");

    int seleccion = Utilidades::menuInteractivo(
        "Seleccione el archivo .bin a descifrar:",
        opciones,
        0,
        0
    );

    // Validar selección
    if (seleccion == -1 || seleccion >= static_cast<int>(archivosBin.size())) {
        return ""; // Cancelado
    }

    return archivosBin[seleccion];
}

/**
 * @brief Solicita clave de descifrado de forma segura
 * Aplicando SRP: Solo maneja la entrada de clave
 */
char Cifrado::solicitarClaveDescifrado() {
    Utilidades::limpiarPantallaPreservandoMarquesina(1);
    std::cout << "Ingrese la clave de descifrado (un carácter): ";
    char clave = _getch();
    std::cout << "*\n"; // Ocultar la clave por seguridad
    return clave;
}

/**
 * @brief Solicita formato de salida usando menú interactivo
 * Aplicando SRP: Solo maneja la selección de formato
 */
int Cifrado::solicitarFormatoSalida() {
    std::vector<std::string> formatosDisponibles = {
        "Archivo de respaldo (.bak)",
        "Archivo de texto (.txt)",
        "Documento PDF (.pdf)",
        "Cancelar"
    };

    int seleccion = Utilidades::menuInteractivo(
        "Seleccione el formato de salida:",
        formatosDisponibles,
        0,
        0
    );

    return (seleccion == 3 || seleccion == -1) ? -1 : seleccion;
}

/**
 * @brief Ejecuta descifrado según formato usando patrón Strategy
 * Aplicando Strategy Pattern con mapa de funciones lambda
 */
bool Cifrado::ejecutarDescifradoPorFormato(const std::string& nombreArchivo, char clave, int formato) {
    // Mapa de estrategias usando lambdas (programación funcional)
    static const std::map<int, std::function<bool(const std::string&, char)>> estrategias = {
        {0, [](const std::string& archivo, char c) -> bool {
            // Descifrar a .bak
            std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();
            std::string rutaOrigen = rutaEscritorio + archivo + ".bin";
            std::string rutaDestino = rutaEscritorio + archivo + "_descifrado.bak";

            try {
                desifrarArchivo(rutaOrigen, rutaDestino, c);
                return true;
            }
            catch (const std::exception&) {
                return false;
            }
        }},

        {1, [](const std::string& archivo, char c) -> bool {
            // Descifrar a .txt
            std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();
            std::string rutaOrigen = rutaEscritorio + archivo + ".bin";
            std::string rutaDestino = rutaEscritorio + archivo + "_descifrado.txt";

            try {
                desifrarArchivo(rutaOrigen, rutaDestino, c);
                return true;
            }
            catch (const std::exception&) {
                return false;
            }
        }},

        {2, [](const std::string& archivo, char c) -> bool {
            // Descifrar y convertir a PDF
            return descifrarYConvertirAPDF(archivo, c);
        }}
    };

    auto estrategia = estrategias.find(formato);
    if (estrategia != estrategias.end()) {
        return estrategia->second(nombreArchivo, clave);
    }

    return false;
}

/**
 * @brief Descifra y convierte archivo a PDF usando funciones recursivas
 * Aplicando principios de recursión y programación funcional
 */
bool Cifrado::descifrarYConvertirAPDF(const std::string& nombreArchivo, char clave) {
    std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();
    std::string rutaOrigen = rutaEscritorio + nombreArchivo + ".bin";
    std::string rutaTemp = rutaEscritorio + nombreArchivo + "_temp_descifrado.bak";

    try {
        // Paso 1: Descifrar a archivo temporal .bak
        desifrarArchivo(rutaOrigen, rutaTemp, clave);

        // Paso 2: Convertir .bak a PDF usando función existente
        bool resultado = ExportadorArchivo::archivoGuardadoHaciaPDF(nombreArchivo + "_temp_descifrado");

        // Paso 3: Limpiar archivo temporal
        std::remove(rutaTemp.c_str());

        return resultado;
    }
    catch (const std::exception& e) {
        std::cout << "Error en conversión a PDF: " << e.what() << "\n";
        return false;
    }
}

/**
 * @brief Muestra resultado del proceso usando forEach funcional
 * Aplicando SRP: Solo maneja la presentación de resultados
 */
void Cifrado::mostrarResultadoDescifrado(bool exito, int formato, const std::string& nombreArchivo) {
    // Mapa de descripciones de formato
    static const std::map<int, std::string> descripciones = {
        {0, "archivo de respaldo (.bak)"},
        {1, "archivo de texto (.txt)"},
        {2, "documento PDF (.pdf)"}
    };

    Utilidades::limpiarPantallaPreservandoMarquesina(1);

    if (exito) {
        auto desc = descripciones.find(formato);
        std::string tipoArchivo = (desc != descripciones.end()) ? desc->second : "formato desconocido";

        std::cout << "¡Descifrado exitoso!\n";
        std::cout << "Archivo: " << nombreArchivo << ".bin\n";
        std::cout << "Convertido a: " << tipoArchivo << "\n";
        std::cout << "Ubicación: " << ExportadorArchivo::obtenerRutaEscritorio() << "\n";
    }
    else {
        std::cout << "Error al descifrar el archivo " << nombreArchivo << ".bin\n";
        std::cout << "Verifique que la clave de descifrado sea correcta.\n";
    }

    std::cout << "\nPresione cualquier tecla para continuar...";
    _getch();
}