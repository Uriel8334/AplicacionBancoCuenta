#pragma once  
#ifndef PERSONA_H  
#define PERSONA_H  

#include <iostream>
#include <string>  
#include "CuentaAhorros.h"  
#include "CuentaCorriente.h"  
#include <functional>
#include <vector>

/**
 * @namespace PersonaUI
 * @brief Namespace que contiene funciones de utilidad para la interfaz de usuario de Persona
 *
 * Proporciona funciones lambda reutilizables para interactuar con el usuario
 * en el contexto de la gestión de personas y cuentas.
 */
namespace PersonaUI {
    /**
     * @brief Función para solicitar una confirmación (Sí/No) al usuario
     * @param mensaje El mensaje a mostrar para la solicitud
     * @return true si el usuario selecciona "Sí", false en caso contrario
     */
    extern const std::function<bool(const std::string&)> seleccionarSiNo;

    /**
     * @brief Función para solicitar el ingreso de un monto con validación
     * @param min Valor mínimo aceptable
     * @param max Valor máximo aceptable
     * @param mensaje El mensaje a mostrar para la solicitud
     * @return El monto ingresado y validado por el usuario
     */
    extern const std::function<double(double, double, const std::string&)> ingresarMonto;
}

using namespace std;

/**
 * @class Persona
 * @brief Clase que representa a una persona cliente del banco
 *
 * Esta clase gestiona la información personal de un cliente y administra
 * sus cuentas bancarias (tanto de ahorros como corrientes). Implementa
 * operaciones para crear, buscar y gestionar cuentas asociadas a la persona.
 */
class Persona {
private:
    /** @brief Número de cédula (identificación) de la persona */
    string cedula;

    /** @brief Nombres de la persona */
    string nombres;

    /** @brief Apellidos de la persona */
    string apellidos;

    /** @brief Fecha de nacimiento en formato de cadena */
    string fechaNacimiento;

    /** @brief Dirección de correo electrónico */
    string correo;

    /** @brief Dirección domiciliaria */
    string direccion;

    /** @brief Puntero a la primera cuenta de ahorros en una lista enlazada */
    CuentaAhorros* cabezaAhorros;

    /** @brief Puntero a la primera cuenta corriente en una lista enlazada */
    CuentaCorriente* cabezaCorriente;

    /** @brief Número de cuentas de ahorro (limitadas a 5 por persona) */
    int numCuentas = 0;

    /** @brief Número de cuentas corrientes (sin límite) */
    int numCorrientes = 0;

    /** @brief Indicador de si el objeto ha sido destruido, para evitar uso después de su destrucción */
    bool isDestroyed = false;

    /**
     * @brief Obtiene el mayor número secuencial de cuentas en memoria para una sucursal
     * @param sucursal Código de sucursal
     * @return Mayor número secuencial encontrado en memoria
     */
    int obtenerMayorSecuencialEnMemoria(const std::string& sucursal);

public:
    /**
     * @brief Constructor por defecto
     *
     * Inicializa una nueva persona con valores vacíos y sin cuentas asociadas
     */
    Persona() : cabezaAhorros(nullptr), cabezaCorriente(nullptr), numCuentas(0), numCorrientes(0), isDestroyed(false) {}

    /**
     * @brief Constructor con parámetros
     *
     * @param cedula Número de identificación de la persona
     * @param nombres Nombres de la persona
     * @param apellidos Apellidos de la persona
     * @param fechaNacimiento Fecha de nacimiento en formato de cadena
     * @param correo Dirección de correo electrónico
     * @param direccion Dirección domiciliaria
     */
    Persona(const string& cedula, const string& nombres, const string& apellidos,
        const string& fechaNacimiento, const string& correo, const string& direccion)
        : cedula(cedula), nombres(nombres), apellidos(apellidos),
        fechaNacimiento(fechaNacimiento), correo(correo), direccion(direccion),
        cabezaAhorros(nullptr), cabezaCorriente(nullptr), numCuentas(0), numCorrientes(0), isDestroyed(false) {
    }

