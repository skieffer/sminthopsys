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
//

#include "findBranchesDialog.h"
#include "libdunnartcanvas/canvas.h"
#include "libdunnartcanvas/canvasitem.h"
#include "libdunnartcanvas/shape.h"

namespace dunnart {

FindBranchesDialog::FindBranchesDialog(Canvas *canvas, QWidget *parent)
    : QDialog(parent)
{
    m_canvas = canvas;

    // Build the dialog.
    QLabel *endpointLabel = new QLabel(this);
    endpointLabel->setText(tr("Endpoint species:"));

    endpointEdit = new QLineEdit(this);

    QComboBox *endpointCBox = new QComboBox(this);
    endpointCBox->addItem(tr("start"));
    endpointCBox->addItem(tr("finish"));

    QLabel *layoutLabel = new QLabel(this);
    layoutLabel->setText(tr("Layout:"));

    QComboBox *layoutCBox = new QComboBox(this);
    layoutCBox->addItem(tr("Vertical"));
    layoutCBox->addItem(tr("Horizontal"));
    layoutCBox->addItem(tr("Clockwise"));
    layoutCBox->addItem(tr("Counterclockwise"));

    QPushButton *findButton = new QPushButton(tr("Find"),this);
    findButton->setDefault(true);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"),this);

    connect(findButton, SIGNAL(clicked()), this, SLOT(findBranches()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(endpointLabel, 0, 0);
    layout->addWidget(endpointEdit,  0, 1);
    layout->addWidget(endpointCBox,  0, 2);
    layout->addWidget(layoutLabel,   1, 0);
    layout->addWidget(layoutCBox,    1, 1);
    layout->addWidget(cancelButton,    2, 0);
    layout->addWidget(findButton,    2, 1);
    setLayout(layout);

    setWindowTitle(tr("Find Branches"));

    // Populate endpoint box.
    getSelectedSpecies();

    // Watch change of selection.
    connect(m_canvas, SIGNAL(selectionChanged()),
            this, SLOT(canvasSelectionChanged()));
}

/* Respond to a change in the canvas selection.
 */
void FindBranchesDialog::canvasSelectionChanged()
{
    getSelectedSpecies();
}

/* Consult the canvas selection, and check whether exactly one
   shape node is selected. If so, populate edit box with its name.
   Otherwise, put a message requesting selection of just one shape.

   FIXME: We should be demanding not just shapes, but SBGN shapes.
 */
void FindBranchesDialog::getSelectedSpecies()
{
    CanvasItemList selection = m_canvas->selectedItems();
    int n = selection.size();
    if (n != 1)
    {
        endpointEdit->setText(tr("not exactly one node selected"));
    }
    else // Exactly one object was selected.
    {
        CanvasItem *item = selection.first();
        ShapeObj *shape = qobject_cast<ShapeObj *> (item);
        if (shape)
        {
            endpointEdit->setText(shape->getLabel());
            endpointIDString = shape->idString();
        }
        else
        {
            endpointEdit->setText(tr("selection is not a node"));
        }

    }

}

/* Carry out the find-branches action, as specified in the dialog box.
  */
void FindBranchesDialog::findBranches()
{
    // TODO: Find the branches!
    accept();
}

}