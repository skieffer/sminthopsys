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

#ifndef DSBSPECIES_H
#define DSBSPECIES_H

#include <QString>
#include <QList>

#include "libdsbpe/dsbreaction.h"
#include "plugins/shapes/sbgn/pdepn.h"

class Species;

namespace dunnart {

class PDEPN;
class Canvas;
class DSBCompartment;

class DSBSpecies
{

public:
    DSBSpecies();
    DSBSpecies(Species *spec);
    QString getCompartmentName();
    void addReactionEntered(DSBReaction *reac);
    void addReactionExited(DSBReaction *reac);
    void addReactionModified(DSBReaction *reac);
    void setCanvas(Canvas *canvas);
    void setCompartment(DSBCompartment *comp);
    DSBCompartment *getCompartment();
    void addClone(PDEPN *epn);
    bool createClone(int x, int y);

private:
    Canvas *m_canvas;
    Species *m_sbmlSpecies;
    QString m_name;
    QString m_id;
    QString m_compartmentName;
    DSBCompartment *m_compartment;
    QList<DSBReaction *> m_reactionsEntered;
    QList<DSBReaction *> m_reactionsExited;
    QList<DSBReaction *> m_reactionsModified;
    QList<PDEPN *> m_clones;
};

}

#endif // DSBSPECIES_H
