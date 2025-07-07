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
 * @brief Espacio de nombres que contiene las clases y funciones para generaci�n de c�digos QR
 *
 * Este namespace proporciona una implementaci�n completa para generar, manipular
 * y visualizar c�digos QR seg�n el est�ndar ISO/IEC 18004.
 */
namespace CodigoQR {

    /**
     * @class QrSegment
     * @brief Representa un segmento de datos en un c�digo QR
     *
     * Un c�digo QR puede contener varios segmentos de diferentes tipos (num�rico,
     * alfanum�rico, bytes, etc.) Esta clase representa uno de estos segmentos.
     */
    class QrSegment final {

        /**
         * @class Mode
         * @brief Define los modos de codificaci�n para segmentos QR
         *
         * Cada segmento QR puede usar un modo de codificaci�n diferente seg�n
         * el tipo de datos que contiene.
         */
    public: class Mode final {

        /** @brief Modo para datos num�ricos (0-9) */
    public: static const Mode NUMERIC;
          /** @brief Modo para datos alfanum�ricos (0-9, A-Z, y algunos s�mbolos) */
    public: static const Mode ALPHANUMERIC;
          /** @brief Modo para datos binarios de 8 bits */
    public: static const Mode BYTE;
          /** @brief Modo para caracteres Kanji (japoneses) */
    public: static const Mode KANJI;
          /** @brief Modo para informaci�n de interpretaci�n extendida de canal */
    public: static const Mode ECI;

          /** @brief Bits que identifican este modo en el c�digo QR */
    private: int modeBits;
           /** @brief N�mero de bits para contar caracteres seg�n versi�n QR */
    private: int numBitsCharCount[3];
           /**
            * @brief Constructor para crear un modo
            * @param mode Bits del modo
            * @param cc0 N�mero de bits para versiones 1-9
            * @param cc1 N�mero de bits para versiones 10-26
            * @param cc2 N�mero de bits para versiones 27-40
            */
    private: Mode(int mode, int cc0, int cc1, int cc2);
           /**
            * @brief Obtiene los bits que identifican este modo
            * @return El valor de los bits del modo
            */
    public: int getModeBits() const;
          /**
           * @brief Obtiene el n�mero de bits para contar caracteres
           * @param ver Versi�n del c�digo QR
           * @return N�mero de bits para la versi�n especificada
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
           * @brief Crea un segmento a partir de d�gitos num�ricos
           * @param digits Cadena con d�gitos num�ricos
           * @return Segmento QR en modo NUMERIC
           */
    public: static QrSegment makeNumeric(const char* digits);
          /**
           * @brief Crea un segmento a partir de texto alfanum�rico
           * @param text Cadena con texto alfanum�rico
           * @return Segmento QR en modo ALPHANUMERIC
           */
    public: static QrSegment makeAlphanumeric(const char* text);
          /**
           * @brief Crea segmentos autom�ticamente a partir de texto
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
           * @brief Verifica si un texto es puramente num�rico
           * @param text Texto a verificar
           * @return true si todos los caracteres son d�gitos num�ricos
           */
    public: static bool isNumeric(const char* text);
          /**
           * @brief Verifica si un texto es alfanum�rico seg�n el est�ndar QR
           * @param text Texto a verificar
           * @return true si todos los caracteres son alfanum�ricos v�lidos
           */
    public: static bool isAlphanumeric(const char* text);
          /** @brief Modo de codificaci�n para este segmento */
    private: const Mode* mode;
           /** @brief N�mero de caracteres en este segmento */
    private: int numChars;
           /** @brief Datos del segmento como bits */
    private: std::vector<bool> data;
           /**
            * @brief Constructor para crear un segmento
            * @param md Modo de codificaci�n
            * @param numCh N�mero de caracteres
            * @param dt Vector de bits con los datos
            */
    public: QrSegment(const Mode& md, int numCh, const std::vector<bool>& dt);
          /**
           * @brief Constructor con movimiento para datos
           * @param md Modo de codificaci�n
           * @param numCh N�mero de caracteres
           * @param dt Vector de bits con los datos (movido)
           */
    public: QrSegment(const Mode& md, int numCh, std::vector<bool>&& dt);
          /**
           * @brief Obtiene el modo de este segmento
           * @return Referencia al modo de codificaci�n
           */
    public: const Mode& getMode() const;
          /**
           * @brief Obtiene el n�mero de caracteres
           * @return Cantidad de caracteres en este segmento
           */
    public: int getNumChars() const;
          /**
           * @brief Obtiene los datos codificados como bits
           * @return Vector de bits con los datos
           */
    public: const std::vector<bool>& getData() const;
          /**
           * @brief Calcula el n�mero total de bits necesarios para los segmentos
           * @param segs Vector de segmentos
           * @param version Versi�n del c�digo QR
           * @return N�mero total de bits
           */
    public: static int getTotalBits(const std::vector<QrSegment>& segs, int version);
          /** @brief Conjunto de caracteres v�lidos para el modo alfanum�rico */
    private: static const char* ALPHANUMERIC_CHARSET;

    };

