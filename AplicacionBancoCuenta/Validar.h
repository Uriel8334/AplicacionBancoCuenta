#pragma once
// Validar.h
#ifndef VALIDAR_H
#define VALIDAR_H

// Validar.h de cabecera de la clase Validar
#include <iostream>
#include <string>
#include <regex>
#include <stdexcept>
#include <Windows.h>

enum class TipoEntrada {
	NUMERICO_CEDULA,
	NUMERICO_CUENTA,
	ALFABETICO,
	ALFANUMERICO,
	CORREO,
	TELEFONO
};

class Validar {
public:
#pragma region VALIDACIONES PARA CUENTA
	// Valida que el numero de cuenta sea valido 
	static bool ValidarNumeroCuenta(const std::string numero);

	// Valida que el saldo sea valido, valor entero, no decimal de dolares USD
	static bool ValidarSaldo(const std::string& saldoStr);

	// Valida el estado de la cuenta 
	static bool ValidarEstadoCuenta(const std::string& estado);

	// Valida el tipo de cuenta, ahorros o corriente
	static bool ValidarTipoCuenta(const std::string& tipo);

	// Valida el tipo de transaccion, retiro o deposito
	static bool ValidarTipoTransaccion(const std::string& tipo);

	// Valida el monto/numero de transaccion, solo numeros enteros
	static bool ValidarMontoTransaccion(int monto);
#pragma endregion

	// Valida el formato de la fecha 
	static bool ValidarFecha(const std::string& fecha);

	//Validar el numero con dos decimales para el monto o saldo 
	static bool ValidarNumeroConDosDecimales(const std::string& monto);

	// Valida el poder copiar y pegar el monto desde el portapapeles
	static std::string leerDesdePortapapeles();

#pragma region VALIDACIONES PARA PERSONA
	// Valida que el nombre del cliente sea valido, solo letras y espacios
	static bool ValidarNombrePersona(const std::string& nombre);

	// Validar el numero de cedula de 10 digitos 
	static bool ValidarCedula(const std::string& cedula);

	static bool todosLosDigitosIguales(const std::string& cedula);

	// Valida el numero de celular de 10 digitos
	/*static bool ValidarCelular(const std::string& celular);

	// Valida el correo electronico, solo letras, numeros y caracteres especiales
	static bool ValidarCorreo(const std::string& correo);

	// Valida el formato de la fecha de nacimiento que no sea mayor a la fecha actual, que sea menor a 130 años y mayor a 0 años
	static bool ValidarFechaNacimiento(const std::string& fechaNacimiento);*/

	// Valida la tecla que se ingresa
	static bool ValidarTecla(char tecla, TipoEntrada tipo);

	// Valida la busqueda del numero de cuenta
	static std::string ValidarLeerNumeroCuenta();
#pragma endregion

	// Verifica si un archivo ya existe
	static bool archivoExiste(const std::string& rutaArchivo);

	// Genera un nombre de respaldo único
	static std::string generarNombreConIndice(const std::string& rutaDirectorio, const std::string& fechaFormateada);



};
#endif // VALIDAR_H