    /**
     * @brief Destructor
     *
     * Libera la memoria de todas las cuentas asociadas a esta persona
     */
    ~Persona() {
        // Liberar memoria de las cuentas de ahorros  
        while (cabezaAhorros != nullptr) {
            CuentaAhorros* temp = cabezaAhorros;
            cabezaAhorros = static_cast<CuentaAhorros*>(cabezaAhorros->getSiguiente());
            delete temp;
        }

        // Liberar memoria de las cuentas corrientes
        while (cabezaCorriente != nullptr) {
            CuentaCorriente* temp = cabezaCorriente;
            cabezaCorriente = static_cast<CuentaCorriente*>(cabezaCorriente->getSiguiente());
            delete temp;
        }
        isDestroyed = true; // Marcar como destruido  
    }

    /**
     * @brief Verifica si la instancia es válida (no ha sido destruida)
     * @return true si la instancia es válida, false en caso contrario
     */
    bool isValidInstance() const { return !isDestroyed; }

    /**
     * @brief Establece el número de cédula
     * @param cedula Nueva cédula
     * @return true si se asignó correctamente
     */
    bool setCedula(const string& cedula) { this->cedula = cedula; return true; }

    /**
     * @brief Establece los nombres
     * @param nombres Nuevos nombres
     * @return true si se asignó correctamente
     */
    bool setNombres(const string& nombres) { this->nombres = nombres; return true; }

    /**
     * @brief Establece los apellidos
     * @param apellidos Nuevos apellidos
     * @return true si se asignó correctamente
     */
    bool setApellidos(const string& apellidos) { this->apellidos = apellidos; return true; }

    /**
     * @brief Establece la fecha de nacimiento
     * @param fechaNacimiento Nueva fecha de nacimiento
     * @return true si se asignó correctamente
     */
    bool setFechaNacimiento(const string& fechaNacimiento) { this->fechaNacimiento = fechaNacimiento; return true; }

    /**
     * @brief Establece el correo electrónico
     * @param correo Nuevo correo electrónico
     * @return true si se asignó correctamente
     */
    bool setCorreo(const string& correo) { this->correo = correo; return true; }

    /**
     * @brief Establece la dirección
     * @param direccion Nueva dirección
     * @return true si se asignó correctamente
     */
    bool setDireccion(const string& direccion) { this->direccion = direccion; return true; }

    /**
     * @brief Establece el número de cuentas de ahorro
     * @param numCuentas Nuevo número de cuentas
     */
    void setNumeCuentas(int numCuentas) { this->numCuentas = numCuentas; }

    /**
     * @brief Establece el número de cuentas corrientes
     * @param numCorrientes Nuevo número de cuentas corrientes
     */
    void setNumCorrientes(int numCorrientes) { this->numCorrientes = numCorrientes; }

    /**
     * @brief Establece la cabeza de la lista de cuentas de ahorro
     * @param nuevaCabeza Puntero a la nueva cuenta que será la cabeza
     * @return Puntero a la nueva cabeza
     */
    CuentaAhorros* setCabezaAhorros(CuentaAhorros* nuevaCabeza) {
        if (nuevaCabeza) {
            nuevaCabeza->setSiguiente(cabezaAhorros);
            nuevaCabeza->setAnterior(nullptr);

            if (cabezaAhorros)
                cabezaAhorros->setAnterior(nuevaCabeza);
        }

        cabezaAhorros = nuevaCabeza;
        return cabezaAhorros;
    }

    /**
     * @brief Establece la cabeza de la lista de cuentas corrientes
     * @param nuevaCabeza Puntero a la nueva cuenta que será la cabeza
     * @return Puntero a la nueva cabeza
     */
    CuentaCorriente* setCabezaCorriente(CuentaCorriente* nuevaCabeza) {
        if (nuevaCabeza)
        {
            nuevaCabeza->setSiguiente(cabezaCorriente);
            nuevaCabeza->setAnterior(nullptr);
            if (cabezaCorriente)
                cabezaCorriente->setAnterior(nuevaCabeza);
        }
        cabezaCorriente = nuevaCabeza;
        return cabezaCorriente;
    }