    /**
     * @class QrCode
     * @brief Representa un c�digo QR completo con todos sus elementos
     *
     * Esta clase implementa la funcionalidad para crear y manipular c�digos QR
     * con diferentes niveles de correcci�n de errores y versiones.
     */
    class QrCode final {

        /**
         * @brief Niveles de correcci�n de errores para c�digos QR
         */
    public: enum class Ecc {
        LOW = 0,      /**< El c�digo QR puede tolerar aproximadamente 7% de palabras c�digo err�neas */
        MEDIUM,       /**< El c�digo QR puede tolerar aproximadamente 15% de palabras c�digo err�neas */
        QUARTILE,     /**< El c�digo QR puede tolerar aproximadamente 25% de palabras c�digo err�neas */
        HIGH,         /**< El c�digo QR puede tolerar aproximadamente 30% de palabras c�digo err�neas */
    };
          /**
           * @brief Obtiene los bits de formato basados en el nivel de correcci�n
           * @param ecl Nivel de correcci�n de errores
           * @return Bits de formato
           */
    private: static int getFormatBits(Ecc ecl);
           /**
            * @brief Codifica texto en un c�digo QR
            * @param text Texto a codificar
            * @param ecl Nivel de correcci�n de errores
            * @return Objeto QrCode con el c�digo generado
            */
    public: static QrCode encodeText(const char* text, Ecc ecl);
          /**
           * @brief Codifica datos binarios en un c�digo QR
           * @param data Datos binarios a codificar
           * @param ecl Nivel de correcci�n de errores
           * @return Objeto QrCode con el c�digo generado
           */
    public: static QrCode encodeBinary(const std::vector<std::uint8_t>& data, Ecc ecl);
          /**
           * @brief Codifica segmentos en un c�digo QR
           * @param segs Segmentos a codificar
           * @param ecl Nivel de correcci�n de errores
           * @param minVersion Versi�n m�nima (1-40)
           * @param maxVersion Versi�n m�xima (1-40)
           * @param mask Patr�n de m�scara (-1 para autom�tico)
           * @param boostEcl Permitir mejora autom�tica del nivel de correcci�n
           * @return Objeto QrCode con el c�digo generado
           */
    public: static QrCode encodeSegments(const std::vector<QrSegment>& segs, Ecc ecl,
        int minVersion = 1, int maxVersion = 40, int mask = -1, bool boostEcl = true);
          /** @brief Versi�n del c�digo QR (1-40) */
    private: int version;
           /** @brief Tama�o del c�digo QR en m�dulos */
    private: int size;
           /** @brief Nivel de correcci�n de errores */
    private: Ecc errorCorrectionLevel;
           /** @brief Patr�n de m�scara utilizado */
    private: int mask;
           /** @brief Matriz de m�dulos (true=negro, false=blanco) */
    private: std::vector<std::vector<bool> > modules;
           /** @brief Matriz de funciones (patrones fijos) */
    private: std::vector<std::vector<bool> > isFunction;
           /**
            * @brief Constructor para crear un c�digo QR
            * @param ver Versi�n (1-40)
            * @param ecl Nivel de correcci�n de errores
            * @param dataCodewords Datos codificados
            * @param msk Patr�n de m�scara
            */
    public: QrCode(int ver, Ecc ecl, const std::vector<std::uint8_t>& dataCodewords, int msk);
          /**
           * @brief Obtiene la versi�n del c�digo QR
           * @return Versi�n (1-40)
           */
    public: int getVersion() const;
          /**
           * @brief Obtiene el tama�o del c�digo QR en m�dulos
           * @return Tama�o (ancho/alto en m�dulos)
           */
    public: int getSize() const;
          /**
           * @brief Obtiene el nivel de correcci�n de errores
           * @return Nivel de correcci�n
           */
    public: Ecc getErrorCorrectionLevel() const;
          /**
           * @brief Obtiene el patr�n de m�scara utilizado
           * @return �ndice del patr�n de m�scara (0-7)
           */
    public: int getMask() const;
          /**
           * @brief Verifica si un m�dulo est� activo (negro) en la posici�n dada
           * @param x Coordenada horizontal
           * @param y Coordenada vertical
           * @return true si el m�dulo est� activo, false si es blanco
           */
    public: bool getModule(int x, int y) const;
          /**
           * @brief Dibuja los patrones de funci�n (fijos) en el c�digo QR
           */
    private: void drawFunctionPatterns();
           /**
            * @brief Dibuja los bits de formato con el patr�n de m�scara dado
            * @param msk �ndice del patr�n de m�scara
            */
    private: void drawFormatBits(int msk);
           /**
            * @brief Dibuja la informaci�n de versi�n (solo para versiones 7+)
            */
    private: void drawVersion();
           /**
            * @brief Dibuja un patr�n de b�squeda en la posici�n especificada
            * @param x Coordenada horizontal
            * @param y Coordenada vertical
            */
    private: void drawFinderPattern(int x, int y);
           /**
            * @brief Dibuja un patr�n de alineaci�n en la posici�n especificada
            * @param x Coordenada horizontal
            * @param y Coordenada vertical
            */
    private: void drawAlignmentPattern(int x, int y);
           /**
            * @brief Establece un m�dulo de funci�n en la posici�n dada
            * @param x Coordenada horizontal
            * @param y Coordenada vertical
            * @param isDark Si el m�dulo debe ser negro (true) o blanco (false)
            */
    private: void setFunctionModule(int x, int y, bool isDark);
           /**
            * @brief Verifica el estado de un m�dulo
            * @param x Coordenada horizontal
            * @param y Coordenada vertical
            * @return true si el m�dulo est� activo, false si es blanco
            */
    private: bool module(int x, int y) const;
           /**
            * @brief A�ade correcci�n de errores y entrelazado a los datos
            * @param data Datos originales
            * @return Vector con datos y correcci�n de errores entrelazados
            */
    private: std::vector<std::uint8_t> addEccAndInterleave(const std::vector<std::uint8_t>& data) const;
           /**
            * @brief Dibuja los datos codificados en el c�digo QR
            * @param data Datos codificados con correcci�n de errores
            */
    private: void drawCodewords(const std::vector<std::uint8_t>& data);
           /**
            * @brief Aplica un patr�n de m�scara a los datos
            * @param msk �ndice del patr�n de m�scara (0-7)
            */
    private: void applyMask(int msk);
           /**
            * @brief Calcula la puntuaci�n de penalizaci�n seg�n las reglas del est�ndar
            * @return Puntuaci�n de penalizaci�n
            */
    private: long getPenaltyScore() const;
           /**
            * @brief Obtiene las posiciones de los patrones de alineaci�n
            * @return Vector con posiciones de patrones de alineaci�n
            */
    private: std::vector<int> getAlignmentPatternPositions() const;
           /**
            * @brief Calcula el n�mero total de m�dulos de datos sin formato
            * @param ver Versi�n del c�digo QR
            * @return N�mero de m�dulos de datos
            */
    private: static int getNumRawDataModules(int ver);
           /**
            * @brief Calcula el n�mero de palabras c�digo de datos
            * @param ver Versi�n del c�digo QR
            * @param ecl Nivel de correcci�n de errores
            * @return N�mero de palabras c�digo
            */
    private: static int getNumDataCodewords(int ver, Ecc ecl);
           /**
            * @brief Calcula el divisor Reed-Solomon para correcci�n de errores
            * @param degree Grado del polinomio
            * @return Vector con coeficientes del divisor
            */
    private: static std::vector<std::uint8_t> reedSolomonComputeDivisor(int degree);
           /**
            * @brief Calcula el resto Reed-Solomon para correcci�n de errores
            * @param data Datos originales
            * @param divisor Divisor Reed-Solomon
            * @return Vector con el resto de la divisi�n
            */
    private: static std::vector<std::uint8_t> reedSolomonComputeRemainder(const std::vector<std::uint8_t>& data, const std::vector<std::uint8_t>& divisor);
           /**
            * @brief Multiplica dos valores en el campo finito GF(256)
            * @param x Primer valor
            * @param y Segundo valor
            * @return Resultado de la multiplicaci�n
            */
    private: static std::uint8_t reedSolomonMultiply(std::uint8_t x, std::uint8_t y);
           /**
            * @brief Cuenta patrones para c�lculo de penalizaci�n
            * @param runHistory Historial de longitudes de ejecuci�n
            * @return Puntuaci�n de penalizaci�n
            */
    private: int finderPenaltyCountPatterns(const std::array<int, 7>& runHistory) const;
           /**
            * @brief Termina una ejecuci�n y cuenta para c�lculo de penalizaci�n
            * @param currentRunColor Color actual
            * @param currentRunLength Longitud de la ejecuci�n actual
            * @param runHistory Historial de longitudes de ejecuci�n
            * @return Puntuaci�n de penalizaci�n
            */
    private: int finderPenaltyTerminateAndCount(bool currentRunColor, int currentRunLength, std::array<int, 7>& runHistory) const;
           /**
            * @brief A�ade una longitud de ejecuci�n al historial
            * @param currentRunLength Longitud de la ejecuci�n
            * @param runHistory Historial de longitudes de ejecuci�n
            */
    private: void finderPenaltyAddHistory(int currentRunLength, std::array<int, 7>& runHistory) const;
           /**
            * @brief Obtiene un bit espec�fico de un valor
            * @param x Valor
            * @param i �ndice del bit (0 = LSB)
            * @return true si el bit est� activo, false si no
            */
    private: static bool getBit(long x, int i);
           /** @brief Versi�n m�nima de c�digo QR (1) */
    public: static constexpr int MIN_VERSION = 1;
          /** @brief Versi�n m�xima de c�digo QR (40) */
    public: static constexpr int MAX_VERSION = 40;
          /** @brief Constante de penalizaci�n N1 */
    private: static const int PENALTY_N1;
           /** @brief Constante de penalizaci�n N2 */
    private: static const int PENALTY_N2;
           /** @brief Constante de penalizaci�n N3 */
    private: static const int PENALTY_N3;
           /** @brief Constante de penalizaci�n N4 */
    private: static const int PENALTY_N4;
           /** @brief Tabla de palabras c�digo de correcci�n de errores por bloque */
    private: static const std::int8_t ECC_CODEWORDS_PER_BLOCK[4][41];
           /** @brief Tabla de n�mero de bloques de correcci�n de errores */
    private: static const std::int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41];
    };

