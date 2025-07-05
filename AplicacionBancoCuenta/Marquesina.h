#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <windows.h>
#include <mutex>

struct ElementoMarquesina {
    std::string texto;
    int colorTexto;   // Código de color del texto
    int colorFondo;   // Código de color del fondo
    bool negrita;     // Texto en negrita
};

class Marquesina {
private:
    std::vector<ElementoMarquesina> elementos;
    std::thread hiloMarquesina;
    std::atomic<bool> ejecutando;
    std::atomic<bool> pausado;
    std::atomic<bool> bloqueado;  
    std::atomic<bool> operacionCritica; // NUEVO: Para operaciones críticas de cursor
    int posX, posY;
    int ancho;
    int velocidad;
    std::string archivoHTML;
    std::mutex mtx;
    std::mutex mtxPantalla;

    // NUEVO: Buffer interno para evitar interferencias
    std::string bufferMarquesina;
    std::atomic<bool> bufferActualizado;

    // Método para leer y parsear HTML simple
    void cargarDesdeHTML();

    // Método que ejecuta el hilo de la marquesina
    void ejecutarMarquesina();

    // Método para analizar etiquetas simples de HTML
    std::vector<ElementoMarquesina> parsearHTML(const std::string& contenido);

    // Verificar tiempo modificación archivo
    FILETIME ultimaModificacion;
    bool archivoModificado();

    // NUEVO: Método para verificar si es seguro escribir
    bool esSafeZone(int fila) const;

    // NUEVO: Métodos internos de gestión
    void actualizarBuffer();
    void renderizarMarquesina();

public:
    Marquesina(int x, int y, int ancho, const std::string& archivoHTML, int velocidad = 100);
    ~Marquesina();

    void iniciar();
    void detener();
    void pausar();
    void reanudar();
    void actualizarTexto(const std::string& nuevoTexto);

    // NUEVOS MÉTODOS: Para control avanzado
    void bloquear();    // Bloquea temporalmente la marquesina
    void desbloquear(); // Desbloquea la marquesina
    void marcarOperacionCritica();    // Para operaciones de cursor críticas
    void finalizarOperacionCritica(); // Finalizar operaciones críticas
    void forzarActualizacion(); // Fuerza una actualización inmediata

    // Método para mostrar ruta (para depuración)
    void mostrarRutaArchivo();

    // NUEVO: Método para verificar si está visible
    bool estaVisible() const { return ejecutando && !pausado && !bloqueado; }
};