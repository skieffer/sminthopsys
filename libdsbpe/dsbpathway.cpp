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

#include <assert.h>

#include "dsbpathway.h"
#include "dsbnode.h"

namespace dunnart {

DSBPathway::DSBPathway(DSBBranch *primaryBranch, QList<DSBBranch *> secondaryBranches)
{
    m_headNode = primaryBranch->nodes.first();
    //...
}

DSBPathway::DSBPathway(DSBNode *endpt, QList<DSBBranch *> branches)
{
    // Throw away length-1 branches, and find branch starting
    // with selected endpt clone.
    QList<DSBBranch*> otherBranches;
    DSBBranch *mainBranch = 0;
    for (int i = 0; i < branches.size(); i++)
    {
        DSBBranch *b = branches.at(i);
        DSBNode *n = b->nodes.first();
        if (n == endpt) { mainBranch = b; }
        else if (b->nodes.size() > 1) { otherBranches.append(b); }
    }
    assert(mainBranch!=0);
    qDebug() << "\nMain branch: " << mainBranch->toString();
    qDebug() << "\nOther branches: ";
    for (int i = 0; i < otherBranches.size(); i++) {
        qDebug() << otherBranches.at(i)->toString();
    }

    // Count branch points in otherBranches.
    QMap<DSBNode*, DSBBranch*> bpCounts = countBranchPoints(otherBranches);
    // Get list of nodes that occur as branch points.
    QList<DSBNode*> branchPts = bpCounts.uniqueKeys();
    // Tell them their "branch head number", i.e. the number of branches
    // of which they are the head node.
    for (int i = 0; i < branchPts.size(); i++)
    {
        DSBNode *n = branchPts.at(i);
        n->setBranchHeadNumber( bpCounts.count(n) );
    }

}

/* Let p1, p2, ..., pn be the parent nodes of the passed branches
  b1, b2, ..., bn. We construct the map with pi --> bi for each i.
  Then for any node n, map.count(n) will say how many branches have
  node n as parent, and map.values(n) will return the list of them.
  */
QMap<DSBNode*, DSBBranch*> DSBPathway::countBranchPoints(QList<DSBBranch *> branches)
{
    QMap<DSBNode*, DSBBranch*> map;
    for (int i = 0; i < branches.size(); i++)
    {
        DSBBranch *b = branches.at(i);
        DSBNode *p = b->parent;
        if (p)
        {
            map.insert(p,b);
        }
    }
    return map;
}

QSizeF DSBPathway::layout()
{
    // TODO
}

void DSBPathway::setRelPt(QPointF p)
{
    m_relpt = p;
}

void DSBPathway::drawRelTo(QPointF q)
{
    QPointF r = m_relpt + q;
    drawAt(r);
}

void DSBPathway::redraw()
{
    drawAt(m_basept);
}

void DSBPathway::drawAt(QPointF r)
{
    m_basept = r;
    // TODO
}

}




















