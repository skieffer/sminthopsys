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
#include "dsbreclayout.h"

class Reaction;

namespace dunnart {

class DSBSpecies;
class DSBCompartment;
class DSBClone;
class DSBBranch;

class DSBReaction : public DSBNode, public DSBRecLayout
{

public:
    // Constructors
    DSBReaction();
    DSBReaction(Reaction *reac);
    // Get and set
    QString getCompartmentName();
    void setCompartment(DSBCompartment *comp);
    DSBCompartment *getCompartment();
    QString getReactionId();
    QString getName();
    // Check properties
    bool isIntercompartmental();
    bool isReversible();
    // RecLayout methods
    QSizeF layout();
    void setRelPt(QPointF p);
    void drawRelTo(QPointF q);
    void drawAt(QPointF r);
    void redraw();
    QSizeF getSize();
    // other
    void doublyLink(QMap<QString,DSBSpecies*> map);
    QList<DSBBranch*> findBranchesRec(
            QList<QString>& seen, QList<QString> blacklist, bool forward, DSBNode *last);
    void addInputBranchHead(DSBClone *head);
    void addOutputBranchHead(DSBClone *head);
    void addSatellite(DSBClone *sat);

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

    QList<DSBClone*> m_inputBranchHeads;
    QList<DSBClone*> m_outputBranchHeads;
    QList<DSBClone*> m_satellites;

    QPointF m_relpt;
    QPointF m_basept;
    QSizeF m_size;

    QList<DSBClone *> getOpposedClones(DSBClone* clone);
    QList<DSBClone *> getInputClones();
    QList<DSBClone *> getOutputClones();


};

}

#endif // DSBREACTION_H
