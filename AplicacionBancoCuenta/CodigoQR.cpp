/**
 * @file CodigoQR.cpp
 * @brief Implementación de las clases QrSegment y QrCode para generación de códigos QR
 *
 * Este archivo contiene la implementación completa de la biblioteca para
 * generar códigos QR. Incluye algoritmos para codificación de datos,
 * corrección de errores Reed-Solomon y generación de la matriz del código QR.
 */
#include <algorithm>
#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <utility>
#include "CodigoQR.h"

using std::int8_t;
using std::uint8_t;
using std::size_t;
using std::vector;

namespace CodigoQR {

	/*---- Class QrSegment ----*/

	/**
	 * @brief Constructor de Mode para definir modos de codificación QR
	 *
	 * @param mode Bits del modo de codificación
	 * @param cc0 Bits del contador de caracteres para versiones 1-9
	 * @param cc1 Bits del contador de caracteres para versiones 10-26
	 * @param cc2 Bits del contador de caracteres para versiones 27-40
	 */
	QrSegment::Mode::Mode(int mode, int cc0, int cc1, int cc2) :
		modeBits(mode) {
		numBitsCharCount[0] = cc0;
		numBitsCharCount[1] = cc1;
		numBitsCharCount[2] = cc2;
	}

	/**
	 * @brief Obtiene los bits que identifican este modo en un código QR
	 *
	 * @return Valor entero que representa los bits del modo
	 */
	int QrSegment::Mode::getModeBits() const {
		return modeBits;
	}

	/**
	 * @brief Obtiene la cantidad de bits necesarios para el contador de caracteres
	 *
	 * @param ver Versión del código QR (1-40)
	 * @return Número de bits para el contador de caracteres
	 */
	int QrSegment::Mode::numCharCountBits(int ver) const {
		return numBitsCharCount[(ver + 7) / 17];
	}

	// Constantes para los modos de codificación
	/**
	 * @brief Modo numérico, solo permite dígitos 0-9
	 */
	const QrSegment::Mode QrSegment::Mode::NUMERIC(0x1, 10, 12, 14);

	/**
	 * @brief Modo alfanumérico, permite dígitos, letras mayúsculas y algunos símbolos
	 */
	const QrSegment::Mode QrSegment::Mode::ALPHANUMERIC(0x2, 9, 11, 13);

	/**
	 * @brief Modo byte, permite cualquier dato en bytes
	 */
	const QrSegment::Mode QrSegment::Mode::BYTE(0x4, 8, 16, 16);

	/**
	 * @brief Modo Kanji, para caracteres japoneses
	 */
	const QrSegment::Mode QrSegment::Mode::KANJI(0x8, 8, 10, 12);

	/**
	 * @brief Modo ECI (Extended Channel Interpretation)
	 */
	const QrSegment::Mode QrSegment::Mode::ECI(0x7, 0, 0, 0);

	/**
	 * @brief Crea un segmento QR a partir de bytes de datos
	 *
	 * @param data Vector de bytes a codificar
	 * @return Un segmento QR en modo BYTE
	 * @throws std::length_error Si los datos son demasiado grandes
	 */
	QrSegment QrSegment::makeBytes(const vector<uint8_t>& data) {
		if (data.size() > static_cast<unsigned int>(INT_MAX))
			throw std::length_error("Data too long");
		BitBuffer bb;
		for (uint8_t b : data)
			bb.appendBits(b, 8);
		return QrSegment(Mode::BYTE, static_cast<int>(data.size()), std::move(bb));
	}

	/**
	 * @brief Crea un segmento QR para datos numéricos
	 *
	 * @param digits Cadena de caracteres con dígitos numéricos
	 * @return Un segmento QR en modo NUMERIC
	 * @throws std::domain_error Si la cadena contiene caracteres no numéricos
	 */
	QrSegment QrSegment::makeNumeric(const char* digits) {
		BitBuffer bb;
		int accumData = 0;
		int accumCount = 0;
		int charCount = 0;
		for (; *digits != '\0'; digits++, charCount++) {
			char c = *digits;
			if (c < '0' || c > '9')
				throw std::domain_error("String contains non-numeric characters");
			accumData = accumData * 10 + (c - '0');
			accumCount++;
			if (accumCount == 3) {
				bb.appendBits(static_cast<uint32_t>(accumData), 10);
				accumData = 0;
				accumCount = 0;
			}
		}
		if (accumCount > 0)  // 1 or 2 digits remaining
			bb.appendBits(static_cast<uint32_t>(accumData), accumCount * 3 + 1);
		return QrSegment(Mode::NUMERIC, charCount, std::move(bb));
	}

	/**
	 * @brief Crea un segmento QR para datos alfanuméricos
	 *
	 * @param text Cadena de caracteres con datos alfanuméricos
	 * @return Un segmento QR en modo ALPHANUMERIC
	 * @throws std::domain_error Si la cadena contiene caracteres no codificables
	 */
	QrSegment QrSegment::makeAlphanumeric(const char* text) {
		BitBuffer bb;
		int accumData = 0;
		int accumCount = 0;
		int charCount = 0;
		for (; *text != '\0'; text++, charCount++) {
			const char* temp = std::strchr(ALPHANUMERIC_CHARSET, *text);
			if (temp == nullptr)
				throw std::domain_error("String contains unencodable characters in alphanumeric mode");
			accumData = accumData * 45 + static_cast<int>(temp - ALPHANUMERIC_CHARSET);
			accumCount++;
			if (accumCount == 2) {
				bb.appendBits(static_cast<uint32_t>(accumData), 11);
				accumData = 0;
				accumCount = 0;
			}
		}
		if (accumCount > 0)  // 1 character remaining
			bb.appendBits(static_cast<uint32_t>(accumData), 6);
		return QrSegment(Mode::ALPHANUMERIC, charCount, std::move(bb));
	}

