#include "stdafx.h"

class DNA {
public:

	DNA(int iTamanho, double taxaMut) {
		//inicia aleat�riedade
		std::random_device rdt;
		std::mt19937 mtt(rdt());

		//letra aleat�ria dentro dos ascii
		std::uniform_int_distribution<int> distt(32, 128);

		//Popula o novo array com novos caracteres
		for (int i = 0; i < iTamanho; i++) {
			cGene.push_back((char)distt(mtt));
		}

		//inicia o fitness e muta��o
		dFit = 0.0f, dMuta = taxaMut;
	};

	DNA(std::shared_ptr<DNA> pai1, std::shared_ptr<DNA> pai2) {
		//inicia aleat�riedade
		std::random_device rdt;
		std::mt19937 mtt(rdt());

		//valor aleat�rio dentro do gene
		std::uniform_int_distribution<int> distt(0, pai1->getTamGene());

		//Ajudata um meio de determinar de qual pai pegar seu filho
		int m = distt(mtt);

		//Come�a a preencher o cGenes
		for(int i = 0; i < pai1->getTamGene(); i++) {
			if (i > m) {
				cGene.push_back(pai1->getGeneAt(i));
			}
			else {
				cGene.push_back(pai1->getGeneAt(i));
			}
		}

		dFit = 0.0f, dMuta = pai1->getTaxaMut();
	};

	double getTaxaMut() {
		return dMuta;
	};

	double getFit() {
		return dFit;
	};

	//Seta o fit baseado no alvo passado
	void setFit(std::string sAlvo) {
		if (sAlvo != "") {
			int pontua = 0;
			for (int i = 0; i < cGene.size(); i++) {
				if (cGene[i] == sAlvo.at(i)) {
					++pontua;
				}
			}
			dFit = ((double)pontua) / sAlvo.size();
		}
	};

	//recebe o gene como uma string
	std::string getGeneString() {
		std::string temp(cGene.begin(), cGene.end());
		return temp;
	};

	//recebe o tamanho do gene
	int getTamGene() {
		return cGene.size();
	};

	char getGeneAt(int i) {
		return cGene[i];
	};

	//Genes de muta��o
	void Mutacao() {
		//inicia aleat�riedade
		std::random_device rdt;
		std::mt19937 mtt(rdt());

		//valor aleatorio entre 1 e 0
		std::uniform_int_distribution<int> distt(0, 1);

		//caractere aleatorio
		std::uniform_int_distribution<int> dist2(32, 128);

		for (int i = 0; i < cGene.size(); i++) {
			double t = distt(mtt);
if (t < dMuta) {
	cGene[i] = dist2(mtt);
}
		}
	};

private:
	std::vector<char> cGene;
	double dFit, dMuta;
};

int _tmain(int argc, _TCHAR* argv[]) {
	//inicia aleatoriedade
	std::random_device rd;
	std::mt19937 mt(rd());

	//Popula��o de DNA
	std::vector<std::shared_ptr<DNA>> vPopulacao;
	std::vector<std::shared_ptr<DNA>> vPopulacaoTemp;
	std::vector<std::shared_ptr<DNA>> vPool;

	//Variaveis
	int iTamGene = 0, iTamPopulacao;
	double dTaxMuta = 0.01;
	std::string sTempString, sAlvo;

	//Display
	std::cout << "Algoritmo Genetico utilizando C++.\n----------------------------\n" << std::endl;
	//system("pause");

	//Pega a popula��o do usu�rio
	std::cout << "Digite o tamanho da populacao: ";
	std::getline(std::cin, sTempString);
	iTamPopulacao = std::stoi(sTempString);
	std::cout << "Tamanho selecionado: " << iTamPopulacao << std::endl;

	//Pega a taxa de muta��o do usu�rio
	std::cout << "Digite a taxa de mutacao: ";
	std::getline(std::cin, sTempString);
	dTaxMuta = std::stod(sTempString);
	std::cout << "Taxa de mutacao selecionada: " << dTaxMuta << std::endl;
	sTempString = "";

	//Pega o alvo do usu�rio
	std::cout << "Digite o alvo a ser alcancado: ";
	std::getline(std::cin, sAlvo);
	std::cout << std::endl;

	//inicia auxiliares
	iTamGene = sAlvo.size();
	bool bAtingiuAlvo = false;
	unsigned long lGeracao = 1;

	//inica popula��o
	for (int i = 0; i < iTamPopulacao; ++i) {
		std::shared_ptr<DNA> temp(new DNA(iTamGene, dTaxMuta));
		vPopulacao.push_back(temp);
	}

	//inicia o jogo da vida
	while (!bAtingiuAlvo) {
		system("cls");
		//Imprime informa��es
		std::cout << "Geracao: " << lGeracao << std::endl;

		for (auto it = vPopulacao.begin(); it != vPopulacao.end(); ++it) {
			(*it)->setFit(sAlvo);
		}

		double maior = 0;
		int index = 0;

		for (auto it = vPopulacao.begin(); it != vPopulacao.end(); ++it) {
			if ((*it)->getFit() > maior) {
				maior = (*it)->getFit();
				index = it - vPopulacao.begin();
			}
		}

		//Imprime resultados e mostra se encontrou o perfeito
		std::cout << "Maior Fitness: " << maior * 100.0 << "% com a sequencia de gene: "
			<< vPopulacao.at(index)->getGeneString() << " de tamanho: " << vPopulacao.size() << std::endl;
		//system("cls");
		if (maior == 1) {
			bAtingiuAlvo = true;
			std::cout << "\n Acabou. Enter para sair." << std::endl;
			std::cin.ignore();
		}

		//Enche o Pool
		vPool.clear();
		for (auto it = vPopulacao.begin(); it != vPopulacao.end(); ++it) {
			int n = (*it)->getFit() * 100;
			for (int i = 0; i < n; i++) {
				vPool.push_back((*it));
			}
		}

		//N�O FAZER ISSO, MAS FUNCIONA
		
		for (auto it = vPopulacao.begin(); it != vPopulacao.end(); ++it) {
			if (vPopulacaoTemp.size() > iTamPopulacao / 2) {
				if ((*it)->getFit() > maior / 2) {
					vPopulacaoTemp.push_back((*it));
				}
			}
		}
		vPopulacao.clear();
		vPopulacao = vPopulacaoTemp;
		vPopulacaoTemp.clear();

		//junta os casais para recriar a popula��o
		std::uniform_int_distribution<int> dist2(0, vPool.size() - 1);

		//Recria a popula��o para o mesmo tamanho com o mesmo DNA
		for (int i = 0; i < iTamPopulacao; ++i) {
			
			//Inicia operadores
			int x = 0;
			int y = 0;

			//atribui um valor aleat�rio baseado no tamanho do Pool
			x = dist2(mt);
			y = dist2(mt);

			//Garante que n�o sejam os mesmos
			while (vPool[x] == vPool[y]) {
				y = dist2(mt);
			}

			//Cria um novo DNA baseado nos pais
			std::shared_ptr<DNA> temp(new DNA(vPool.at(x), vPool.at(y)));

			//Adiciona muta��o
			temp->Mutacao();

			//Adiciona o novo DNA na popula��o
			vPopulacao.push_back(temp);

		}
		++lGeracao;

	}
	return 0;


}