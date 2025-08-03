#pragma once
#include "ConexionMongo.h"
#include "Marquesina.h"
#include "Utilidades.h"
#include <string>
#include <vector>
#include <memory>

/**
 * @class ConfiguradorSistema
 * @brief Responsable de la configuración inicial del sistema
 *
 * Aplicando SRP: Una sola responsabilidad - configurar el sistema al inicio
 */
class ConfiguradorSistema {
private:
    std::unique_ptr<Marquesina> marquesina;

    bool configurarConexionMongoDB();
    bool seleccionarModoConexion();
    void mostrarInformacionModo(int seleccion);
    void configurarConsolaUTF8();
    void inicializarMarquesina();
    void verificarConexionBaseDatos();

public:
    ConfiguradorSistema() = default;
    ~ConfiguradorSistema() = default;

    bool inicializarSistema();
    Marquesina* getMarquesina() const;
    void finalizarSistema();
};