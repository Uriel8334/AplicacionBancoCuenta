#define _CRT_SECURE_NO_WARNINGS
 
#include "_BaseDatosPersona.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <iostream>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <string>
#include "Persona.h"


_BaseDatosPersona::_BaseDatosPersona(mongocxx::client& client)
	: _client(client) {
}

/**
 *@brief Inserta una nueva persona en la base de datos MongoDB
 *
 * @param persona La persona a insertar
 * @return true si la inserción fue exitosa, false en caso contrario
 */
bool _BaseDatosPersona::insertarPersona(const Persona& persona) {
	try {
		auto db = _client["Banco"];
		auto collection = db["personas"];

		// Construye el documento BSON a partir del objeto Persona
		auto doc = make_document(
			kvp("cedula", persona.getCedula()),
			kvp("nombre", persona.getNombres()),
			kvp("apellido", persona.getApellidos()),
			kvp("fechaNacimiento", persona.getFechaNacimiento()),
			kvp("correo", persona.getCorreo()),
			kvp("direccion", persona.getDireccion()),
			kvp("numCuentas", persona.getNumCuentas()),
			kvp("numCorrientes", persona.getNumCorrientes()),
			kvp("cuentas", bsoncxx::builder::basic::array{}) // <-- Array vacío de cuentas
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
			numCuentas = std::distance(cuentasElement.get_array().value.begin(), cuentasElement.get_array().value.end());
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