#define _CRT_SECURE_NO_WARNINGS

/**
 * @file _BaseDatosPersona.cpp
 * @brief Implementación de la clase _BaseDatosPersona para manejar operaciones con MongoDB
 *
 * Esta clase proporciona métodos para insertar, consultar y verificar personas en una base de datos MongoDB.
 */
#include "_BaseDatosPersona.h"
#include "ConexionMongo.h"
#include "ArbolBPlusGrafico.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/pipeline.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>
#include <chrono>
#include "Persona.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

/**
 * @brief Constructor de la clase _BaseDatosPersona
 *
 * @param client Referencia al cliente de MongoDB
 */
_BaseDatosPersona::_BaseDatosPersona(mongocxx::client& client)
	: _client(client) {
}

/**
 *@brief Inserta una nueva persona en la base de datos MongoDB
 *
 * @param persona La persona a insertar
 * @return true si la inserción fue exitosa, false en caso contrario
 */
bool _BaseDatosPersona::insertarNuevaPersona(const Persona& persona)  {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Construye el documento BSON a partir del objeto Persona con la misma estructura que insertarPersona
		auto doc = make_document(
			kvp("cedula", persona.getCedula()),
			kvp("nombre", persona.getNombres()),
			kvp("apellido", persona.getApellidos()),
			kvp("fechaNacimiento", persona.getFechaNacimiento()),
			kvp("correo", persona.getCorreo()),
			kvp("direccion", persona.getDireccion()),
			kvp("numAhorros", persona.getNumCuentas()), // Cambiado a numAhorros para consistencia
			kvp("numCorrientes", persona.getNumCorrientes()), // Mantenido igual para consistencia
			kvp("totalCuentasExistentes", 0), // Siempre 0 para una persona nueva sin cuentas
			kvp("cuentas", bsoncxx::builder::basic::array{}) // Array vacío de cuentas para persona nueva
		);

		auto result = collection.insert_one(doc.view());
		return result ? true : false;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al insertar persona: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Inserta una nueva persona en la base de datos MongoDB con una cuenta inicial opcional
 *
 * @param persona La persona a insertar
 * @param cuentaInicial Documento BSON de la cuenta inicial (opcional)
 * @return true si la inserción fue exitosa, false en caso contrario
 */
bool _BaseDatosPersona::insertarPersona(const Persona& persona, const bsoncxx::document::value* cuentaInicial) {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		bsoncxx::builder::basic::array cuentasArray;
		if (cuentaInicial) {
			cuentasArray.append(*cuentaInicial);
		}

		auto doc = make_document(
			kvp("cedula", persona.getCedula()),
			kvp("nombre", persona.getNombres()),
			kvp("apellido", persona.getApellidos()),
			kvp("fechaNacimiento", persona.getFechaNacimiento()),
			kvp("correo", persona.getCorreo()),
			kvp("direccion", persona.getDireccion()),
			kvp("numAhorros", persona.getNumCuentas()), // Cambiado a numAhorros para mayor claridad
			kvp("numCorrientes", persona.getNumCorrientes()), // Cambiado a numCorrientes para mayor claridad
			kvp("totalCuentasExistentes", (cuentaInicial ? 1 : 0)),
			kvp("cuentas", cuentasArray)
		);

		auto result = collection.insert_one(doc.view());
		return result ? true : false;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al insertar persona: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Verifica si existe una persona con la cédula dada en la base de datos MongoDB
 *
 * @param cedula Cédula a buscar
 * @return true si la persona existe, false en caso contrario
 */
bool _BaseDatosPersona::existePersonaPorCedula(const std::string& cedula) {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];
		auto filter = bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("cedula", cedula)
		);
		auto result = collection.find_one(filter.view());
		return result ? true : false;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al consultar persona: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Obtiene una persona por su cédula desde la base de datos MongoDB
 *
 * @param cedula Cédula de la persona a buscar
 * @return Puntero a la Persona encontrada, o nullptr si no se encuentra
 */
Persona* _BaseDatosPersona::obtenerPersonaPorCedula(const std::string& cedula) {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];
		auto filter = bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("cedula", cedula)
		);
		auto result = collection.find_one(filter.view());
		if (!result) return nullptr;

		auto view = result->view();
		std::string nombres = std::string(view["nombre"].get_string().value);
		std::string apellidos = std::string(view["apellido"].get_string().value);
		std::string fechaNacimiento = std::string(view["fechaNacimiento"].get_string().value);
		std::string correo = std::string(view["correo"].get_string().value);
		std::string direccion = std::string(view["direccion"].get_string().value);

		// Crea el objeto Persona con los datos recuperados
		return new Persona(cedula, nombres, apellidos, fechaNacimiento, correo, direccion);
	}
	catch (const std::exception& e) {
		std::cerr << "Error al obtener persona: " << e.what() << std::endl;
		return nullptr;
	}
}

