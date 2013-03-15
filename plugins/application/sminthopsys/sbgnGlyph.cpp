/*
 * Sminthopsys - Dunnart Systems Biology plugin
 *
 * Copyright (C) 2011-2012  Monash University
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 *
 *
 * Author(s): Steven Kieffer  <http://skieffer.info>
*/

#include "sbgnGlyph.h"

namespace dunnart {

SBGNGlyph::SBGNGlyph(QDomNode node)
{
    QDomNamedNodeMap attrs = node.attributes();
    m_id = attrs.namedItem("id").toAttr().value();
    m_class = attrs.namedItem("class").toAttr().value();

    QDomNode label = node.namedItem("label");
    QDomNamedNodeMap labelAttrs = label.attributes();
    m_labelText = labelAttrs.namedItem("text").toAttr().value();

    // Get bbox of label, if it is specified.
    if (label.hasChildNodes())
    {
        QDomNode labelBboxNode = label.namedItem("bbox");
        if (!labelBboxNode.isNull())
        {
            QDomNamedNodeMap lbboxAttrs = labelBboxNode.attributes();
            float x = lbboxAttrs.namedItem("x").toAttr().value().toFloat();
            float y = lbboxAttrs.namedItem("y").toAttr().value().toFloat();
            float w = lbboxAttrs.namedItem("w").toAttr().value().toFloat();
            float h = lbboxAttrs.namedItem("h").toAttr().value().toFloat();
            m_labelBbox = QRectF(x,y,w,h);
        }
    }

    // Is it marked as a clone?
    QDomNode cloneMarker = node.namedItem("clone");
    m_isCloned = !cloneMarker.isNull();

    // Bbox of node
    QDomNode bbox = node.namedItem("bbox");
    QDomNamedNodeMap bboxAttrs = bbox.attributes();
    float x = bboxAttrs.namedItem("x").toAttr().value().toFloat();
    float y = bboxAttrs.namedItem("y").toAttr().value().toFloat();
    float w = bboxAttrs.namedItem("w").toAttr().value().toFloat();
    float h = bboxAttrs.namedItem("h").toAttr().value().toFloat();
    m_Bbox = QRectF(x,y,w,h);

}

QString SBGNGlyph::toString()
{
    QString s = "";
    s += "SBGN Glyph:\n";
    s += QString("  id: %1, class: %2\n").arg(m_id).arg(m_class);
    s += (m_isCloned ? "  clone\n" : "");
    s += QString("  label: %1\n").arg(m_labelText);
    //
    return s;
}

}









