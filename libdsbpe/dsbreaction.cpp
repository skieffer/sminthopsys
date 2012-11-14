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

#include "libdsbpe/dsbreaction.h"
#include "libdsbpe/dsbspecies.h"

#include "sbml/SBMLTypes.h"

namespace dunnart {

DSBReaction::DSBReaction() {}

DSBReaction::DSBReaction(Reaction *reac) :
    m_sbmlReaction(reac)
{
    m_name = QString(reac->getName().c_str());
    m_id = QString(reac->getId().c_str());
    m_compartmentName = QString(reac->getCompartment().c_str());
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

QString DSBReaction::getReactionId()
{
    return m_id;
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

}
