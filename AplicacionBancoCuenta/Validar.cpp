#include "Validar.h"
#include <regex>
#include <cctype>
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <sstream>
#include <iomanip>
#include <fstream>



#pragma region VALIDACIONES PARA CUENTA

#pragma endregion


// Valida que el numero de cuenta sea valido: exactamente 10 digitos, tipo string
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

// Valida que el saldo solo contenga digitos y, opcionalmente, un punto decimal
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

// Valida el estado de la cuenta
bool Validar::ValidarEstadoCuenta(const std::string& estado) {
    std::regex regex("^(Activa|Inactiva)$");
    return std::regex_match(estado, regex);
}

// Valida el tipo de cuenta, ahorros o corriente
bool Validar::ValidarTipoCuenta(const std::string& tipo) {
    std::regex regex("^(Ahorros|Corriente)$");
    return std::regex_match(tipo, regex);
}

// Valida el tipo de transaccion, retiro o deposito
bool Validar::ValidarTipoTransaccion(const std::string& tipo) {
    std::regex regex("^(Retiro|Deposito)$");
    return std::regex_match(tipo, regex);
}

// Valida el monto de transaccion, solo numeros enteros
bool Validar::ValidarMontoTransaccion(int saldo) {
    return saldo >= 0;
}

// Valida la fecha sea acorde al sistema operativo
bool Validar::ValidarFecha(const std::string& fecha) {
    // Validar formato dd/mm/aaaa
    std::regex regex("^([0]?[1-9]|[12][0-9]|3[01])/([0]?[1-9]|1[0-2])/([0-9]{4})$");
    if (!std::regex_match(fecha, regex)) {
        return false;
    }

    // Extraer dia, mes y año
    int dia, mes, anio;
    if (sscanf_s(fecha.c_str(), "%d/%d/%d", &dia, &mes, &anio) != 3) {
        return false;
    }

    // Validar año razonable
    if (anio < 1900) return false;

    // Validar mes
    if (mes < 1 || mes > 12) return false;

    // Validar dias maximos por mes y año bisiesto
    int diasMes[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    // Año bisiesto
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

// Valida el numero con dos decimales para el monto o saldo
bool Validar::ValidarNumeroConDosDecimales(const std::string& monto) {
    // Permite numeros positivos, opcionalmente con hasta dos decimales
    std::regex regex("^[0-9]+(\\.[0-9]{1,2})?$");
    return std::regex_match(monto, regex);
}

// Valida el poder copiar y pegar el monto desde el portapapeles
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

// Valida que el nombre de la persona sea valido, solo letras y espacios
bool Validar::ValidarNombrePersona(const std::string& nombre) {
    std::regex regex("^[a-zA-Z ]+$");
    return std::regex_match(nombre, regex);
}

// Validar el numero de cedula de 10 digitos, solo numeros para Ecuador 
// modelo usado https://medium.com/@bryansuarez/c%C3%B3mo-validar-c%C3%A9dula-y-ruc-en-ecuador-b62c5666186f
// https://www.skypack.dev/view/udv-ec
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

// Valida que la cedula no tenga todos los digitos iguales
bool Validar::todosLosDigitosIguales(const std::string& cedula) {
    return std::all_of(cedula.begin(), cedula.end(), [primero = cedula[0]](char c) {
        return c == primero;
        });
}

#pragma endregion




#pragma endregion

// Funcion para validar la tecla ingresada
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

bool Validar::archivoExiste(const std::string& rutaArchivo) {
    std::ifstream f(rutaArchivo.c_str());
    return f.is_open();
}


std::string Validar::generarNombreConIndice(const std::string& rutaDirectorio, const std::string& fechaFormateada)
{
    // Por ejemplo: "Respaldo" + "00" + "_" + "30_05_2025" + ".bak"
    // Incrementa el índice si se detecta un archivo existente
    int indice = 0;
    while (true) {
        std::ostringstream oss;
        oss << "Respaldo" << std::setw(2) << std::setfill('0')
            << indice << "_" << fechaFormateada;

        std::string posibleNombre = rutaDirectorio + oss.str() + ".bak";
        if (!archivoExiste(posibleNombre)) {
            // Devuelve el nombre si no está repetido
            return oss.str();
        }
        indice++;
    }
}