#pragma once
#ifndef CIFRADO_H
#define CIFRADO_H

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

/**
 * @class Cifrado
 * @brief Clase que implementa funcionalidades de cifrado y descifrado de archivos
 *
 * Esta clase estática proporciona métodos para cifrar y descifrar archivos utilizando
 * el algoritmo César. También contiene funcionalidades específicas para el manejo
 * de datos del banco, permitiendo cifrar, guardar, y recuperar información bancaria
 * de manera segura.
 */
class Cifrado {
public:
	/**
	 * @brief Cifra un archivo usando el algoritmo César
	 * @param rutaEntrada Ruta completa del archivo origen a cifrar
	 * @param rutaSalida Ruta completa donde se guardará el archivo cifrado
	 * @param clave Carácter usado como clave de cifrado para el algoritmo César
	 *
	 * Este método lee un archivo byte por byte, aplica una operación César con la clave
	 * especificada, y guarda el resultado en un nuevo archivo.
	 */
	static void CifrarArchivo(const std::string& rutaEntrada, const std::string& rutaSalida, char clave);

	/**
	 * @brief Descifra un archivo previamente cifrado con el algoritmo César
	 * @param rutaEntrada Ruta completa del archivo cifrado
	 * @param rutaSalida Ruta completa donde se guardará el archivo descifrado
	 * @param valor Carácter usado como clave de descifrado (debe ser el mismo usado para cifrar)
	 *
	 * Este método invierte el proceso de cifrado, aplicando César con la misma clave
	 * para recuperar el contenido original.
	 */
	static void desifrarArchivo(const std::string& rutaEntrada, const std::string& rutaSalida, char valor);

	/**
	 * @brief Cifra y guarda los datos del banco en un archivo binario
	 * @param banco Referencia al objeto Banco cuyos datos serán cifrados
	 * @param nombreArchivo Nombre base del archivo (sin extensión) donde guardar los datos
	 * @param claveCifrado Carácter usado como clave de cifrado
	 *
	 * Este método serializa los datos del banco a un archivo temporal, luego
	 * cifra este archivo y guarda el resultado como un archivo binario (.bin).
	 */
	static void cifrarYGuardarDatos(const class Banco& banco, const std::string& nombreArchivo, char claveCifrado);

	/**
	 * @brief Descifra y carga datos desde un archivo binario al objeto Banco
	 * @param banco Referencia al objeto Banco donde se cargarán los datos descifrados
	 * @param nombreArchivo Nombre base del archivo cifrado (sin extensión) a cargar
	 * @param claveCifrado Carácter usado como clave de descifrado
	 * @return true si el proceso fue exitoso, false en caso contrario
	 *
	 * Este método descifra el archivo binario especificado y carga los datos
	 * recuperados en el objeto Banco proporcionado.
	 */
	static bool descifrarYCargarDatos(class Banco& banco, const std::string& nombreArchivo, char claveCifrado);

	/**
	 * @brief Descifra un archivo sin cargar los datos al banco
	 * @param banco Referencia al objeto Banco (usado para obtener rutas)
	 * @param nombreArchivo Nombre base del archivo cifrado (sin extensión) a descifrar
	 * @param claveCifrado Carácter usado como clave de descifrado
	 * @param opcion Tipo de archivo de salida: 0 para .bak, 1 para .txt
	 * @return true si el proceso fue exitoso, false en caso contrario
	 *
	 * Este método descifra el archivo binario especificado y guarda el resultado
	 * en un nuevo archivo, sin modificar el estado del objeto Banco.
	 * La opción determina si se genera un archivo de backup (.bak) o texto (.txt).
	 */
	static bool descifrarSinCargarDatos(const Banco& banco, const std::string& nombreArchivo, char claveCifrado, int opcion);

	/**
	 * @brief Inicia el proceso interactivo de descifrado de archivos .bin
	 * @param baseDatos Referencia a la base de datos de personas
	 * @return true si el proceso fue exitoso, false en caso contrario
	 *
	 * Este método proporciona una interfaz interactiva para descifrar archivos .bin
	 * generados por ExportadorArchivo::guardarArchivoConCifrado. Aplica principios SOLID,
	 * usa programación funcional y recursión donde sea posible.
	 */
	static bool iniciarProcesoDescifrado(const class _BaseDatosPersona& baseDatos);

private:
	// === MÉTODOS AUXILIARES PARA EL PROCESO DE DESCIFRADO ===

	/**
	 * @brief Valida que existan archivos .bin disponibles para descifrar
	 * @return true si existen archivos .bin, false en caso contrario
	 */
	static bool validarArchivosDisponibles();

	/**
	 * @brief Solicita al usuario seleccionar un archivo .bin para descifrar
	 * @return Nombre del archivo seleccionado (sin extensión), vacío si cancela
	 */
	static std::string solicitarSeleccionArchivo();

	/**
	 * @brief Solicita al usuario la clave de descifrado
	 * @return Carácter de la clave de descifrado
	 */
	static char solicitarClaveDescifrado();

	/**
	 * @brief Solicita al usuario el formato de salida deseado
	 * @return Opción seleccionada: 0 para .bak, 1 para .txt, 2 para .pdf
	 */
	static int solicitarFormatoSalida();

	/**
	 * @brief Ejecuta el proceso de descifrado según el formato seleccionado
	 * @param nombreArchivo Nombre del archivo a descifrar
	 * @param clave Clave de descifrado
	 * @param formato Formato de salida (0=.bak, 1=.txt, 2=.pdf)
	 * @return true si el descifrado fue exitoso, false en caso contrario
	 */
	static bool ejecutarDescifradoPorFormato(const std::string& nombreArchivo, char clave, int formato);

	/**
	 * @brief Obtiene lista de archivos .bin disponibles en el directorio BancoApp
	 * @return Vector con los nombres de archivos .bin encontrados
	 */
	static std::vector<std::string> obtenerArchivosBINDisponibles();

	/**
	 * @brief Descifra y convierte archivo .bin a formato PDF usando funciones recursivas
	 * @param nombreArchivo Nombre del archivo .bin a procesar
	 * @param clave Clave de descifrado
	 * @return true si la conversión fue exitosa, false en caso contrario
	 */
	static bool descifrarYConvertirAPDF(const std::string& nombreArchivo, char clave);

	/**
	 * @brief Muestra el resultado del proceso de descifrado
	 * @param exito Indica si el proceso fue exitoso
	 * @param formato Formato de salida utilizado
	 * @param nombreArchivo Nombre del archivo procesado
	 */
	static void mostrarResultadoDescifrado(bool exito, int formato, const std::string& nombreArchivo);
};

#endif // CIFRADO_H