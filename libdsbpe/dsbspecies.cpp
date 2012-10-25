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

#include "libdsbpe/dsbspecies.h"

#include "sbml/SBMLTypes.h"

namespace dunnart {

DSBSpecies::DSBSpecies() {}

DSBSpecies::DSBSpecies(Species *spec)
{
    m_name = QString(spec->getName().c_str());
    m_id = QString(spec->getId().c_str());
    m_compartment = QString(spec->getCompartment().c_str());
}

QString DSBSpecies::getCompartmentName()
{
    return m_compartment;
}

}
