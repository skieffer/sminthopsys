/*
 * Sminthopsys - Dunnart Systems Biology plugin
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

#include <QObject>
#include <QtGui>
#include <QString>
#include <QList>
#include <QSet>
#include <QRectF>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QTimer>
#include <QThread>

#include <math.h>
#include <assert.h>

#include "dsbspecies.h"
#include "dsbreaction.h"
#include "dsbclone.h"
#include "dsbnode.h"
#include "dsbbranch.h"
#include "dsbpathway.h"
#include "freepathway.h"

#include "libdunnartcanvas/canvas.h"
#include "libdunnartcanvas/cluster.h"

//debugging:
#include "libdunnartcanvas/graphlayout.h"
#include "libdunnartcanvas/graphdata.h"
#include "pdepn.h"
//

namespace dunnart {

CompartmentShape::CompartmentShape(DSBCompartment *comp, qreal x, qreal y, qreal w, qreal h) :
    m_compartment(comp),
    m_penWidth(10),
    m_cornerRadius(20)
{
    setX(x);
    setY(y);
    resize(w,h);
}

void CompartmentShape::resize(qreal w, qreal h)
{
    qreal cr2 = 2*m_cornerRadius;
    m_width  = (w > cr2 ? w : cr2);
    m_height = (h > cr2 ? h : cr2);
    update();
}

QRectF CompartmentShape::boundingRect() const
{
    QRectF rect = QRectF(- m_penWidth/2, - m_penWidth/2,
                  m_width + m_penWidth, m_height + m_penWidth);
    return rect;
}

void CompartmentShape::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPen pen;
    pen.setWidthF(m_penWidth);
    painter->setPen(pen);
    QBrush brush(Qt::white);
    painter->setBrush(brush);
    QPainterPath path;
    QRectF rect = QRectF(0,0,m_width,m_height);
    path.addRoundedRect(rect, m_cornerRadius, m_cornerRadius);
    painter->drawPath(path);
}

void CompartmentShape::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_compartment->getCanvas() == NULL)
    {
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        // Drop through to parent handler.
    }
    else if (event->button() == Qt::RightButton)
    {
        QMenu menu;
        QAction *action = buildAndExecContextMenu(event, menu);
        if (action)
        {
            event->accept();
        }
    }

    QGraphicsItem::mousePressEvent(event);
}

QAction *CompartmentShape::buildAndExecContextMenu(QGraphicsSceneMouseEvent *event, QMenu &menu)
{
    if (!menu.isEmpty())
    {
        menu.addSeparator();
    }
    // Add actions to menu.
    QAction *cloneCurrencyMolecules = menu.addAction(QObject::tr("Clone currency molecules"));
    QAction *showPathways = menu.addAction(QObject::tr("Show pathways"));
    //

    QApplication::restoreOverrideCursor();
    QAction *action = menu.exec(event->screenPos());

    if (action == cloneCurrencyMolecules)
    {
        m_compartment->cloneCurrencyMolecules();
        m_compartment->redisplay();
    }
    else if (action == showPathways)
    {
        m_compartment->buildConnectedPathways();
        m_compartment->redisplay();
    }
    return action;
}

DSBCompartment::DSBCompartment(QString compartmentName)
    : m_compartmentName(compartmentName),
      m_parentCompartment(NULL),
      m_show_reactions(false),
      m_boundaryVisible(true),
      m_boundaryShape(NULL),
      m_cluster(NULL)
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

void DSBCompartment::setBoundaryVisible(bool b)
{
    m_boundaryVisible = b;
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

Canvas *DSBCompartment::getCanvas()
{
    return m_canvas;
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

void DSBCompartment::acceptCanvasBaseAndRelPts(QPointF parentBasePt)
{
    if (m_boundaryShape)
    {
        QRectF rect = m_boundaryShape->boundingRect();
        m_basept = rect.topLeft();
    }
    m_relpt = m_basept - parentBasePt;
    foreach (DSBCompartment *comp, m_compartments)
    {
        comp->acceptCanvasBaseAndRelPts(m_basept);
    }
    foreach (DSBPathway *pw, m_pathways)
    {
        pw->acceptCanvasBaseAndRelPts(m_basept);
    }
    QList<DSBClone*> loose = getLooseClones();
    foreach (DSBClone *cl, loose)
    {
        cl->acceptCanvasBaseAndRelPts(m_basept);
    }
}

QSizeF DSBCompartment::rowLayout()
{
    int topPad=50, bottomPad=50, leftPad=50, rightPad=50;
    int horizSpacer=100;
    int x = leftPad;
    int y = topPad;
    int width = leftPad+rightPad;
    int height = topPad+bottomPad;
    int maxObjHeight = 0;
    int objCount = 0;

    // Compartments
    for (int i = 0; i < m_compartments.size(); i++)
    {
        DSBCompartment *comp = m_compartments.at(i);
        QSizeF size = comp->layout();
        if (objCount > 0) { x += horizSpacer; }
        comp->setRelPt(QPointF(x,y));
        x += size.width();
        width += size.width();
        int h = size.height();
        maxObjHeight = (h > maxObjHeight? h : maxObjHeight);
        objCount++;
    }

    // Pathways
    for (int i = 0; i < m_pathways.size(); i++)
    {
        DSBPathway *pw = m_pathways.at(i);
        QSizeF size = pw->layout();
        if (objCount > 0) { x += horizSpacer; }
        pw->setRelPt(QPointF(x,y));
        x += size.width();
        width += size.width();
        int h = size.height();
        maxObjHeight = (h > maxObjHeight? h : maxObjHeight);
        objCount++;
    }

    // Loose clones
    QList<DSBClone*> loose = getLooseClones();
    if (objCount > 0) { x += horizSpacer; }
    QSizeF size = layoutSquareCloneArray(loose, x, y);
    x += size.width();
    width += size.width();
    int h = size.height();
    maxObjHeight = (h > maxObjHeight? h : maxObjHeight);
    objCount++;

    height += maxObjHeight;
    m_size = QSizeF(width,height);

    // KLUDGE:
    if (!m_pathways.empty()) { m_size = QSizeF(2000,1500); }
    //

    return m_size;
}

/* Set all clonings to trivial.
  */
