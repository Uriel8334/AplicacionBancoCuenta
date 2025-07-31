#pragma once

#ifndef BANCO_H
#define BANCO_H

#include "BancoManejoPersona.h"
#include "BancoManejoCuenta.h"
#include "ManejoMenus.h"
#include <string>
#include <functional>  // ← AGREGADO: Para resolver el error E1086

/**
 * @class Banco
 * @brief Clase coordinadora que orquesta las operaciones bancarias
 *
 * Aplicando SRP: Se encarga únicamente de coordinar entre los diferentes manejadores
 * Aplicando DIP: Depende de abstracciones, no de implementaciones concretas
 */
class Banco {
private:
    BancoManejoPersona manejoPersonas;
    BancoManejoCuenta manejoCuentas;

    // Métodos de coordinación simplificados
    bool validarOperacion(const std::string& operacion);
    void registrarOperacion(const std::string& operacion);

public:
    Banco();
    ~Banco() = default;

    // API pública simplificada - solo coordina
    void agregarPersonaConCuenta();
    void buscarCuenta();
    void realizarTransferencia();
    void subMenuCuentasBancarias();

    // Verificaciones del sistema
    bool verificarCuentasBanco() const;

    // Acceso a los manejadores (si es necesario)
    BancoManejoPersona& getManejoPersonas() { return manejoPersonas; }
    BancoManejoCuenta& getManejoCuentas() { return manejoCuentas; }

    // Compatibilidad con código existente
    NodoPersona* getListaPersonas() const { return manejoPersonas.getListaPersonas(); }
    void setListaPersonas(NodoPersona* lista) { manejoPersonas.setListaPersonas(lista); }
    void forEachPersona(const std::function<void(Persona*)>& funcion) const {  
        const_cast<BancoManejoPersona&>(manejoPersonas).forEachPersona(funcion);  
    }
};

#endif // BANCO_H