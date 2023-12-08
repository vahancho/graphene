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

#ifndef __KMLFILE_H__
#define __KMLFILE_H__

#include <fstream>
#include <string>
#include <functional>

class KmlFile
{
public:
    KmlFile(const std::string &filePath);
    ~KmlFile();

    operator bool() const;

    template<typename Container, typename ToStringConverter>
    void addPlacemark(Container && path, ToStringConverter func);

private:
    std::ofstream m_file;
};

template<typename Container, typename ToStringConverter>
void KmlFile::addPlacemark(Container && path, ToStringConverter func)
{
    static size_t routeId{};

    if (!m_file) {
        return;
    }

    m_file << "    <Placemark>\n"
        "      <name>Route " << routeId++ << "</name>\n"
        "      <styleUrl>#redPoly</styleUrl>\n"
        "      <LineString>\n"
        "        <coordinates>\n";

    for (auto && node : path) {
        m_file << func(std::move(node)) << ",0" << '\n';
    }

    m_file << "        </coordinates>\n"
        "      </LineString>\n"
        "    </Placemark>\n";
}

#endif // __KMLFILE_H__

