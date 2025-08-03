#include "AplicacionPrincipal.h"
#include "Utilidades.h"
#include <iostream>

// Variable global para la marquesina (compatibilidad con código existente)
extern Marquesina* marquesinaGlobal;

AplicacionPrincipal::AplicacionPrincipal() {
    configurador = std::make_unique<ConfiguradorSistema>();
}

AplicacionPrincipal::~AplicacionPrincipal() {
    limpiarRecursos();
}

int AplicacionPrincipal::ejecutar() {
    if (!inicializarComponentes()) {
        return -1;
    }

    ejecutarBucleMenu();
    return 0;
}

bool AplicacionPrincipal::inicializarComponentes() {
    if (!configurador->inicializarSistema()) {
        return false;
    }

    // Establecer marquesina global para compatibilidad
    marquesinaGlobal = configurador->getMarquesina();

    banco = std::make_unique<Banco>();
    sistemaMenu = std::make_unique<SistemaMenuPrincipal>(*banco);

    inicializado = true;
    return true;
}

void AplicacionPrincipal::ejecutarBucleMenu() {
    if (!inicializado) {
        return;
    }

    sistemaMenu->mostrarMenu();

    int x = 0, y = 0;
    const auto& opciones = sistemaMenu->getOpciones();

    while (true) {
        bool necesitaRedibujado = false;
        int seleccion = Utilidades::menuInteractivo("SISTEMA BANCARIO-EDUCATIVO", opciones, x, y);
        sistemaMenu->procesarSeleccion(seleccion, necesitaRedibujado);
    }
}

void AplicacionPrincipal::limpiarRecursos() {
    if (configurador) {
        configurador->finalizarSistema();
    }
    marquesinaGlobal = nullptr;
}