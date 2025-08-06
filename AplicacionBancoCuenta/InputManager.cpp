#include "InputManager.h"
#include "Utilidades.h"
#include <windows.h>
#include <algorithm>
#include <iomanip>

// Implementación de InputManager
InputManager::InputManager() {
    inicializarConfiguraciones();
}

void InputManager::inicializarConfiguraciones() {
    //configurarNumerico();
    configurarCedula();
    //configurarAlfabetico();
    configurarAlfabetiroEspacios();
    //configurarAlfanumerico();
    //configurarDecimal();
    configurarCorreoLocal();
    configurarCorreoCompleto();
    configurarFechaNacimiento();
    configurarDireccion();
    //configurarNumeroCuenta();
    configurarMonto();
    //configurarTextoLibre();
}

void InputManager::configurarCorreoLocal() {
    ConfiguracionInput config;
    config.longitudMaxima = 64;
    config.longitudMinima = 1;
    config.permitirVacio = false;
    config.mensajeError = "Formato de correo inválido en la parte local";

    config.validadorCaracter = [](char c) -> bool {
        return ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '.' || c == '_' || c == '-');
        };

    config.validadorCompleto = [](const std::string& valor) -> bool {
        // Validaciones para la parte local del correo
        if (valor.empty() || valor.front() == '.' || valor.back() == '.') return false;
        if (valor.find("..") != std::string::npos) return false;
        return true;
        };

    configuraciones[TipoInput::CORREO_LOCAL] = config;
}

void InputManager::configurarCorreoCompleto() {
    ConfiguracionInput config;
    config.longitudMaxima = 254;
    config.longitudMinima = 5;
    config.permitirVacio = false;
    config.mensajeError = "Formato de correo electrónico inválido";

    config.validadorCaracter = [](char c) -> bool {
        return ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '.' || c == '_' || c == '-' || c == '@');
        };

    config.validadorCompleto = [](const std::string& valor) -> bool {
        // Validación completa del correo usando regex
        std::regex regexCorreo(R"(^[A-Za-z0-9]+([._-]?[A-Za-z0-9]+)*@[A-Za-z0-9]+([.-]?[A-Za-z0-9]+)*\.[A-Za-z]{2,}$)");
        if (!std::regex_match(valor, regexCorreo)) return false;

        size_t atPos = valor.find('@');
        if (atPos == std::string::npos) return false;

        std::string usuario = valor.substr(0, atPos);
        std::string dominio = valor.substr(atPos + 1);

        // Validaciones adicionales
        if (dominio.find('@') != std::string::npos) return false;
        if (!dominio.empty() && (dominio[0] == '.' || dominio[0] == '-' || dominio[0] == '_')) return false;
        if (dominio.find("..") != std::string::npos) return false;
        if (!usuario.empty() && (usuario.back() == '.' || usuario.front() == '.')) return false;
        if (usuario.find("..") != std::string::npos) return false;

        return true;
        };

    configuraciones[TipoInput::CORREO_COMPLETO] = config;
}

void InputManager::configurarFechaNacimiento() {
    ConfiguracionInput config;
    config.longitudMaxima = 10;
    config.longitudMinima = 10;
    config.permitirVacio = false;
    config.mensajeError = "Formato de fecha inválido (DD/MM/AAAA)";

    config.validadorCaracter = [](char c) -> bool {
        return (c >= '0' && c <= '9') || c == '/';
        };

    config.validadorCompleto = [](const std::string& valor) -> bool {
        // Validar formato básico
        if (!Validar::ValidarFecha(valor)) return false;

        // Verificar que no sea fecha futura
        SYSTEMTIME st;
        GetLocalTime(&st);

        int anioActual = st.wYear;
        int mesActual = st.wMonth;
        int diaActual = st.wDay;

        // Extraer componentes de la fecha
        std::stringstream ss(valor);
        std::string item;
        std::vector<int> fechaComponentes;

        while (std::getline(ss, item, '/')) {
            fechaComponentes.push_back(std::stoi(item));
        }

        if (fechaComponentes.size() != 3) return false;

        int dia = fechaComponentes[0];
        int mes = fechaComponentes[1];
        int anio = fechaComponentes[2];

        // Verificar que no sea fecha futura
        bool fechaNoFutura = (anio < anioActual) ||
            (anio == anioActual && mes < mesActual) ||
            (anio == anioActual && mes == mesActual && dia <= diaActual);

        return fechaNoFutura;
        };

    configuraciones[TipoInput::FECHA_NACIMIENTO] = config;
}

void InputManager::configurarCedula() {
    ConfiguracionInput config;
    config.longitudMaxima = 10;
    config.longitudMinima = 10;
    config.permitirVacio = false;
    config.mensajeError = "Cédula debe tener exactamente 10 dígitos";

    config.validadorCaracter = [](char c) -> bool {
        return c >= '0' && c <= '9';
        };

    config.validadorCompleto = [](const std::string& valor) -> bool {
        return valor.length() == 10 && Validar::ValidarCedula(valor);
        };

    configuraciones[TipoInput::CEDULA] = config;
}

