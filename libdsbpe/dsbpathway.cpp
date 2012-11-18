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
#include "dsbbranch.h"
#include "dsbclone.h"
#include "dsbreaction.h"
#include "dsbfork.h"

namespace dunnart {

DSBPathway::DSBPathway(DSBNode *head, QList<DSBBranch *> branches)
{
    // Find branch starting with selected endpt clone.
    QList<DSBBranch*> otherBranches;
    DSBBranch *mainBranch = 0;
    for (int i = 0; i < branches.size(); i++)
    {
        DSBBranch *b = branches.at(i);
        DSBNode *n = b->nodes.first();
        if (n == head) { mainBranch = b; }
        else { otherBranches.append(b); }
    }
    assert(mainBranch!=0);
    /*
    qDebug() << "\nMain branch: " << mainBranch->toString();
    qDebug() << "\nOther branches: ";
    for (int i = 0; i < otherBranches.size(); i++) {
        qDebug() << otherBranches.at(i)->toString();
    }
    */

    /*
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
    */

    setFirstBranch(mainBranch);

    for (int i = 0; i < otherBranches.size(); i++)
    {
        DSBBranch *b = otherBranches.at(i);
        addBranch(b);
    }

}

DSBFork *DSBPathway::getFork(DSBClone *cl)
{
    return m_forkMembership.value(cl, NULL);
}

DSBBranch *DSBPathway::getBranch(DSBNode *node)
{
    return m_branchMembership.value(node, NULL);
}

void DSBPathway::setFirstBranch(DSBBranch *branch)
{
    m_branches.clear();
    m_allNodes.clear();
    // FIXME: Do we need these maps?
    // If each Node belongs to exactly one branch, and each Clone to at most
    // one fork, then this is overkill.
    m_branchMembership.clear();
    m_forkMembership.clear();

    m_branches.append(branch);
    addBranchNodes(branch);
    branch->setPathway(this);

    m_headNode = branch->nodes.first();
}

void DSBPathway::addBranchNodes(DSBBranch *branch)
{
    for (int i = 0; i < branch->nodes.size(); i++)
    {
        DSBNode *n = branch->nodes.at(i);
        m_allNodes.append(n);
        // FIXME: Do not need insertMulti, if each Node belongs to exactly one branch.
        m_branchMembership.insertMulti(n,branch);
    }
}

void DSBPathway::addBranch(DSBBranch *branch)
{
    // Find parent node.
    DSBNode *parent = branch->parent;
    if (!m_allNodes.contains(parent))
    {
        // TODO: report error: could not find node in pathway
        return;
    }
    // No node should belong to more than one branch.
    // Can we also be sure that every node belongs to at least one branch?
    // For now we proceed as if we can.

    // If parent is a reaction, simply add the branch to it.
    DSBReaction *reac = dynamic_cast<DSBReaction*>(parent);
    if (reac)
    {
        DSBNode *first = branch->nodes.first();
        DSBClone *cl = dynamic_cast<DSBClone*>(first);
        assert(cl);
        reac->addOutputBranchHead(cl);
        return;
    }

    // Otherwise parent is a clone.
    DSBClone *cl = dynamic_cast<DSBClone*>(parent);
    //assert(cl);
    // Get existing fork, or create one.
    DSBFork *fork = 0;
    if (m_forkMembership.contains(parent))
    {
        fork = m_forkMembership.value(parent);
    }
    else
    {
        fork = new DSBFork(cl);
        fork->setPathway(this);
        // Get branch with parent in it.
        DSBBranch *main = m_branchMembership.value(cl);
        // Get predecessor and successor in branch, if any.
        DSBNode *n = main->getPredecessor(cl);
        if (n) {
            DSBReaction *reac = dynamic_cast<DSBReaction*>(n);
            fork->addUpstream(reac);
        }
        n = main->getSuccessor(cl);
        if (n) {
            DSBReaction *reac = dynamic_cast<DSBReaction*>(n);
            fork->addDownstream(reac);
        }
        // Register fork.
        m_forkMembership.insert(cl,fork);
    }
    // Add branch head as downstream member of fork.
    assert(fork);
    DSBNode *n = branch->nodes.first();
    DSBReaction *downstr = dynamic_cast<DSBReaction*>(n);
    fork->addDownstream(downstr);
    // And set main input to first reaction in branch.
    downstr->setMainInput(cl);

    // Add new branch to list.
    m_branches.append(branch);
    addBranchNodes(branch);
    branch->setPathway(this);
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
            map.insertMulti(p,b);
        }
    }
    return map;
}

QSizeF DSBPathway::layout()
{
    // First layout all branches, so that their sizes are available.
    for (int i = 0; i < m_branches.size(); i++)
    {
        DSBBranch *b = m_branches.at(i);
        b->layout();
    }
    // Set relpt of main branch to (0,0).
    DSBBranch *mainBranch = m_branchMembership.value(m_headNode);
    mainBranch->setRelPt(QPointF(0,0));
    // Now layout all forks.
    // They will set the relpts of all remaining branches.
    QList<DSBFork*> allForks = m_forkMembership.values();
    for (int i = 0; i < allForks.size(); i++)
    {
        DSBFork *fork = allForks.at(i);
        fork->layout();
    }
    //...
    m_size = QSizeF(10,10); // TODO
    return m_size;
}

QSizeF DSBPathway::getSize()
{
    return m_size;
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
    for (int i = 0; i < m_branches.size(); i++)
    {
        DSBBranch *b = m_branches.at(i);
        b->drawRelTo(r);
    }
}

}




















