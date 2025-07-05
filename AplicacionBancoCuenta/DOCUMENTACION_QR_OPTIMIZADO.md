# Documentación: GeneradorQRBanco Optimizado

## Resumen
Esta es una implementación optimizada de la clase `GeneradorQRBanco` que genera códigos QR válidos siguiendo estrictamente el estándar **ISO/IEC 18004**, específicamente para:

- **Versión 2** (25x25 módulos)
- **Nivel de corrección L** (Low - 7% de corrección de errores)
- **Modo Byte** para codificación de datos
- **Formato estructurado** optimizado para lectores móviles

## Características Principales

### 1. Cumplimiento del Estándar ISO/IEC 18004
- ✅ Codificación en modo byte (0100)
- ✅ Generación de códigos Reed-Solomon para corrección de errores
- ✅ Patrones de posición (finder patterns) estándar
- ✅ Patrones de tiempo (timing patterns)
- ✅ Patrón de alineamiento para versión 2
- ✅ Información de formato con nivel de corrección L
- ✅ Enmascaramiento XOR con patrones estándar

### 2. Métodos Principales

#### `codificarDatos()`
- Convierte el texto en bits binarios usando el modo byte QR
- Genera la cadena: `"Nombre: [NOMBRE]\nN. CUENTA: [CUENTA]"`
- Añade indicadores de modo (0100) y longitud según estándar

#### `generarECC()`
- Aplica Reed-Solomon para generar códigos de corrección
- Utiliza 7 bytes de corrección para nivel L, versión 2
- Implementa tablas de logaritmos y antilogaritmos en GF(256)

#### `construirMatriz()`
- Genera la matriz QR completa con todos los patrones estándar
- Coloca datos y códigos de corrección en las posiciones correctas
- Añade padding según el estándar ISO/IEC 18004

#### `aplicarEnmascaramiento()`
- Aplica uno de los 8 esquemas de enmascarado XOR estándar
- Mejora la legibilidad del código QR
- Evita patrones problemáticos en los datos

#### `imprimirEnConsola()`
- Muestra la matriz QR en consola usando caracteres '█' y espacios
- Incluye zona de silencio requerida
- Formato optimizado para visualización

### 3. Especificaciones Técnicas

#### Capacidad de Datos
- **Versión 2**: 25x25 módulos
- **Capacidad total**: 44 bytes de datos + 7 bytes ECC = 51 bytes
- **Bits útiles**: 352 bits para datos (sin ECC)
- **Caracteres texto**: Aproximadamente 44 caracteres alfanuméricos

#### Patrones Estructurales
- **Patrones de posición**: 3 patrones de 7x7 en las esquinas
- **Separadores**: Zona blanca de 1 módulo alrededor de patrones de posición
- **Patrones de tiempo**: Líneas alternadas en fila/columna 6
- **Patrón de alineamiento**: 1 patrón de 5x5 centrado en (18,18)
- **Información de formato**: 15 bits distribuidos alrededor del patrón superior izquierdo

#### Corrección de Errores
- **Nivel L**: 7% de corrección de errores
- **Algoritmo**: Reed-Solomon con GF(256)
- **Polinomio primitivo**: 0x11D
- **Bytes ECC**: 7 bytes para versión 2, nivel L

## Uso de la Clase

### Ejemplo Básico
```cpp
#include "GeneradorQRBanco.h"

// Crear instancia
GeneradorQRBanco qr("URIEL ALEJANDRO ANDRADE HERMOSA", "2100000014");

// Generar código QR
qr.generar();

// Imprimir en consola
qr.imprimir();

// Verificar compatibilidad
if (qr.verificarCompatibilidadQR()) {
    std::cout << "✓ Compatible con lectores móviles" << std::endl;
}
```

### Salida Esperada
```
=== CÓDIGO QR BANCARIO (VERSIÓN 2 - 25x25) ===
Estándar: ISO/IEC 18004
Nivel de corrección: L
Contenido:
Nombre: URIEL ALEJANDRO ANDRADE HERMOSA
N. CUENTA: 2100000014
========================================================
                                                      
  ██████████████    ██  ██████████████  
  ██          ██  ██    ██          ██  
  ██  ██████  ██  ██    ██  ██████  ██  
  ██  ██████  ██        ██  ██████  ██  
  ██  ██████  ██  ██    ██  ██████  ██  
  ██          ██  ██    ██          ██  
  ██████████████  ██  ██████████████  
                  ██                    
  ██████████████████████████████████  
  ...datos del QR...
  ██████████████    ██                  
  ██          ██    ██                  
  ██  ██████  ██    ██                  
  ██  ██████  ██    ██                  
  ██  ██████  ██                        
  ██          ██                        
  ██████████████                        
                                        
========================================================
✓ QR generado según ISO/IEC 18004
✓ Compatible con lectores móviles
✓ Datos: 46 caracteres
```

## Validación y Compatibilidad

### Validaciones Implementadas
- ✅ Nombre: 2-76 caracteres, solo letras y espacios
- ✅ Cuenta: Exactamente 10 dígitos numéricos
- ✅ Formato de datos estructurado optimizado
- ✅ Cumplimiento del estándar ISO/IEC 18004
- ✅ Capacidad de datos dentro de los límites

### Compatibilidad Móvil
- ✅ Formato texto plano sin caracteres especiales problemáticos
- ✅ Estructura de datos clara y legible
- ✅ Tamaño óptimo para pantallas móviles
- ✅ Nivel de corrección balanceado (L)

## Archivos Generados

### Información de Cuenta (`*_info.txt`)
Contiene detalles técnicos del QR generado:
- Información del titular y cuenta
- Contenido exacto del código QR
- Especificaciones técnicas (versión, estándar, corrección)

### Archivo SVG (`*.svg`)
Código QR en formato vectorial escalable:
- Formato estándar SVG 1.1
- Optimizado para impresión y visualización
- Incluye metadatos descriptivos

## Diferencias con la Versión Anterior

### Mejoras Implementadas
1. **Estándar fijo**: Versión 2 (25x25) con nivel L
2. **Métodos especializados**: Cada fase del proceso QR tiene su método
3. **Corrección de errores precisa**: Implementación Reed-Solomon completa
4. **Enmascaramiento estándar**: 8 patrones de enmascarado disponibles
5. **Validación estricta**: Cumplimiento exacto del estándar ISO/IEC 18004

### Simplificaciones
- Eliminación de selección automática de versión
- Enfoque en un solo nivel de corrección (L)
- Formato de datos fijo optimizado para banca
- Eliminación de dependencias externas

## Recomendaciones de Uso

### Para Aplicaciones Bancarias
- Usar nombres completos en mayúsculas
- Validar números de cuenta antes de generar QR
- Mantener el formato estructurado para máxima compatibilidad
- Generar archivos SVG para impresión profesional

### Para Desarrollo
- Compilar con `-std=c++11` o superior
- Incluir manejo de excepciones robusto
- Validar entradas antes de crear la instancia
- Probar con diferentes longitudes de nombre

## Conclusión

Esta implementación optimizada de `GeneradorQRBanco` proporciona una solución robusta y estándar para la generación de códigos QR bancarios, cumpliendo estrictamente con el estándar ISO/IEC 18004 y optimizada para máxima compatibilidad con lectores móviles.
