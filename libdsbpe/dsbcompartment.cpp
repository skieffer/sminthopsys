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
#include "libdsbpe/dsbnode.h"

namespace dunnart {

DSBCompartment::DSBCompartment(QString compartmentName)
    : m_compartmentName(compartmentName)
{
    m_default_blacklist <<
                           "ATP" <<
                           "ADP" <<
                           "NADH" <<
                           "NAD+" <<
                           "NADPH" <<
                           "NADP+" <<
                           "AMP" <<
                           "L-glutamate" <<
                           "2-oxoglutarate" <<
                           "CoA" <<
                           "acetyl-CoA" <<
                           "CO2" <<
                           "P" <<
                           "PP";
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

void DSBCompartment::setCell(DSBCell *cell)
{
    m_cell = cell;
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

/* Just a method with which to switch to discrete cloning,
  while still in square layout. For debugging purposes.
  Delete when done.
  */
QSizeF DSBCompartment::squareLayout2()
{
    // Set all clonings to discrete.
    for (int i = 0; i < m_species.size(); i++)
    {
        m_species.at(i)->setDiscreteCloning();
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

QSizeF DSBCompartment::longestBranchLayout(DSBClone *endpt, bool forward)
{
    return longestBranchLayout(endpt, forward, m_default_blacklist);
}

QSizeF DSBCompartment::longestBranchLayout(
        DSBClone *endpt, bool forward, QList<QString> blacklist)
{
    // Set discrete clonings for all blacklisted species.
    // The exception is that if the selected endpoint clone is of a
    // blacklisted species, then we do not change its cloning.
    QString endptSpecName = endpt->getSpecies()->getName();
    for (int i = 0; i < m_species.size(); i++)
    {
        DSBSpecies *spec = m_species.at(i);
        QString name = spec->getName();
        if (blacklist.contains(name) && name != endptSpecName)
        {
            spec->setDiscreteCloning();
        }
    }

    // Find branches.
    QList<DSBBranch*> branches = endpt->findBranches(blacklist, forward);
    for (int i = 0; i < branches.size(); i++) {
        qDebug() << branches.at(i)->toString();
    }

    // For now, the rest is just a square layout.
    // TODO -- write the correct method

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
    drawAt(r);
}

void DSBCompartment::redraw()
{
    drawAt(m_basept);
}

void DSBCompartment::drawAt(QPointF r)
{
    m_basept = r;
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
        cl->drawRelTo(m_basept);
    }
}

}