void DSBCompartment::setTrivialCloning()
{
    m_canvas->stop_graph_layout();
    for (int i = 0; i < m_species.size(); i++)
    {
        m_species.at(i)->setTrivialCloning();
    }
    m_canvas->restart_graph_layout();
}

/* Set discrete clonings for all species named.
  */
void DSBCompartment::setDiscreteCloningsByName(QList<QString> names)
{
    m_canvas->stop_graph_layout();
    foreach (DSBSpecies *spec, m_species)
    {
        QString name = spec->getName();
        if (names.contains(name))
        {
            spec->setDiscreteCloning();
        }
    }
    m_canvas->restart_graph_layout();
}

void DSBCompartment::cloneCurrencyMolecules()
{
    setDiscreteCloningsByName(m_default_blacklist);
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
    int rows = ceil(numClones/float(cols)); // number of rows
    int horizSpacer = 30;
    int vertSpacer  = 30;
    int w = 70, h = 50;
    int x0 = ulx + w/2, y0 = uly + h/2;
    int x, y, col, row;
    for (int i = 0; i < numClones; i++)
    {
        col = i%cols;
        row = i/cols;
        x = x0 + col*(w + horizSpacer);
        y = y0 + row*(h + vertSpacer);
        clones.at(i)->setRelPt(QPointF(x,y));
    }
    int width  = w + (cols - 1)*(w + horizSpacer);
    int height = h + (rows - 1)*(h + vertSpacer);
    return QSizeF(width,height);
}

QList<DSBBranch*> DSBCompartment::findBranches(DSBClone *endpt, bool forward)
{
    return findBranches(endpt, forward, m_default_blacklist);
}

QList<DSBBranch*> DSBCompartment::findBranches(
        DSBClone *endpt, bool forward, QList<QString> blacklist)
{
    /*
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
    */

    // Find branches.
    bool extended = true; // Throw away branches of length 1.
    QList<DSBBranch*> branches = endpt->findBranches(blacklist, forward, extended);
    for (int i = 0; i < branches.size(); i++) {
        qDebug() << branches.at(i)->toString();
    }
    return branches;
}

void DSBCompartment::addPathway(DSBPathway *pw)
{
    m_pathways.append(pw);
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
    //qDebug() << "=====================================================================";
    //qDebug() << m_compartmentName << " basept: " << m_basept.x() << ", " << m_basept.y();

    // Compartment boundary
    if (m_boundaryVisible)
    {
        // Boundary shape
        if (!m_boundaryShape)
        {
            m_boundaryShape = new CompartmentShape(this, m_basept.x(), m_basept.y(),
                                                          m_size.width(), m_size.height());
            m_canvas->addItem(m_boundaryShape);
        }
        else
        {
            m_boundaryShape->setPos(m_basept.x(), m_basept.y());
            m_boundaryShape->resize(m_size.width(), m_size.height());
        }
#if 0
        // Bounding cluster
        if (!m_cluster)
        {
            CanvasItemList cil;
            QString id = m_compartmentName + "_cluster";
            bool rectangular = true;
            m_cluster = new Cluster(cil,id,rectangular);
            //m_cluster->rectangular = true;
            qreal cx = m_basept.x() + m_size.width()/2.0;
            qreal cy = m_basept.y() + m_size.height()/2.0;
            m_cluster->setCentrePos(QPointF(cx,cy));
            m_cluster->setSize(m_size);
            m_canvas->addItem(m_cluster);
        }
        else
        {
            qreal cx = m_basept.x() + m_size.width()/2.0;
            qreal cy = m_basept.y() + m_size.height()/2.0;
            m_cluster->setCentrePos(QPointF(cx,cy));
            m_cluster->setSize(m_size);
        }
#endif
    }

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

    //debug:
    //dumpAllClonePositions();
    //dumpPathwayNodePositions();
    //
}

