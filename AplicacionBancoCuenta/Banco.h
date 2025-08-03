#pragma once
#ifndef BANCO_H
#define BANCO_H

// === FORWARD DECLARATIONS ===
class BancoManejoPersona;
class BancoManejoCuenta;
class BancoManejaRegistro;
class _BaseDatosPersona;
class BuscadorCuentas;
class CreadorCuentas;
class ValidadorBaseDatos;
class Persona;
class NodoPersona;
class CuentaAhorros;
class CuentaCorriente;

#include "_BaseDatosPersona.h"
#include <string>
#include <tuple>
#include <memory>
#include <functional>

class Banco {
private:
	std::unique_ptr<BancoManejoPersona> manejoPersonas;
	std::unique_ptr<BancoManejoCuenta> manejoCuentas;
	std::unique_ptr<BancoManejaRegistro> manejoRegistros;
	_BaseDatosPersona baseDatosPersona;
	std::unique_ptr<BuscadorCuentas> buscadorCuentas;
	std::unique_ptr<ValidadorBaseDatos> validadorBaseDatos;

	// === MÉTODOS REFACTORIZADOS APLICANDO SOLID ===

	// Extract Method - Métodos auxiliares pequeños y específicos
	void mostrarMensajeCreacionCliente();
	std::unique_ptr<Persona> prepararNuevaPersona(const std::string& cedula);
	std::pair<bool, std::string> crearCuentaSegunTipo(const std::string& tipoCuenta, Persona* persona, const std::string& cedula);
	bool persistirPersonaEnBaseDatos(const Persona& persona);

	// Template Method Pattern - Para personas existentes
	std::unique_ptr<Persona> obtenerPersonaExistente(const std::string& cedula);
	std::pair<bool, std::string> crearCuentaParaPersonaExistente(CreadorCuentas& creador, const std::string& tipoCuenta, Persona* persona, const std::string& cedula);

	// === MÉTODOS EXISTENTES ===
	bool procesarPersonaExistente(Persona* persona, const std::string& tipoCuenta, const std::string& cedula);
	bool procesarPersonaNueva(const std::string& tipoCuenta, const std::string& cedula);
	bool crearCuentaPorTipo(const std::string& tipoCuenta, const std::string& cedula, const std::string& nombreCompleto = "");
	void mostrarResultadoCreacion(bool exito, const std::string& tipoCuenta, const std::string& nombreCompleto);
	void finalizarOperacion(bool exitoso, const std::string& cedula, const std::string& tipoOperacion);
	std::tuple<std::string, std::string, int> obtenerDatosIniciales();
	bool validarOperacion(const std::string& operacion);
	void registrarOperacion(const std::string& operacion);
	void realizarDeposito();
	void realizarRetiro();
	void consultarSaldo();
	void mostrarInformacionCuenta();

public:

	Banco();
	~Banco();

	void agregarPersonaConCuenta();
	void buscarCuenta();
	bool verificarCuentasBanco() const;
	void realizarTransferencia();
	void subMenuCuentasBancarias();

	// Acceso a los manejadores (si es necesario)
	BancoManejoPersona& getManejoPersonas();
	BancoManejoCuenta& getManejoCuentas();

	// Compatibilidad con código existente
	NodoPersona* getListaPersonas() const;
	void setListaPersonas(NodoPersona* lista);
	void forEachPersona(const std::function<void(Persona*)>& funcion) const;
};

#endif // BANCO_H