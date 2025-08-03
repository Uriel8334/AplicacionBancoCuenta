#pragma once
#ifndef ARBOLBPLUSGRAFICO_H
#define ARBOLBPLUSGRAFICO_H

#include "Persona.h"
#include "_BaseDatosPersona.h"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>

/**
 * @brief Nodo hoja especializado para árbol B+ que contiene datos y puntero al siguiente
 */
template<typename T>
struct NodoHojaB {
    std::vector<T*> datos;
    NodoHojaB<T>* siguiente;

    explicit NodoHojaB() : siguiente(nullptr) {}
    ~NodoHojaB() = default;
};

/**
 * @brief Nodo interno especializado para árbol B+ que solo contiene claves de navegación
 */
template<typename T>
struct NodoInternoB {
    std::vector<std::string> claves;
    std::vector<std::unique_ptr<NodoInternoB<T>>> hijosInternos;
    std::vector<std::unique_ptr<NodoHojaB<T>>> hijosHoja;
    bool esNodoInterno;

    explicit NodoInternoB(bool interno = true) : esNodoInterno(interno) {}
};

/**
 * @brief Estrategia para extraer diferentes campos de Persona
 */
class IExtractorCampo {
public:
    virtual ~IExtractorCampo() = default;
    virtual std::string extraer(const Persona* persona) const = 0;
    virtual std::string obtenerNombre() const = 0;
};

/**
 * @brief Manejador de vista para SFML con scrollbar
 */
class ManejadorVisualizacion {
private:
    sf::View vista;
    sf::Vector2f posicionScroll;
    sf::Vector2f limiteScroll;
    float velocidadScroll;

public:
    explicit ManejadorVisualizacion(const sf::Vector2u& tamanoVentana);
    void actualizar(sf::RenderWindow& ventana);
    void manejarScroll(float delta);
    void reiniciar();
    sf::Vector2f obtenerPosicionVista() const { return vista.getCenter(); }
};

/**
 * @brief Renderizador especializado para nodos del árbol B+
 */
class RenderizadorNodos {
private:
    sf::Font& fuente;
    std::string busquedaActual;

    sf::Color colorNormal = sf::Color(80, 120, 200);
    sf::Color colorResaltado = sf::Color(255, 220, 80);
    sf::Color colorTexto = sf::Color::Black;

public:
    explicit RenderizadorNodos(sf::Font& font) : fuente(font) {}

    void setBusqueda(const std::string& busqueda) { busquedaActual = busqueda; }

    void dibujarNodoHoja(sf::RenderWindow& ventana, const NodoHojaB<Persona>* nodo,
        sf::Vector2f posicion, const IExtractorCampo& extractor);
    void dibujarNodoInterno(sf::RenderWindow& ventana, const NodoInternoB<Persona>* nodo,
        sf::Vector2f posicion);
    void dibujarConexion(sf::RenderWindow& ventana, sf::Vector2f inicio, sf::Vector2f fin);

private:
    bool debeResaltar(const std::string& texto) const;
    std::string limitarTexto(const std::string& texto, size_t limite = 3) const;
};

/**
 * @brief Árbol B+ especializado para grado 3 con integración a base de datos
 */
template<typename T>
class ArbolBPlus {
private:
    static constexpr int GRADO = 3;
    static constexpr int MAX_CLAVES = GRADO - 1;

    std::unique_ptr<NodoInternoB<T>> raiz;
    NodoHojaB<T>* primeraHoja;
    _BaseDatosPersona& baseDatos;

    std::function<bool(const T*, const T*)> comparador;
    std::function<std::string(const T*)> extractorClave;

public:
    explicit ArbolBPlus(_BaseDatosPersona& bd);
    ~ArbolBPlus() = default;

    void cargarDesdeBaseDatos(const IExtractorCampo& extractor);
    void construir(const std::vector<T*>& elementos);
    T* buscar(const std::string& clave) const;

    const NodoInternoB<T>* obtenerRaiz() const { return raiz.get(); }
    const NodoHojaB<T>* obtenerPrimeraHoja() const { return primeraHoja; }

private:
    //void insertarEnNodo(NodoHojaB<T>*& nodo, T* elemento);
    //void dividirHoja(NodoHojaB<T>*& nodo);
    std::vector<std::unique_ptr<NodoHojaB<T>>> construirHojas(const std::vector<T*>& elementos);
    std::unique_ptr<NodoInternoB<T>> construirArbolInterno(std::vector<std::unique_ptr<NodoHojaB<T>>>& hojas);
};

/**
 * @brief Interfaz principal para la visualización gráfica del árbol B+
 */
class ArbolBPlusGrafico {
public:
    static void mostrarAnimadoSFMLGrado3(_BaseDatosPersona& baseDatos,
        const std::string& elementoResaltado = "",
        int selCriterio = 0);

private:
    static std::unique_ptr<IExtractorCampo> crearExtractor(int criterio);
    static void manejarEventos(sf::RenderWindow& ventana, std::string& busqueda,
        bool& busquedaActiva, ArbolBPlus<Persona>& arbol,
        const IExtractorCampo& extractor);
    static void renderizarArbol(sf::RenderWindow& ventana, const ArbolBPlus<Persona>& arbol,
        RenderizadorNodos& renderizador, const IExtractorCampo& extractor,
        ManejadorVisualizacion& manejadorVista);
};

// Implementaciones de extractores específicos
class ExtractorCedula : public IExtractorCampo {
public:
    std::string extraer(const Persona* persona) const override {
        return persona ? persona->getCedula() : "";
    }
    std::string obtenerNombre() const override { return "Cédula"; }
};

class ExtractorNombre : public IExtractorCampo {
public:
    std::string extraer(const Persona* persona) const override {
        return persona ? persona->getNombres().substr(0, 3) : "";
    }
    std::string obtenerNombre() const override { return "Nombre"; }
};

class ExtractorApellido : public IExtractorCampo {
public:
    std::string extraer(const Persona* persona) const override {
        return persona ? persona->getApellidos().substr(0, 3) : "";
    }
    std::string obtenerNombre() const override { return "Apellido"; }
};

class ExtractorFecha : public IExtractorCampo {
public:
    std::string extraer(const Persona* persona) const override {
        return persona ? persona->getFechaNacimiento() : "";
    }
    std::string obtenerNombre() const override { return "Fecha"; }
};

#endif // ARBOLBPLUSGRAFICO_H