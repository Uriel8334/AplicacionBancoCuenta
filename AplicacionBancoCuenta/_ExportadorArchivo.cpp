#define NOMINMAX
#include "_ExportadorArchivo.h"
#include "Banco.h"
#include "Persona.h"
#include "Fecha.h"
#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <comdef.h>
#include <shlobj.h>
#include <sstream>
#include <ctime>

/**
 * @brief Guarda las cuentas del banco en un archivo de respaldo
 *
 * Genera un archivo con el nombre especificado y guarda los datos de las personas
 * y sus cuentas en un formato estructurado. Utiliza la ruta del escritorio del usuario.
 *
 * @param banco Referencia al objeto Banco que contiene las cuentas a guardar
 * @param nombreArchivo Nombre del archivo donde se guardarán los datos
 */
void ExportadorArchivo::guardarCuentasEnArchivo(const Banco& banco, const std::string& nombreArchivo) {
    std::string rutaEscritorio = obtenerRutaEscritorio();
    std::string rutaCompleta = rutaEscritorio + nombreArchivo + ".bak";

    std::ofstream archivo(rutaCompleta, std::ios::out | std::ios::trunc);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo para guardar en: " << rutaCompleta << "\n";
        return;
    }

    archivo << "BANCO_BACKUP_V1.0\n";

    int contadorPersonas = 0;
    banco.forEachPersona([&](Persona* p) {
        if (p && p->isValidInstance()) {
            guardarPersonaEnArchivo(archivo, p);
            contadorPersonas++;
        }
        });

    archivo << "TOTAL_PERSONAS:" << contadorPersonas << "\n";
    archivo << "FIN_BACKUP\n";
    archivo.close();
    std::cout << "Respaldo guardado correctamente en " << rutaCompleta << "\n";
}

/**
 * @brief Guarda las cuentas del banco en un archivo de respaldo
 *
 * Valida la fecha del sistema antes de proceder a guardar el respaldo.
 * Genera un nombre de archivo basado en la fecha actual y guarda los datos
 * de las personas y sus cuentas en el archivo especificado.
 *
 * @param banco Referencia al objeto Banco que contiene las cuentas a guardar
 */ 
void ExportadorArchivo::guardarCuentasEnArchivo(const Banco& banco) {
    // Validar la fecha del sistema antes de guardar
    Fecha fechaActual;
    if (fechaActual.getEsFechaSistemaManipulada()) {
        std::cout << "Error: la fecha del sistema parece haber sido manipulada. No se guardara el respaldo.\n";
        return;
    }

    // Obtener fecha formateada
    std::string fechaFormateada = fechaActual.obtenerFechaFormateada();
    for (auto& c : fechaFormateada) {
        if (c == '/') c = '_';
    }

    // Obtener hora actual del sistema
    std::time_t t = std::time(nullptr);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char horaBuffer[9];
    std::strftime(horaBuffer, sizeof(horaBuffer), "%H_%M_%S", &tm);
    std::string horaFormateada(horaBuffer);

    // Construir nombre de archivo con fecha y hora
    std::string nombreArchivo = "Respaldo_" + fechaFormateada + "_" + horaFormateada;
    std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();
    std::string nombreArchivoUnico = Validar::generarNombreConIndice(rutaEscritorio, fechaFormateada + "_" + horaFormateada);
    std::string rutaCompleta = rutaEscritorio + nombreArchivoUnico + ".bak";

    std::ofstream archivo(rutaCompleta, std::ios::out | std::ios::trunc);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo para guardar en: " << rutaCompleta << "\n";
        return;
    }

    archivo << "BANCO_BACKUP_V1.0\n";

    int contadorPersonas = 0;
    banco.forEachPersona([&](Persona* p) {
        if (p && p->isValidInstance()) {
            ExportadorArchivo::guardarPersonaEnArchivo(archivo, p);
            contadorPersonas++;
        }
        });

    archivo << "TOTAL_PERSONAS:" << contadorPersonas << "\n";
    archivo << "FIN_BACKUP\n";
    archivo.close();
    std::cout << "Respaldo guardado correctamente en " << rutaCompleta << "\n";
}

/**
 * @brief Guarda los datos de una persona en el archivo de respaldo
 *
 * Escribe la información de la persona, incluyendo sus cuentas de ahorro y corriente,
 * en el archivo proporcionado. Utiliza un formato estructurado para facilitar la lectura.
 *
 * @param archivo Referencia al archivo de salida abierto
 * @param p Puntero a la persona a guardar
 */