    /**
     * @brief Obtiene la cédula de la persona
     * @return Cédula como cadena
     */
    string getCedula() const { return this->cedula; }

    /**
     * @brief Obtiene los nombres de la persona
     * @return Nombres como cadena
     */
    string getNombres() const { return this->nombres; }

    /**
     * @brief Obtiene los apellidos de la persona
     * @return Apellidos como cadena
     */
    string getApellidos() const { return this->apellidos; }

    /**
     * @brief Obtiene la fecha de nacimiento
     * @return Fecha de nacimiento como cadena
     */
    string getFechaNacimiento() const { return this->fechaNacimiento; }

    /**
     * @brief Obtiene el correo electrónico
     * @return Correo electrónico como cadena
     */
    string getCorreo() const { return this->correo; }

    /**
     * @brief Obtiene la dirección
     * @return Dirección como cadena
     */
    string getDireccion() const { return this->direccion; }

    /**
     * @brief Obtiene el número de cuentas de ahorro
     * @return Número de cuentas de ahorro
     */
    int getNumCuentas() const { return this->numCuentas; }

    /**
     * @brief Obtiene el número de cuentas corrientes
     * @return Número de cuentas corrientes
     */
    int getNumCorrientes() const { return this->numCorrientes; }

    /**
     * @brief Obtiene la cabeza de la lista de cuentas de ahorro
     * @return Puntero a la primera cuenta de ahorros
     */
    CuentaAhorros* getCabezaAhorros() const { return this->cabezaAhorros; }

    /**
     * @brief Obtiene la cabeza de la lista de cuentas corrientes
     * @return Puntero a la primera cuenta corriente
     */
    CuentaCorriente* getCabezaCorriente() const { return this->cabezaCorriente; }

    /**
     * @brief Solicita al usuario ingresar todos sus datos personales
     */
    void ingresarDatos();

    /**
     * @brief Solicita al usuario ingresar datos con una cédula predeterminada
     * @param cedulaEsperada Cédula que se espera validar
     */
    void ingresarDatos(const std::string& cedulaEsperada);

    /**
     * @brief Solicita al usuario ingresar su número de cédula
     * @param cedula Variable donde se almacenará la cédula
     * @return Cédula ingresada y validada
     */
    std::string ingresarCedula(std::string& cedula);

    /**
     * @brief Solicita al usuario ingresar sus nombres
     * @param nombres Variable donde se almacenarán los nombres
     * @return Nombres ingresados y validados
     */
    std::string ingresarNombres(std::string& nombres) const;

    /**
     * @brief Solicita al usuario ingresar sus apellidos
     * @param apellidos Variable donde se almacenarán los apellidos
     * @return Apellidos ingresados y validados
     */
    std::string ingresarApellidos(std::string& apellidos) const;

    /**
     * @brief Solicita al usuario ingresar su fecha de nacimiento
     * @param fechaNacimiento Variable donde se almacenará la fecha
     * @return Fecha de nacimiento ingresada y validada
     */
    std::string ingresarFechaNacimiento(std::string& fechaNacimiento);

    /**
     * @brief Solicita al usuario ingresar su correo electrónico
     * @param correo Variable donde se almacenará el correo
     * @return Correo electrónico ingresado y validado
     */
    std::string ingresarCorreo(std::string& correo) const;

    /**
     * @brief Solicita al usuario ingresar su dirección
     * @param direccion Variable donde se almacenará la dirección
     * @return Dirección ingresada y validada
     */
    std::string ingresarDireccion(std::string& direccion) const;

    /**
     * @brief Permite al usuario corregir sus datos personales
     * @return true si se modificaron datos, false si no
     */
    bool corregirDatos();

    /**
     * @brief Muestra los datos personales del cliente
     */
    void mostrarDatos() const;

    /**
     * @brief Muestra las cuentas del cliente según el tipo especificado
     * @param tipoCuenta Tipo de cuenta a mostrar ("ahorros" o "corriente")
     * @return Número de cuentas mostradas
     */
    int mostrarCuentas(const std::string& tipoCuenta) const;