    /**
     * @class data_too_long
     * @brief Excepci�n lanzada cuando los datos son demasiado grandes para el c�digo QR
     *
     * Esta excepci�n se lanza cuando se intenta codificar datos que exceden
     * la capacidad de la versi�n m�xima del c�digo QR.
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
     * Extiende std::vector<bool> con m�todos para a�adir bits de manera eficiente
     */
    class BitBuffer final : public std::vector<bool> {
        /**
         * @brief Constructor por defecto
         */
    public: BitBuffer();
          /**
           * @brief A�ade bits al buffer
           * @param val Valor a codificar
           * @param len N�mero de bits a usar (desde el LSB)
           */
    public: void appendBits(std::uint32_t val, int len);
    };

    /**
     * @class GeneradorQRTextoPlano
     * @brief Clase para generar c�digos QR con informaci�n personal en texto plano
     *
     * Facilita la generaci�n de c�digos QR que contienen informaci�n de una persona
     * y un n�mero de cuenta en formato texto plano.
     */
    class GeneradorQRTextoPlano {
        /** @brief Objeto QR de la biblioteca */
    public: CodigoQR::QrCode qr;
          /** @brief Nombre completo de la persona */
    private: std::string nombreCompleto;
           /** @brief N�mero de cuenta bancaria */
    private: std::string numeroCuenta;

