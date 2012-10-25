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

#include <QString>
#include <QList>

#include "libdsbpe/dsbabstractcontainer.h"
#include "libdsbpe/dsbspecies.h"
#include "libdsbpe/dsbreaction.h"

namespace dunnart {

DSBAbstractContainer::DSBAbstractContainer(QString containerName)
    : m_containerName(containerName)
{
    m_species = new QList<DSBSpecies>();
    m_reactions = new QList<DSBReaction>();
}

void DSBAbstractContainer::addSpecies(DSBSpecies *spec)
{
    m_species->append(*spec);
}

void DSBAbstractContainer::addReaction(DSBReaction *reac)
{
    m_reactions->append(*reac);
}

}
