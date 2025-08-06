#include "ArbolBPlusGrafico.h"
#include "Utilidades.h"
#include "ConexionMongo.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <cmath>

// ===== IMPLEMENTACIÓN ManejadorVisualizacion =====

ManejadorVisualizacion::ManejadorVisualizacion(const sf::Vector2u& tamanoVentana)
	: posicionScroll(0.f, 0.f), velocidadScroll(50.f) {
	vista = sf::View(sf::FloatRect(0.f, 0.f,
		static_cast<float>(tamanoVentana.x),
		static_cast<float>(tamanoVentana.y)));
	limiteScroll = sf::Vector2f(2000.f, 2000.f); // Área scrolleable grande
}

void ManejadorVisualizacion::actualizar(sf::RenderWindow& ventana) {
	vista.setCenter(posicionScroll);
	ventana.setView(vista);
}

void ManejadorVisualizacion::manejarScroll(float delta) {
	posicionScroll.y = std::max(0.f, std::min(limiteScroll.y, posicionScroll.y + delta * velocidadScroll));
}

void ManejadorVisualizacion::reiniciar() {
	posicionScroll = sf::Vector2f(600.f, 350.f); // Centro inicial
}

// ===== IMPLEMENTACIÓN RenderizadorNodos =====

void RenderizadorNodos::dibujarNodoHoja(sf::RenderWindow& ventana, const NodoHojaB<Persona>* nodo,
	sf::Vector2f posicion, const IExtractorCampo& extractor) {
	if (!nodo) return;

	const float anchoNodo = 120.f;
	const float altoNodo = 40.f;

	// Construir texto del nodo
	std::string textoNodo;
	std::for_each(nodo->datos.begin(), nodo->datos.end(),
		[&textoNodo, &extractor, this](const Persona* persona) {
			if (persona) {
				if (!textoNodo.empty()) textoNodo += "|";
				textoNodo += limitarTexto(extractor.extraer(persona));
			}
		});

	// Determinar color basado en búsqueda
	sf::Color colorFondo = debeResaltar(textoNodo) ? colorResaltado : colorNormal;

	// Dibujar rectángulo del nodo
	sf::RectangleShape rectangulo(sf::Vector2f(anchoNodo, altoNodo));
	rectangulo.setPosition(posicion.x - anchoNodo / 2, posicion.y - altoNodo / 2);
	rectangulo.setFillColor(colorFondo);
	rectangulo.setOutlineThickness(2.f);
	rectangulo.setOutlineColor(sf::Color::White);
	ventana.draw(rectangulo);

	// Dibujar texto
	sf::Text texto(textoNodo, fuente, 14);
	texto.setFillColor(colorTexto);
	sf::FloatRect boundsTexto = texto.getLocalBounds();
	texto.setPosition(posicion.x - boundsTexto.width / 2,
		posicion.y - boundsTexto.height / 2 - 5);
	ventana.draw(texto);
}

void RenderizadorNodos::dibujarNodoInterno(sf::RenderWindow& ventana, const NodoInternoB<Persona>* nodo,
	sf::Vector2f posicion) {
	if (!nodo) return;

	const float anchoNodo = 100.f;
	const float altoNodo = 30.f;

	// Construir texto con claves
	std::string textoNodo;
	std::for_each(nodo->claves.begin(), nodo->claves.end(),
		[&textoNodo, this](const std::string& clave) {
			if (!textoNodo.empty()) textoNodo += "|";
			textoNodo += limitarTexto(clave);
		});

	// Dibujar nodo interno (color diferente)
	sf::RectangleShape rectangulo(sf::Vector2f(anchoNodo, altoNodo));
	rectangulo.setPosition(posicion.x - anchoNodo / 2, posicion.y - altoNodo / 2);
	rectangulo.setFillColor(sf::Color(100, 100, 150));
	rectangulo.setOutlineThickness(2.f);
	rectangulo.setOutlineColor(sf::Color::Magenta);
	ventana.draw(rectangulo);

	// Dibujar texto
	sf::Text texto(textoNodo, fuente, 12);
	texto.setFillColor(sf::Color::White);
	sf::FloatRect boundsTexto = texto.getLocalBounds();
	texto.setPosition(posicion.x - boundsTexto.width / 2,
		posicion.y - boundsTexto.height / 2 - 3);
	ventana.draw(texto);
}

void RenderizadorNodos::dibujarConexion(sf::RenderWindow& ventana, sf::Vector2f inicio, sf::Vector2f fin) {
	sf::Vertex linea[] = {
		sf::Vertex(inicio, sf::Color::White),
		sf::Vertex(fin, sf::Color(180, 180, 180))
	};
	ventana.draw(linea, 2, sf::Lines);
}

