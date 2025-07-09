#pragma once
#ifndef CODIGOQR_H
#define CODIGOQR_H

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

/**
 * @namespace CodigoQR
 * @brief Espacio de nombres que contiene las clases y funciones para generación de códigos QR
 *
 * Este namespace proporciona una implementación completa para generar, manipular
 * y visualizar códigos QR según el estándar ISO/IEC 18004.
 */
namespace CodigoQR {

    /**
     * @class QrSegment
     * @brief Representa un segmento de datos en un código QR
     *
     * Un código QR puede contener varios segmentos de diferentes tipos (numérico,
     * alfanumérico, bytes, etc.) Esta clase representa uno de estos segmentos.
     */
    class QrSegment final {

        /**
         * @class Mode
         * @brief Define los modos de codificación para segmentos QR
         *
         * Cada segmento QR puede usar un modo de codificación diferente según
         * el tipo de datos que contiene.
         */
    public: class Mode final {

        /** @brief Modo para datos numéricos (0-9) */
    public: static const Mode NUMERIC;
          /** @brief Modo para datos alfanuméricos (0-9, A-Z, y algunos símbolos) */
    public: static const Mode ALPHANUMERIC;
          /** @brief Modo para datos binarios de 8 bits */
    public: static const Mode BYTE;
          /** @brief Modo para caracteres Kanji (japoneses) */
    public: static const Mode KANJI;
          /** @brief Modo para información de interpretación extendida de canal */
    public: static const Mode ECI;

          /** @brief Bits que identifican este modo en el código QR */
    private: int modeBits;
           /** @brief Número de bits para contar caracteres según versión QR */
    private: int numBitsCharCount[3];
           /**
            * @brief Constructor para crear un modo
            * @param mode Bits del modo
            * @param cc0 Número de bits para versiones 1-9
            * @param cc1 Número de bits para versiones 10-26
            * @param cc2 Número de bits para versiones 27-40
            */
    private: Mode(int mode, int cc0, int cc1, int cc2);
           /**
            * @brief Obtiene los bits que identifican este modo
            * @return El valor de los bits del modo
            */
    public: int getModeBits() const;
          /**
           * @brief Obtiene el número de bits para contar caracteres
           * @param ver Versión del código QR
           * @return Número de bits para la versión especificada
           */
    public: int numCharCountBits(int ver) const;

    };
          /**
           * @brief Crea un segmento a partir de datos binarios
           * @param data Vector de bytes a codificar
           * @return Segmento QR en modo BYTE
           */
    public: static QrSegment makeBytes(const std::vector<std::uint8_t>& data);
          /**
           * @brief Crea un segmento a partir de dígitos numéricos
           * @param digits Cadena con dígitos numéricos
           * @return Segmento QR en modo NUMERIC
           */
    public: static QrSegment makeNumeric(const char* digits);
          /**
           * @brief Crea un segmento a partir de texto alfanumérico
           * @param text Cadena con texto alfanumérico
           * @return Segmento QR en modo ALPHANUMERIC
           */
    public: static QrSegment makeAlphanumeric(const char* text);
          /**
           * @brief Crea segmentos automáticamente a partir de texto
           * @param text Texto a codificar
           * @return Vector de segmentos QR optimizados para el texto
           */
    public: static std::vector<QrSegment> makeSegments(const char* text);
          /**
           * @brief Crea un segmento ECI con el valor asignado
           * @param assignVal Valor ECI a codificar
           * @return Segmento QR en modo ECI
           */
    public: static QrSegment makeEci(long assignVal);
          /**
           * @brief Verifica si un texto es puramente numérico
           * @param text Texto a verificar
           * @return true si todos los caracteres son dígitos numéricos
           */
    public: static bool isNumeric(const char* text);
          /**
           * @brief Verifica si un texto es alfanumérico según el estándar QR
           * @param text Texto a verificar
           * @return true si todos los caracteres son alfanuméricos válidos
           */
    public: static bool isAlphanumeric(const char* text);
          /** @brief Modo de codificación para este segmento */
    private: const Mode* mode;
           /** @brief Número de caracteres en este segmento */
    private: int numChars;
           /** @brief Datos del segmento como bits */
    private: std::vector<bool> data;
           /**
            * @brief Constructor para crear un segmento
            * @param md Modo de codificación
            * @param numCh Número de caracteres
            * @param dt Vector de bits con los datos
            */
    public: QrSegment(const Mode& md, int numCh, const std::vector<bool>& dt);
          /**
           * @brief Constructor con movimiento para datos
           * @param md Modo de codificación
           * @param numCh Número de caracteres
           * @param dt Vector de bits con los datos (movido)
           */
    public: QrSegment(const Mode& md, int numCh, std::vector<bool>&& dt);
          /**
           * @brief Obtiene el modo de este segmento
           * @return Referencia al modo de codificación
           */
    public: const Mode& getMode() const;
          /**
           * @brief Obtiene el número de caracteres
           * @return Cantidad de caracteres en este segmento
           */
    public: int getNumChars() const;
          /**
           * @brief Obtiene los datos codificados como bits
           * @return Vector de bits con los datos
           */
    public: const std::vector<bool>& getData() const;
          /**
           * @brief Calcula el número total de bits necesarios para los segmentos
           * @param segs Vector de segmentos
           * @param version Versión del código QR
           * @return Número total de bits
           */
    public: static int getTotalBits(const std::vector<QrSegment>& segs, int version);
          /** @brief Conjunto de caracteres válidos para el modo alfanumérico */
    private: static const char* ALPHANUMERIC_CHARSET;

    };

