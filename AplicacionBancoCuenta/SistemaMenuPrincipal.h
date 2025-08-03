#pragma once
#include "Banco.h"
#include "_BaseDatosArchivos.h"
#include "_BaseDatosPersona.h"
#include "DocumentacionDoxygen.h"
#include "AdministradorChatSocket.h"
#include "CodigoQR.h"
#include "Utilidades.h"
#include "ArbolBGrafico.h"
#include <vector>
#include <string>
#include <functional>
#include <map>

/**
 * @class SistemaMenuPrincipal
 * @brief Responsable únicamente del manejo del menú principal del sistema
 *
 * Aplicando SRP: Una sola responsabilidad - gestionar el menú principal
 */
class SistemaMenuPrincipal {
private:
    Banco& banco;
    _BaseDatosArchivos baseDatosArchivos;
	_BaseDatosPersona baseDatosPersona;
    DocumentacionDoxygen documentoDoxygen;

    std::vector<std::string> opcionesMenu;
    std::map<int, std::function<void()>> mapaAcciones;

    void inicializarOpciones();
    void inicializarAcciones();
    void ejecutarOpcionMenu(int seleccion, bool& necesitaRedibujado);

    // Métodos específicos para cada acción del menú
    void ejecutarCrearCuenta();
    void ejecutarBuscarCuenta();
    void ejecutarOperacionesCuenta();
    void ejecutarTransferencias();
    void ejecutarMenuAyuda();
    void ejecutarDocumentacion();
    void ejecutarBaseDatos();
    void ejecutarChat();
    void ejecutarSalir();
	void ejecutarGenerarQR();
    void ejecutarExploradorArchivos();
    void ejecutarGestionHash();
    void ejecutarArbolB();
    void ejecutarGuardarArchivo();
    void ejecutarRecuperarArchivo();
	void ejecutarDescifrarArchivo();

public:
    explicit SistemaMenuPrincipal(Banco& bancoRef);
    void mostrarMenu();
    void procesarSeleccion(int seleccion, bool& necesitaRedibujado);
    const std::vector<std::string>& getOpciones() const;
};