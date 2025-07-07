#pragma once  
#ifndef PERSONA_H  
#define PERSONA_H  

#include <iostream>
#include <string>  
#include "CuentaAhorros.h"  
#include "CuentaCorriente.h"  
#include <functional>

/**
 * @namespace PersonaUI
 * @brief Namespace que contiene funciones de utilidad para la interfaz de usuario de Persona
 *
 * Proporciona funciones lambda reutilizables para interactuar con el usuario
 * en el contexto de la gesti�n de personas y cuentas.
 */
namespace PersonaUI {
    /**
     * @brief Funci�n para solicitar una confirmaci�n (S�/No) al usuario
     * @param mensaje El mensaje a mostrar para la solicitud
     * @return true si el usuario selecciona "S�", false en caso contrario
     */
    extern const std::function<bool(const std::string&)> seleccionarSiNo;

    /**
     * @brief Funci�n para solicitar el ingreso de un monto con validaci�n
     * @param min Valor m�nimo aceptable
     * @param max Valor m�ximo aceptable
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
 * Esta clase gestiona la informaci�n personal de un cliente y administra
 * sus cuentas bancarias (tanto de ahorros como corrientes). Implementa
 * operaciones para crear, buscar y gestionar cuentas asociadas a la persona.
 */
class Persona {
private:
    /** @brief N�mero de c�dula (identificaci�n) de la persona */
    string cedula;

    /** @brief Nombres de la persona */
    string nombres;

    /** @brief Apellidos de la persona */
    string apellidos;

    /** @brief Fecha de nacimiento en formato de cadena */
    string fechaNacimiento;

    /** @brief Direcci�n de correo electr�nico */
    string correo;

    /** @brief Direcci�n domiciliaria */
    string direccion;

    /** @brief Puntero a la primera cuenta de ahorros en una lista enlazada */
    CuentaAhorros* cabezaAhorros;

    /** @brief Puntero a la primera cuenta corriente en una lista enlazada */
    CuentaCorriente* cabezaCorriente;

    /** @brief N�mero de cuentas de ahorro (limitadas a 5 por persona) */
    int numCuentas = 0;

    /** @brief N�mero de cuentas corrientes (sin l�mite) */
    int numCorrientes = 0;

    /** @brief Indicador de si el objeto ha sido destruido, para evitar uso despu�s de su destrucci�n */
    bool isDestroyed = false;

public:
    /**
     * @brief Constructor por defecto
     *
     * Inicializa una nueva persona con valores vac�os y sin cuentas asociadas
     */
    Persona() : cabezaAhorros(nullptr), cabezaCorriente(nullptr), numCuentas(0), numCorrientes(0), isDestroyed(false) {}

    /**
     * @brief Constructor con par�metros
     *
     * @param cedula N�mero de identificaci�n de la persona
     * @param nombres Nombres de la persona
     * @param apellidos Apellidos de la persona
     * @param fechaNacimiento Fecha de nacimiento en formato de cadena
     * @param correo Direcci�n de correo electr�nico
     * @param direccion Direcci�n domiciliaria
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
     * @brief Verifica si la instancia es v�lida (no ha sido destruida)
     * @return true si la instancia es v�lida, false en caso contrario
     */
    bool isValidInstance() const { return !isDestroyed; }

    /**
     * @brief Establece el n�mero de c�dula
     * @param cedula Nueva c�dula
     * @return true si se asign� correctamente
     */
    bool setCedula(const string& cedula) { this->cedula = cedula; return true; }

    /**
     * @brief Establece los nombres
     * @param nombres Nuevos nombres
     * @return true si se asign� correctamente
     */
    bool setNombres(const string& nombres) { this->nombres = nombres; return true; }

    /**
     * @brief Establece los apellidos
     * @param apellidos Nuevos apellidos
     * @return true si se asign� correctamente
     */
    bool setApellidos(const string& apellidos) { this->apellidos = apellidos; return true; }

    /**
     * @brief Establece la fecha de nacimiento
     * @param fechaNacimiento Nueva fecha de nacimiento
     * @return true si se asign� correctamente
     */
    bool setFechaNacimiento(const string& fechaNacimiento) { this->fechaNacimiento = fechaNacimiento; return true; }

    /**
     * @brief Establece el correo electr�nico
     * @param correo Nuevo correo electr�nico
     * @return true si se asign� correctamente
     */
    bool setCorreo(const string& correo) { this->correo = correo; return true; }

    /**
     * @brief Establece la direcci�n
     * @param direccion Nueva direcci�n
     * @return true si se asign� correctamente
     */
    bool setDireccion(const string& direccion) { this->direccion = direccion; return true; }

