/**
 * @file Validar.cpp
 * @brief Implementaci�n de funciones de validaci�n para datos bancarios
 *
 * Este archivo contiene la implementaci�n de diversas funciones para validar
 * informaci�n bancaria como n�meros de cuenta, c�dulas, montos, fechas, etc.
 * Las validaciones est�n organizadas por categor�as para facilitar su uso.
 */
#include "Validar.h"
#include <regex>
#include <cctype>
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <sstream>
#include <iomanip>
#include <fstream>

 /**
  * @brief Valida que un n�mero de cuenta tenga el formato correcto
  *
  * Un n�mero de cuenta v�lido debe contener exactamente 10 d�gitos num�ricos.
  *
  * @param numero N�mero de cuenta a validar
  * @return bool true si el n�mero de cuenta es v�lido, false en caso contrario
  */
bool Validar::ValidarNumeroCuenta(const std::string numero) {
    if (numero.length() != 10) {
        return false;
    }
    for (char c : numero) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Valida que una cadena represente un saldo v�lido
 *
 * Un saldo v�lido debe contener solo d�gitos y opcionalmente un punto decimal
 * seguido de uno o dos d�gitos. Adem�s, el valor num�rico debe ser no negativo.
 *
 * @param saldoStr Cadena que representa el saldo a validar
 * @return bool true si el saldo es v�lido, false en caso contrario
 */
bool Validar::ValidarSaldo(const std::string& saldoStr) {
    std::regex regex("^[0-9]+(\\.[0-9]{1,2})?$");
    if (!std::regex_match(saldoStr, regex)) {
        return false;
    }
    try {
        double saldo = std::stod(saldoStr);
        if (saldo < 0) {
            return false;
        }
    }
    catch (...) {
        return false;
    }
    return true;
}

/**
 * @brief Valida que el estado de una cuenta sea v�lido
 *
 * Un estado v�lido debe ser "Activa" o "Inactiva" (case sensitive).
 *
 * @param estado Estado de cuenta a validar
 * @return bool true si el estado es v�lido, false en caso contrario
 */
bool Validar::ValidarEstadoCuenta(const std::string& estado) {
    std::regex regex("^(Activa|Inactiva)$");
    return std::regex_match(estado, regex);
}

/**
 * @brief Valida que el tipo de cuenta sea v�lido
 *
 * Un tipo de cuenta v�lido debe ser "Ahorros" o "Corriente" (case sensitive).
 *
 * @param tipo Tipo de cuenta a validar
 * @return bool true si el tipo es v�lido, false en caso contrario
 */
bool Validar::ValidarTipoCuenta(const std::string& tipo) {
    std::regex regex("^(Ahorros|Corriente)$");
    return std::regex_match(tipo, regex);
}

/**
 * @brief Valida que el tipo de transacci�n sea v�lido
 *
 * Un tipo de transacci�n v�lido debe ser "Retiro" o "Deposito" (case sensitive).
 *
 * @param tipo Tipo de transacci�n a validar
 * @return bool true si el tipo es v�lido, false en caso contrario
 */
bool Validar::ValidarTipoTransaccion(const std::string& tipo) {
    std::regex regex("^(Retiro|Deposito)$");
    return std::regex_match(tipo, regex);
}

/**
 * @brief Valida que un monto de transacci�n sea v�lido
 *
 * Un monto v�lido debe ser un valor entero no negativo.
 *
 * @param saldo Monto a validar
 * @return bool true si el monto es v�lido, false en caso contrario
 */
bool Validar::ValidarMontoTransaccion(int saldo) {
    return saldo >= 0;
}

/**
 * @brief Valida que una fecha tenga formato correcto y sea una fecha v�lida
 *
 * Verifica que la fecha tenga el formato "dd/mm/aaaa", que sea una fecha
 * existente en el calendario y que no sea futura respecto a la fecha del sistema.
 *
 * @param fecha Fecha a validar en formato "dd/mm/aaaa"
 * @return bool true si la fecha es v�lida, false en caso contrario
 */
bool Validar::ValidarFecha(const std::string& fecha) {
    // Validar formato dd/mm/aaaa
    std::regex regex("^([0]?[1-9]|[12][0-9]|3[01])/([0]?[1-9]|1[0-2])/([0-9]{4})$");
    if (!std::regex_match(fecha, regex)) {
        return false;
    }

    // Extraer dia, mes y a�o
    int dia, mes, anio;
    if (sscanf_s(fecha.c_str(), "%d/%d/%d", &dia, &mes, &anio) != 3) {
        return false;
    }

    // Validar a�o razonable
    if (anio < 1900) return false;

    // Validar mes
    if (mes < 1 || mes > 12) return false;

    // Validar dias maximos por mes y a�o bisiesto
    int diasMes[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    // A�o bisiesto
    if (mes == 2 && ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0))) {
        diasMes[1] = 29;
    }
    if (dia < 1 || dia > diasMes[mes - 1]) return false;

    // Obtener fecha actual del sistema
    SYSTEMTIME st;
    GetLocalTime(&st);
    int diaActual = st.wDay;
    int mesActual = st.wMonth;
    int anioActual = st.wYear;

    // Comparar con la fecha actual
    if (anio > anioActual) return false;
    if (anio == anioActual && mes > mesActual) return false;
    if (anio == anioActual && mes == mesActual && dia > diaActual) return false;

    return true;
}

