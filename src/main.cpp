//#include "Activations.h"
//#include "DataSet.h"
//#include "Neuron.h"
//#include "Rules.h"
//#include "Run.h"
//#include "Statistics.h"
//#include "StopWatch.h"
//#include "Threader.h"
//#include "VisualUpdate.h"






#include "NetworkToVisualiserInterface.h"
#include "Visualiser.h"
#include "NeuralNetwork.h"
#include "DataSet.h"

#include <memory>
#include <string>
#include <sstream>



int main() {
	std::shared_ptr<NetworkToVisualiserInterface> face=std::make_shared<NetworkToVisualiserInterface>();
	std::shared_ptr<NeuralNetwork> n = std::make_shared<NeuralNetwork>();
	std::shared_ptr<Visualiser> v=std::make_shared<Visualiser>(face);
	face->setMainNeuralNetwork(n);
	face->setVisualiser(v);

	n->setup(face);
	DataSet data = { 49 };
	std::ostringstream oss;
	oss << "data\\data" << 7 << 7 << ".txt";
	std::string filename = oss.str();

	data.generateImageDataFromTextFileRandom(filename.c_str(), 1000);
	n->train(data);

	data.generateImageDataFromTextFileRandom(filename.c_str(), 1000);
	n->test(data);

	n->shutdown();
}