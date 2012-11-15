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

#ifndef DSBNODE_H
#define DSBNODE_H

#include <QList>
#include <QString>

namespace dunnart {

struct DSBBranch;

class DSBNode
{
public:
    virtual QList<DSBBranch*> findBranchesRec(QList<QString> &seen, DSBNode *last = 0) = 0;

    QList<DSBBranch*> findBranches()
    {
        QList<QString> seen;
        return findBranchesRec(seen);
    }

    QList<DSBBranch*> mergeSelfWithBranches(QList<DSBBranch*> branches);

    static bool s_followTransporters;

private:
    DSBBranch *findMergeTarget(QList<DSBBranch*> branches);
};

struct DSBBranch
{
    bool cycle;
    QList<DSBNode*> nodes;
    DSBNode *parent;
    QString toString();
    DSBBranch() : cycle(false), parent(0) {}
};

}

#endif // DSBNODE_H
