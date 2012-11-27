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

#ifndef DSBCLONE_H
#define DSBCLONE_H

#include <QList>
#include <QString>

#include "dsbreclayout.h"
#include "dsbnode.h"

class QRectF;

namespace dunnart {

class PDEPN;
class DSBSpecies;
class DSBReaction;
class DSBBranch;
class ShapeObj;

class DSBClone : public DSBNode
{

public:
    DSBClone(DSBSpecies *dsbspec);
    void setCloneNum(int num);
    QString getCloneId();
    void addReactionEntered(DSBReaction *reac);
    void addReactionExited(DSBReaction *reac);
    void addReactionModified(DSBReaction *reac);
    void deleteShape();
    void set_is_cloned(bool b);
    DSBSpecies *getSpecies();
    void setReactionsEntered(QList<DSBReaction*> reacs);
    void setReactionsExited(QList<DSBReaction*> reacs);
    void setReactionsModified(QList<DSBReaction*> reacs);
    QSizeF layout();
    void setRelPt(QPointF p);
    void drawRelTo(QPointF q);
    void drawAt(QPointF r);
    void redraw();
    QSizeF getSize();
    QPointF getBasePt();
    QList<DSBBranch*> findBranchesRec(
            QList<QString>& seen, QList<QString> blacklist, bool forward, DSBNode *last);
    friend class DSBFork;
    ShapeObj *getShape();
    QRectF getBbox();

private:
    QString m_cloneId;
    DSBSpecies *m_dsbspec;
    QPointF m_relpt;
    QPointF m_basept;
    QSizeF m_size;
    PDEPN *m_epn;
    bool m_is_cloned;
    QList<DSBReaction *> m_reactionsEntered;
    QList<DSBReaction *> m_reactionsExited;
    QList<DSBReaction *> m_reactionsModified;

    QList<DSBReaction *> computeEnterableReactions();
    QList<DSBReaction *> computeExitableReactions();
};

}

#endif // DSBCLONE_H