	/**
	 * @brief Crea segmentos QR automáticamente seleccionando el modo más eficiente
	 *
	 * @param text Cadena de caracteres a codificar
	 * @return Vector de segmentos QR con la codificación más eficiente
	 */
	vector<QrSegment> QrSegment::makeSegments(const char* text) {
		// Select the most efficient segment encoding automatically
		vector<QrSegment> result;
		if (*text == '\0');  // Leave result empty
		else if (isNumeric(text))
			result.push_back(makeNumeric(text));
		else if (isAlphanumeric(text))
			result.push_back(makeAlphanumeric(text));
		else {
			vector<uint8_t> bytes;
			for (; *text != '\0'; text++)
				bytes.push_back(static_cast<uint8_t>(*text));
			result.push_back(makeBytes(bytes));
		}
		return result;
	}

	/**
	 * @brief Crea un segmento ECI con el valor de asignación especificado
	 *
	 * @param assignVal Valor de asignación ECI
	 * @return Un segmento QR de tipo ECI
	 * @throws std::domain_error Si el valor está fuera del rango permitido
	 */
	QrSegment QrSegment::makeEci(long assignVal) {
		BitBuffer bb;
		if (assignVal < 0)
			throw std::domain_error("ECI assignment value out of range");
		else if (assignVal < (1 << 7))
			bb.appendBits(static_cast<uint32_t>(assignVal), 8);
		else if (assignVal < (1 << 14)) {
			bb.appendBits(2, 2);
			bb.appendBits(static_cast<uint32_t>(assignVal), 14);
		}
		else if (assignVal < 1000000L) {
			bb.appendBits(6, 3);
			bb.appendBits(static_cast<uint32_t>(assignVal), 21);
		}
		else
			throw std::domain_error("ECI assignment value out of range");
		return QrSegment(Mode::ECI, 0, std::move(bb));
	}

	/**
	 * @brief Constructor de QrSegment con vector de bool
	 *
	 * @param md Modo de codificación del segmento
	 * @param numCh Número de caracteres representados
	 * @param dt Vector de bits que contiene los datos codificados
	 * @throws std::domain_error Si el número de caracteres es negativo
	 */
	QrSegment::QrSegment(const Mode& md, int numCh, const std::vector<bool>& dt) :
		mode(&md),
		numChars(numCh),
		data(dt) {
		if (numCh < 0)
			throw std::domain_error("Invalid value");
	}

	/**
	 * @brief Constructor de QrSegment con movimiento del vector de datos
	 *
	 * @param md Modo de codificación del segmento
	 * @param numCh Número de caracteres representados
	 * @param dt Vector de bits que contiene los datos codificados (movido)
	 * @throws std::domain_error Si el número de caracteres es negativo
	 */
	QrSegment::QrSegment(const Mode& md, int numCh, std::vector<bool>&& dt) :
		mode(&md),
		numChars(numCh),
		data(std::move(dt)) {
		if (numCh < 0)
			throw std::domain_error("Invalid value");
	}

	/**
	 * @brief Calcula el número total de bits para un conjunto de segmentos
	 *
	 * @param segs Vector de segmentos QR
	 * @param version Versión del código QR
	 * @return Número total de bits necesarios o -1 si hay un error
	 */
	int QrSegment::getTotalBits(const vector<QrSegment>& segs, int version) {
		int result = 0;
		for (const QrSegment& seg : segs) {
			int ccbits = seg.mode->numCharCountBits(version);
			if (seg.numChars >= (1L << ccbits))
				return -1;  // The segment's length doesn't fit the field's bit width
			if (4 + ccbits > INT_MAX - result)
				return -1;  // The sum will overflow an int type
			result += 4 + ccbits;
			if (seg.data.size() > static_cast<unsigned int>(INT_MAX - result))
				return -1;  // The sum will overflow an int type
			result += static_cast<int>(seg.data.size());
		}
		return result;
	}

	/**
	 * @brief Verifica si una cadena contiene solo caracteres numéricos
	 *
	 * @param text Cadena de caracteres a verificar
	 * @return true si solo contiene dígitos, false en caso contrario
	 */
	bool QrSegment::isNumeric(const char* text) {
		for (; *text != '\0'; text++) {
			char c = *text;
			if (c < '0' || c > '9')
				return false;
		}
		return true;
	}

	/**
	 * @brief Verifica si una cadena contiene solo caracteres alfanuméricos válidos
	 *
	 * @param text Cadena de caracteres a verificar
	 * @return true si solo contiene caracteres alfanuméricos válidos, false en caso contrario
	 */
	bool QrSegment::isAlphanumeric(const char* text) {
		for (; *text != '\0'; text++) {
			if (std::strchr(ALPHANUMERIC_CHARSET, *text) == nullptr)
				return false;
		}
		return true;
	}

	/**
	 * @brief Obtiene el modo de codificación del segmento
	 *
	 * @return Referencia al modo de codificación
	 */
	const QrSegment::Mode& QrSegment::getMode() const {
		return *mode;
	}

	/**
	 * @brief Obtiene el número de caracteres en el segmento
	 *
	 * @return Número de caracteres
	 */
	int QrSegment::getNumChars() const {
		return numChars;
	}