bool RenderizadorNodos::debeResaltar(const std::string& texto) const {
	if (busquedaActual.empty()) return false;

	std::string textoLower = Utilidades::ConvertirAMinusculas(texto);
	std::string busquedaLower = Utilidades::ConvertirAMinusculas(busquedaActual);

	return textoLower.find(busquedaLower) != std::string::npos;
}

std::string RenderizadorNodos::limitarTexto(const std::string& texto, size_t limite) const {
	if (texto.length() <= limite) return texto;
	return texto.substr(0, limite);
}

// ===== IMPLEMENTACIÓN ArbolBPlus =====

template<typename T>
ArbolBPlus<T>::ArbolBPlus(_BaseDatosPersona& bd)
	: baseDatos(bd), primeraHoja(nullptr) {

	// Configurar comparador para Persona por cédula
	comparador = [](const T* a, const T* b) {
		if (auto personaA = dynamic_cast<const Persona*>(a)) {
			if (auto personaB = dynamic_cast<const Persona*>(b)) {
				return personaA->getCedula() < personaB->getCedula();
			}
		}
		return false;
		};

	extractorClave = [](const T* elemento) {
		if (auto persona = dynamic_cast<const Persona*>(elemento)) {
			return persona->getCedula();
		}
		return std::string();
		};
}

template<typename T>
void ArbolBPlus<T>::cargarDesdeBaseDatos(const IExtractorCampo& extractor) {
	try {
		auto documentos = baseDatos.mostrarTodasPersonas();
		std::vector<T*> personas;

		// Convertir documentos BSON a objetos Persona
		std::for_each(documentos.begin(), documentos.end(),
			[&personas](const auto& doc) {
				auto view = doc.view();
				auto persona = std::make_unique<Persona>(
					std::string(view["cedula"].get_string().value),
					std::string(view["nombre"].get_string().value),
					std::string(view["apellido"].get_string().value),
					std::string(view["fechaNacimiento"].get_string().value),
					std::string(view["correo"].get_string().value),
					std::string(view["direccion"].get_string().value)
				);

				personas.push_back(reinterpret_cast<T*>(persona.release()));
			});

		construir(personas);

	}
	catch (const std::exception& e) {
		std::cerr << "Error cargando datos: " << e.what() << std::endl;
	}
}

template<typename T>
void ArbolBPlus<T>::construir(const std::vector<T*>& elementos) {
	if (elementos.empty()) return;

	// Ordenar elementos
	std::vector<T*> elementosOrdenados = elementos;
	std::sort(elementosOrdenados.begin(), elementosOrdenados.end(), comparador);

	// Construir hojas
	auto hojas = construirHojas(elementosOrdenados);

	if (hojas.empty()) return;

	// Establecer primera hoja
	primeraHoja = hojas[0].get();

	// Conectar hojas secuencialmente
	for (size_t i = 0; i < hojas.size() - 1; ++i) {
		hojas[i]->siguiente = hojas[i + 1].get();
	}

	// Construir árbol interno
	raiz = construirArbolInterno(hojas);
}

template<typename T>
std::vector<std::unique_ptr<NodoHojaB<T>>> ArbolBPlus<T>::construirHojas(const std::vector<T*>& elementos) {
	std::vector<std::unique_ptr<NodoHojaB<T>>> hojas;

	auto hojaActual = std::make_unique<NodoHojaB<T>>();

	std::for_each(elementos.begin(), elementos.end(),
		[&hojas, &hojaActual](T* elemento) {
			if (hojaActual->datos.size() >= MAX_CLAVES) {
				hojas.push_back(std::move(hojaActual));
				hojaActual = std::make_unique<NodoHojaB<T>>();
			}
			hojaActual->datos.push_back(elemento);
		});

	if (!hojaActual->datos.empty()) {
		hojas.push_back(std::move(hojaActual));
	}

	return hojas;
}

