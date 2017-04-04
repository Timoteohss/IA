#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <cmath>

struct Connection
{
	double weight;
	double deltaWeight;
};

class Neuron;

typedef std::vector<Neuron> Layer;

// *************** classe Neuron ******************
class Neuron
{
public:
	Neuron(unsigned numOutputs, unsigned myIndex);
	double getOutputVal(void) const { return m_outputVal; }
	void setOutputVal(double val) { m_outputVal = val; }
	void feedFoward(const Layer &prevLayer);
	void calcOutputGradients(double targetVal);
	void calcHiddenGradients(const Layer &nextLayer);
	void updateInputWeights(Layer &prevLayer);
	

private:
	static double eta;
	static double alpha;
	static double transferFunction(double x);
	static double transferFunctionDerivative(double x);
	static double randomWeight(void) { return rand() / double(RAND_MAX); }
	double m_outputVal;
	double sumDOW(const Layer &nextLayer) const;
	std::vector<Connection> m_outputWeights;
	unsigned m_myIndex;
	double m_gradient;

};

double Neuron::eta = 0.15;
double Neuron::alpha = 0.5;



Neuron::Neuron(unsigned numOutputs, unsigned myIndex)
{
	for (unsigned c = 0; c < numOutputs; ++c)
	{
		m_outputWeights.push_back(Connection());
		m_outputWeights.back().weight = randomWeight();
	}

	m_myIndex = myIndex;
}

void Neuron::feedFoward(const Layer & prevLayer)
{
	double sum = 0.0;

	for (unsigned n = 0; n < prevLayer.size(); ++n)
	{
		sum += prevLayer[n].getOutputVal() * 
				prevLayer[n].m_outputWeights[m_myIndex].weight;
	}

	m_outputVal = Neuron::transferFunction(sum);
}

void Neuron::calcOutputGradients(double targetVal)
{
	double delta = targetVal - m_outputVal;
	m_gradient = delta * Neuron::transferFunctionDerivative(m_outputVal);
}

void Neuron::calcHiddenGradients(const Layer & nextLayer)
{
	double dow = sumDOW(nextLayer);
	m_gradient = dow * Neuron::transferFunctionDerivative(m_outputVal);
}

void Neuron::updateInputWeights(Layer & prevLayer)
{
	for (unsigned n = 0; n < prevLayer.size(); ++n)
	{
		Neuron &neuron = prevLayer[n];
		double oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

		double newDeltaWeight =
			eta
			* neuron.getOutputVal()
			* m_gradient
			+ alpha
			* oldDeltaWeight;

		neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
		neuron.m_outputWeights[m_myIndex].weight += newDeltaWeight;
	}

}

double Neuron::transferFunction(double x)
{
	return tanh(x);
}

double Neuron::transferFunctionDerivative(double x)
{
	return 1.0 - x * x;
}

double Neuron::sumDOW(const Layer & nextLayer) const
{
	double sum = 0.0;

	for (unsigned n = 0; n < nextLayer.size() - 1; ++n)
	{
		sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
	}

	return sum;
}



// *************** classe Net ******************



class Net
{
private:
	std::vector<Layer> m_layers;
	double m_error;
	double m_recentAverageError;
	double m_recentAverageSmoothingFactor;

public:
	Net(const std::vector<unsigned> &topology);
	void feedFoward(const std::vector<double> &inputVals);
	void backProp(const std::vector<double> &targetVals);
	void getResults(std::vector<double> &resultVals) const;
};

Net::Net(const std::vector<unsigned> &topology)
{
	unsigned numLayers = topology.size();
	for (unsigned layerNum = 0; layerNum < numLayers; ++layerNum)
	{
		m_layers.push_back(Layer());
		unsigned numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];

		for (unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum)
		{
			m_layers.back().push_back(Neuron(numOutputs, neuronNum));
			std::cout << "Feito um neuronio!" << std::endl;
		}

		m_layers.back().back().setOutputVal(1.0);

	}
}

void Net::feedFoward(const std::vector<double> &inputVals)
{
	//assert(inputVals.size() == m_layers[0].size() - 1);

	for (unsigned i = 0; i < inputVals.size(); ++i)
	{
		m_layers[0][i].setOutputVal(inputVals[i]);
	}

	//Foward Prop
	for (unsigned layerNum = 1; layerNum < m_layers.size(); ++layerNum)
	{
		Layer &prevLayer = m_layers[layerNum - 1];
		for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n){
			m_layers[layerNum][n].feedFoward(prevLayer);
		}
	}

}

void Net::backProp(const std::vector<double> &targetVals)
{
	//Calcula o net error
	Layer &outputLayer = m_layers.back();
	m_error = 0.0;

	for (unsigned n = 0; n < outputLayer.size() - 1; ++n)
	{
		double delta = targetVals[n] - outputLayer[n].getOutputVal();
		m_error += delta * delta;
	}
	m_error /= outputLayer.size() - 1;
	m_error = sqrt(m_error);

	//Implementa uma média de error gerais
	m_recentAverageError =
		(m_recentAverageError * m_recentAverageSmoothingFactor + m_error) /
		(m_recentAverageSmoothingFactor + 1.0);

	//calcula o gradiente 

	for (unsigned n = 0; n < outputLayer.size() - 1; ++n)
	{
		outputLayer[n].calcOutputGradients(targetVals[n]);
	}

	//calcula o gradiente nas camadas escondidas

	for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; --layerNum)
	{
		Layer &hiddenLayer = m_layers[layerNum];
		Layer &nextLayer = m_layers[layerNum + 1];

		for (unsigned n = 0; n < hiddenLayer.size() - 1; ++n)
		{
			hiddenLayer[n].calcHiddenGradients(nextLayer);
		}

	}

	//para todas as camadas da saida para a primeira camada escondida, atualizar os pesos

	for (unsigned layerNum = m_layers.size() - 1; layerNum > 0; --layerNum)
	{
		Layer &layer = m_layers[layerNum];
		Layer &prevLayer = m_layers[layerNum - 1];

		for (unsigned n = 0; n < layer.size() - 1; ++n)
			layer[n].updateInputWeights(prevLayer);

	}


}

void Net::getResults(std::vector<double> &resultVals) const
{
	resultVals.clear();

	for (unsigned n = 0; n < m_layers.back().size() - 1; ++n)
	{
		resultVals.push_back(m_layers.back()[n].getOutputVal());
	}

}



int main()
{

	std::vector<unsigned> topology;
	topology.push_back(3);
	topology.push_back(2);
	topology.push_back(1);

	Net nMinha_net(topology);

	std::vector<double> inputVals;
	nMinha_net.feedFoward(inputVals);

	std::vector<double> targetVals;
	nMinha_net.backProp(targetVals);

	std::vector<double> resultVals;
	nMinha_net.getResults(resultVals);

	std::cin.ignore();
	return 0;
}