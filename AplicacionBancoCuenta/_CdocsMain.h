/**
 * @file _CdocsMain.h
 * @mainpage Sistema de Gesti�n Bancaria
 *
 * @section intro_sec Introducci�n
 *
 * El Sistema de Gesti�n Bancaria es una aplicaci�n de consola desarrollada en C++ que simula
 * las operaciones b�sicas de un banco. El sistema permite gestionar clientes, cuentas bancarias
 * (ahorro y corriente), realizar transacciones y generar reportes.
 *
 * La aplicaci�n utiliza conceptos avanzados de programaci�n orientada a objetos como herencia,
 * polimorfismo, templates y estructuras de datos personalizadas.
 *
 * @section features_sec Caracter�sticas Principales
 *
 * - Gesti�n de clientes (creaci�n, b�squeda, actualizaci�n)
 * - Administraci�n de cuentas de ahorro y cuentas corrientes
 * - Operaciones bancarias (dep�sitos, retiros, transferencias)
 * - Validaci�n de datos y fechas
 * - Persistencia de datos mediante archivos
 * - Interfaz de texto con marquesina informativa
 * - Generaci�n de c�digos QR para cuentas
 * - Implementaci�n did�ctica de estructuras de datos (listas enlazadas, �rbol B)
 * - Cifrado y descifrado de informaci�n
 *
 * @section modules_sec M�dulos Principales
 *
 * @subsection cliente_mod Gesti�n de Clientes
 * Este m�dulo se encarga de administrar la informaci�n personal de los clientes del banco.
 * La clase principal es @ref Persona que almacena datos como nombre, apellido, direcci�n, etc.
 *
 * @subsection cuentas_mod Gesti�n de Cuentas
 * M�dulo para administrar las cuentas bancarias. La clase base es @ref Cuenta, de la cual
 * heredan @ref CuentaAhorros y @ref CuentaCorriente, cada una con sus propias reglas de negocio.
 *
 * @subsection util_mod Utilidades
 * Proporciona funcionalidades comunes como validaci�n de datos (@ref Validar),
 * manejo de fechas (@ref Fecha), y diversas herramientas (@ref Utilidades).
 *
 * @subsection ui_mod Interfaz de Usuario
 * M�dulo que implementa la interfaz de usuario en modo texto, incluyendo una
 * marquesina informativa (@ref Marquesina).
 *
 * @section install_sec Requisitos e Instalaci�n
 *
 * - Visual Studio 2022 o superior
 * - C++14 o superior
 * - Windows 8 o superior
 *
 * Para compilar el proyecto, abra la soluci�n en Visual Studio y seleccione la opci�n "Compilar soluci�n".
 *
 * @section usage_sec Uso B�sico
 *
 * 1. Ejecute la aplicaci�n compilada
 * 2. Utilice el men� principal para navegar entre las diferentes opciones
 * 3. Siga las instrucciones en pantalla para realizar operaciones
 *
 * @section authors_sec Autores
 *
 * - Uriel Andrade
 * - Kerly Chuqui
 * - Abner Proano
 *
 * @section license_sec Licencia
 *
 * Este proyecto es de uso educativo y est� protegido por derechos de autor.
 *
 * @page clases Clases Principales
 *
 * - @ref Cuenta - Clase abstracta base para las cuentas bancarias
 * - @ref CuentaAhorros - Implementaci�n de cuenta de ahorros
 * - @ref CuentaCorriente - Implementaci�n de cuenta corriente
 * - @ref Persona - Gesti�n de datos personales de clientes
 * - @ref Fecha - Manejo y validaci�n de fechas
 * - @ref Validar - Validaciones de datos para el sistema
 * - @ref Banco - N�cleo principal de la aplicaci�n
 * - @ref Marquesina - Interfaz visual para mostrar mensajes en movimiento
 * - @ref NodoPersona - Implementaci�n de nodo para lista enlazada de personas
 * - @ref Utilidades - Funciones de utilidad general
 *
 * @page estructuras Estructuras de Datos
 *
 * - **Lista Enlazada**: Implementada para gestionar colecciones de personas
 * - **Lista Doblemente Enlazada**: Utilizada para gestionar cuentas bancarias
 * - **�rbol B**: Implementaci�n did�ctica para b�squedas eficientes
 *
 * @page tecnologias Tecnolog�as Utilizadas
 *
 * - **C++14**: Lenguaje de programaci�n principal
 * - **Visual Studio 2022**: Entorno de desarrollo
 * - **Doxygen**: Generaci�n de documentaci�n
 * - **Windows API**: Para funcionalidades espec�ficas de Windows
 *
 * @defgroup MenuPrincipal Men� Principal
 * @brief Organizaci�n jer�rquica del proyecto
 *
 * @defgroup ArchivosEncabezado Archivos de Encabezado
 * @ingroup MenuPrincipal
 * @brief Archivos de declaraci�n (.h)
 * @{
 *     @defgroup CuentaH Cuenta.h
 *     @brief Definici�n de la clase abstracta Cuenta
 *
 *     @defgroup CuentaAhorrosH CuentaAhorros.h
 *     @brief Definici�n de la clase CuentaAhorros
 *
 *     @defgroup CuentaCorrienteH CuentaCorriente.h
 *     @brief Definici�n de la clase CuentaCorriente
 *
 *     @defgroup PersonaH Persona.h
 *     @brief Definici�n de la clase Persona
 *
 *     @defgroup FechaH Fecha.h
 *     @brief Definici�n de la clase Fecha
 *
 *     @defgroup ValidarH Validar.h
 *     @brief Definici�n de la clase Validar
 *
 *     @defgroup BancoH Banco.h
 *     @brief Definici�n de la clase Banco
 *
 *     @defgroup MarquesinaH Marquesina.h
 *     @brief Definici�n de la clase Marquesina
 *
 *     @defgroup NodoPersonaH NodoPersona.h
 *     @brief Definici�n de la clase NodoPersona
 *
 *     @defgroup UtilidadesH Utilidades.h
 *     @brief Definici�n de la clase Utilidades
 *
 *     @defgroup CifradoH Cifrado.h
 *     @brief Definici�n de la clase Cifrado
 *
 *     @defgroup CodigoQRH CodigoQR.h
 *     @brief Definici�n de la clase CodigoQR
 * @}
 *
 * @defgroup ArchivosOrigen Archivos de Origen
 * @ingroup MenuPrincipal
 * @brief Archivos de implementaci�n (.cpp)
 * @{
 *     @defgroup MainCPP main.cpp
 *     @brief Implementaci�n del punto de entrada de la aplicaci�n
 *
 *     @defgroup CuentaAhorrosCPP CuentaAhorros.cpp
 *     @brief Implementaci�n de la clase CuentaAhorros
 *
 *     @defgroup CuentaCorrienteCPP CuentaCorriente.cpp
 *     @brief Implementaci�n de la clase CuentaCorriente
 *
 *     @defgroup PersonaCPP Persona.cpp
 *     @brief Implementaci�n de la clase Persona
 *
 *     @defgroup FechaCPP Fecha.cpp
 *     @brief Implementaci�n de la clase Fecha
 *
 *     @defgroup ValidarCPP Validar.cpp
 *     @brief Implementaci�n de la clase Validar
 *
 *     @defgroup BancoCPP Banco.cpp
 *     @brief Implementaci�n de la clase Banco
 *
 *     @defgroup MarquesinaCPP Marquesina.cpp
 *     @brief Implementaci�n de la clase Marquesina
 *
 *     @defgroup UtilidadesCPP Utilidades.cpp
 *     @brief Implementaci�n de la clase Utilidades
 *
 *     @defgroup CifradoCPP Cifrado.cpp
 *     @brief Implementaci�n de la clase Cifrado
 *
 *     @defgroup CodigoQRCPP CodigoQR.cpp
 *     @brief Implementaci�n de la clase CodigoQR
 * @}
 */