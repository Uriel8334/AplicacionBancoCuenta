# GeneradorQRBanco - Guía de Referencia Rápida

## Descripción
Clase optimizada para generar códigos QR bancarios siguiendo estrictamente el estándar **ISO/IEC 18004**.

## Especificaciones Técnicas
- **Versión**: 2 (25x25 módulos)
- **Nivel de corrección**: L (7% de corrección de errores)
- **Modo de codificación**: Byte (binario)
- **Capacidad**: 44 bytes de datos + 7 bytes ECC
- **Patrón de enmascaramiento**: 1 (filas pares)

## Métodos Principales

### Constructores
```cpp
// Con objeto Persona
GeneradorQRBanco(const Persona& persona, const std::string& numCuenta);

// Con strings directos
GeneradorQRBanco(const std::string& nombre, const std::string& numCuenta);
```

### Métodos de Generación
```cpp
void generar();                    // Genera la matriz QR completa
void imprimir();                   // Imprime QR en consola con '█' y espacios
```

### Métodos de Validación
```cpp
static bool esNombreValido(const std::string& nombre);
static bool esNumeroCuentaValido(const std::string& cuenta);
bool verificarCompatibilidadQR();
bool generarCodigoQRCompatible();
```

### Métodos de Exportación
```cpp
void guardarComoSVG(const std::string& nombreArchivo);
void guardarInformacionCuenta(const std::string& nombreArchivo);
std::string obtenerContenidoQR();
```

## Uso Básico

### Ejemplo Simple
```cpp
#include "GeneradorQRBanco.h"

int main() {
    // Crear instancia
    GeneradorQRBanco qr("URIEL ALEJANDRO ANDRADE HERMOSA", "2100000014");
    
    // Generar código QR
    qr.generar();
    
    // Mostrar en consola
    qr.imprimir();
    
    // Guardar archivos
    qr.guardarComoSVG("mi_qr.svg");
    qr.guardarInformacionCuenta("mi_info.txt");
    
    return 0;
}
```

### Validación de Datos
```cpp
std::string nombre = "MARIA GONZALEZ";
std::string cuenta = "1234567890";

if (GeneradorQRBanco::esNombreValido(nombre) && 
    GeneradorQRBanco::esNumeroCuentaValido(cuenta)) {
    
    GeneradorQRBanco qr(nombre, cuenta);
    qr.generar();
    qr.imprimir();
}
```

## Formato de Datos del QR

### Estructura del Contenido
```
Nombre: [NOMBRE EN MAYÚSCULAS]
N. CUENTA: [10 DÍGITOS]
```

### Ejemplo de Salida
```
Nombre: URIEL ALEJANDRO ANDRADE HERMOSA
N. CUENTA: 2100000014
```

## Validaciones Implementadas

### Nombre
- ✅ Longitud: 2-76 caracteres
- ✅ Caracteres permitidos: letras, espacios, guiones, puntos
- ✅ Conversión automática a mayúsculas

### Número de Cuenta
- ✅ Longitud: exactamente 10 dígitos
- ✅ Solo caracteres numéricos (0-9)
- ✅ Sin espacios ni caracteres especiales

## Archivos Generados

### Archivo de Información (*.txt)
```
=== INFORMACIÓN DE CUENTA BANCARIA ===

BANCO PICHINCHA DIDÁCTICO
Titular: URIEL ALEJANDRO ANDRADE HERMOSA
N. Cuenta: 2100000014
Formato de datos: ESTRUCTURADO

Contenido del código QR:
------------------------
Nombre: URIEL ALEJANDRO ANDRADE HERMOSA
N. CUENTA: 2100000014

Detalles técnicos:
- Versión QR: 2 (25x25)
- Estándar: ISO/IEC 18004
- Nivel de corrección: L (7%)
```

### Archivo SVG (*.svg)
- Formato vectorial escalable
- Compatible con navegadores web
- Optimizado para impresión
- Incluye zona de silencio requerida

