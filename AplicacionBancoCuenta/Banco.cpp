#include "Banco.h"
#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include "Utilidades.h"
#include "NodoPersona.h"

// Constructor y destructor de la clase Banco
Banco::Banco() : listaPersonas(nullptr) {} // Constructor

Banco::~Banco() { // Destructor
    // Liberar memoria de la lista enlazada
    NodoPersona* actual = listaPersonas;
    while (actual) {
        NodoPersona* temp = actual;
        actual = actual->siguiente;
        delete temp;
    }
}

// Metodo para agregar una persona con cuenta
void Banco::agregarPersonaConCuenta() {
    // variables locales
    std::string opciones[] = { "Cuenta de Ahorros", "Cuenta Corriente", "Cancelar" };
    int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
    int seleccion = 0;
    // Creamos la persona dinamicamente
    Persona* persona = new Persona();
    int x = 5; // Posicion X inicial del menu
    int y = 5; // Posicion Y inicial del menu
    // Pasamos el puntero al constructor
    NodoPersona* nuevo = new NodoPersona(persona);
    nuevo->siguiente = listaPersonas;
    listaPersonas = nuevo;


    // Menu con cursor
    while (true) {
        system("cls");
        std::cout << "Seleccione el tipo de cuenta a crear para la persona:\n\n";
        for (int i = 0; i < numOpciones; i++) {
            if (i == seleccion)
                std::cout << " > " << opciones[i] << std::endl;
            else
                std::cout << "   " << opciones[i] << std::endl;
        }
        // Limpia cualquier linea sobrante si el menu se reduce
        Utilidades::gotoxy(0, y + numOpciones);

        int tecla = _getch();
        if (tecla == 224) { // Teclas especiales
            tecla = _getch();
            if (tecla == 72) // Flecha arriba
                seleccion = (seleccion - 1 + numOpciones) % numOpciones;
            else if (tecla == 80) // Flecha abajo
                seleccion = (seleccion + 1) % numOpciones;
        }
        else if (tecla == 13) { // ENTER
            break;
        }
    }

    if (seleccion == 2) {
        std::cout << "Operacion cancelada.\n";
        return;
    }


	// Ingreso de datos de la persona con cursor
    if (seleccion == 0 || seleccion == 1) {
        

        std::cout << "\nPresione ENTER para confirmar y crear la cuenta, o ESC para cancelar...\n";
        int tecla = _getch();
        if (tecla == 27) { // ESC
            std::cout << "Operacion cancelada por el usuario.\n";
            return;
        }

		// Selecciona el tipo de cuenta AHORROS
        if (seleccion == 0) {
            // Solicitar cedula antes de ingresar todos los datos
            std::string cedulaTemp;
            while (true) {
                system("cls");
                cout << "----- INGRESE SUS DATOS -----\n";
                std::cout << "Ingrese su cedula (10 digitos): ";
                cedulaTemp.clear();
                int digitos = 0;
                while (true) {
                    char tecla = _getch();
                    if ((tecla >= '0' && tecla <= '9') && digitos < 10) {
                        cedulaTemp += tecla;
                        digitos++;
                        std::cout << tecla;
                    }
                    else if (tecla == 8 && !cedulaTemp.empty()) {
                        cedulaTemp.pop_back();
                        digitos--;
                        std::cout << "\b \b";
                    }
                    else if (tecla == 13) {
                        std::cout << std::endl;
                        break;
                    }
                }
                if (!Validar::ValidarCedula(cedulaTemp)) {
                    std::cout << "Cedula invalida, presione cualquier tecla para volver a ingresar." << std::endl;
                    int teclaCualquiera = _getch();
					(void)teclaCualquiera; // Ignorar la tecla presionada
                    continue;
                }

                // Verificar si la cedula ya existe
                bool existe = false;
                NodoPersona* actual = listaPersonas;
                while (actual) {
                    Persona* p = actual->persona;
                    if (p && p->getCedula() == cedulaTemp) {
                        existe = true;
                        break;
                    }
                    actual = actual->siguiente;
                }
                if (existe) {
                    const char* opciones[2] = { "Si", "No" };
                    int seleccion = 1; // Por defecto "No"
                    int tecla = 0;

                    // Guardar una referencia a la persona existente con la cedula
                    Persona* personaExistente = nullptr;
                    NodoPersona* nodoActual = listaPersonas;
                    while (nodoActual) {
                        if (nodoActual->persona && nodoActual->persona->getCedula() == cedulaTemp) {
                            personaExistente = nodoActual->persona;
                            break;
                        }
                        nodoActual = nodoActual->siguiente;
                    }

                    while (true) {
                        system("cls");
                        std::cout << "La cedula " << cedulaTemp << " ya esta registrada en el sistema.\n";
                        std::cout << "Titular: " << personaExistente->getNombres() << " "
                            << personaExistente->getApellidos() << "\n\n";
                        std::cout << "Desea agregar una nueva cuenta para este titular?\n\n";
                        for (int i = 0; i < 2; ++i) {
                            if (i == seleccion) {
                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                                std::cout << " > " << opciones[i] << " < ";
                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                            }
                            else {
                                std::cout << "   " << opciones[i] << "   ";
                            }
                        }
                        std::cout << "\r";
                        tecla = _getch();
                        if (tecla == 224) {
                            tecla = _getch();
                            if (tecla == 75 && seleccion > 0) --seleccion; // Izquierda
                            else if (tecla == 77 && seleccion < 1) ++seleccion; // Derecha
                        }
                        else if (tecla == 13) break; // Enter
                    }
                    std::cout << std::endl;

                    if (seleccion == 1) { // No
                        std::cout << "Operacion cancelada.\n";
                        // Eliminar el nodo y la persona recien creados para evitar memoria no utilizada
                        NodoPersona* temp = listaPersonas;
                        listaPersonas = listaPersonas->siguiente;
                        delete temp; // Esto liberara tambien la memoria de persona
                        return;
                    }
                    else { // Si - usar la persona existente
                        // Reemplazar la persona recien creada con la persona existente
                        delete persona; // Liberar la memoria de la persona recien creada

                        // Modificar el nodo para que apunte a la persona existente
                        nuevo->persona = personaExistente;

                        // Ahora persona apunta a la persona existente
                        persona = personaExistente;

                        std::cout << "Usando datos de titular existente.\n";
                        system("pause");
                        
                        // Crear directamente la cuenta sin volver a pedir los datos
                        if (seleccion == 0) { // Si estamos en la opción Cuenta de Ahorros
                            persona->crearYAgregarCuentaAhorros();
                            std::cout << "Cuenta de ahorros creada correctamente para el titular existente.\n";
                        }
                        else { // Si estamos en la opción Cuenta Corriente
                            persona->crearYAgregarCuentaCorriente();
                            std::cout << "Cuenta corriente creada correctamente para el titular existente.\n";
                        }
                        system("pause");
                        return; // Salimos del método después de crear la cuenta
                    }
                }
                // Si no existe o selecciona "Si" (usando persona existente), continuar
                break;
            }
            
            // Esta sección se ejecuta solo para personas nuevas (que no existen)
            // Asignar la cedula validada a la persona antes de ingresar el resto de datos
            persona->setCedula(cedulaTemp);
            persona->ingresarDatos(cedulaTemp);
            persona->crearYAgregarCuentaAhorros();
            std::cout << "Persona y cuenta de ahorros creadas correctamente.\n";
        }
		else if (seleccion == 1) { // Selecciona el tipo de cuenta CORRIENTE

            // Solicitar cedula antes de ingresar todos los datos
            std::string cedulaTemp;
            while (true) {
                system("cls");
                cout << "----- INGRESE SUS DATOS -----\n";
                std::cout << "Ingrese su cedula (10 digitos): ";
                cedulaTemp.clear();
                int digitos = 0;
                while (true) {
                    char tecla = _getch();
                    if ((tecla >= '0' && tecla <= '9') && digitos < 10) {
                        cedulaTemp += tecla;
                        digitos++;
                        std::cout << tecla;
                    }
                    else if (tecla == 8 && !cedulaTemp.empty()) {
                        cedulaTemp.pop_back();
                        digitos--;
                        std::cout << "\b \b";
                    }
                    else if (tecla == 13) {
                        std::cout << std::endl;
                        break;
                    }
                }
                if (!Validar::ValidarCedula(cedulaTemp)) {
                    std::cout << "Cedula invalida, presione cualquier tecla para volver a ingresar." << std::endl;
                    int teclaCualquiera = _getch();
                    (void)teclaCualquiera;
                    continue;
                }

                // Verificar si la cedula ya existe
                bool existe = false;
                NodoPersona* actual = listaPersonas;
                while (actual) {
                    Persona* p = actual->persona;
                    if (p && p->getCedula() == cedulaTemp) {
                        existe = true;
                        break;
                    }
                    actual = actual->siguiente;
                }
                if (existe) {
                    const char* opciones[2] = { "Si", "No" };
                    int seleccion = 1; // Por defecto "No"
                    int tecla = 0;

                    // Guardar una referencia a la persona existente con la cedula
                    Persona* personaExistente = nullptr;
                    NodoPersona* nodoActual = listaPersonas;
                    while (nodoActual) {
                        if (nodoActual->persona && nodoActual->persona->getCedula() == cedulaTemp) {
                            personaExistente = nodoActual->persona;
                            break;
                        }
                        nodoActual = nodoActual->siguiente;
                    }

                    while (true) {
                        system("cls");
                        std::cout << "La cedula " << cedulaTemp << " ya esta registrada en el sistema.\n";
                        std::cout << "Titular: " << personaExistente->getNombres() << " "
                            << personaExistente->getApellidos() << "\n\n";
                        std::cout << "Desea crear una nueva cuenta para este titular?\n\n";
                        for (int i = 0; i < 2; ++i) {
                            if (i == seleccion) {
                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                                std::cout << " > " << opciones[i] << " < ";
                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                            }
                            else {
                                std::cout << "   " << opciones[i] << "   ";
                            }
                        }
                        std::cout << "\r";
                        tecla = _getch();
                        if (tecla == 224) {
                            tecla = _getch();
                            if (tecla == 75 && seleccion > 0) --seleccion; // Izquierda
                            else if (tecla == 77 && seleccion < 1) ++seleccion; // Derecha
                        }
                        else if (tecla == 13) break; // Enter
                    }
                    std::cout << std::endl;

                    if (seleccion == 1) { // No
                        std::cout << "Operacion cancelada.\n";
                        // Eliminar el nodo y la persona recien creados para evitar memoria no utilizada
                        NodoPersona* temp = listaPersonas;
                        listaPersonas = listaPersonas->siguiente;
                        delete temp; // Esto liberara tambien la memoria de persona
                        return;
                    }
                    else { // Si - usar la persona existente
                        // Reemplazar la persona recien creada con la persona existente
                        delete persona; // Liberar la memoria de la persona recien creada

                        // Modificar el nodo para que apunte a la persona existente
                        nuevo->persona = personaExistente;

                        // Ahora persona apunta a la persona existente
                        persona = personaExistente;

                        std::cout << "Usando datos de titular existente.\n";
                        system("pause");

                        // Crear directamente la cuenta corriente sin volver a pedir los datos
                        persona->crearYAgregarCuentaCorriente();
                        std::cout << "Cuenta corriente creada correctamente para el titular existente.\n";
                        system("pause");
                        return; // Salimos del método después de crear la cuenta
                    }
                }
                // Si no existe o selecciona "Si", continuar
                break;
            }

            // Esta sección se ejecuta solo para personas nuevas (que no existen)
            // Asignar la cedula validada a la persona antes de ingresar el resto de datos
            persona->setCedula(cedulaTemp);
            persona->ingresarDatos(cedulaTemp);
            persona->crearYAgregarCuentaCorriente();

            std::cout << "Persona y cuenta corriente creadas correctamente.\n";
        }
        system("pause");
    }
}

