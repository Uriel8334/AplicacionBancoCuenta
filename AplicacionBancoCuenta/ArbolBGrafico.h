#pragma once
#ifndef ARBOLBGRAFICO_H
#define ARBOLBGRAFICO_H

#include "ArbolB.h"
#include "Persona.h"
#include <string>

class ArbolBGrafico {
public:
    static void mostrarAnimadoSFMLGrado3(ArbolB<Persona>* arbol, const std::string& elementoResaltado, int selCriterio);

    static void interfazInteractivaSFML(ArbolB<Persona>* arbol);

};

#endif // ARBOLBGRAFICO_H