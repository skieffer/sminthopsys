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
#include "libdsbpe/dsbcompartment.h"

#include <QString>
#include <QList>

#include <math.h>

#include "libdsbpe/dsbspecies.h"
#include "libdsbpe/dsbreaction.h"

namespace dunnart {

DSBCompartment::DSBCompartment(QString compartmentName)
    : m_compartmentName(compartmentName)
{
}

void DSBCompartment::addSpecies(DSBSpecies *spec)
{
    m_species.append(spec);
}

void DSBCompartment::addReaction(DSBReaction *reac)
{
    m_reactions.append(reac);
}

}

QSizeF DSBCompartment::layout()
{
    return squareLayout();
}

QSizeF DSBCompartment::squareLayout()
{
    // TODO: Take account of the sizes of the species nodes.
    // For now we simply assume they are the default size
    // of 70x50.
    int numSpecies = m_species.size();
    int cols = ceil(sqrt(numSpecies)); // number of columns in array
    int rows = ceil(numSpecies/cols);

}

void DSBCompartment::drawAt(QPointF p)
{
    // TODO
}
