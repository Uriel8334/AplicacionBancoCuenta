#include "BuscadorCuentas.h"
#include "Validar.h"
#include <iostream>
#include <iomanip>

BuscadorCuentas::BuscadorCuentas(_BaseDatosPersona& bd) : baseDatos(bd) {
    inicializarEstrategias();
}

void BuscadorCuentas::inicializarEstrategias() {
    mapaEstrategiasBusqueda = {
        { 0, [this]() { buscarPorFechaCreacion(); }},
        { 1, [this]() { buscarPorCriterioUsuario(); }},
        { 2, [this]() { buscarPorNumeroCuenta(); }},
        { 3, [this]() { buscarPorCedula(); }}
    };
}

void BuscadorCuentas::ejecutarBusqueda(int tipoBusqueda) {
    auto it = mapaEstrategiasBusqueda.find(tipoBusqueda);
    if (it != mapaEstrategiasBusqueda.end()) {
        it->second();
    }
}

void BuscadorCuentas::buscarPorFechaCreacion() {
    std::string fecha = ManejoMenus::seleccionarFecha();
    if (fecha.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    std::cout << "Buscando cuentas creadas desde " << fecha << " hasta la fecha actual...\n";
    auto resultados = baseDatos.buscarCuentasPorRangoFechas(fecha);

    if (resultados.empty()) {
        std::cout << "No se encontraron cuentas en el rango de fechas especificado.\n";
    }
    else {
        mostrarResultadosFechas(resultados);
    }

    system("pause");
}

void BuscadorCuentas::buscarPorCriterioUsuario() {
    std::vector<std::string> criterios = {
        "Nombre",
        "Apellido",
        "Fecha de Nacimiento",
        "Correo",
        "Dirección",
        "Número de Cuentas de Ahorros",
        "Número de Cuentas Corrientes",
        "Total de Cuentas",
        "Cancelar"
    };

    int seleccion = Utilidades::menuInteractivo("Seleccione el criterio de búsqueda:", criterios, 0, 0);

    if (seleccion == 8 || seleccion == -1) { // Cancelar o ESC
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    std::map<int, std::function<void()>> criteriosFunciones = {
        { 0, [this]() { buscarPorNombre(); }},
        { 1, [this]() { buscarPorApellido(); }},
        { 2, [this]() { buscarPorFechaNacimiento(); }},
        { 3, [this]() { buscarPorCorreo(); }},
        { 4, [this]() { buscarPorDireccion(); }},
        { 5, [this]() { buscarPorNumeroAhorros(); }},
        { 6, [this]() { buscarPorNumeroCorrientes(); }},
        { 7, [this]() { buscarPorTotalCuentas(); }}
    };

    auto it = criteriosFunciones.find(seleccion);
    if (it != criteriosFunciones.end()) {
        it->second();
    }
}

void BuscadorCuentas::buscarPorNombre() {
    std::cout << "Ingrese el nombre a buscar: ";
    std::string nombre;
    std::getline(std::cin, nombre);

    if (nombre.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    auto resultados = baseDatos.buscarPersonasPorCriterio("nombre", nombre);

    if (resultados.empty()) {
        std::cout << "No se encontraron personas con el nombre: " << nombre << "\n";
    }
    else {
        std::cout << "Personas encontradas con el nombre '" << nombre << "':\n\n";
        mostrarResultadosCriterio(resultados);
    }

    system("pause");
}

void BuscadorCuentas::buscarPorApellido() {
    std::cout << "Ingrese el apellido a buscar: ";
    std::string apellido;
    std::getline(std::cin, apellido);

    if (apellido.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    auto resultados = baseDatos.buscarPersonasPorCriterio("apellido", apellido);

    if (resultados.empty()) {
        std::cout << "No se encontraron personas con el apellido: " << apellido << "\n";
    }
    else {
        std::cout << "Personas encontradas con el apellido '" << apellido << "':\n\n";
        mostrarResultadosCriterio(resultados);
    }

    system("pause");
}

void BuscadorCuentas::buscarPorFechaNacimiento() {
    std::string fecha = ManejoMenus::seleccionarFecha();
    if (fecha.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    auto resultados = baseDatos.buscarPersonasPorCriterio("fechaNacimiento", fecha);

    if (resultados.empty()) {
        std::cout << "No se encontraron personas nacidas el: " << fecha << "\n";
    }
    else {
        std::cout << "Personas encontradas nacidas el " << fecha << ":\n\n";
        mostrarResultadosCriterio(resultados);
    }

    system("pause");
}

void BuscadorCuentas::buscarPorCorreo() {
    std::cout << "Ingrese el correo a buscar: ";
    std::string correo;
    std::getline(std::cin, correo);

    if (correo.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    auto resultados = baseDatos.buscarPersonasPorCriterio("correo", correo);

    if (resultados.empty()) {
        std::cout << "No se encontraron personas con el correo: " << correo << "\n";
    }
    else {
        std::cout << "Personas encontradas con el correo '" << correo << "':\n\n";
        mostrarResultadosCriterio(resultados);
    }

    system("pause");
}

void BuscadorCuentas::buscarPorDireccion() {
    std::cout << "Ingrese la dirección a buscar: ";
    std::string direccion;
    std::getline(std::cin, direccion);

    if (direccion.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    auto resultados = baseDatos.buscarPersonasPorCriterio("direccion", direccion);

    if (resultados.empty()) {
        std::cout << "No se encontraron personas con la dirección: " << direccion << "\n";
    }
    else {
        std::cout << "Personas encontradas con la dirección '" << direccion << "':\n\n";
        mostrarResultadosCriterio(resultados);
    }

    system("pause");
}

void BuscadorCuentas::buscarPorNumeroAhorros() {
    std::cout << "Ingrese el número de cuentas de ahorros: ";
    std::string numero;
    std::getline(std::cin, numero);

    if (numero.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    auto resultados = baseDatos.buscarPersonasPorCriterio("numAhorros", numero);

    if (resultados.empty()) {
        std::cout << "No se encontraron personas con " << numero << " cuentas de ahorros.\n";
    }
    else {
        std::cout << "Personas encontradas con " << numero << " cuentas de ahorros:\n\n";
        mostrarResultadosCriterio(resultados);
    }

    system("pause");
}

void BuscadorCuentas::buscarPorNumeroCorrientes() {
    std::cout << "Ingrese el número de cuentas corrientes: ";
    std::string numero;
    std::getline(std::cin, numero);

    if (numero.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    auto resultados = baseDatos.buscarPersonasPorCriterio("numCorrientes", numero);

    if (resultados.empty()) {
        std::cout << "No se encontraron personas con " << numero << " cuentas corrientes.\n";
    }
    else {
        std::cout << "Personas encontradas con " << numero << " cuentas corrientes:\n\n";
        mostrarResultadosCriterio(resultados);
    }

    system("pause");
}

void BuscadorCuentas::buscarPorTotalCuentas() {
    std::cout << "Ingrese el total de cuentas: ";
    std::string numero;
    std::getline(std::cin, numero);

    if (numero.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    auto resultados = baseDatos.buscarPersonasPorCriterio("totalCuentasExistentes", numero);

    if (resultados.empty()) {
        std::cout << "No se encontraron personas con " << numero << " cuentas totales.\n";
    }
    else {
        std::cout << "Personas encontradas con " << numero << " cuentas totales:\n\n";
        mostrarResultadosCriterio(resultados);
    }

    system("pause");
}

void BuscadorCuentas::buscarPorCedula() {
    std::string cedula = ManejoMenus::solicitarCedula();
    if (cedula.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    auto resultado = baseDatos.buscarPersonaCompletaPorCedula(cedula);

    // Verificar si el documento está vacío
    auto view = resultado.view();
    if (view.empty() || view.find("cedula") == view.end()) {
        std::cout << "No se encontró ninguna persona con la cédula: " << cedula << "\n";
    }
    else {
        std::cout << "Información completa de la persona con cédula " << cedula << ":\n\n";
        mostrarResultadoCedula(resultado);
    }

    system("pause");
}

void BuscadorCuentas::buscarPorNumeroCuenta() {
    std::string numero = ManejoMenus::solicitarNumeroCuenta();
    if (numero.empty()) {
        std::cout << "Búsqueda cancelada.\n";
        return;
    }

    // Buscar la cuenta específica
    auto infoCuenta = baseDatos.obtenerInformacionCuenta(numero);
    auto view = infoCuenta.view();

    if (view.empty() || view.find("numeroCuenta") == view.end()) {
        std::cout << "No se encontró ninguna cuenta con el número: " << numero << "\n";
    }
    else {
        std::cout << "Información de la cuenta " << numero << ":\n\n";

        // Mostrar información del titular
        if (view.find("titular") != view.end()) {
            auto titular = view["titular"].get_document().view();
            std::cout << "=== INFORMACIÓN DEL TITULAR ===\n";
            std::cout << "Cédula: " << view["cedula"].get_string().value << "\n";
            std::cout << "Nombre: " << titular["nombre"].get_string().value << " "
                << titular["apellido"].get_string().value << "\n";
            std::cout << "Correo: " << titular["correo"].get_string().value << "\n\n";
        }

        // Mostrar información de la cuenta
        if (view.find("cuenta") != view.end()) {
            auto cuenta = view["cuenta"].get_document().view();
            std::cout << "=== INFORMACIÓN DE LA CUENTA ===\n";
            std::cout << "Número: " << view["numeroCuenta"].get_string().value << "\n";

            if (cuenta.find("tipo") != cuenta.end()) {
                std::cout << "Tipo: " << cuenta["tipo"].get_string().value << "\n";
            }
            if (cuenta.find("saldo") != cuenta.end()) {
                std::cout << "Saldo: $" << std::fixed << std::setprecision(2)
                    << cuenta["saldo"].get_double().value << "\n";
            }
            if (cuenta.find("fechaApertura") != cuenta.end()) {
                std::cout << "Fecha de apertura: " << cuenta["fechaApertura"].get_string().value << "\n";
            }
            if (cuenta.find("estado") != cuenta.end()) {
                std::cout << "Estado: " << cuenta["estado"].get_string().value << "\n";
            }
        }
    }

    system("pause");
}

// === MÉTODOS DE PRESENTACIÓN ===

void BuscadorCuentas::mostrarResultadosFechas(const std::vector<bsoncxx::document::value>& resultados) {
    std::cout << "=== CUENTAS ENCONTRADAS ===\n";
    std::cout << "Total de cuentas: " << resultados.size() << "\n\n";

    for (const auto& doc : resultados) {
        auto view = doc.view();

        std::cout << "--- CUENTA ---\n";
        std::cout << "Titular: " << view["nombre"].get_string().value << " "
            << view["apellido"].get_string().value << "\n";
        std::cout << "Cédula: " << view["cedula"].get_string().value << "\n";
        std::cout << "Correo: " << view["correo"].get_string().value << "\n";

        auto cuenta = view["cuenta"].get_document().view();
        std::cout << "Número de cuenta: " << cuenta["numeroCuenta"].get_string().value << "\n";
        std::cout << "Tipo: " << cuenta["tipo"].get_string().value << "\n";
        std::cout << "Fecha de apertura: " << cuenta["fechaApertura"].get_string().value << "\n";
        std::cout << "Saldo: $" << std::fixed << std::setprecision(2)
            << cuenta["saldo"].get_double().value << "\n";
        std::cout << std::string(30, '-') << "\n";
    }
}

void BuscadorCuentas::mostrarResultadosCriterio(const std::vector<bsoncxx::document::value>& resultados) {
    std::cout << "Total de personas encontradas: " << resultados.size() << "\n\n";

    for (const auto& doc : resultados) {
        auto view = doc.view();
        mostrarPersonaCompleta(view);
        std::cout << std::string(50, '=') << "\n";
    }
}

void BuscadorCuentas::mostrarResultadoCedula(const bsoncxx::document::value& resultado) {
    auto view = resultado.view();
    mostrarPersonaCompleta(view);
}

void BuscadorCuentas::mostrarPersonaCompleta(const bsoncxx::document::view& persona) {
    std::cout << "=== INFORMACIÓN PERSONAL ===\n";
    std::cout << "Cédula: " << persona["cedula"].get_string().value << "\n";
    std::cout << "Nombre: " << persona["nombre"].get_string().value << " "
        << persona["apellido"].get_string().value << "\n";
    std::cout << "Fecha de nacimiento: " << persona["fechaNacimiento"].get_string().value << "\n";
    std::cout << "Correo: " << persona["correo"].get_string().value << "\n";
    std::cout << "Dirección: " << persona["direccion"].get_string().value << "\n";

    // Mostrar estadísticas de cuentas
    std::cout << "\n=== ESTADÍSTICAS DE CUENTAS ===\n";
    if (persona.find("numAhorros") != persona.end()) {
        std::cout << "Cuentas de ahorros: " << persona["numAhorros"].get_int32().value << "\n";
    }
    if (persona.find("numCorrientes") != persona.end()) {
        std::cout << "Cuentas corrientes: " << persona["numCorrientes"].get_int32().value << "\n";
    }
    if (persona.find("totalCuentasExistentes") != persona.end()) {
        std::cout << "Total de cuentas: " << persona["totalCuentasExistentes"].get_int32().value << "\n";
    }

    // Mostrar todas las cuentas
    if (persona.find("cuentas") != persona.end() &&
        persona["cuentas"].type() == bsoncxx::type::k_array) {
        auto cuentas = persona["cuentas"].get_array().value;
        if (std::distance(cuentas.begin(), cuentas.end()) > 0) {
            std::cout << "\n=== CUENTAS ASOCIADAS ===\n";
            mostrarCuentasPersona(cuentas);
        }
        else {
            std::cout << "\n--- No tiene cuentas asociadas ---\n";
        }
    }

    std::cout << "\n";
}

void BuscadorCuentas::mostrarCuentasPersona(const bsoncxx::array::view& cuentas) {
    int contador = 1;

    for (auto& cuenta : cuentas) {
        if (cuenta.type() == bsoncxx::type::k_document) {
            auto cuentaDoc = cuenta.get_document().view();

            std::cout << "--- CUENTA " << contador++ << " ---\n";

            if (cuentaDoc.find("numeroCuenta") != cuentaDoc.end()) {
                std::cout << "Número: " << cuentaDoc["numeroCuenta"].get_string().value << "\n";
            }
            if (cuentaDoc.find("tipo") != cuentaDoc.end()) {
                std::cout << "Tipo: " << cuentaDoc["tipo"].get_string().value << "\n";
            }
            if (cuentaDoc.find("saldo") != cuentaDoc.end()) {
                std::cout << "Saldo: $" << std::fixed << std::setprecision(2)
                    << cuentaDoc["saldo"].get_double().value << "\n";
            }
            if (cuentaDoc.find("fechaApertura") != cuentaDoc.end()) {
                std::cout << "Fecha de apertura: " << cuentaDoc["fechaApertura"].get_string().value << "\n";
            }
            if (cuentaDoc.find("estado") != cuentaDoc.end()) {
                std::cout << "Estado: " << cuentaDoc["estado"].get_string().value << "\n";
            }
            if (cuentaDoc.find("sucursal") != cuentaDoc.end()) {
                std::cout << "Sucursal: " << cuentaDoc["sucursal"].get_string().value << "\n";
            }

            std::cout << std::string(25, '-') << "\n";
        }
    }
}