	/**
	 * @brief Obtiene los datos binarios del segmento
	 *
	 * @return Referencia al vector de bits
	 */
	const std::vector<bool>& QrSegment::getData() const {
		return data;
	}

	/**
	 * @brief Conjunto de caracteres permitidos en el modo alfanumérico
	 */
	const char* QrSegment::ALPHANUMERIC_CHARSET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";



	/*---- Class QrCode ----*/

	/**
	 * @brief Obtiene los bits de formato para un nivel de corrección de errores
	 *
	 * @param ecl Nivel de corrección de errores
	 * @return Valor entero con los bits de formato
	 * @throws std::logic_error Si el nivel de corrección no es válido
	 */
	int QrCode::getFormatBits(Ecc ecl) {
		switch (ecl) {
		case Ecc::LOW:  return 1;
		case Ecc::MEDIUM:  return 0;
		case Ecc::QUARTILE:  return 3;
		case Ecc::HIGH:  return 2;
		default:  throw std::logic_error("Unreachable");
		}
	}

	/**
	 * @brief Codifica texto en un código QR
	 *
	 * @param text Texto a codificar
	 * @param ecl Nivel de corrección de errores
	 * @return Objeto QrCode que contiene el código generado
	 */
	QrCode QrCode::encodeText(const char* text, Ecc ecl) {
		vector<QrSegment> segs = QrSegment::makeSegments(text);
		return encodeSegments(segs, ecl);
	}

	/**
	 * @brief Codifica datos binarios en un código QR
	 *
	 * @param data Vector de bytes a codificar
	 * @param ecl Nivel de corrección de errores
	 * @return Objeto QrCode que contiene el código generado
	 */
	QrCode QrCode::encodeBinary(const vector<uint8_t>& data, Ecc ecl) {
		vector<QrSegment> segs{ QrSegment::makeBytes(data) };
		return encodeSegments(segs, ecl);
	}

	/**
	 * @brief Codifica segmentos en un código QR con opciones avanzadas
	 *
	 * @param segs Vector de segmentos a codificar
	 * @param ecl Nivel de corrección de errores
	 * @param minVersion Versión mínima a usar (1-40)
	 * @param maxVersion Versión máxima a usar (1-40)
	 * @param mask Patrón de máscara (-1 para automático)
	 * @param boostEcl Aumentar nivel de corrección si hay espacio
	 * @return Objeto QrCode que contiene el código generado
	 * @throws std::invalid_argument Si los parámetros son inválidos
	 * @throws data_too_long Si los datos son demasiado grandes para la versión
	 */
	QrCode QrCode::encodeSegments(const vector<QrSegment>& segs, Ecc ecl,
		int minVersion, int maxVersion, int mask, bool boostEcl) {
		if (!(MIN_VERSION <= minVersion && minVersion <= maxVersion && maxVersion <= MAX_VERSION) || mask < -1 || mask > 7)
			throw std::invalid_argument("Invalid value");

		// Find the minimal version number to use
		int version, dataUsedBits;
		for (version = minVersion; ; version++) {
			int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;  // Number of data bits available
			dataUsedBits = QrSegment::getTotalBits(segs, version);
			if (dataUsedBits != -1 && dataUsedBits <= dataCapacityBits)
				break;  // This version number is found to be suitable
			if (version >= maxVersion) {  // All versions in the range could not fit the given data
				std::ostringstream sb;
				if (dataUsedBits == -1)
					sb << "Segment too long";
				else {
					sb << "Data length = " << dataUsedBits << " bits, ";
					sb << "Max capacity = " << dataCapacityBits << " bits";
				}
				throw data_too_long(sb.str());
			}
		}
		assert(dataUsedBits != -1);

		// Increase the error correction level while the data still fits in the current version number
		for (Ecc newEcl : {Ecc::MEDIUM, Ecc::QUARTILE, Ecc::HIGH}) {  // From low to high
			if (boostEcl && dataUsedBits <= getNumDataCodewords(version, newEcl) * 8)
				ecl = newEcl;
		}

		// Concatenate all segments to create the data bit string
		BitBuffer bb;
		for (const QrSegment& seg : segs) {
			bb.appendBits(static_cast<uint32_t>(seg.getMode().getModeBits()), 4);
			bb.appendBits(static_cast<uint32_t>(seg.getNumChars()), seg.getMode().numCharCountBits(version));
			bb.insert(bb.end(), seg.getData().begin(), seg.getData().end());
		}
		assert(bb.size() == static_cast<unsigned int>(dataUsedBits));

		// Add terminator and pad up to a byte if applicable
		size_t dataCapacityBits = static_cast<size_t>(getNumDataCodewords(version, ecl)) * 8;
		assert(bb.size() <= dataCapacityBits);
		bb.appendBits(0, std::min(4, static_cast<int>(dataCapacityBits - bb.size())));
		bb.appendBits(0, (8 - static_cast<int>(bb.size() % 8)) % 8);
		assert(bb.size() % 8 == 0);

		// Pad with alternating bytes until data capacity is reached
		for (uint8_t padByte = 0xEC; bb.size() < dataCapacityBits; padByte ^= 0xEC ^ 0x11)
			bb.appendBits(padByte, 8);

		// Pack bits into bytes in big endian
		vector<uint8_t> dataCodewords(bb.size() / 8);
		for (size_t i = 0; i < bb.size(); i++)
			dataCodewords.at(i >> 3) |= (bb.at(i) ? 1 : 0) << (7 - (i & 7));

		// Create the QR Code object
		return QrCode(version, ecl, dataCodewords, mask);
	}

