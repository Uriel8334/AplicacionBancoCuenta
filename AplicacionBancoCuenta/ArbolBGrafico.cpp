#define _CRT_SECURE_NO_WARNINGS
 
#include <conio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <regex>
#include <string>
#include <ctime>
#include <chrono>
#include <queue>
#include <limits>
#include <set>
#include "ArbolBGrafico.h"
//#include "Utilidades.h"
#include "Persona.h"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include <vector>
#include <stack>
#include <memory>
#include <algorithm>
#include <cctype>

void ArbolBGrafico::mostrarAnimadoSFMLGrado3(ArbolB<Persona>* arbol, const std::string& elementoResaltado, int selCriterio) {
    if (!arbol) return;
    if (arbol->grado < 3) return; // Solo mostrar si el grado es al menos 3

    sf::RenderWindow window(sf::VideoMode(1200, 700), "Árbol B Gráfico (Grado >= 3)", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return;
    }

    using NodoPtr = NodoArbolB<Persona>*; // Updated to specify NodoArbolB<Persona>
    std::vector<std::vector<NodoPtr>> niveles;
    std::queue<NodoPtr> cola;
    if (arbol->raiz == nullptr) return;
    cola.push(arbol->raiz);

    while (!cola.empty()) {
        int n = static_cast<int>(cola.size());
        std::vector<NodoPtr> nivel;
        for (int i = 0; i < n; ++i) {
            NodoPtr nodo = cola.front();
            cola.pop();
            nivel.push_back(nodo);
            for (auto hijo : nodo->hijos) {
                if (hijo) cola.push(hijo);
            }
        }
        niveles.push_back(nivel);
    }

    std::vector<std::vector<sf::Vector2f>> posiciones;
    float yBase = 80.f, yStep = 100.f;
    float xMargin = 60.f;
    for (size_t nivel = 0; nivel < niveles.size(); ++nivel) {
        float y = yBase + static_cast<float>(nivel) * yStep;
        size_t numNodos = niveles[nivel].size();
        float xStep = (window.getSize().x - 2 * xMargin) / (static_cast<float>(numNodos) + 1);
        std::vector<sf::Vector2f> posNivel;
        for (size_t i = 0; i < numNodos; ++i) {
            float x = xMargin + (static_cast<float>(i) + 1) * xStep;
            posNivel.push_back(sf::Vector2f(x, y));
        }
        posiciones.push_back(posNivel);
    }

    // Bucle principal de la ventana 
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed: {
                window.close();
                break;
            }
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                break;
            default:
                break;
            }
        }

        window.clear(sf::Color(30, 30, 40));

        // Dibujar conexiones
        for (size_t nivel = 0; nivel + 1 < niveles.size(); ++nivel) {
            for (size_t i = 0; i < niveles[nivel].size(); ++i) {
                NodoPtr nodo = niveles[nivel][i];
                size_t hijoInicio = 0;
                for (size_t k = 0; k < i; ++k) {
                    hijoInicio += niveles[nivel][k]->hijos.size();
                }
                for (size_t j = 0; j < nodo->hijos.size(); ++j) {
                    if (hijoInicio + j < posiciones[nivel + 1].size()) {
                        sf::Vertex line[] = {
                            sf::Vertex(posiciones[nivel][i], sf::Color::White),
                            sf::Vertex(posiciones[nivel + 1][hijoInicio + j], sf::Color(180, 180, 180))
                        };
                        window.draw(line, 2, sf::Lines);
                    }
                }
            }
        }
        sf::Text text;

        // Dibujar nodos
        for (size_t nivel = 0; nivel < niveles.size(); ++nivel) {
            for (size_t i = 0; i < niveles[nivel].size(); ++i) {
                NodoPtr nodo = niveles[nivel][i];
                std::string textoNodo;
                for (size_t k = 0; k < nodo->claves.size(); ++k) {
                    Persona* persona = nullptr;
                    persona = static_cast<Persona*>(nodo->claves[k]);
                    if (persona) {
                        if (selCriterio == 0) textoNodo += persona->getCedula();
                        else if (selCriterio == 1) textoNodo += persona->getNombres();
                        else if (selCriterio == 2) textoNodo += persona->getApellidos();
                        else textoNodo += persona->getFechaNacimiento();
                    }
                    else {
                        textoNodo += "obj";
                    }
                    if (k + 1 < nodo->claves.size()) textoNodo += " | ";
                }

                bool resaltado = !elementoResaltado.empty() &&
                    Utilidades::ConvertirAMinusculas(textoNodo).find(
                        Utilidades::ConvertirAMinusculas(elementoResaltado)) != std::string::npos;

                sf::RectangleShape rect(sf::Vector2f(120, 40));
                rect.setOrigin(60, 20);
                rect.setPosition(posiciones[nivel][i]);
                rect.setFillColor(resaltado ? sf::Color(255, 220, 80) : sf::Color(80, 120, 200));
                rect.setOutlineThickness(2);
                rect.setOutlineColor(sf::Color::White);
                window.draw(rect);

                text.setFont(font);
                text.setCharacterSize(16);
                text.setFillColor(sf::Color::Black);

                text.setString(textoNodo);
                sf::FloatRect bounds = text.getLocalBounds();
                text.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
                text.setPosition(posiciones[nivel][i].x, posiciones[nivel][i].y - 8);
                window.draw(text);
            }
        }

        window.display();
    }
}
void ArbolBGrafico::interfazInteractivaSFML(ArbolB<Persona>* arbol) {
    if (!arbol || arbol->grado < 3) return;

    sf::RenderWindow window(sf::VideoMode(900, 600), "Árbol B Gráfico Interactivo");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "No se pudo cargar la fuente arial.ttf\n";
        return;
    }

    std::string input;
    bool mostrarArbol = false;
    int selCriterio = 0; // 0: cédula, 1: nombres, 2: apellidos, 3: fecha nacimiento

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Captura texto del teclado para ingresar cédula o nombre
            if (!mostrarArbol && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') { // Backspace
                    if (!input.empty())
                        input.pop_back();
                }
                else if (event.text.unicode == '\r' || event.text.unicode == '\n') { // Enter
                    // Aquí podrías crear una persona e insertarla al árbol
                    // Por simplicidad, solo mostramos el árbol
                    mostrarArbol = true;
                }
                else if (event.text.unicode < 128) {
                    input += static_cast<char>(event.text.unicode);
                }
            }
            // Permitir volver a ingresar otra persona con Escape
            if (mostrarArbol && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                input.clear();
                mostrarArbol = false;
            }
        }

        window.clear(sf::Color(30, 30, 40));

        if (!mostrarArbol) {
            sf::RectangleShape textbox(sf::Vector2f(700, 40));
            textbox.setFillColor(sf::Color(230, 230, 255));
            textbox.setOutlineColor(sf::Color::Blue);
            textbox.setOutlineThickness(2);
            textbox.setPosition(sf::Vector2f(100, 40));
            window.draw(textbox);

            sf::Text texto("Ingrese cédula o nombre para buscar/insertar:", font, 20);
            texto.setFillColor(sf::Color::Black);
            texto.setPosition(sf::Vector2f(100, 5));
            window.draw(texto);

            sf::Text entrada(input, font, 22);
            entrada.setFillColor(sf::Color::Black);
            entrada.setPosition(sf::Vector2f(110, 45));
            window.draw(entrada);

            sf::Text ayuda("Presione ENTER para mostrar el árbol. ESC para limpiar.", font, 16);
            ayuda.setFillColor(sf::Color(100, 100, 100));
            ayuda.setPosition(sf::Vector2f(100, 90));
            window.draw(ayuda);
        }
        else {
            // Aquí llamamos a la función de dibujo del árbol B
            ArbolBGrafico::mostrarAnimadoSFMLGrado3(arbol, input, selCriterio);

            sf::Text ayuda("ESC para ingresar otro dato.", font, 16);
            ayuda.setFillColor(sf::Color(200, 200, 200));
            ayuda.setPosition(sf::Vector2f(20, 20));
            window.draw(ayuda);
        }

        window.display();
    }
}