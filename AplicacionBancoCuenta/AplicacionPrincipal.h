#pragma once
#include "ConfiguradorSistema.h"
#include "SistemaMenuPrincipal.h"
#include "Banco.h"
#include <memory>

/**
 * @class AplicacionPrincipal
 * @brief Clase principal que coordina toda la aplicación
 *
 * Aplicando SRP: Coordina los diferentes componentes del sistema
 * Aplicando DIP: Depende de abstracciones, no implementaciones concretas
 */
class AplicacionPrincipal {
private:
    std::unique_ptr<ConfiguradorSistema> configurador;
    std::unique_ptr<Banco> banco;
    std::unique_ptr<SistemaMenuPrincipal> sistemaMenu;

    bool inicializado = false;

    bool inicializarComponentes();
    void ejecutarBucleMenu();
    void limpiarRecursos();

public:
    AplicacionPrincipal();
    ~AplicacionPrincipal();

    int ejecutar();
};