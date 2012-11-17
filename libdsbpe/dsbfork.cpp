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

#include "dsbfork.h"

namespace dunnart {

DSBFork::DSBFork(DSBClone *c)
{
    m_centre = c;
}

void DSBFork::addUpstream(DSBReaction *reac)
{
    m_upstreamReacs.append(reac);
}

void DSBFork::addDownstream(DSBReaction *reac)
{
    m_downstreamReacs.append(reac);
}

QSizeF DSBFork::layout()
{
    // TODO
    m_size = QSizeF(10,10);
    return m_size;
}

QSizeF DSBFork::getSize()
{
    return m_size;
}

void DSBFork::setRelPt(QPointF p)
{
    m_relpt = p;
}

void DSBFork::drawRelTo(QPointF q)
{
    QPointF r = m_relpt + q;
    drawAt(r);
}

void DSBFork::redraw()
{
    drawAt(m_basept);
}

void DSBFork::drawAt(QPointF r)
{
    m_basept = r;
    // TODO
}


}
