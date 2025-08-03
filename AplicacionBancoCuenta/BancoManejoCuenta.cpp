/**
 * @file BancoManejoCuenta.cpp
 */
#include "BancoManejoCuenta.h"
#include "ConexionMongo.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>
#include <vector>
#include "Cuenta.h"

BancoManejoCuenta::BancoManejoCuenta(BancoManejoPersona& manejadorPersonas)
	: manejoPersonas(manejadorPersonas) {
}

// Implementación recursiva para búsqueda de cuentas
template<typename TipoCuenta>
TipoCuenta* BancoManejoCuenta::buscarCuentaRecursivo(TipoCuenta* cuenta, const std::string& numeroCuenta) {
	if (!cuenta) return nullptr;

	if (cuenta->getNumeroCuenta() == numeroCuenta) {
		return cuenta;
	}

	return buscarCuentaRecursivo(cuenta->getSiguiente(), numeroCuenta);
}

std::pair<CuentaAhorros*, Persona*> BancoManejoCuenta::buscarCuentaAhorros(const std::string& numeroCuenta) {
	std::pair<CuentaAhorros*, Persona*> resultado = { nullptr, nullptr };

	manejoPersonas.forEachPersona([&](Persona* persona) {
		if (resultado.first) return; // Ya encontrado

		CuentaAhorros* cuenta = buscarCuentaRecursivo(persona->getCabezaAhorros(), numeroCuenta);
		if (cuenta) {
			resultado = { cuenta, persona };
		}
		});

	return resultado;
}

std::pair<CuentaCorriente*, Persona*> BancoManejoCuenta::buscarCuentaCorriente(const std::string& numeroCuenta) {
	std::pair<CuentaCorriente*, Persona*> resultado = { nullptr, nullptr };

	manejoPersonas.forEachPersona([&](Persona* persona) {
		if (resultado.first) return; // Ya encontrado

		CuentaCorriente* cuenta = buscarCuentaRecursivo(persona->getCabezaCorriente(), numeroCuenta);
		if (cuenta) {
			resultado = { cuenta, persona };
		}
		});

	return resultado;
}

// Búsqueda recursiva por criterios múltiples
void BancoManejoCuenta::buscarCuentasPorCriterioRecursivo(NodoPersona* nodo, const std::string& criterio, const std::string& valor, std::vector<std::pair<Persona*, void*>>& resultados) {
	if (!nodo || !nodo->persona) return;

	// Buscar en cuentas de ahorros
	std::function<void(CuentaAhorros*)> buscarEnAhorros = [&](CuentaAhorros* cuenta) {
		if (!cuenta) return;

		bool coincide = false;
		if (criterio == "numero" && cuenta->getCuentaAhorros()->getNumeroCuenta() == valor) coincide = true;
		if (criterio == "fecha" && cuenta->getCuentaAhorros()->getFechaApertura() == valor) coincide = true;

		if (coincide) {
			resultados.push_back({ nodo->persona, cuenta->getCuentaAhorros() });
		}

		buscarEnAhorros(cuenta->getSiguiente());
		};

	// Buscar en cuentas corrientes
	std::function<void(CuentaCorriente*)> buscarEnCorrientes = [&](CuentaCorriente* cuenta) {
		if (!cuenta) return;

		bool coincide = false;
		if (criterio == "numero" && cuenta->getCuentaCorriente()->getNumeroCuenta() == valor) coincide = true;
		if (criterio == "fecha" && cuenta->getCuentaCorriente()->getFechaApertura() == valor) coincide = true;

		if (coincide) {
			resultados.push_back({ nodo->persona, cuenta->getCuentaCorriente() });
		}

		buscarEnCorrientes(cuenta->getSiguiente());
		};

	buscarEnAhorros(nodo->persona->getCabezaAhorros());
	buscarEnCorrientes(nodo->persona->getCabezaCorriente());

	// Recursión al siguiente nodo
	buscarCuentasPorCriterioRecursivo(nodo->siguiente, criterio, valor, resultados);
}

std::vector<std::pair<Persona*, void*>> BancoManejoCuenta::buscarCuentasPorNumero(const std::string& numero) {
	std::vector<std::pair<Persona*, void*>> resultados;
	buscarCuentasPorCriterioRecursivo(manejoPersonas.getListaPersonas(), "numero", numero, resultados);
	return resultados;
}

