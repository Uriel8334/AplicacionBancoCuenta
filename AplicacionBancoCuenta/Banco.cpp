#include "Banco.h"
#include "Utilidades.h"
#include "Validar.h"
#include <iostream>
#include <conio.h>
#include "ManejoMenus.h"

Banco::Banco() : manejoPersonas(), manejoCuentas(manejoPersonas) {}

bool Banco::validarOperacion(const std::string& operacion) {
    // Lógica de validación centralizada
    if (operacion == "buscar" || operacion == "transferir") {
        return manejoPersonas.tienePersonas();
    }
    return true;
}

void Banco::registrarOperacion(const std::string& operacion) {
    // Log de operaciones si es necesario
    // std::cout << "Operación realizada: " << operacion << std::endl;
}

void Banco::agregarPersonaConCuenta() {
    if (!validarOperacion("agregar")) return;

    // 1. Obtener tipo de cuenta usando el manejador de menús
    int tipoCuenta = ManejoMenus::mostrarMenuTipoCuenta();
    if (tipoCuenta == 2) {
        std::cout << "Operacion cancelada.\n";
        return;
    }

    // 2. Obtener cédula
    std::string cedula = ManejoMenus::solicitarCedula();
    if (cedula.empty()) {
        std::cout << "Operacion cancelada por el usuario.\n";
        return;
    }

    // 3. Verificar si la persona existe
    Persona* personaExistente = manejoPersonas.buscarPersonaPorCedula(cedula);

    if (personaExistente) {
        // Persona existe - solo agregar cuenta
        if (tipoCuenta == 0) {
            CuentaAhorros* nuevaCuenta = new CuentaAhorros();
            if (manejoCuentas.crearCuentaAhorros(cedula, nuevaCuenta)) {
                std::cout << "Cuenta de ahorros agregada correctamente.\n";
            }
        }
        else {
            CuentaCorriente* nuevaCuenta = new CuentaCorriente();
            if (manejoCuentas.crearCuentaCorriente(cedula, nuevaCuenta)) {
                std::cout << "Cuenta corriente agregada correctamente.\n";
            }
        }
    }
    else {
        // Persona nueva - crear persona y cuenta
        Persona* nuevaPersona = new Persona();
        nuevaPersona->setCedula(cedula);
        nuevaPersona->ingresarDatos(cedula);

        manejoPersonas.agregarPersona(nuevaPersona);

        // Crear la cuenta correspondiente
        if (tipoCuenta == 0) {
            CuentaAhorros* nuevaCuenta = new CuentaAhorros();
            manejoCuentas.crearCuentaAhorros(cedula, nuevaCuenta);
        }
        else {
            CuentaCorriente* nuevaCuenta = new CuentaCorriente();
            manejoCuentas.crearCuentaCorriente(cedula, nuevaCuenta);
        }
    }

    registrarOperacion("agregar_persona_cuenta");
    system("pause");
}

void Banco::buscarCuenta() {
    if (!validarOperacion("buscar")) {
        Utilidades::limpiarPantallaPreservandoMarquesina(1);
        std::cout << "No hay personas registradas todavia.\n";
        system("pause");
        return;
    }

    // Delegar la búsqueda al manejador de menús
    std::vector<std::string> opcionesBusqueda = {
        "Por fecha de creacion",
        "Por criterio de usuario",
        "Por numero de cuenta",
        "Por cedula",
        "Cancelar"
    };

    int seleccion = Utilidades::menuInteractivo("Seleccione el tipo de busqueda:", opcionesBusqueda, 0, 0);

    switch (seleccion) {
    case 0: { // Por fecha
        std::string fecha = ManejoMenus::seleccionarFecha();
        auto resultados = manejoCuentas.buscarCuentasPorFecha(fecha);
        ManejoMenus::mostrarResultadosBusqueda(resultados);
        break;
    }
    case 1: { // Por criterio
        // Implementar búsqueda por criterio usando el manejador
        break;
    }
    case 2: { // Por número de cuenta
        std::string numero = ManejoMenus::solicitarNumeroCuenta();
        auto resultados = manejoCuentas.buscarCuentasPorNumero(numero);
        ManejoMenus::mostrarResultadosBusqueda(resultados);
        break;
    }
    case 3: { // Por cédula
        std::string cedula = ManejoMenus::solicitarCedula();
        auto resultados = manejoCuentas.buscarCuentasPorCedula(cedula);
        ManejoMenus::mostrarResultadosBusqueda(resultados);
        break;
    }
    }

    registrarOperacion("buscar_cuenta");
}

bool Banco::verificarCuentasBanco() const {
    return manejoPersonas.tienePersonas();
}