    /**
     * @brief Establece el n�mero de cuentas de ahorro
     * @param numCuentas Nuevo n�mero de cuentas
     */
    void setNumeCuentas(int numCuentas) { this->numCuentas = numCuentas; }

    /**
     * @brief Establece el n�mero de cuentas corrientes
     * @param numCorrientes Nuevo n�mero de cuentas corrientes
     */
    void setNumCorrientes(int numCorrientes) { this->numCorrientes = numCorrientes; }

    /**
     * @brief Establece la cabeza de la lista de cuentas de ahorro
     * @param nuevaCabeza Puntero a la nueva cuenta que ser� la cabeza
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
     * @param nuevaCabeza Puntero a la nueva cuenta que ser� la cabeza
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
     * @brief Obtiene la c�dula de la persona
     * @return C�dula como cadena
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
     * @brief Obtiene el correo electr�nico
     * @return Correo electr�nico como cadena
     */
    string getCorreo() const { return this->correo; }

    /**
     * @brief Obtiene la direcci�n
     * @return Direcci�n como cadena
     */
    string getDireccion() const { return this->direccion; }

    /**
     * @brief Obtiene el n�mero de cuentas de ahorro
     * @return N�mero de cuentas de ahorro
     */
    int getNumCuentas() const { return this->numCuentas; }

    /**
     * @brief Obtiene el n�mero de cuentas corrientes
     * @return N�mero de cuentas corrientes
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
     * @brief Solicita al usuario ingresar datos con una c�dula predeterminada
     * @param cedulaEsperada C�dula que se espera validar
     */
    void ingresarDatos(const std::string& cedulaEsperada);

    /**
     * @brief Solicita al usuario ingresar su n�mero de c�dula
     * @param cedula Variable donde se almacenar� la c�dula
     * @return C�dula ingresada y validada
     */
    std::string ingresarCedula(std::string& cedula);

    /**
     * @brief Solicita al usuario ingresar sus nombres
     * @param nombres Variable donde se almacenar�n los nombres
     * @return Nombres ingresados y validados
     */
    std::string ingresarNombres(std::string& nombres) const;

    /**
     * @brief Solicita al usuario ingresar sus apellidos
     * @param apellidos Variable donde se almacenar�n los apellidos
     * @return Apellidos ingresados y validados
     */
    std::string ingresarApellidos(std::string& apellidos) const;

    /**
     * @brief Solicita al usuario ingresar su fecha de nacimiento
     * @param fechaNacimiento Variable donde se almacenar� la fecha
     * @return Fecha de nacimiento ingresada y validada
     */
    std::string ingresarFechaNacimiento(std::string& fechaNacimiento);

    /**
     * @brief Solicita al usuario ingresar su correo electr�nico
     * @param correo Variable donde se almacenar� el correo
     * @return Correo electr�nico ingresado y validado
     */
    std::string ingresarCorreo(std::string& correo) const;

    /**
     * @brief Solicita al usuario ingresar su direcci�n
     * @param direccion Variable donde se almacenar� la direcci�n
     * @return Direcci�n ingresada y validada
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
     * @brief Muestra las cuentas del cliente seg�n el tipo especificado
     * @param tipoCuenta Tipo de cuenta a mostrar ("ahorros" o "corriente")
     * @return N�mero de cuentas mostradas
     */
    int mostrarCuentas(const std::string& tipoCuenta) const;

    /**
     * @brief Guarda los datos de la persona y sus cuentas en archivos
     */
    void guardarEnArchivo() const;

    /**
     * @brief Busca cuentas seg�n diferentes criterios
     * @param criterioBusqueda Criterio de b�squeda
     * @param numeroCuenta N�mero de cuenta a buscar
     * @param fechaApertura Fecha de apertura a buscar
     * @param saldo Saldo a buscar
     * @return N�mero de resultados encontrados
     */
    int buscarPersonaPorCriterio(const std::string& criterioBusqueda, const std::string& numeroCuenta, const std::string& fechaApertura, double saldo) const;

    /**
     * @brief Busca cuentas por fecha de apertura
     * @param fechaApertura Fecha de apertura a buscar
     */
    void buscarPersonaPorFecha(const std::string& fechaApertura) const;

    /**
     * @brief Busca una persona por el n�mero de alguna de sus cuentas
     * @param numeroCuenta N�mero de cuenta a buscar
     * @return N�mero de resultados encontrados
     */
    int buscarPersonaPorCuentas(const string& numeroCuenta) const;

    /**
     * @brief Guarda las cuentas de un tipo espec�fico en un archivo
     * @param archivo Flujo de salida donde guardar los datos
     * @param tipo Tipo de cuenta ("ahorros" o "corriente")
     * @return N�mero de cuentas guardadas
     */
    int guardarCuentas(std::ofstream& archivo, std::string tipo) const;

