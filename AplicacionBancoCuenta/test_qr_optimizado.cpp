// Test para la clase GeneradorQRBanco optimizada
// Genera un código QR válido siguiendo ISO/IEC 18004
// Versión 2 (25x25) con nivel de corrección L

#include <iostream>
#include "GeneradorQRBanco.h"
#include "Persona.h"

int main() {
    std::cout << "=== TEST DE GENERADOR QR BANCARIO OPTIMIZADO ===" << std::endl;
    std::cout << "Estándar: ISO/IEC 18004" << std::endl;
    std::cout << "Versión: 2 (25x25 módulos)" << std::endl;
    std::cout << "Nivel de corrección: L" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    try {
        // Crear instancia del generador QR
        GeneradorQRBanco qr("URIEL ALEJANDRO ANDRADE HERMOSA", "2100000014");
        
        // Generar el código QR
        std::cout << "\n1. Generando código QR..." << std::endl;
        qr.generar();
        
        // Imprimir en consola
        std::cout << "\n2. Imprimiendo código QR en consola:" << std::endl;
        qr.imprimir();
        
        // Verificar compatibilidad
        std::cout << "\n3. Verificando compatibilidad:" << std::endl;
        if (qr.verificarCompatibilidadQR()) {
            std::cout << "✓ Código QR compatible con lectores móviles" << std::endl;
        } else {
            std::cout << "✗ Código QR no compatible" << std::endl;
        }
        
        // Guardar archivos
        std::cout << "\n4. Guardando archivos:" << std::endl;
        qr.guardarInformacionCuenta("qr_test_info.txt");
        qr.guardarComoSVG("qr_test.svg");
        
        std::cout << "\n=== TEST COMPLETADO EXITOSAMENTE ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error durante el test: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