	/**
	 * @brief Constructor principal de QrCode
	 *
	 * @param ver Versión del código QR (1-40)
	 * @param ecl Nivel de corrección de errores
	 * @param dataCodewords Vector con los bytes de datos
	 * @param msk Patrón de máscara (-1 para selección automática)
	 * @throws std::domain_error Si los parámetros están fuera de rango
	 */
	QrCode::QrCode(int ver, Ecc ecl, const vector<uint8_t>& dataCodewords, int msk) :
		// Initialize fields and check arguments
		version(ver),
		errorCorrectionLevel(ecl) {
		if (ver < MIN_VERSION || ver > MAX_VERSION)
			throw std::domain_error("Version value out of range");
		if (msk < -1 || msk > 7)
			throw std::domain_error("Mask value out of range");
		size = ver * 4 + 17;
		size_t sz = static_cast<size_t>(size);
		modules = vector<vector<bool> >(sz, vector<bool>(sz));  // Initially all light
		isFunction = vector<vector<bool> >(sz, vector<bool>(sz));

		// Compute ECC, draw modules
		drawFunctionPatterns();
		const vector<uint8_t> allCodewords = addEccAndInterleave(dataCodewords);
		drawCodewords(allCodewords);

		// Do masking
		if (msk == -1) {  // Automatically choose best mask
			long minPenalty = LONG_MAX;
			for (int i = 0; i < 8; i++) {
				applyMask(i);
				drawFormatBits(i);
				long penalty = getPenaltyScore();
				if (penalty < minPenalty) {
					msk = i;
					minPenalty = penalty;
				}
				applyMask(i);  // Undoes the mask due to XOR
			}
		}
		assert(0 <= msk && msk <= 7);
		mask = msk;
		applyMask(msk);  // Apply the final choice of mask
		drawFormatBits(msk);  // Overwrite old format bits

		isFunction.clear();
		isFunction.shrink_to_fit();
	}

	/**
	 * @brief Obtiene la versión del código QR
	 *
	 * @return Versión (1-40)
	 */
	int QrCode::getVersion() const {
		return version;
	}

	/**
	 * @brief Obtiene el tamaño del código QR en módulos
	 *
	 * @return Tamaño en módulos (pixeles)
	 */
	int QrCode::getSize() const {
		return size;
	}

	/**
	 * @brief Obtiene el nivel de corrección de errores
	 *
	 * @return Nivel de corrección de errores
	 */
	QrCode::Ecc QrCode::getErrorCorrectionLevel() const {
		return errorCorrectionLevel;
	}

	/**
	 * @brief Obtiene el patrón de máscara utilizado
	 *
	 * @return Patrón de máscara (0-7)
	 */
	int QrCode::getMask() const {
		return mask;
	}

	/**
	 * @brief Verifica si un módulo está activo (oscuro) en las coordenadas especificadas
	 *
	 * @param x Coordenada X
	 * @param y Coordenada Y
	 * @return true si el módulo está activo, false en caso contrario
	 */
	bool QrCode::getModule(int x, int y) const {
		return 0 <= x && x < size && 0 <= y && y < size && module(x, y);
	}

	/**
	 * @brief Dibuja los patrones de función fijos del código QR
	 */
	void QrCode::drawFunctionPatterns() {
		// Draw horizontal and vertical timing patterns
		for (int i = 0; i < size; i++) {
			setFunctionModule(6, i, i % 2 == 0);
			setFunctionModule(i, 6, i % 2 == 0);
		}

		// Draw 3 finder patterns (all corners except bottom right; overwrites some timing modules)
		drawFinderPattern(3, 3);
		drawFinderPattern(size - 4, 3);
		drawFinderPattern(3, size - 4);

		// Draw numerous alignment patterns
		const vector<int> alignPatPos = getAlignmentPatternPositions();
		size_t numAlign = alignPatPos.size();
		for (size_t i = 0; i < numAlign; i++) {
			for (size_t j = 0; j < numAlign; j++) {
				// Don't draw on the three finder corners
				if (!((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0)))
					drawAlignmentPattern(alignPatPos.at(i), alignPatPos.at(j));
			}
		}

		// Draw configuration data
		drawFormatBits(0);  // Dummy mask value; overwritten later in the constructor
		drawVersion();
	}

	/**
	 * @brief Dibuja los bits de formato en el código QR
	 *
	 * Los bits de formato contienen información sobre el nivel de corrección de errores
	 * y el patrón de máscara utilizado. Esta función dibuja estos bits en las posiciones
	 * específicas requeridas por el estándar QR.
	 *
	 * @param msk Patrón de máscara a utilizar (0-7)
	 */
	void QrCode::drawFormatBits(int msk) {
		// Calculate error correction code and pack bits
		int data = getFormatBits(errorCorrectionLevel) << 3 | msk;  // errCorrLvl is uint2, msk is uint3
		int rem = data;
		for (int i = 0; i < 10; i++)
			rem = (rem << 1) ^ ((rem >> 9) * 0x537);
		int bits = (data << 10 | rem) ^ 0x5412;  // uint15
		assert(bits >> 15 == 0);

		// Draw first copy
		for (int i = 0; i <= 5; i++)
			setFunctionModule(8, i, getBit(bits, i));
		setFunctionModule(8, 7, getBit(bits, 6));
		setFunctionModule(8, 8, getBit(bits, 7));
		setFunctionModule(7, 8, getBit(bits, 8));
		for (int i = 9; i < 15; i++)
			setFunctionModule(14 - i, 8, getBit(bits, i));

		// Draw second copy
		for (int i = 0; i < 8; i++)
			setFunctionModule(size - 1 - i, 8, getBit(bits, i));
		for (int i = 8; i < 15; i++)
			setFunctionModule(8, size - 15 + i, getBit(bits, i));
		setFunctionModule(8, size - 8, true);  // Always dark
	}