    /**
     * @brief Guarda los datos de la persona y sus cuentas en archivos
     */
    void guardarEnArchivo() const;

    /**
     * @brief Busca cuentas según diferentes criterios
     * @param criterioBusqueda Criterio de búsqueda
     * @param numeroCuenta Número de cuenta a buscar
     * @param fechaApertura Fecha de apertura a buscar
     * @param saldo Saldo a buscar
     * @return Número de resultados encontrados
     */
    int buscarPersonaPorCriterio(const std::string& criterioBusqueda, const std::string& numeroCuenta, const std::string& fechaApertura, double saldo) const;

    /**
     * @brief Busca cuentas por fecha de apertura
     * @param fechaApertura Fecha de apertura a buscar
     */
    void buscarPersonaPorFecha(const std::string& fechaApertura) const;

    /**
     * @brief Busca una persona por el número de alguna de sus cuentas
     * @param numeroCuenta Número de cuenta a buscar
     * @return Número de resultados encontrados
     */
    int buscarPersonaPorCuentas(const string& numeroCuenta) const;

    /**
     * @brief Guarda las cuentas de un tipo específico en un archivo
     * @param archivo Flujo de salida donde guardar los datos
     * @param tipo Tipo de cuenta ("ahorros" o "corriente")
     * @return Número de cuentas guardadas
     */
    int guardarCuentas(std::ofstream& archivo, std::string tipo) const;

    /**
     * @brief Crea y agrega una cuenta de ahorros a la persona
     * @param nuevaCuenta Puntero a la cuenta de ahorros a agregar
     * @param cedulaEsperada Cédula para validación
     * @return true si se agregó correctamente, false en caso contrario
     */
    bool crearAgregarCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada);

    /**
     * @brief Crea y agrega una cuenta corriente a la persona
     * @param nuevaCuenta Puntero a la cuenta corriente a agregar
     * @param cedulaEsperada Cédula para validación
     * @return true si se agregó correctamente, false en caso contrario
     */
    bool crearAgregarCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada);

    /**
     * @brief Crea una cuenta de ahorros sin ingresar datos personales
     * @param nuevaCuenta Puntero a la cuenta de ahorros a crear
     * @param cedulaEsperada Cédula para validación
     * @return true si se creó correctamente, false en caso contrario
     */
    bool crearSoloCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada);

    /**
     * @brief Crea una cuenta corriente sin ingresar datos personales
     * @param nuevaCuenta Puntero a la cuenta corriente a crear
     * @param cedulaEsperada Cédula para validación
     * @return true si se creó correctamente, false en caso contrario
     */
    bool crearSoloCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada);

    /**
     * @brief Genera un número de cuenta único basado en la sucursal
     * @param nuevaCuenta Puntero a la cuenta para la que se generará el número
     * @param sucursal Código de sucursal
     * @return Número de cuenta generado
     */
    std::string crearNumeroCuenta(Cuenta<double>* nuevaCuenta, const std::string& sucursal);

    /**
     * @brief Permite al usuario seleccionar una sucursal bancaria
     * @return Código de la sucursal seleccionada
     */
    std::string seleccionSucursal();

    /**
     * @brief Genera un mensaje para solicitar datos personales
     * @return Mensaje formateado
     */
    std::string msgIngresoDatos() const;

    /**
     * @brief Convierte la lista enlazada de cuentas de ahorros a un vector para iteración limpia
     * @return std::vector<CuentaAhorros*> Vector de punteros a cuentas de ahorros
     */
    std::vector<CuentaAhorros*> obtenerCuentasAhorros() const;

    /**
     * @brief Convierte la lista enlazada de cuentas corrientes a un vector para iteración limpia
     * @return std::vector<CuentaCorriente*> Vector de punteros a cuentas corrientes
     */
    std::vector<CuentaCorriente*> obtenerCuentasCorriente() const;
};

#endif // PERSONA_H