// Metodo para guardar cuentas en un archivo
void Banco::guardarCuentasEnArchivo(const std::string& nombreArchivo) const {
    std::string rutaEscritorio = obtenerRutaEscritorio();
    std::string rutaCompleta = rutaEscritorio + nombreArchivo + ".bak";
    
    std::ofstream archivo(rutaCompleta, std::ios::out | std::ios::trunc);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo para guardar en: " << rutaCompleta << "\n";
        return;
    }

    // Escribir cabecera para identificar el formato
    archivo << "BANCO_BACKUP_V1.0\n";

    // Contador de personas guardadas
    int contadorPersonas = 0;

    NodoPersona* actual = listaPersonas;
    while (actual) {
        Persona* p = actual->persona;
        if (p && p->isValidInstance()) {
            // Marca de inicio de persona
            archivo << "===PERSONA_INICIO===\n";

            // Guardar datos personales basicos
            archivo << "CEDULA:" << p->getCedula() << "\n";
            archivo << "NOMBRES:" << p->getNombres() << "\n";
            archivo << "APELLIDOS:" << p->getApellidos() << "\n";
            archivo << "FECHA_NACIMIENTO:" << p->getFechaNacimiento() << "\n";
            archivo << "CORREO:" << p->getCorreo() << "\n";
            archivo << "DIRECCION:" << p->getDireccion() << "\n";

            // Guardar cuentas de ahorro
            archivo << "===CUENTAS_AHORRO_INICIO===\n";
            int cuentasAhorro = p->guardarCuentasAhorro(archivo);
            archivo << "TOTAL_CUENTAS_AHORRO:" << cuentasAhorro << "\n";
            archivo << "===CUENTAS_AHORRO_FIN===\n";

            // Guardar cuentas corrientes
            archivo << "===CUENTAS_CORRIENTE_INICIO===\n";
            int cuentasCorriente = p->guardarCuentasCorriente(archivo);
            archivo << "TOTAL_CUENTAS_CORRIENTE:" << cuentasCorriente << "\n";
            archivo << "===CUENTAS_CORRIENTE_FIN===\n";

            // Marca de fin de persona
            archivo << "===PERSONA_FIN===\n\n";

            contadorPersonas++;
        }
        actual = actual->siguiente;
    }

    // Añadir resumen al final
    archivo << "TOTAL_PERSONAS:" << contadorPersonas << "\n";
    archivo << "FIN_BACKUP\n";

    archivo.close();
    std::cout << "Respaldo guardado correctamente en " << rutaCompleta << "\n";
}