/**
 * @brief Valida que una cadena represente un n�mero con hasta dos decimales
 *
 * Un valor v�lido debe contener solo d�gitos y opcionalmente un punto decimal
 * seguido de uno o dos d�gitos.
 *
 * @param monto Cadena que representa el monto a validar
 * @return bool true si el formato es v�lido, false en caso contrario
 */
bool Validar::ValidarNumeroConDosDecimales(const std::string& monto) {
    // Permite numeros positivos, opcionalmente con hasta dos decimales
    std::regex regex("^[0-9]+(\\.[0-9]{1,2})?$");
    return std::regex_match(monto, regex);
}

/**
 * @brief Lee texto desde el portapapeles del sistema
 *
 * �til para permitir operaciones de pegado en formularios.
 *
 * @return std::string Texto contenido en el portapapeles o cadena vac�a si hay error
 */
std::string Validar::leerDesdePortapapeles() {
    if (!OpenClipboard(nullptr)) return "";
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        CloseClipboard();
        return "";
    }

    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == nullptr) {
        CloseClipboard();
        return "";
    }

    std::string text(pszText);
    GlobalUnlock(hData);
    CloseClipboard();
    return text;
}

/**
 * @brief Lee y valida interactivamente un n�mero de cuenta
 *
 * Permite al usuario ingresar un n�mero de cuenta de hasta 10 d�gitos,
 * con validaci�n en tiempo real.
 *
 * @return std::string N�mero de cuenta ingresado
 */
std::string Validar::ValidarLeerNumeroCuenta() {
    // Lee un numero de cuenta hasta 10 digitos
    std::string numeroCuenta;
    int digitos = 0;
    std::cout << "Ingrese numero de cuenta (maximo 10 digitos): ";
    while (true) {
        char tecla = _getch();
        if ((tecla >= '0' && tecla <= '9') && digitos < 10) {
            numeroCuenta += tecla;
            digitos++;
            std::cout << tecla;
        } else if (tecla == 8 && !numeroCuenta.empty()) { // backspace
            numeroCuenta.pop_back();
            digitos--;
            std::cout << "\b \b";
        } else if (tecla == 13) { // enter
            std::cout << std::endl;
            break;
        }
        // Ignorar teclas especiales u otros caracteres
    }
    return numeroCuenta;
}

#pragma region VALIDACIONES PARA PERSONA 

/**
 * @brief Valida que un nombre de persona tenga formato correcto
 *
 * Un nombre v�lido debe contener solo letras y espacios.
 *
 * @param nombre Nombre a validar
 * @return bool true si el nombre es v�lido, false en caso contrario
 */
bool Validar::ValidarNombrePersona(const std::string& nombre) {
    std::regex regex("^[a-zA-Z ]+$");
    return std::regex_match(nombre, regex);
}

// Validar el numero de cedula de 10 digitos, solo numeros para Ecuador 
// modelo usado https://medium.com/@bryansuarez/c%C3%B3mo-validar-c%C3%A9dula-y-ruc-en-ecuador-b62c5666186f
// https://www.skypack.dev/view/udv-ec
/**
 * @brief Valida que una c�dula ecuatoriana tenga formato correcto
 *
 * Implementa el algoritmo oficial de validaci�n de c�dulas ecuatorianas,
 * verificando longitud, c�digo de provincia, d�gito verificador y otros requisitos.
 *
 * @param cedula C�dula a validar (debe tener 10 d�gitos)
 * @return bool true si la c�dula es v�lida, false en caso contrario
 */
bool Validar::ValidarCedula(const std::string& cedula) {
    // Paso 1: longitud y solo digitos
    if (cedula.length() != 10 || !std::all_of(cedula.begin(), cedula.end(), ::isdigit)) {
        return false;
    }

    // Paso 2: todos los digitos iguales (ej. 0000000000, 1111111111)
    if (todosLosDigitosIguales(cedula)) {
        return false;
    }

    // Paso 3: validar codigo de provincia
    int provincia = std::stoi(cedula.substr(0, 2));
    if (provincia < 1 || provincia > 24) { // 00 no es valido
        return false;
    }

    // Paso 4: tercer digito debe estar entre 0 y 5 para personas naturales
    int tercerDigito = cedula[2] - '0';
    if (tercerDigito < 0 || tercerDigito > 5) {
        return false;
    }

    // Paso 5: validacion del digito verificador con el algoritmo oficial
    int coeficientes[9] = { 2, 1, 2, 1, 2, 1, 2, 1, 2 };
    int suma = 0;

    for (int i = 0; i < 9; ++i) {
        int digito = cedula[i] - '0';
        int producto = digito * coeficientes[i];
        if (producto >= 10) {
            producto -= 9;
        }
        suma += producto;
    }

    int digitoVerificador = cedula[9] - '0';
    int resultado = suma % 10;
    int comparacion = (resultado == 0) ? 0 : 10 - resultado;

    return comparacion == digitoVerificador;
} 



