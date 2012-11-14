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

#include <QtGui>
#include <QString>
#include <QList>

#include <math.h>

#include "libdsbpe/dsbspecies.h"
#include "libdsbpe/dsbreaction.h"
#include "libdsbpe/dsbclone.h"

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

QString DSBCompartment::getName()
{
    return m_compartmentName;
}

QList<DSBClone*> DSBCompartment::getAllClones()
{
    QList<DSBClone*> clones;
    for (int i = 0; i < m_species.size(); i++)
    {
        DSBSpecies *spec = m_species.at(i);
        clones.append(spec->getClones());
    }
    return clones;
}

QSizeF DSBCompartment::layout()
{
    // TODO: Implement more layout methods.
    return squareLayout();
}

QSizeF DSBCompartment::squareLayout()
{
    // Set all clonings to trivial.
    for (int i = 0; i < m_species.size(); i++)
    {
        m_species.at(i)->setTrivialCloning();
    }
    // Get the clones.
    QList<DSBClone*> clones = getAllClones();

    // TODO: Take account of the sizes of the EPN nodes.
    // For now we simply assume they are the default size
    // of 70x50.
    int numClones = clones.size();
    // If there are no clones, then it is an empty compartment.
    // We return a default "small" size.
    if (numClones == 0)
    {
        m_size = QSizeF(100,100);
        return m_size;
    }
    // Call the clones' layout methods, even though for now
    // we are not using the sizes that they return. This serves
    // to get them to initialize their own sizes, which are needed
    // when we ask them to draw themselves.
    for (int i = 0; i < numClones; i++)
    {
        clones.at(i)->layout();
    }

    int cols = ceil(sqrt(numClones)); // number of columns in array
    int rows = ceil(numClones/cols);
    int u = 50; // unit of separation
    int sepUnits = 2; // separation between adjacent nodes, in units u
    int x0 = 0, y0 = 0, x, y, col, row;
    for (int i = 0; i < numClones; i++)
    {
        col = i%cols;
        row = i/cols;
        x = x0 + col*sepUnits*u;
        y = y0 + row*sepUnits*u;
        clones.at(i)->setRelPt(QPointF(x,y));
    }
    int width = cols*sepUnits*u + 70;
    int height = rows*sepUnits*u + 50;
    // Set reactions to be undisplayed.
    m_show_reactions = false;
    m_size = QSizeF(width,height);
    return m_size;
}

void DSBCompartment::setRelPt(QPointF p)
{
    m_relpt = p;
}

void DSBCompartment::drawRelTo(QPointF q)
{
    QPointF r = m_relpt + q;
    // Compartment boundary
    //   (TODO)

    // Reactions:
    if (m_show_reactions)
    {
        // TODO
    }

    // Clones:
    QList<DSBClone*> clones = getAllClones();
    for (int i = 0; i < clones.size(); i++)
    {
        DSBClone *cl = clones.at(i);
        cl->drawRelTo(r);
    }
}

}
