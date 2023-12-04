# The Graphene

`Graphene` is a header-only library that implements an abstract graph. The class template allows
creation of graphs of any types of nodes both directed and undirected with
the custom weight function.

[![Latest release](https://img.shields.io/github/v/release/vahancho/graphene?include_prereleases)](https://github.com/vahancho/graphene/releases)
[![Build and test (CMake)](https://github.com/vahancho/graphene/actions/workflows/cmake.yml/badge.svg)](https://github.com/vahancho/graphene/actions/workflows/cmake.yml)
[![Code coverage](https://codecov.io/gh/vahancho/graphene/branch/master/graph/badge.svg)](https://codecov.io/gh/vahancho/graphene)

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

## Example (California road network)

# See also

[California Road Network](https://users.cs.utah.edu/~lifeifei/SpatialDataset.htm)