void Banco::guardarCuentasEnArchivo() {
    // Obtén la fecha del sistema
    Fecha fechaActual;

    // Verifica si la fecha del sistema fue modificada
    if (fechaActual.getEsFechaSistemaManipulada()) {
        std::cout << "Error: la fecha del sistema parece haber sido manipulada. No se guardará el respaldo.\n";
        return;
    }

    // Forma el nombre del archivo usando la fecha
    std::string fechaFormateada = fechaActual.obtenerFechaFormateada(); // DD/MM/AAAA
    for (auto& c : fechaFormateada) {
        if (c == '/') c = '_';  // Reemplaza '/' por '_'
    }

    // Construye el nombre con extensión
    std::string nombreArchivo = "Respaldo_" + fechaFormateada;
    std::string rutaEscritorio = obtenerRutaEscritorio();
    //std::string rutaCompleta = rutaEscritorio + nombreArchivo + ".bak";

    std::string nombreArchivoUnico = Validar::generarNombreConIndice(rutaEscritorio, fechaFormateada);
    std::string rutaCompleta = rutaEscritorio + nombreArchivoUnico + ".bak";


    std::ofstream archivo(rutaCompleta, std::ios::out | std::ios::trunc);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo para guardar en: " << rutaCompleta << "\n";
        return;
    }

    // Resto de la lógica de guardado (sin cambios)
    archivo << "BANCO_BACKUP_V1.0\n";

    int contadorPersonas = 0;
    NodoPersona* actual = listaPersonas;
    while (actual) {
        Persona* p = actual->persona;
        if (p && p->isValidInstance()) {
            archivo << "===PERSONA_INICIO===\n";
            archivo << "CEDULA:" << p->getCedula() << "\n";
            archivo << "NOMBRES:" << p->getNombres() << "\n";
            archivo << "APELLIDOS:" << p->getApellidos() << "\n";
            archivo << "FECHA_NACIMIENTO:" << p->getFechaNacimiento() << "\n";
            archivo << "CORREO:" << p->getCorreo() << "\n";
            archivo << "DIRECCION:" << p->getDireccion() << "\n";

            archivo << "===CUENTAS_AHORRO_INICIO===\n";
            int cuentasAhorro = p->guardarCuentasAhorro(archivo);
            archivo << "TOTAL_CUENTAS_AHORRO:" << cuentasAhorro << "\n";
            archivo << "===CUENTAS_AHORRO_FIN===\n";

            archivo << "===CUENTAS_CORRIENTE_INICIO===\n";
            int cuentasCorriente = p->guardarCuentasCorriente(archivo);
            archivo << "TOTAL_CUENTAS_CORRIENTE:" << cuentasCorriente << "\n";
            archivo << "===CUENTAS_CORRIENTE_FIN===\n";

            archivo << "===PERSONA_FIN===\n\n";
            contadorPersonas++;
        }
        actual = actual->siguiente;
    }

    archivo << "TOTAL_PERSONAS:" << contadorPersonas << "\n";
    archivo << "FIN_BACKUP\n";

    archivo.close();
    std::cout << "Respaldo guardado correctamente en " << rutaCompleta << "\n";
}

// Metodo para cargar cuentas desde un archivo
void Banco::cargarCuentasDesdeArchivo(const std::string& nombreArchivo) {
    std::string rutaEscritorio = obtenerRutaEscritorio();
    std::string rutaCompleta = rutaEscritorio + nombreArchivo + ".bak";
    
    std::ifstream archivo(rutaCompleta);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo para cargar desde: " << rutaCompleta << "\n";
        return;
    }

    // Limpiar datos existentes
    while (listaPersonas) {
        NodoPersona* temp = listaPersonas;
        listaPersonas = listaPersonas->siguiente;
        delete temp;
    }

    // Variables para el parsing
    std::string linea;
    std::getline(archivo, linea); // Leer cabecera

    if (linea != "BANCO_BACKUP_V1.0") {
        std::cout << "Formato de archivo invalido.\n";
        archivo.close();
        return;
    }

    Persona* personaActual = nullptr;
    bool enPersona = false;
    bool enCuentasAhorro = false;
    bool enCuentasCorriente = false;
    bool enCuentaAhorro = false;
    bool enCuentaCorriente = false;
    int contadorPersonas = 0;

    // Variables para construir cuentas
    int numCuenta = 0;
    int saldo = 0;
    std::string fechaApertura, estado;

    while (std::getline(archivo, linea)) {
        // Control de secciones
        if (linea == "===PERSONA_INICIO===") {
            personaActual = new Persona();
            enPersona = true;
            continue;
        }
        else if (linea == "===PERSONA_FIN===") {
            if (personaActual && enPersona) {
                // Añadir persona a la lista
                NodoPersona* nuevo = new NodoPersona(personaActual);
                nuevo->siguiente = listaPersonas;
                listaPersonas = nuevo;
                contadorPersonas++;
            }
            enPersona = false;
            personaActual = nullptr;
            continue;
        }
        else if (linea == "===CUENTAS_AHORRO_INICIO===") {
            enCuentasAhorro = true;
            continue;
        }
        else if (linea == "===CUENTAS_AHORRO_FIN===") {
            enCuentasAhorro = false;
            continue;
        }
        else if (linea == "===CUENTAS_CORRIENTE_INICIO===") {
            enCuentasCorriente = true;
            continue;
        }
        else if (linea == "===CUENTAS_CORRIENTE_FIN===") {
            enCuentasCorriente = false;
            continue;
        }
        else if (linea == "CUENTA_AHORRO_INICIO") {
            enCuentaAhorro = true;
            // Reiniciar variables para nueva cuenta
            numCuenta = 0;
            saldo = 0;
            fechaApertura = "";
            estado = "";
            continue;
        }
        else if (linea == "CUENTA_AHORRO_FIN" && enCuentaAhorro && personaActual) {
            // Crear y añadir cuenta de ahorro
            CuentaAhorros* nuevaCuenta = new CuentaAhorros(numCuenta, saldo, fechaApertura, estado, 5);
            NodoCuentaAhorros* nodo = new NodoCuentaAhorros(nuevaCuenta);

            // Insertar en la lista de cuentas de ahorro
            nodo->siguiente = personaActual->getListaCuentasAhorros();
            if (personaActual->getListaCuentasAhorros() != nullptr) {
                personaActual->getListaCuentasAhorros()->anterior = nodo;
            }
            personaActual->setCabezaAhorros(nodo);

            enCuentaAhorro = false;
            continue;
        }
        else if (linea == "CUENTA_CORRIENTE_INICIO") {
            enCuentaCorriente = true;
            // Reiniciar variables para nueva cuenta
            numCuenta = 0;
            saldo = 0;
            fechaApertura = "";
            estado = "";
            continue;
        }
        else if (linea == "CUENTA_CORRIENTE_FIN" && enCuentaCorriente && personaActual) {
            // Crear y añadir cuenta corriente
            CuentaCorriente* nuevaCuenta = new CuentaCorriente(numCuenta, saldo, fechaApertura, estado, 0);
            NodoCuentaCorriente* nodo = new NodoCuentaCorriente(nuevaCuenta);

            // Insertar en la lista de cuentas corrientes
            nodo->siguiente = personaActual->getListaCuentasCorriente();
            if (personaActual->getListaCuentasCorriente() != nullptr) {
                personaActual->getListaCuentasCorriente()->anterior = nodo;
            }
            personaActual->setCabezaCorriente(nodo);

            enCuentaCorriente = false;
            continue;
        }

        // Procesar datos
        if (enPersona && personaActual) {
            // Campos de persona
            if (linea.substr(0, 7) == "CEDULA:")
                personaActual->setCedula(linea.substr(7));
            else if (linea.substr(0, 8) == "NOMBRES:")
                personaActual->setNombres(linea.substr(8));
            else if (linea.substr(0, 10) == "APELLIDOS:")
                personaActual->setApellidos(linea.substr(10));
            else if (linea.substr(0, 17) == "FECHA_NACIMIENTO:")
                personaActual->setFechaNacimiento(linea.substr(17));
            else if (linea.substr(0, 7) == "CORREO:")
                personaActual->setCorreo(linea.substr(7));
            else if (linea.substr(0, 10) == "DIRECCION:")
                personaActual->setDireccion(linea.substr(10));
        }

        // Campos de cuenta de ahorro o corriente
        if ((enCuentaAhorro || enCuentaCorriente) && (enCuentasAhorro || enCuentasCorriente)) {
            if (linea.substr(0, 13) == "NUMERO_CUENTA:")
                numCuenta = std::stoi(linea.substr(13));
            else if (linea.substr(0, 6) == "SALDO:")
                saldo = std::stoi(linea.substr(6));
            else if (linea.substr(0, 14) == "FECHA_APERTURA:")
                fechaApertura = linea.substr(14);
            else if (linea.substr(0, 7) == "ESTADO:")
                estado = linea.substr(7);
        }
    }

    archivo.close();
    std::cout << "Se cargaron " << contadorPersonas << " personas desde el archivo.\n";
}

