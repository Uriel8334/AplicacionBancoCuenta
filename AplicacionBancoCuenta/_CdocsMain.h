/**
 * @file _CdocsMain.h
 * @mainpage Sistema de Gestión Bancaria
 *
 * @section intro_sec Introducción
 *
 * El Sistema de Gestión Bancaria es una aplicación de consola desarrollada en C++ que simula
 * las operaciones básicas de un banco. El sistema permite gestionar clientes, cuentas bancarias
 * (ahorro y corriente), realizar transacciones y generar reportes.
 *
 * La aplicación utiliza conceptos avanzados de programación orientada a objetos como herencia,
 * polimorfismo, templates y estructuras de datos personalizadas.
 *
 * @section features_sec Características Principales
 *
 * - Gestión de clientes (creación, búsqueda, actualización)
 * - Administración de cuentas de ahorro y cuentas corrientes
 * - Operaciones bancarias (depósitos, retiros, transferencias)
 * - Validación de datos y fechas
 * - Persistencia de datos mediante archivos
 * - Interfaz de texto con marquesina informativa
 * - Generación de códigos QR para cuentas
 * - Implementación didáctica de estructuras de datos (listas enlazadas, árbol B)
 * - Cifrado y descifrado de información
 *
 * @section modules_sec Módulos Principales
 *
 * @subsection cliente_mod Gestión de Clientes
 * Este módulo se encarga de administrar la información personal de los clientes del banco.
 * La clase principal es @ref Persona que almacena datos como nombre, apellido, dirección, etc.
 *
 * @subsection cuentas_mod Gestión de Cuentas
 * Módulo para administrar las cuentas bancarias. La clase base es @ref Cuenta, de la cual
 * heredan @ref CuentaAhorros y @ref CuentaCorriente, cada una con sus propias reglas de negocio.
 *
 * @subsection util_mod Utilidades
 * Proporciona funcionalidades comunes como validación de datos (@ref Validar),
 * manejo de fechas (@ref Fecha), y diversas herramientas (@ref Utilidades).
 *
 * @subsection ui_mod Interfaz de Usuario
 * Módulo que implementa la interfaz de usuario en modo texto, incluyendo una
 * marquesina informativa (@ref Marquesina).
 *
 * @section install_sec Requisitos e Instalación
 *
 * - Visual Studio 2022 o superior
 * - C++14 o superior
 * - Windows 8 o superior
 *
 * Para compilar el proyecto, abra la solución en Visual Studio y seleccione la opción "Compilar solución".
 *
 * @section usage_sec Uso Básico
 *
 * 1. Ejecute la aplicación compilada
 * 2. Utilice el menú principal para navegar entre las diferentes opciones
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
 * Este proyecto es de uso educativo y está protegido por derechos de autor.
 *
 * @page clases Clases Principales
 *
 * - @ref Cuenta - Clase abstracta base para las cuentas bancarias
 * - @ref CuentaAhorros - Implementación de cuenta de ahorros
 * - @ref CuentaCorriente - Implementación de cuenta corriente
 * - @ref Persona - Gestión de datos personales de clientes
 * - @ref Fecha - Manejo y validación de fechas
 * - @ref Validar - Validaciones de datos para el sistema
 * - @ref Banco - Núcleo principal de la aplicación
 * - @ref Marquesina - Interfaz visual para mostrar mensajes en movimiento
 * - @ref NodoPersona - Implementación de nodo para lista enlazada de personas
 * - @ref Utilidades - Funciones de utilidad general
 *
 * @page estructuras Estructuras de Datos
 *
 * - **Lista Enlazada**: Implementada para gestionar colecciones de personas
 * - **Lista Doblemente Enlazada**: Utilizada para gestionar cuentas bancarias
 * - **Árbol B**: Implementación didáctica para búsquedas eficientes
 *
 * @page tecnologias Tecnologías Utilizadas
 *
 * - **C++14**: Lenguaje de programación principal
 * - **Visual Studio 2022**: Entorno de desarrollo
 * - **Doxygen**: Generación de documentación
 * - **Windows API**: Para funcionalidades específicas de Windows
 *
 * @defgroup MenuPrincipal Menú Principal
 * @brief Organización jerárquica del proyecto
 *
 * @defgroup ArchivosEncabezado Archivos de Encabezado
 * @ingroup MenuPrincipal
 * @brief Archivos de declaración (.h)
 * @{
 *     @defgroup CuentaH Cuenta.h
 *     @brief Definición de la clase abstracta Cuenta
 *
 *     @defgroup CuentaAhorrosH CuentaAhorros.h
 *     @brief Definición de la clase CuentaAhorros
 *
 *     @defgroup CuentaCorrienteH CuentaCorriente.h
 *     @brief Definición de la clase CuentaCorriente
 *
 *     @defgroup PersonaH Persona.h
 *     @brief Definición de la clase Persona
 *
 *     @defgroup FechaH Fecha.h
 *     @brief Definición de la clase Fecha
 *
 *     @defgroup ValidarH Validar.h
 *     @brief Definición de la clase Validar
 *
 *     @defgroup BancoH Banco.h
 *     @brief Definición de la clase Banco
 *
 *     @defgroup MarquesinaH Marquesina.h
 *     @brief Definición de la clase Marquesina
 *
 *     @defgroup NodoPersonaH NodoPersona.h
 *     @brief Definición de la clase NodoPersona
 *
 *     @defgroup UtilidadesH Utilidades.h
 *     @brief Definición de la clase Utilidades
 *
 *     @defgroup CifradoH Cifrado.h
 *     @brief Definición de la clase Cifrado
 *
 *     @defgroup CodigoQRH CodigoQR.h
 *     @brief Definición de la clase CodigoQR
 * @}
 *
 * @defgroup ArchivosOrigen Archivos de Origen
 * @ingroup MenuPrincipal
 * @brief Archivos de implementación (.cpp)
 * @{
 *     @defgroup MainCPP main.cpp
 *     @brief Implementación del punto de entrada de la aplicación
 *
 *     @defgroup CuentaAhorrosCPP CuentaAhorros.cpp
 *     @brief Implementación de la clase CuentaAhorros
 *
 *     @defgroup CuentaCorrienteCPP CuentaCorriente.cpp
 *     @brief Implementación de la clase CuentaCorriente
 *
 *     @defgroup PersonaCPP Persona.cpp
 *     @brief Implementación de la clase Persona
 *
 *     @defgroup FechaCPP Fecha.cpp
 *     @brief Implementación de la clase Fecha
 *
 *     @defgroup ValidarCPP Validar.cpp
 *     @brief Implementación de la clase Validar
 *
 *     @defgroup BancoCPP Banco.cpp
 *     @brief Implementación de la clase Banco
 *
 *     @defgroup MarquesinaCPP Marquesina.cpp
 *     @brief Implementación de la clase Marquesina
 *
 *     @defgroup UtilidadesCPP Utilidades.cpp
 *     @brief Implementación de la clase Utilidades
 *
 *     @defgroup CifradoCPP Cifrado.cpp
 *     @brief Implementación de la clase Cifrado
 *
 *     @defgroup CodigoQRCPP CodigoQR.cpp
 *     @brief Implementación de la clase CodigoQR
 * @}
 */