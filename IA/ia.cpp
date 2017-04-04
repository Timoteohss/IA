#include "stdafx.h"


class DNA {
public:

	DNA(int iTamanho, double taxaMut) {
		//inicia aleatóriedade
		std::random_device rdt;
		std::mt19937 mtt(rdt());

		//letra aleatória dentro dos ascii
		std::uniform_int_distribution<int> distt(32, 128);

		//Popula o novo array com novos caracteres
		for (int i = 0; i < iTamanho; ++i) {
			cGene.push_back((char)distt(mtt));
		}

		//inicia o fitness e mutação
		dFit = 0.0f, dMuta = taxaMut;
	};

	DNA(std::shared_ptr<DNA> pai1, std::shared_ptr<DNA> pai2) {
		//inicia aleatóriedade
		std::random_device rdt;
		std::mt19937 mtt(rdt());

		//valor aleatório dentro do gene
		std::uniform_int_distribution<int> distt(0, pai1->getTamGene());

		//Um meio de determinar de qual pai pegar seu filho
		int m = distt(mtt);

		//Começa a preencher o cGenes
		for(int i = 0; i < pai1->getTamGene(); ++i) {
			if (i > m) {
				cGene.push_back(pai1->getGeneAt(i));
			}
			else {
				cGene.push_back(pai2->getGeneAt(i));
			}
		}

		dFit = 0.0f, dMuta = pai1->getTaxaMut();
	};

	double getTaxaMut() {
		return dMuta;
	};

	double setTaxaMut() {

	};

	double getFit() {
		return dFit;
	};	

	//Seta o fit baseado no alvo passado
	void setFit(std::string sAlvo) {
		if (sAlvo != "") {
			int pontua = 0;
			for (int i = 0; i < cGene.size(); ++i) {
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

	//Genes de mutação
	void Mutacao() {
		//inicia aleatóriedade
		std::random_device rdt;
		std::mt19937 mtt(rdt());

		//valor aleatorio entre 1 e 0
		std::uniform_real_distribution<double> distt(0, 1);

		//caractere aleatorio
		std::uniform_int_distribution<int> dist2(32, 128);

		for (int i = 0; i < cGene.size(); ++i) {
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

	//População de DNA
	std::vector<std::shared_ptr<DNA>> vPopulacao;
	std::vector<std::shared_ptr<DNA>> vPool;

	//Variaveis
	int iTamGene = 0, iTamPopulacao;
	double dTaxMuta = 0.01;
	std::string sTempString, sAlvo;
	std::vector<std::pair <unsigned long,double> > grafico;

	//Display
	std::cout << "Algoritmo Genetico utilizando C++.\n----------------------------\n" << std::endl;
	//system("pause");

	//Pega a população do usuário
	std::cout << "Digite o tamanho da populacao: ";
	std::getline(std::cin, sTempString);
	iTamPopulacao = std::stoi(sTempString);
	std::cout << "Tamanho selecionado: " << iTamPopulacao << std::endl;

	//Pega a taxa de mutação do usuário
	std::cout << "Digite a taxa de mutacao: ";
	std::getline(std::cin, sTempString);

	dTaxMuta = std::stod(sTempString);
	std::cout << "Taxa de mutacao selecionada: " << dTaxMuta << std::endl;
	sTempString = "";

	//Pega o alvo do usuário
	std::cout << "Digite o alvo a ser alcancado: ";
	std::getline(std::cin, sAlvo);
	std::cout << std::endl;

	time_t ti, tf;
	time(&ti);


	//inicia auxiliares
	iTamGene = sAlvo.size();
	bool bAtingiuAlvo = false;
	unsigned long lGeracao = 1;

	//inica população
	for (int i = 0; i < iTamPopulacao; ++i) {
		std::shared_ptr<DNA> temp(new DNA(iTamGene, dTaxMuta));
		vPopulacao.push_back(temp);
	}

	//inicia o jogo da vida
	while (!bAtingiuAlvo) {
		//system("cls");
		//Imprime informações
		std::cout << "Geracao: " << lGeracao << std::endl;

		for (auto it = vPopulacao.begin(); it != vPopulacao.end(); ++it) {
			(*it)->setFit(sAlvo);
		}

		double maior = 0;
		int indexMai = 0;

		for (auto it = vPopulacao.begin(); it != vPopulacao.end(); ++it) {
			if ((*it)->getFit() > maior) {
				maior = (*it)->getFit();
				indexMai = it - vPopulacao.begin();
			}
		}

		grafico.push_back(std::make_pair(lGeracao, maior));

		//Imprime resultados e mostra se encontrou o perfeito
		std::cout << "Maior Fitness: " << maior * 100.0 << "% com a sequencia de gene: "
			<< vPopulacao.at(indexMai)->getGeneString() << std::endl;

		//system("cls");
		if (maior == 1) {
			bAtingiuAlvo = true;
			time(&tf);
			double seg = difftime(tf, ti);
			std::cout << "\n Finalizado em " << seg << " segundos." << std::endl;
			
			std::ofstream file;
			file.open("grafico.txt");
			for (auto it = grafico.begin(); it != grafico.end(); ++it)
			{
				auto it2 = *it;
				file << it2.first << ":" << it2.second * 100 << std::endl;
			}

			std::cin.ignore();
		}

		//Enche o Pool
		vPool.clear();
		for (auto it = vPopulacao.begin(); it != vPopulacao.end(); ++it) {
			int n = (*it)->getFit() * 100;
			for (int i = 0; i < n; ++i) {
				vPool.push_back((*it));
			}
		}

		//NÂO FAZER ISSO, MAS FUNCIONA
		vPopulacao.clear();

		//junta os casais para recriar a população
		std::uniform_int_distribution<int> dist2(0, vPool.size() - 1);

		//Recria a população para o mesmo tamanho com o mesmo DNA
		for (int i = 0; i < iTamPopulacao; ++i) {
			
			//Inicia operadores
			int x = 0;
			int y = 0;

			//atribui um valor aleatório baseado no tamanho do Pool
			x = dist2(mt);
			y = dist2(mt);

			//Garante que não sejam os mesmos
			while (vPool[x] == vPool[y]) {
				y = dist2(mt);
			}

			//Cria um novo DNA baseado nos pais
			std::shared_ptr<DNA> temp(new DNA(vPool.at(x), vPool.at(y)));

			//Adiciona mutação
			temp->Mutacao();

			//Adiciona o novo DNA na população
			vPopulacao.push_back(temp);

		}
		++lGeracao;

	}
	return 0;


}