bool BancoManejoCuenta::crearCuentaAhorros(const std::string& cedula, CuentaAhorros* cuenta) {
	if (!cuenta) return false;

	// Crear documento para MongoDB
	try {
		mongocxx::client& client = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona dbPersona(client);

		auto cuentaDoc = bsoncxx::builder::basic::document{};
		cuentaDoc.append(
			bsoncxx::builder::basic::kvp("tipo", "ahorros"),
			bsoncxx::builder::basic::kvp("numeroCuenta", cuenta->getNumeroCuenta()),
			bsoncxx::builder::basic::kvp("saldo", cuenta->getSaldo()),
			bsoncxx::builder::basic::kvp("fechaApertura", cuenta->getFechaApertura()),
			bsoncxx::builder::basic::kvp("estado", cuenta->getEstadoCuenta())
		);

		return dbPersona.agregarCuentaPersona(cedula, cuentaDoc.extract());
	}
	catch (const std::exception& e) {
		std::cerr << "Error creando cuenta: " << e.what() << std::endl;
		return false;
	}
}

bool BancoManejoCuenta::crearCuentaCorriente(const std::string& cedula, CuentaCorriente* cuenta) {
	if (!cuenta) return false;

	// Crear documento para MongoDB
	try {
		mongocxx::client& client = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona dbPersona(client);

		auto cuentaDoc = bsoncxx::builder::basic::document{};
		cuentaDoc.append(
			bsoncxx::builder::basic::kvp("tipo", "corriente"),
			bsoncxx::builder::basic::kvp("numeroCuenta", cuenta->getNumeroCuenta()),
			bsoncxx::builder::basic::kvp("saldo", cuenta->getSaldo()),
			bsoncxx::builder::basic::kvp("fechaApertura", cuenta->getFechaApertura()),
			bsoncxx::builder::basic::kvp("estado", cuenta->getEstadoCuenta())
		);

		return dbPersona.agregarCuentaPersona(cedula, cuentaDoc.extract());
	}
	catch (const std::exception& e) {
		std::cerr << "Error creando cuenta corriente: " << e.what() << std::endl;
		return false;
	}
}

// === IMPLEMENTACIONES DE MÉTODOS OPERACIONALES ===

bool BancoManejoCuenta::depositar(const std::string& numeroCuenta, double monto) {
	try {
		mongocxx::client& client = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona dbPersona(client);

		return dbPersona.depositarEnCuenta(numeroCuenta, monto);
	}
	catch (const std::exception& e) {
		std::cerr << "Error en depósito: " << e.what() << std::endl;
		return false;
	}
}

bool BancoManejoCuenta::retirar(const std::string& numeroCuenta, double monto) {
	try {
		mongocxx::client& client = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona dbPersona(client);

		return dbPersona.retirarDeCuenta(numeroCuenta, monto);
	}
	catch (const std::exception& e) {
		std::cerr << "Error en retiro: " << e.what() << std::endl;
		return false;
	}
}

double BancoManejoCuenta::consultarSaldo(const std::string& numeroCuenta) {
	try {
		mongocxx::client& client = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona dbPersona(client);

		return dbPersona.obtenerSaldoCuenta(numeroCuenta);
	}
	catch (const std::exception& e) {
		std::cerr << "Error al consultar saldo: " << e.what() << std::endl;
		return -1.0;
	}
}

bool BancoManejoCuenta::validarFondosSuficientes(const std::string& numeroCuenta, double monto) {
	try {
		mongocxx::client& client = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona dbPersona(client);

		return dbPersona.verificarFondosSuficientes(numeroCuenta, monto);
	}
	catch (const std::exception& e) {
		std::cerr << "Error al validar fondos: " << e.what() << std::endl;
		return false;
	}
}

bool BancoManejoCuenta::transferir(const std::string& cuentaOrigen, const std::string& cuentaDestino, double monto) {
	try {
		mongocxx::client& client = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona dbPersona(client);

		return dbPersona.realizarTransferencia(cuentaOrigen, cuentaDestino, monto);
	}
	catch (const std::exception& e) {
		std::cerr << "Error en transferencia: " << e.what() << std::endl;
		return false;
	}
}

