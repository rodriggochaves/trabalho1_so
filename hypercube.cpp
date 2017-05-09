// hypercube.cpp

/* 
	Implementação da classe Hypercube 
*/

#include	<vector>
using	std::vector;

#include	<iostream>
using	std::cout;
using	std::endl;

typedef	vector<vector<vector<vector<int> > > >	Vector4D;

class	Hypercube {
	public:
		// Parâmetro do construtor = nº de elementos de cada dimensão
		Hypercube(int);
		
		// override operador [] para a classe Hypercube		
		vector<vector<vector<int> > >& operator[](int i){
			return	int4vector[i];
		}
		
		// vector 4D
		Vector4D	int4vector;
		
		
		// migrar métodos para o escalonador ???
		bool		isNeighbor(int, int);	// verifica se 2 nodos são vizinhos (possuem conexão)
		vector<int>	bestRoute(int, int); 	// dados 2 nodos, retorna uma sequência de nodos indicando a melhor rota
};

// implementa Hypercube como um vector 4D, com 2 elementos cada.
Hypercube::Hypercube(int num) : int4vector(num){
	for(int i = 0; i < num; i++){
		int4vector[i].resize(num);
		for(int j = 0; j < num; j++){
			int4vector[i][j].resize(num);
			for(int k = 0; k < num; k++){
				int4vector[i][j][k].resize(num);
				for(int l = 0; l < num; l++){
					int4vector[i][j][k][l] = 8*i + 4*j + 2*k + l;  
					// armazenar pid do processo gerente?
					// gerar processos gerentes no construtor do hypercubo?
				}
			}
		}
	}
}



#define	NUM_ELEM	2   // bits possíveis 0 ou 1

// exemplo de uso da classe Hypercube
int	main(){
	cout << "Criando hypercube 4D com 0's e 1's" << endl;
	Hypercube	h(NUM_ELEM);
	for(int i = 0; i < NUM_ELEM; i++){
		for(int j = 0; j < NUM_ELEM; j++){
			for(int k = 0; k < NUM_ELEM; k++){
				for(int l = 0; l < NUM_ELEM; l++){
					cout << "nodo " << i << j << k << l << "=" << h[i][j][k][l] << endl;
				}
			}
		}
	}	

	return	0;
}
