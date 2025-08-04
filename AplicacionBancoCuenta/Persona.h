#pragma once  
#ifndef PERSONA_H  
#define PERSONA_H  

#include <iostream>
#include <string>  
#include "CuentaAhorros.h"  
#include "CuentaCorriente.h"  
#include <functional>
#include <vector>
#include <memory>

// Forward declarations para evitar dependencias circulares
class PersonaValidator;
class PersonaDataProcessor;

/**
 * @namespace PersonaUI
 * @brief Namespace que contiene funciones de utilidad para la interfaz de usuario de Persona
 */
namespace PersonaUI {
    extern const std::function<bool(const std::string&)> seleccionarSiNo;
    extern const std::function<double(double, double, const std::string&)> ingresarMonto;
}

using namespace std;

/**
 * @class Persona
 * @brief Clase que representa a una persona cliente del banco
 * Refactorizada aplicando principios SOLID
 */
class Persona {
private:
    // === DATOS MIEMBRO ===
    string cedula;
    string nombres;
    string apellidos;
    string fechaNacimiento;
    string correo;
    string direccion;
    CuentaAhorros* cabezaAhorros;
    CuentaCorriente* cabezaCorriente;
    int numCuentas = 0;
    int numCorrientes = 0;
    bool isDestroyed = false;

    // === COLABORADORES (Dependency Injection) ===
    std::unique_ptr<PersonaValidator> validator;
    std::unique_ptr<PersonaDataProcessor> dataProcessor;

    // === MÉTODOS PRIVADOS REFACTORIZADOS ===
    int obtenerMayorSecuencialEnMemoria(const std::string& sucursal);

    // Métodos recursivos para manejo de listas enlazadas
    template<typename T>
    void liberarListaRecursivo(T* nodo);

    template<typename T>
    T* buscarEnListaRecursivo(T* nodo, const std::function<bool(T*)>& criterio);

    template<typename T>
    T* configurarCabezaLista(T* nuevaCabeza, T*& cabezaActual);

    // Input processors refactorizados
    std::string procesarEntradaConValidacion(const std::string& tipo, const std::string& prompt, const std::function<std::string()>& inputFunction);

public:
    // === CONSTRUCTORES Y DESTRUCTOR ===
    Persona();
    Persona(const string& cedula, const string& nombres, const string& apellidos,
        const string& fechaNacimiento, const string& correo, const string& direccion);
    ~Persona();

    // === VALIDACIÓN ===
    bool isValidInstance() const { return !isDestroyed; }

    // === SETTERS (aplicando Builder Pattern) ===
    Persona& setCedula(const string& cedula);
    Persona& setNombres(const string& nombres);
    Persona& setApellidos(const string& apellidos);
    Persona& setFechaNacimiento(const string& fechaNacimiento);
    Persona& setCorreo(const string& correo);
    Persona& setDireccion(const string& direccion);
    void setNumeCuentas(int numCuentas) { this->numCuentas = numCuentas; }
    void setNumCorrientes(int numCorrientes) { this->numCorrientes = numCorrientes; }

    CuentaAhorros* setCabezaAhorros(CuentaAhorros* nuevaCabeza);
    CuentaCorriente* setCabezaCorriente(CuentaCorriente* nuevaCabeza);

    // === GETTERS ===
    string getCedula() const { return this->cedula; }
    string getNombres() const { return this->nombres; }
    string getApellidos() const { return this->apellidos; }
    string getFechaNacimiento() const { return this->fechaNacimiento; }
    string getCorreo() const { return this->correo; }
    string getDireccion() const { return this->direccion; }
    int getNumCuentas() const { return this->numCuentas; }
    int getNumCorrientes() const { return this->numCorrientes; }
    CuentaAhorros* getCabezaAhorros() const { return this->cabezaAhorros; }
    CuentaCorriente* getCabezaCorriente() const { return this->cabezaCorriente; }

    // === MÉTODOS DE ENTRADA DE DATOS REFACTORIZADOS ===
    void ingresarDatos();
    void ingresarDatos(const std::string& cedulaEsperada) ;

    // Métodos específicos de entrada (aplicando Command Pattern)
    std::string ingresarCedula(std::string& cedula);
    std::string ingresarNombres(std::string& nombres) const;
    std::string ingresarApellidos(std::string& apellidos) const;
    std::string ingresarFechaNacimiento(std::string& fechaNacimiento);
    std::string ingresarCorreo(std::string& correo) const;
    std::string ingresarDireccion(std::string& direccion) const;

    // === MÉTODOS DE PRESENTACIÓN ===
    bool corregirDatos();
    void mostrarDatos() const;
    int mostrarCuentas(const std::string& tipoCuenta) const;

    // === MÉTODOS DE PERSISTENCIA ===
    void guardarEnArchivo() const;
    int guardarCuentas(std::ofstream& archivo, std::string tipo) const;

    // === MÉTODOS DE BÚSQUEDA REFACTORIZADOS ===
    int buscarPersonaPorCriterio(const std::string& criterioBusqueda, const std::string& numeroCuenta,
        const std::string& fechaApertura, double saldo) const;
    void buscarPersonaPorFecha(const std::string& fechaApertura) const;
    int buscarPersonaPorCuentas(const string& numeroCuenta) const;

    // === MÉTODOS DE CREACIÓN DE CUENTAS ===
    bool crearAgregarCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada);
    bool crearAgregarCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada);
    bool crearSoloCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada);
    bool crearSoloCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada);

    // === MÉTODOS UTILITARIOS ===
    std::string crearNumeroCuenta(Cuenta<double>* nuevaCuenta, const std::string& sucursal);
    std::string seleccionSucursal();
    std::string msgIngresoDatos() const;

    // === MÉTODOS FUNCIONALES NUEVOS ===
    std::vector<CuentaAhorros*> obtenerCuentasAhorros() const;
    std::vector<CuentaCorriente*> obtenerCuentasCorriente() const;

    // Métodos que aplican programación funcional
    void forEachCuentaAhorros(const std::function<void(CuentaAhorros*)>& accion) const;
    void forEachCuentaCorriente(const std::function<void(CuentaCorriente*)>& accion) const;

    // Métodos de filtrado
    std::vector<CuentaAhorros*> filtrarCuentasAhorros(const std::function<bool(CuentaAhorros*)>& filtro) const;
    std::vector<CuentaCorriente*> filtrarCuentasCorriente(const std::function<bool(CuentaCorriente*)>& filtro) const;
};

#endif // PERSONA_H