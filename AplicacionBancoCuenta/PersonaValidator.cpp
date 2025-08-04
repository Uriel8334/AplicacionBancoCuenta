#include "PersonaValidator.h"
#include "Validar.h"
#include <unordered_map>

// === Implementaciones de estrategias ===

bool ValidadorCedula::validar(const std::string& cedula) const {
    return Validar::ValidarCedula(cedula);
}

std::string ValidadorCedula::obtenerMensajeError() const {
    return "Cedula invalida, presione cualquier tecla para volver a ingresar.";
}

bool ValidadorNombrePersona::validar(const std::string& nombre) const {
    return Validar::ValidarNombrePersona(nombre);
}

std::string ValidadorNombrePersona::obtenerMensajeError() const {
    return "Nombre invalido, presione cualquier tecla para volver a ingresar.";
}

bool ValidadorFecha::validar(const std::string& fecha) const {
    return Validar::ValidarFecha(fecha);
}

std::string ValidadorFecha::obtenerMensajeError() const {
    return "Fecha invalida. Presione una tecla para continuar.";
}

// === Implementación del validador principal ===

PersonaValidator::PersonaValidator() {
    estrategias.push_back(std::make_unique<ValidadorCedula>());
    estrategias.push_back(std::make_unique<ValidadorNombrePersona>());
    estrategias.push_back(std::make_unique<ValidadorFecha>());
}

bool PersonaValidator::validarDato(const std::string& tipo, const std::string& valor) const {
    static const std::unordered_map<std::string, int> tipoIndice = {
        {"cedula", 0},
        {"nombre", 1},
        {"apellido", 1},
        {"fecha", 2}
    };

    auto it = tipoIndice.find(tipo);
    if (it != tipoIndice.end() && it->second < estrategias.size()) {
        return estrategias[it->second]->validar(valor);
    }
    return false;
}

std::string PersonaValidator::obtenerMensajeError(const std::string& tipo) const {
    static const std::unordered_map<std::string, int> tipoIndice = {
        {"cedula", 0},
        {"nombre", 1},
        {"apellido", 1},
        {"fecha", 2}
    };

    auto it = tipoIndice.find(tipo);
    if (it != tipoIndice.end() && it->second < estrategias.size()) {
        return estrategias[it->second]->obtenerMensajeError();
    }
    return "Error de validacion desconocido.";
}