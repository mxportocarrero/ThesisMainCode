#include "feature.hpp"



// CLASE NODO
// ----------
// 		->Feature // Solo es necesario agregar esta libreria para tener las otras
// 			->random
// 			->settings
// 			->general_includes // No hay conflicto por que tambien solo son cabeceras

// Esta clase representa la unidad basica del bosque

template<class T>
void Serialize_(std::ostream& o, const T& t)
{
  o.write((const char*)(&t), sizeof(T));
}

template<class T>
void Deserialize_(std::istream& o, T& t)
{
  o.read((char*)(&t), sizeof(T));
}

template<typename D, typename RGB>
class Node
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	// Atributos
	// ---------

	bool is_leaf_ = false;
	bool is_split_ = false;
	uint32_t depth_ = 0;
	Node<D, RGB> *left_;
	Node<D, RGB> *right_;
	DepthAdaptiveRGB<D, RGB> feature_;
	Eigen::Vector3d mode_;

	// METODOS
	// -------

	// El siguiente operador sirve para que los vectores tipo Eigen declarados dentro de la
	// clase puedan estar alineados y puedan optimizarse mediante instrucciones SSE
	
	Node();

	~Node();

	// Estos metodos son la escritura de la informacion de los nodos
	void Serialize(std::ostream& o) const;

	void Deserialize(std::istream& i);
}; // Fin de la declaración de clase





