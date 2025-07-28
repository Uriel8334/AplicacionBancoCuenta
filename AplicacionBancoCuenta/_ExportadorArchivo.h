#pragma once
#ifndef _EXPORTADORARCHIVO_H
#define _EXPORTADORARCHIVO_H

#define NOMINMAX
#include <string>
#include <fstream>

class Banco;
class Persona;

class ExportadorArchivo
{
public:
    /**
     * @brief Guarda todas las cuentas del banco en un archivo específico
     *
     * Genera un archivo con el nombre especificado y guarda los datos de las personas
     * y sus cuentas en un formato estructurado. Utiliza la ruta del escritorio del usuario.
     *
     * @param banco Referencia al objeto Banco que contiene las cuentas a guardar
     * @param nombreArchivo Nombre del archivo donde se guardarán los datos
	 */
    static void guardarCuentasEnArchivo(const Banco& banco, const std::string& nombreArchivo);

    /**
     * @brief Guarda todas las cuentas del banco en un archivo con nombre automático
     *
     * Valida la fecha del sistema antes de proceder a guardar el respaldo.
     * Genera un nombre de archivo basado en la fecha actual y guarda los datos
     * de las personas y sus cuentas en el archivo especificado.
     *
     * @param banco Referencia al objeto Banco que contiene las cuentas a guardar
	 */
    static void guardarCuentasEnArchivo(const Banco& banco);

    /**
     * @brief Carga las cuentas desde un archivo de respaldo
     *
     * Este método lee un archivo de respaldo previamente generado y carga las cuentas
     * en el banco. Se espera que el archivo tenga un formato específico.
     *
     * @param banco Referencia al objeto Banco donde se cargarán las cuentas
     * @param nombreArchivo Nombre del archivo desde donde se cargarán los datos
	 */
    static void cargarCuentasDesdeArchivo(Banco& banco, const std::string& nombreArchivo);

    /**
     * @brief Convierte un archivo de respaldo .bak a PDF
     *
     * Genera un archivo PDF a partir de un archivo de respaldo .bak utilizando
     * la herramienta wkhtmltopdf. Si la conversión falla, se genera un archivo HTML.
     *
     * @param nombreArchivo Nombre del archivo de respaldo (sin extensión)
	 * @return true si el PDF se generó correctamente, false en caso contrario
     */
    static bool archivoGuardadoHaciaPDF(const std::string& nombreArchivo);

    /**
     * @brief Obtiene la ruta del escritorio del usuario actual
     *
     * Utiliza la API de Windows para obtener la ruta del escritorio y crea un directorio
     * específico para almacenar los archivos de respaldo.
     *
     * @return String con la ruta completa al escritorio
	 */
    static std::string obtenerRutaEscritorio();

private:
    /**
    * @brief Guarda los datos de una persona en el archivo de respaldo
    *
    * Escribe la información de la persona, incluyendo sus cuentas de ahorro y corriente,
    * en el archivo proporcionado. Utiliza un formato estructurado para facilitar la lectura.
    *
    * @param archivo Referencia al archivo de salida abierto
    * @param p Puntero a la persona a guardar
    */
    static void guardarPersonaEnArchivo(std::ofstream& archivo, Persona* p);

    /**
     * @brief Procesa una persona desde el archivo y la agrega al banco
     *
     * Lee los datos de una persona desde el archivo, incluyendo sus cuentas de ahorro
     * y corriente, y los agrega a la lista de personas del banco.
     *
     * @param banco Referencia al objeto Banco donde se agregará la persona
     * @param archivo Referencia al archivo de entrada abierto
     * @param linea Referencia a la línea actual del archivo
	 */
    static void procesarPersona(Banco& banco, std::ifstream& archivo, std::string& linea);

    /**
     * @brief Limpia la lista de personas del banco
     * Elimina todas las personas y sus cuentas del banco, dejando la lista vacía.
     *
     * @param banco Referencia al objeto Banco que se desea limpiar
	 */
    static void limpiarBanco(Banco& banco);
};

#endif // _EXPORTADORARCHIVO_H
