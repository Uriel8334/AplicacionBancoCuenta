/**
 * @file Cifrado.cpp
 * @brief Implementación de la clase Cifrado que proporciona funcionalidades de cifrado/descifrado
 */
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "Cifrado.h"
#include "Banco.h"

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
    std::string rutaEscritorio = banco.obtenerRutaEscritorio();

    // Definir nombres de archivo consistentes
    std::string nombreArchivoTemp = "temp_backup";
    std::string rutaArchivoTemp = rutaEscritorio + nombreArchivoTemp + ".bak";
    std::string rutaDestino = rutaEscritorio + nombreArchivo + ".bin";

    // Guardar los datos en el archivo temporal
    banco.guardarCuentasEnArchivo(nombreArchivoTemp);

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
    std::string rutaEscritorio = banco.obtenerRutaEscritorio();

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
        banco.cargarCuentasDesdeArchivo(nombreArchivoTemp);

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
    std::string rutaEscritorio = banco.obtenerRutaEscritorio();
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
