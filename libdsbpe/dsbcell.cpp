/*
 * DSBPE plugin - For editing Systems Biology pathways.
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

#include <QtGui>

#include "dsbcell.h"
#include "dsbcompartment.h"

namespace dunnart {

DSBCell::DSBCell() {}

void DSBCell::addCompartment(DSBCompartment *comp)
{
    m_compartments.append(comp);
    comp->setCell(this);
}

void DSBCell::setCompartments(QList<DSBCompartment *> comps)
{
    m_compartments.clear();
    for (int i = 0; i < comps.size(); i++)
    {
        DSBCompartment *comp = comps.at(i);
        addCompartment(comp);
    }
}

QSizeF DSBCell::layout()
{
    // TODO: Implement more layout methods.
    return rowLayout();
}

QSizeF DSBCell::rowLayout()
{
    int pad = 100;
    int numComps = m_compartments.size();
    int *widths = new int[numComps];
    int maxHeight = 0;
    for (int i = 0; i < numComps; i++)
    {
        DSBCompartment *comp = m_compartments.at(i);
        QSizeF size = comp->layout();
        widths[i] = size.width();
        int h = size.height();
        if (h > maxHeight) { maxHeight = h; }
    }
    int x = 0, y = 0;
    for (int i = 0; i < numComps; i++)
    {
        DSBCompartment *comp = m_compartments.at(i);
        comp->setRelPt(QPointF(x,y));
        x += widths[i] + pad;
    }
    delete[] widths;
    m_size = QSizeF(x,maxHeight);
    return m_size;
}

void DSBCell::setRelPt(QPointF p)
{
    m_relpt = p;
}

void DSBCell::drawRelTo(QPointF q)
{
    QPointF r = m_relpt + q;
    for (int i = 0; i < m_compartments.size(); i++)
    {
        m_compartments.at(i)->drawRelTo(r);
    }
}

}
