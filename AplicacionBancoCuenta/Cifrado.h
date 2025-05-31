#pragma once
#ifndef CIFRADO_H
#define CIFRADO_H

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

// Clase Cifrado con la logica para cifrar y descifrar el archivo binario usando XOR
class Cifrado {
public:
    // Metodos estaticos para cifrado/descifrado de archivos
    static void CifrarArchivo(const std::string& rutaEntrada, const std::string& rutaSalida, char clave);
    static void desifrarArchivo(const std::string& rutaEntrada, const std::string& rutaSalida, char valor);
    
    // Metodos para trabajar con datos del banco
    static void cifrarYGuardarDatos(const class Banco& banco, const std::string& nombreArchivo, char claveCifrado);
    static bool descifrarYCargarDatos(class Banco& banco, const std::string& nombreArchivo, char claveCifrado);
    static bool descifrarSinCargarDatos(const Banco& banco, const std::string& nombreArchivo, char claveCifrado, int opcion);
};

#endif // CIFRADO_H