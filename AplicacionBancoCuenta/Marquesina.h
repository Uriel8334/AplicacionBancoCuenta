#pragma once

#ifndef MARQUESINA_H
#define MARQUESINA_H

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <windows.h>
#include <mutex>

/**
 * @struct ElementoMarquesina
 * @brief Estructura que representa un elemento individual en la marquesina
 *
 * Contiene información sobre el texto a mostrar y sus atributos de formato
 * como color, fondo y estilo.
 */
struct ElementoMarquesina {
    /** @brief Texto a mostrar en la marquesina */
    std::string texto;
    /** @brief Código de color del texto */
    int colorTexto;
    /** @brief Código de color del fondo */
    int colorFondo;
    /** @brief Indica si el texto debe mostrarse en negrita */
    bool negrita;
};

/**
 * @class Marquesina
 * @brief Clase que implementa una marquesina de texto deslizante
 *
 * Permite mostrar textos con efectos de desplazamiento horizontal en consola.
 * Funciona en un hilo separado y puede ser controlada dinámicamente (pausar, detener, etc.).
 * Admite formateo básico a través de archivos HTML simples.
 */
class Marquesina {
private:
    /** @brief Vector de elementos que se mostrarán en la marquesina */
    std::vector<ElementoMarquesina> elementos;

    /** @brief Hilo donde se ejecuta la marquesina */
    std::thread hiloMarquesina;

    /** @brief Bandera atómica que controla la ejecución del hilo */
    std::atomic<bool> ejecutando;

    /** @brief Bandera atómica que controla la pausa de la marquesina */
    std::atomic<bool> pausado;

    /** @brief Bandera atómica que controla el bloqueo total de la marquesina */
    std::atomic<bool> bloqueado;

    /** @brief Bandera para indicar operaciones críticas de posicionamiento de cursor */
    std::atomic<bool> operacionCritica;

    /** @brief Posición X en la consola donde inicia la marquesina */
    int posX;

    /** @brief Posición Y en la consola donde se ubica la marquesina */
    int posY;

    /** @brief Ancho en caracteres de la marquesina */
    int ancho;

    /** @brief Velocidad de desplazamiento (delay en milisegundos) */
    int velocidad;

    /** @brief Ruta del archivo HTML que contiene el contenido de la marquesina */
    std::string archivoHTML;

    /** @brief Mutex para sincronización de acceso a datos compartidos */
    std::mutex mtx;

    /** @brief Mutex específico para operaciones de salida en pantalla */
    std::mutex mtxPantalla;

    /** @brief Buffer interno para evitar interferencias en la visualización */
    std::string bufferMarquesina;

    /** @brief Indica si el buffer ha sido actualizado y necesita redibujarse */
    std::atomic<bool> bufferActualizado;

    /** @brief Buffer interno primario para separar la marquesina del main */
    std::vector<std::pair<char, WORD>> bufferPrimario;

    /** @brief Buffer interno secundario para separar el main de la marquesina */
	std::vector<std::pair<char, WORD>> bufferSecundario;

	/** @brief Indica si el buffer esta o no listo */
	std::atomic<bool> bufferListo;

	/** @brief Indica si el buffer esta en conjunto de mutex */
	std::mutex bufferMutex;

    /** @brief Timestamp de la última modificación del archivo HTML */
    FILETIME ultimaModificacion;

    /**
     * @brief Carga el contenido de la marquesina desde un archivo HTML
     */
    void cargarDesdeHTML();

    /**
     * @brief Función principal que se ejecuta en el hilo de la marquesina
     */
    void ejecutarMarquesina();

    /**
     * @brief Analiza el contenido HTML y lo convierte en elementos de marquesina
     * @param contenido Cadena con el contenido HTML a analizar
     * @return Vector de elementos de marquesina formateados
     */
    std::vector<ElementoMarquesina> parsearHTML(const std::string& contenido);

    /**
     * @brief Verifica si el archivo HTML ha sido modificado desde la última lectura
     * @return true si el archivo ha sido modificado, false en caso contrario
     */
    bool archivoModificado();

    /**
     * @brief Verifica si es seguro escribir en una posición específica de la consola
     * @param fila Número de fila a verificar
     * @return true si es seguro escribir, false si puede interferir con otras operaciones
     */
    bool esSafeZone(int fila) const;

    /**
     * @brief Actualiza el buffer interno con el contenido actual
     */
    void actualizarBuffer();

    /**
     * @brief Renderiza el contenido del buffer en la consola
     */
    void renderizarMarquesina();

    /**
	* @brief Verifica si la marquesina debe renderizarse en pantalla
    */
    bool debeRenderizarse();

public:
    /**
     * @brief Constructor de la marquesina
     * @param x Posición X en la consola
     * @param y Posición Y en la consola
     * @param ancho Ancho en caracteres de la marquesina
     * @param archivoHTML Ruta del archivo HTML con el contenido
     * @param velocidad Velocidad de desplazamiento (delay en ms, por defecto 100)
     */
    Marquesina(int x, int y, int ancho, const std::string& archivoHTML, int velocidad = 100);

    /**
     * @brief Destructor de la marquesina
     *
     * Detiene el hilo y libera los recursos
     */
    ~Marquesina();

    /**
     * @brief Inicia la ejecución de la marquesina
     */
    void iniciar();

    /**
     * @brief Detiene permanentemente la ejecución de la marquesina
     */
    void detener();

    /**
     * @brief Pausa temporalmente la marquesina
     */
    void pausar();

    /**
     * @brief Reanuda la ejecución de la marquesina pausada
     */
    void reanudar();

    /**
     * @brief Actualiza el texto de la marquesina
     * @param nuevoTexto Nuevo texto a mostrar
     */
    void actualizarTexto(const std::string& nuevoTexto);

    /**
     * @brief Bloquea temporalmente la marquesina
     *
     * Impide que la marquesina se dibuje en pantalla
     */
    void bloquear();

    /**
     * @brief Desbloquea la marquesina
     *
     * Permite que la marquesina reanude su visualización
     */
    void desbloquear();

    /**
     * @brief Marca el inicio de una operación crítica de cursor
     *
     * Evita que la marquesina interfiera con operaciones de posicionamiento del cursor
     */
    void marcarOperacionCritica();

    /**
     * @brief Marca el fin de una operación crítica de cursor
     */
    void finalizarOperacionCritica();

    /**
     * @brief Fuerza una actualización inmediata de la marquesina
     */
    void forzarActualizacion();

    /**
     * @brief Muestra la ruta del archivo HTML (para depuración)
     */
    void mostrarRutaArchivo();

    /**
     * @brief Verifica si la marquesina está visible
     * @return true si la marquesina está ejecutándose y es visible, false en caso contrario
     */
    bool estaVisible() const { return ejecutando && !pausado && !bloqueado; }
};
#endif // MARQUESINA_H
