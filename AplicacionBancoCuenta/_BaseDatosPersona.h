#pragma once  
#ifndef _BASEDATOSPERSONA_H  
#define _BASEDATOSPERSONA_H  

#include <mongocxx/client.hpp>
#include <bsoncxx/document/value.hpp>
#include <string>

class Persona;

/**
 * @class _BaseDatosPersona
 * @brief Clase para gestionar operaciones de base de datos relacionadas con personas y cuentas
 *
 * Esta clase maneja todas las operaciones CRUD para personas y sus cuentas bancarias
 * en MongoDB, incluyendo operaciones financieras como transferencias, depósitos y retiros.
 */
class _BaseDatosPersona {
private:
    mongocxx::client& _client;

public:

    /**
     * @brief Constructor que inicializa la conexión a MongoDB
     * @param client Referencia al cliente de MongoDB
     */
    _BaseDatosPersona(mongocxx::client& client);



#pragma region === OPERACIONES DE PERSONA ===
    /**
     * @brief Inserta una nueva persona en la base de datos
     * @param persona La persona a insertar
     * @return true si la inserción fue exitosa, false en caso contrario
     */
    bool insertarNuevaPersona(const Persona& persona);

    /**
     * @brief Inserta una nueva persona con una cuenta inicial
     * @param persona La persona a insertar
     * @param cuentaInicial Documento BSON de la cuenta inicial (opcional)
     * @return true si la inserción fue exitosa, false en caso contrario
     */
    bool insertarPersona(const Persona& persona, const bsoncxx::document::value* cuentaInicial = nullptr);

    /**
     * @brief Verifica si existe una persona con la cédula dada
     * @param cedula Cédula a buscar
     * @return true si existe, false si no existe
     */
    bool existePersonaPorCedula(const std::string& cedula);

    /**
     * @brief Obtiene una persona por su cédula
     * @param cedula Cédula de la persona a buscar
     * @return Puntero a la Persona encontrada, o nullptr si no se encuentra
     */
    Persona* obtenerPersonaPorCedula(const std::string& cedula);

    /**
     * @brief Agrega una cuenta a una persona existente
     * @param cedula Cédula de la persona
     * @param cuentaDoc Documento BSON de la cuenta a agregar
     * @return true si se agregó exitosamente, false en caso contrario
     */
    bool agregarCuentaPersona(const std::string& cedula, const bsoncxx::document::value& cuentaDoc);

    /**
     * @brief Busca personas por criterio específico en la base de datos
     * @param criterio Campo por el cual buscar (nombre, apellido, fechaNacimiento, etc.)
     * @param valor Valor a buscar
     * @return Vector de documentos BSON con las personas encontradas
     */
    std::vector<bsoncxx::document::value> buscarPersonasPorCriterio(const std::string& criterio, const std::string& valor);

    /**
     * @brief Busca cuentas por rango de fechas desde una fecha hasta hoy
     * @param fechaInicio Fecha de inicio en formato DD/MM/AAAA
     * @return Vector de documentos BSON con las cuentas encontradas
     */
    std::vector<bsoncxx::document::value> buscarCuentasPorRangoFechas(const std::string& fechaInicio);

    /**
     * @brief Busca todas las cuentas de una persona por su cédula
     * @param cedula Cédula de la persona a buscar
     * @return Documento BSON con la información completa de la persona y sus cuentas
     */
    bsoncxx::document::value buscarPersonaCompletaPorCedula(const std::string& cedula);

    /**
     * @brief Convierte una fecha de formato DD/MM/AAAA a ISO para MongoDB
     * @param fecha Fecha en formato DD/MM/AAAA
     * @return Fecha en formato ISO string
     */
    std::string convertirFechaAISO(const std::string& fecha);

    void iniciarBaseDatosArbolB();

#pragma endregion

#pragma region === OPERACIONES BANCARIAS ===
    /**
     * @brief Realiza un depósito en una cuenta específica
     * @param numeroCuenta Número de cuenta destino
     * @param monto Monto a depositar (formato double: 123.45)
     * @return true si el depósito fue exitoso, false en caso contrario
     */
    bool depositarEnCuenta(const std::string& numeroCuenta, double monto);

    /**
     * @brief Realiza un retiro de una cuenta específica
     * @param numeroCuenta Número de cuenta origen
     * @param monto Monto a retirar (formato double: 123.45)
     * @return true si el retiro fue exitoso, false en caso contrario
     */
    bool retirarDeCuenta(const std::string& numeroCuenta, double monto);

