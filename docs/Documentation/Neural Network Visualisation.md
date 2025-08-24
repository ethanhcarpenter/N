
---
# <ins>Analysis</ins>

## Overview
This project is a fully native, interactive neural network builder and visualiser designed to intuitively demonstrate how a neural network learns. The user creates a customised network by inputting parameters such as:
+ Training Data
+ Hidden Layers/Nodes
+ Epochs
+ Learning Rate
+ Activation Function
+ Batch Size
During training the network is visually represented to the user with each weighted connection updating their magnitude and polarity in real time. The elapsed time per epoch is displayed alongside the average elapsed time per epoch. This visual representation and time data allows users to comprehend many aspects of the network:
+ How a network converges over time
+ How the amount of layers and nodes affects speed 
+ How activation functions change weights
Once training has finished users can directly use the network by inputting custom data and comparing the networks output to their expected output. This testing can either be done individually or the user can chose to input testing data that consists of multiple inputs and their expected outputs to calculate a percentage accuracy. The user can then tweak their parameters to create a network of satisfactory accuracy.

## Background Information
#### What is a neural network?
> "A neural network is a machine learning program, or model, that makes decisions in a manner similar to the human brain, by using processes that mimic the way biological neurons work together to identify phenomena, weigh options and arrive at conclusions." [IBM](https://www.ibm.com/think/topics/neural-networks)

Essentially, a neural network is a system that closely copies the way a human brain interprets data. Like a human brain a neural network needs to be trained, passing data through an untrained network results in a meaningless output.

#### Training and Weights 
Training a network relies on repeated exposure to inputs paired with their expected output, known as training data. The network adjusts its weights - values applied to inputs as they pass from layer to layer - based on this data. The networks goal when training is to set the weights at such values that when new data is inputted the network produces the correct output. 
The more varied the data , the harder it is to achieve good accuracy as it has to generalise across many patterns using a single set of weights. 

#### Overfitting and Epochs
An Epoch is one complete pass of all training data through the network. A common misconception is that increasing the number of epochs will always improve accuracy. This isn't true due to a factor called overfitting which is when the network becomes too specialised to the training data. This is a problem because whilst the network may have great accuracy for its training data, it often struggles when with new data that has slight variations, noise or different formatting. The key to creating an accurate neural network is finding the optimal number of epochs - enough for the network to learn from the data but not so many that it overfits.


# <ins>Design</ins>

## Project Specifications
| Feature              | Specification        |
| -------------------- | -------------------- |
| IDE                  | `Visual Studio 2022` |
| Programming Language | `C++ 20`             |
| Graphics Library     | `GLFW3`, `ImGui`     |
| Compiler             | `Legacy MSVC`        |
| Runtime Library      | `Multi-Threaded DLL` |
#### Why C++20?
+ **Performance** -> Close-to-hardware execution enables efficient CPU usage and avoids the overhead of higher-level languages.
+ **Memory Control** -> Careful use of references and storage reduces unnecessary copies, keeping memory usage low even with frequent function calls and complex operations.
+ **Proven Choice** -> Many high-performance ML frameworks (like PyTorch) use C++ internally for their core computations.
+ **Stability** -> Fully supported and documented as opposed to **Preview C++23** which MSVC hasn't marked as stable as of `24/08/25` and therefore isn't fully documented.

## Project Organisation

<h4 style="text-align: center;">Project File Structure</h4>



<pre style="
display:flex;
justify-content: center;
padding-top:0px;
padding-bottom:10px;
">
<div>
Neuron
├───data  
│   ├───txtImages       → Text representations of images (.txt)
│   └───images          → Actual image files (.png)
├───docs  
│   └───documentation   → Project documentation files
├───libs   
│   ├───fonts           → Font files used by the project
│   ├───glfw3           → GLFW library (for window/input handling)
│   ├───imgui           → ImGui library (for GUI)
│   └───stb             → stb libraries (single-file graphics/util libs)
├───src  
│   ├───header          → Header files (.h)
│   └───implementation  → Source code implementation files (.cpp)
 
</div>
</pre>
<h4 style="text-align: center;">Folder Contents</h4>

<pre style="display:flex;justify-content: center">
<div style="margin-right:0px">
<h3 >data</h3>
txtImages
├── data77.txt  
├── data1414.txt  
├── data2121.txt  
└── data2828.txt   

images
├───77
│   ├── 0.png
│   ├── ...
│   └── 9.png
├───1414
│   ├── 0.png
│   ├── ...
│   └── 9.png
├───2121
│   ├── 0.png
│   ├── ...
│   └── 9.png
├───2828
│   ├── 0.png
│   ├── ...
│   └── 9.png
</div >
<div style="margin-right:5px;margin-left:20px;">
<h3 >libs</h3>
fonts
└── cascadiaMono.ttf  

glfw3
├───include
│   ├───GLFW
│   │   ├── glfw3.h
│   │   └── glfw3native.h
│   ├───GLFW
│   │   ├───Debug
│   │   │   ├── glfw3.lib
│   │   │   └── glfw3.pdb
│   │   ├───Release
│   │   │   └── glfw3.lib
│   │   ├── glfw3.dll
│   │   ├── glfw3.lib
│   │   ├── glfw3_mt.lib
│   │   └── glfw3dll.lib

imgui
├── imconfig.h
├── imgui.cpp
├── imgui.h
├── imgui_demo.cpp
├── imgui_draw.cpp
├── imgui_impl_glfw.cpp
├── imgui_impl_glfw.h
├── imgui_impl_opengl3.cpp
├── imgui_impl_opengl3.h
├── imgui_impl_opengl3_loader.h
├── imgui_internal.h
├── imgui_tables.cpp
├── imgui_widgets.cpp
├── imstb_rectpack.h
├── imstb_textedit.h
└── imstb_truetype.h

</div>
<div style="margin-right:0px">
<h3 >src</h3>
header
├── Activations.h
├── DataSet.h
├── InputDataManager.h
├── NetworkVisualiserInterface.h
├── NeuralNetwork.h
├── Statistics.h
├── StopWatch.h
├── Threader.h
├── Visualiser.h
└── VisualUpdate.h

implementation
├── Activations.cpp
├── DataSet.cpp
├── InputDataManager.cpp
├── main.cpp
├── NetworkVisualiserInterface.cpp
├── NeuralNetwork.cpp
├── Statistics.cpp
├── StopWatch.cpp
├── Threader.cpp
├── Visualiser.cpp
└── VisualUpdate.cpp


</div >
</pre>
## Class Diagrams
