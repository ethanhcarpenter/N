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
#include "InputDataManager.h"
#include "Visualiser.h"
#include "NeuralNetwork.h"

#include <memory>
#include <string>
#include <vector>
#include <sstream>



int main() {
	std::vector < std::string> imageQualities = { "77","1414","2121","2828" };
	std::shared_ptr<NetworkVisualiserInterface> face = std::make_shared<NetworkVisualiserInterface>();
	std::shared_ptr<InputDataManager> manager = std::make_shared<InputDataManager>(imageQualities, ".png");
	std::shared_ptr<NeuralNetwork> n = std::make_shared<NeuralNetwork>();
	std::shared_ptr<Visualiser> v = std::make_shared<Visualiser>(face);
	face->setMainNeuralNetwork(n);
	face->setVisualiser(v);
	face->setInputDataManager(manager);

	n->setup(face);

	n->train();

	n->test();

	n->shutdown();
}