void InputManager::configurarAlfabetiroEspacios() {
    ConfiguracionInput config;
    config.longitudMaxima = 100;
    config.longitudMinima = 1;
    config.permitirVacio = false;
    config.mensajeError = "Solo se permiten letras y espacios";

    config.validadorCaracter = [](char c) -> bool {
        return ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == ' ');
        };

    config.validadorCompleto = [](const std::string& valor) -> bool {
        return Validar::ValidarNombrePersona(valor);
        };

    configuraciones[TipoInput::ALFABETICO_ESPACIOS] = config;
}

void InputManager::configurarMonto() {
    ConfiguracionInput config;
    config.longitudMaxima = 15;
    config.longitudMinima = 1;
    config.permitirVacio = false;
    config.mensajeError = "Formato de monto inválido";

    config.validadorCaracter = [](char c) -> bool {
        return (c >= '0' && c <= '9') || c == '.';
        };

    config.validadorCompleto = [](const std::string& valor) -> bool {
        try {
            double monto = std::stod(valor);
            return monto >= 0;
        }
        catch (...) {
            return false;
        }
        };

    configuraciones[TipoInput::MONTO] = config;
}

void InputManager::configurarDireccion() {
    ConfiguracionInput config;
    config.longitudMaxima = 100;
    config.longitudMinima = 1;
    config.permitirVacio = false;
    config.mensajeError = "Dirección no válida";

    config.validadorCaracter = [](char c) -> bool {
        return ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == ' ' || c == '.' || c == '-' ||
            c == '#' || c == ',' || c == 'º');
        };

    configuraciones[TipoInput::DIRECCION] = config;
}

ConfiguracionInput& InputManager::obtenerConfiguracion(TipoInput tipo) {
    auto it = configuraciones.find(tipo);
    if (it == configuraciones.end()) {
        throw std::runtime_error("Tipo de entrada no configurado: " + std::to_string(static_cast<int>(tipo)));
    }
    return it->second;
}

const ConfiguracionInput& InputManager::obtenerConfiguracion(TipoInput tipo) const {
    auto it = configuraciones.find(tipo);
    if (it == configuraciones.end()) {
        throw std::runtime_error("Tipo de entrada no configurado: " + std::to_string(static_cast<int>(tipo)));
    }
    return it->second;
}

// Agregar al switch en obtenerEntrada
std::string InputManager::obtenerEntrada(TipoInput tipo, const std::string& prompt,
    const std::string& valorInicial) {
    auto it = configuraciones.find(tipo);
    if (it == configuraciones.end()) {
        throw std::runtime_error("Tipo de entrada no configurado");
    }

    const ConfiguracionInput& config = it->second;
    std::string entrada = valorInicial;
    bool espacioPermitido = false;
    bool puntoDecimalUsado = false;
    bool arrobaUsado = false;
    int decimalesIngresados = 0;
    int barrasUsadas = 0;

    while (true) {
        Utilidades::limpiarPantallaPreservandoMarquesina(1);
        std::cout << prompt << ": ";
        std::cout << entrada;

        char tecla = _getch();

        // Manejar teclas especiales
        if (procesarTeclaEspecial(tecla, entrada, decimalesIngresados)) {
            continue;
        }

        // Enter - validar y terminar
        if (tecla == 13) {
            if (entrada.empty() && !config.permitirVacio) {
                std::cout << "\nEl campo no puede estar vacío. Presione cualquier tecla...";
                int teclaCualquiera = _getch();
                (void)teclaCualquiera;
                continue;
            }

            if (validarEntradaCompleta(entrada, config)) {
                std::cout << std::endl;
                return entrada;
            }
            else {
                std::cout << "\n" << config.mensajeError << ". Presione cualquier tecla...";
                int teclaCualquiera = _getch();
                (void)teclaCualquiera;
                continue;
            }
        }

        // Backspace
        if (tecla == 8 && !entrada.empty()) {
            char ultimoCaracter = entrada.back();
            entrada.pop_back();

            // Ajustar estados especiales según el tipo
            if (tipo == TipoInput::ALFABETICO_ESPACIOS && ultimoCaracter == ' ') {
                espacioPermitido = false;
            }
            else if (tipo == TipoInput::MONTO && ultimoCaracter == '.') {
                puntoDecimalUsado = false;
                decimalesIngresados = 0;
            }
            else if (tipo == TipoInput::MONTO && puntoDecimalUsado && decimalesIngresados > 0) {
                decimalesIngresados--;
            }
            else if ((tipo == TipoInput::CORREO_COMPLETO || tipo == TipoInput::CORREO_LOCAL) && ultimoCaracter == '@') {
                arrobaUsado = false;
            }
            else if (tipo == TipoInput::FECHA_NACIMIENTO && ultimoCaracter == '/') {
                barrasUsadas--;
            }
            continue;
        }

        // Validar carácter según el tipo
        if (entrada.length() >= config.longitudMaxima) {
            continue; // No permitir más caracteres
        }

        bool caracterValido = false;

        // Lógica específica por tipo
        switch (tipo) {
        case TipoInput::ALFABETICO_ESPACIOS:
            if (config.validadorCaracter(tecla)) {
                if (tecla == ' ') {
                    if (espacioPermitido && !entrada.empty()) {
                        caracterValido = true;
                        espacioPermitido = false;
                    }
                }
                else {
                    caracterValido = true;
                    espacioPermitido = true;
                }
            }
            break;

        case TipoInput::MONTO:
            if (tecla == '.' && !puntoDecimalUsado && !entrada.empty()) {
                caracterValido = true;
                puntoDecimalUsado = true;
                decimalesIngresados = 0;
            }
            else if (tecla >= '0' && tecla <= '9') {
                if (puntoDecimalUsado && decimalesIngresados >= 2) {
                    caracterValido = false; // Máximo 2 decimales
                }
                else {
                    caracterValido = true;
                    if (puntoDecimalUsado) decimalesIngresados++;
                }
            }
            break;

        case TipoInput::CORREO_COMPLETO:
            if (config.validadorCaracter(tecla)) {
                if (tecla == '@' && !arrobaUsado && !entrada.empty()) {
                    caracterValido = true;
                    arrobaUsado = true;
                }
                else if (tecla != '@') {
                    caracterValido = true;
                }
            }
            break;

        case TipoInput::FECHA_NACIMIENTO:
            if (tecla >= '0' && tecla <= '9') {
                // Validar formato DD/MM/AAAA
                if ((entrada.length() == 2 && barrasUsadas == 0) ||
                    (entrada.length() == 5 && barrasUsadas == 1)) {
                    continue; // Debe ingresar '/' en estas posiciones
                }
                caracterValido = true;
            }
            else if (tecla == '/' && barrasUsadas < 2) {
                if ((entrada.length() == 2 && barrasUsadas == 0) ||
                    (entrada.length() == 5 && barrasUsadas == 1)) {
                    caracterValido = true;
                    barrasUsadas++;
                }
            }
            break;

        default:
            caracterValido = config.validadorCaracter(tecla);
            break;
        }

        if (caracterValido) {
            entrada += tecla;
        }
    }
}

