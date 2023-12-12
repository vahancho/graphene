# The Graphene

`Graphene` is a header-only library that implements an abstract graph. The class template allows
creation of graphs of any types of nodes both directed and undirected with
the custom weight function.

[![Latest release](https://img.shields.io/github/v/release/vahancho/graphene?include_prereleases)](https://github.com/vahancho/graphene/releases)
[![Build and test (CMake)](https://github.com/vahancho/graphene/actions/workflows/cmake.yml/badge.svg)](https://github.com/vahancho/graphene/actions/workflows/cmake.yml)
[![Code coverage](https://codecov.io/gh/vahancho/graphene/graph/badge.svg?token=YLMFIEZBGG)](https://codecov.io/gh/vahancho/graphene)

## Prerequisites

No special requirements except *C++17* compliant compiler. The class is tested with
*gcc* and *MSVC* compilers. In order to build and run unit tests
for this project you are required to have Google Test library installed on the system.
For more details see the CI badges or [GitHub Actions](https://github.com/vahancho/graphene/actions).

## Shortest paths

The library calculates shortest path between two nodes using the Dijkstra algorithm.
However the term "shortest" is an abstraction, because the distance between
to nodes is just a particular notion of the weight of the graph edge. `Graphene`
allows to define custom weight functions, so that the algorithm will calculate
not only the shortest path, but the path that corresponds to the minimal overall
"weight".

## API and usage

```cpp
#include "graphene.h"

// Declare a graph with nodes as integer values
Graphene<int> graph;

graph.addNode(0);
graph.addNode(42);

graph.addEdge(0, 42);
graph.addEdge(1, 2);

```

Calculate the shortest path between two nodes

```cpp
#include "graphene.h"

// 1--2--5--8
//  \     \/
//   10---6---7
//
Graphene<int> graph;

auto weightFunction = [](int x, int y) -> int {
    return std::abs(x - y);
};

graph.addEdge(1, 2);
graph.addEdge(2, 5);
graph.addEdge(5, 6);
graph.addEdge(5, 8);
graph.addEdge(8, 6);
graph.addEdge(1, 10);
graph.addEdge(10, 6);
graph.addEdge(6, 7);

path = graph.shortestPath(1, 6, weightFunction);
// path = {1, 2, 5, 6}
```

## Build and test

In order to build the project please use the following commands:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTING=True
cmake --build build --config Release
```

To run the unit tests

```bash
ctest -C Release --verbose
```

## Examples

The `ca_roadmap` and `hh_roadmap` examples demonstrate how to use `Graphene` library
to create road maps and find shortest paths between two nodes.

### Hamburger road network

The Hamburger road network is an interactive sample application which uses the large dataset
[Road network Hamburg (INSPIRE)](https://data.europa.eu/data/datasets/19a39b3a-2d9e-4805-a5e6-56a5ca3ec8cb?locale=en)
provided by Behörde für Verkehr und Mobilitätswende (BVM)
[Data license Germany – attribution – Version 2.0](http://www.govdata.de/dl-de/by-2-0).
The data set is represented by a number of CSV files which we parse and extract
the streets network (geodetic coordinates and street names) of the city.

The `hh_roadmap` application's usage example is:

```bash
./hh_roadmap
```

The application prompts for the start and end street names and calculates the
shortest path between them as well as the distance. The resulting `KML` file
saves into the `/data/hh_roadmap_output.kml` file.

#### The Hamburger road network

<img src="./examples/data/hamburg_roadmap.png" alt="Hamburg Road Network" width="400">

#### The shortest path between two streets

<img src="./examples/data/hamburg_shortestpath.png" alt="The shortest path between two streets" width="600">

### California road network

As an input we used the [California Road Network](https://users.cs.utah.edu/~lifeifei/SpatialDataset.htm)
which represented by two files: `/examples/data/nodes_lon_lat.txt` with the nodes'
geodetic coordinates and `/examples/data/edges.txt` file with nodes indexes and
distance between them.
Using this data we created an undirected graph and use `Graphene::shortestPaths()`
function to find all paths that connect a node with all others. The tool exports
the results as a [KML](https://en.wikipedia.org/wiki/Keyhole_Markup_Language) file
which can be loaded, for example, into Google Earth application for visualization.

The `ca_roadmap` application's usage example is:

```bash
./ca_roadmap 10000
```
where `10000` is the max. number of paths to export.

<img src="./examples/data/california_roadmap.png" alt="California Road Network" width="350">

# See also

[California Road Network](https://users.cs.utah.edu/~lifeifei/SpatialDataset.htm)
