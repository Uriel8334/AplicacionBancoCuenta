/* 
 * Ejemplo de uso de GeneradorQRBanco optimizado
 * Genera un código QR válido siguiendo ISO/IEC 18004
 * Versión 2 (25x25) con nivel de corrección L
 * 
 * Compilar con: g++ -std=c++11 ejemplo_uso_qr.cpp GeneradorQRBanco.cpp Utilidades.cpp Persona.cpp -o ejemplo_qr
 */

#include <iostream>
#include <string>
#include "GeneradorQRBanco.h"

void mostrarEjemploBasico() {
    std::cout << "\n=== EJEMPLO 1: USO BÁSICO ===" << std::endl;
    
    // Crear instancia del generador QR
    GeneradorQRBanco qr("URIEL ALEJANDRO ANDRADE HERMOSA", "2100000014");
    
    // Generar el código QR
    std::cout << "\nGenerando código QR..." << std::endl;
    qr.generar();
    
    // Imprimir en consola
    qr.imprimir();
    
    // Guardar archivos
    qr.guardarInformacionCuenta("ejemplo_info.txt");
    qr.guardarComoSVG("ejemplo_qr.svg");
    
    std::cout << "\nArchivos generados:" << std::endl;
    std::cout << "- ejemplo_info.txt (información del QR)" << std::endl;
    std::cout << "- ejemplo_qr.svg (código QR en formato SVG)" << std::endl;
}

void mostrarEjemploValidacion() {
    std::cout << "\n=== EJEMPLO 2: VALIDACIÓN DE DATOS ===" << std::endl;
    
    // Probar con datos válidos
    std::string nombreValido = "MARIA GONZALEZ RODRIGUEZ";
    std::string cuentaValida = "1234567890";
    
    std::cout << "\nValidando datos:" << std::endl;
    std::cout << "Nombre: " << nombreValido << " -> " 
              << (GeneradorQRBanco::esNombreValido(nombreValido) ? "✓ VÁLIDO" : "✗ INVÁLIDO") << std::endl;
    std::cout << "Cuenta: " << cuentaValida << " -> " 
              << (GeneradorQRBanco::esNumeroCuentaValido(cuentaValida) ? "✓ VÁLIDO" : "✗ INVÁLIDO") << std::endl;
    
    // Probar con datos inválidos
    std::string nombreInvalido = "NOMBRE_CON_SIMBOLOS@#$";
    std::string cuentaInvalida = "123ABC";
    
    std::cout << "\nValidando datos incorrectos:" << std::endl;
    std::cout << "Nombre: " << nombreInvalido << " -> " 
              << (GeneradorQRBanco::esNombreValido(nombreInvalido) ? "✓ VÁLIDO" : "✗ INVÁLIDO") << std::endl;
    std::cout << "Cuenta: " << cuentaInvalida << " -> " 
              << (GeneradorQRBanco::esNumeroCuentaValido(cuentaInvalida) ? "✓ VÁLIDO" : "✗ INVÁLIDO") << std::endl;
}

void mostrarEjemploCompatibilidad() {
    std::cout << "\n=== EJEMPLO 3: VERIFICACIÓN DE COMPATIBILIDAD ===" << std::endl;
    
    GeneradorQRBanco qr("JUAN CARLOS PEREZ MARTINEZ", "9876543210");
    
    // Generar QR compatible
    std::cout << "\nGenerando QR compatible..." << std::endl;
    if (qr.generarCodigoQRCompatible()) {
        std::cout << "✓ QR compatible generado exitosamente" << std::endl;
        
        // Mostrar contenido
        std::cout << "\nContenido del QR:" << std::endl;
        std::cout << "\"" << qr.obtenerContenidoQR() << "\"" << std::endl;
        
        // Imprimir QR
        qr.imprimir();
    } else {
        std::cout << "✗ No se pudo generar QR compatible" << std::endl;
    }
}

void mostrarCaracteristicasTecnicas() {
    std::cout << "\n=== CARACTERÍSTICAS TÉCNICAS ===" << std::endl;
    std::cout << "• Estándar: ISO/IEC 18004" << std::endl;
    std::cout << "• Versión: 2 (25x25 módulos)" << std::endl;
    std::cout << "• Nivel de corrección: L (7%)" << std::endl;
    std::cout << "• Modo de codificación: Byte" << std::endl;
    std::cout << "• Capacidad de datos: 44 bytes" << std::endl;
    std::cout << "• Bytes de corrección: 7" << std::endl;
    std::cout << "• Patrón de enmascaramiento: 1 (filas pares)" << std::endl;
    std::cout << "• Compatible con lectores móviles: ✓" << std::endl;
    std::cout << "• Formato de salida: SVG escalable" << std::endl;
    
    std::cout << "\n• Estructura del código QR:" << std::endl;
    std::cout << "  - 3 Patrones de posición (esquinas)" << std::endl;
    std::cout << "  - 1 Patrón de alineamiento (centro)" << std::endl;
    std::cout << "  - Patrones de tiempo (filas/columnas 6)" << std::endl;
    std::cout << "  - Información de formato (alrededor de patrones)" << std::endl;
    std::cout << "  - Zona de datos con corrección Reed-Solomon" << std::endl;
    std::cout << "  - Enmascaramiento XOR para optimización" << std::endl;
}

int main() {
    std::cout << "=== GENERADOR QR BANCARIO OPTIMIZADO ===" << std::endl;
    std::cout << "Implementación completa según ISO/IEC 18004" << std::endl;
    std::cout << "Versión 2 (25x25) con nivel de corrección L" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    try {
        // Mostrar características técnicas
        mostrarCaracteristicasTecnicas();
        
        // Ejemplo básico de uso
        mostrarEjemploBasico();
        
        // Ejemplo de validación
        mostrarEjemploValidacion();
        
        // Ejemplo de compatibilidad
        mostrarEjemploCompatibilidad();
        
        std::cout << "\n=== EJEMPLOS COMPLETADOS EXITOSAMENTE ===" << std::endl;
        std::cout << "Los códigos QR generados pueden ser leídos por cualquier" << std::endl;
        std::cout << "aplicación móvil estándar compatible con ISO/IEC 18004." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error durante la ejecución: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
