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

#ifndef DSBCOMPARTMENT_H
#define DSBCOMPARTMENT_H

#include <QString>
#include <QList>
#include <QMap>

#include "dsbreclayout.h"

namespace dunnart {

class DSBSpecies;
class DSBReaction;
class DSBClone;
class DSBCell;
class DSBNode;
class DSBBranch;

class DSBCompartment : public DSBRecLayout
{

public:
    DSBCompartment();
    DSBCompartment(QString compartmentName);
    void addSpecies(DSBSpecies *spec);
    void addReaction(DSBReaction *reac);
    QSizeF squareLayout();
    QSizeF squareLayout2();
    QSizeF longestBranchLayout(DSBClone *endpt, bool forward);
    QSizeF longestBranchLayout(DSBClone *endpt, bool forward, QList<QString> blacklist);
    QSizeF layout();
    void setRelPt(QPointF p);
    void drawRelTo(QPointF q);
    void drawAt(QPointF r);
    void redraw();
    QString getName();
    void setCell(DSBCell *cell);
    QMap<DSBNode*, DSBBranch*> countBranchPoints(QList<DSBBranch*> branches);

    QList<QString> m_default_blacklist;

private:
    QString m_compartmentName;
    QPointF m_relpt;
    QPointF m_basept;
    QSizeF m_size;
    DSBCell *m_cell;
    QList<DSBSpecies *> m_species;
    QList<DSBReaction *> m_reactions;
    bool m_show_reactions;

    QList<DSBClone*> getAllClones();

};

}

#endif // DSBCOMPARTMENT_H