    /**
     * @class QrCode
     * @brief Representa un código QR completo con todos sus elementos
     *
     * Esta clase implementa la funcionalidad para crear y manipular códigos QR
     * con diferentes niveles de corrección de errores y versiones.
     */
    class QrCode final {

        /**
         * @brief Niveles de corrección de errores para códigos QR
         */
    public: enum class Ecc {
        LOW = 0,      /**< El código QR puede tolerar aproximadamente 7% de palabras código erróneas */
        MEDIUM,       /**< El código QR puede tolerar aproximadamente 15% de palabras código erróneas */
        QUARTILE,     /**< El código QR puede tolerar aproximadamente 25% de palabras código erróneas */
        HIGH,         /**< El código QR puede tolerar aproximadamente 30% de palabras código erróneas */
    };
          /**
           * @brief Obtiene los bits de formato basados en el nivel de corrección
           * @param ecl Nivel de corrección de errores
           * @return Bits de formato
           */
    private: static int getFormatBits(Ecc ecl);
           /**
            * @brief Codifica texto en un código QR
            * @param text Texto a codificar
            * @param ecl Nivel de corrección de errores
            * @return Objeto QrCode con el código generado
            */
    public: static QrCode encodeText(const char* text, Ecc ecl);
          /**
           * @brief Codifica datos binarios en un código QR
           * @param data Datos binarios a codificar
           * @param ecl Nivel de corrección de errores
           * @return Objeto QrCode con el código generado
           */
    public: static QrCode encodeBinary(const std::vector<std::uint8_t>& data, Ecc ecl);
          /**
           * @brief Codifica segmentos en un código QR
           * @param segs Segmentos a codificar
           * @param ecl Nivel de corrección de errores
           * @param minVersion Versión mínima (1-40)
           * @param maxVersion Versión máxima (1-40)
           * @param mask Patrón de máscara (-1 para automático)
           * @param boostEcl Permitir mejora automática del nivel de corrección
           * @return Objeto QrCode con el código generado
           */
    public: static QrCode encodeSegments(const std::vector<QrSegment>& segs, Ecc ecl,
        int minVersion = 1, int maxVersion = 40, int mask = -1, bool boostEcl = true);
          /** @brief Versión del código QR (1-40) */
    private: int version;
           /** @brief Tamaño del código QR en módulos */
    private: int size;
           /** @brief Nivel de corrección de errores */
    private: Ecc errorCorrectionLevel;
           /** @brief Patrón de máscara utilizado */
    private: int mask;
           /** @brief Matriz de módulos (true=negro, false=blanco) */
    private: std::vector<std::vector<bool> > modules;
           /** @brief Matriz de funciones (patrones fijos) */
    private: std::vector<std::vector<bool> > isFunction;
           /**
            * @brief Constructor para crear un código QR
            * @param ver Versión (1-40)
            * @param ecl Nivel de corrección de errores
            * @param dataCodewords Datos codificados
            * @param msk Patrón de máscara
            */
    public: QrCode(int ver, Ecc ecl, const std::vector<std::uint8_t>& dataCodewords, int msk);
          /**
           * @brief Obtiene la versión del código QR
           * @return Versión (1-40)
           */
    public: int getVersion() const;
          /**
           * @brief Obtiene el tamaño del código QR en módulos
           * @return Tamaño (ancho/alto en módulos)
           */
    public: int getSize() const;
          /**
           * @brief Obtiene el nivel de corrección de errores
           * @return Nivel de corrección
           */
    public: Ecc getErrorCorrectionLevel() const;
          /**
           * @brief Obtiene el patrón de máscara utilizado
           * @return Índice del patrón de máscara (0-7)
           */
    public: int getMask() const;
          /**
           * @brief Verifica si un módulo está activo (negro) en la posición dada
           * @param x Coordenada horizontal
           * @param y Coordenada vertical
           * @return true si el módulo está activo, false si es blanco
           */
    public: bool getModule(int x, int y) const;
          /**
           * @brief Dibuja los patrones de función (fijos) en el código QR
           */
    private: void drawFunctionPatterns();
           /**
            * @brief Dibuja los bits de formato con el patrón de máscara dado
            * @param msk Índice del patrón de máscara
            */
    private: void drawFormatBits(int msk);
           /**
            * @brief Dibuja la información de versión (solo para versiones 7+)
            */
    private: void drawVersion();
           /**
            * @brief Dibuja un patrón de búsqueda en la posición especificada
            * @param x Coordenada horizontal
            * @param y Coordenada vertical
            */
    private: void drawFinderPattern(int x, int y);
           /**
            * @brief Dibuja un patrón de alineación en la posición especificada
            * @param x Coordenada horizontal
            * @param y Coordenada vertical
            */
    private: void drawAlignmentPattern(int x, int y);
           /**
            * @brief Establece un módulo de función en la posición dada
            * @param x Coordenada horizontal
            * @param y Coordenada vertical
            * @param isDark Si el módulo debe ser negro (true) o blanco (false)
            */
    private: void setFunctionModule(int x, int y, bool isDark);
           /**
            * @brief Verifica el estado de un módulo
            * @param x Coordenada horizontal
            * @param y Coordenada vertical
            * @return true si el módulo está activo, false si es blanco
            */
    private: bool module(int x, int y) const;
           /**
            * @brief Añade corrección de errores y entrelazado a los datos
            * @param data Datos originales
            * @return Vector con datos y corrección de errores entrelazados
            */
    private: std::vector<std::uint8_t> addEccAndInterleave(const std::vector<std::uint8_t>& data) const;
           /**
            * @brief Dibuja los datos codificados en el código QR
            * @param data Datos codificados con corrección de errores
            */
    private: void drawCodewords(const std::vector<std::uint8_t>& data);
           /**
            * @brief Aplica un patrón de máscara a los datos
            * @param msk Índice del patrón de máscara (0-7)
            */
    private: void applyMask(int msk);
           /**
            * @brief Calcula la puntuación de penalización según las reglas del estándar
            * @return Puntuación de penalización
            */
    private: long getPenaltyScore() const;
           /**
            * @brief Obtiene las posiciones de los patrones de alineación
            * @return Vector con posiciones de patrones de alineación
            */
    private: std::vector<int> getAlignmentPatternPositions() const;
           /**
            * @brief Calcula el número total de módulos de datos sin formato
            * @param ver Versión del código QR
            * @return Número de módulos de datos
            */
    private: static int getNumRawDataModules(int ver);
           /**
            * @brief Calcula el número de palabras código de datos
            * @param ver Versión del código QR
            * @param ecl Nivel de corrección de errores
            * @return Número de palabras código
            */
    private: static int getNumDataCodewords(int ver, Ecc ecl);
           /**
            * @brief Calcula el divisor Reed-Solomon para corrección de errores
            * @param degree Grado del polinomio
            * @return Vector con coeficientes del divisor
            */
    private: static std::vector<std::uint8_t> reedSolomonComputeDivisor(int degree);
           /**
            * @brief Calcula el resto Reed-Solomon para corrección de errores
            * @param data Datos originales
            * @param divisor Divisor Reed-Solomon
            * @return Vector con el resto de la división
            */
    private: static std::vector<std::uint8_t> reedSolomonComputeRemainder(const std::vector<std::uint8_t>& data, const std::vector<std::uint8_t>& divisor);
           /**
            * @brief Multiplica dos valores en el campo finito GF(256)
            * @param x Primer valor
            * @param y Segundo valor
            * @return Resultado de la multiplicación
            */
    private: static std::uint8_t reedSolomonMultiply(std::uint8_t x, std::uint8_t y);
           /**
            * @brief Cuenta patrones para cálculo de penalización
            * @param runHistory Historial de longitudes de ejecución
            * @return Puntuación de penalización
            */
    private: int finderPenaltyCountPatterns(const std::array<int, 7>& runHistory) const;
           /**
            * @brief Termina una ejecución y cuenta para cálculo de penalización
            * @param currentRunColor Color actual
            * @param currentRunLength Longitud de la ejecución actual
            * @param runHistory Historial de longitudes de ejecución
            * @return Puntuación de penalización
            */
    private: int finderPenaltyTerminateAndCount(bool currentRunColor, int currentRunLength, std::array<int, 7>& runHistory) const;
           /**
            * @brief Añade una longitud de ejecución al historial
            * @param currentRunLength Longitud de la ejecución
            * @param runHistory Historial de longitudes de ejecución
            */
    private: void finderPenaltyAddHistory(int currentRunLength, std::array<int, 7>& runHistory) const;
           /**
            * @brief Obtiene un bit específico de un valor
            * @param x Valor
            * @param i Índice del bit (0 = LSB)
            * @return true si el bit está activo, false si no
            */
    private: static bool getBit(long x, int i);
           /** @brief Versión mínima de código QR (1) */
    public: static constexpr int MIN_VERSION = 1;
          /** @brief Versión máxima de código QR (40) */
    public: static constexpr int MAX_VERSION = 40;
          /** @brief Constante de penalización N1 */
    private: static const int PENALTY_N1;
           /** @brief Constante de penalización N2 */
    private: static const int PENALTY_N2;
           /** @brief Constante de penalización N3 */
    private: static const int PENALTY_N3;
           /** @brief Constante de penalización N4 */
    private: static const int PENALTY_N4;
           /** @brief Tabla de palabras código de corrección de errores por bloque */
    private: static const std::int8_t ECC_CODEWORDS_PER_BLOCK[4][41];
           /** @brief Tabla de número de bloques de corrección de errores */
    private: static const std::int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41];
    };

    /**
     * @class data_too_long
     * @brief Excepción lanzada cuando los datos son demasiado grandes para el código QR
     *
     * Esta excepción se lanza cuando se intenta codificar datos que exceden
     * la capacidad de la versión máxima del código QR.
     */
    class data_too_long : public std::length_error {
        /**
         * @brief Constructor
         * @param msg Mensaje de error
         */
    public: explicit data_too_long(const std::string& msg);
    };

    /**
     * @class BitBuffer
     * @brief Buffer de bits para construir datos QR
     *
     * Extiende std::vector<bool> con métodos para añadir bits de manera eficiente
     */
    class BitBuffer final : public std::vector<bool> {
        /**
         * @brief Constructor por defecto
         */
    public: BitBuffer();
          /**
           * @brief Añade bits al buffer
           * @param val Valor a codificar
           * @param len Número de bits a usar (desde el LSB)
           */
    public: void appendBits(std::uint32_t val, int len);
    };

    /**
     * @class GeneradorQRTextoPlano
     * @brief Clase para generar códigos QR con información personal en texto plano
     *
     * Facilita la generación de códigos QR que contienen información de una persona
     * y un número de cuenta en formato texto plano.
     */
    class GeneradorQRTextoPlano {
        /** @brief Objeto QR de la biblioteca */
    public: CodigoQR::QrCode qr;
          /** @brief Nombre completo de la persona */
    private: std::string nombreCompleto;
           /** @brief Número de cuenta bancaria */
    private: std::string numeroCuenta;

           /**
            * @brief Genera la cadena en formato de texto plano con los datos personales
            * @return Texto formateado con nombre y número de cuenta
            */
    private: std::string generarCadenaTextoPlano() const {
        std::string resultado = "NOMBRE: " + nombreCompleto + "\n";
        resultado += "CUENTA: " + numeroCuenta;
        return resultado;
    }

           /**
            * @brief Constructor
            * @param nombre Nombre completo de la persona
            * @param cuenta Número de cuenta bancaria
            *
            * Inicializa el generador con los datos personales y un QR vacío
            */
    public: GeneradorQRTextoPlano(const std::string& nombre, const std::string& cuenta)
        : nombreCompleto(nombre), numeroCuenta(cuenta),
        qr(CodigoQR::QrCode::encodeText("", CodigoQR::QrCode::Ecc::LOW)) {
    }

          /**
           * @brief Genera el código QR con los datos personales
           *
           * Crea el código QR con formato de texto plano y nivel de corrección bajo
           */
    public: void generarQR() {
        // Generamos la cadena de texto plano
        std::string datosTexto = generarCadenaTextoPlano();

        std::cout << "Generando código QR..." << std::endl;

        // Generamos el código QR con la biblioteca (nivel de corrección LOW = L)
        qr = CodigoQR::QrCode::encodeText(datosTexto.c_str(), CodigoQR::QrCode::Ecc::LOW);

        std::cout << "Código QR generado correctamente." << std::endl;
    }

          /**
           * @brief Imprime el código QR en la consola
           *
           * Muestra una representación visual del código QR utilizando caracteres ASCII,
           * junto con información técnica sobre el código.
           */
    public: void imprimirEnConsola() const {
        if (qr.getSize() == 0) {
            std::cout << "Error: Primero debes generar el código QR" << std::endl;
            return;
        }

        std::cout << "\n=== CÓDIGO QR GENERADO ===" << std::endl;
        std::cout << "- Nombre completo: " << nombreCompleto << std::endl;
        std::cout << "- Número de cuenta: " << numeroCuenta << std::endl;
        std::cout << "\nCódigo QR:\n" << std::endl;

        const int quietZone = 4;
        const int size = qr.getSize() + quietZone * 2;

        // Borde superior
        for (int i = 0; i < size; i++) {
            std::cout << "??";
        }
        std::cout << std::endl;

        // Imprimir QR con bordes
        for (int y = -quietZone; y < qr.getSize() + quietZone; y++) {
            std::cout << "????"; // Borde izquierdo extra
            for (int x = -quietZone; x < qr.getSize() + quietZone; x++) {
                bool module = (x >= 0 && y >= 0 && x < qr.getSize() && y < qr.getSize()) && qr.getModule(x, y);
                std::cout << (module ? "??" : "  ");
            }
            std::cout << "????" << std::endl; // Borde derecho extra
        }

        // Borde inferior
        for (int i = 0; i < size; i++) {
            std::cout << "??";
        }
        std::cout << std::endl;

        // Información técnica
        std::cout << "\nInformación técnica:" << std::endl;
        std::cout << "- Versión: " << qr.getVersion() << std::endl;
        std::cout << "- Tamaño: " << qr.getSize() << "x" << qr.getSize() << " módulos" << std::endl;
        std::cout << "- Nivel de corrección: L (permite hasta 7% de errores)" << std::endl;
        std::cout << "- Máscara aplicada: " << qr.getMask() << std::endl;
        std::cout << "- Formato: Texto plano (NOMBRE: " << nombreCompleto << ", CUENTA: " << numeroCuenta << ")" << std::endl;
        std::cout << "\nEste QR puede ser escaneado por cualquier lector QR estándar." << std::endl;
        std::cout << "Al escanearlo, el móvil mostrará los datos personales en formato texto." << std::endl;
    }

          /**
           * @brief Genera un archivo PDF con el código QR
           * @param qr Objeto QR a convertir en PDF
           * @param nombreArchivo Ruta completa del archivo PDF a generar
           *
           * Crea un archivo PDF que contiene el código QR con un tamaño adecuado
           * para ser escaneado fácilmente por dispositivos móviles.
           */
    public: void generarPDFQR(const CodigoQR::QrCode& qr, const std::string& nombreArchivo) {
        if (qr.getSize() == 0) {
            std::cout << "Error: Primero debes generar el código QR" << std::endl;
            return;
        }

        std::cout << "\nGenerando PDF..." << std::endl;

        // Crear archivo PDF
        std::ofstream pdf(nombreArchivo, std::ios::binary);
        if (!pdf.is_open()) {
            std::cout << "Error: No se pudo crear el archivo PDF." << std::endl;
            return;
        }

        // Constantes para el PDF - convertir mm a puntos (1/72 pulgadas)
        const auto MM_TO_POINTS = [](double mm) { return mm * 2.83465; };

        // Tamaño de cada módulo QR en puntos
        const double MODULO_SIZE = MM_TO_POINTS(8.0);       // 8mm por módulo para mejor escaneo
        const double MARGEN = MODULO_SIZE * 4;              // Margen mínimo recomendado (4 módulos)
        const double QR_SIZE = MODULO_SIZE * qr.getSize();  // Tamaño total del QR
        const double PAGE_WIDTH = QR_SIZE + 2 * MARGEN;
        const double PAGE_HEIGHT = PAGE_WIDTH;              // Página cuadrada para mejor presentación
        const double MARGEN_EXTRA = MARGEN * 0.5;           // Margen adicional para mejor contraste

        // Lista de objetos PDF
        std::vector<size_t> objetos;
        std::string xref;

        // Escribir cabecera PDF
        pdf << "%PDF-1.4\n%\xE2\xE3\xCF\xD3\n";

        // Catálogo (objeto 1)
        objetos.push_back(pdf.tellp());
        pdf << "1 0 obj\n<</Type /Catalog /Pages 2 0 R>>\nendobj\n";

        // Pages (objeto 2)
        objetos.push_back(pdf.tellp());
        pdf << "2 0 obj\n<</Type /Pages /Kids [3 0 R] /Count 1>>\nendobj\n";

        // Page (objeto 3)
        objetos.push_back(pdf.tellp());
        pdf << "3 0 obj\n"
            << "<</Type /Page /Parent 2 0 R "
            << "/MediaBox [0 0 " << PAGE_WIDTH << " " << PAGE_HEIGHT << "] "
            << "/Contents 4 0 R>>\nendobj\n";

        // Contenido (objeto 4) - Aquí va el QR
        std::stringstream contenido;
        contenido << "q\n"; // Guardar estado gráfico

        // Establecer color blanco y dibujar fondo
        contenido << "1 1 1 rg\n"; // RGB blanco
        contenido << "0 0 " << PAGE_WIDTH << " " << PAGE_HEIGHT << " re f\n";

        // Establecer color negro sólido para el QR
        contenido << "0 0 0 rg\n"; // RGB negro

        // Definir área de recorte y dibujo con margen extra para mejor contraste
        contenido << MARGEN_EXTRA << " " << MARGEN_EXTRA << " "
            << (PAGE_WIDTH - 2 * MARGEN_EXTRA) << " "
            << (PAGE_HEIGHT - 2 * MARGEN_EXTRA) << " re W n\n";

        // Dibujar módulos del QR con mayor precisión
        for (int y = 0; y < qr.getSize(); y++) {
            for (int x = 0; x < qr.getSize(); x++) {
                if (qr.getModule(x, y)) {
                    // Calcular posición con más precisión
                    double posX = MARGEN + x * MODULO_SIZE;
                    double posY = MARGEN + (qr.getSize() - 1 - y) * MODULO_SIZE;
                    // Usar un path más eficiente para cada módulo con tamaño exacto
                    contenido << std::fixed << std::setprecision(3)
                        << posX << " " << posY << " "
                        << MODULO_SIZE << " " << MODULO_SIZE << " re\n";
                }
            }
        }
        // Rellenar todos los módulos de una vez con máxima densidad
        contenido << "f\n";
        contenido << "Q\n"; // Restaurar estado gráfico

        std::string contenidoStr = contenido.str();
        objetos.push_back(pdf.tellp());
        pdf << "4 0 obj\n"
            << "<</Length " << contenidoStr.length() << ">>\nstream\n"
            << contenidoStr
            << "endstream\nendobj\n";

        // xref
        size_t xrefPos = pdf.tellp();
        pdf << "xref\n0 " << (objetos.size() + 1) << "\n";
        pdf << "0000000000 65535 f\r\n"; // Usar CRLF para mejor compatibilidad
        for (size_t offset : objetos) {
            pdf << std::setfill('0') << std::setw(10) << offset << " 00000 n\r\n";
        }

        // Añadir línea en blanco después del xref
        pdf << "\n";

        // trailer
        pdf << "trailer\n"
            << "<</Size " << (objetos.size() + 1) << " /Root 1 0 R>>\n"
            << "startxref\n"
            << xrefPos << "\n"
            << "%%EOF\n";

        pdf.close();
        std::cout << "PDF generado exitosamente: " << nombreArchivo << std::endl;
    }
    };

} // namespace CodigoQR
#endif // CODIGOQR_H