void Banco::realizarTransferencia() {
    if (!validarOperacion("transferir")) {
        Utilidades::limpiarPantallaPreservandoMarquesina(1);
        std::cout << "No hay cuentas registradas en el sistema.\n";
        system("pause");
        return;
    }

    // 1. Obtener cuenta origen
    std::string cuentaOrigen = ManejoMenus::solicitarNumeroCuenta("origen");
    if (cuentaOrigen.empty()) return;

    // 2. Obtener cuenta destino
    std::string cuentaDestino = ManejoMenus::solicitarNumeroCuenta("destino");
    if (cuentaDestino.empty() || cuentaDestino == cuentaOrigen) {
        std::cout << "Cuenta de destino inválida.\n";
        return;
    }

    // 3. Verificar que ambas cuentas existen
    if (!manejoCuentas.existeCuenta(cuentaOrigen) || !manejoCuentas.existeCuenta(cuentaDestino)) {
        std::cout << "Una o ambas cuentas no existen.\n";
        return;
    }

    // 4. Mostrar saldos actuales
    double saldoOrigen = manejoCuentas.consultarSaldo(cuentaOrigen);
    std::cout << "Saldo actual cuenta origen: $" << std::fixed << std::setprecision(2) << saldoOrigen << std::endl;

    // 5. Obtener monto
    double monto = ManejoMenus::solicitarMonto();
    if (monto <= 0) return;

    // 6. Validar fondos
    if (!manejoCuentas.validarFondosSuficientes(cuentaOrigen, monto)) {
        std::cout << "Fondos insuficientes.\n";
        return;
    }

    // 7. Confirmar transferencia
    if (!ManejoMenus::confirmarOperacion("¿Confirma la transferencia de $" + std::to_string(monto) + "?")) {
        std::cout << "Transferencia cancelada.\n";
        return;
    }

    // 8. Realizar transferencia
    if (manejoCuentas.transferir(cuentaOrigen, cuentaDestino, monto)) {
        std::cout << "Transferencia realizada exitosamente.\n";
        std::cout << "Nuevo saldo cuenta origen: $" << std::fixed << std::setprecision(2)
            << manejoCuentas.consultarSaldo(cuentaOrigen) << std::endl;
    }
    else {
        std::cout << "Error al realizar la transferencia.\n";
    }

    registrarOperacion("transferencia");
    system("pause");
}


/**
 * @brief Submenú para gestionar operaciones específicas de cuentas bancarias
 *
 * Proporciona un menú interactivo para operaciones como depósitos, retiros,
 * consulta de saldos y otras operaciones específicas de cuentas.
 */
void Banco::subMenuCuentasBancarias() {
    if (!validarOperacion("cuentas")) {
        Utilidades::limpiarPantallaPreservandoMarquesina(1);
        std::cout << "No hay cuentas registradas en el sistema.\n";
        system("pause");
        return;
    }

    std::vector<std::string> opcionesCuentas = {
        "Realizar Deposito",
        "Realizar Retiro",
        "Consultar Saldo",
        "Mostrar Informacion de Cuenta",
        "Volver al Menu Principal"
    };

    int seleccion;
    do {
        Utilidades::limpiarPantallaPreservandoMarquesina(1);
        seleccion = Utilidades::menuInteractivo("=== SUBMENU CUENTAS BANCARIAS ===", opcionesCuentas, 0, 0);

        switch (seleccion) {
        case 0: { // Realizar Depósito
            std::string numeroCuenta = ManejoMenus::solicitarNumeroCuenta("para depósito");
            if (!numeroCuenta.empty() && manejoCuentas.existeCuenta(numeroCuenta)) {
                double monto = ManejoMenus::solicitarMonto();
                if (monto > 0) {
                    if (manejoCuentas.depositar(numeroCuenta, monto)) {
                        ManejoMenus::mostrarMensajeExito("Depósito realizado exitosamente.");
                        std::cout << "Nuevo saldo: $" << std::fixed << std::setprecision(2)
                            << manejoCuentas.consultarSaldo(numeroCuenta) << std::endl;
                    }
                    else {
                        ManejoMenus::mostrarMensajeError("Error al realizar el depósito.");
                    }
                }
            }
            else {
                ManejoMenus::mostrarMensajeError("Cuenta no encontrada.");
            }
            system("pause");
            break;
        }
        case 1: { // Realizar Retiro
            std::string numeroCuenta = ManejoMenus::solicitarNumeroCuenta("para retiro");
            if (!numeroCuenta.empty() && manejoCuentas.existeCuenta(numeroCuenta)) {
                double saldoActual = manejoCuentas.consultarSaldo(numeroCuenta);
                std::cout << "Saldo actual: $" << std::fixed << std::setprecision(2) << saldoActual << std::endl;

                double monto = ManejoMenus::solicitarMonto();
                if (monto > 0 && manejoCuentas.validarFondosSuficientes(numeroCuenta, monto)) {
                    if (manejoCuentas.retirar(numeroCuenta, monto)) {
                        ManejoMenus::mostrarMensajeExito("Retiro realizado exitosamente.");
                        std::cout << "Nuevo saldo: $" << std::fixed << std::setprecision(2)
                            << manejoCuentas.consultarSaldo(numeroCuenta) << std::endl;
                    }
                    else {
                        ManejoMenus::mostrarMensajeError("Error al realizar el retiro.");
                    }
                }
                else {
                    ManejoMenus::mostrarMensajeError("Fondos insuficientes o monto inválido.");
                }
            }
            else {
                ManejoMenus::mostrarMensajeError("Cuenta no encontrada.");
            }
            system("pause");
            break;
        }
        case 2: { // Consultar Saldo
            std::string numeroCuenta = ManejoMenus::solicitarNumeroCuenta("para consulta");
            if (!numeroCuenta.empty() && manejoCuentas.existeCuenta(numeroCuenta)) {
                double saldo = manejoCuentas.consultarSaldo(numeroCuenta);
                std::cout << "Saldo actual de la cuenta " << numeroCuenta << ": $"
                    << std::fixed << std::setprecision(2) << saldo << std::endl;
            }
            else {
                ManejoMenus::mostrarMensajeError("Cuenta no encontrada.");
            }
            system("pause");
            break;
        }
        case 3: { // Mostrar Información Completa
            std::string numeroCuenta = ManejoMenus::solicitarNumeroCuenta("para información");
            if (!numeroCuenta.empty() && manejoCuentas.existeCuenta(numeroCuenta)) {
                std::string info = manejoCuentas.obtenerInformacionCompleta(numeroCuenta);
                std::cout << info << std::endl;
            }
            else {
                ManejoMenus::mostrarMensajeError("Cuenta no encontrada.");
            }
            system("pause");
            break;
        }
        case 4: // Volver al menú principal
            break;
        }
    } while (seleccion != 4);

    registrarOperacion("submenu_cuentas");
}