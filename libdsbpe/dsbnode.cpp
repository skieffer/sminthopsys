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

#include "dsbnode.h"

namespace dunnart {

bool DSBNode::s_followTransporters = false;

QList<DSBBranch> DSBNode::mergeSelfWithBranches(QList<DSBBranch> branches)
{
    // Were there no branches?
    if (branches.isEmpty())
    {
        // Then return one branch, containing just self.
        DSBBranch b;
        b.nodes.append(this);
        branches.append(b);
    }
    // Or was there exactly one branch?
    else if (branches.size() == 1)
    {
        // Then must merge self with the one branch.
        branches.first().nodes.prepend(this);
    }
    // Otherwise there were two or more branches.
    // Will merge with longest linear branch, or, failing that, longest cycle.
    // Separate branches into linear and cycles.
    QList<DSBBranch> linearBranches;
    QList<DSBBranch> cycles;
    for (int i = 0; i < branches.size(); i++)
    {
        DSBBranch b = branches.at(i);
        if (b.cycle) { cycles.append(b); }
        else { linearBranches.append(b); }
    }
    // Find a longest linear branch and longest cycle.
    int lin = 0; DSBBranch *linB;
    int cyc = 0; DSBBranch *cycB;
    for (int i = 0; i < linearBranches.size(); i++)
    {
        DSBBranch b = linearBranches.at(i);
        if (b.nodes.size() > lin)
        {
            lin = b.nodes.size();
            linB = &b;
        }
    }
    for (int i = 0; i < cycles.size(); i++)
    {
        DSBBranch b = cycles.at(i);
        if (b.nodes.size() > cyc)
        {
            cyc = b.nodes.size();
            cycB = &b;
        }
    }
    DSBBranch *mergeTarget;
    if (linB) { mergeTarget = linB; }
    else { mergeTarget = cycB; }
    // Finally, merge.
    mergeTarget->nodes.prepend(this);

    return branches;
}

}
