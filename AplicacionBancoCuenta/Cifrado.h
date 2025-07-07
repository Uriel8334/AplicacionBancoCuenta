#pragma once
#ifndef CIFRADO_H
#define CIFRADO_H

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

/**
 * @class Cifrado
 * @brief Clase que implementa funcionalidades de cifrado y descifrado de archivos
 *
 * Esta clase est�tica proporciona m�todos para cifrar y descifrar archivos utilizando
 * el algoritmo XOR. Tambi�n contiene funcionalidades espec�ficas para el manejo
 * de datos del banco, permitiendo cifrar, guardar, y recuperar informaci�n bancaria
 * de manera segura.
 */
class Cifrado {
public:
    /**
     * @brief Cifra un archivo usando el algoritmo XOR
     * @param rutaEntrada Ruta completa del archivo origen a cifrar
     * @param rutaSalida Ruta completa donde se guardar� el archivo cifrado
     * @param clave Car�cter usado como clave de cifrado para el algoritmo XOR
     *
     * Este m�todo lee un archivo byte por byte, aplica una operaci�n XOR con la clave
     * especificada, y guarda el resultado en un nuevo archivo.
     */
    static void CifrarArchivo(const std::string& rutaEntrada, const std::string& rutaSalida, char clave);

    /**
     * @brief Descifra un archivo previamente cifrado con el algoritmo XOR
     * @param rutaEntrada Ruta completa del archivo cifrado
     * @param rutaSalida Ruta completa donde se guardar� el archivo descifrado
     * @param valor Car�cter usado como clave de descifrado (debe ser el mismo usado para cifrar)
     *
     * Este m�todo invierte el proceso de cifrado, aplicando XOR con la misma clave
     * para recuperar el contenido original.
     */
    static void desifrarArchivo(const std::string& rutaEntrada, const std::string& rutaSalida, char valor);

    /**
     * @brief Cifra y guarda los datos del banco en un archivo binario
     * @param banco Referencia al objeto Banco cuyos datos ser�n cifrados
     * @param nombreArchivo Nombre base del archivo (sin extensi�n) donde guardar los datos
     * @param claveCifrado Car�cter usado como clave de cifrado
     *
     * Este m�todo serializa los datos del banco a un archivo temporal, luego
     * cifra este archivo y guarda el resultado como un archivo binario (.bin).
     */
    static void cifrarYGuardarDatos(const class Banco& banco, const std::string& nombreArchivo, char claveCifrado);

    /**
     * @brief Descifra y carga datos desde un archivo binario al objeto Banco
     * @param banco Referencia al objeto Banco donde se cargar�n los datos descifrados
     * @param nombreArchivo Nombre base del archivo cifrado (sin extensi�n) a cargar
     * @param claveCifrado Car�cter usado como clave de descifrado
     * @return true si el proceso fue exitoso, false en caso contrario
     *
     * Este m�todo descifra el archivo binario especificado y carga los datos
     * recuperados en el objeto Banco proporcionado.
     */
    static bool descifrarYCargarDatos(class Banco& banco, const std::string& nombreArchivo, char claveCifrado);

    /**
     * @brief Descifra un archivo sin cargar los datos al banco
     * @param banco Referencia al objeto Banco (usado para obtener rutas)
     * @param nombreArchivo Nombre base del archivo cifrado (sin extensi�n) a descifrar
     * @param claveCifrado Car�cter usado como clave de descifrado
     * @param opcion Tipo de archivo de salida: 0 para .bak, 1 para .txt
     * @return true si el proceso fue exitoso, false en caso contrario
     *
     * Este m�todo descifra el archivo binario especificado y guarda el resultado
     * en un nuevo archivo, sin modificar el estado del objeto Banco.
     * La opci�n determina si se genera un archivo de backup (.bak) o texto (.txt).
     */
    static bool descifrarSinCargarDatos(const Banco& banco, const std::string& nombreArchivo, char claveCifrado, int opcion);
};

#endif // CIFRADO_H