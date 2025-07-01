#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <windows.h>
#include <mutex>

struct ElementoMarquesina {
    std::string texto;
    int colorTexto;   // C�digo de color del texto
    int colorFondo;   // C�digo de color del fondo
    bool negrita;     // Texto en negrita
};

class Marquesina {
private:
    std::vector<ElementoMarquesina> elementos;
    std::thread hiloMarquesina;
    std::atomic<bool> ejecutando;
    std::atomic<bool> pausado;
    int posX, posY;
    int ancho;
    int velocidad;
    std::string archivoHTML;
    std::mutex mtx;

    // M�todo para leer y parsear HTML simple
    void cargarDesdeHTML();

    // M�todo que ejecuta el hilo de la marquesina
    void ejecutarMarquesina();

    // M�todo para analizar etiquetas simples de HTML
    std::vector<ElementoMarquesina> parsearHTML(const std::string& contenido);

    // Verificar tiempo modificaci�n archivo (usando API Windows en vez de std::filesystem)
    FILETIME ultimaModificacion;
    bool archivoModificado();

public:
    Marquesina(int x, int y, int ancho, const std::string& archivoHTML, int velocidad = 100);
    ~Marquesina();

    void iniciar();
    void detener();
    void pausar();
    void reanudar();
    void actualizarTexto(const std::string& nuevoTexto);
    void mostrarRutaArchivo();

};