           /**
            * @brief Genera la cadena en formato de texto plano con los datos personales
            * @return Texto formateado con nombre y n�mero de cuenta
            */
    private: std::string generarCadenaTextoPlano() const {
        std::string resultado = "NOMBRE: " + nombreCompleto + "\n";
        resultado += "CUENTA: " + numeroCuenta;
        return resultado;
    }

           /**
            * @brief Constructor
            * @param nombre Nombre completo de la persona
            * @param cuenta N�mero de cuenta bancaria
            *
            * Inicializa el generador con los datos personales y un QR vac�o
            */
    public: GeneradorQRTextoPlano(const std::string& nombre, const std::string& cuenta)
        : nombreCompleto(nombre), numeroCuenta(cuenta),
        qr(CodigoQR::QrCode::encodeText("", CodigoQR::QrCode::Ecc::LOW)) {
    }

          /**
           * @brief Genera el c�digo QR con los datos personales
           *
           * Crea el c�digo QR con formato de texto plano y nivel de correcci�n bajo
           */
    public: void generarQR() {
        // Generamos la cadena de texto plano
        std::string datosTexto = generarCadenaTextoPlano();

        std::cout << "Generando c�digo QR..." << std::endl;

        // Generamos el c�digo QR con la biblioteca (nivel de correcci�n LOW = L)
        qr = CodigoQR::QrCode::encodeText(datosTexto.c_str(), CodigoQR::QrCode::Ecc::LOW);

        std::cout << "C�digo QR generado correctamente." << std::endl;
    }