	/**
	 * @brief Dibuja la información de versión si el código QR es versión 7 o superior
	 *
	 * Para códigos QR de versión 7 o superior, se debe incluir información
	 * de la versión en ubicaciones específicas.
	 */
	void QrCode::drawVersion() {
		if (version < 7)
			return;

		// Calculate error correction code and pack bits
		int rem = version;  // version is uint6, in the range [7, 40]
		for (int i = 0; i < 12; i++)
			rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
		long bits = static_cast<long>(version) << 12 | rem;  // uint18
		assert(bits >> 18 == 0);

		// Draw two copies
		for (int i = 0; i < 18; i++) {
			bool bit = getBit(bits, i);
			int a = size - 11 + i % 3;
			int b = i / 3;
			setFunctionModule(a, b, bit);
			setFunctionModule(b, a, bit);
		}
	}

	/**
	 * @brief Dibuja un patrón de búsqueda (finder pattern) en la posición especificada
	 *
	 * Los patrones de búsqueda son marcadores cuadrados ubicados en tres esquinas
	 * del código QR que ayudan al escáner a localizar y orientar el código.
	 *
	 * @param x Coordenada X del centro del patrón
	 * @param y Coordenada Y del centro del patrón
	 */
	void QrCode::drawFinderPattern(int x, int y) {
		for (int dy = -4; dy <= 4; dy++) {
			for (int dx = -4; dx <= 4; dx++) {
				int dist = std::max(std::abs(dx), std::abs(dy));  // Chebyshev/infinity norm
				int xx = x + dx, yy = y + dy;
				if (0 <= xx && xx < size && 0 <= yy && yy < size)
					setFunctionModule(xx, yy, dist != 2 && dist != 4);
			}
		}
	}

	/**
	 * @brief Dibuja un patrón de alineación en la posición especificada
	 *
	 * Los patrones de alineación son marcadores más pequeños que ayudan
	 * a corregir la distorsión en códigos QR de mayor tamaño.
	 *
	 * @param x Coordenada X del centro del patrón
	 * @param y Coordenada Y del centro del patrón
	 */
	void QrCode::drawAlignmentPattern(int x, int y) {
		for (int dy = -2; dy <= 2; dy++) {
			for (int dx = -2; dx <= 2; dx++)
				setFunctionModule(x + dx, y + dy, std::max(std::abs(dx), std::abs(dy)) != 1);
		}
	}

	/**
	 * @brief Establece un módulo como parte de un patrón funcional
	 *
	 * Los módulos funcionales incluyen patrones de búsqueda, alineación,
	 * temporización y formato, que no deben ser modificados por el proceso de enmascaramiento.
	 *
	 * @param x Coordenada X del módulo
	 * @param y Coordenada Y del módulo
	 * @param isDark True para un módulo oscuro, false para un módulo claro
	 */
	void QrCode::setFunctionModule(int x, int y, bool isDark) {
		size_t ux = static_cast<size_t>(x);
		size_t uy = static_cast<size_t>(y);
		modules.at(uy).at(ux) = isDark;
		isFunction.at(uy).at(ux) = true;
	}

	/**
	 * @brief Obtiene el estado (oscuro o claro) de un módulo en coordenadas específicas
	 *
	 * @param x Coordenada X del módulo
	 * @param y Coordenada Y del módulo
	 * @return true si el módulo es oscuro, false si es claro
	 */
	bool QrCode::module(int x, int y) const {
		return modules.at(static_cast<size_t>(y)).at(static_cast<size_t>(x));
	}

	/**
	 * @brief Añade códigos de corrección de errores y realiza el entrelazado de datos
	 *
	 * Esta función implementa el algoritmo Reed-Solomon para generar códigos
	 * de corrección de errores y luego entrelaza los bloques según el estándar QR.
	 *
	 * @param data Vector de bytes con los datos originales
	 * @return Vector de bytes con datos y códigos de corrección entrelazados
	 * @throws std::invalid_argument Si los datos no coinciden con la capacidad esperada
	 */
	vector<uint8_t> QrCode::addEccAndInterleave(const vector<uint8_t>& data) const {
		if (data.size() != static_cast<unsigned int>(getNumDataCodewords(version, errorCorrectionLevel)))
			throw std::invalid_argument("Invalid argument");

		// Calculate parameter numbers
		int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[static_cast<int>(errorCorrectionLevel)][version];
		int blockEccLen = ECC_CODEWORDS_PER_BLOCK[static_cast<int>(errorCorrectionLevel)][version];
		int rawCodewords = getNumRawDataModules(version) / 8;
		int numShortBlocks = numBlocks - rawCodewords % numBlocks;
		int shortBlockLen = rawCodewords / numBlocks;

		// Split data into blocks and append ECC to each block
		vector<vector<uint8_t> > blocks;
		const vector<uint8_t> rsDiv = reedSolomonComputeDivisor(blockEccLen);
		for (int i = 0, k = 0; i < numBlocks; i++) {
			vector<uint8_t> dat(data.cbegin() + k, data.cbegin() + (k + shortBlockLen - blockEccLen + (i < numShortBlocks ? 0 : 1)));
			k += static_cast<int>(dat.size());
			const vector<uint8_t> ecc = reedSolomonComputeRemainder(dat, rsDiv);
			if (i < numShortBlocks)
				dat.push_back(0);
			dat.insert(dat.end(), ecc.cbegin(), ecc.cend());
			blocks.push_back(std::move(dat));
		}

		// Interleave (not concatenate) the bytes from every block into a single sequence
		vector<uint8_t> result;
		for (size_t i = 0; i < blocks.at(0).size(); i++) {
			for (size_t j = 0; j < blocks.size(); j++) {
				// Skip the padding byte in short blocks
				if (i != static_cast<unsigned int>(shortBlockLen - blockEccLen) || j >= static_cast<unsigned int>(numShortBlocks))
					result.push_back(blocks.at(j).at(i));
			}
		}
		assert(result.size() == static_cast<unsigned int>(rawCodewords));
		return result;
	}

