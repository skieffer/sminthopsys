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

#ifndef DSBREACTION_H
#define DSBREACTION_H

#include <QString>
#include <QList>
#include <QMap>

#include "dsbnode.h"

class Reaction;

namespace dunnart {

class DSBSpecies;
class DSBCompartment;
class DSBClone;

class DSBReaction : public DSBNode
{

public:
    DSBReaction();
    DSBReaction(Reaction *reac);
    QString getCompartmentName();
    void doublyLink(QMap<QString,DSBSpecies*> map);
    void setCompartment(DSBCompartment *comp);
    DSBCompartment *getCompartment();
    QString getReactionId();
    QString getName();
    bool isIntercompartmental();
    bool isReversible();
    QList<DSBBranch*> findBranchesRec(
            QList<QString> &seen, QList<QString> blacklist, bool forward, DSBNode *last);

private:
    Reaction *m_sbmlReaction;
    QString m_name;
    QString m_id;
    QString m_compartmentName;
    bool m_reversible;
    DSBCompartment *m_compartment;
    QList<DSBSpecies *> m_inputs;
    QList<DSBSpecies *> m_outputs;
    QList<DSBSpecies *> m_modifiers;

    QList<DSBClone *> getOpposedClones(DSBClone* clone);
    QList<DSBClone *> getInputClones();
    QList<DSBClone *> getOutputClones();
};

}

#endif // DSBREACTION_H