          /**
           * @brief Imprime el c�digo QR en la consola
           *
           * Muestra una representaci�n visual del c�digo QR utilizando caracteres ASCII,
           * junto con informaci�n t�cnica sobre el c�digo.
           */
    public: void imprimirEnConsola() const {
        if (qr.getSize() == 0) {
            std::cout << "Error: Primero debes generar el c�digo QR" << std::endl;
            return;
        }

        std::cout << "\n=== C�DIGO QR GENERADO ===" << std::endl;
        std::cout << "- Nombre completo: " << nombreCompleto << std::endl;
        std::cout << "- N�mero de cuenta: " << numeroCuenta << std::endl;
        std::cout << "\nC�digo QR:\n" << std::endl;

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

        // Informaci�n t�cnica
        std::cout << "\nInformaci�n t�cnica:" << std::endl;
        std::cout << "- Versi�n: " << qr.getVersion() << std::endl;
        std::cout << "- Tama�o: " << qr.getSize() << "x" << qr.getSize() << " m�dulos" << std::endl;
        std::cout << "- Nivel de correcci�n: L (permite hasta 7% de errores)" << std::endl;
        std::cout << "- M�scara aplicada: " << qr.getMask() << std::endl;
        std::cout << "- Formato: Texto plano (NOMBRE: " << nombreCompleto << ", CUENTA: " << numeroCuenta << ")" << std::endl;
        std::cout << "\nEste QR puede ser escaneado por cualquier lector QR est�ndar." << std::endl;
        std::cout << "Al escanearlo, el m�vil mostrar� los datos personales en formato texto." << std::endl;
    }

