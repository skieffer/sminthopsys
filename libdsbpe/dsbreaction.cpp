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
#include <QSet>

#include "libdsbpe/dsbreaction.h"
#include "libdsbpe/dsbspecies.h"
#include "libdsbpe/dsbclone.h"

#include "sbml/SBMLTypes.h"

namespace dunnart {

DSBReaction::DSBReaction() {}

DSBReaction::DSBReaction(Reaction *reac) :
    m_sbmlReaction(reac)
{
    m_name = QString(reac->getName().c_str());
    m_id = QString(reac->getId().c_str());
    m_compartmentName = QString(reac->getCompartment().c_str());
    m_reversible = false; // Default to not reversible.
    if (reac->isSetReversible()) // If reversibility has been stated,
    {
        m_reversible = reac->getReversible(); // then accept the stated value.
    }
}

void DSBReaction::setCompartment(DSBCompartment *comp)
{
    m_compartment = comp;
}

DSBCompartment *DSBReaction::getCompartment()
{
    return m_compartment;
}

QString DSBReaction::getCompartmentName()
{
    return m_compartmentName;
}

QString DSBReaction::getName()
{
    return m_name;
}

QString DSBReaction::getReactionId()
{
    return m_id;
}

bool DSBReaction::isReversible()
{
    return m_reversible;
}

/* Check whether this reaction spans multiple compartments.
  */
bool DSBReaction::isIntercompartmental()
{
    QSet<QString> comps;
    for (int i = 0; i < m_inputs.size(); i++)
    {
        DSBSpecies *spec = m_inputs.at(i);
        QString comp =spec->getCompartmentName();
        comps.insert(comp);
    }
    for (int i = 0; i < m_outputs.size(); i++)
    {
        DSBSpecies *spec = m_outputs.at(i);
        QString comp =spec->getCompartmentName();
        comps.insert(comp);
    }
    for (int i = 0; i < m_modifiers.size(); i++)
    {
        DSBSpecies *spec = m_modifiers.at(i);
        QString comp =spec->getCompartmentName();
        comps.insert(comp);
    }
    return (comps.size() > 1);
}

/* Give this reaction links to all species involved in it,
   and give those species links to this reaction.
  */
void DSBReaction::doublyLink(QMap<QString, DSBSpecies*> map)
{
    ListOfSpeciesReferences *lsr;
    SimpleSpeciesReference *ssr;
    unsigned int N;
    QString specId;

    // "reactants", or inputs
    lsr = m_sbmlReaction->getListOfReactants();
    N = lsr->size();
    for (unsigned int i = 0; i < N; i++)
    {
        ssr = lsr->get(i);
        specId = QString(ssr->getSpecies().c_str());
        if (!map.contains(specId))
        {
            // TODO: Report error. Reaction is referring to a species that
            // was not declared in the SBML list of species.
            qDebug() << "map does not contain species id " << specId;
        }
        else
        {
            DSBSpecies *dsbspec = map.value(specId);
            m_inputs.append(dsbspec);
            dsbspec->addReactionEntered(this);
        }
    }

    // "products", or outputs
    lsr = m_sbmlReaction->getListOfProducts();
    N = lsr->size();
    for (unsigned int i = 0; i < N; i++)
    {
        ssr = lsr->get(i);
        specId = QString(ssr->getSpecies().c_str());
        if (!map.contains(specId))
        {
            // TODO: Report error. Reaction is referring to a species that
            // was not declared in the SBML list of species.
            qDebug() << "map does not contain species id " << specId;
        }
        else
        {
            DSBSpecies *dsbspec = map.value(specId);
            m_outputs.append(dsbspec);
            dsbspec->addReactionExited(this);
        }
    }

    // modifiers (e.g. catalysts)
    lsr = m_sbmlReaction->getListOfModifiers();
    N = lsr->size();
    for (unsigned int i = 0; i < N; i++)
    {
        ssr = lsr->get(i);
        specId = QString(ssr->getSpecies().c_str());
        if (!map.contains(specId))
        {
            // TODO: Report error. Reaction is referring to a species that
            // was not declared in the SBML list of species.
            qDebug() << "map does not contain species id " << specId;
        }
        else
        {
            DSBSpecies *dsbspec = map.value(specId);
            m_modifiers.append(dsbspec);
            dsbspec->addReactionModified(this);
        }
    }
}

QList<DSBClone*> DSBReaction::getInputClones()
{
    QList<DSBClone*> clones;
    for (int i = 0; i < m_inputs.size(); i++)
    {
        DSBSpecies *spec = m_inputs.at(i);
        DSBCloneAssignment *cla = spec->getCloneAssignmentByReactionId(m_id);
        clones.append(cla->reactants);
    }
    return clones;
}

QList<DSBClone*> DSBReaction::getOutputClones()
{
    QList<DSBClone*> clones;
    for (int i = 0; i < m_outputs.size(); i++)
    {
        DSBSpecies *spec = m_outputs.at(i);
        DSBCloneAssignment *cla = spec->getCloneAssignmentByReactionId(m_id);
        clones.append(cla->products);
    }
    return clones;
}

QList<DSBClone*> DSBReaction::getOpposedClones(DSBClone *clone)
{
    QList<DSBClone*> opp; // Prepare return value.

    // First determine which side, or sides, the clone is on.
    DSBSpecies *spec = clone->getSpecies();
    DSBCloneAssignment *cla = spec->getCloneAssignmentByReactionId(m_id);
    bool isProd = cla->products.contains(clone);
    bool isReac = cla->reactants.contains(clone);

    // If it is a product and not a reactant, then the reactants are opposed.
    if (isProd && !isReac)
    {
        opp = getInputClones();
    }
    // If it is a reactant and not a product, then the products are opposed.
    else if (isReac && !isProd)
    {
        opp = getOutputClones();
    }
    // Otherwise we cannot call anything opposed.

    return opp;
}

QList<DSBBranch*> DSBReaction::findBranchesRec(
        QList<QString> &seen, QList<QString> blacklist, bool forward, DSBNode *last)
{
    seen.append(m_id); // Mark self as seen.

    QList<DSBBranch*> branches; // Prepare return value.

    // Check which side of this reaction the last node 'last' lies on.
    // Then only consider flowing out on opposite side.
    DSBClone *clone = dynamic_cast<DSBClone*>(last);
    QList<DSBClone*> opp = getOpposedClones(clone);

    for (int i = 0; i < opp.size(); i++)
    {
        DSBClone *cl = opp.at(i);

        // Do not turn around and go backwards.
        if (cl == last) {continue;}

        // Consider whether this clone has already been seen or not.
        QString cid = cl->getCloneId();

        if (seen.contains(cid))
        {
            // Clone has already been seen, so we have found a cycle.
            DSBBranch *b = new DSBBranch;
            b->nodes.append(cl);
            b->cycle = true;
            branches.append(b);
        }
        else
        {
            // No cycle. Recurse.
            QList<DSBBranch*> bb = cl->findBranchesRec(seen, blacklist, forward, this);
            branches.append(bb);
        }
    }

    return mergeSelfWithBranches(branches, blacklist);
}

}

