#pragma region Funciones internas de ValidarPersona

/**
 * @brief Verifica si todos los d�gitos de una cadena son iguales
 *
 * Funci�n auxiliar utilizada en la validaci�n de c�dulas para detectar
 * c�dulas inv�lidas como "0000000000" o "9999999999".
 *
 * @param cedula Cadena de d�gitos a verificar
 * @return bool true si todos los d�gitos son iguales, false en caso contrario
 */
bool Validar::todosLosDigitosIguales(const std::string& cedula) {
    return std::all_of(cedula.begin(), cedula.end(), [primero = cedula[0]](char c) {
        return c == primero;
        });
}

#pragma endregion




#pragma endregion

/**
 * @brief Valida si una tecla es aceptable seg�n el tipo de entrada requerido
 *
 * Permite filtrar caracteres seg�n el contexto de entrada (num�rico, alfab�tico, etc.).
 *
 * @param tecla Car�cter a validar
 * @param tipo Tipo de entrada que determina las reglas de validaci�n
 * @return bool true si la tecla es v�lida para el tipo de entrada, false en caso contrario
 */
bool Validar::ValidarTecla(char tecla, TipoEntrada tipo) {
    switch (tipo)
    {
	case TipoEntrada::NUMERICO_CEDULA: // Se valida la tecla para el ingreso de cedula
        // Solo aceptar digitos (0-9), Backspace y Enter
		return (tecla >= '0' && tecla <= '9') || tecla == 8 || tecla == 13; // 8 es Backspace, 13 es Enter
    case TipoEntrada::NUMERICO_CUENTA: // Se valida la tecla para el ingreso de monto
        // Solo digitos, un solo punto, y dos decimales, no permitir signo negativo
        return (std::isdigit(static_cast<unsigned char>(tecla)) || tecla == '.' || tecla == 8) && (tecla != '-');
    case TipoEntrada::ALFABETICO:
        // Solo letras y espacio
        return std::isalpha(static_cast<unsigned char>(tecla)) || tecla == ' ' || tecla == 8 || tecla == 13;
    case TipoEntrada::ALFANUMERICO:
        // Letras, digitos y espacio
        return std::isalnum(static_cast<unsigned char>(tecla)) || tecla == ' ' || tecla == 8 || tecla == 13;
    case TipoEntrada::CORREO:
        // Letras, digitos, @, ., _, y guion
        return std::isalnum(static_cast<unsigned char>(tecla)) || tecla == '@' || tecla == '.' || tecla == '_' || tecla == '-' || tecla == 8 || tecla == 13;
    case TipoEntrada::TELEFONO:
        // Solo digitos, permitir retroceso y Enter
        return std::isdigit(static_cast<unsigned char>(tecla)) || tecla == 8 || tecla == 13;
    default:
        return false;
    }
}

/**
 * @brief Verifica si un archivo existe en el sistema de archivos
 *
 * @param rutaArchivo Ruta completa al archivo
 * @return bool true si el archivo existe, false en caso contrario
 */
bool Validar::archivoExiste(const std::string& rutaArchivo) {
    std::ifstream f(rutaArchivo.c_str());
    return f.is_open();
}

/**
 * @brief Genera un nombre de archivo �nico basado en fecha y un �ndice incremental
 *
 * �til para crear nombres de archivo para respaldos, evitando colisiones
 * al agregar un �ndice num�rico si ya existe un archivo con el mismo nombre.
 *
 * @param rutaDirectorio Directorio donde se almacenar� el archivo
 * @param fechaFormateada Fecha en formato de texto para incluir en el nombre
 * @return std::string Nombre de archivo �nico
 */
std::string Validar::generarNombreConIndice(const std::string& rutaDirectorio, const std::string& fechaFormateada)
{
    // Por ejemplo: "Respaldo" + "00" + "_" + "30_05_2025" + ".bak"
    // Incrementa el indice si se detecta un archivo existente
    int indice = 0;
    while (true) {
        std::ostringstream oss;
        oss << "Respaldo" << std::setw(2) << std::setfill('0')
            << indice << "_" << fechaFormateada;

        std::string posibleNombre = rutaDirectorio + oss.str() + ".bak";
        if (!archivoExiste(posibleNombre)) {
            // Devuelve el nombre si no esta repetido
            return oss.str();
        }
        indice++;
    }
}