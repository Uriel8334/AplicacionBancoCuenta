#include "GeneradorQRBanco.h"
#include "Utilidades.h"
#include <ctime>

// Enum para tipos de formato de datos
enum class FormatoDatos {
    ESTRUCTURADO,  // Formato legible estructurado
    JSON          // Formato JSON estándar
};

// Constructor que recibe datos de persona y cuenta
GeneradorQRBanco::GeneradorQRBanco(const Persona& persona, const std::string& numCuenta) {
    numeroCuenta = numCuenta;
    nombreCompleto = convertirAMayusculas(persona.getNombres().substr(0,38) + " " + persona.getApellidos().substr(0,38));
    formatoSalida = FormatoDatos::ESTRUCTURADO; // Inicializar formato por defecto

    // Truncar nombre si es muy largo (máximo 76 caracteres)
    if (nombreCompleto.length() > 76) {
        nombreCompleto = nombreCompleto.substr(0, 76);
    }

    // Determinar el tamaño según la cantidad de datos
    int longitudDatos = calcularLongitudDatos();
    if (longitudDatos <= 152) {
        size = 21; // Versión 1
    }
    else if (longitudDatos <= 272) {
        size = 25; // Versión 2
    }
    else {
        size = 29; // Versión 3
    }

    matrix.resize(size, std::vector<bool>(size, false));
}

// Constructor directo con strings
GeneradorQRBanco::GeneradorQRBanco(const std::string& nombre, const std::string& numCuenta) {
    numeroCuenta = numCuenta;
    nombreCompleto = convertirAMayusculas(nombre);
    formatoSalida = FormatoDatos::ESTRUCTURADO; // Inicializar formato por defecto

    // Truncar nombre si es muy largo
    if (nombreCompleto.length() > 76) {
        nombreCompleto = nombreCompleto.substr(0, 76);
    }

    int longitudDatos = calcularLongitudDatos();
    if (longitudDatos <= 152) {
        size = 21; // Versión 1
    }
    else if (longitudDatos <= 272) {
        size = 25; // Versión 2
    }
    else {
        size = 29; // Versión 3
    }

    matrix.resize(size, std::vector<bool>(size, false));
}

/// @brief Convierte una cadena de texto a mayúsculas.
/// @param str La cadena de texto a convertir.
/// @return Una cadena convertida a mayúsculas.
/// @throws No se lanza ninguna excepción.
/// @details Esta función utiliza la clase Utilidades para realizar la conversión.
std::string GeneradorQRBanco::convertirAMayusculas(const std::string& str) {
    return Utilidades::ConvertirAMayusculas(str);
}

// Método mejorado para calcular longitud de datos
int GeneradorQRBanco::calcularLongitudDatos() {
    std::string datos = generarDatosEstructurados();
    return datos.length() * 8; // Aproximación en bits
}

// NUEVO: Método para generar datos en formato estructurado
std::string GeneradorQRBanco::generarDatosEstructurados() {
    if (formatoSalida == FormatoDatos::JSON) {
        return generarFormatoJSON();
    } else {
        return generarFormatoEstructurado();
    }
}

// NUEVO: Generar formato estructurado legible
std::string GeneradorQRBanco::generarFormatoEstructurado() {
    std::string datos;
    datos += "NOMBRE: " + nombreCompleto + "\n";
    datos += "N. CUENTA: " + numeroCuenta + "\n";
    datos += "BANCO: BANCO PICHINCHA DIDACTICO\n";
    datos += "TIPO: CUENTA BANCARIA\n";
    
    // Añadir timestamp para autenticidad
    time_t tiempoActual = time(nullptr);
    struct tm timeinfo;
    localtime_s(&timeinfo, &tiempoActual);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    datos += "GENERADO: " + std::string(buffer);
    
    return datos;
}

// NUEVO: Generar formato JSON estándar
std::string GeneradorQRBanco::generarFormatoJSON() {
    // Obtener timestamp
    time_t tiempoActual = time(nullptr);
    struct tm timeinfo;
    localtime_s(&timeinfo, &tiempoActual);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    
    std::string json = "{\n";
    json += "  \"tipo\": \"cuenta_bancaria\",\n";
    json += "  \"banco\": \"BANCO PICHINCHA DIDACTICO\",\n";
    json += "  \"titular\": \"" + nombreCompleto + "\",\n";
    json += "  \"numero_cuenta\": \"" + numeroCuenta + "\",\n";
    json += "  \"formato\": \"QR_BANCARIO_V1\",\n";
    json += "  \"generado\": \"" + std::string(buffer) + "\"\n";
    json += "}";
    
    return json;
}

