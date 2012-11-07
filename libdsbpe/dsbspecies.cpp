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

#include "dsbspecies.h"
#include "dsbclone.h"

#include "libdunnartcanvas/canvas.h"

#include "sbml/SBMLTypes.h"

namespace dunnart {

DSBSpecies::DSBSpecies() {}

DSBSpecies::DSBSpecies(Species *spec) :
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

void DSBSpecies::deleteClones()
{
    for (int i = 0; i < m_clones.size(); i++)
    {
        m_clones.at(i)->deleteShape();
    }
    while (!m_clones.isEmpty())
    {
        DSBClone *cl = m_clones.at(0);
        m_clones.removeFirst();
        delete cl;
    }
}

void DSBSpecies::setTrivialCloning()
{
    deleteClones();
    DSBClone *cl = new DSBClone(this);
    cl->setReactionsEntered(m_reactionsEntered);
    cl->setReactionsExited(m_reactionsExited);
    cl->setReactionsModified(m_reactionsModified);
    m_clones.append(cl);
    setCloneMarkers();
}

void DSBSpecies::setDiscreteCloning()
{
    deleteClones();

    for (int i = 0; i < m_reactionsEntered.size(); i++)
    {
        DSBClone *cl = new DSBClone(this);
        cl->addReactionEntered(m_reactionsEntered.at(i));
        m_clones.append(cl);
    }

    for (int i = 0; i < m_reactionsExited.size(); i++)
    {
        DSBClone *cl = new DSBClone(this);
        cl->addReactionExited(m_reactionsExited.at(i));
        m_clones.append(cl);
    }

    for (int i = 0; i < m_reactionsModified.size(); i++)
    {
        DSBClone *cl = new DSBClone(this);
        cl->addReactionModified(m_reactionsModified.at(i));
        m_clones.append(cl);
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
