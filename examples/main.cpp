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

#include <iostream>
#include <fstream>
#include <string>

static const constexpr char usage[] =
"Usage: roadmap edges nodes maxpaths output\n"
"Generates an KML file with the shortest paths from the given nodes to all connected nodes\n\n"
"    edges      path to the file with edges\n"
"    nodes      path to the file with nodes' coordinates\n"
"    maxpaths   the maximum number of paths to export\n"
"    output     the path to the output file.\n";

int main(int argc, char **argv)
{
    if (argc != 5) {
        std::cerr << "Incorrect number of arguments\n";
        std::cerr << usage;
        return 1;
    }

    const std::string edgesFile(argv[1]);
    const std::string nodesFile(argv[2]);
    const size_t maxPaths(std::stoi(argv[3]));
    const std::string outputFile(argv[4]);

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

    std::ofstream kmlFile(outputFile);
    if (!kmlFile) {
        std::cerr << "Failed to open file" << outputFile << std::endl;
        return 1;
    }

    kmlFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n" \
        "  <Document>\n" \
        "    <name>Paths</name>\n" \
        "      <Style id=\"redPoly\">\n" \
        "        <LineStyle>\n" \
        "          <color>ff0000ff</color>\n" \
        "          <width>0.5</width>\n" \
        "        </LineStyle>\n" \
        "      </Style>\n";

    size_t routeId = {};

    for (auto && path : paths) {
        if (routeId > maxPaths) {
            break;
        }

        kmlFile << "    <Placemark>\n" \
        "      <name>Route " << routeId++ << "</name>\n" \
        "      <styleUrl>#redPoly</styleUrl>\n" \
        "      <LineString>\n" \
        "        <coordinates>\n";

        for (const auto &nodeId : path) {
            const auto &point = nodes[nodeId];
            kmlFile << std::setprecision(15) << point.first << ',' << point.second << ",0" << '\n';
        }

        kmlFile << "        </coordinates>\n"
                   "      </LineString>\n"
                   "    </Placemark>\n";
    }

    kmlFile << "  </Document>\n" \
               "</kml>";

    return 0;
}
