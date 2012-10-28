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

#include "libdunnartcanvas/canvas.h"
#include "libdsbpe/dsbspecies.h"

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

QString DSBSpecies::getCompartmentName()
{
    return m_compartmentName;
}

void DSBSpecies::addReactionEntered(DSBReaction& reac)
{
    m_reactionsEntered.append(reac);
}

void DSBSpecies::addReactionExited(DSBReaction& reac)
{
    m_reactionsExited.append(reac);
}

void DSBSpecies::addReactionModified(DSBReaction& reac)
{
    m_reactionsModified.append(reac);
}

void DSBSpecies::addClone(PDEPN *epn)
{
    m_clones.append(epn);
}

bool DSBSpecies::createClone(int x, int y)
{
    // Make sure we have a canvas.
    if (!m_canvas) { return false; }
    // Make sure there is already at least one clone.
    // (There always should be.)
    if (m_clones.size() < 1) { return false; }

    // TODO:
    // Create a copy of the first clone in m_clones.
    // Set the position of the new clone to the passed x,y.
    // To make copy, imitate what Edit->Copy, Paste does.
    // Or maybe just /use/ the canvas's copySelection and pasteSelection methods?
    // Since we don't know which subclass of PDEPN we might have, it's hard to
    // implement this manually.
    // If we do use Copy-Paste, then afterward we still have to manually
    // call the clone's setSpecies method, passing *this.

    m_canvas->deselectAll();
    PDEPN *epn = m_clones.at(0);
    //epn->setSelected(true);

    return true;

}

}