template<typename T>
std::unique_ptr<NodoInternoB<T>> ArbolBPlus<T>::construirArbolInterno(std::vector<std::unique_ptr<NodoHojaB<T>>>& hojas) {
	if (hojas.size() <= 1) {
		auto nodoRaiz = std::make_unique<NodoInternoB<T>>();
		if (!hojas.empty()) {
			nodoRaiz->hijosHoja.push_back(std::move(hojas[0]));
		}
		return nodoRaiz;
	}

	// Crear nodos internos nivel por nivel
	std::vector<std::unique_ptr<NodoInternoB<T>>> nivelActual;

	// Primer nivel: nodos que apuntan a hojas
	for (size_t i = 0; i < hojas.size(); i += GRADO) {
		auto nodoInterno = std::make_unique<NodoInternoB<T>>();

		size_t fin = std::min(i + GRADO, hojas.size());
		for (size_t j = i; j < fin; ++j) {
			if (j > i && !hojas[j]->datos.empty()) {
				nodoInterno->claves.push_back(extractorClave(hojas[j]->datos[0]));
			}
			nodoInterno->hijosHoja.push_back(std::move(hojas[j]));
		}

		nivelActual.push_back(std::move(nodoInterno));
	}

	// Construir niveles superiores
	while (nivelActual.size() > 1) {
		std::vector<std::unique_ptr<NodoInternoB<T>>> siguienteNivel;

		for (size_t i = 0; i < nivelActual.size(); i += GRADO) {
			auto nodoInterno = std::make_unique<NodoInternoB<T>>();

			size_t fin = std::min(i + GRADO, nivelActual.size());
			for (size_t j = i; j < fin; ++j) {
				if (j > i && !nivelActual[j]->claves.empty()) {
					nodoInterno->claves.push_back(nivelActual[j]->claves[0]);
				}
				nodoInterno->hijosInternos.push_back(std::move(nivelActual[j]));
			}

			siguienteNivel.push_back(std::move(nodoInterno));
		}

		nivelActual = std::move(siguienteNivel);
	}

	return std::move(nivelActual[0]);
}

template<typename T>
T* ArbolBPlus<T>::buscar(const std::string& clave) const {
	const NodoHojaB<T>* hoja = primeraHoja;

	while (hoja) {
		auto encontrado = std::find_if(hoja->datos.begin(), hoja->datos.end(),
			[&clave, this](const T* elemento) {
				return extractorClave(elemento) == clave;
			});

		if (encontrado != hoja->datos.end()) {
			return *encontrado;
		}

		hoja = hoja->siguiente;
	}

	return nullptr;
}

// ===== IMPLEMENTACIÓN ArbolBPlusGrafico =====