// Metodo para buscar cuentas
void Banco::buscarCuenta() {
    // Verificacion 
    if (listaPersonas == nullptr || reinterpret_cast<uintptr_t>(listaPersonas) > 0xFFFFFFFF00000000) {
		system("cls");
        std::cout << "No hay personas registradas todavia.\n";
        std::cout << "Presione cualquier tecla para continuar";
        int tecla = _getch();
        (void)tecla;
        return;
    }

    // Añadir la opcion "Por cedula" en las opciones de busqueda
    std::string opcionesBusqueda[] = { "Por fecha de creacion", "Por criterio de usuario", "Por numero de cuenta", "Por cedula", "Cancelar" };
    int numOpcionesBusqueda = sizeof(opcionesBusqueda) / sizeof(opcionesBusqueda[0]);
    int seleccionBusqueda = 0;

    // Menu de seleccion de tipo de busqueda
    while (true) {
        system("cls");
        std::cout << "Seleccione el tipo de busqueda:\n\n";
        for (int i = 0; i < numOpcionesBusqueda; i++) {
            if (i == seleccionBusqueda)
                std::cout << " > " << opcionesBusqueda[i] << std::endl;
            else
                std::cout << "   " << opcionesBusqueda[i] << std::endl;
        }
        int tecla = _getch();
        if (tecla == 224) {
            tecla = _getch();
            if (tecla == 72) seleccionBusqueda = (seleccionBusqueda - 1 + numOpcionesBusqueda) % numOpcionesBusqueda;
            else if (tecla == 80) seleccionBusqueda = (seleccionBusqueda + 1) % numOpcionesBusqueda;
        }
        else if (tecla == 13) break;
    }
    if (seleccionBusqueda == 4) return; // Cancelar 

    // Busqueda por fecha de creacion
    if (seleccionBusqueda == 0) {
        // Seleccion de fecha con cursor
        int dia = 1, mes = 1, anio = 2000, campo = 0;
        bool fechaSeleccionada = false;
        SYSTEMTIME st;
        GetLocalTime(&st);
        int anioActual = st.wYear, mesActual = st.wMonth, diaActual = st.wDay;

        auto esBisiesto = [](int anio) {
            return (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0));
        };
        auto diasEnMes = [&](int mes, int anio) {
            switch (mes) {
                case 2: return esBisiesto(anio) ? 29 : 28;
                case 4: case 6: case 9: case 11: return 30;
                default: return 31;
            }
        };

        while (!fechaSeleccionada) {
            system("cls");
            std::cout << "Seleccione la fecha de creacion, usando las flechas del teclado. ENTER para aceptar.\n";
            for (int i = 0; i < 3; ++i) {
                if (i == campo)
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                if (i == 0) std::cout << (dia < 10 ? "0" : "") << dia;
                if (i == 1) std::cout << "/" << (mes < 10 ? "0" : "") << mes;
                if (i == 2) std::cout << "/" << anio;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            std::cout << std::endl;

            int tecla = _getch();
            if (tecla == 224) {
                tecla = _getch();
                switch (tecla) {
                    case 75: if (campo > 0) campo--; break; // Izquierda
                    case 77: if (campo < 2) campo++; break; // Derecha
                    case 72: // Arriba
                        if (campo == 0) { int maxDia = diasEnMes(mes, anio); dia++; if (dia > maxDia) dia = 1; }
                        else if (campo == 1) { mes++; if (mes > 12) mes = 1; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
                        else if (campo == 2) { anio++; if (anio > anioActual) anio = 1900; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
                        break;
                    case 80: // Abajo
                        if (campo == 0) { int maxDia = diasEnMes(mes, anio); dia--; if (dia < 1) dia = maxDia; }
                        else if (campo == 1) { mes--; if (mes < 1) mes = 12; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
                        else if (campo == 2) { anio--; if (anio < 1900) anio = anioActual; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
                        break;
                }
            } else if (tecla == 13) { // Enter
                char buffer[11];
                snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", dia, mes, anio);
                std::string fechaBuscar(buffer);
                // Aqui puedes validar la fecha si lo deseas
                fechaSeleccionada = true;

                // Realizar la busqueda usando la lista enlazada
                NodoPersona* actual = listaPersonas;
                int encontrados = 0;
                while (actual) {
                    actual->persona->buscarPersonaPorFecha(fechaBuscar);
                    actual = actual->siguiente;
                }
                system("pause");
                return;
            }
        }
    }

    // Busqueda por criterio de usuario
    if (seleccionBusqueda == 1) {
        buscarCuentasPorCriterio();
        return;
    }

    // Busqueda por numero de cuenta
    if (seleccionBusqueda == 2) {
        std::string numCuentaBuscar;
        std::cout << "Ingrese el numero de cuenta: ";
        // Ingreso solo numerico con getch
        while (true) {
            char tecla = _getch();
            if (tecla >= '0' && tecla <= '9') {
                numCuentaBuscar += tecla;
                std::cout << tecla;
            } else if (tecla == 8 && !numCuentaBuscar.empty()) { // Backspace
                numCuentaBuscar.pop_back();
                std::cout << "\b \b";
            } else if (tecla == 13 && !numCuentaBuscar.empty()) { // Enter
                std::cout << std::endl;
                break;
            }
            // Ignora cualquier otra tecla
        }

        int cuentasEncontradas = 0;
        NodoPersona* actual = listaPersonas;
        while (actual) {
            if (actual->persona && actual->persona->isValidInstance()) {
                try {
                    // El metodo muestra la informacion y devuelve cuantas cuentas encontro
					system("cls");
                    cuentasEncontradas += actual->persona->buscarPersonaPorCuentas(numCuentaBuscar);
                } catch (...) {
                    std::cerr << "Error al buscar en persona." << std::endl;
                }
            }
            actual = actual->siguiente;
        }
        
        if (cuentasEncontradas == 0) {
            std::cout << "No se encontraron cuentas con el numero: " << numCuentaBuscar << std::endl;
        } else {
            std::cout << "Se encontraron " << cuentasEncontradas << " cuenta(s)." << std::endl;
        }
        
        system("pause");
        return;
    }

    // Nuevo bloque para busqueda por cedula
    if (seleccionBusqueda == 3) { // Por cedula
        std::string cedulaBuscar;
        std::cout << "Ingrese la cedula (10 digitos): ";
        
        // Control de entrada para la cedula (solo numeros y maximo 10 digitos)
        int digitos = 0;
        while (true) {
            char tecla = _getch();
            if ((tecla >= '0' && tecla <= '9') && digitos < 10) {
                cedulaBuscar += tecla;
                digitos++;
                std::cout << tecla;
            }
            else if (tecla == 8 && !cedulaBuscar.empty()) { // Backspace
                cedulaBuscar.pop_back();
                digitos--;
                std::cout << "\b \b";
            }
            else if (tecla == 13 && digitos == 10) { // Enter y cedula completa
                std::cout << std::endl;
                break;
            }
            else if (tecla == 27) { // ESC para cancelar
                std::cout << "\nBusqueda cancelada.\n";
                system("pause");
                return;
            }
        }

        if (!Validar::ValidarCedula(cedulaBuscar)) {
            std::cout << "Cedula invalida.\n";
            system("pause");
            return;
        }

        // Buscar la persona con esa cedula
        bool encontrado = false;
        NodoPersona* actual = listaPersonas;
        while (actual) {
            if (actual->persona && actual->persona->isValidInstance() && 
                actual->persona->getCedula() == cedulaBuscar) {
                
                // Mostrar datos basicos de la persona
                std::cout << "\n----- DATOS DEL TITULAR -----\n";
                std::cout << "Cedula: " << actual->persona->getCedula() << std::endl;
                std::cout << "Nombre: " << actual->persona->getNombres() << " " 
                          << actual->persona->getApellidos() << std::endl;
                std::cout << "Correo: " << actual->persona->getCorreo() << std::endl;
                
                // Mostrar todas las cuentas de esta persona
                std::cout << "\n----- CUENTAS ASOCIADAS -----\n";
                
                // Mostrar cuentas de ahorro
                NodoCuentaAhorros* nodoAhorro = actual->persona->getListaCuentasAhorros();
                int contadorAhorros = 0;
                while (nodoAhorro) {
                    if (nodoAhorro->cuenta) {
                        std::cout << "\nCUENTA DE AHORROS #" << ++contadorAhorros << std::endl;
                        // Pasar false para no limpiar pantalla entre cuentas
                        nodoAhorro->cuenta->mostrarInformacion(cedulaBuscar, false);
                    }
                    nodoAhorro = nodoAhorro->siguiente;
                }
                
                // Mostrar cuentas corrientes
                NodoCuentaCorriente* nodoCorriente = actual->persona->getListaCuentasCorriente();
                int contadorCorrientes = 0;
                while (nodoCorriente) {
                    if (nodoCorriente->cuenta) {
                        std::cout << "\nCUENTA CORRIENTE #" << ++contadorCorrientes << std::endl;
                        // Pasar false para no limpiar pantalla entre cuentas
                        nodoCorriente->cuenta->mostrarInformacion(cedulaBuscar, false);
                    }
                    nodoCorriente = nodoCorriente->siguiente;
                }
                
                if (contadorAhorros == 0 && contadorCorrientes == 0) {
                    std::cout << "Esta persona no tiene cuentas asociadas.\n";
                } else {
                    std::cout << "\nTotal de cuentas: " << (contadorAhorros + contadorCorrientes) << std::endl;
                }
                
                encontrado = true;
                break;
            }
            actual = actual->siguiente;
        }
        
        if (!encontrado) {
            std::cout << "No se encontro ninguna persona con la cedula: " << cedulaBuscar << std::endl;
        }
        
        system("pause");
        return;
    }
}

// Busqueda recursiva por fecha de creacion
void buscarCuentasPorFechaRec(NodoPersona* nodo, const std::string& fecha, int& encontrados) {
    if (!nodo) return;
    nodo->persona->buscarPersonaPorFecha(fecha);
    // Puedes incrementar encontrados dentro de buscarPersonaPorFecha si lo deseas
    buscarCuentasPorFechaRec(nodo->siguiente, fecha, encontrados);
}

// Metodo para buscar cuentas por fecha de creacion
void Banco::buscarCuentasPorFecha(const std::string& fecha) const {
    int encontrados = 0;
    buscarCuentasPorFechaRec(listaPersonas, fecha, encontrados);
    if (encontrados == 0) {
        std::cout << "No se encontraron cuentas con esa fecha en el banco.\n";
    }
}

// Metodo para buscar cuentas por criterio
void Banco::buscarCuentasPorCriterio() {
    std::string criterios[] = {
        "Numero de cuenta",
        "Fecha de apertura",
        "Saldo mayor a",
        "Tipo de cuenta",
        "Cedula de titular", // Nueva opcion
        "Cancelar"
    };

    int numCriterios = sizeof(criterios) / sizeof(criterios[0]);
    int seleccion = 0;

    // Menu de seleccion con cursor
    while (true) {
        system("cls");
        std::cout << "Seleccione el criterio de busqueda de cuentas:\n\n";
        for (int i = 0; i < numCriterios; i++) {
            if (i == seleccion)
                std::cout << " > " << criterios[i] << std::endl;
            else
                std::cout << "   " << criterios[i] << std::endl;
        }
        int tecla = _getch();
        if (tecla == 224) {
            tecla = _getch();
            if (tecla == 72) seleccion = (seleccion - 1 + numCriterios) % numCriterios;
            else if (tecla == 80) seleccion = (seleccion + 1) % numCriterios;
        }
        else if (tecla == 13) break;
    }
    if (seleccion == numCriterios - 1) return; // Cancelar

    std::string valorStr;
    double valorNum = 0.0;

    // Solicitar el valor según el criterio
    if (seleccion == 0) { // Número de cuenta
        std::cout << "Ingrese el número de cuenta: ";
        valorStr.clear();
        
        // Control de entrada numérica similar a otros lugares
        while (true) {
            char tecla = _getch();
            if (tecla >= '0' && tecla <= '9') {
                valorStr += tecla;
                std::cout << tecla;
            } else if (tecla == 8 && !valorStr.empty()) { // Backspace
                valorStr.pop_back();
                std::cout << "\b \b";
            } else if (tecla == 13 && !valorStr.empty()) { // Enter
                std::cout << std::endl;
                break;
            } else if (tecla == 27) { // ESC para cancelar
                return;
            }
        }
    }
    else if (seleccion == 1) { // Fecha de apertura
        // Implementar selección de fecha con cursor como en otras partes del código
        int dia = 1, mes = 1, anio = 2000, campo = 0;
        bool fechaSeleccionada = false;
        SYSTEMTIME st;
        GetLocalTime(&st);
        int anioActual = st.wYear, mesActual = st.wMonth, diaActual = st.wDay;

        auto esBisiesto = [](int anio) {
            return (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0));
        };
        auto diasEnMes = [&](int mes, int anio) {
            switch (mes) {
                case 2: return esBisiesto(anio) ? 29 : 28;
                case 4: case 6: case 9: case 11: return 30;
                default: return 31;
            }
        };

        while (!fechaSeleccionada) {
            system("cls");
            std::cout << "Seleccione la fecha de apertura, usando las flechas del teclado. ENTER para aceptar.\n";
            for (int i = 0; i < 3; ++i) {
                if (i == campo)
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                if (i == 0) std::cout << (dia < 10 ? "0" : "") << dia;
                if (i == 1) std::cout << "/" << (mes < 10 ? "0" : "") << mes;
                if (i == 2) std::cout << "/" << anio;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            std::cout << std::endl;

            int tecla = _getch();
            if (tecla == 224) {
                tecla = _getch();
                switch (tecla) {
                    case 75: if (campo > 0) campo--; break; // Izquierda
                    case 77: if (campo < 2) campo++; break; // Derecha
                    case 72: // Arriba
                        if (campo == 0) { int maxDia = diasEnMes(mes, anio); dia++; if (dia > maxDia) dia = 1; }
                        else if (campo == 1) { mes++; if (mes > 12) mes = 1; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
                        else if (campo == 2) { anio++; if (anio > anioActual) anio = 1900; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
                        break;
                    case 80: // Abajo
                        if (campo == 0) { int maxDia = diasEnMes(mes, anio); dia--; if (dia < 1) dia = maxDia; }
                        else if (campo == 1) { mes--; if (mes < 1) mes = 12; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
                        else if (campo == 2) { anio--; if (anio < 1900) anio = anioActual; int maxDia = diasEnMes(mes, anio); if (dia > maxDia) dia = maxDia; }
                        break;
                }
            } else if (tecla == 13) { // Enter
                char buffer[11];
                snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", dia, mes, anio);
                valorStr = buffer;
                fechaSeleccionada = true;
            } else if (tecla == 27) { // ESC
                return;
            }
        }
    }
    else if (seleccion == 2) { // Saldo mayor a
        std::cout << "Ingrese el saldo mínimo: ";
        
        // Implementar validación similar a otras partes
        std::string entrada;
        bool tienePunto = false;
        int digitosDecimales = 0;
        
        while (true) {
            char tecla = _getch();
            
            // ESC para cancelar
            if (tecla == 27) {
                return;
            }
            
            // ENTER
            if (tecla == 13) {
                if (!entrada.empty()) {
                    if (Validar::ValidarNumeroConDosDecimales(entrada)) {
                        try {
                            valorNum = std::stod(entrada);
                            if (valorNum >= 0) {
                                std::cout << std::endl;
                                break;
                            }
                        }
                        catch (...) {}
                    }
                }
                std::cout << "\nFormato inválido. Ingrese nuevamente: ";
                entrada.clear();
                tienePunto = false;
                digitosDecimales = 0;
                continue;
            }
            
            // BACKSPACE
            if (tecla == 8 && !entrada.empty()) {
                if (entrada.back() == '.') {
                    tienePunto = false;
                }
                else if (tienePunto && digitosDecimales > 0) {
                    --digitosDecimales;
                }
                entrada.pop_back();
                std::cout << "\b \b";
                continue;
            }
            
            // Ignoramos teclas especiales
            if (tecla == 0 || tecla == -32) {
                int teclaEspecial = _getch();
                (void)teclaEspecial;
                continue;
            }
            
            // Dígitos
            if (isdigit(tecla)) {
                if (tienePunto && digitosDecimales == 2) continue;
                if (tienePunto) ++digitosDecimales;
                entrada += tecla;
                std::cout << tecla;
            }
            // Punto decimal
            else if (tecla == '.' && !tienePunto && !entrada.empty()) {
                tienePunto = true;
                entrada += tecla;
                std::cout << tecla;
            }
        }
    }
    
    // El resto de criterios está bien implementado

    // A continuación, modificamos cómo se muestran los resultados de búsqueda:
    
    int totalCuentasEncontradas = 0;
    
    if (seleccion == 4) { // Si es búsqueda por cédula, ya está implementado correctamente
        bool encontrado = false;
        NodoPersona* actual = listaPersonas;
        
        while (actual) {
            if (actual->persona && actual->persona->isValidInstance() &&
                actual->persona->getCedula() == valorStr) {
                
                // Mostrar información básica de la persona
                std::cout << "\n===== DATOS DEL TITULAR =====\n";
                std::cout << "Cédula: " << actual->persona->getCedula() << std::endl;
                std::cout << "Nombre: " << actual->persona->getNombres() << " " 
                          << actual->persona->getApellidos() << std::endl;
                std::cout << "Fecha de nacimiento: " << actual->persona->getFechaNacimiento() << std::endl;
                std::cout << "Correo: " << actual->persona->getCorreo() << std::endl;
                std::cout << "Dirección: " << actual->persona->getDireccion() << std::endl;
                
                // Mostrar todas sus cuentas
                std::cout << "\n===== CUENTAS DE AHORRO =====\n";
                NodoCuentaAhorros* ahorros = actual->persona->getListaCuentasAhorros();
                int contadorAhorros = 0;
                if (!ahorros) {
                    std::cout << "  No tiene cuentas de ahorro.\n";
                }
                while (ahorros) {
                    if (ahorros->cuenta) {
                        std::cout << "\nCUENTA DE AHORROS #" << ++contadorAhorros << std::endl;
                        ahorros->cuenta->mostrarInformacion(actual->persona->getCedula());
                        totalCuentasEncontradas++;
                    }
                    ahorros = ahorros->siguiente;
                }
                
                std::cout << "\n===== CUENTAS CORRIENTES =====\n";
                NodoCuentaCorriente* corrientes = actual->persona->getListaCuentasCorriente();
                int contadorCorrientes = 0;
                if (!corrientes) {
                    std::cout << "  No tiene cuentas corrientes.\n";
                }
                while (corrientes) {
                    if (corrientes->cuenta) {
                        std::cout << "\nCUENTA CORRIENTE #" << ++contadorCorrientes << std::endl;
                        corrientes->cuenta->mostrarInformacion(actual->persona->getCedula());
                        totalCuentasEncontradas++;
                    }
                    corrientes = corrientes->siguiente;
                }
                
                encontrado = true;
                break;
            }
            actual = actual->siguiente;
        }
        
        if (!encontrado) {
            std::cout << "No se encontró ninguna persona con la cédula: " << valorStr << std::endl;
        } else {
            std::cout << "\nTotal de cuentas encontradas: " << totalCuentasEncontradas << std::endl;
        }
    }
    else {
        // Para otros criterios, modificamos el método buscarPersonaPorCriterio en la clase Persona
        // para que muestre los datos del titular junto con cada cuenta.
        
        // Recorremos todas las personas y buscamos según criterio
        NodoPersona* actual = listaPersonas;
        while (actual) {
            if (actual->persona && actual->persona->isValidInstance()) {
                // Contar las cuentas encontradas por persona
                int cuentasEncontradas = actual->persona->buscarPersonaPorCriterio(
                    criterios[seleccion], valorStr, valorNum);
                totalCuentasEncontradas += cuentasEncontradas;
            }
            actual = actual->siguiente;
        }
        
        if (totalCuentasEncontradas == 0) {
            std::cout << "\nNo se encontraron cuentas que cumplan con el criterio seleccionado." << std::endl;
        } else {
            std::cout << "\nTotal de cuentas encontradas: " << totalCuentasEncontradas << std::endl;
        }
    }
    
    system("pause");
}

// Metodo para realizar transferencias entre cuentas
void Banco::realizarTransferencia() {
    // Verificar que existan personas con cuentas
    if (!listaPersonas) {
		system("cls");
        std::cout << "No hay cuentas registradas en el sistema.\n";
        system("pause");
        return;
    }

    // Variables para almacenar referencias a las cuentas
    CuentaAhorros* cuentaAhorrosOrigen = nullptr;
    CuentaCorriente* cuentaCorrienteOrigen = nullptr;
    CuentaAhorros* cuentaAhorrosDestino = nullptr;
    CuentaCorriente* cuentaCorrienteDestino = nullptr;
    Persona* personaOrigen = nullptr;
    Persona* personaDestino = nullptr;
    std::string numCuentaOrigen, numCuentaDestino;
    bool esAhorrosOrigen = false, esAhorrosDestino = false;
    bool cuentaOrigenEncontrada = false, cuentaDestinoEncontrada = false;

    // 1. Obtener la cuenta de origen
    while (!cuentaOrigenEncontrada) {
        system("cls");
        std::cout << "=== TRANSFERENCIA BANCARIA ===\n\n";
        std::cout << "Ingrese el numero de cuenta de origen (o ESC para cancelar): ";

        numCuentaOrigen.clear();
        while (true) {
            char tecla = _getch();
            if (tecla == 27) { // ESC
                std::cout << "\nOperacion cancelada por el usuario.\n";
                system("pause");
                return;
            }
            else if (tecla >= '0' && tecla <= '9') {
                numCuentaOrigen += tecla;
                std::cout << tecla;
            }
            else if (tecla == 8 && !numCuentaOrigen.empty()) { // Backspace
                numCuentaOrigen.pop_back();
                std::cout << "\b \b";
            }
            else if (tecla == 13 && !numCuentaOrigen.empty()) { // Enter
                std::cout << std::endl;
                break;
            }
        }

        // Buscar la cuenta de origen en todas las personas
        NodoPersona* actual = listaPersonas;
        while (actual && !cuentaOrigenEncontrada) {
            if (!actual->persona) {
                actual = actual->siguiente;
                continue;
            }

            // Buscar en cuentas de ahorro
            NodoCuentaAhorros* nodoAhorros = actual->persona->getListaCuentasAhorros();
            while (nodoAhorros && !cuentaOrigenEncontrada) {
                if (nodoAhorros->cuenta && std::to_string(nodoAhorros->cuenta->getNumeroCuenta()) == numCuentaOrigen) {
                    cuentaAhorrosOrigen = nodoAhorros->cuenta;
                    personaOrigen = actual->persona;
                    cuentaOrigenEncontrada = true;
                    esAhorrosOrigen = true;
                    break;
                }
                nodoAhorros = nodoAhorros->siguiente;
            }

            // Si no se encontro en ahorro, buscar en corriente
            if (!cuentaOrigenEncontrada) {
                NodoCuentaCorriente* nodoCorriente = actual->persona->getListaCuentasCorriente();
                while (nodoCorriente && !cuentaOrigenEncontrada) {
                    if (nodoCorriente->cuenta && std::to_string(nodoCorriente->cuenta->getNumeroCuenta()) == numCuentaOrigen) {
                        cuentaCorrienteOrigen = nodoCorriente->cuenta;
                        personaOrigen = actual->persona;
                        cuentaOrigenEncontrada = true;
                        esAhorrosOrigen = false;
                        break;
                    }
                    nodoCorriente = nodoCorriente->siguiente;
                }
            }

            actual = actual->siguiente;
        }

        if (!cuentaOrigenEncontrada) {
            // Configuracion de opciones para el menu de confirmacion
            const char* opciones[] = { "Si", "No" };
            int seleccion = 0; // Por defecto seleccionar "Si"
            int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
            bool continuar = true;
            
            system("cls");
            std::cout << "Cuenta de origen no encontrada.\n";
            std::cout << "Desea intentar con otro numero?\n\n";
            
            // Obtener handle para manipular la consola
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            int posY = csbi.dwCursorPosition.Y;
            
            // Bucle de manejo del menu
            while (continuar) {
                // Mostrar las opciones
                for (int i = 0; i < numOpciones; i++) {
                    // Posicionar el cursor
                    COORD pos = { 5, static_cast<SHORT>(posY + i) };
                    SetConsoleCursorPosition(hConsole, pos);
                    
                    // Resaltar la opcion seleccionada
                    if (i == seleccion) {
                        SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                        std::cout << " > " << opciones[i] << " < ";
                    } else {
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        std::cout << "   " << opciones[i] << "   ";
                    }
                }
                
                // Restaurar atributos por defecto
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                
                // Capturar tecla
                int tecla = _getch();
                
                // Procesar la tecla
                if (tecla == 224 || tecla == 0) { // Teclas especiales
                    tecla = _getch();
                    if (tecla == 72) { // Flecha arriba
                        seleccion = (seleccion > 0) ? seleccion - 1 : 0;
                    }
                    else if (tecla == 80) { // Flecha abajo
                        seleccion = (seleccion < numOpciones - 1) ? seleccion + 1 : numOpciones - 1;
                    }
                }
                else if (tecla == 13) { // Enter
                    continuar = false;
                }
                else if (tecla == 27) { // ESC - cancelar
                    seleccion = 1; // Seleccionar "No"
                    continuar = false;
                }
            }
            
            // Procesar la seleccion
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            std::cout << "\n\n";
            
            if (seleccion == 1) { // "No" seleccionado
                std::cout << "Operacion cancelada.\n";
                system("pause");
                return;
            }
            // Si selecciono "Si", continuara el flujo del programa
        }
    }

    // Mostrar informacion de la cuenta origen
    system("cls");
    std::cout << "=== TRANSFERENCIA BANCARIA ===\n\n";
    std::cout << "CUENTA ORIGEN:\n";
    if (esAhorrosOrigen) {
        cuentaAhorrosOrigen->mostrarInformacion();
    }
    else {
        cuentaCorrienteOrigen->mostrarInformacion();
    }
    std::cout << "\nTitular: " << personaOrigen->getNombres() << " " << personaOrigen->getApellidos() << "\n\n";

    // 2. Obtener la cuenta de destino
    while (!cuentaDestinoEncontrada) {
        std::cout << "Ingrese el numero de cuenta de destino (o ESC para cancelar): ";

        numCuentaDestino.clear();
        while (true) {
            char tecla = _getch();
            if (tecla == 27) { // ESC
                std::cout << "\nOperacion cancelada por el usuario.\n";
                system("pause");
                return;
            }
            else if (tecla >= '0' && tecla <= '9') {
                numCuentaDestino += tecla;
                std::cout << tecla;
            }
            else if (tecla == 8 && !numCuentaDestino.empty()) { // Backspace
                numCuentaDestino.pop_back();
                std::cout << "\b \b";
            }
            else if (tecla == 13 && !numCuentaDestino.empty()) { // Enter
                std::cout << std::endl;
                break;
            }
        }

        // No permitir transferir a la misma cuenta
        if (numCuentaDestino == numCuentaOrigen) {
            std::cout << "No se puede transferir a la misma cuenta. Intente con otra.\n";
            continue;
        }

        // Buscar la cuenta de destino en todas las personas
        NodoPersona* actual = listaPersonas;
        while (actual && !cuentaDestinoEncontrada) {
            if (!actual->persona) {
                actual = actual->siguiente;
                continue;
            }

            // Buscar en cuentas de ahorro
            NodoCuentaAhorros* nodoAhorros = actual->persona->getListaCuentasAhorros();
            while (nodoAhorros && !cuentaDestinoEncontrada) {
                if (nodoAhorros->cuenta && std::to_string(nodoAhorros->cuenta->getNumeroCuenta()) == numCuentaDestino) {
                    cuentaAhorrosDestino = nodoAhorros->cuenta;
                    personaDestino = actual->persona;
                    cuentaDestinoEncontrada = true;
                    esAhorrosDestino = true;
                    break;
                }
                nodoAhorros = nodoAhorros->siguiente;
            }

            // Si no se encontro en ahorro, buscar en corriente
            if (!cuentaDestinoEncontrada) {
                NodoCuentaCorriente* nodoCorriente = actual->persona->getListaCuentasCorriente();
                while (nodoCorriente && !cuentaDestinoEncontrada) {
                    if (nodoCorriente->cuenta && std::to_string(nodoCorriente->cuenta->getNumeroCuenta()) == numCuentaDestino) {
                        cuentaCorrienteDestino = nodoCorriente->cuenta;
                        personaDestino = actual->persona;
                        cuentaDestinoEncontrada = true;
                        esAhorrosDestino = false;
                        break;
                    }
                    nodoCorriente = nodoCorriente->siguiente;
                }
            }

            actual = actual->siguiente;
        }

        if (!cuentaDestinoEncontrada) {
            std::cout << "Cuenta de destino no encontrada. Desea intentar con otro numero? (S/N): ";
            char respuesta = _getch();
            if (respuesta != 'S' && respuesta != 's') {
                std::cout << "\nOperacion cancelada.\n";
                system("pause");
                return;
            }
        }
    }

    // Mostrar informacion de la cuenta destino
    system("cls");
    std::cout << "=== TRANSFERENCIA BANCARIA ===\n\n";
    std::cout << "CUENTA ORIGEN:\n";
    if (esAhorrosOrigen) {
        cuentaAhorrosOrigen->mostrarInformacion();
    }
    else {
        cuentaCorrienteOrigen->mostrarInformacion();
    }
    std::cout << "\nTitular: " << personaOrigen->getNombres() << " " << personaOrigen->getApellidos() << "\n\n";

    std::cout << "CUENTA DESTINO:\n";
    if (esAhorrosDestino) {
        cuentaAhorrosDestino->mostrarInformacion();
    }
    else {
        cuentaCorrienteDestino->mostrarInformacion();
    }
    std::cout << "\nTitular: " << personaDestino->getNombres() << " " << personaDestino->getApellidos() << "\n\n";

    // 3. Solicitar monto a transferir
    int montoEnCentavos = 0;
    std::string entrada;
    bool tienePunto = false;
    int digitosDecimales = 0;

    std::cout << "Ingrese el monto a transferir (ejemplo: 1000.50): ";
    while (true) {
        char tecla = _getch();

        // ESC para cancelar
        if (tecla == 27) {
            std::cout << "\nOperacion cancelada por el usuario.\n";
            system("pause");
            return;
        }

        // Detecta Ctrl+V (para pegar)
        if (tecla == 22) {
            std::string pegado = Validar::leerDesdePortapapeles();
            if (Validar::ValidarNumeroConDosDecimales(pegado)) {
                try {
                    double valor = std::stod(pegado);
                    if (valor > 0) {
                        entrada = pegado;
                        std::cout << "\rIngrese el monto a transferir (ejemplo: 1000.50): " << entrada;
                        std::cout << std::endl;
                        montoEnCentavos = static_cast<int>(valor * 100);
                        break;
                    }
                }
                catch (...) {}
            }
            std::cout << "\nFormato invalido. Ingrese nuevamente (ejemplo: 1000.50): ";
            entrada.clear();
            tienePunto = false;
            digitosDecimales = 0;
            continue;
        }

        // ENTER
        if (tecla == 13) {
            if (!entrada.empty()) {
                if (Validar::ValidarNumeroConDosDecimales(entrada)) {
                    try {
                        double valor = std::stod(entrada);
                        if (valor > 0) {
                            std::cout << std::endl;
                            montoEnCentavos = static_cast<int>(valor * 100);
                            break;
                        }
                    }
                    catch (...) {}
                }
            }
            std::cout << "\nFormato invalido o monto menor o igual a cero. \nIngrese nuevamente (ejemplo: 1000.50): ";
            entrada.clear();
            tienePunto = false;
            digitosDecimales = 0;
            continue;
        }

        // BACKSPACE
        if (tecla == 8 && !entrada.empty()) {
            if (entrada.back() == '.') {
                tienePunto = false;
            }
            else if (tienePunto && digitosDecimales > 0) {
                --digitosDecimales;
            }
            entrada.pop_back();
            std::cout << "\rIngrese el monto a transferir (ejemplo: 1000.50): " << std::string(40, ' ') << "\r";
            std::cout << "Ingrese el monto a transferir (ejemplo: 1000.50): " << entrada;
            continue;
        }

        // Ignora teclas especiales (como flechas, etc.)
        if (tecla == 0 || tecla == -32) {
            int teclaEspecial = _getch();
            (void)teclaEspecial;
            continue;
        }

        // Digitos
        if (isdigit(tecla)) {
            if (tienePunto && digitosDecimales == 2) continue;
            if (tienePunto) ++digitosDecimales;
            entrada += tecla;
            std::cout << tecla;
        }
        // Punto decimal
        else if (tecla == '.' && !tienePunto && !entrada.empty()) {
            tienePunto = true;
            entrada += tecla;
            std::cout << tecla;
        }
    }

    // 4. Verificar fondos suficientes
    int saldoOrigen = esAhorrosOrigen ? cuentaAhorrosOrigen->consultarSaldo() : cuentaCorrienteOrigen->consultarSaldo();

    if (montoEnCentavos > saldoOrigen) {
        std::cout << "Fondos insuficientes para realizar la transferencia.\n";
        system("pause");
        return;
    }

    // 5. Confirmar transferencia
    std::cout << "Confirmar transferencia de $"
        << (esAhorrosOrigen ? cuentaAhorrosOrigen->formatearConComas(montoEnCentavos) :
            formatearConComas(montoEnCentavos))
        << "? (S/N): ";

    char confirmacion = _getch();
    if (confirmacion != 'S' && confirmacion != 's') {
        std::cout << "\nOperacion cancelada.\n";
        system("pause");
        return;
    }

    // 6. Realizar la transferencia
    if (esAhorrosOrigen) {
        cuentaAhorrosOrigen->retirar(montoEnCentavos);
    }
    else {
        cuentaCorrienteOrigen->retirar(montoEnCentavos);
    }

    if (esAhorrosDestino) {
        cuentaAhorrosDestino->depositar(montoEnCentavos);
    }
    else {
        cuentaCorrienteDestino->depositar(montoEnCentavos);
    }

    // 7. Mostrar confirmacion
    std::cout << "\nTransferencia realizada con exito!\n\n";
    std::cout << "NUEVO SALDO CUENTA ORIGEN: $"
        << (esAhorrosOrigen ? cuentaAhorrosOrigen->formatearConComas(cuentaAhorrosOrigen->consultarSaldo()) :
            formatearConComas(cuentaCorrienteOrigen->consultarSaldo()))
        << "\n";

    system("pause");
}

// Metodo helper para formatear valores con comas (para cuentas corrientes)
std::string Banco::formatearConComas(int valorEnCentavos) const {
    double valorReal = valorEnCentavos / 100.0;
    std::ostringstream oss;
    oss.imbue(std::locale(""));
    oss << std::fixed << std::setprecision(2) << valorReal;
    return oss.str();
}

// Implementacion de la funcion para obtener la ruta del escritorio
std::string Banco::obtenerRutaEscritorio() const {
    PWSTR path = NULL;
    std::string rutaEscritorio = "";

    // Obtener la ruta del escritorio
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &path))) {
        // Convertir de PWSTR a std::string
        _bstr_t b(path);
        rutaEscritorio = (char*)b;

        // Liberar la memoria asignada por SHGetKnownFolderPath
        CoTaskMemFree(path);
    }
    else {
        // Si falla, usar una ubicacion alternativa como respaldo
        rutaEscritorio = "."; // Directorio actual como respaldo
    }

    // Asegurar que la ruta termine con una barra diagonal
    if (!rutaEscritorio.empty() && rutaEscritorio.back() != '\\') {
        rutaEscritorio += '\\';
    }

    // Crear una subcarpeta especifica para la aplicacion
    rutaEscritorio += "BancoApp\\";

    // Asegurar que la carpeta existe
    std::string comando = "if not exist \"" + rutaEscritorio + "\" mkdir \"" + rutaEscritorio + "\"";
    system(comando.c_str());

    return rutaEscritorio;
}