	/**
	 * @brief Dibuja los codewords (bytes de datos y corrección) en el código QR
	 *
	 * Coloca los datos en el código QR siguiendo el patrón de zigzag
	 * definido por el estándar.
	 *
	 * @param data Vector de bytes con datos y códigos de corrección entrelazados
	 * @throws std::invalid_argument Si la cantidad de datos no coincide con la capacidad
	 */
	void QrCode::drawCodewords(const vector<uint8_t>& data) {
		if (data.size() != static_cast<unsigned int>(getNumRawDataModules(version) / 8))
			throw std::invalid_argument("Invalid argument");

		size_t i = 0;  // Bit index into the data
		// Do the funny zigzag scan
		for (int right = size - 1; right >= 1; right -= 2) {  // Index of right column in each column pair
			if (right == 6)
				right = 5;
			for (int vert = 0; vert < size; vert++) {  // Vertical counter
				for (int j = 0; j < 2; j++) {
					size_t x = static_cast<size_t>(right - j);  // Actual x coordinate
					bool upward = ((right + 1) & 2) == 0;
					size_t y = static_cast<size_t>(upward ? size - 1 - vert : vert);  // Actual y coordinate
					if (!isFunction.at(y).at(x) && i < data.size() * 8) {
						modules.at(y).at(x) = getBit(data.at(i >> 3), 7 - static_cast<int>(i & 7));
						i++;
					}
					// If this QR Code has any remainder bits (0 to 7), they were assigned as
					// 0/false/light by the constructor and are left unchanged by this method
				}
			}
		}
		assert(i == data.size() * 8);
	}

	/**
	 * @brief Aplica un patrón de máscara al código QR
	 *
	 * La máscara ayuda a evitar patrones que podrían confundir a los escáneres
	 * alternando los bits según reglas específicas.
	 *
	 * @param msk Patrón de máscara a aplicar (0-7)
	 * @throws std::domain_error Si el patrón de máscara está fuera del rango válido
	 */
	void QrCode::applyMask(int msk) {
		if (msk < 0 || msk > 7)
			throw std::domain_error("Mask value out of range");
		size_t sz = static_cast<size_t>(size);
		for (size_t y = 0; y < sz; y++) {
			for (size_t x = 0; x < sz; x++) {
				bool invert;
				switch (msk) {
				case 0:  invert = (x + y) % 2 == 0;                    break;
				case 1:  invert = y % 2 == 0;                          break;
				case 2:  invert = x % 3 == 0;                          break;
				case 3:  invert = (x + y) % 3 == 0;                    break;
				case 4:  invert = (x / 3 + y / 2) % 2 == 0;            break;
				case 5:  invert = x * y % 2 + x * y % 3 == 0;          break;
				case 6:  invert = (x * y % 2 + x * y % 3) % 2 == 0;    break;
				case 7:  invert = ((x + y) % 2 + x * y % 3) % 2 == 0;  break;
				default:  throw std::logic_error("Unreachable");
				}
				modules.at(y).at(x) = modules.at(y).at(x) ^ (invert & !isFunction.at(y).at(x));
			}
		}
	}

	/**
	 * @brief Calcula la puntuación de penalización para el código QR actual
	 *
	 * Las penalizaciones se basan en características como patrones repetidos,
	 * bloques del mismo color, o desequilibrio entre módulos claros y oscuros.
	 *
	 * @return Puntuación de penalización (valores más bajos son mejores)
	 */
	long QrCode::getPenaltyScore() const {
		long result = 0;

		// Adjacent modules in row having same color, and finder-like patterns
		for (int y = 0; y < size; y++) {
			bool runColor = false;
			int runX = 0;
			std::array<int, 7> runHistory = {};
			for (int x = 0; x < size; x++) {
				if (module(x, y) == runColor) {
					runX++;
					if (runX == 5)
						result += PENALTY_N1;
					else if (runX > 5)
						result++;
				}
				else {
					finderPenaltyAddHistory(runX, runHistory);
					if (!runColor)
						result += finderPenaltyCountPatterns(runHistory) * PENALTY_N3;
					runColor = module(x, y);
					runX = 1;
				}
			}
			result += finderPenaltyTerminateAndCount(runColor, runX, runHistory) * PENALTY_N3;
		}
		// Adjacent modules in column having same color, and finder-like patterns
		for (int x = 0; x < size; x++) {
			bool runColor = false;
			int runY = 0;
			std::array<int, 7> runHistory = {};
			for (int y = 0; y < size; y++) {
				if (module(x, y) == runColor) {
					runY++;
					if (runY == 5)
						result += PENALTY_N1;
					else if (runY > 5)
						result++;
				}
				else {
					finderPenaltyAddHistory(runY, runHistory);
					if (!runColor)
						result += finderPenaltyCountPatterns(runHistory) * PENALTY_N3;
					runColor = module(x, y);
					runY = 1;
				}
			}
			result += finderPenaltyTerminateAndCount(runColor, runY, runHistory) * PENALTY_N3;
		}

		// 2*2 blocks of modules having same color
		for (int y = 0; y < size - 1; y++) {
			for (int x = 0; x < size - 1; x++) {
				bool  color = module(x, y);
				if (color == module(x + 1, y) &&
					color == module(x, y + 1) &&
					color == module(x + 1, y + 1))
					result += PENALTY_N2;
			}
		}

		// Balance of dark and light modules
		int dark = 0;
		for (const vector<bool>& row : modules) {
			for (bool color : row) {
				if (color)
					dark++;
			}
		}
		int total = size * size;  // Note that size is odd, so dark/total != 1/2
		// Compute the smallest integer k >= 0 such that (45-5k)% <= dark/total <= (55+5k)%
		int k = static_cast<int>((std::abs(dark * 20L - total * 10L) + total - 1) / total) - 1;
		assert(0 <= k && k <= 9);
		result += k * PENALTY_N4;
		assert(0 <= result && result <= 2568888L);  // Non-tight upper bound based on default values of PENALTY_N1, ..., N4
		return result;
	}

