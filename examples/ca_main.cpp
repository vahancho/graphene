/**********************************************************************************
*  MIT License                                                                    *
*                                                                                 *
*  Copyright (c) 2023 Vahan Aghajanyan <vahancho@gmail.com>                       *
*                                                                                 *
*  Permission is hereby granted, free of charge, to any person obtaining a copy   *
*  of this software and associated documentation files (the "Software"), to deal  *
*  in the Software without restriction, including without limitation the rights   *
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
*  copies of the Software, and to permit persons to whom the Software is          *
*  furnished to do so, subject to the following conditions:                       *
*                                                                                 *
*  The above copyright notice and this permission notice shall be included in all *
*  copies or substantial portions of the Software.                                *
*                                                                                 *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
*  SOFTWARE.                                                                      *
***********************************************************************************/

#include "graphene.h"
#include "kmlfile.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

static const constexpr char usage[] =
"Usage: roadmap maxpaths output\n"
"Generates an KML file with the shortest paths from the given nodes to all connected nodes\n\n"
"    maxpaths   the maximum number of paths to export\n";

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Incorrect number of arguments\n";
        std::cerr << usage;
        return 1;
    }

    const auto binDirPath = std::filesystem::path(argv[0]).parent_path();

    const auto edgesFile  = binDirPath / "data/edges.txt";
    const auto nodesFile  = binDirPath / "data/nodes_lon_lat.txt";
    const auto outputFile = binDirPath / "data/ca_roadmap_output.kml";
    const size_t maxPaths(std::stoi(argv[1]));

    Graphene<int, GraphType::Undirected> graph;
    std::map<std::pair<int, int>, double> edges;
    std::map<int, std::pair<double, double>> nodes;

    std::ifstream fileEdges(edgesFile);
    if (!fileEdges) {
        std::cerr << "Failed to open file" << edgesFile << std::endl;
        return 1;
    }

    std::ifstream fileNodes(nodesFile);
    if (!fileNodes) {
        std::cerr << "Failed to open file" << nodesFile << std::endl;
        return 1;
    }

    int id{}, start{}, end{};
    double distance{};

    while (fileEdges >> id >> start >> end >> distance) {
        edges.emplace(std::make_pair(start, end), distance);
        // This is an undirected graph.
        edges.emplace(std::make_pair(end, start), distance);
        graph.addEdge(start, end);
    }

    double lon{}, lat{};
    while (fileNodes >> id >> lon >> lat) {
        nodes.emplace(id, std::make_pair(lon, lat));
    }

    auto weight = [&](int x, int y) -> double {
        return edges[{x, y}];
    };

    // Extract all paths that link to the given node
    auto paths = graph.shortestPaths(1, weight);

    KmlFile kmlFile(outputFile.string());
    if (!kmlFile) {
        std::cerr << "Failed to open file" << outputFile << std::endl;
        return 1;
    }

    for (auto && path : paths) {
        kmlFile.addPlacemark(path, [&] (int nodeId) {
            const auto &point = nodes[nodeId];
            std::stringstream stream;
            stream << std::setprecision(15) << point.first << ',' << point.second;
            return stream.str();
        });
    }

    return 0;
}
