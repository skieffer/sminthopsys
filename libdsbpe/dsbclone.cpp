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

#include <QtGui>
#include <QSet>

#include "dsbclone.h"
#include "dsbspecies.h"
#include "dsbreaction.h"

#include "libdunnartcanvas/canvas.h"
#include "libdunnartcanvas/pluginshapefactory.h"
#include "plugins/shapes/sbgn/pdepn.h"

namespace dunnart {

DSBClone::DSBClone(DSBSpecies *dsbspec) :
    m_dsbspec(dsbspec)
{}

void DSBClone::setCloneNum(int num)
{
    QString snum = QString::number(num);
    QString specId = m_dsbspec->getId();
    m_cloneId = specId + "_clone" + snum;
}

QString DSBClone::getCloneId()
{
    return m_cloneId;
}

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

/* Both reactions entered, and reversible reactions exited, are
  enterable. Compute list of all those.
  */
QList<DSBReaction*> DSBClone::computeEnterableReactions()
{
    QSet<DSBReaction*> enterable;
    // Get reactions entered.
    for (int i = 0; i < m_reactionsEntered.size(); i++)
    {
        DSBReaction *reac = m_reactionsEntered.at(i);
        enterable.insert(reac);
    }
    // Examine reactions exited.
    for (int i = 0; i < m_reactionsExited.size(); i++)
    {
        DSBReaction *reac = m_reactionsExited.at(i);
        // We can only use it if it is reversible.
        if (reac->isReversible()) { enterable.insert(reac); }
    }
    return QList<DSBReaction*>::fromSet(enterable);
}

QList<DSBBranch*> DSBClone::findBranchesRec(QList<QString> &seen, DSBNode *last)
{
    seen.append(m_cloneId); // Mark self as seen.

    QList<DSBBranch*> branches; // Prepare return value.

    // Now consider all enterable reactions.
    QList<DSBReaction*> enterable = computeEnterableReactions();
    for (int i = 0; i < enterable.size(); i++)
    {
        DSBReaction *reac = enterable.at(i);

        // Do not turn around and go backwards.
        if (reac == last) {continue;}

        // Are we avoiding transporter processes?
        if (!DSBNode::s_followTransporters && reac->isIntercompartmental()) {
            continue;
        }

        // Consider whether this reaction has already been seen or not.
        QString rid = reac->getReactionId();
        if (seen.contains(rid))
        {
            // Reaction has already been seen, so we have found a cycle.
            DSBBranch *b = new DSBBranch;
            b->nodes.append(reac);
            b->cycle = true;
            branches.append(b);
        }
        else
        {
            // No cycle. Recurse.
            QList<DSBBranch*> bb = reac->findBranchesRec(seen, this);
            branches.append(bb);
        }
    }
    return mergeSelfWithBranches(branches);
}

}





















