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

#include "kmlfile.h"

#include <iostream>

KmlFile::KmlFile(const std::string &filePath)
    :
        m_file(filePath)
{
    if (m_file) {
        m_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
            "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n" \
            "  <Document>\n" \
            "    <name>Paths</name>\n" \
            "      <Style id=\"redPoly\">\n" \
            "        <LineStyle>\n" \
            "          <color>ff0000ff</color>\n" \
            "          <width>0.5</width>\n" \
            "        </LineStyle>\n" \
            "      </Style>\n";
    }
}

KmlFile::~KmlFile()
{
    if (m_file) {
        m_file << "  </Document>\n" \
                  "</kml>";
    }
}

KmlFile::operator bool() const
{
    return (bool)m_file;
}

