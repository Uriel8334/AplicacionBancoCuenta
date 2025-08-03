#include "ValidadorBaseDatos.h"
#include <iostream>
#include <iomanip>

ValidadorBaseDatos::ValidadorBaseDatos(_BaseDatosPersona& bd) : baseDatos(bd) {}

bool ValidadorBaseDatos::tienePersonasRegistradas() {
    return baseDatos.existenPersonasEnBaseDatos();
}

bool ValidadorBaseDatos::tieneCuentasRegistradas() {
    return baseDatos.existenCuentasEnBaseDatos();
}

ValidadorBaseDatos::EstadisticasBaseDatos ValidadorBaseDatos::obtenerEstadisticas() {
    EstadisticasBaseDatos stats{};

    stats.totalPersonas = baseDatos.obtenerTotalPersonasRegistradas();
    stats.totalCuentas = baseDatos.obtenerTotalCuentasRegistradas();
    stats.tienePersonas = stats.totalPersonas > 0;
    stats.tieneCuentas = stats.totalCuentas > 0;

    return stats;
}

void ValidadorBaseDatos::mostrarEstadoBaseDatos() {
    auto stats = obtenerEstadisticas();

    std::cout << "\n=== ESTADO DE LA BASE DE DATOS ===\n";
    std::cout << std::left << std::setw(20) << "Personas registradas:"
        << std::right << std::setw(10) << stats.totalPersonas << "\n";
    std::cout << std::left << std::setw(20) << "Cuentas registradas:"
        << std::right << std::setw(10) << stats.totalCuentas << "\n";
    std::cout << std::left << std::setw(20) << "Estado del sistema:"
        << std::right << std::setw(10)
        << (stats.tienePersonas ? "Operativo" : "Sin datos") << "\n";
    std::cout << std::string(35, '=') << "\n";
}