void ArbolBPlusGrafico::mostrarAnimadoSFMLGrado3(_BaseDatosPersona& baseDatos, const std::string& elementoResaltado, int selCriterio) {
	sf::RenderWindow ventana(sf::VideoMode(1400, 800),
		"Árbol B+ Gráfico (Grado 3) - Base de Datos",
		sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
	ventana.setFramerateLimit(60);

	sf::Font fuente;
	if (!fuente.loadFromFile("arial.ttf")) {
		std::cerr << "Error: No se pudo cargar la fuente arial.ttf" << std::endl;
		return;
	}

	// Crear extractor según criterio
	auto extractor = crearExtractor(selCriterio);

	// Crear árbol B+ y cargar datos
	ArbolBPlus<Persona> arbol(baseDatos);
	arbol.cargarDesdeBaseDatos(*extractor);

	// Crear manejadores
	ManejadorVisualizacion manejadorVista(ventana.getSize());
	manejadorVista.reiniciar();

	RenderizadorNodos renderizador(fuente);
	renderizador.setBusqueda(elementoResaltado);

	std::string busquedaActual = elementoResaltado;
	bool busquedaActiva = false;

	// Bucle principal
	while (ventana.isOpen()) {
		manejarEventos(ventana, busquedaActual, busquedaActiva, arbol, *extractor);

		renderizador.setBusqueda(busquedaActual);

		ventana.clear(sf::Color(30, 30, 40));

		manejadorVista.actualizar(ventana);
		renderizarArbol(ventana, arbol, renderizador, *extractor, manejadorVista);

		// Dibujar interfaz de búsqueda
		sf::View vistaUI = ventana.getDefaultView();
		ventana.setView(vistaUI);

		sf::Text textoBusqueda("Búsqueda: " + busquedaActual, fuente, 18);
		textoBusqueda.setPosition(10, 10);
		textoBusqueda.setFillColor(sf::Color::White);
		ventana.draw(textoBusqueda);

		sf::Text instrucciones("ESC: Salir | F: Buscar | Scroll: Navegar", fuente, 14);
		instrucciones.setPosition(10.0f, static_cast<float>(ventana.getSize().y) - 30.0f);
		instrucciones.setFillColor(sf::Color(200, 200, 200));
		ventana.draw(instrucciones);

		ventana.display();
	}
}

std::unique_ptr<IExtractorCampo> ArbolBPlusGrafico::crearExtractor(int criterio) {
	switch (criterio) {
	case 0: return std::make_unique<ExtractorCedula>();
	case 1: return std::make_unique<ExtractorNombre>();
	case 2: return std::make_unique<ExtractorApellido>();
	case 3: return std::make_unique<ExtractorFecha>();
	default: return std::make_unique<ExtractorCedula>();
	}
}

void ArbolBPlusGrafico::manejarEventos(sf::RenderWindow& ventana, std::string& busqueda,
	bool& busquedaActiva, ArbolBPlus<Persona>& arbol,
	const IExtractorCampo& extractor) {
	static ManejadorVisualizacion* manejadorVista = nullptr;
	static bool primeraLlamada = true;

	if (primeraLlamada) {
		manejadorVista = new ManejadorVisualizacion(ventana.getSize());
		primeraLlamada = false;
	}

	sf::Event evento;
	while (ventana.pollEvent(evento)) {
		switch (evento.type) {
		case sf::Event::Closed:
			ventana.close();
			break;

		case sf::Event::KeyPressed:
			if (evento.key.code == sf::Keyboard::Escape) {
				ventana.close();
			}
			else if (evento.key.code == sf::Keyboard::F) {
				busquedaActiva = true;
				busqueda.clear();
			}
			break;

		case sf::Event::TextEntered:
			if (busquedaActiva) {
				if (evento.text.unicode == '\b' && !busqueda.empty()) {
					busqueda.pop_back();
				}
				else if (evento.text.unicode == '\r') {
					busquedaActiva = false;
				}
				else if (evento.text.unicode < 128 && evento.text.unicode != '\b') {
					busqueda += static_cast<char>(evento.text.unicode);
				}
			}
			break;

		case sf::Event::MouseWheelScrolled:
			if (manejadorVista) {
				manejadorVista->manejarScroll(-evento.mouseWheelScroll.delta * 30.f);
			}
			break;

		default:
			break;
		}
	}
}

void ArbolBPlusGrafico::renderizarArbol(sf::RenderWindow& ventana, const ArbolBPlus<Persona>& arbol,
	RenderizadorNodos& renderizador, const IExtractorCampo& extractor,
	ManejadorVisualizacion& manejadorVista) {
	const auto* raiz = arbol.obtenerRaiz();
	if (!raiz) return;

	// Renderizar estructura del árbol
	float yInicial = 100.f;
	float espacioNivel = 120.f;

	// Función recursiva para renderizar nodos internos
	std::function<void(const NodoInternoB<Persona>*, sf::Vector2f, int)> renderizarNodosInternos;
	renderizarNodosInternos = [&](const NodoInternoB<Persona>* nodo, sf::Vector2f posicion, int nivel) {
		if (!nodo) return;

		renderizador.dibujarNodoInterno(ventana, nodo, posicion);

		// Renderizar hijos internos
		float xOffset = -200.f * nodo->hijosInternos.size() / 2.f;
		std::for_each(nodo->hijosInternos.begin(), nodo->hijosInternos.end(),
			[&](const auto& hijo) {
				sf::Vector2f posicionHijo(posicion.x + xOffset, posicion.y + espacioNivel);
				renderizador.dibujarConexion(ventana, posicion, posicionHijo);
				renderizarNodosInternos(hijo.get(), posicionHijo, nivel + 1);
				xOffset += 200.f;
			});

		// Renderizar hojas si es el último nivel interno
		if (!nodo->hijosHoja.empty()) {
			float xOffsetHoja = -150.f * nodo->hijosHoja.size() / 2.f;
			std::for_each(nodo->hijosHoja.begin(), nodo->hijosHoja.end(),
				[&](const auto& hoja) {
					sf::Vector2f posicionHoja(posicion.x + xOffsetHoja, posicion.y + espacioNivel);
					renderizador.dibujarConexion(ventana, posicion, posicionHoja);
					renderizador.dibujarNodoHoja(ventana, hoja.get(), posicionHoja, extractor);
					xOffsetHoja += 150.f;
				});
		}
		};

	// Comenzar renderizado desde la raíz
	sf::Vector2f posicionRaiz(700.f, yInicial);
	renderizarNodosInternos(raiz, posicionRaiz, 0);

	// Renderizar secuencia de hojas en la parte inferior
	const auto* hoja = arbol.obtenerPrimeraHoja();
	float xHoja = 100.f;
	float yHojas = yInicial + espacioNivel * 3.f;

	while (hoja) {
		renderizador.dibujarNodoHoja(ventana, hoja, sf::Vector2f(xHoja, yHojas), extractor);

		// Dibujar conexión a siguiente hoja
		if (hoja->siguiente) {
			renderizador.dibujarConexion(ventana,
				sf::Vector2f(xHoja + 60.f, yHojas),
				sf::Vector2f(xHoja + 90.f, yHojas));
		}

		hoja = hoja->siguiente;
		xHoja += 150.f;
	}
}

// Instanciaciones explícitas de template
template class ArbolBPlus<Persona>;