	/**
	 * @brief Obtiene las posiciones de los patrones de alineación para la versión actual
	 *
	 * @return Vector con las coordenadas donde deben ubicarse los patrones de alineación
	 */
	vector<int> QrCode::getAlignmentPatternPositions() const {
		if (version == 1)
			return vector<int>();
		else {
			int numAlign = version / 7 + 2;
			int step = (version * 8 + numAlign * 3 + 5) / (numAlign * 4 - 4) * 2;
			vector<int> result;
			for (int i = 0, pos = size - 7; i < numAlign - 1; i++, pos -= step)
				result.insert(result.begin(), pos);
			result.insert(result.begin(), 6);
			return result;
		}
	}

	/**
	 * @brief Calcula el número total de módulos de datos sin formato en una versión específica
	 *
	 * @param ver Versión del código QR (1-40)
	 * @return Número de módulos de datos disponibles
	 * @throws std::domain_error Si la versión está fuera del rango válido
	 */
	int QrCode::getNumRawDataModules(int ver) {
		if (ver < MIN_VERSION || ver > MAX_VERSION)
			throw std::domain_error("Version number out of range");
		int result = (16 * ver + 128) * ver + 64;
		if (ver >= 2) {
			int numAlign = ver / 7 + 2;
			result -= (25 * numAlign - 10) * numAlign - 55;
			if (ver >= 7)
				result -= 36;
		}
		assert(208 <= result && result <= 29648);
		return result;
	}

	/**
	 * @brief Calcula el número de codewords de datos disponibles
	 *
	 * @param ver Versión del código QR
	 * @param ecl Nivel de corrección de errores
	 * @return Número de codewords disponibles para datos
	 */
	int QrCode::getNumDataCodewords(int ver, Ecc ecl) {
		return getNumRawDataModules(ver) / 8
			- ECC_CODEWORDS_PER_BLOCK[static_cast<int>(ecl)][ver]
			* NUM_ERROR_CORRECTION_BLOCKS[static_cast<int>(ecl)][ver];
	}

	/**
	 * @brief Calcula el polinomio divisor para la codificación Reed-Solomon
	 *
	 * @param degree Grado del polinomio (igual al número de codewords ECC)
	 * @return Vector de coeficientes del polinomio divisor
	 * @throws std::domain_error Si el grado está fuera del rango válido
	 */
	vector<uint8_t> QrCode::reedSolomonComputeDivisor(int degree) {
		if (degree < 1 || degree > 255)
			throw std::domain_error("Degree out of range");
		// Polynomial coefficients are stored from highest to lowest power, excluding the leading term which is always 1.
		// For example the polynomial x^3 + 255x^2 + 8x + 93 is stored as the uint8 array {255, 8, 93}.
		vector<uint8_t> result(static_cast<size_t>(degree));
		result.at(result.size() - 1) = 1;  // Start off with the monomial x^0

		// Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
		// and drop the highest monomial term which is always 1x^degree.
		// Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
		uint8_t root = 1;
		for (int i = 0; i < degree; i++) {
			// Multiply the current product by (x - r^i)
			for (size_t j = 0; j < result.size(); j++) {
				result.at(j) = reedSolomonMultiply(result.at(j), root);
				if (j + 1 < result.size())
					result.at(j) ^= result.at(j + 1);
			}
			root = reedSolomonMultiply(root, 0x02);
		}
		return result;
	}

	/**
	 * @brief Calcula el resto de la división polinómica para la corrección Reed-Solomon
	 *
	 * Esta función implementa la división polinómica en el campo de Galois GF(2^8)
	 * para calcular los bytes de corrección de errores.
	 *
	 * @param data Vector de bytes con los datos originales (dividendo)
	 * @param divisor Vector de bytes representando el polinomio divisor
	 * @return Vector de bytes con el resto de la división (bytes ECC)
	 */
	vector<uint8_t> QrCode::reedSolomonComputeRemainder(const vector<uint8_t>& data, const vector<uint8_t>& divisor) {
		vector<uint8_t> result(divisor.size());
		for (uint8_t b : data) {  // Polynomial division
			uint8_t factor = b ^ result.at(0);
			result.erase(result.begin());
			result.push_back(0);
			for (size_t i = 0; i < result.size(); i++)
				result.at(i) ^= reedSolomonMultiply(divisor.at(i), factor);
		}
		return result;
	}


