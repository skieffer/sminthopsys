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

#include "dsbnode.h"
#include "dsbclone.h"
#include "dsbspecies.h"
#include "dsbreaction.h"

namespace dunnart {

bool DSBNode::s_followTransporters = false;

QList<DSBBranch*> DSBNode::mergeSelfWithBranches(QList<DSBBranch*> branches)
{
    // Were there no branches?
    if (branches.isEmpty())
    {
        // Then return one branch, containing just self.
        DSBBranch *b = new DSBBranch;
        b->nodes.append(this);
        branches.append(b);
    }
    // Or was there exactly one branch?
    else if (branches.size() == 1)
    {
        // Then must merge self with the one branch.
        branches.first()->nodes.prepend(this);
    }
    else
    {
        // Otherwise there were two or more branches.
        // Will merge with longest linear branch, or, failing that, longest cycle.
        // Separate branches into linear and cycles.
        QList<DSBBranch*> linearBranches;
        QList<DSBBranch*> cycles;
        for (int i = 0; i < branches.size(); i++)
        {
            DSBBranch *b = branches.at(i);
            if (b->cycle) { cycles.append(b); }
            else { linearBranches.append(b); }
        }
        // Find a longest linear branch and longest cycle.
        int lin = 0; DSBBranch *linB = 0;
        int cyc = 0; DSBBranch *cycB = 0;
        qDebug() << "num lin branches: " << linearBranches.size();
        qDebug() << "num cycles: " << cycles.size();
        for (int i = 0; i < linearBranches.size(); i++)
        {
            DSBBranch *b = linearBranches.at(i);
            if (b->nodes.size() > lin)
            {
                lin = b->nodes.size();
                linB = b;
            }
        }
        for (int i = 0; i < cycles.size(); i++)
        {
            DSBBranch *b = cycles.at(i);
            if (b->nodes.size() > cyc)
            {
                cyc = b->nodes.size();
                cycB = b;
            }
        }
        DSBBranch *mergeTarget;
        if (linB) {
            qDebug() << "merging with linear branch at " << linB;
            mergeTarget = linB;
        }
        else {
            qDebug() << "merging with cycle at " << cycB;
            mergeTarget = cycB;
        }
        // Finally, merge.
        mergeTarget->nodes.prepend(this);
        // And set self as parent of that branch.
        mergeTarget->parent = this;
        // And set self as parent of all branches that do not already have one.
        for (int i = 0; i < branches.size(); i++)
        {
            DSBBranch *b = branches.at(i);
            if (!b->parent) { b->parent = this; }
        }
    }

    return branches;
}

QString DSBBranch::toString()
{
    QString s;
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
            s += " --[ ]--> ";
        }
    }
    return s;
}

}
















