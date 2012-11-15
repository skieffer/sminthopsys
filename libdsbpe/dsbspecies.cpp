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

#include "dsbspecies.h"
#include "dsbreaction.h"
#include "dsbclone.h"

#include "libdunnartcanvas/canvas.h"

#include "sbml/SBMLTypes.h"

namespace dunnart {

DSBSpecies::DSBSpecies() : m_nextCloneId(0) {}

DSBSpecies::DSBSpecies(Species *spec) :
    m_nextCloneId(0),
    m_sbmlSpecies(spec)
{
    m_name = QString(spec->getName().c_str());
    m_id = QString(spec->getId().c_str());
    m_compartmentName = QString(spec->getCompartment().c_str());
}

void DSBSpecies::setCanvas(Canvas *canvas)
{
    m_canvas = canvas;
}

Canvas *DSBSpecies::canvas()
{
    return m_canvas;
}

void DSBSpecies::setCompartment(DSBCompartment *comp)
{
    m_compartment = comp;
}

DSBCompartment *DSBSpecies::getCompartment()
{
    return m_compartment;
}

QString DSBSpecies::getCompartmentName()
{
    return m_compartmentName;
}

QString DSBSpecies::getName()
{
    return m_name;
}

QString DSBSpecies::getId()
{
    return m_id;
}

void DSBSpecies::addReactionEntered(DSBReaction *reac)
{
    m_reactionsEntered.append(reac);
}

void DSBSpecies::addReactionExited(DSBReaction *reac)
{
    m_reactionsExited.append(reac);
}

void DSBSpecies::addReactionModified(DSBReaction *reac)
{
    m_reactionsModified.append(reac);
}

QList<DSBClone*> DSBSpecies::getClones()
{
    return m_clones;
}

/* This map must be maintained by all methods that change the
   cloning.

   It associates with each reaction (ID) a list of clones of the present
   species which have been assigned to participate in that reaction, as
   reactant, product, or modifier.

   (SBGN PD diagrams do sometimes feature multiple clones of one
    and the same species playing one and the same role in a reaction,
    be it as reactant or product (or maybe even modifier?).)
  */
DSBCloneAssignment *DSBSpecies::getCloneAssignmentByReactionId(QString rid)
{
    return m_cloneAssignmentsByReactionId.value(rid);
}

void DSBSpecies::deleteClonesAndAssignments()
{
    // Delete shapes from the canvas.
    for (int i = 0; i < m_clones.size(); i++)
    {
        m_clones.at(i)->deleteShape();
    }
    // Clear the clone assignment map, and delete the DSBCloneAssignment
    // structs themselves.
    QList<QString> reacIds = m_cloneAssignmentsByReactionId.keys();
    for (int i = 0; i < reacIds.size(); i++)
    {
        QString id = reacIds.at(i);
        DSBCloneAssignment *ca = m_cloneAssignmentsByReactionId.value(id);
        m_cloneAssignmentsByReactionId.remove(id);
        delete ca;
    }
    // Clear the list of clones, and delete the DSBClone objects themselves.
    while (!m_clones.isEmpty())
    {
        DSBClone *cl = m_clones.takeFirst(); // removes it from the list
        delete cl;
    }
    // Reset nextCloneId to 0.
    m_nextCloneId = 0;
}

/*  Allocate a new DSBClone object, cloning this species,
    give it the next available clone id, add it to the
    list of clones, and return a pointer to it.
  */
DSBClone *DSBSpecies::makeNewClone()
{
    int idNum = m_nextCloneId++;
    DSBClone *cl = new DSBClone(this);
    cl->setCloneNum(idNum);
    m_clones.append(cl);
    return cl;
}

/* The trivial cloning is that in which there is precisely one clone
   of this species, participating in all reactions in which this
   species participates.
  */
void DSBSpecies::setTrivialCloning()
{
    // Delete old clones.
    deleteClonesAndAssignments();

    // Create new clone.
    //DSBClone *cl = new DSBClone(this);
    //m_clones.append(cl);
    DSBClone *cl = makeNewClone();

    // Set it as the sole clone assigned to each reaction.
    for (int i = 0; i < m_reactionsEntered.size(); i++)
    {
        DSBReaction *reac = m_reactionsEntered.at(i);
        cl->addReactionEntered(reac);
        QString rid = reac->getReactionId();
        DSBCloneAssignment *ca =
                m_cloneAssignmentsByReactionId.value( rid, new DSBCloneAssignment() );
        ca->reactants.append(cl);
        m_cloneAssignmentsByReactionId.insert(rid,ca);
    }

    for (int i = 0; i < m_reactionsExited.size(); i++)
    {
        DSBReaction *reac = m_reactionsExited.at(i);
        cl->addReactionExited(reac);
        QString rid = reac->getReactionId();
        DSBCloneAssignment *ca =
                m_cloneAssignmentsByReactionId.value( rid, new DSBCloneAssignment() );
        ca->products.append(cl);
        m_cloneAssignmentsByReactionId.insert(rid,ca);
    }

    for (int i = 0; i < m_reactionsModified.size(); i++)
    {
        DSBReaction *reac = m_reactionsModified.at(i);
        cl->addReactionModified(reac);
        QString rid = reac->getReactionId();
        DSBCloneAssignment *ca =
                m_cloneAssignmentsByReactionId.value( rid, new DSBCloneAssignment() );
        ca->modifiers.append(cl);
        m_cloneAssignmentsByReactionId.insert(rid,ca);
    }

    setCloneMarkers();
}

/* The discrete cloning (named after "the discrete topology") is that
   in which there is one clone for each role played by the species.
  */
void DSBSpecies::setDiscreteCloning()
{
    // Delete old clones.
    deleteClonesAndAssignments();

    // Create a new clone for each role played by this species.
    for (int i = 0; i < m_reactionsEntered.size(); i++)
    {
        //DSBClone *cl = new DSBClone(this);
        //m_clones.append(cl);
        DSBClone *cl = makeNewClone();
        DSBReaction *reac = m_reactionsEntered.at(i);
        cl->addReactionEntered(reac);
        QString rid = reac->getReactionId();
        DSBCloneAssignment *ca =
                m_cloneAssignmentsByReactionId.value( rid, new DSBCloneAssignment() );
        ca->reactants.append(cl);
        m_cloneAssignmentsByReactionId.insert(rid,ca);
    }

    for (int i = 0; i < m_reactionsExited.size(); i++)
    {
        //DSBClone *cl = new DSBClone(this);
        //m_clones.append(cl);
        DSBClone *cl = makeNewClone();
        DSBReaction *reac = m_reactionsExited.at(i);
        cl->addReactionExited(reac);
        QString rid = reac->getReactionId();
        DSBCloneAssignment *ca =
                m_cloneAssignmentsByReactionId.value( rid, new DSBCloneAssignment() );
        ca->products.append(cl);
        m_cloneAssignmentsByReactionId.insert(rid,ca);
    }

    for (int i = 0; i < m_reactionsModified.size(); i++)
    {
        //DSBClone *cl = new DSBClone(this);
        //m_clones.append(cl);
        DSBClone *cl = makeNewClone();
        DSBReaction *reac = m_reactionsModified.at(i);
        cl->addReactionModified(reac);
        QString rid = reac->getReactionId();
        DSBCloneAssignment *ca =
                m_cloneAssignmentsByReactionId.value( rid, new DSBCloneAssignment() );
        ca->modifiers.append(cl);
        m_cloneAssignmentsByReactionId.insert(rid,ca);
    }

    setCloneMarkers();
}

void DSBSpecies::setCloneMarkers()
{
    if (m_clones.size() > 1)
    {
        for (int i = 0; i < m_clones.size(); i++)
        {
            m_clones.at(i)->set_is_cloned(true);
        }
    }
    else if (m_clones.size() == 1)
    {
        m_clones.at(0)->set_is_cloned(false);
    }
}

}