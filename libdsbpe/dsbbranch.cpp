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

#include "dsbbranch.h"
#include "dsbclone.h"
#include "dsbspecies.h"
#include "dsbreaction.h"

namespace dunnart {

DSBNode *DSBBranch::getPredecessor(DSBNode *node)
{
    DSBNode *pred = 0;
    int i = nodes.indexOf(node);
    if (i>0)
    {
        pred = nodes.at(i-1);
    }
    return pred;
}

DSBNode *DSBBranch::getSuccessor(DSBNode *node)
{
    DSBNode *succ = 0;
    int i = nodes.indexOf(node);
    if (i>=0 && i+1 < nodes.size())
    {
        succ = nodes.at(i+1);
    }
    return succ;
}

QSizeF DSBBranch::layout()
{
    // TODO
    m_size = QSizeF(10,10);
    return m_size;
}

QSizeF DSBBranch::getSize()
{
    return m_size;
}

void DSBBranch::setRelPt(QPointF p)
{
    m_relpt = p;
}

void DSBBranch::drawRelTo(QPointF q)
{
    QPointF r = m_relpt + q;
    drawAt(r);
}

void DSBBranch::redraw()
{
    drawAt(m_basept);
}

void DSBBranch::drawAt(QPointF r)
{
    m_basept = r;
    // TODO
}

QString DSBBranch::toString()
{
    QString s;
    if (cycle)
    {
        s += "\nCycle:------------------------------------------\n";
    } else {
        s += "\nBranch:-----------------------------------------\n";
    }
    if (parent)
    {
        s += "( ";
        DSBClone *cl = dynamic_cast<DSBClone*>(parent);
        DSBReaction *reac = dynamic_cast<DSBReaction*>(parent);
        if (cl)
        {
            s += cl->getSpecies()->getName();
            s += "-"+cl->getCloneId();
        }
        else if (reac)
        {
            s += reac->getReactionId();
        }
        s += " ) ";
    }
    for (int j = 0; j < nodes.size(); j++)
    {
        DSBClone *cl = dynamic_cast<DSBClone*>(nodes.at(j));
        DSBReaction *reac = dynamic_cast<DSBReaction*>(nodes.at(j));
        if (cl)
        {
            s += cl->getSpecies()->getName();
            s += "-"+cl->getCloneId();
        }
        else if (reac)
        {
            s += " --[ " + reac->getReactionId() + " ]--> ";
        }
    }
    return s;
}

}