## Salida en Consola

### Ejemplo de Visualización
```
=== CÓDIGO QR BANCARIO (VERSIÓN 2 - 25x25) ===
Estándar: ISO/IEC 18004
Nivel de corrección: L
Contenido:
Nombre: URIEL ALEJANDRO ANDRADE HERMOSA
N. CUENTA: 2100000014
======================================================
                                                      
  ██████████████    ██  ██████████████  
  ██          ██  ██    ██          ██  
  ██  ██████  ██  ██    ██  ██████  ██  
  ██  ██████  ██        ██  ██████  ██  
  ██  ██████  ██  ██    ██  ██████  ██  
  ██          ██  ██    ██          ██  
  ██████████████  ██  ██████████████  
                  ██                    
  ████████████████████████████████████  
  ...resto del código QR...
  ████████████████████████████████████  
                                        
======================================================
✓ QR generado según ISO/IEC 18004
✓ Compatible con lectores móviles
✓ Datos: 46 caracteres
```

## Métodos Internos (Privados)

### Codificación
- `codificarDatos()`: Convierte texto a bits según ISO/IEC 18004
- `generarECC()`: Genera códigos Reed-Solomon para corrección
- `construirMatriz()`: Ensambla la matriz QR completa

### Patrones Estructurales
- `agregarPatronesPosicion()`: Patrones de esquina (finder patterns)
- `agregarPatronesTiempo()`: Patrones de tiempo (timing patterns)
- `agregarPatronesAlineamiento()`: Patrón central (alignment pattern)
- `agregarInformacionFormato()`: Información de formato y corrección

### Enmascaramiento
- `aplicarEnmascaramiento()`: Aplica máscara XOR para optimización
- `estaReservado()`: Verifica si una posición está reservada

## Compatibilidad

### Lectores Móviles
- ✅ iOS (Cámara nativa, apps QR)
- ✅ Android (Google Lens, apps QR)
- ✅ Aplicaciones bancarias
- ✅ Navegadores web modernos

### Estándares Soportados
- ✅ ISO/IEC 18004:2015
- ✅ JIS X 0510
- ✅ Nivel de corrección L estándar
- ✅ Codificación UTF-8 compatible

## Compilación

### Comandos de Compilación
```bash
# Compilación básica
g++ -std=c++11 main.cpp GeneradorQRBanco.cpp Utilidades.cpp Persona.cpp -o mi_programa

# Con optimización
g++ -std=c++11 -O2 main.cpp GeneradorQRBanco.cpp Utilidades.cpp Persona.cpp -o mi_programa

# Con debug
g++ -std=c++11 -g -DDEBUG main.cpp GeneradorQRBanco.cpp Utilidades.cpp Persona.cpp -o mi_programa
```

### Dependencias
- C++11 o superior
- No requiere librerías externas
- Compatible con GCC, Clang, MSVC

## Notas Importantes

1. **Formato fijo**: La clase está optimizada para Versión 2 (25x25) únicamente
2. **Datos estructurados**: Usa formato "Nombre: X\nN. CUENTA: Y" para máxima compatibilidad
3. **Sin caracteres especiales**: Evita símbolos que puedan causar problemas en lectores móviles
4. **Corrección balanceada**: Nivel L proporciona buen balance entre capacidad y robustez
5. **Zona de silencio**: Incluye automáticamente la zona de silencio requerida

## Resolución de Problemas

### Errores Comunes
- **Nombre demasiado largo**: Máximo 76 caracteres
- **Cuenta inválida**: Debe ser exactamente 10 dígitos
- **Caracteres especiales**: Solo letras, espacios, guiones y puntos en nombres
- **Codificación**: Usar UTF-8 para caracteres especiales

### Verificación
```cpp
if (qr.verificarCompatibilidadQR()) {
    std::cout << "QR compatible generado" << std::endl;
} else {
    std::cout << "Revisar datos de entrada" << std::endl;
}
```