void ExportadorArchivo::guardarPersonaEnArchivo(std::ofstream& archivo, Persona* p) {
    archivo << "===PERSONA_INICIO===\n";
    archivo << "CEDULA:" << p->getCedula() << "\n";
    archivo << "NOMBRES:" << p->getNombres() << "\n";
    archivo << "APELLIDOS:" << p->getApellidos() << "\n";
    archivo << "FECHA_NACIMIENTO:" << p->getFechaNacimiento() << "\n";
    archivo << "CORREO:" << p->getCorreo() << "\n";
    archivo << "DIRECCION:" << p->getDireccion() << "\n";

    archivo << "===CUENTAS_AHORRO_INICIO===\n";
    int cuentasAhorro = p->guardarCuentas(archivo, "AHORROS");
    archivo << "TOTAL_CUENTAS_AHORRO:" << cuentasAhorro << "\n";
    archivo << "===CUENTAS_AHORRO_FIN===\n";

    archivo << "===CUENTAS_CORRIENTE_INICIO===\n";
    int cuentasCorriente = p->guardarCuentas(archivo, "CORRIENTE");
    archivo << "TOTAL_CUENTAS_CORRIENTE:" << cuentasCorriente << "\n";
    archivo << "===CUENTAS_CORRIENTE_FIN===\n";

    archivo << "===PERSONA_FIN===\n\n";
}

/**
 * @brief Carga las cuentas desde un archivo de respaldo
 *
 * Este método lee un archivo de respaldo previamente generado y carga las cuentas
 * en el banco. Se espera que el archivo tenga un formato específico.
 *
 * @param banco Referencia al objeto Banco donde se cargarán las cuentas
 * @param nombreArchivo Nombre del archivo desde donde se cargarán los datos
 */ 
void ExportadorArchivo::cargarCuentasDesdeArchivo(Banco& banco, const std::string& nombreArchivo) {
    std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();
    std::string rutaCompleta = rutaEscritorio + nombreArchivo + ".bak";

    std::ifstream archivo(rutaCompleta);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo para cargar desde: " << rutaCompleta << "\n";
        return;
    }

    ExportadorArchivo::limpiarBanco(banco);

    std::string linea;
    std::getline(archivo, linea);
    if (linea != "BANCO_BACKUP_V1.0") {
        std::cout << "Formato de archivo invalido.\n";
        archivo.close();
        return;
    }

    while (std::getline(archivo, linea)) {
        if (linea == "===PERSONA_INICIO===") {
            ExportadorArchivo::procesarPersona(banco, archivo, linea);
        }
    }

    archivo.close();
    std::cout << "Cuentas cargadas correctamente desde el archivo.\n";
}

/**
 * @brief Limpia la lista de personas del banco
 * Elimina todas las personas y sus cuentas del banco, dejando la lista vacía.
 *
 * @param banco Referencia al objeto Banco que se desea limpiar
 */