	/**
	 * @brief Realiza la multiplicación en el campo finito GF(2^8)
	 *
	 * Implementa la multiplicación de dos bytes en el campo de Galois GF(2^8)
	 * usado en la codificación Reed-Solomon.
	 *
	 * @param x Primer operando (byte)
	 * @param y Segundo operando (byte)
	 * @return Resultado de la multiplicación en GF(2^8)
	 */
	uint8_t QrCode::reedSolomonMultiply(uint8_t x, uint8_t y) {
		// Russian peasant multiplication
		int z = 0;
		for (int i = 7; i >= 0; i--) {
			z = (z << 1) ^ ((z >> 7) * 0x11D);
			z ^= ((y >> i) & 1) * x;
		}
		assert(z >> 8 == 0);
		return static_cast<uint8_t>(z);
	}


	/**
	 * @brief Cuenta el número de patrones tipo finder en una secuencia de longitudes de módulos
	 *
	 * @param runHistory Historial de longitudes de secuencias de módulos
	 * @return Número de patrones encontrados (usado para calcular penalizaciones)
	 */
	int QrCode::finderPenaltyCountPatterns(const std::array<int, 7>& runHistory) const {
		int n = runHistory.at(1);
		assert(n <= size * 3);
		bool core = n > 0 && runHistory.at(2) == n && runHistory.at(3) == n * 3 && runHistory.at(4) == n && runHistory.at(5) == n;
		return (core && runHistory.at(0) >= n * 4 && runHistory.at(6) >= n ? 1 : 0)
			+ (core && runHistory.at(6) >= n * 4 && runHistory.at(0) >= n ? 1 : 0);
	}


	/**
	 * @brief Termina una secuencia de módulos y cuenta patrones tipo finder
	 *
	 * @param currentRunColor Color actual de la secuencia (true=oscuro, false=claro)
	 * @param currentRunLength Longitud de la secuencia actual
	 * @param runHistory Historial de longitudes de secuencias
	 * @return Número de patrones encontrados
	 */
	int QrCode::finderPenaltyTerminateAndCount(bool currentRunColor, int currentRunLength, std::array<int, 7>& runHistory) const {
		if (currentRunColor) {  // Terminate dark run
			finderPenaltyAddHistory(currentRunLength, runHistory);
			currentRunLength = 0;
		}
		currentRunLength += size;  // Add light border to final run
		finderPenaltyAddHistory(currentRunLength, runHistory);
		return finderPenaltyCountPatterns(runHistory);
	}


	/**
	 * @brief Añade una longitud de secuencia al historial de secuencias
	 *
	 * @param currentRunLength Longitud de la secuencia actual a añadir
	 * @param runHistory Historial de longitudes de secuencias
	 */
	void QrCode::finderPenaltyAddHistory(int currentRunLength, std::array<int, 7>& runHistory) const {
		if (runHistory.at(0) == 0)
			currentRunLength += size;  // Add light border to initial run
		std::copy_backward(runHistory.cbegin(), runHistory.cend() - 1, runHistory.end());
		runHistory.at(0) = currentRunLength;
	}


	/**
	 * @brief Verifica si un bit específico está activado en un valor
	 *
	 * @param x Valor entero a examinar
	 * @param i Posición del bit a verificar (0 = LSB)
	 * @return true si el bit está activado, false en caso contrario
	 */
	bool QrCode::getBit(long x, int i) {
		return ((x >> i) & 1) != 0;
	}


	/*---- Constantes ----*/

	/**
	 * @brief Penalización por grupos de módulos del mismo color en línea
	 */
	const int QrCode::PENALTY_N1 = 3;


	/**
	 * @brief Penalización por bloques 2x2 del mismo color
	 */
	const int QrCode::PENALTY_N2 = 3;

	/**
	 * @brief Penalización por patrones similares a los patrones de búsqueda
	 */
	const int QrCode::PENALTY_N3 = 40;

	/**
	 * @brief Penalización por desequilibrio entre módulos claros y oscuros
	 */
	const int QrCode::PENALTY_N4 = 10;

	
	const int8_t QrCode::ECC_CODEWORDS_PER_BLOCK[4][41] = {
		// Version: (note that index 0 is for padding, and is set to an illegal value)
		//0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
		{-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Low
		{-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},  // Medium
		{-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Quartile
		{-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // High
	};

	const int8_t QrCode::NUM_ERROR_CORRECTION_BLOCKS[4][41] = {
		// Version: (note that index 0 is for padding, and is set to an illegal value)
		//0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
		{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Low
		{-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Medium
		{-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Quartile
		{-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // High
	};

	/**
	 * @brief Constructor para la excepción de datos demasiado largos
	 *
	 * @param msg Mensaje de error descriptivo
	 */
	data_too_long::data_too_long(const std::string& msg) :
		std::length_error(msg) {}

	/*---- Class BitBuffer ----*/
	/**
	 * @brief Constructor por defecto para BitBuffer
	 *
	 * Crea un buffer de bits vacío
	 */
	BitBuffer::BitBuffer()
		: std::vector<bool>() {}

	/**
	 * @brief Añade bits al buffer a partir de un valor entero
	 *
	 * @param val Valor entero del que se extraerán los bits
	 * @param len Número de bits a añadir (empezando por el MSB)
	 * @throws std::domain_error Si len está fuera de rango o val requiere más de len bits
	 */
	void BitBuffer::appendBits(std::uint32_t val, int len) {
		if (len < 0 || len > 31 || val >> len != 0)
			throw std::domain_error("Value out of range");
		for (int i = len - 1; i >= 0; i--)  // Append bit by bit
			this->push_back(((val >> i) & 1) != 0);
	}

}