# Application Code Files to Graph

## About
Generates a graphical representation of the relationship between files in the form of a graph.
The graphs that this application creates are visible in the [Graphs](#Graphs) section.

Created using C++, [JSON](https://github.com/Cezary-Androsiuk/petanque-team) from nlohmann, [Log](https://github.com/Cezary-Androsiuk/) (no repository yet) and [Graphviz](https://gitlab.com/graphviz/graphviz).


## Usage
Open the file ```startup.json```, set the ```"directory path"``` to your project directory, and then run ```ApplicationCodeFilesToGraph.exe```. After that, an image will be created in the ```"output image path"``` (by default, in the same directory as the executable).

Optionally, you can adjust the values in the startup file to support other languages or to specify your preferences for which files/directories should be included.


## Images

### Graphs
#### CPP Relations
![graph - CPP relations](https://github.com/Cezary-Androsiuk/application-code-files-to-graph/blob/master/graphs/ApplicationCodeFilesToGraph_0.png)

#### CPP relations - no singletons (Log.*)
![graph - CPP relations - no singletons](https://github.com/Cezary-Androsiuk/application-code-files-to-graph/blob/master/graphs/ApplicationCodeFilesToGraph_1.png)
