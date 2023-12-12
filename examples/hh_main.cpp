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

// Dataset: https://data.europa.eu/data/datasets/19a39b3a-2d9e-4805-a5e6-56a5ca3ec8cb?locale=en

#include "graphene.h"
#include "kmlfile.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

class Node
{
public:
    Node(double lon, double lat)
        :
            m_lon(lon),
            m_lat(lat)
    {}

    bool operator<(const Node &other) const
    {
        if (m_lon < other.m_lon) {
            return true;
        } else if (equal(m_lon, other.m_lon)) {
            return m_lat < other.m_lat;
        }
        return false;
    }

    bool operator==(const Node &other) const
    {
        return equal(m_lon, other.m_lon) && equal(m_lat, other.m_lat);
    }

    /// The distance between two geodetic points (spherical earth model).
    double distance(const Node &node, double radius = 6371e3)
    {
        static constexpr const double radiansInDegree = M_PI / 180.0;

        auto toRadians = [] (double degrees) {
            return degrees * radiansInDegree;
        };

        const auto phi1 = toRadians(latitude());
        const auto lambda1 = toRadians(longitude());
        const auto phi2 = toRadians(node.latitude());
        const auto lambda2 = toRadians(node.longitude());
        const auto deltaPhi = phi2 - phi1;
        const auto deltaLambda = lambda2 - lambda1;

        const auto a = std::sin(deltaPhi / 2.0) * std::sin(deltaPhi / 2.0) +
            std::cos(phi1) * std::cos(phi2) *
            std::sin(deltaLambda / 2.0) * std::sin(deltaLambda / 2.0);
        const auto c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));

        return radius * c;
    }

    std::string toString() const
    {
        std::stringstream stream;
        stream << std::setprecision(15) << m_lon << ',' << m_lat;
        return stream.str();
    }

private:

    bool equal(double p1, double p2) const
    {
        return (std::abs(p1 - p2) <= 0.000000000001 * std::min(std::abs(p1), std::abs(p2)));
    }

    double latitude() const
    {
        return m_lat;
    }

    double longitude() const
    {
        return m_lon;
    }

    double m_lon;
    double m_lat;
};


int main(int argc, char **argv)
{
    const auto binDirPath = std::filesystem::path(argv[0]).parent_path();

    const std::vector<std::filesystem::path> roadNetworkFiles =
    {
        binDirPath / "data/app_strassennetz_inspire_bab_EPSG_4326.csv",
        binDirPath / "data/app_strassennetz_inspire_bfs_EPSG_4326.csv",
        binDirPath / "data/app_strassennetz_inspire_bod_EPSG_4326.csv",
        binDirPath / "data/app_strassennetz_inspire_eu_EPSG_4326.csv",
        binDirPath / "data/app_strassennetz_inspire_g_EPSG_4326.csv"
    };

    const auto outputFile = binDirPath / "data/hh_roadmap_output.kml";

    Graphene<Node> graph;
    std::unordered_map<std::string, Graphene<Node>::Path> streets;

    // Create the road network.
    for (auto && roadNetworkFile : roadNetworkFiles) {
        std::ifstream fileEdges(roadNetworkFile);
        if (!fileEdges) {
            std::cerr << "Failed to open file" << roadNetworkFile << std::endl;
            return 1;
        }

        std::string line;
        while (std::getline(fileEdges, line)) {
            static const std::regex regexpSplit{ R"((?:^|,)("[^"]*(?:""[^"]*)*"|[^,]*?)(?=,|$))" };
            static const std::regex regexpGeom{ R"(((\d+\.\d+) (\d+\.\d+)))" };

            std::sregex_iterator iter(line.begin(), line.end(), regexpSplit);
            std::sregex_iterator end;

            if (iter == end) {
                continue;
            }

            assert(std::distance(iter, end) == 14);
            auto it = std::next(iter, 4);
            assert(it->size() == 2);
            const auto &streetName = (*it)[1];

            iter = std::sregex_iterator(line.begin(), line.end(), regexpGeom);

            if (iter != end) {
                auto r = streets.emplace(streetName, Graphene<Node>::Path{});

                while (iter != end) {
                    assert(iter->size() == 4);
                    std::smatch match = *iter;
                    const auto &lon = match[2].str();
                    const auto &lat = match[3].str();

                    auto &path = r.first->second;
                    Node node{ std::stod(lon), std::stod(lat) };

                    if (!path.empty()) {
                        graph.addEdge(path.back(), node);
                        // This is an undirected graph.
                        graph.addEdge(node, path.back());
                    }
                    path.emplace_back(node);

                    ++iter;
                }
            }
        }
    }

    std::string fromStreet;
    std::string toStreet;
    auto itFrom = streets.cend();
    auto itTo   = streets.cend();
    bool entryError = false;

    auto prompt = [&] (const std::string &title) {
        if (!entryError) {
            std::cout << title << '\n';
        }
        std::getline(std::cin, fromStreet);
        auto it = streets.find(fromStreet);
        if (it == streets.cend()) {
            std::cout << "Not found. Try again, please\n";
            entryError = true;
        } else {
            entryError = false;
        }
        return it;
    };

    while (true) {
        if (itFrom == streets.cend() && (itFrom = prompt("Enter start street")) == streets.cend()) {
            continue;
        }

        if (itTo == streets.cend() && (itTo = prompt("Enter destination street")) == streets.cend()) {
            continue;
        }

        // Shortest path
        auto weight = [&] (Node x, Node y) {
            return x.distance(y);
        };
        auto sp = graph.shortestPath(itFrom->second.front(),
                                     itTo->second.front(),
                                     weight);

        if (!sp.empty()) {
            // Calculate the route length.
            double distance{};
            for (size_t i = 0; i < sp.size() - 1; ++i) {
                distance += sp[i].distance(sp[i + 1]);
            }
            std::cout << std::setprecision(6) << "The route found. Length " << distance << " m" << std::endl;
        } else {
            std::cout << "Route not found" << std::endl;
        }

        KmlFile kmlFile(outputFile.string());
        if (!kmlFile) {
            std::cerr << "Failed to open file" << outputFile << std::endl;
            return 1;
        }

        kmlFile.addPlacemark(sp, [] (Node &&node) { return node.toString(); });

        itFrom = streets.cend();
        itTo   = streets.cend();
    }

    return 0;
}