// Método codificarDatosBancarios actualizado
std::vector<bool> GeneradorQRBanco::codificarDatosBancarios() {
    std::vector<bool> codificado;

    // Generar datos en formato estructurado
    std::string datos = generarDatosEstructurados();

    // Para QR compatibles con escáneres estándar, usar modo byte (0100)
    // en lugar de alfanumérico para mejor compatibilidad
    codificado.push_back(0);
    codificado.push_back(1);
    codificado.push_back(0);
    codificado.push_back(0);

    // Indicador de longitud (8 bits para versión 1, 16 para versión 2+)
    int bitsLongitud = (size == 21) ? 8 : 16;
    int longitud = static_cast<int>(datos.length());

    for (int i = bitsLongitud - 1; i >= 0; i--) {
        codificado.push_back((longitud >> i) & 1);
    }

    // Codificación en modo byte (UTF-8)
    for (char c : datos) {
        unsigned char byte = static_cast<unsigned char>(c);
        for (int j = 7; j >= 0; j--) {
            codificado.push_back((byte >> j) & 1);
        }
    }

    return codificado;
}

// Añade patrones de posición
void GeneradorQRBanco::agregarPatronesPosicion() {
    // Patrón 7x7 para cada esquina
    auto agregarPatron = [this](int filaInicio, int colInicio) {
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                bool oscuro = (i == 0 || i == 6 || j == 0 || j == 6 ||
                    (i >= 2 && i <= 4 && j >= 2 && j <= 4));
                matrix[filaInicio + i][colInicio + j] = oscuro;
            }
        }
        };

    agregarPatron(0, 0);                    // Superior izquierda
    agregarPatron(0, size - 7);            // Superior derecha
    agregarPatron(size - 7, 0);            // Inferior izquierda

    agregarSeparadores();
}

// Añade separadores alrededor de los patrones de posición
void GeneradorQRBanco::agregarSeparadores() {
    // Separador superior izquierdo
    for (int i = 0; i < 8; i++) {
        matrix[7][i] = false;
        matrix[i][7] = false;
    }

    // Separador superior derecho
    for (int i = 0; i < 8; i++) {
        matrix[7][size - 8 + i] = false;
        matrix[i][size - 8] = false;
    }

    // Separador inferior izquierdo
    for (int i = 0; i < 8; i++) {
        matrix[size - 8][i] = false;
        matrix[size - 8 + i][7] = false;
    }
}

// Añade patrones de tiempo
void GeneradorQRBanco::agregarPatronesTiempo() {
    for (int i = 8; i < size - 8; i++) {
        matrix[6][i] = (i % 2 == 0);
        matrix[i][6] = (i % 2 == 0);
    }
}

// Añade información de formato
void GeneradorQRBanco::agregarInformacionFormato() {
    // Información de formato simplificada (nivel de corrección L, máscara 0)
    std::vector<bool> infoFormato = {
        0,1,1,0,1,1,0,0,0,0,1,0,0,1,0  // 15 bits
    };

    // Colocar alrededor del patrón superior izquierdo
    for (int i = 0; i < 6; i++) {
        matrix[8][i] = infoFormato[i];
        matrix[size - 1 - i][8] = infoFormato[i];
    }

    matrix[8][7] = infoFormato[6];
    matrix[8][8] = infoFormato[7];
    matrix[7][8] = infoFormato[8];

    for (int i = 0; i < 7; i++) {
        matrix[8][size - 7 + i] = infoFormato[8 + i];
        matrix[6 - i][8] = infoFormato[8 + i];
    }
}

// Añade el módulo oscuro
void GeneradorQRBanco::agregarModuloOscuro() {
    matrix[4 * ((size - 17) / 4) + 9][8] = true;
}

// Verifica si una posición está reservada
bool GeneradorQRBanco::estaReservado(int fila, int col) {
    // Patrones de posición y separadores
    if ((fila < 9 && col < 9) ||
        (fila < 9 && col >= size - 8) ||
        (fila >= size - 8 && col < 9)) {
        return true;
    }

    // Patrones de tiempo
    if (fila == 6 || col == 6) {
        return true;
    }

    // Información de formato
    if ((fila == 8 && (col < 9 || col >= size - 8)) ||
        (col == 8 && (fila < 9 || fila >= size - 7))) {
        return true;
    }

    return false;
}

