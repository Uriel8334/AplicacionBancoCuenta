#pragma once
#include <functional>
#include <string>

namespace PersonaUI {
    extern const std::function<bool(const std::string&)> seleccionarSiNo;
    extern const std::function<double(double, double, const std::string&)> ingresarMonto;
}