          /**
           * @brief Genera un archivo PDF con el c�digo QR
           * @param qr Objeto QR a convertir en PDF
           * @param nombreArchivo Ruta completa del archivo PDF a generar
           *
           * Crea un archivo PDF que contiene el c�digo QR con un tama�o adecuado
           * para ser escaneado f�cilmente por dispositivos m�viles.
           */
    public: void generarPDFQR(const CodigoQR::QrCode& qr, const std::string& nombreArchivo) {
        if (qr.getSize() == 0) {
            std::cout << "Error: Primero debes generar el c�digo QR" << std::endl;
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

        // Tama�o de cada m�dulo QR en puntos
        const double MODULO_SIZE = MM_TO_POINTS(8.0);       // 8mm por m�dulo para mejor escaneo
        const double MARGEN = MODULO_SIZE * 4;              // Margen m�nimo recomendado (4 m�dulos)
        const double QR_SIZE = MODULO_SIZE * qr.getSize();  // Tama�o total del QR
        const double PAGE_WIDTH = QR_SIZE + 2 * MARGEN;
        const double PAGE_HEIGHT = PAGE_WIDTH;              // P�gina cuadrada para mejor presentaci�n
        const double MARGEN_EXTRA = MARGEN * 0.5;           // Margen adicional para mejor contraste

        // Lista de objetos PDF
        std::vector<size_t> objetos;
        std::string xref;

        // Escribir cabecera PDF
        pdf << "%PDF-1.4\n%\xE2\xE3\xCF\xD3\n";

        // Cat�logo (objeto 1)
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

        // Contenido (objeto 4) - Aqu� va el QR
        std::stringstream contenido;
        contenido << "q\n"; // Guardar estado gr�fico

        // Establecer color blanco y dibujar fondo
        contenido << "1 1 1 rg\n"; // RGB blanco
        contenido << "0 0 " << PAGE_WIDTH << " " << PAGE_HEIGHT << " re f\n";

        // Establecer color negro s�lido para el QR
        contenido << "0 0 0 rg\n"; // RGB negro

        // Definir �rea de recorte y dibujo con margen extra para mejor contraste
        contenido << MARGEN_EXTRA << " " << MARGEN_EXTRA << " "
            << (PAGE_WIDTH - 2 * MARGEN_EXTRA) << " "
            << (PAGE_HEIGHT - 2 * MARGEN_EXTRA) << " re W n\n";

        // Dibujar m�dulos del QR con mayor precisi�n
        for (int y = 0; y < qr.getSize(); y++) {
            for (int x = 0; x < qr.getSize(); x++) {
                if (qr.getModule(x, y)) {
                    // Calcular posici�n con m�s precisi�n
                    double posX = MARGEN + x * MODULO_SIZE;
                    double posY = MARGEN + (qr.getSize() - 1 - y) * MODULO_SIZE;
                    // Usar un path m�s eficiente para cada m�dulo con tama�o exacto
                    contenido << std::fixed << std::setprecision(3)
                        << posX << " " << posY << " "
                        << MODULO_SIZE << " " << MODULO_SIZE << " re\n";
                }
            }
        }
        // Rellenar todos los m�dulos de una vez con m�xima densidad
        contenido << "f\n";
        contenido << "Q\n"; // Restaurar estado gr�fico

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

        // A�adir l�nea en blanco despu�s del xref
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