void ExportadorArchivo::limpiarBanco(Banco& banco) {
    banco.setListaPersonas(nullptr);
}

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
void ExportadorArchivo::procesarPersona(Banco& banco, std::ifstream& archivo, std::string& linea) {
    std::unique_ptr<Persona> personaActual(new Persona());
    bool enPersona = true;
    CuentaAhorros* cuentaAhorrosTemp = nullptr;
    CuentaCorriente* cuentaCorrienteTemp = nullptr;
    std::string numCuenta, fechaApertura, estado;
    double saldo = 0.0;

    while (enPersona && std::getline(archivo, linea)) {
        if (linea == "===PERSONA_FIN===") {
            // Agregar persona al banco
            NodoPersona* nuevo = new NodoPersona(personaActual.release());
            nuevo->siguiente = banco.getListaPersonas();
            banco.setListaPersonas(nuevo);
            enPersona = false;
            break;
        }
        if (linea == "===CUENTAS_AHORRO_INICIO===") {
            while (std::getline(archivo, linea) && linea != "===CUENTAS_AHORRO_FIN===") {
                if (linea == "CUENTA_AHORROS_INICIO") {
                    cuentaAhorrosTemp = new CuentaAhorros();
                    numCuenta.clear(); fechaApertura.clear(); estado.clear(); saldo = 0.0;
                    while (std::getline(archivo, linea) && linea != "CUENTA_AHORROS_FIN") {
                        if (linea.find("NUMERO_CUENTA:") == 0)
                            numCuenta = linea.substr(14);
                        else if (linea.find("SALDO:") == 0)
                            saldo = std::stod(linea.substr(6));
                        else if (linea.find("FECHA_APERTURA:") == 0)
                            fechaApertura = linea.substr(15);
                        else if (linea.find("ESTADO:") == 0)
                            estado = linea.substr(7);
                    }
                    cuentaAhorrosTemp->setNumeroCuenta(numCuenta);
                    cuentaAhorrosTemp->setSaldo(saldo);
                    cuentaAhorrosTemp->setFechaApertura(fechaApertura);
                    cuentaAhorrosTemp->setEstadoCuenta(estado);
                    // Enlazar cuenta
                    if (!personaActual->getCabezaAhorros())
                        personaActual->setCabezaAhorros(cuentaAhorrosTemp);
                    else {
                        CuentaAhorros* actual = personaActual->getCabezaAhorros();
                        while (actual->getSiguiente())
                            actual = actual->getSiguiente();
                        actual->setSiguiente(cuentaAhorrosTemp);
                        cuentaAhorrosTemp->setAnterior(actual);
                    }
                }
            }
        }
        else if (linea == "===CUENTAS_CORRIENTE_INICIO===") {
            while (std::getline(archivo, linea) && linea != "===CUENTAS_CORRIENTE_FIN===") {
                if (linea == "CUENTA_CORRIENTE_INICIO") {
                    cuentaCorrienteTemp = new CuentaCorriente();
                    numCuenta.clear(); fechaApertura.clear(); estado.clear(); saldo = 0.0;
                    while (std::getline(archivo, linea) && linea != "CUENTA_CORRIENTE_FIN") {
                        if (linea.find("NUMERO_CUENTA:") == 0)
                            numCuenta = linea.substr(14);
                        else if (linea.find("SALDO:") == 0)
                            saldo = std::stod(linea.substr(6));
                        else if (linea.find("FECHA_APERTURA:") == 0)
                            fechaApertura = linea.substr(15);
                        else if (linea.find("ESTADO:") == 0)
                            estado = linea.substr(7);
                    }
                    cuentaCorrienteTemp->setNumeroCuenta(numCuenta);
                    cuentaCorrienteTemp->setSaldo(saldo);
                    cuentaCorrienteTemp->setFechaApertura(fechaApertura);
                    cuentaCorrienteTemp->setEstadoCuenta(estado);
                    // Enlazar cuenta
                    if (!personaActual->getCabezaCorriente())
                        personaActual->setCabezaCorriente(cuentaCorrienteTemp);
                    else {
                        CuentaCorriente* actual = personaActual->getCabezaCorriente();
                        while (actual->getSiguiente())
                            actual = actual->getSiguiente();
                        actual->setSiguiente(cuentaCorrienteTemp);
                        cuentaCorrienteTemp->setAnterior(actual);
                    }
                }
            }
        }
        else if (linea.find("CEDULA:") == 0) {
            personaActual->setCedula(linea.substr(7));
        }
        else if (linea.find("NOMBRES:") == 0) {
            personaActual->setNombres(linea.substr(8));
        }
        else if (linea.find("APELLIDOS:") == 0) {
            personaActual->setApellidos(linea.substr(10));
        }
        else if (linea.find("FECHA_NACIMIENTO:") == 0) {
            personaActual->setFechaNacimiento(linea.substr(17));
        }
        else if (linea.find("CORREO:") == 0) {
            personaActual->setCorreo(linea.substr(7));
        }
        else if (linea.find("DIRECCION:") == 0) {
            personaActual->setDireccion(linea.substr(10));
        }
    }
}

/**
 * @brief Convierte un archivo de respaldo .bak a formato PDF
 *
 * Este método lee un archivo de respaldo previamente generado, crea un
 * archivo HTML con formato mejorado y lo convierte a PDF usando wkhtmltopdf.
 *
 * @param nombreArchivo Nombre del archivo de respaldo (sin extensión)
 * @return bool true si la conversión fue exitosa, false en caso contrario
 */
