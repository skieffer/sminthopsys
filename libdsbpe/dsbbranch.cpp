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

#include "dsbbranch.h"
#include "dsbclone.h"
#include "dsbspecies.h"
#include "dsbreaction.h"
#include "dsbpathway.h"
#include "dsbfork.h"
#include "dsbnode.h"

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

QList<DSBNode*> DSBBranch::getOwnNodes()
{
    int s = nodes.size();
    int n = cycle ? s - 1 : s;
    return nodes.mid(0,n);
}

QSizeF DSBBranch::layout()
{
    QList<DSBNode*> own = getOwnNodes();
    setMainConnections(own);
    // Put lead node at (0,0).
    // So base point of branch is center of lead node.
    qreal x = 0, y = 0, w = 0, h = 0;
    qreal width = 0, height = 0;
    DSBNode *node = own.first();
    node->setRelPt(QPointF(x,y));
    QSizeF size = node->layout();
    h = size.height();
    y += h/2;
    height += h;
    qreal gap = 20;
    // Now place all other nodes according to their size.
    for (int i = 1; i < own.size(); i++)
    {
        y += gap;
        height += gap;
        node = own.at(i);
        QSizeF size = node->layout();
        h = size.height();
        y += h/2;
        node->setRelPt(QPointF(x,y));
        y += h/2;
        height += h;
        w = size.width();
        width = w > width ? w : width;
    }
    m_size = QSizeF(width, height);
    return m_size;
}

void DSBBranch::setMainConnections(QList<DSBNode*> own)
{
    for (int i = 0; i < own.size(); i++)
    {
        DSBNode *n = own.at(i);
        DSBReaction *reac = dynamic_cast<DSBReaction*>(n);
        DSBClone *cl = dynamic_cast<DSBClone*>(n);
        if (reac)
        {
            // predecessor
            n = getPredecessor(reac);
            if (n)
            {
                DSBClone *c = dynamic_cast<DSBClone*>(n);
                reac->setMainInput(c);
            }
            else { reac->setMainInput(NULL); }
            // successor
            n = getSuccessor(reac);
            if (n)
            {
                DSBClone *c = dynamic_cast<DSBClone*>(n);
                reac->setMainOutput(c);
            }
            else { reac->setMainOutput(NULL); }
        }
        else if (cl)
        {
            DSBFork *fork = m_pathway->getFork(cl);
            if (fork)
            {
                // predecessor
                n = getPredecessor(cl);
                if (n)
                {
                    DSBReaction *r = dynamic_cast<DSBReaction*>(n);
                    fork->setMainInput(r);
                }
                else { fork->setMainInput(NULL); }
                // successor
                n = getSuccessor(cl);
                if (n)
                {
                    DSBReaction *r = dynamic_cast<DSBReaction*>(n);
                    fork->setMainOutput(r);
                }
                else { fork->setMainOutput(NULL); }
            }
        }
        else
        {
            // TODO: report error
        }
    }
}

void DSBBranch::setPathway(DSBPathway *pw)
{
    m_pathway = pw;
}

DSBPathway *DSBBranch::getPathway()
{
    return m_pathway;
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
    QList<DSBNode*> own = getOwnNodes();
    for (int i = 0; i < own.size(); i++)
    {
        DSBNode *node = own.at(i);
        node->drawRelTo(r);
    }
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