// Coloca los datos en la matriz
void GeneradorQRBanco::colocarDatos(const std::vector<bool>& datos) {
    int indicedatos = 0;
    bool arriba = true;

    for (int col = size - 1; col >= 1; col -= 2) {
        if (col == 6) col--; // Saltar columna de timing

        for (int cuenta = 0; cuenta < size; cuenta++) {
            int fila = arriba ? size - 1 - cuenta : cuenta;

            for (int c = 0; c < 2; c++) {
                int colActual = col - c;

                if (!estaReservado(fila, colActual)) {
                    if (indicedatos < static_cast<int>(datos.size())) {
                        matrix[fila][colActual] = datos[indicedatos++];
                    }
                    else {
                        matrix[fila][colActual] = false; // Padding
                    }
                }
            }
        }
        arriba = !arriba;
    }
}

// Aplica máscara para mejorar la legibilidad
void GeneradorQRBanco::aplicarMascara(PatronMascara patron) {
    for (int fila = 0; fila < size; fila++) {
        for (int col = 0; col < size; col++) {
            if (!estaReservado(fila, col)) {
                bool condicionMascara = false;

                switch (patron) {
                case MASCARA_000:
                    condicionMascara = ((fila + col) % 2 == 0);
                    break;
                case MASCARA_001:
                    condicionMascara = (fila % 2 == 0);
                    break;
                case MASCARA_010:
                    condicionMascara = (col % 3 == 0);
                    break;
                case MASCARA_011:
                    condicionMascara = ((fila + col) % 3 == 0);
                    break;
                case MASCARA_100:
                    condicionMascara = (((fila / 2) + (col / 3)) % 2 == 0);
                    break;
                case MASCARA_101:
                    condicionMascara = ((fila * col) % 2 + (fila * col) % 3 == 0);
                    break;
                case MASCARA_110:
                    condicionMascara = (((fila * col) % 2 + (fila * col) % 3) % 2 == 0);
                    break;
                case MASCARA_111:
                    condicionMascara = (((fila + col) % 2 + (fila * col) % 3) % 2 == 0);
                    break;
                }

                if (condicionMascara) {
                    matrix[fila][col] = !matrix[fila][col];
                }
            }
        }
    }
}

// Obtiene la capacidad de datos según el tamaño
int GeneradorQRBanco::obtenerCapacidadDatos() {
    switch (size) {
    case 21: return 152;  // Versión 1
    case 25: return 272;  // Versión 2
    case 29: return 440;  // Versión 3
    default: return 152;
    }
}

// Método generar actualizado con mejores mensajes
void GeneradorQRBanco::generar() {
    std::cout << "Generando QR bancario compatible con escaneres..." << std::endl;
    std::cout << "- Titular: " << nombreCompleto << std::endl;
    std::cout << "- Cuenta: " << numeroCuenta << std::endl;
    std::cout << "- Banco: BANCO PICHINCHA DIDACTICO" << std::endl;
    std::cout << "- Formato: " << (formatoSalida == FormatoDatos::JSON ? "JSON" : "ESTRUCTURADO") << std::endl;

    agregarPatronesPosicion();
    agregarPatronesTiempo();
    agregarInformacionFormato();
    agregarModuloOscuro();

    std::vector<bool> datosCodificados = codificarDatosBancarios();

    // Añadir bits de terminación
    for (int i = 0; i < 4 && static_cast<int>(datosCodificados.size()) < obtenerCapacidadDatos(); i++) {
        datosCodificados.push_back(0);
    }

    // Padding con bytes alternos
    while (static_cast<int>(datosCodificados.size()) < obtenerCapacidadDatos()) {
        // 11101100
        for (int i = 0; i < 8 && static_cast<int>(datosCodificados.size()) < obtenerCapacidadDatos(); i++) {
            datosCodificados.push_back((0xEC >> (7 - i)) & 1);
        }
        // 00010001
        for (int i = 0; i < 8 && static_cast<int>(datosCodificados.size()) < obtenerCapacidadDatos(); i++) {
            datosCodificados.push_back((0x11 >> (7 - i)) & 1);
        }
    }

    colocarDatos(datosCodificados);
    aplicarMascara(MASCARA_000); // Aplicar máscara básica
}