    /**
     * @brief Obtiene el saldo actual de una cuenta
     * @param numeroCuenta Número de cuenta a consultar
     * @return Saldo actual como double, -1.0 si la cuenta no existe
     */
    double obtenerSaldoCuenta(const std::string& numeroCuenta);

    /**
     * @brief Verifica si una cuenta tiene fondos suficientes
     * @param numeroCuenta Número de cuenta a verificar
     * @param monto Monto a verificar
     * @return true si tiene fondos suficientes, false en caso contrario
     */
    bool verificarFondosSuficientes(const std::string& numeroCuenta, double monto);

    /**
     * @brief Realiza una transferencia entre dos cuentas
     * @param cuentaOrigen Número de cuenta origen
     * @param cuentaDestino Número de cuenta destino
     * @param monto Monto a transferir (formato double: 123.45)
     * @return true si la transferencia fue exitosa, false en caso contrario
     */
    bool realizarTransferencia(const std::string& cuentaOrigen, const std::string& cuentaDestino, double monto);

    /**
     * @brief Obtiene información completa de una cuenta
     * @param numeroCuenta Número de cuenta a consultar
     * @return Documento BSON con la información de la cuenta, o documento vacío si no existe
     */
    bsoncxx::document::value obtenerInformacionCuenta(const std::string& numeroCuenta);

    /**
     * @brief Obtiene la cédula del titular de una cuenta
     * @param numeroCuenta Número de cuenta
     * @return Cédula del titular, o cadena vacía si no se encuentra
     */
    std::string obtenerCedulaPorNumeroCuenta(const std::string& numeroCuenta);

    /**
     * @brief Verifica si existen personas registradas en la base de datos MongoDB
     * @return true si existen personas registradas, false en caso contrario
     */
    bool existenPersonasEnBaseDatos();

    /**
     * @brief Obtiene el número total de personas registradas en la base de datos
     * @return Número total de personas registradas
     */
    long obtenerTotalPersonasRegistradas();

    /**
     * @brief Verifica si existen cuentas en la base de datos MongoDB
     * @return true si existen cuentas registradas, false en caso contrario
     */
    bool existenCuentasEnBaseDatos();

    /**
     * @brief Obtiene el número total de cuentas registradas en la base de datos
     * @return Número total de cuentas registradas
     */
    long obtenerTotalCuentasRegistradas();

    /**
     * @brief Obtiene todas las personas registradas en la base de datos MongoDB
     * @return Vector de documentos BSON con todas las personas encontradas
     */
    std::vector<bsoncxx::document::value> mostrarTodasPersonas();
#pragma endregion

#pragma region === UTILIDADES ===
    /**
     * @brief Prueba la conexión a MongoDB
     * @param uri_str Cadena de conexión URI de MongoDB
     * @return true si la conexión fue exitosa, false en caso contrario
     */
    static bool probarConexionMongoDB(const std::string& uri_str);
#pragma endregion

    /**
     * @brief Obtiene el último número secuencial usado para una sucursal específica
     * @param sucursal Código de sucursal (210, 220, 480, 560)
     * @return Último número secuencial usado, 0 si no existe
     */
    int obtenerUltimoSecuencial(const std::string& sucursal);

    /**
     * @brief Actualiza el último número secuencial para una sucursal específica
     * @param sucursal Código de sucursal
     * @param nuevoSecuencial Nuevo número secuencial a guardar
     * @return true si se actualizó correctamente, false en caso contrario
     */
    bool actualizarSecuencial(const std::string& sucursal, int nuevoSecuencial);

    /**
     * @brief Obtiene el mayor número de cuenta existente para una sucursal específica
     * @param sucursal Código de sucursal
     * @return Mayor número secuencial encontrado en las cuentas existentes
     */
    int obtenerMayorNumeroCuentaPorSucursal(const std::string& sucursal);

    /**
     * @brief Busca una cuenta específica dentro de un documento de persona
     * @param personaDoc Documento de la persona
     * @param numeroCuenta Número de cuenta a buscar
     * @return Índice de la cuenta en el array, -1 si no se encuentra
     */
    int buscarIndiceCuentaEnDocumento(const bsoncxx::document::view& personaDoc, const std::string& numeroCuenta);

    /**
     * @brief Valida que el monto sea un número válido y positivo
     * @param monto Monto a validar
     * @return true si el monto es válido, false en caso contrario
     */
    bool validarMonto(double monto);

    /**
     * @brief Redondea un monto a 2 decimales para operaciones monetarias
     * @param monto Monto a redondear
     * @return Monto redondeado a 2 decimales
     */
    double redondearMonto(double monto);
    
};
#endif // _BASEDATOSPERSONA_H