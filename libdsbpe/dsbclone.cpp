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

#include "dsbclone.h"
#include "dsbspecies.h"

#include "libdunnartcanvas/canvas.h"
#include "libdunnartcanvas/pluginshapefactory.h"
#include "plugins/shapes/sbgn/pdepn.h"

namespace dunnart {

DSBClone::DSBClone(DSBSpecies *dsbspec) :
    m_dsbspec(dsbspec)
{}

void DSBClone::addReactionEntered(DSBReaction *reac)
{
    m_reactionsEntered.append(reac);
}

void DSBClone::addReactionExited(DSBReaction *reac)
{
    m_reactionsExited.append(reac);
}

void DSBClone::addReactionModified(DSBReaction *reac)
{
    m_reactionsModified.append(reac);
}

void DSBClone::setReactionsEntered(QList<DSBReaction *> reacs)
{
    while (!m_reactionsEntered.isEmpty()) { m_reactionsEntered.removeFirst(); }
    for (int i = 0; i < reacs.size(); i++) {
        m_reactionsEntered.append(reacs.at(i));
    }
}

void DSBClone::setReactionsExited(QList<DSBReaction *> reacs)
{
    while (!m_reactionsExited.isEmpty()) { m_reactionsExited.removeFirst(); }
    for (int i = 0; i < reacs.size(); i++) {
        m_reactionsExited.append(reacs.at(i));
    }
}

void DSBClone::setReactionsModified(QList<DSBReaction *> reacs)
{
    while (!m_reactionsModified.isEmpty()) { m_reactionsModified.removeFirst(); }
    for (int i = 0; i < reacs.size(); i++) {
        m_reactionsModified.append(reacs.at(i));
    }
}

void DSBClone::deleteShape()
{
    Canvas *canvas = m_dsbspec->canvas();
    canvas->deselectAll();
    m_epn->setSelected(true);
    canvas->deleteSelection();
}

DSBSpecies *DSBClone::getSpecies()
{
    return m_dsbspec;
}

void DSBClone::set_is_cloned(bool b)
{
    m_is_cloned = b;
}

QSizeF DSBClone::layout()
{
    // TODO: Figure out layout of label, and then base the size of
    // the node on the result.
    // For now:
    m_size = QSizeF(70,50);
    return m_size;
}

void DSBClone::setRelPt(QPointF p)
{
    m_relpt = p;
}

void DSBClone::drawRelTo(QPointF q)
{
    QPointF r = m_relpt + q;
    drawAt(r);
}

void DSBClone::redraw()
{
    drawAt(m_basept);
}

void DSBClone::drawAt(QPointF r)
{
    m_basept = r;
    PluginShapeFactory *factory = sharedPluginShapeFactory();
    QString type("org.sbgn.pd.00UnspecifiedEPN");
    ShapeObj *shape = factory->createShape(type);
    m_epn = dynamic_cast<PDEPN*>  (shape);
    // Give the epn a pointer to the species it represents.
    m_epn->setClone(this);
    // Set its properties.
    // Clone marker state
    m_epn->set_is_cloned(m_is_cloned);
    // Size
    m_epn->setSize(m_size);
    // Position
    m_epn->setCentrePos(m_basept);
    // Label
    m_epn->setLabel(m_dsbspec->getName());
    // Add it to the canvas.
    QUndoCommand *cmd = new CmdCanvasSceneAddItem(m_dsbspec->canvas(), m_epn);
    m_dsbspec->canvas()->currentUndoMacro()->addCommand(cmd);
}

}
