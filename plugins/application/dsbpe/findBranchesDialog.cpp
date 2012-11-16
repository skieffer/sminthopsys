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

//diag:
#include <iostream>
#include "libdunnartcanvas/templates.h"
//

#include "findBranchesDialog.h"
#include "libdunnartcanvas/canvas.h"
#include "libdunnartcanvas/canvasitem.h"
#include "libdunnartcanvas/shape.h"

#include "plugins/shapes/sbgn/pdepn.h"
#include "libdsbpe/dsbclone.h"
#include "libdsbpe/dsbspecies.h"
#include "libdsbpe/dsbcompartment.h"

namespace dunnart {

FindBranchesDialog::FindBranchesDialog(Canvas *canvas, QWidget *parent)
    : QDialog(parent)
{
    m_canvas = canvas;

    // Build the dialog.
    QLabel *endpointLabel = new QLabel(this);
    endpointLabel->setText(tr("Endpoint species:"));

    m_endpointEdit = new QLineEdit(this);

    m_endpointCBox = new QComboBox(this);
    m_endpointCBox->addItem(tr("start"));
    m_endpointCBox->addItem(tr("finish"));

    QLabel *layoutLabel = new QLabel(this);
    layoutLabel->setText(tr("Layout:"));

    m_layoutCBox = new QComboBox(this);
    m_layoutCBox->addItem(tr("Vertical"));
    m_layoutCBox->addItem(tr("Horizontal"));
    m_layoutCBox->addItem(tr("Clockwise"));
    m_layoutCBox->addItem(tr("Counterclockwise"));

    QPushButton *findButton = new QPushButton(tr("Find"),this);
    findButton->setDefault(true);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"),this);

    connect(findButton, SIGNAL(clicked()), this, SLOT(findBranches()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(endpointLabel,  0, 0);
    layout->addWidget(m_endpointEdit, 0, 1);
    layout->addWidget(m_endpointCBox,   0, 2);
    layout->addWidget(layoutLabel,    1, 0);
    layout->addWidget(m_layoutCBox,     1, 1);
    layout->addWidget(cancelButton,   2, 0);
    layout->addWidget(findButton,     2, 1);
    setLayout(layout);

    setWindowTitle(tr("Find Branches"));

    // Populate endpoint box.
    getSelectedClone();

    // Watch change of selection.
    connect(m_canvas, SIGNAL(selectionChanged()),
            this, SLOT(canvasSelectionChanged()));
}

/* Respond to a change in the canvas selection.
 */
void FindBranchesDialog::canvasSelectionChanged()
{
    getSelectedClone();
}

/* Consult the canvas selection, and check whether exactly one
   shape node is selected. If so, populate edit box with its name.
   Otherwise, put a message requesting selection of just one shape.
 */
void FindBranchesDialog::getSelectedClone()
{
    CanvasItemList selection = m_canvas->selectedItems();
    int n = selection.size();
    if (n != 1)
    {
        m_endpointEdit->setText(tr("not exactly one node selected"));
    }
    else // Exactly one object was selected.
    {
        CanvasItem *item = selection.first();
        //ShapeObj *shape = qobject_cast<ShapeObj *> (item);
        PDEPN *epn = dynamic_cast<PDEPN *>(item);
        if (epn)
        {
            m_endpointEdit->setText(epn->getLabel());
            m_endpointIDString = epn->idString();
            m_endpointClone = epn->getClone();
        }
        else
        {
            m_endpointEdit->setText(tr("selection is not an EPN"));
        }

    }

}

/* Carry out the find-branches action, as specified in the dialog box.
  */
void FindBranchesDialog::findBranches()
{
    // TODO: Find the branches!
    //
    // random testing:
    //LinearTemplate *lintemp = new LinearTemplate(0,0,TEMPLATE_LINEAR_VERT,m_canvas);
    //
    if (m_endpointClone)
    {
        // Get the compartment in which the endpoint clone lives.
        DSBCompartment *comp = m_endpointClone->getSpecies()->getCompartment();

        // Determine whether we're searching forward, or backward.
        int whichEnd = m_endpointCBox->currentIndex();
        bool forward = (whichEnd == 0);

        // Ask the compartment to switch to a "longest branch layout", and redraw.
        comp->longestBranchLayout(m_endpointClone, forward);
        comp->redraw();
    }
    //
    accept();
}

}
