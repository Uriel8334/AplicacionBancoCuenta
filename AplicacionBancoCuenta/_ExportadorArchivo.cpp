#define _CRT_SECURE_NO_WARNINGS

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
#include <conio.h>
#include "NodoPersona.h"
#include "CodigoQR.h"
#include "_BaseDatosPersona.h"
#include "ConexionMongo.h"
#include "Utilidades.h"
#include <functional>
#include <algorithm>


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

// Agregar estas líneas después de los includes existentes
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>

// === NUEVOS MÉTODOS IMPLEMENTADOS ===

/**
 * @brief Guarda archivos múltiples desde la base de datos MongoDB con diferentes extensiones
 */
bool ExportadorArchivo::guardarArchivosVarios(const _BaseDatosPersona& baseDatos, int tipoArchivo,
	const std::string& nombreArchivo, char claveCifrado) {
	try {
		// Aplicando SRP: Separar responsabilidades en métodos específicos
		return std::invoke([&]() -> bool {
			switch (tipoArchivo) {
			case 0: return guardarDesdeBaseDatos(baseDatos, nombreArchivo, ".bak");
			case 1: return guardarArchivoConCifrado(baseDatos, nombreArchivo, claveCifrado);
			case 2: return generarPDFDesdeBaseDatos(baseDatos, nombreArchivo);
			default: return false;
			}
			});
	}
	catch (const std::exception& e) {
		std::cerr << "Error en guardarArchivosVarios: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Guarda datos desde la base de datos en formato específico
 */
bool ExportadorArchivo::guardarDesdeBaseDatos(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo, const std::string& extension) {
	std::string rutaEscritorio = obtenerRutaEscritorio();
	std::string rutaCompleta = rutaEscritorio + nombreArchivo + extension;

	std::ofstream archivo(rutaCompleta, std::ios::out | std::ios::trunc);
	if (!archivo.is_open()) {
		std::cout << "No se pudo crear el archivo: " << rutaCompleta << "\n";
		return false;
	}

	// Escribir cabecera del archivo
	archivo << "BANCO_BACKUP_V2.0_FROM_MONGODB\n";

	// Crear instancia temporal para llamar al método no estático
	auto& clienteDB = ConexionMongo::obtenerClienteBaseDatos();
	_BaseDatosPersona baseDatosTemp(clienteDB);
	auto todasPersonas = baseDatosTemp.mostrarTodasPersonas();

	// Aplicando programación funcional con forEach
	std::for_each(todasPersonas.begin(), todasPersonas.end(),
		[&archivo](const auto& personaDoc) {
			procesarPersonaRecursivamente(personaDoc, archivo);
		});

	archivo << "TOTAL_PERSONAS:" << todasPersonas.size() << "\n";
	archivo << "FIN_BACKUP\n";
	archivo.close();

	std::cout << "Archivo " << extension << " guardado correctamente en " << rutaCompleta << "\n";
	return true;
}

/**
 * @brief Procesa una persona de forma recursiva desde documento MongoDB
 */
void ExportadorArchivo::procesarPersonaRecursivamente(const bsoncxx::document::value& personaDoc, std::ofstream& archivo) {
	auto view = personaDoc.view();

	// Escribir datos de la persona
	archivo << "===PERSONA_INICIO===\n";
	escribirCampoPersona(archivo, "CEDULA", view["cedula"]);
	escribirCampoPersona(archivo, "NOMBRES", view["nombre"]);
	escribirCampoPersona(archivo, "APELLIDOS", view["apellido"]);
	escribirCampoPersona(archivo, "FECHA_NACIMIENTO", view["fechaNacimiento"]);
	escribirCampoPersona(archivo, "CORREO", view["correo"]);
	escribirCampoPersona(archivo, "DIRECCION", view["direccion"]);

	// Procesar cuentas si existen
	if (view.find("cuentas") != view.end() && view["cuentas"].type() == bsoncxx::type::k_array) {
		auto cuentasArray = view["cuentas"].get_array().value;
		procesarCuentasBSON(cuentasArray, archivo);
	}

	archivo << "===PERSONA_FIN===\n\n";
}

/**
 * @brief Escribe un campo de persona en el archivo - Compatible con mongocxx 3.11
 */
void ExportadorArchivo::escribirCampoPersona(std::ofstream& archivo, const std::string& nombreCampo, const bsoncxx::document::element& elemento) {
	// Verificar que el elemento existe y es de tipo string
	if (elemento && elemento.type() == bsoncxx::type::k_utf8) {
		archivo << nombreCampo << ":" << std::string(elemento.get_string().value) << "\n";
	}
}

/**
 * @brief Procesa cuentas desde BSON usando recursión - Compatible con mongocxx 3.11
 */
void ExportadorArchivo::procesarCuentasBSON(const bsoncxx::array::view& cuentasArray, std::ofstream& archivo) {
	// Usar funciones lambda para separar por tipo de cuenta
	auto procesarPorTipo = [&archivo](const bsoncxx::array::view& cuentas, const std::string& tipo, const std::string& valorTipo) {
		archivo << "===CUENTAS_" << tipo << "_INICIO===\n";

		std::for_each(cuentas.begin(), cuentas.end(), [&archivo, &tipo, &valorTipo](const auto& cuenta) {
			if (cuenta.type() == bsoncxx::type::k_document) {
				auto cuentaView = cuenta.get_document().value;
				auto tipoCuentaElement = cuentaView["tipo"];

				if (tipoCuentaElement && tipoCuentaElement.type() == bsoncxx::type::k_utf8) {
					std::string tipoCuenta = std::string(tipoCuentaElement.get_string().value);

					if ((tipo == "AHORRO" && tipoCuenta == "ahorros") ||
						(tipo == "CORRIENTE" && tipoCuenta == "corriente")) {
						archivo << "CUENTA_" << tipo << "S_INICIO\n";

						auto numCuentaElement = cuentaView["numeroCuenta"];
						if (numCuentaElement && numCuentaElement.type() == bsoncxx::type::k_utf8) {
							archivo << "NUMERO_CUENTA:" << std::string(numCuentaElement.get_string().value) << "\n";
						}

						auto saldoElement = cuentaView["saldo"];
						archivo << "SALDO:" << std::fixed << std::setprecision(2);
						if (saldoElement) {
							if (saldoElement.type() == bsoncxx::type::k_double) {
								archivo << saldoElement.get_double().value;
							}
							else if (saldoElement.type() == bsoncxx::type::k_int32) {
								archivo << static_cast<double>(saldoElement.get_int32().value);
							}
							else if (saldoElement.type() == bsoncxx::type::k_int64) {
								archivo << static_cast<double>(saldoElement.get_int64().value);
							}
						}
						archivo << "\n";

						auto fechaElement = cuentaView["fechaApertura"];
						if (fechaElement && fechaElement.type() == bsoncxx::type::k_utf8) {
							archivo << "FECHA_APERTURA:" << std::string(fechaElement.get_string().value) << "\n";
						}

						auto estadoElement = cuentaView["estado"];
						if (estadoElement && estadoElement.type() == bsoncxx::type::k_utf8) {
							archivo << "ESTADO:" << std::string(estadoElement.get_string().value) << "\n";
						}

						archivo << "CUENTA_" << tipo << "S_FIN\n";
					}
				}
			}
			});

		archivo << "===CUENTAS_" << tipo << "_FIN===\n";
		};

	procesarPorTipo(cuentasArray, "AHORRO", "ahorros");
	procesarPorTipo(cuentasArray, "CORRIENTE", "corriente");
}

/**
 * @brief Guarda archivo con cifrado César
 */
bool ExportadorArchivo::guardarArchivoConCifrado(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo, char claveCifrado) {
	// Primero crear archivo temporal .bak
	std::string archivoTemporal = nombreArchivo + "_temp";
	if (!guardarDesdeBaseDatos(baseDatos, archivoTemporal, ".bak")) {
		return false;
	}

	// Aplicar cifrado César y guardar como .bin
	std::string rutaEscritorio = obtenerRutaEscritorio();
	std::string rutaTemporal = rutaEscritorio + archivoTemporal + ".bak";
	std::string rutaCifrada = rutaEscritorio + nombreArchivo + ".bin";

	std::ifstream archivoOrigen(rutaTemporal);
	std::ofstream archivoCifrado(rutaCifrada, std::ios::binary);

	if (!archivoOrigen.is_open() || !archivoCifrado.is_open()) {
		return false;
	}

	// Cifrado César usando función lambda
	auto cifrarCaracter = [claveCifrado](char c) -> char {
		return static_cast<char>(c + static_cast<int>(claveCifrado));
		};

	std::string linea;
	while (std::getline(archivoOrigen, linea)) {
		std::transform(linea.begin(), linea.end(), linea.begin(), cifrarCaracter);
		archivoCifrado << linea << "\n";
	}

	archivoOrigen.close();
	archivoCifrado.close();

	// Eliminar archivo temporal
	std::remove(rutaTemporal.c_str());

	std::cout << "Archivo cifrado .bin guardado correctamente\n";
	return true;
}

/**
 * @brief Genera PDF desde la base de datos con códigos QR
 */
bool ExportadorArchivo::generarPDFDesdeBaseDatos(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo) {
	// Primero crear archivo .bak desde la base de datos
	if (!guardarDesdeBaseDatos(baseDatos, nombreArchivo, ".bak")) {
		return false;
	}

	// Usar la función existente modificada para incluir QR
	return archivoGuardadoHaciaPDFConQR(nombreArchivo, baseDatos);
}

/**
 * @brief Versión modificada de archivoGuardadoHaciaPDF que incluye códigos QR
 */
bool ExportadorArchivo::archivoGuardadoHaciaPDFConQR(const std::string& nombreArchivo, const _BaseDatosPersona& baseDatos) {
	std::string rutaEscritorio = obtenerRutaEscritorio();
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

	// Escribir cabecera HTML con estilos mejorados para QR
	escribirCabeceraHTML(archivoHtml);

	std::string linea;
	std::map<std::string, std::string> datosPersona;
	EstadoProcesamiento estado = {};

	// Validar formato del archivo
	std::getline(archivoEntrada, linea);
	if (linea != "BANCO_BACKUP_V2.0_FROM_MONGODB" && linea != "BANCO_BACKUP_V1.0") {
		archivoHtml << "<p style='color: red;'>Formato de archivo inválido.</p>\n";
		finalizarHTML(archivoHtml);
		archivoEntrada.close();
		return false;
	}

	// Procesar líneas usando función recursiva
	procesarLineasPDFRecursivamente(archivoEntrada, archivoHtml, datosPersona, estado);

	finalizarHTML(archivoHtml);
	archivoEntrada.close();

	return convertirHTMLaPDF(rutaHtml, rutaPdf);
}

/**
 * @brief Procesa líneas del archivo recursivamente para generar PDF
 */
void ExportadorArchivo::procesarLineasPDFRecursivamente(std::ifstream& archivo, std::ofstream& archivoHtml,
	std::map<std::string, std::string>& datosPersona, EstadoProcesamiento& estado) {

	std::string linea;
	while (std::getline(archivo, linea)) {
		// Usar mapa de funciones lambda para procesar diferentes tipos de líneas
		static const std::map<std::string, std::function<void(std::ofstream&, std::map<std::string, std::string>&, EstadoProcesamiento&)>> procesadores = {
			{"===PERSONA_INICIO===", [](std::ofstream&, std::map<std::string, std::string>& datos, EstadoProcesamiento& est) {
				est.enPersona = true;
				datos.clear();
			}},
			{"===PERSONA_FIN===", [](std::ofstream& html, std::map<std::string, std::string>& datos, EstadoProcesamiento& est) {
				if (est.enPersona) {
					escribirPersonaConQR(html, datos);
					est.contadorPersonas++;
				}
				est.enPersona = false;
			}},
			{"===CUENTAS_AHORRO_INICIO===", [](std::ofstream& html, std::map<std::string, std::string>&, EstadoProcesamiento& est) {
				est.enCuentasAhorro = true;
				html << "<h3>Cuentas de Ahorro</h3>\n";
			}},
			{"===CUENTAS_AHORRO_FIN===", [](std::ofstream&, std::map<std::string, std::string>&, EstadoProcesamiento& est) {
				est.enCuentasAhorro = false;
			}},
			{"===CUENTAS_CORRIENTE_INICIO===", [](std::ofstream& html, std::map<std::string, std::string>&, EstadoProcesamiento& est) {
				est.enCuentasCorriente = true;
				html << "<h3>Cuentas Corrientes</h3>\n";
			}},
			{"===CUENTAS_CORRIENTE_FIN===", [](std::ofstream&, std::map<std::string, std::string>&, EstadoProcesamiento& est) {
				est.enCuentasCorriente = false;
			}}
		};

		// Buscar y ejecutar procesador correspondiente
		auto procesador = procesadores.find(linea);
		if (procesador != procesadores.end()) {
			procesador->second(archivoHtml, datosPersona, estado);
			continue;
		}

		// Procesar otros tipos de líneas
		procesarLineaEspecifica(linea, archivoHtml, datosPersona, estado);
	}
}

/**
 * @brief Procesa líneas específicas para generar PDF con QR
 * @param linea Línea actual del archivo
 * @param archivoHtml Archivo HTML de salida
 * @param datosPersona Datos de la persona actual
 * @param estado Estado del procesamiento
 */
void ExportadorArchivo::procesarLineaEspecifica(const std::string& linea, std::ofstream& archivoHtml,
	std::map<std::string, std::string>& datosPersona, EstadoProcesamiento& estado) {

	// Variable estática para almacenar los datos de la cuenta actual
	static std::map<std::string, std::string> cuentaActual;

	// Manejar marcadores de cuentas individuales
	if (linea == "CUENTA_AHORROS_INICIO") {
		estado.enCuentaAhorro = true;
		cuentaActual.clear();
		archivoHtml << "<div class='cuenta cuenta-ahorro'>\n";
		archivoHtml << "<div class='cuenta-info'>\n";
		return;
	}
	if (linea == "CUENTA_AHORROS_FIN") {
		// Generar QR para esta cuenta antes de cerrar
		if (!cuentaActual.empty() && !datosPersona.empty()) {
			std::string qrData = generarQRPersona(
				datosPersona["CEDULA"],
				datosPersona["NOMBRES"],
				datosPersona["APELLIDOS"],
				cuentaActual["NUMERO_CUENTA"]
			);
			archivoHtml << "</div>\n"; // Cierra cuenta-info
			archivoHtml << "<div class='cuenta-qr'>\n";
			archivoHtml << "  <h5>QR Cuenta</h5>\n";
			archivoHtml << "  <div class='qr-code'>" << qrData << "</div>\n";
			archivoHtml << "</div>\n"; // Cierra cuenta-qr
		}
		else {
			archivoHtml << "</div>\n"; // Solo cierra cuenta-info si no hay QR
		}
		archivoHtml << "</div>\n"; // Cierra cuenta
		estado.enCuentaAhorro = false;
		estado.totalCuentasAhorro++;
		return;
	}
	if (linea == "CUENTA_CORRIENTE_INICIO") {
		estado.enCuentaCorriente = true;
		cuentaActual.clear();
		archivoHtml << "<div class='cuenta cuenta-corriente'>\n";
		archivoHtml << "<div class='cuenta-info'>\n";
		return;
	}
	if (linea == "CUENTA_CORRIENTE_FIN") {
		// Generar QR para esta cuenta antes de cerrar
		if (!cuentaActual.empty() && !datosPersona.empty()) {
			std::string qrData = generarQRPersona(
				datosPersona["CEDULA"],
				datosPersona["NOMBRES"],
				datosPersona["APELLIDOS"],
				cuentaActual["NUMERO_CUENTA"]
			);
			archivoHtml << "</div>\n"; // Cierra cuenta-info
			archivoHtml << "<div class='cuenta-qr'>\n";
			archivoHtml << "  <h5>QR Cuenta</h5>\n";
			archivoHtml << "  <div class='qr-code'>" << qrData << "</div>\n";
			archivoHtml << "</div>\n"; // Cierra cuenta-qr
		}
		else {
			archivoHtml << "</div>\n"; // Solo cierra cuenta-info si no hay QR
		}
		archivoHtml << "</div>\n"; // Cierra cuenta
		estado.enCuentaCorriente = false;
		estado.totalCuentasCorriente++;
		return;
	}

	// Manejar totales
	if (linea.substr(0, 14) == "TOTAL_PERSONAS:") {
		archivoHtml << "<div class='total'>Total de Clientes: " << estado.contadorPersonas << "</div>\n";
		return;
	}
	if (linea.substr(0, 21) == "TOTAL_CUENTAS_AHORRO:") {
		archivoHtml << "<div class='total'>Total de Cuentas de Ahorro: " << linea.substr(21) << "</div>\n";
		return;
	}
	if (linea.substr(0, 24) == "TOTAL_CUENTAS_CORRIENTE:") {
		archivoHtml << "<div class='total'>Total de Cuentas Corrientes: " << linea.substr(24) << "</div>\n";
		return;
	}
	if (linea == "FIN_BACKUP") {
		return;
	}

	// Procesar datos individuales
	size_t pos = linea.find(':');
	if (pos != std::string::npos) {
		std::string clave = linea.substr(0, pos);
		std::string valor = linea.substr(pos + 1);

		if (estado.enPersona && !estado.enCuentasAhorro && !estado.enCuentasCorriente) {
			datosPersona[clave] = valor;
		}
		else if (estado.enCuentaAhorro || estado.enCuentaCorriente) {
			// Guardar datos de cuenta para el QR
			cuentaActual[clave] = valor;

			// Mostrar detalles de cuentas
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

/**
 * @brief Finaliza el archivo HTML con pie de página
 * @param archivoHtml Archivo HTML de salida
 * @param datosPersona Datos de la persona actual
 */
void ExportadorArchivo::escribirPersonaConQR(std::ofstream& archivoHtml, const std::map<std::string, std::string>& datosPersona) {
	archivoHtml << "<div class='persona'>\n";
	archivoHtml << "<div class='persona-info'>\n";

	auto nombres = datosPersona.find("NOMBRES");
	auto apellidos = datosPersona.find("APELLIDOS");
	auto cedula = datosPersona.find("CEDULA");
	auto fechaNac = datosPersona.find("FECHA_NACIMIENTO");
	auto correo = datosPersona.find("CORREO");
	auto direccion = datosPersona.find("DIRECCION");

	if (nombres != datosPersona.end() && apellidos != datosPersona.end()) {
		archivoHtml << "  <h2>Cliente: " << nombres->second << " " << apellidos->second << "</h2>\n";
	}
	if (cedula != datosPersona.end()) {
		archivoHtml << "  <p><span class='label'>Cédula:</span> " << cedula->second << "</p>\n";
	}
	if (fechaNac != datosPersona.end()) {
		archivoHtml << "  <p><span class='label'>Fecha Nacimiento:</span> " << fechaNac->second << "</p>\n";
	}
	if (correo != datosPersona.end()) {
		archivoHtml << "  <p><span class='label'>Correo:</span> " << correo->second << "</p>\n";
	}
	if (direccion != datosPersona.end()) {
		archivoHtml << "  <p><span class='label'>Dirección:</span> " << direccion->second << "</p>\n";
	}

	archivoHtml << "</div>\n"; // Cierra persona-info

	// Generar QR general del cliente (sin número de cuenta específico)
	if (cedula != datosPersona.end() && nombres != datosPersona.end() && apellidos != datosPersona.end()) {
		std::string qrData = generarQRPersona(cedula->second, nombres->second, apellidos->second, "CLIENTE");

		archivoHtml << "<div class='qr-container'>\n";
		archivoHtml << "  <h4>QR Cliente</h4>\n";
		archivoHtml << "  <div class='qr-code'>" << qrData << "</div>\n";
		archivoHtml << "</div>\n";
	}

	archivoHtml << "</div>\n"; // Cierra persona
}

/**
 * @brief Genera código QR para una persona usando Utilidades - CORREGIDO
 */
std::string ExportadorArchivo::generarQRPersona(const std::string& cedula, const std::string& nombres,
	const std::string& apellidos, const std::string& numeroCuenta) {
	try {
		// Crear objeto Persona temporal
		Persona personaTemp;
		personaTemp.setCedula(cedula);
		personaTemp.setNombres(nombres);
		personaTemp.setApellidos(apellidos);

		// Usar la nueva función que solo genera el QR sin mostrarlo
		std::string qrTexto = Utilidades::generarQRSoloMostrar(personaTemp, numeroCuenta);

		if (!qrTexto.empty()) {
			return qrTexto;
		}
		else {
			return "Error al generar código QR";
		}
	}
	catch (const std::exception& e) {
		return "Error: " + std::string(e.what());
	}
}

/**
 * @brief Escribe cabecera HTML con estilos para QR
 */
void ExportadorArchivo::escribirCabeceraHTML(std::ofstream& archivoHtml) {
	archivoHtml << "<!DOCTYPE html>\n<html>\n<head>\n";
	archivoHtml << "<meta charset=\"UTF-8\">\n";
	archivoHtml << "<title>Informe de Cuentas Bancarias</title>\n";
	archivoHtml << "<style>\n";
	archivoHtml << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
	archivoHtml << "h1, h2 { color: #003366; }\n";
	archivoHtml << "h1 { text-align: center; border-bottom: 2px solid #003366; padding-bottom: 10px; }\n";
	archivoHtml << "h2 { margin-top: 20px; border-bottom: 1px solid #ccc; }\n";
	archivoHtml << ".persona { background-color: #f9f9f9; border: 1px solid #ddd; margin: 15px 0; padding: 10px; border-radius: 5px; display: flex; justify-content: space-between; }\n";
	archivoHtml << ".persona-info { flex: 1; }\n";
	archivoHtml << ".qr-container { flex: 0 0 200px; margin-left: 20px; text-align: center; }\n";
	archivoHtml << ".qr-code { font-family: monospace; font-size: 8px; line-height: 8px; background-color: white; padding: 10px; border: 1px solid #ccc; white-space: pre; }\n";
	archivoHtml << ".cuenta { background-color: #eef6ff; margin: 10px 0; padding: 8px; border-left: 4px solid #003366; display: flex; justify-content: space-between; }\n";
	archivoHtml << ".cuenta-info { flex: 1; }\n";
	archivoHtml << ".cuenta-qr { flex: 0 0 150px; margin-left: 15px; text-align: center; }\n";
	archivoHtml << ".cuenta-ahorro { border-left-color: #007700; }\n";
	archivoHtml << ".cuenta-corriente { border-left-color: #770000; }\n";
	archivoHtml << ".label { font-weight: bold; color: #555; min-width: 150px; display: inline-block; }\n";
	archivoHtml << ".total { font-weight: bold; margin-top: 20px; color: #003366; }\n";
	archivoHtml << "footer { text-align: center; margin-top: 30px; font-size: 0.8em; color: #777; }\n";
	archivoHtml << "</style>\n</head>\n<body>\n";

	Fecha fechaActual;
	archivoHtml << "<h1>Informe de Cuentas Bancarias</h1>\n";
	archivoHtml << "<p style='text-align: center;'>Generado el " << fechaActual.obtenerFechaFormateada() << "</p>\n";
}

/**
 * @brief Finaliza el archivo HTML
 */
void ExportadorArchivo::finalizarHTML(std::ofstream& archivoHtml) {
	archivoHtml << "<footer>Este documento fue generado automáticamente por el sistema bancario.</footer>\n";
	archivoHtml << "</body>\n</html>";
	archivoHtml.close();
}

/**
 * @brief Convierte HTML a PDF usando wkhtmltopdf
 */
bool ExportadorArchivo::convertirHTMLaPDF(const std::string& rutaHtml, const std::string& rutaPdf) {
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
 * @brief Procesa una persona desde datos BSON de MongoDB
 */
bool ExportadorArchivo::procesarPersonaDesdeBSON(const bsoncxx::document::value& personaDoc, std::ofstream& archivo) {
	try {
		auto view = personaDoc.view();

		// Escribir datos de persona en formato tradicional
		archivo << "===PERSONA_INICIO===\n";
		escribirCampoPersona(archivo, "CEDULA", view["cedula"]);
		escribirCampoPersona(archivo, "NOMBRES", view["nombre"]);
		escribirCampoPersona(archivo, "APELLIDOS", view["apellido"]);
		escribirCampoPersona(archivo, "FECHA_NACIMIENTO", view["fechaNacimiento"]);
		escribirCampoPersona(archivo, "CORREO", view["correo"]);
		escribirCampoPersona(archivo, "DIRECCION", view["direccion"]);

		// Procesar cuentas si existen
		if (view.find("cuentas") != view.end() && view["cuentas"].type() == bsoncxx::type::k_array) {
			auto cuentasArray = view["cuentas"].get_array().value;
			procesarCuentasBSON(cuentasArray, archivo);
		}

		archivo << "===PERSONA_FIN===\n\n";
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error procesando BSON: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Modifica procesarPersona para manejar datos BSON
 */
void ExportadorArchivo::procesarPersona(Banco& banco, std::ifstream& archivo, std::string& linea) {
	// Código original para archivos .bak tradicionales
	procesarPersonaTradicional(banco, archivo, linea);
}

/**
 * @brief Procesa una persona de forma tradicional (código original)
 */
void ExportadorArchivo::procesarPersonaTradicional(Banco& banco, std::ifstream& archivo, std::string& linea) {
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
 * @brief Estrategia para respaldo en formato .bak
 */
class ExportadorArchivo::EstrategiaRespaldoBD : public IEstrategiaGuardado {
public:
	bool ejecutar(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo) override {
		return ExportadorArchivo::guardarDesdeBaseDatos(baseDatos, nombreArchivo, ".bak");
	}

	std::string obtenerExtension() const override { return ".bak"; }
	std::string obtenerDescripcion() const override { return "Respaldo desde BD"; }
};

/**
 * @brief Estrategia para archivo cifrado
 */
class ExportadorArchivo::EstrategiaCifrado : public IEstrategiaGuardado {
private:
	char claveCifrado;

public:
	explicit EstrategiaCifrado(char clave) : claveCifrado(clave) {}

	bool ejecutar(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo) override {
		return ExportadorArchivo::guardarArchivoConCifrado(baseDatos, nombreArchivo, claveCifrado);
	}

	std::string obtenerExtension() const override { return ".bin"; }
	std::string obtenerDescripcion() const override { return "Archivo cifrado desde BD"; }
};

/**
 * @brief Estrategia para PDF con código QR
 */
class ExportadorArchivo::EstrategiaPDFConQR : public IEstrategiaGuardado {
public:
	bool ejecutar(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo) override {
		return ExportadorArchivo::generarPDFDesdeBaseDatos(baseDatos, nombreArchivo);
	}

	std::string obtenerExtension() const override { return ".pdf"; }
	std::string obtenerDescripcion() const override { return "Archivo PDF desde BD con QR"; }
};

// === IMPLEMENTACIÓN DEL FACTORY ===

std::unique_ptr<IEstrategiaGuardado> FabricaEstrategiasGuardado::crear(int tipoGuardado, char claveCifrado) {
	switch (tipoGuardado) {
	case 0: return std::make_unique<ExportadorArchivo::EstrategiaRespaldoBD>();
	case 1: return std::make_unique<ExportadorArchivo::EstrategiaCifrado>(claveCifrado);
	case 2: return std::make_unique<ExportadorArchivo::EstrategiaPDFConQR>();
	default: return nullptr;
	}
}

std::vector<std::string> FabricaEstrategiasGuardado::obtenerOpcionesDisponibles() {
	return {
		"Respaldo desde BD (.bak)",
		"Archivo cifrado desde BD (.bin)",
		"Archivo PDF desde BD con QR",
		"Cancelar"
	};
}

// === IMPLEMENTACIÓN DEL GESTOR ===

GestorGuardadoArchivos::GestorGuardadoArchivos(const _BaseDatosPersona& bd)
	: baseDatos(bd), estrategia(nullptr) {
}

bool GestorGuardadoArchivos::configurarEstrategia(int tipoGuardado, char claveCifrado) {
	estrategia = FabricaEstrategiasGuardado::crear(tipoGuardado, claveCifrado);
	return estrategia != nullptr;
}

bool GestorGuardadoArchivos::ejecutarGuardado(const std::string& nombreArchivo) {
	if (!estrategia) {
		std::cerr << "Error: No se ha configurado una estrategia de guardado.\n";
		return false;
	}

	try {
		return estrategia->ejecutar(baseDatos, nombreArchivo);
	}
	catch (const std::exception& e) {
		std::cerr << "Error durante el guardado: " << e.what() << std::endl;
		return false;
	}
}

std::vector<std::string> GestorGuardadoArchivos::obtenerOpcionesGuardado() const {
	return FabricaEstrategiasGuardado::obtenerOpcionesDisponibles();
}

bool GestorGuardadoArchivos::validarDatosDisponibles() const {
	// Crear una instancia temporal no-const para llamar al método no estático
	// Esto es necesario porque existenPersonasEnBaseDatos() no es const ni estático
	_BaseDatosPersona& baseDatosMutable = const_cast<_BaseDatosPersona&>(baseDatos);
	return baseDatosMutable.existenPersonasEnBaseDatos();
}

// === MÉTODOS PRINCIPALES REFACTORIZADOS ===

bool ExportadorArchivo::procesarSolicitudGuardado(const _BaseDatosPersona& baseDatos) {
	// Aplicando Single Responsibility Principle
	if (!ExportadorArchivo::validarDatosEnBaseDatos(baseDatos)) {
		return false;
	}

	Utilidades::limpiarPantallaPreservandoMarquesina(1);

	// Crear gestor con Dependency Injection
	GestorGuardadoArchivos gestor(baseDatos);

	// Solicitar tipo de guardado
	int tipoGuardado = ExportadorArchivo::solicitarTipoGuardado();
	if (tipoGuardado == 3 || tipoGuardado == -1) { // Cancelar
		return false;
	}

	// Solicitar nombre del archivo
	std::string nombreArchivo = ExportadorArchivo::solicitarNombreArchivo();
	if (nombreArchivo.empty()) {
		return false;
	}

	// Configurar clave de cifrado si es necesario
	char claveCifrado = '\0';
	if (tipoGuardado == 1) { // Archivo cifrado
		claveCifrado = ExportadorArchivo::solicitarClaveParaCifrado();
	}

	// Configurar estrategia
	if (!gestor.configurarEstrategia(tipoGuardado, claveCifrado)) {
		std::cout << "Error: Tipo de guardado no válido.\n";
		return false;
	}

	// Ejecutar guardado
	bool resultado = gestor.ejecutarGuardado(nombreArchivo);

	// Mostrar resultado
	auto opciones = FabricaEstrategiasGuardado::obtenerOpcionesDisponibles();
	std::string tipoOperacion = (tipoGuardado < static_cast<int>(opciones.size())) ? opciones[tipoGuardado] : "Desconocido";
	ExportadorArchivo::mostrarResultado(resultado, tipoOperacion);

	return resultado;
}

// === MÉTODOS DE INTERFAZ DE USUARIO ===

bool ExportadorArchivo::validarDatosEnBaseDatos(const _BaseDatosPersona& baseDatos) {
	// Crear una instancia temporal no-const para llamar al método no estático
	_BaseDatosPersona& baseDatosMutable = const_cast<_BaseDatosPersona&>(baseDatos);

	if (!baseDatosMutable.existenPersonasEnBaseDatos()) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "No hay datos para guardar en la base de datos. Registre personas primero.\n";
		std::cout << "Presione cualquier tecla para continuar...";
		int teclaCualquiera = _getch();
		(void)teclaCualquiera; 
		return false;
	}
	return true;
}

int ExportadorArchivo::solicitarTipoGuardado() {
	auto opciones = FabricaEstrategiasGuardado::obtenerOpcionesDisponibles();
	return Utilidades::menuInteractivo("Seleccione el tipo de guardado:", opciones, 0, 0);
}

std::string ExportadorArchivo::solicitarNombreArchivo() {
	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "Ingrese el nombre del archivo (sin extension): ";
	std::string nombreArchivo;
	std::getline(std::cin, nombreArchivo);

	// Limpiar buffer si es necesario
	if (std::cin.peek() == '\n') {
		std::cin.ignore();
	}

	return nombreArchivo;
}

char ExportadorArchivo::solicitarClaveParaCifrado() {
	std::cout << "Ingrese una clave para cifrar (un solo caracter): ";
	char clave = _getch();
	std::cout << "*\n"; // Ocultar la clave
	return clave;
}

void ExportadorArchivo::mostrarResultado(bool exito, const std::string& tipoOperacion) {
	if (exito) {
		std::cout << "\nArchivo guardado correctamente desde la base de datos (" << tipoOperacion << ").\n";
	}
	else {
		std::cout << "\nError al guardar el archivo (" << tipoOperacion << ").\n";
	}

	std::cout << "Presione cualquier tecla para continuar...";
	int teclaCualquiera = _getch();
	(void)teclaCualquiera;
}

/**
 * @brief Interfaz Strategy para diferentes tipos de recuperación
 * Aplicando Open/Closed Principle
 */
class IEstrategiaRecuperacion {
public:
	virtual ~IEstrategiaRecuperacion() = default;
	virtual bool ejecutar(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo) = 0;
	virtual std::string obtenerExtension() const = 0;
	virtual std::string obtenerDescripcion() const = 0;
};

/**
 * @brief Estrategia para recuperación desde archivo .bak
 */
class ExportadorArchivo::EstrategiaRecuperacionBAK : public IEstrategiaRecuperacion {
public:
	bool ejecutar(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo) override {
		return ExportadorArchivo::recuperarDesdeRespaldo(baseDatos, nombreArchivo);
	}

	std::string obtenerExtension() const override { return ".bak"; }
	std::string obtenerDescripcion() const override { return "Recuperar desde respaldo"; }
};

/**
 * @brief Estrategia para recuperación desde archivo cifrado .bin
 */
class ExportadorArchivo::EstrategiaRecuperacionBIN : public IEstrategiaRecuperacion {
private:
	char claveDescifrado;

public:
	explicit EstrategiaRecuperacionBIN(char clave) : claveDescifrado(clave) {}

	bool ejecutar(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo) override {
		return ExportadorArchivo::recuperarDesdeCifrado(baseDatos, nombreArchivo, claveDescifrado);
	}

	std::string obtenerExtension() const override { return ".bin"; }
	std::string obtenerDescripcion() const override { return "Recuperar desde archivo cifrado"; }
};

/**
 * @brief Factory para crear estrategias de recuperación
 */
class FabricaEstrategiasRecuperacion {
public:
	static std::unique_ptr<IEstrategiaRecuperacion> crear(int tipoRecuperacion, char claveDescifrado = '\0') {
		switch (tipoRecuperacion) {
		case 0: return std::make_unique<ExportadorArchivo::EstrategiaRecuperacionBAK>();
		case 1: return std::make_unique<ExportadorArchivo::EstrategiaRecuperacionBIN>(claveDescifrado);
		default: return nullptr;
		}
	}

	static std::vector<std::string> obtenerOpcionesDisponibles() {
		return {
			"Recuperar de Respaldo (.bak)",
			"Recuperar de Archivo cifrado (.bin)",
			"Cancelar"
		};
	}
};

/**
 * @brief Gestor principal de recuperación de archivos
 * Aplicando Single Responsibility Principle
 */
class GestorRecuperacionArchivos {
private:
	const _BaseDatosPersona& baseDatos;
	std::unique_ptr<IEstrategiaRecuperacion> estrategia;

public:
	explicit GestorRecuperacionArchivos(const _BaseDatosPersona& bd) : baseDatos(bd), estrategia(nullptr) {}

	bool configurarEstrategia(int tipoRecuperacion, char claveDescifrado = '\0') {
		estrategia = FabricaEstrategiasRecuperacion::crear(tipoRecuperacion, claveDescifrado);
		return estrategia != nullptr;
	}

	bool ejecutarRecuperacion(const std::string& nombreArchivo) {
		if (!estrategia) {
			std::cerr << "Error: No se ha configurado una estrategia de recuperación.\n";
			return false;
		}

		try {
			return estrategia->ejecutar(baseDatos, nombreArchivo);
		}
		catch (const std::exception& e) {
			std::cerr << "Error durante la recuperación: " << e.what() << std::endl;
			return false;
		}
	}

	std::vector<std::string> obtenerOpcionesRecuperacion() const {
		return FabricaEstrategiasRecuperacion::obtenerOpcionesDisponibles();
	}
};

/**
 * @brief Procesa solicitud de recuperación de archivos desde MongoDB
 * Similar en estructura a exportarBackupMongoDB
 */
bool ExportadorArchivo::procesarSolicitudRecuperacion(const _BaseDatosPersona& baseDatos) {
	try {
		// Crear gestor con Dependency Injection
		GestorRecuperacionArchivos gestor(baseDatos);

		Utilidades::limpiarPantallaPreservandoMarquesina(1);

		// Solicitar tipo de recuperación
		int tipoRecuperacion = ExportadorArchivo::solicitarTipoRecuperacion();
		if (tipoRecuperacion == 2 || tipoRecuperacion == -1) { // Cancelar
			return false;
		}

		// Solicitar nombre del archivo
		std::string nombreArchivo = ExportadorArchivo::solicitarNombreArchivoRecuperacion();
		if (nombreArchivo.empty()) {
			return false;
		}

		// Configurar clave de descifrado si es necesario
		char claveDescifrado = '\0';
		if (tipoRecuperacion == 1) { // Archivo cifrado
			claveDescifrado = ExportadorArchivo::solicitarClaveDescifrado();
		}

		// Configurar estrategia
		if (!gestor.configurarEstrategia(tipoRecuperacion, claveDescifrado)) {
			std::cout << "Error: Tipo de recuperación no válido.\n";
			return false;
		}

		// Ejecutar recuperación
		bool resultado = gestor.ejecutarRecuperacion(nombreArchivo);

		// Mostrar resultado
		auto opciones = FabricaEstrategiasRecuperacion::obtenerOpcionesDisponibles();
		std::string tipoOperacion = (tipoRecuperacion < static_cast<int>(opciones.size())) ? opciones[tipoRecuperacion] : "Desconocido";
		ExportadorArchivo::mostrarResultadoRecuperacion(resultado, tipoOperacion);

		return resultado;
	}
	catch (const std::exception& e) {
		std::cerr << "Error en procesarSolicitudRecuperacion: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Recupera datos desde archivo .bak y los carga en MongoDB
 * Siguiendo estructura similar a exportarBackupMongoDB
 */
bool ExportadorArchivo::recuperarDesdeRespaldo(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo) {
	// Obtener la ruta del escritorio del usuario (similar a exportarBackupMongoDB)
	std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();
	std::string rutaCompleta = rutaEscritorio + nombreArchivo + ".bak";

	std::ifstream archivo(rutaCompleta);
	if (!archivo.is_open()) {
		std::cout << "No se pudo abrir el archivo para recuperar desde: " << rutaCompleta << "\n";
		return false;
	}

	// Validar formato del archivo (similar a la validación en exportarBackupMongoDB)
	std::string linea;
	std::getline(archivo, linea);

	if (linea != "BANCO_BACKUP_V2.0_FROM_MONGODB" && linea != "BANCO_BACKUP_V1.0") {
		std::cout << "Formato de archivo inválido. Se esperaba formato de backup válido.\n";
		archivo.close();
		return false;
	}

	try {
		// Crear instancia temporal para operaciones (similar a exportarBackupMongoDB)
		auto& clienteDB = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona baseDatosTemp(clienteDB);

		// Procesar archivo línea por línea y cargar en MongoDB
		bool resultado = procesarArchivoRecuperacion(archivo, baseDatosTemp);

		archivo.close();

		if (resultado) {
			std::cout << "Datos recuperados correctamente desde " << rutaCompleta << "\n";
		}
		else {
			std::cout << "Error al recuperar algunos datos desde el archivo.\n";
		}

		return resultado;
	}
	catch (const std::exception& e) {
		std::cout << "Error durante la recuperación: " << e.what() << "\n";
		archivo.close();
		return false;
	}
}

/**
 * @brief Recupera datos desde archivo cifrado .bin y los carga en MongoDB
 * Usando funciones de Cifrado.cpp
 */
bool ExportadorArchivo::recuperarDesdeCifrado(const _BaseDatosPersona& baseDatos, const std::string& nombreArchivo, char claveDescifrado) {
	try {
		// Primero descifrar el archivo usando las funciones de Cifrado.cpp
		std::string rutaEscritorio = ExportadorArchivo::obtenerRutaEscritorio();
		std::string archivoTemporal = nombreArchivo + "_temp_descifrado";
		std::string rutaOrigen = rutaEscritorio + nombreArchivo + ".bin";
		std::string rutaTemporal = rutaEscritorio + archivoTemporal + ".bak";

		// Usar función de descifrado de Cifrado.cpp
		try {
			Cifrado::desifrarArchivo(rutaOrigen, rutaTemporal, claveDescifrado);
		}
		catch (const std::exception& e) {
			std::cout << "Error al descifrar el archivo: " << e.what() << "\n";
			return false;
		}

		// Verificar que el archivo se descifró correctamente
		std::ifstream archivoDescifrado(rutaTemporal);
		if (!archivoDescifrado.is_open()) {
			std::cout << "No se pudo abrir el archivo descifrado.\n";
			return false;
		}

		// Validar formato del archivo descifrado
		std::string linea;
		std::getline(archivoDescifrado, linea);

		if (linea != "BANCO_BACKUP_V2.0_FROM_MONGODB" && linea != "BANCO_BACKUP_V1.0") {
			std::cout << "Formato de archivo descifrado inválido. Verifique la clave de descifrado.\n";
			archivoDescifrado.close();
			std::remove(rutaTemporal.c_str());
			return false;
		}

		// Procesar archivo descifrado
		auto& clienteDB = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona baseDatosTemp(clienteDB);

		bool resultado = procesarArchivoRecuperacion(archivoDescifrado, baseDatosTemp);

		archivoDescifrado.close();

		// Eliminar archivo temporal
		if (std::remove(rutaTemporal.c_str()) != 0) {
			std::cout << "Advertencia: No se pudo eliminar el archivo temporal.\n";
		}

		if (resultado) {
			std::cout << "Datos recuperados correctamente desde archivo cifrado.\n";
		}
		else {
			std::cout << "Error al recuperar algunos datos desde el archivo cifrado.\n";
		}

		return resultado;
	}
	catch (const std::exception& e) {
		std::cout << "Error durante la recuperación desde archivo cifrado: " << e.what() << "\n";
		return false;
	}
}

/**
 * @brief Procesa archivo de respaldo y carga datos en MongoDB
 * Usando funciones de _BaseDatosPersona.cpp
 */
bool ExportadorArchivo::procesarArchivoRecuperacion(std::ifstream& archivo, _BaseDatosPersona& baseDatos) {
	std::string linea;
	std::map<std::string, std::string> datosPersona;
	std::vector<std::map<std::string, std::string>> cuentasAhorro;
	std::vector<std::map<std::string, std::string>> cuentasCorriente;

	bool enPersona = false;
	bool enCuentasAhorro = false;
	bool enCuentasCorriente = false;
	bool enCuentaIndividual = false;
	std::map<std::string, std::string> cuentaActual;

	int personasRecuperadas = 0;
	int errores = 0;

	while (std::getline(archivo, linea)) {
		// Procesar marcadores de persona
		if (linea == "===PERSONA_INICIO===") {
			enPersona = true;
			datosPersona.clear();
			cuentasAhorro.clear();
			cuentasCorriente.clear();
			continue;
		}

		if (linea == "===PERSONA_FIN===") {
			if (enPersona) {
				// Cargar persona en MongoDB usando funciones de _BaseDatosPersona.cpp
				bool resultado = cargarPersonaEnMongoDB(datosPersona, cuentasAhorro, cuentasCorriente, baseDatos);
				if (resultado) {
					personasRecuperadas++;
				}
				else {
					errores++;
				}
			}
			enPersona = false;
			continue;
		}

		// Procesar marcadores de cuentas
		if (linea == "===CUENTAS_AHORRO_INICIO===") {
			enCuentasAhorro = true;
			continue;
		}
		if (linea == "===CUENTAS_AHORRO_FIN===") {
			enCuentasAhorro = false;
			continue;
		}
		if (linea == "===CUENTAS_CORRIENTE_INICIO===") {
			enCuentasCorriente = true;
			continue;
		}
		if (linea == "===CUENTAS_CORRIENTE_FIN===") {
			enCuentasCorriente = false;
			continue;
		}

		// Procesar cuentas individuales
		if (linea == "CUENTA_AHORROS_INICIO" || linea == "CUENTA_CORRIENTE_INICIO") {
			enCuentaIndividual = true;
			cuentaActual.clear();
			continue;
		}
		if (linea == "CUENTA_AHORROS_FIN") {
			if (enCuentaIndividual && enCuentasAhorro) {
				cuentasAhorro.push_back(cuentaActual);
			}
			enCuentaIndividual = false;
			continue;
		}
		if (linea == "CUENTA_CORRIENTE_FIN") {
			if (enCuentaIndividual && enCuentasCorriente) {
				cuentasCorriente.push_back(cuentaActual);
			}
			enCuentaIndividual = false;
			continue;
		}

		// Procesar datos individuales
		size_t pos = linea.find(':');
		if (pos != std::string::npos) {
			std::string clave = linea.substr(0, pos);
			std::string valor = linea.substr(pos + 1);

			if (enPersona && !enCuentasAhorro && !enCuentasCorriente) {
				datosPersona[clave] = valor;
			}
			else if (enCuentaIndividual) {
				cuentaActual[clave] = valor;
			}
		}
	}

	std::cout << "Recuperación completada: " << personasRecuperadas << " personas cargadas";
	if (errores > 0) {
		std::cout << ", " << errores << " errores encontrados";
	}
	std::cout << "\n";

	return errores == 0;
}

/**
 * @brief Carga una persona completa en MongoDB
 * Usando funciones de _BaseDatosPersona.cpp
 */
bool ExportadorArchivo::cargarPersonaEnMongoDB(const std::map<std::string, std::string>& datosPersona,
	const std::vector<std::map<std::string, std::string>>& cuentasAhorro,
	const std::vector<std::map<std::string, std::string>>& cuentasCorriente,
	_BaseDatosPersona& baseDatos) {

	try {
		// Verificar datos mínimos de persona
		auto cedulaIt = datosPersona.find("CEDULA");
		auto nombresIt = datosPersona.find("NOMBRES");
		auto apellidosIt = datosPersona.find("APELLIDOS");

		if (cedulaIt == datosPersona.end() || nombresIt == datosPersona.end() || apellidosIt == datosPersona.end()) {
			std::cout << "Error: Datos incompletos de persona\n";
			return false;
		}

		std::string cedula = cedulaIt->second;
		std::string nombres = nombresIt->second;
		std::string apellidos = apellidosIt->second;

		// Verificar si la persona ya existe
		if (baseDatos.existePersonaPorCedula(cedula)) {
			std::cout << "Advertencia: Persona con cédula " << cedula << " ya existe. Omitiendo...\n";
			return true; // No es error, simplemente ya existe
		}

		// Crear objeto Persona
		std::string fechaNacimiento = datosPersona.count("FECHA_NACIMIENTO") ? datosPersona.at("FECHA_NACIMIENTO") : "";
		std::string correo = datosPersona.count("CORREO") ? datosPersona.at("CORREO") : "";
		std::string direccion = datosPersona.count("DIRECCION") ? datosPersona.at("DIRECCION") : "";

		Persona persona(cedula, nombres, apellidos, fechaNacimiento, correo, direccion);
		persona.setNumeCuentas(static_cast<int>(cuentasAhorro.size()));
		persona.setNumCorrientes(static_cast<int>(cuentasCorriente.size()));

		// Insertar persona sin cuentas inicialmente
		bool resultadoPersona = baseDatos.insertarNuevaPersona(persona);
		if (!resultadoPersona) {
			std::cout << "Error al insertar persona con cédula " << cedula << "\n";
			return false;
		}

		// Agregar cuentas de ahorro usando funciones de _BaseDatosPersona.cpp
		for (const auto& cuenta : cuentasAhorro) {
			if (!agregarCuentaDesdeBackup(cedula, cuenta, "ahorros", baseDatos)) {
				std::cout << "Advertencia: Error al agregar cuenta de ahorro para " << cedula << "\n";
			}
		}

		// Agregar cuentas corrientes
		for (const auto& cuenta : cuentasCorriente) {
			if (!agregarCuentaDesdeBackup(cedula, cuenta, "corriente", baseDatos)) {
				std::cout << "Advertencia: Error al agregar cuenta corriente para " << cedula << "\n";
			}
		}

		return true;
	}
	catch (const std::exception& e) {
		std::cout << "Error al cargar persona en MongoDB: " << e.what() << "\n";
		return false;
	}
}

/**
 * @brief Agrega una cuenta desde backup usando funciones de _BaseDatosPersona.cpp
 */
bool ExportadorArchivo::agregarCuentaDesdeBackup(const std::string& cedula,
	const std::map<std::string, std::string>& datosCuenta,
	const std::string& tipoCuenta,
	_BaseDatosPersona& baseDatos) {

	try {
		// Extraer datos de la cuenta
		std::string numeroCuenta = datosCuenta.count("NUMERO_CUENTA") ? datosCuenta.at("NUMERO_CUENTA") : "";
		std::string saldoStr = datosCuenta.count("SALDO") ? datosCuenta.at("SALDO") : "0.0";
		std::string fechaApertura = datosCuenta.count("FECHA_APERTURA") ? datosCuenta.at("FECHA_APERTURA") : "";
		std::string estado = datosCuenta.count("ESTADO") ? datosCuenta.at("ESTADO") : "ACTIVA";

		if (numeroCuenta.empty()) {
			return false;
		}

		double saldo = 0.0;
		try {
			saldo = std::stod(saldoStr);
		}
		catch (const std::exception&) {
			saldo = 0.0;
		}

		// Crear documento BSON para la cuenta usando funciones de _BaseDatosPersona.cpp
		auto cuentaDoc = bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("numeroCuenta", numeroCuenta),
			bsoncxx::builder::basic::kvp("tipo", tipoCuenta),
			bsoncxx::builder::basic::kvp("saldo", saldo),
			bsoncxx::builder::basic::kvp("fechaApertura", fechaApertura),
			bsoncxx::builder::basic::kvp("estado", estado),
			bsoncxx::builder::basic::kvp("fechaCreacion", bsoncxx::types::b_date{ std::chrono::system_clock::now() })
		);

		// Usar función de _BaseDatosPersona.cpp para agregar la cuenta
		return baseDatos.agregarCuentaPersona(cedula, cuentaDoc);
	}
	catch (const std::exception& e) {
		std::cout << "Error al agregar cuenta desde backup: " << e.what() << "\n";
		return false;
	}
}

// === MÉTODOS DE INTERFAZ DE USUARIO ===

int ExportadorArchivo::solicitarTipoRecuperacion() {
	auto opciones = FabricaEstrategiasRecuperacion::obtenerOpcionesDisponibles();
	return Utilidades::menuInteractivo("Seleccione el tipo de recuperación:", opciones, 0, 0);
}

std::string ExportadorArchivo::solicitarNombreArchivoRecuperacion() {
	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "Ingrese el nombre del archivo a recuperar (sin extensión): ";
	std::string nombreArchivo;
	std::getline(std::cin, nombreArchivo);

	// Limpiar buffer si es necesario
	if (std::cin.peek() == '\n') {
		std::cin.ignore();
	}

	return nombreArchivo;
}

char ExportadorArchivo::solicitarClaveDescifrado() {
	std::cout << "Ingrese la clave de descifrado (un solo carácter): ";
	char clave = _getch();
	std::cout << "*\n"; // Ocultar la clave por seguridad
	return clave;
}

void ExportadorArchivo::mostrarResultadoRecuperacion(bool exito, const std::string& tipoOperacion) {
	if (exito) {
		std::cout << "\nDatos recuperados correctamente desde archivo (" << tipoOperacion << ").\n";
	}
	else {
		std::cout << "\nError al recuperar datos desde archivo (" << tipoOperacion << ").\n";
	}

	std::cout << "Presione cualquier tecla para continuar...";
	int teclaCualquiera = _getch();
	(void)teclaCualquiera;
}