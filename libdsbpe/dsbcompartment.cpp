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
#include "dsbcompartment.h"

#include <QtGui>
#include <QString>
#include <QList>

#include <math.h>
#include <assert.h>

#include "dsbspecies.h"
#include "dsbreaction.h"
#include "dsbclone.h"
#include "dsbnode.h"
#include "dsbbranch.h"
#include "dsbpathway.h"

#include "libdunnartcanvas/canvas.h"

//debugging:
#include "libdunnartcanvas/graphlayout.h"
#include "libdunnartcanvas/graphdata.h"
//

namespace dunnart {

DSBCompartment::DSBCompartment(QString compartmentName)
    : m_compartmentName(compartmentName),
      m_parentCompartment(NULL)
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

void DSBCompartment::addCompartment(DSBCompartment *comp)
{
    m_compartments.append(comp);
    comp->setParent(this);
}

void DSBCompartment::addCompartments(QList<DSBCompartment *> comps)
{
    for (int i = 0; i < comps.size(); i++)
    {
        DSBCompartment *comp = comps.at(i);
        addCompartment(comp);
    }
}

QString DSBCompartment::getName()
{
    return m_compartmentName;
}

void DSBCompartment::setParent(DSBCompartment *comp)
{
    m_parentCompartment = comp;
}

void DSBCompartment::setCanvas(Canvas *canvas)
{
    m_canvas = canvas;
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
    return rowLayout();
}

QSizeF DSBCompartment::getSize()
{
    return m_size;
}

/* Return the list of all clones that do not belong to a pathway.
 */
QList<DSBClone*> DSBCompartment::getLooseClones()
{
    QList<DSBClone*> all = getAllClones();
    QList<DSBClone*> loose;
    for (int i = 0; i < all.size(); i++)
    {
        DSBClone *cl = all.at(i);
        DSBPathway *pw = cl->getPathway();
        if (!pw)
        {
            loose.append(cl);
        }
    }
    return loose;
}

QSizeF DSBCompartment::rowLayout()
{
    int pad = 100;
    int x = 0;
    int maxHeight = 0;

    // Compartments
    for (int i = 0; i < m_compartments.size(); i++)
    {
        DSBCompartment *comp = m_compartments.at(i);
        QSizeF size = comp->layout();
        comp->setRelPt(QPointF(x,0));
        x += size.width() + pad;
        int h = size.height();
        maxHeight = (h > maxHeight? h : maxHeight);
    }

    // Pathways
    for (int i = 0; i < m_pathways.size(); i++)
    {
        DSBPathway *pw = m_pathways.at(i);
        QSizeF size = pw->layout();
        pw->setRelPt(QPointF(x,0));
        x += size.width() + pad;
        int h = size.height();
        maxHeight = (h > maxHeight? h : maxHeight);
    }

    // Loose clones
    QList<DSBClone*> loose = getLooseClones();
    QSizeF size = layoutSquareCloneArray(loose, x, 0);
    x += size.width() + pad;
    int h = size.height();
    maxHeight = (h > maxHeight? h : maxHeight);

    int width = (pad <= x ? x-pad : x);
    m_size = QSizeF(width,maxHeight);
    return m_size;
}

void DSBCompartment::setTrivialCloning()
{
    // Set all clonings to trivial.
    for (int i = 0; i < m_species.size(); i++)
    {
        m_species.at(i)->setTrivialCloning();
    }
}

QSizeF DSBCompartment::layoutSquareCloneArray(
        QList<DSBClone *> clones, int ulx, int uly)
{
    // TODO: Take account of the sizes of the EPN nodes.
    // For now we simply assume they are the default size
    // of 70x50.
    int numClones = clones.size();
    // If there are no clones, then there is nothing to do.
    if (numClones == 0)
    {
        return QSizeF(0,0);
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
    int rows = ceil(numClones/cols); // number of rows
    int u = 50; // unit of separation
    int sepUnits = 2; // separation between adjacent nodes, in units u
    int x0 = ulx, y0 = uly, x, y, col, row;
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
    return QSizeF(width,height);
}

void DSBCompartment::findBranches(DSBClone *endpt, bool forward)
{
    findBranches(endpt, forward, m_default_blacklist);
}

void DSBCompartment::findBranches(
        DSBClone *endpt, bool forward, QList<QString> blacklist)
{
    // Set discrete clonings for all blacklisted species, with
    // the exception that if the selected endpoint clone is of a
    // blacklisted species, then we do not change its cloning.
    QString endptSpecName = endpt->getSpecies()->getName();
    m_canvas->stop_graph_layout();
    for (int i = 0; i < m_species.size(); i++)
    {
        DSBSpecies *spec = m_species.at(i);
        QString name = spec->getName();
        if (blacklist.contains(name) && name != endptSpecName)
        {
            spec->setDiscreteCloning();
        }
    }
    m_canvas->restart_graph_layout();

    // Find branches.
    bool extended = true; // Throw away branches of length 1.
    QList<DSBBranch*> branches = endpt->findBranches(blacklist, forward, extended);
    for (int i = 0; i < branches.size(); i++) {
        qDebug() << branches.at(i)->toString();
    }
    // Build pathway.
    if (branches.size()>0)
    {
        DSBPathway *pathway = new DSBPathway(endpt, branches);
        pathway->setCanvas(m_canvas);
        m_pathways.append(pathway);
    }
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

    // Loose reactions:
    if (m_show_reactions)
    {
        // TODO
    }

    // Sub-compartments:
    for (int i = 0; i < m_compartments.size(); i++)
    {
        DSBCompartment *comp = m_compartments.at(i);
        comp->drawRelTo(m_basept);
    }

    // Pathways:
    for (int i = 0; i < m_pathways.size(); i++)
    {
        DSBPathway *pw = m_pathways.at(i);
        pw->drawRelTo(m_basept);
    }

    // Loose clones:
    QList<DSBClone*> clones = getLooseClones();
    for (int i = 0; i < clones.size(); i++)
    {
        DSBClone *cl = clones.at(i);
        cl->drawRelTo(m_basept);
    }
}

void DSBCompartment::redisplay()
{
    if (m_parentCompartment)
    {
        m_parentCompartment->redisplay();
    }
    else
    {
        layout();
        redraw();
        //debugging:
        /*
        qDebug() << "Flag1";
        qDebug() << "canvas is at: " << m_canvas;
        GraphLayout *gl = m_canvas->getGraphLayout();
        GraphData *gd = gl->getGraphData();
        qDebug() << "Graph Data =================================";
        qDebug() << "  Node count: " << gd->getNodeCount();
        qDebug() << "  Edge count: " << gd->getEdgeCount();
        qDebug() << "  Constraint count: " << gd->ccs.size();
        */
        //
    }
}

}
