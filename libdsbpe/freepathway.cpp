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

#include "freepathway.h"
#include "dsbreaction.h"

namespace dunnart{

FreePathway::FreePathway(QList<DSBClone *> clones, QList<DSBReaction *> reacs) :
    m_clones(clones),
    m_reactions(reacs)
{
    //...
}

QSizeF FreePathway::getSize()
{
    return m_size;
}

void FreePathway::setRelPt(QPointF p)
{
    m_relpt = p;
}

void FreePathway::drawRelTo(QPointF q)
{
    QPointF r = m_relpt + q;
    drawAt(r);
}

void FreePathway::redraw()
{
    drawAt(m_basept);
}

QSizeF FreePathway::layout()
{
    foreach (DSBReaction *reac, m_reactions)
    {
        reac->layout();
    }
    m_size = QSizeF(1000,1000);
    return m_size;
}

void FreePathway::drawAt(QPointF r)
{
    m_basept = r;
    foreach (DSBReaction *reac, m_reactions)
    {
        reac->drawRelTo(m_basept);
    }
}

}