bool InputManager::procesarTeclaEspecial(char tecla, std::string& entrada, int& posicionCursor) {
    // Ignorar teclas especiales (flechas, etc.)
    if (tecla == 0 || tecla == -32 || tecla == 224) {
        int teclaCualquiera = _getch(); // Consumir el siguiente valor
		(void)teclaCualquiera; // No hacer nada con la tecla especial
        return true;
    }
    return false;
}

bool InputManager::validarEntradaCompleta(const std::string& entrada, const ConfiguracionInput& config) {
    if (entrada.length() < config.longitudMinima || entrada.length() > config.longitudMaxima) {
        return false;
    }

    if (config.validadorCompleto) {
        return config.validadorCompleto(entrada);
    }

    return true;
}

double InputManager::obtenerMonto(double min, double max, const std::string& prompt) {
    while (true) {
        std::string entrada = obtenerEntrada(TipoInput::MONTO, prompt);

        try {
            double valor = std::stod(entrada);
            if (valor >= min && valor <= max) {
                return valor;
            }
            std::cout << "\nEl valor debe estar entre " << min << " y " << max
                << ". Presione cualquier tecla...";
            int teclaCualquiera = _getch();
			(void)teclaCualquiera; // Consumir la tecla
        }
        catch (...) {
            std::cout << "\nFormato inválido. Presione cualquier tecla...";
            int teclaCualquiera = _getch();
			(void)teclaCualquiera; // Consumir la tecla
        }
    }
}

// Implementación del namespace InputUI
namespace InputUI {
    std::unique_ptr<InputManager> manager;

    void inicializar() {
        if (!manager) {
            manager = std::make_unique<InputManager>();
        }
    }

    const std::function<std::string(const std::string&)> ingresarCedula =
        [](const std::string& prompt) -> std::string {
        return manager->obtenerEntrada(TipoInput::CEDULA, prompt);
        };

    const std::function<std::string(const std::string&)> ingresarNombres =
        [](const std::string& prompt) -> std::string {
        return manager->obtenerEntrada(TipoInput::ALFABETICO_ESPACIOS, prompt);
        };

    const std::function<std::string(const std::string&)> ingresarCorreoLocal =
        [](const std::string& prompt) -> std::string {
        return manager->obtenerEntrada(TipoInput::CORREO_LOCAL, prompt);
        };

    const std::function<std::string(const std::string&)> ingresarCorreoCompleto =
        [](const std::string& prompt) -> std::string {
        return manager->obtenerEntrada(TipoInput::CORREO_COMPLETO, prompt);
        };

    const std::function<std::string(const std::string&)> ingresarFechaNacimiento =
        [](const std::string& prompt) -> std::string {
        return manager->obtenerEntrada(TipoInput::FECHA_NACIMIENTO, prompt);
        };

    const std::function<std::string(const std::string&)> ingresarDireccion =
        [](const std::string& prompt) -> std::string {
        return manager->obtenerEntrada(TipoInput::DIRECCION, prompt);
        };

    const std::function<double(double, double, const std::string&)> ingresarMonto =
        [](double min, double max, const std::string& prompt) -> double {
        return manager->obtenerMonto(min, max, prompt);
        };
}