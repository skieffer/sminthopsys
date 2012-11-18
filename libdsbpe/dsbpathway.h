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

#ifndef DSBPATHWAY_H
#define DSBPATHWAY_H

#include <QList>
#include <QMap>

#include "dsbreclayout.h"

namespace dunnart {

class DSBBranch;
class DSBFork;
class DSBNode;
class DSBClone;

class DSBPathway : public DSBRecLayout
{
public:
    // Constructors
    DSBPathway(DSBNode *head, QList<DSBBranch*> branches);
    // Get and set
    DSBFork *getFork(DSBClone *cl);
    DSBBranch *getBranch(DSBNode *node);
    // RecLayout methods
    QSizeF layout();
    void setRelPt(QPointF p);
    void drawRelTo(QPointF q);
    void drawAt(QPointF r);
    void redraw();
    QSizeF getSize();
    // Other
    QMap<DSBNode*, DSBBranch*> countBranchPoints(QList<DSBBranch*> branches);
    void setFirstBranch(DSBBranch *branch);
    void addBranch(DSBBranch *branch);

private:
    QPointF m_relpt;
    QPointF m_basept;
    QSizeF m_size;
    DSBNode *m_headNode;
    QList<DSBBranch*> m_branches;
    QList<DSBNode*> m_allNodes;
    QMap<DSBNode*, DSBBranch*> m_branchMembership;
    QMap<DSBNode*, DSBFork*> m_forkMembership;

    void addBranchNodes(DSBBranch *branch);
};

}

#endif // DSBPATHWAY_H
