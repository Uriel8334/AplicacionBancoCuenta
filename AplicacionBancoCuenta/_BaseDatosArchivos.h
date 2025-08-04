#pragma once
#ifndef _BASEDATOSARCHIVOS_H  
#define _BASEDATOSARCHIVOS_H  


#include <string>

/*
* @file _BaseDatosArchivos.h
* @brief Clase para manejar la exportación e importación de bases de datos MongoDB a archivos
* Esta clase permite exportar colecciones de MongoDB a archivos JSON y restaurar datos desde archivos a MongoDB.
*/
class _BaseDatosArchivos  
{  
public:  

    /**  
     * @brief Selecciona una colección de MongoDB para exportar  
     *  
     * Muestra un menú para que el usuario seleccione la colección a exportar.  
     * Las opciones disponibles son "personas", "archivos" o "Cancelar".  
     *  
     * @return Nombre de la colección seleccionada o una cadena vacía si se cancela  
	 */
    std::string seleccionarColeccionParaExportar();

    /**  
     * @brief Muestra el menú de opciones para la base de datos  
     *  
     * Permite al usuario seleccionar entre exportar o importar la base de datos.  
	 */
    void mostrarMenuBaseDatos();

    /**  
     * @brief Exporta un backup de la base de datos MongoDB a un archivo  
     *  
     * Exporta los datos de una colección específica de MongoDB a un archivo JSON.  
     *  
     * @param uri URI de conexión a MongoDB (URI ->se refiere a Uniform Resource Identifier, o mas conocido como "mongodb+srv:<cluster-url>")  
     * @param db Nombre de la base de datos  
     * @param coleccion Nombre de la colección a exportar  
     */  
    static void exportarBackupMongoDB(const std::string& uri, const std::string& db, const std::string& coleccion);  

    /**  
     * @brief Restaura un backup desde un archivo hacia la base de datos MongoDB  
     *  
     * Restaura los datos desde un archivo JSON a la base de datos MongoDB.  
     *  
     * @param uri URI de conexión a MongoDB  
     * @param db Nombre de la base de datos  
     * @param nombreArchivo Ruta del archivo desde donde se restaurarán los datos  
     */  
    static void restaurarBackupMongoDB(const std::string& uri, const std::string& db, const std::string& nombreArchivo);  

};  
#endif //_BASEDATOSARCHIVOS_H