    /**
     * @brief Crea y agrega una cuenta de ahorros a la persona
     * @param nuevaCuenta Puntero a la cuenta de ahorros a agregar
     * @param cedulaEsperada C�dula para validaci�n
     * @return true si se agreg� correctamente, false en caso contrario
     */
    bool crearAgregarCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada);

    /**
     * @brief Crea y agrega una cuenta corriente a la persona
     * @param nuevaCuenta Puntero a la cuenta corriente a agregar
     * @param cedulaEsperada C�dula para validaci�n
     * @return true si se agreg� correctamente, false en caso contrario
     */
    bool crearAgregarCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada);

    /**
     * @brief Crea una cuenta de ahorros sin ingresar datos personales
     * @param nuevaCuenta Puntero a la cuenta de ahorros a crear
     * @param cedulaEsperada C�dula para validaci�n
     * @return true si se cre� correctamente, false en caso contrario
     */
    bool crearSoloCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada);

    /**
     * @brief Crea una cuenta corriente sin ingresar datos personales
     * @param nuevaCuenta Puntero a la cuenta corriente a crear
     * @param cedulaEsperada C�dula para validaci�n
     * @return true si se cre� correctamente, false en caso contrario
     */
    bool crearSoloCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada);

    /**
     * @brief Genera un n�mero de cuenta �nico basado en la sucursal
     * @param nuevaCuenta Puntero a la cuenta para la que se generar� el n�mero
     * @param sucursal C�digo de sucursal
     * @return N�mero de cuenta generado
     */
    std::string crearNumeroCuenta(Cuenta<double>* nuevaCuenta, const std::string& sucursal);

    /**
     * @brief Permite al usuario seleccionar una sucursal bancaria
     * @return C�digo de la sucursal seleccionada
     */
    std::string seleccionSucursal();

    /**
     * @brief Genera un mensaje para solicitar datos personales
     * @return Mensaje formateado
     */
    std::string msgIngresoDatos() const;

    /*
    // Metodo unificado para crear y agregar cuenta de ahorros
    bool crearYAgregarCuentaAhorros(Persona* persona) {
        // Verificar limite
        if (numCuentas >= 5) {
            std::cout << "No puede tener mas de 5 cuentas de ahorros.\n";
            return false;
        }

        // Crear la cuenta directamente como objeto dinamico
        CuentaAhorros* nuevaCuenta;

        // Llamar al metodo crearCuenta() del objeto nuevaCuenta
        // que configura numero de cuenta, fecha, saldo inicial, etc.
        nuevaCuenta->crearCuenta(persona);

        // Crear el nodo y agregarlo a la lista directamente
        NodoCuentaAhorros* nuevo = new NodoCuentaAhorros(nuevaCuenta);

        // Verificacion de seguridad
        if (nuevo->cuenta == nullptr) {
            std::cout << "[ERROR] Cuenta no inicializada" << std::endl;
            delete nuevo;
            return false;
        }

        // Agregar a la lista con las validaciones normales
        nuevo->siguiente = cabezaAhorros;
        nuevo->anterior = nullptr;
        if (cabezaAhorros != nullptr) {
            cabezaAhorros->anterior = nuevo;
        }
        cabezaAhorros = nuevo;
        numCuentas++;

        std::cout << "Cuenta de Ahorros creada y agregada con exito." << std::endl;
        return true;
    }

    // Metodo unificado para crear y agregar cuenta corriente
    bool crearYAgregarCuentaCorriente(Persona* persona) {
        // No hay limite para cuentas corrientes (a diferencia de las de ahorro)

        // Crear la cuenta directamente como objeto dinamico
        CuentaCorriente* nuevaCuenta;

        // Llamar al metodo crearCuenta() del objeto nuevaCuenta
        // que configura numero de cuenta, fecha, saldo inicial, etc.
        nuevaCuenta->crearCuenta(persona);

        // Crear el nodo y agregarlo a la lista directamente
        //NodoCuentaCorriente* nuevo = new NodoCuentaCorriente(nuevaCuenta);

        // Verificacion de seguridad
        if (nuevo->cuenta == nullptr) {
            std::cout << "[ERROR] Cuenta corriente no inicializada" << std::endl;
            delete nuevo;
            return false;
        }

        // Agregar a la lista con las validaciones normales
        nuevo->siguiente = cabezaCorriente;
        nuevo->anterior = nullptr;
        if (cabezaCorriente != nullptr) {
            cabezaCorriente->anterior = nuevo;
        }
        cabezaCorriente = nuevo;

        std::cout << "Cuenta Corriente creada y agregada con exito." << std::endl;
        return true;
    }

    void seleccionarFecha();
    }; */
};
#endif // PERSONA_H