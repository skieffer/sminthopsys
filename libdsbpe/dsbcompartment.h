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
class DSBNode;
class DSBBranch;
class DSBPathway;
class Canvas;

class DSBCompartment : public DSBRecLayout
{

public:
    // Constructors
    //DSBCompartment();
    DSBCompartment(QString compartmentName);
    // Building methods
    void addSpecies(DSBSpecies *spec);
    void addReaction(DSBReaction *reac);
    void addCompartment(DSBCompartment *comp);
    void addCompartments(QList<DSBCompartment*> comps);
    QSizeF findBranches(DSBClone *endpt, bool forward);
    QSizeF findBranches(DSBClone *endpt, bool forward, QList<QString> blacklist);
    void setTrivialCloning();
    // Various layout methods
    QSizeF rowLayout();
    QSizeF layoutSquareCloneArray(QList<DSBClone*> clones, int ulx, int uly);
    // RecLayout methods
    QSizeF layout();
    void setRelPt(QPointF p);
    void drawRelTo(QPointF q);
    void drawAt(QPointF r);
    void redraw();
    QSizeF getSize();
    // Misc get and set
    QString getName();
    void setParent(DSBCompartment *comp);
    void setCanvas(Canvas *canvas);

    QList<QString> m_default_blacklist;

private:
    QString m_compartmentName;
    QPointF m_relpt;
    QPointF m_basept;
    QSizeF m_size;
    Canvas *m_canvas;
    DSBCompartment *m_parentCompartment;
    QList<DSBSpecies *> m_species;
    QList<DSBReaction *> m_reactions;
    QList<DSBCompartment*> m_compartments;
    QList<DSBPathway*> m_pathways;
    bool m_show_reactions;

    QList<DSBClone*> getAllClones();
    QList<DSBClone*> getLooseClones();

};

}

#endif // DSBCOMPARTMENT_H
