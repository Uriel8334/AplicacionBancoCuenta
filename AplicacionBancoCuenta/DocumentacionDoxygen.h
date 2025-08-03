#ifndef DOCUMENTACION_DOXYGEN_H
#define DOCUMENTACION_DOXYGEN_H

class DocumentacionDoxygen
{
public: 
	
	/**
	 * @brief Abre la documentación generada por Doxygen en el navegador predeterminado
	 *
	 * Esta función intenta abrir el archivo HTML de la documentación en el navegador.
	 * Primero verifica si el archivo existe en la ruta relativa esperada, y si no lo encuentra,
	 * busca en una ubicación alternativa común. Si se encuentra el archivo, se abre; de lo contrario,
	 * se muestra un mensaje de error.
	 */
	void abrirDocumentacion();

};

#endif // DOCUMENTACION_DOXYGEN_H