bool ExportadorArchivo::archivoGuardadoHaciaPDF(const std::string& nombreArchivo) {
    std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();
    std::string rutaBak = rutaEscritorio + nombreArchivo + ".bak";
    std::string rutaHtml = rutaEscritorio + nombreArchivo + "_temp.html";
    std::string rutaPdf = rutaEscritorio + nombreArchivo + ".pdf";

    std::ifstream archivoEntrada(rutaBak);
    if (!archivoEntrada.is_open()) {
        std::cout << "No se pudo abrir el archivo de respaldo: " << rutaBak << std::endl;
        return false;
    }

    std::ofstream archivoHtml(rutaHtml);
    if (!archivoHtml.is_open()) {
        std::cout << "No se pudo crear el archivo HTML temporal" << std::endl;
        archivoEntrada.close();
        return false;
    }

    archivoHtml << "<!DOCTYPE html>\n<html>\n<head>\n";
    archivoHtml << "<meta charset=\"UTF-8\">\n";
    archivoHtml << "<title>Informe de Cuentas Bancarias</title>\n";
    archivoHtml << "<style>\n";
    archivoHtml << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
    archivoHtml << "h1, h2 { color: #003366; }\n";
    archivoHtml << "h1 { text-align: center; border-bottom: 2px solid #003366; padding-bottom: 10px; }\n";
    archivoHtml << "h2 { margin-top: 20px; border-bottom: 1px solid #ccc; }\n";
    archivoHtml << ".persona { background-color: #f9f9f9; border: 1px solid #ddd; margin: 15px 0; padding: 10px; border-radius: 5px; }\n";
    archivoHtml << ".cuenta { background-color: #eef6ff; margin: 10px 0; padding: 8px; border-left: 4px solid #003366; }\n";
    archivoHtml << ".cuenta-ahorro { border-left-color: #007700; }\n";
    archivoHtml << ".cuenta-corriente { border-left-color: #770000; }\n";
    archivoHtml << ".label { font-weight: bold; color: #555; min-width: 150px; display: inline-block; }\n";
    archivoHtml << ".total { font-weight: bold; margin-top: 20px; color: #003366; }\n";
    archivoHtml << "footer { text-align: center; margin-top: 30px; font-size: 0.8em; color: #777; }\n";
    archivoHtml << "</style>\n</head>\n<body>\n";

    Fecha fechaActual;
    archivoHtml << "<h1>Informe de Cuentas Bancarias</h1>\n";
    archivoHtml << "<p style='text-align: center;'>Generado el " << fechaActual.obtenerFechaFormateada() << "</p>\n";

    std::string linea;
    bool enPersona = false, enCuentasAhorro = false, enCuentasCorriente = false;
    bool enCuentaAhorro = false, enCuentaCorriente = false;
    int contadorPersonas = 0;
    int totalCuentasAhorro = 0, totalCuentasCorriente = 0;
    std::map<std::string, std::string> datosPersona;

    std::getline(archivoEntrada, linea);
    if (linea != "BANCO_BACKUP_V1.0") {
        archivoHtml << "<p style='color: red;'>Formato de archivo inválido.</p>\n";
        archivoHtml << "</body>\n</html>";
        archivoHtml.close();
        archivoEntrada.close();
        return false;
    }

    while (std::getline(archivoEntrada, linea)) {
        if (linea == "===PERSONA_INICIO===") {
            enPersona = true;
            datosPersona.clear();
            continue;
        }
        if (linea == "===PERSONA_FIN===") {
            if (enPersona) {
                archivoHtml << "<div class='persona'>\n";
                archivoHtml << "  <h2>Cliente: " << datosPersona["NOMBRES"] << " " << datosPersona["APELLIDOS"] << "</h2>\n";
                archivoHtml << "  <p><span class='label'>Cédula:</span> " << datosPersona["CEDULA"] << "</p>\n";
                archivoHtml << "  <p><span class='label'>Fecha Nacimiento:</span> " << datosPersona["FECHA_NACIMIENTO"] << "</p>\n";
                archivoHtml << "  <p><span class='label'>Correo:</span> " << datosPersona["CORREO"] << "</p>\n";
                archivoHtml << "  <p><span class='label'>Dirección:</span> " << datosPersona["DIRECCION"] << "</p>\n";
                archivoHtml << "</div>\n";
                contadorPersonas++;
            }
            enPersona = false;
            continue;
        }
        if (linea == "===CUENTAS_AHORRO_INICIO===") {
            enCuentasAhorro = true;
            archivoHtml << "<h3>Cuentas de Ahorro</h3>\n";
            continue;
        }
        if (linea == "===CUENTAS_AHORRO_FIN===") {
            enCuentasAhorro = false;
            continue;
        }
        if (linea == "===CUENTAS_CORRIENTE_INICIO===") {
            enCuentasCorriente = true;
            archivoHtml << "<h3>Cuentas Corrientes</h3>\n";
            continue;
        }
        if (linea == "===CUENTAS_CORRIENTE_FIN===") {
            enCuentasCorriente = false;
            continue;
        }
        if (linea == "CUENTA_AHORROS_INICIO") {
            enCuentaAhorro = true;
            archivoHtml << "<div class='cuenta cuenta-ahorro'>\n";
            continue;
        }
        if (linea == "CUENTA_AHORROS_FIN") {
            archivoHtml << "</div>\n";
            enCuentaAhorro = false;
            totalCuentasAhorro++;
            continue;
        }
        if (linea == "CUENTA_CORRIENTE_INICIO") {
            enCuentaCorriente = true;
            archivoHtml << "<div class='cuenta cuenta-corriente'>\n";
            continue;
        }
        if (linea == "CUENTA_CORRIENTE_FIN") {
            archivoHtml << "</div>\n";
            enCuentaCorriente = false;
            totalCuentasCorriente++;
            continue;
        }
        if (linea.substr(0, 14) == "TOTAL_PERSONAS:") {
            archivoHtml << "<div class='total'>Total de Clientes: " << contadorPersonas << "</div>\n";
            continue;
        }
        if (linea.substr(0, 21) == "TOTAL_CUENTAS_AHORRO:") {
            archivoHtml << "<div class='total'>Total de Cuentas de Ahorro: " << linea.substr(21) << "</div>\n";
            continue;
        }
        if (linea.substr(0, 24) == "TOTAL_CUENTAS_CORRIENTE:") {
            archivoHtml << "<div class='total'>Total de Cuentas Corrientes: " << linea.substr(24) << "</div>\n";
            continue;
        }
        if (linea == "FIN_BACKUP") {
            break;
        }

        // Procesar datos individuales
        if (enPersona) {
            size_t pos = linea.find(':');
            if (pos != std::string::npos) {
                std::string clave = linea.substr(0, pos);
                std::string valor = linea.substr(pos + 1);
                datosPersona[clave] = valor;
            }
        }

        // Procesar detalles de cuentas
        if (enCuentaAhorro || enCuentaCorriente) {
            size_t pos = linea.find(':');
            if (pos != std::string::npos) {
                std::string clave = linea.substr(0, pos);
                std::string valor = linea.substr(pos + 1);

                if (clave == "NUMERO_CUENTA") {
                    archivoHtml << "  <p><span class='label'>Número de Cuenta:</span> " << valor << "</p>\n";
                }
                else if (clave == "SALDO") {
                    double saldo = std::stod(valor);
                    archivoHtml << "  <p><span class='label'>Saldo:</span> $" << std::fixed << std::setprecision(2) << saldo << "</p>\n";
                }
                else if (clave == "FECHA_APERTURA") {
                    archivoHtml << "  <p><span class='label'>Fecha de Apertura:</span> " << valor << "</p>\n";
                }
                else if (clave == "ESTADO") {
                    archivoHtml << "  <p><span class='label'>Estado:</span> " << valor << "</p>\n";
                }
            }
        }
    }

    archivoHtml << "<footer>Este documento fue generado automáticamente por el sistema bancario.</footer>\n";
    archivoHtml << "</body>\n</html>";
    archivoHtml.close();
    archivoEntrada.close();

    std::string comando = "wkhtmltopdf \"" + rutaHtml + "\" \"" + rutaPdf + "\"";
    int resultado = system(comando.c_str());

    if (resultado != 0) {
        std::cout << "Error al convertir a PDF. Asegúrese de que wkhtmltopdf esté instalado." << std::endl;
        std::cout << "Puede descargar wkhtmltopdf desde: https://wkhtmltopdf.org/downloads.html" << std::endl;
        std::cout << "Se ha generado un archivo HTML en: " << rutaHtml << std::endl;
        return false;
    }

    if (remove(rutaHtml.c_str()) != 0) {
        std::cout << "Advertencia: No se pudo eliminar el archivo HTML temporal." << std::endl;
    }

    std::cout << "PDF generado correctamente: " << rutaPdf << std::endl;
    return true;
}

/**
 * @brief Obtiene la ruta del escritorio del usuario actual
 *
 * Utiliza la API de Windows para obtener la ruta del escritorio y crea un directorio
 * específico para almacenar los archivos de respaldo.
 *
 * @return String con la ruta completa al escritorio
 */ 
std::string ExportadorArchivo::obtenerRutaEscritorio() {
    PWSTR path = NULL;
    std::string rutaEscritorio = "";

    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &path))) {
        _bstr_t b(path);
        rutaEscritorio = (char*)b;
        CoTaskMemFree(path);
    }
    else {
        rutaEscritorio = ".";
    }

    if (!rutaEscritorio.empty() && rutaEscritorio.back() != '\\') {
        rutaEscritorio += '\\';
    }
    rutaEscritorio += "BancoApp\\";
    std::string comando = "if not exist \"" + rutaEscritorio + "\" mkdir \"" + rutaEscritorio + "\"";
    system(comando.c_str());

    return rutaEscritorio;
}