std::string BancoManejoCuenta::obtenerInformacionCompleta(const std::string& numeroCuenta) {
	try {
		mongocxx::client& client = ConexionMongo::obtenerClienteBaseDatos();
		_BaseDatosPersona dbPersona(client);

		auto infoDoc = dbPersona.obtenerInformacionCuenta(numeroCuenta);

		if (infoDoc.view().empty()) {
			return "Cuenta no encontrada";
		}

		// Convertir el documento BSON a JSON legible
		std::string jsonStr = bsoncxx::to_json(infoDoc.view());

		// Formatear para mejor legibilidad (opcional)
		return jsonStr;
	}
	catch (const std::exception& e) {
		std::cerr << "Error al obtener información: " << e.what() << std::endl;
		return "Error al obtener información de la cuenta";
	}
}

bool BancoManejoCuenta::existeCuenta(const std::string& numeroCuenta) {
	double saldo = consultarSaldo(numeroCuenta);
	return saldo >= 0.0; // Si devuelve -1.0, la cuenta no existe
}

// === IMPLEMENTACIONES DE MÉTODOS FALTANTES ===

std::vector<std::pair<Persona*, void*>> BancoManejoCuenta::buscarCuentasPorFecha(const std::string& fecha) {
	std::vector<std::pair<Persona*, void*>> resultados;
	buscarCuentasPorCriterioRecursivo(manejoPersonas.getListaPersonas(), "fecha", fecha, resultados);
	return resultados;
}

std::vector<std::pair<Persona*, void*>> BancoManejoCuenta::buscarCuentasPorSaldo(double saldoMinimo) {
	std::vector<std::pair<Persona*, void*>> resultados;

	// Esta implementación necesita lógica diferente ya que el saldo se debe comparar numéricamente
	manejoPersonas.forEachPersona([&](Persona* persona) {
		// Buscar en cuentas de ahorros
		std::function<void(CuentaAhorros*)> buscarEnAhorros = [&](CuentaAhorros* cuenta) {
			if (!cuenta || !cuenta->getCuentaAhorros()) return;

			if (cuenta->getCuentaAhorros()->getSaldo() >= saldoMinimo) {
				resultados.push_back({ persona, cuenta->getCuentaAhorros() });
			}

			buscarEnAhorros(cuenta->getSiguiente());
			};

		// Buscar en cuentas corrientes
		std::function<void(CuentaCorriente*)> buscarEnCorrientes = [&](CuentaCorriente* cuenta) {
			if (!cuenta || !cuenta->getCuentaCorriente()) return;

			if (cuenta->getCuentaCorriente()->getSaldo() >= saldoMinimo) {
				resultados.push_back({ persona, cuenta->getCuentaCorriente() });
			}

			buscarEnCorrientes(cuenta->getSiguiente());
			};

		buscarEnAhorros(persona->getCabezaAhorros());
		buscarEnCorrientes(persona->getCabezaCorriente());
		});

	return resultados;
}

std::vector<std::pair<Persona*, void*>> BancoManejoCuenta::buscarCuentasPorCedula(const std::string& cedula) {
	std::vector<std::pair<Persona*, void*>> resultados;

	Persona* persona = manejoPersonas.buscarPersonaPorCedula(cedula);
	if (!persona) return resultados;

	// Agregar todas las cuentas de esta persona
	std::function<void(CuentaAhorros*)> agregarAhorros = [&](CuentaAhorros* cuenta) {
		if (!cuenta || !cuenta->getCuentaAhorros()) return;

		resultados.push_back({ persona, cuenta->getCuentaAhorros() });
		agregarAhorros(cuenta->getSiguiente());
		};

	std::function<void(CuentaCorriente*)> agregarCorrientes = [&](CuentaCorriente* cuenta) {
		if (!cuenta || !cuenta->getCuentaCorriente()) return;

		resultados.push_back({ persona, cuenta->getCuentaCorriente() });
		agregarCorrientes(cuenta->getSiguiente());
		};

	agregarAhorros(persona->getCabezaAhorros());
	agregarCorrientes(persona->getCabezaCorriente());

	return resultados;
}