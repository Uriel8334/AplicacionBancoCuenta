
/**
 * @file main.cpp
 * @brief Sistema de gestión bancaria con interfaz de consola
 *
 * Este archivo implementa la aplicación principal del sistema bancario, proporcionando
 * un menú interactivo para gestionar cuentas, realizar transferencias, buscar información,
 * y generar reportes. Refactorizado aplicando principios SOLID y código limpio.
 *
 * @author Uriel Andrade
 * @author Kerly Chuqui
 * @author Abner Proano
 */
#define _CRT_SECURE_NO_WARNINGS

#include "AplicacionPrincipal.h"
#include "Marquesina.h"
#include <atomic>
#include <mutex>

 /** @brief Puntero global a la marquesina utilizada en la aplicación */
Marquesina* marquesinaGlobal = nullptr;

/** @brief Bandera atómica para coordinar actualizaciones entre menú y marquesina */
std::atomic<bool> actualizandoMenu(false);

/** @brief Mutex para sincronización entre hilos */
std::mutex mtxActualizacion;

/**
 * @brief Pausa temporalmente la marquesina durante operaciones críticas
 */
static void pausarMarquesina() {
	if (marquesinaGlobal) {
		marquesinaGlobal->pausar();
	}
}

/**
 * @brief Reanuda la marquesina después de una operación crítica
 */
static void reanudarMarquesina() {
	if (marquesinaGlobal) {
		marquesinaGlobal->reanudar();
	}
}

/**
 * @brief Función principal del programa
 *
 * Punto de entrada principal que utiliza el patrón Application Controller
 * para coordinar toda la aplicación del sistema bancario.
 *
 * @return Código de salida del programa
 */
int main() {
	try {
		AplicacionPrincipal aplicacion;
		return aplicacion.ejecutar();
	}
	catch (const std::exception& e) {
		std::cerr << "Error crítico en la aplicación: " << e.what() << std::endl;
		system("pause");
		return -1;
	}
	catch (...) {
		std::cerr << "Error desconocido en la aplicación" << std::endl;
		system("pause");
		return -1;
	}
}

