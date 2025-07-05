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

namespace CodigoQR {

	class QrSegment final {

	public: class Mode final {

	public: static const Mode NUMERIC;
	public: static const Mode ALPHANUMERIC;
	public: static const Mode BYTE;
	public: static const Mode KANJI;
	public: static const Mode ECI;

	private: int modeBits;
	private: int numBitsCharCount[3];
	private: Mode(int mode, int cc0, int cc1, int cc2);
	public: int getModeBits() const;
	public: int numCharCountBits(int ver) const;

	};
	public: static QrSegment makeBytes(const std::vector<std::uint8_t>& data);
	public: static QrSegment makeNumeric(const char* digits);
	public: static QrSegment makeAlphanumeric(const char* text);
	public: static std::vector<QrSegment> makeSegments(const char* text);
	public: static QrSegment makeEci(long assignVal);
	public: static bool isNumeric(const char* text);
	public: static bool isAlphanumeric(const char* text);
	private: const Mode* mode;
	private: int numChars;
	private: std::vector<bool> data;
	public: QrSegment(const Mode& md, int numCh, const std::vector<bool>& dt);
	public: QrSegment(const Mode& md, int numCh, std::vector<bool>&& dt);
	public: const Mode& getMode() const;
	public: int getNumChars() const;
	public: const std::vector<bool>& getData() const;
	public: static int getTotalBits(const std::vector<QrSegment>& segs, int version);
	private: static const char* ALPHANUMERIC_CHARSET;

	};
	class QrCode final {

	public: enum class Ecc {
		LOW = 0,  // The QR Code can tolerate about  7% erroneous codewords
		MEDIUM,  // The QR Code can tolerate about 15% erroneous codewords
		QUARTILE,  // The QR Code can tolerate about 25% erroneous codewords
		HIGH,  // The QR Code can tolerate about 30% erroneous codewords
	};
	private: static int getFormatBits(Ecc ecl);
	public: static QrCode encodeText(const char* text, Ecc ecl);
	public: static QrCode encodeBinary(const std::vector<std::uint8_t>& data, Ecc ecl);
	public: static QrCode encodeSegments(const std::vector<QrSegment>& segs, Ecc ecl,
		int minVersion = 1, int maxVersion = 40, int mask = -1, bool boostEcl = true);  // All optional parameters
	private: int version;
	private: int size;
	private: Ecc errorCorrectionLevel;
	private: int mask;
	private: std::vector<std::vector<bool> > modules;
	private: std::vector<std::vector<bool> > isFunction;
	public: QrCode(int ver, Ecc ecl, const std::vector<std::uint8_t>& dataCodewords, int msk);
	public: int getVersion() const;
	public: int getSize() const;
	public: Ecc getErrorCorrectionLevel() const;
	public: int getMask() const;
	public: bool getModule(int x, int y) const;
	private: void drawFunctionPatterns();
	private: void drawFormatBits(int msk);
	private: void drawVersion();
	private: void drawFinderPattern(int x, int y);
	private: void drawAlignmentPattern(int x, int y);
	private: void setFunctionModule(int x, int y, bool isDark);
	private: bool module(int x, int y) const;
	private: std::vector<std::uint8_t> addEccAndInterleave(const std::vector<std::uint8_t>& data) const;
	private: void drawCodewords(const std::vector<std::uint8_t>& data);
	private: void applyMask(int msk);
	private: long getPenaltyScore() const;
	private: std::vector<int> getAlignmentPatternPositions() const;
	private: static int getNumRawDataModules(int ver);
	private: static int getNumDataCodewords(int ver, Ecc ecl);
	private: static std::vector<std::uint8_t> reedSolomonComputeDivisor(int degree);
	private: static std::vector<std::uint8_t> reedSolomonComputeRemainder(const std::vector<std::uint8_t>& data, const std::vector<std::uint8_t>& divisor);
	private: static std::uint8_t reedSolomonMultiply(std::uint8_t x, std::uint8_t y);
	private: int finderPenaltyCountPatterns(const std::array<int, 7>& runHistory) const;
	private: int finderPenaltyTerminateAndCount(bool currentRunColor, int currentRunLength, std::array<int, 7>& runHistory) const;
	private: void finderPenaltyAddHistory(int currentRunLength, std::array<int, 7>& runHistory) const;
	private: static bool getBit(long x, int i);
	public: static constexpr int MIN_VERSION = 1;
	public: static constexpr int MAX_VERSION = 40;
	private: static const int PENALTY_N1;
	private: static const int PENALTY_N2;
	private: static const int PENALTY_N3;
	private: static const int PENALTY_N4;
	private: static const std::int8_t ECC_CODEWORDS_PER_BLOCK[4][41];
	private: static const std::int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41];
	};

	class data_too_long : public std::length_error {
	public: explicit data_too_long(const std::string& msg);
	};

	class BitBuffer final : public std::vector<bool> {
	public: BitBuffer();
	public: void appendBits(std::uint32_t val, int len);
	};

	class GeneradorQRTextoPlano {
	public:CodigoQR::QrCode qr; // Objeto QR de la biblioteca
	private:std::string nombreCompleto;
	private:std::string numeroCuenta;

		   // Genera la cadena en formato de texto plano con los datos personales
	private: std::string generarCadenaTextoPlano() const {
		std::string resultado = "NOMBRE: " + nombreCompleto + "\n";
		resultado += "CUENTA: " + numeroCuenta;
		return resultado;
	}
	public: GeneradorQRTextoPlano(const std::string& nombre, const std::string& cuenta)
		: nombreCompleto(nombre), numeroCuenta(cuenta),
		qr(CodigoQR::QrCode::encodeText("", CodigoQR::QrCode::Ecc::LOW)) {
	}
	public: void generarQR() {
		// Generamos la cadena de texto plano
		std::string datosTexto = generarCadenaTextoPlano();

		std::cout << "Generando código QR..." << std::endl;

		// Generamos el código QR con la biblioteca (nivel de corrección LOW = L)
		qr = CodigoQR::QrCode::encodeText(datosTexto.c_str(), CodigoQR::QrCode::Ecc::LOW);

		std::cout << "Código QR generado correctamente." << std::endl;
	}
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

}
#endif // CODIGOQR_H