/**
 * @brief Agrega una cuenta a una persona existente en la base de datos MongoDB
 *
 * @param cedula Cédula de la persona a la que se le agregará la cuenta
 * @param cuentaDoc Documento BSON de la cuenta a agregar
 * @return true si la cuenta fue agregada exitosamente, false en caso contrario
 */
bool _BaseDatosPersona::agregarCuentaPersona(const std::string& cedula, const bsoncxx::document::value& cuentaDoc) {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Buscar la persona por cédula
		auto filter = make_document(kvp("cedula", cedula));
		auto personaDoc = collection.find_one(filter.view());

		if (!personaDoc) {
			std::cerr << "No se encontró la persona con cédula: " << cedula << std::endl;
			return false;
		}

		// Contar cuántas cuentas tiene actualmente
		auto cuentasElement = personaDoc->view()["cuentas"];
		int numCuentas = 0;
		if (cuentasElement && cuentasElement.type() == bsoncxx::type::k_array) {
			numCuentas = static_cast<int>(std::distance(cuentasElement.get_array().value.begin(), cuentasElement.get_array().value.end()));
		}

		if (numCuentas >= 5) {
			std::cerr << "La persona ya tiene el máximo de 5 cuentas." << std::endl;
			return false;
		}

		// Detectar el tipo de cuenta
		std::string tipoCuenta = "";
		auto tipoElement = cuentaDoc.view()["tipo"];
		if (tipoElement && tipoElement.type() == bsoncxx::type::k_utf8) {
			tipoCuenta = std::string(tipoElement.get_string().value);
		}

		// Construir el update según el tipo de cuenta
		bsoncxx::builder::basic::document update;
		update.append(
			bsoncxx::builder::basic::kvp("$push", bsoncxx::builder::basic::make_document(
				bsoncxx::builder::basic::kvp("cuentas", cuentaDoc)
			))
		);

		// Incrementar los contadores correctos
		bsoncxx::builder::basic::document incDoc;
		incDoc.append(bsoncxx::builder::basic::kvp("totalCuentasExistentes", 1)); // Siempre se incrementa

		if (tipoCuenta == "ahorros") {
			incDoc.append(bsoncxx::builder::basic::kvp("numCuentas", 1));
		}
		else if (tipoCuenta == "corriente") {
			incDoc.append(bsoncxx::builder::basic::kvp("numCorrientes", 1));
		}
		update.append(bsoncxx::builder::basic::kvp("$inc", incDoc.extract()));

		auto result = collection.update_one(filter.view(), update.view());
		return result && result->modified_count() == 1;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al agregar cuenta: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Prueba la conexión a una base de datos MongoDB
 *
 * @param uri_str La cadena de conexión URI de MongoDB
 * @return true si la conexión fue exitosa, false en caso contrario
 */
bool _BaseDatosPersona::probarConexionMongoDB(const std::string& uri_str) {
	try {
		mongocxx::instance instance{}; // Solo debe haber una instancia por aplicación
		mongocxx::uri uri(uri_str);
		mongocxx::client client(uri);

		// Intentar listar las bases de datos para verificar la conexión
		auto dbs = client.list_database_names();
		std::cout << "Conexión exitosa a MongoDB. Bases de datos disponibles:\n";
		for (const auto& db_name : dbs) {
			std::cout << " - " << db_name << std::endl;
		}
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al conectar a MongoDB: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Valida que el monto sea un número válido y positivo
 *
 * @param monto Monto a validar
 * @return true si el monto es válido, false en caso contrario
 */
bool _BaseDatosPersona::validarMonto(double monto) {
	return monto > 0.0 && monto <= 15000.00 && !std::isnan(monto) && !std::isinf(monto);
}

/**
 * @brief Redondea un monto a dos decimales
 *
 * @param monto Monto a redondear
 * @return Monto redondeado a dos decimales
 */
double _BaseDatosPersona::redondearMonto(double monto) {
	return std::round(monto * 100.0) / 100.0;
}

/**
 * @brief Busca el índice de una cuenta específica dentro del documento de una persona
 *
 * @param personaDoc Documento BSON de la persona
 * @param numeroCuenta Número de cuenta a buscar
 * @return Índice de la cuenta en el array, -1 si no se encuentra
 */
int _BaseDatosPersona::buscarIndiceCuentaEnDocumento(const bsoncxx::document::view& personaDoc, const std::string& numeroCuenta) {
	auto cuentasElement = personaDoc["cuentas"];
	if (!cuentasElement || cuentasElement.type() != bsoncxx::type::k_array) {
		return -1;
	}

	auto cuentasArray = cuentasElement.get_array().value;
	int indice = 0;

	for (auto& cuenta : cuentasArray) {
		if (cuenta.type() == bsoncxx::type::k_document) {
			auto cuentaDoc = cuenta.get_document().value;
			auto numCuentaElement = cuentaDoc["numeroCuenta"];

			if (numCuentaElement && numCuentaElement.type() == bsoncxx::type::k_utf8 &&
				std::string(numCuentaElement.get_string().value) == numeroCuenta) {
				return indice;
			}
		}
		indice++;
	}
	return -1;
}

/**
 * @brief Obtiene la cédula del titular de una cuenta por su número
 *
 * @param numeroCuenta Número de cuenta a buscar
 * @return Cédula del titular, o cadena vacía si no se encuentra
 */
std::string _BaseDatosPersona::obtenerCedulaPorNumeroCuenta(const std::string& numeroCuenta) {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Buscar en todas las personas una cuenta con ese número
		auto cursor = collection.find({});

		for (auto& doc : cursor) {
			auto& view = doc;
			int indice = buscarIndiceCuentaEnDocumento(view, numeroCuenta);

			if (indice >= 0) {
				auto cedulaElement = view["cedula"];
				if (cedulaElement && cedulaElement.type() == bsoncxx::type::k_utf8) {
					return std::string(cedulaElement.get_string().value);
				}
			}
		}

		return ""; // No se encontró la cuenta
	}
	catch (const std::exception& e) {
		std::cerr << "Error al obtener cédula por número de cuenta: " << e.what() << std::endl;
		return "";
	}
}

/**
 * @brief Obtiene el saldo actual de una cuenta por su número
 *
 * @param numeroCuenta Número de cuenta a consultar
 * @return Saldo actual como double, -1.0 si la cuenta no existe
 */
double _BaseDatosPersona::obtenerSaldoCuenta(const std::string& numeroCuenta) {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Buscar en todas las personas una cuenta con ese número
		auto cursor = collection.find({});

		for (auto& doc : cursor) {
			auto &view = doc;
			int indice = buscarIndiceCuentaEnDocumento(view, numeroCuenta);

			if (indice >= 0) {
				auto cuentasArray = view["cuentas"].get_array().value;
				auto cuentaIterator = cuentasArray.begin();
				std::advance(cuentaIterator, indice);

				if (cuentaIterator != cuentasArray.end() && cuentaIterator->type() == bsoncxx::type::k_document) {
					auto cuentaDoc = cuentaIterator->get_document().value;
					auto saldoElement = cuentaDoc["saldo"];

					if (saldoElement) {
						if (saldoElement.type() == bsoncxx::type::k_double) {
							return redondearMonto(saldoElement.get_double().value);
						}
						else if (saldoElement.type() == bsoncxx::type::k_int32) {
							return static_cast<double>(saldoElement.get_int32().value);
						}
						else if (saldoElement.type() == bsoncxx::type::k_int64) {
							return static_cast<double>(saldoElement.get_int64().value);
						}
					}
				}
			}
		}

		return -1.0; // Cuenta no encontrada
	}
	catch (const std::exception& e) {
		std::cerr << "Error al obtener saldo: " << e.what() << std::endl;
		return -1.0;
	}
}

/**
 * @brief Verifica si una cuenta tiene fondos suficientes para una operación
 *
 * @param numeroCuenta Número de cuenta a verificar
 * @param monto Monto a verificar
 * @return true si tiene fondos suficientes, false en caso contrario
 */
bool _BaseDatosPersona::verificarFondosSuficientes(const std::string& numeroCuenta, double monto) {
	if (!validarMonto(monto)) {
		return false;
	}

	double saldoActual = obtenerSaldoCuenta(numeroCuenta);
	return saldoActual >= 0.0 && saldoActual >= redondearMonto(monto);
}

/**
 * @brief Deposita un monto en una cuenta específica
 *
 * @param numeroCuenta Número de cuenta donde se depositará el monto
 * @param monto Monto a depositar
 * @return true si el depósito fue exitoso, false en caso contrario
 */
bool _BaseDatosPersona::depositarEnCuenta(const std::string& numeroCuenta, double monto) {
	try {
		if (!validarMonto(monto)) {
			std::cerr << "Monto inválido para depósito: " << monto << std::endl;
			return false;
		}

		monto = redondearMonto(monto);
		std::string cedula = obtenerCedulaPorNumeroCuenta(numeroCuenta);

		if (cedula.empty()) {
			std::cerr << "No se encontró la cuenta: " << numeroCuenta << std::endl;
			return false;
		}

		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Encontrar el documento de la persona
		auto filter = make_document(kvp("cedula", cedula));
		auto personaDoc = collection.find_one(filter.view());

		if (!personaDoc) {
			return false;
		}

		int indice = buscarIndiceCuentaEnDocumento(personaDoc->view(), numeroCuenta);
		if (indice < 0) {
			return false;
		}

		// Actualizar el saldo usando el operador $inc para incrementar
		std::string campoSaldo = "cuentas." + std::to_string(indice) + ".saldo";

		auto update = make_document(
			kvp("$inc", make_document(kvp(campoSaldo, monto)))
		);

		auto result = collection.update_one(filter.view(), update.view());

		if (result && result->modified_count() == 1) {
			std::cout << "Depósito exitoso: $" << std::fixed << std::setprecision(2) << monto
				<< " en cuenta " << numeroCuenta << std::endl;
			return true;
		}

		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al depositar: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Retira un monto de una cuenta específica
 *
 * @param numeroCuenta Número de cuenta de donde se retirará el monto
 * @param monto Monto a retirar
 * @return true si el retiro fue exitoso, false en caso contrario
 */
bool _BaseDatosPersona::retirarDeCuenta(const std::string& numeroCuenta, double monto) {
	try {
		if (!validarMonto(monto)) {
			std::cerr << "Monto inválido para retiro: " << monto << std::endl;
			return false;
		}

		monto = redondearMonto(monto);

		// Verificar fondos suficientes antes del retiro
		if (!verificarFondosSuficientes(numeroCuenta, monto)) {
			std::cerr << "Fondos insuficientes para el retiro de $" << std::fixed << std::setprecision(2) << monto << std::endl;
			return false;
		}

		std::string cedula = obtenerCedulaPorNumeroCuenta(numeroCuenta);
		if (cedula.empty()) {
			std::cerr << "No se encontró la cuenta: " << numeroCuenta << std::endl;
			return false;
		}

		auto db = _client["Banco"];
		auto collection = db["personas"];

		auto filter = make_document(kvp("cedula", cedula));
		auto personaDoc = collection.find_one(filter.view());

		if (!personaDoc) {
			return false;
		}

		int indice = buscarIndiceCuentaEnDocumento(personaDoc->view(), numeroCuenta);
		if (indice < 0) {
			return false;
		}

		// Actualizar el saldo restando el monto (usando $inc con valor negativo)
		std::string campoSaldo = "cuentas." + std::to_string(indice) + ".saldo";

		auto update = make_document(
			kvp("$inc", make_document(kvp(campoSaldo, -monto)))
		);

		auto result = collection.update_one(filter.view(), update.view());

		if (result && result->modified_count() == 1) {
			std::cout << "Retiro exitoso: $" << std::fixed << std::setprecision(2) << monto
				<< " de cuenta " << numeroCuenta << std::endl;
			return true;
		}

		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al retirar: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Realiza una transferencia entre dos cuentas
 *
 * @param cuentaOrigen Número de cuenta origen
 * @param cuentaDestino Número de cuenta destino
 * @param monto Monto a transferir
 * @return true si la transferencia fue exitosa, false en caso contrario
 */
bool _BaseDatosPersona::realizarTransferencia(const std::string& cuentaOrigen, const std::string& cuentaDestino, double monto) {
	try {
		if (!validarMonto(monto)) {
			std::cerr << "Monto inválido para transferencia: " << monto << std::endl;
			return false;
		}

		if (cuentaOrigen == cuentaDestino) {
			std::cerr << "No se puede transferir a la misma cuenta" << std::endl;
			return false;
		}

		monto = redondearMonto(monto);

		// Verificar que ambas cuentas existan
		if (obtenerSaldoCuenta(cuentaOrigen) < 0.0) {
			std::cerr << "Cuenta origen no encontrada: " << cuentaOrigen << std::endl;
			return false;
		}

		if (obtenerSaldoCuenta(cuentaDestino) < 0.0) {
			std::cerr << "Cuenta destino no encontrada: " << cuentaDestino << std::endl;
			return false;
		}

		// Verificar fondos suficientes
		if (!verificarFondosSuficientes(cuentaOrigen, monto)) {
			std::cerr << "Fondos insuficientes en cuenta origen" << std::endl;
			return false;
		}

		// Usar transacción para garantizar atomicidad
		auto session = _client.start_session();

		try {
			session.start_transaction();

			// 1. Retirar de cuenta origen
			bool retiroExitoso = retirarDeCuenta(cuentaOrigen, monto);
			if (!retiroExitoso) {
				session.abort_transaction();
				std::cerr << "Error en el retiro de la cuenta origen" << std::endl;
				return false;
			}

			// 2. Depositar en cuenta destino
			bool depositoExitoso = depositarEnCuenta(cuentaDestino, monto);
			if (!depositoExitoso) {
				session.abort_transaction();
				std::cerr << "Error en el depósito a la cuenta destino" << std::endl;
				return false;
			}

			// 3. Confirmar transacción
			session.commit_transaction();

			std::cout << "Transferencia exitosa: $" << std::fixed << std::setprecision(2) << monto
				<< " de " << cuentaOrigen << " a " << cuentaDestino << std::endl;

			return true;
		}
		catch (const std::exception& e) {
			std::cerr << "Error en transferencia: " << e.what() << std::endl;
			session.abort_transaction();
			throw; // Re-lanzar la excepción
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error en transferencia: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Obtiene información completa de una cuenta por su número
 *
 * @param numeroCuenta Número de cuenta a consultar
 * @return Documento BSON con la información del titular y la cuenta, o un documento vacío si no se encuentra
 */
bsoncxx::document::value _BaseDatosPersona::obtenerInformacionCuenta(const std::string& numeroCuenta) {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		auto cursor = collection.find({});

		for (auto& doc : cursor) {
			auto &view = doc;
			int indice = buscarIndiceCuentaEnDocumento(view, numeroCuenta);

			if (indice >= 0) {
				auto cuentasArray = view["cuentas"].get_array().value;
				auto cuentaIterator = cuentasArray.begin();
				std::advance(cuentaIterator, indice);

				if (cuentaIterator != cuentasArray.end() && cuentaIterator->type() == bsoncxx::type::k_document) {
					auto cuentaDoc = cuentaIterator->get_document().value;

					// Crear un documento completo con información del titular
					auto infoCompleta = bsoncxx::builder::basic::make_document(
						kvp("numeroCuenta", numeroCuenta),
						kvp("cedula", view["cedula"].get_string().value),
						kvp("titular", bsoncxx::builder::basic::make_document(
							kvp("nombre", view["nombre"].get_string().value),
							kvp("apellido", view["apellido"].get_string().value),
							kvp("correo", view["correo"].get_string().value)
						)),
						kvp("cuenta", cuentaDoc)
					);

                    return bsoncxx::document::value{infoCompleta.view()};
				}
			}
		}

		// Retornar documento vacío si no se encuentra
		return bsoncxx::document::value(bsoncxx::builder::basic::make_document().view());
	}
	catch (const std::exception& e) {
		std::cerr << "Error al obtener información de cuenta: " << e.what() << std::endl;
		return bsoncxx::document::value(bsoncxx::builder::basic::make_document().view());
	}
}

/**
 * @brief Obtiene el último número secuencial usado para una sucursal específica
 */
int _BaseDatosPersona::obtenerUltimoSecuencial(const std::string& sucursal) {
	try {
		auto db = _client["Banco"];
		auto collection = db["secuenciales"];

		auto filter = make_document(kvp("sucursal", sucursal));
		auto result = collection.find_one(filter.view());

		if (result) {
			auto view = result->view();
			auto secuencialElement = view["ultimo_secuencial"];
			if (secuencialElement && secuencialElement.type() == bsoncxx::type::k_int32) {
				return secuencialElement.get_int32().value;
			}
			else if (secuencialElement && secuencialElement.type() == bsoncxx::type::k_int64) {
				return static_cast<int>(secuencialElement.get_int64().value);
			}
		}
		return 0; // No existe secuencial para esta sucursal
	}
	catch (const std::exception& e) {
		std::cerr << "Error al obtener último secuencial: " << e.what() << std::endl;
		return 0;
	}
}

/*
* @brief 
*/
bool _BaseDatosPersona::actualizarSecuencial(const std::string& sucursal, int nuevoSecuencial) {
	try {
		auto db = _client["Banco"];
		auto collection = db["secuenciales"];

		auto filter = make_document(kvp("sucursal", sucursal));

		// Usar upsert para crear el documento si no existe
		auto update = make_document(
			kvp("$set", make_document(
				kvp("sucursal", sucursal),
				kvp("ultimo_secuencial", nuevoSecuencial),
				kvp("fecha_actualizacion", bsoncxx::types::b_date{ std::chrono::system_clock::now() })
			))
		);

		mongocxx::options::update options;
		options.upsert(true); // Crear si no existe

		auto result = collection.update_one(filter.view(), update.view(), options);
		return result && (result->modified_count() == 1 || result->upserted_count() == 1);
	}
	catch (const std::exception& e) {
		std::cerr << "Error al actualizar secuencial: " << e.what() << std::endl;
		return false;
	}
}

/*
 * @brief Obtiene el mayor número de cuenta por sucursal
 *
 * @param sucursal Código de la sucursal para buscar el mayor número de cuenta
 * @return El mayor número de cuenta encontrado, o 0 si no se encuentra ninguna cuenta
 */ 
int _BaseDatosPersona::obtenerMayorNumeroCuentaPorSucursal(const std::string& sucursal) {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Pipeline de agregación para buscar el mayor número de cuenta por sucursal
		mongocxx::pipeline pipeline;

		// Desenrollar el array de cuentas
		pipeline.unwind("$cuentas");

		// Filtrar por cuentas que empiecen con el código de sucursal
		pipeline.match(make_document(
			kvp("cuentas.numeroCuenta", make_document(
				kvp("$regex", "^" + sucursal),
				kvp("$options", "i")
			))
		));

		// Proyectar solo el número de cuenta y extraer la parte secuencial
		// Crear el array de argumentos para $substr separadamente
		bsoncxx::builder::basic::array substrArgs;
		substrArgs.append("$cuentas.numeroCuenta");
		substrArgs.append(3);
		substrArgs.append(6);

		pipeline.project(make_document(
			kvp("numeroCuenta", "$cuentas.numeroCuenta"),
			kvp("secuencial", make_document(
				kvp("$toInt", make_document(
					kvp("$substr", substrArgs)
				))
			))
		));

		// Ordenar por secuencial descendente
		pipeline.sort(make_document(kvp("secuencial", -1)));

		// Limitar a 1 resultado
		pipeline.limit(1);

		auto cursor = collection.aggregate(pipeline);
		for (auto&& doc : cursor) {
			auto secuencialElement = doc["secuencial"];
			if (secuencialElement && secuencialElement.type() == bsoncxx::type::k_int32) {
				return secuencialElement.get_int32().value;
			}
			else if (secuencialElement && secuencialElement.type() == bsoncxx::type::k_int64) {
				return static_cast<int>(secuencialElement.get_int64().value);
			}
		}

		return 0; // No se encontraron cuentas para esta sucursal
	}
	catch (const std::exception& e) {
		std::cerr << "Error al obtener mayor número de cuenta: " << e.what() << std::endl;
		return 0;
	}
}

/**
 * @brief Busca personas por criterio específico en la base de datos
 */
std::vector<bsoncxx::document::value> _BaseDatosPersona::buscarPersonasPorCriterio(const std::string& criterio, const std::string& valor) {
	std::vector<bsoncxx::document::value> resultados;

	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		bsoncxx::builder::basic::document filtro;

		if (criterio == "numAhorros" || criterio == "numCorrientes" || criterio == "totalCuentasExistentes") {
			// Para campos numéricos
			try {
				int valorNumerico = std::stoi(valor);
				filtro.append(kvp(criterio, valorNumerico));
			}
			catch (const std::exception&) {
				std::cerr << "Error: El valor debe ser numérico para el criterio " << criterio << std::endl;
				return resultados;
			}
		}
		else {
			// Para campos de texto (usar regex para búsqueda parcial)
			filtro.append(kvp(criterio, make_document(
				kvp("$regex", valor),
				kvp("$options", "i") // case insensitive
			)));
		}

		auto cursor = collection.find(filtro.view());

		for (auto&& doc : cursor) {
			resultados.emplace_back(bsoncxx::document::value(doc));
		}

	}
	catch (const std::exception& e) {
		std::cerr << "Error al buscar por criterio: " << e.what() << std::endl;
	}

	return resultados;
}

/**
 * @brief Busca cuentas por rango de fechas desde una fecha hasta hoy
 */
std::vector<bsoncxx::document::value> _BaseDatosPersona::buscarCuentasPorRangoFechas(const std::string& fechaInicio) {
	std::vector<bsoncxx::document::value> resultados;

	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Convertir fecha de inicio a formato ISO
		std::string fechaInicioISO = convertirFechaAISO(fechaInicio);

		// Obtener fecha actual en formato ISO
		auto now = std::chrono::system_clock::now();
		auto time_t_now = std::chrono::system_clock::to_time_t(now);
		std::tm tm_now;
		localtime_s(&tm_now, &time_t_now);

		char fechaActualISO[11];
		std::strftime(fechaActualISO, sizeof(fechaActualISO), "%d/%m/%Y", &tm_now);

		// Pipeline de agregación para buscar cuentas en el rango de fechas
		mongocxx::pipeline pipeline;

		// Desenrollar el array de cuentas
		pipeline.unwind("$cuentas");

		// Filtrar por rango de fechas
		pipeline.match(make_document(
			kvp("cuentas.fechaApertura", make_document(
				kvp("$gte", fechaInicioISO),
				kvp("$lte", std::string(fechaActualISO))
			))
		));

		// Proyectar la información necesaria
		pipeline.project(make_document(
			kvp("cedula", "$cedula"),
			kvp("nombre", "$nombre"),
			kvp("apellido", "$apellido"),
			kvp("correo", "$correo"),
			kvp("cuenta", "$cuentas")
		));

		auto cursor = collection.aggregate(pipeline);

		for (auto&& doc : cursor) {
			resultados.emplace_back(bsoncxx::document::value(doc));
		}

	}
	catch (const std::exception& e) {
		std::cerr << "Error al buscar por rango de fechas: " << e.what() << std::endl;
	}

	return resultados;
}

/**
 * @brief Busca todas las cuentas de una persona por su cédula
 */
bsoncxx::document::value _BaseDatosPersona::buscarPersonaCompletaPorCedula(const std::string& cedula) {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		auto filtro = make_document(kvp("cedula", cedula));
		auto resultado = collection.find_one(filtro.view());

		if (resultado) {
			return bsoncxx::document::value(*resultado);
		}

	}
	catch (const std::exception& e) {
		std::cerr << "Error al buscar persona completa: " << e.what() << std::endl;
	}

	// Retornar documento vacío si no se encuentra
	return bsoncxx::document::value(make_document().view());
}

/**
 * @brief Convierte una fecha de formato DD/MM/AAAA a formato compatible
 */
std::string _BaseDatosPersona::convertirFechaAISO(const std::string& fecha) {
	// La fecha ya está en formato DD/MM/AAAA que es compatible con nuestra BD
	return fecha;
}

void _BaseDatosPersona::iniciarBaseDatosArbolB()
{
	// Obtener referencia a la base de datos desde el banco
	mongocxx::client& clienteDB = ConexionMongo::obtenerClienteBaseDatos();
	_BaseDatosPersona baseDatos(clienteDB);

	// Verificar que existan datos en la base de datos
	if (!baseDatos.existenPersonasEnBaseDatos()) {
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "No hay personas registradas en la base de datos.\n";
		std::cout << "Por favor, registre personas primero.\n";
		system("pause");
		return;
	}

	// Menú de selección de criterio
	std::vector<std::string> criterios = {
		"Ordenar por Cédula",
		"Ordenar por Nombre (3 chars)",
		"Ordenar por Apellido (3 chars)",
		"Ordenar por Fecha de Nacimiento",
		"Volver al menú principal"
	};

	Utilidades::limpiarPantallaPreservandoMarquesina(1);

	int seleccion = Utilidades::menuInteractivo(
		"=== Árbol B+ Gráfico - Seleccione Criterio ===",
		criterios, 0, 0
	);

	if (seleccion >= 0 && seleccion <= 3) {
		// Mostrar árbol B+ gráfico con SFML
		ArbolBPlusGrafico::mostrarAnimadoSFMLGrado3(baseDatos, "", seleccion);
	}
	// Si seleccion == 4 o -1 (ESC), simplemente regresa al menú
}

/**
 * @brief Verifica si existen personas registradas en la base de datos MongoDB
 */
bool _BaseDatosPersona::existenPersonasEnBaseDatos() {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Contar documentos en la colección personas
		auto count = collection.estimated_document_count();

		return count > 0;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al verificar existencia de personas en BD: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Obtiene el número total de personas registradas en la base de datos
 */
long _BaseDatosPersona::obtenerTotalPersonasRegistradas() {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		return static_cast<long>(collection.estimated_document_count());
	}
	catch (const std::exception& e) {
		std::cerr << "Error al obtener total de personas: " << e.what() << std::endl;
		return 0;
	}
}

/**
 * @brief Verifica si existen cuentas en la base de datos MongoDB
 */
bool _BaseDatosPersona::existenCuentasEnBaseDatos() {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Pipeline de agregación para verificar si existen cuentas
		mongocxx::pipeline pipeline;

		// Filtrar personas que tengan al menos una cuenta
		pipeline.match(make_document(
			kvp("cuentas.0", make_document(kvp("$exists", true)))
		));

		// Limitar a 1 resultado para verificación de existencia
		pipeline.limit(1);

		auto cursor = collection.aggregate(pipeline);

		// Si hay al menos un resultado, existen cuentas
		return cursor.begin() != cursor.end();
	}
	catch (const std::exception& e) {
		std::cerr << "Error al verificar existencia de cuentas en BD: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Obtiene el número total de cuentas registradas en la base de datos
 */
long _BaseDatosPersona::obtenerTotalCuentasRegistradas() {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Pipeline de agregación para contar todas las cuentas
		mongocxx::pipeline pipeline;

		// Desenrollar el array de cuentas
		pipeline.unwind("$cuentas");

		// Contar los documentos resultantes
		pipeline.count("totalCuentas");

		auto cursor = collection.aggregate(pipeline);

		for (auto&& doc : cursor) {
			auto totalElement = doc["totalCuentas"];
			if (totalElement && totalElement.type() == bsoncxx::type::k_int32) {
				return static_cast<long>(totalElement.get_int32().value);
			}
			else if (totalElement && totalElement.type() == bsoncxx::type::k_int64) {
				return static_cast<long>(totalElement.get_int64().value);
			}
		}

		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al obtener total de cuentas: " << e.what() << std::endl;
		return 0;
	}
}

/**
 * @brief Obtiene todas las personas registradas en la base de datos MongoDB
 * @return Vector de documentos BSON con todas las personas encontradas
 */
std::vector<bsoncxx::document::value> _BaseDatosPersona::mostrarTodasPersonas() {
	std::vector<bsoncxx::document::value> resultados;

	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Pipeline de agregación optimizado para obtener datos esenciales
		mongocxx::pipeline pipeline;

		// Proyección de solo los campos necesarios para eficiencia
		pipeline.project(make_document(
			kvp("cedula", 1),
			kvp("nombre", 1),
			kvp("apellido", 1),
			kvp("correo", 1),
			kvp("cuentas.numeroCuenta", 1),
			kvp("cuentas.tipo", 1),
			kvp("cuentas.saldo", 1),
			kvp("totalCuentasExistentes", 1)
		));

		// Ordenar por apellido y nombre por defecto
		pipeline.sort(make_document(
			kvp("apellido", 1),
			kvp("nombre", 1)
		));

		auto cursor = collection.aggregate(pipeline);

		// Reservar espacio estimado para mejor rendimiento
		resultados.reserve(100);

		std::for_each(cursor.begin(), cursor.end(), [&resultados](const auto& doc) {
			resultados.emplace_back(bsoncxx::document::value(doc));
			});

	}
	catch (const std::exception& e) {
		std::cerr << "Error al obtener todas las personas: " << e.what() << std::endl;
	}

	return resultados;
}