// Método imprimir actualizado
void GeneradorQRBanco::imprimir() {
    std::cout << "\n=== CODIGO QR BANCARIO GENERADO ===" << std::endl;
    std::cout << "Tamano: " << size << "x" << size << " (Version " << (size - 17) / 4 + 1 << ")" << std::endl;
    std::cout << "Formato: " << (formatoSalida == FormatoDatos::JSON ? "JSON" : "ESTRUCTURADO") << std::endl;
    std::cout << std::string(size * 2 + 4, '=') << std::endl;

    for (int i = 0; i < size; i++) {
        std::cout << "|| ";
        for (int j = 0; j < size; j++) {
            // OPCIÓN 1: Usar caracteres ASCII básicos
            std::cout << (matrix[i][j] ? "##" : "  ");

            // OPCIÓN 2: Usar caracteres del conjunto extendido ASCII
            // std::cout << (matrix[i][j] ? "\xDB\xDB" : "  ");

            // OPCIÓN 3: Usar asteriscos y espacios
            // std::cout << (matrix[i][j] ? "**" : "  ");
        }
        std::cout << " ||" << std::endl;
    }

    std::cout << std::string(size * 2 + 4, '=') << std::endl;
    
    // Mostrar contenido del QR para verificación
    std::cout << "\nCONTENIDO DEL QR:" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    std::cout << generarDatosEstructurados() << std::endl;
    std::cout << std::string(50, '-') << std::endl;
}

// Exporta como imagen SVG
void GeneradorQRBanco::guardarComoSVG(const std::string& nombreArchivo) {
    std::ofstream archivo(nombreArchivo);
    if (archivo.is_open()) {
        int escala = 8;
        int borde = 4 * escala;
        int tamanoTotal = size * escala + 2 * borde;

        archivo << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        archivo << "<svg width=\"" << tamanoTotal << "\" height=\"" << tamanoTotal
            << "\" xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;

        // Fondo blanco
        archivo << "<rect width=\"" << tamanoTotal << "\" height=\"" << tamanoTotal
            << "\" fill=\"white\"/>" << std::endl;

        // Módulos del QR
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (matrix[i][j]) {
                    archivo << "<rect x=\"" << borde + j * escala
                        << "\" y=\"" << borde + i * escala
                        << "\" width=\"" << escala << "\" height=\"" << escala
                        << "\" fill=\"black\"/>" << std::endl;
                }
            }
        }

        // Información del usuario
        archivo << "<text x=\"" << tamanoTotal / 2 << "\" y=\"" << tamanoTotal - 10
            << "\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"12\" fill=\"black\">"
            << nombreCompleto << " - " << numeroCuenta << "</text>" << std::endl;

        archivo << "</svg>" << std::endl;
        archivo.close();
        std::cout << "QR guardado como: " << nombreArchivo << std::endl;
    }
}

// Método guardarInformacionCuenta actualizado
void GeneradorQRBanco::guardarInformacionCuenta(const std::string& nombreArchivo) {
    std::ofstream archivo(nombreArchivo);
    if (archivo.is_open()) {
        time_t tiempoActual = time(nullptr);
        struct tm timeinfo;
        localtime_s(&timeinfo, &tiempoActual);
        char buffer[128];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);

        archivo << "=== INFORMACION DE QR BANCARIO ===" << std::endl;
        archivo << "Fecha de generacion: " << buffer << std::endl;
        archivo << "Banco: BANCO PICHINCHA DIDACTICO" << std::endl;
        archivo << "Titular: " << nombreCompleto << std::endl;
        archivo << "Numero de cuenta: " << numeroCuenta << std::endl;
        archivo << "Tamano del QR: " << size << "x" << size << std::endl;
        archivo << "Formato de datos: " << (formatoSalida == FormatoDatos::JSON ? "JSON" : "ESTRUCTURADO") << std::endl;
        archivo << std::endl;
        archivo << "CONTENIDO CODIFICADO EN EL QR:" << std::endl;
        archivo << std::string(40, '-') << std::endl;
        archivo << generarDatosEstructurados() << std::endl;
        archivo << std::string(40, '-') << std::endl;
        archivo << std::endl;
        archivo << "NOTAS DE SEGURIDAD:" << std::endl;
        archivo << "- Este QR contiene solo informacion basica de identificacion" << std::endl;
        archivo << "- No incluye datos sensibles como cedula o PIN" << std::endl;
        archivo << "- Compatible con aplicaciones de escaneo estandar" << std::endl;
        archivo << "- Generado para fines didacticos" << std::endl;
        archivo.close();
        std::cout << "Informacion completa guardada en: " << nombreArchivo << std::endl;
    }
}

// Función para validar número de cuenta
bool GeneradorQRBanco::esNumeroCuentaValido(const std::string& cuenta) {
    // Validar que sea exactamente 10 dígitos
    if (cuenta.length() != 10) {
        return false;
    }
    // Validar que todos los caracteres sean dígitos
    for (char c : cuenta) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

// Función para validar nombre
bool GeneradorQRBanco::esNombreValido(const std::string& nombre) {
    if (nombre.length() < 2 || nombre.length() > 60) {
        return false;
    }

    for (char c : nombre) {
        if (!std::isalpha(static_cast<unsigned char>(c)) && c != ' ' && c != '-') {
            return false;
        }
    }

    return true;
}