void DSBCompartment::redisplay(bool reLayout)
{
    if (m_parentCompartment)
    {
        m_parentCompartment->redisplay(reLayout);
    }
    else
    {
        if (reLayout)
        {
            layout();
        }
        redraw();
        // The following two commands are necessary in order to get
        // the layout to respond. Neither one alone is sufficient.
        m_canvas->getActions().clear();
        m_canvas->restart_graph_layout();
    }
}

// TODO This function was for debugging and can be removed.
void DSBCompartment::jogPathways()
{
    foreach (DSBPathway *pw, m_pathways)
    {
        //Try to jog the canvas to get layout to take effect!
        DSBClone *cl = pw->getLeadClone();
        if (cl)
        {
            //ShapeObj *shape = cl->getShape();
            //m_canvas->restart_graph_layout();
            //m_canvas->restart_graph_layout();
            //m_canvas->processResponseTasks();
            //m_canvas->processResponseTasks();
            m_canvas->getActions().clear();
            m_canvas->restart_graph_layout();

            /*
            Actions& actions = m_canvas->getActions();
            //
            actions.moveList.push_back(shape);
            cl->moveShape(1,1);
            m_canvas->interrupt_graph_layout();
            //
            actions.moveList.push_back(shape);
            cl->moveShape(1,1);
            m_canvas->interrupt_graph_layout();
            */
        }
    }
    foreach (DSBCompartment *comp, m_compartments)
    {
        comp->jogPathways();
    }
}

/* For debugging output.
  */
void DSBCompartment::dumpPathwayNodePositions()
{
    foreach(DSBPathway *pw, m_pathways)
    {
        foreach(DSBBranch *br, pw->getBranches())
        {
            foreach(DSBNode *nd, br->nodes)
            {
                ShapeObj *shape = nd->getShape();
                DSBClone *cl = dynamic_cast<DSBClone*>(nd);
                DSBReaction *re = dynamic_cast<DSBReaction*>(nd);
                if (cl)
                {
                    if (shape) {
                        qDebug() << cl->getSpecies()->getName() << " " << cl->getCloneId() << " " << cl->getBasePt() << " " << shape->pos();
                    } else {
                        qDebug() << cl->getSpecies()->getName() << " " << cl->getCloneId() << " " << cl->getBasePt() << " no shape";
                    }
                }
                else if (re)
                {
                    if (shape) {
                        qDebug() << re->getName() << " " << re->getBasePt() << " " << shape->pos();
                    } else {
                        qDebug() << re->getName() << " " << re->getBasePt() << " no shape";
                    }
                }
            }
        }
    }
}

// More debugging output
void DSBCompartment::dumpAllClonePositions()
{
    QList<DSBClone*> clones = getAllClones();
    foreach (DSBClone *cl, clones)
    {
        ShapeObj *shape = cl->getShape();
        if (shape) {
            qDebug() << cl->getSpecies()->getName() << " " << cl->getCloneId() << " " << cl->getBasePt() << " " << shape->pos();
        } else {
            qDebug() << cl->getSpecies()->getName() << " " << cl->getCloneId() << " " << cl->getBasePt() << " no shape";
        }
    }
}

/* Build one pathway (FreePathway class) for each connected component made
   up by the reactions and the current clones.
  */
void DSBCompartment::buildConnectedPathways()
{
    QSet<DSBClone*> clones = getAllClones().toSet();
    while (!clones.empty())
    {
        DSBClone *cl = clones.toList().first();
        QSet<DSBClone*> ccClones; // connected component clones
        QSet<DSBReaction*> ccReacs; // connected component reactions
        cl->connectedComponent(ccClones, ccReacs);
        FreePathway *pw = new FreePathway(ccClones.toList(), ccReacs.toList());
        m_pathways.append(pw);
        clones.subtract(ccClones);
    }
}


}



















