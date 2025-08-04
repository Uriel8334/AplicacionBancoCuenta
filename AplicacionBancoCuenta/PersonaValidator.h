#pragma once
#ifndef PERSONAVALIDATOR_H
#define PERSONAVALIDATOR_H

#include <string>
#include <functional>
#include <vector>
#include <memory>

/**
 * @brief Interface para estrategias de validación
 * Aplicando Strategy Pattern y Open/Closed Principle
 */
class IValidacionStrategy {
public:
    virtual ~IValidacionStrategy() = default;
    virtual bool validar(const std::string& dato) const = 0;
    virtual std::string obtenerMensajeError() const = 0;
};

/**
 * @brief Validador de cédulas ecuatorianas
 */
class ValidadorCedula : public IValidacionStrategy {
public:
    bool validar(const std::string& cedula) const override;
    std::string obtenerMensajeError() const override;
};

/**
 * @brief Validador de nombres y apellidos
 */
class ValidadorNombrePersona : public IValidacionStrategy {
public:
    bool validar(const std::string& nombre) const override;
    std::string obtenerMensajeError() const override;
};

/**
 * @brief Validador de fechas
 */
class ValidadorFecha : public IValidacionStrategy {
public:
    bool validar(const std::string& fecha) const override;
    std::string obtenerMensajeError() const override;
};

/**
 * @brief Clase responsable de validar datos de persona
 * Aplicando Single Responsibility Principle
 */
class PersonaValidator {
private:
    std::vector<std::unique_ptr<IValidacionStrategy>> estrategias;

public:
    PersonaValidator();
    bool validarDato(const std::string& tipo, const std::string& valor) const;
    std::string obtenerMensajeError(const std::string& tipo) const;
};

#endif // PERSONAVALIDATOR_H