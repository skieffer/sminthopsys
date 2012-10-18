/*
 * Garuda plugin - Enables communication with other Garuda enabled apps.
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
 * Author(s): Michael Wybrow  <http://michael.wybrow.info/>
*/

//! @file
//! Plugin that provides methods for manipulating biological pathways
//! in systems biology diagrams.

#include <QtGui>
#include <QtCore>

#include "libdunnartcanvas/applicationplugininterface.h"
#include "libdunnartcanvas/canvas.h"
#include "libdunnartcanvas/canvasapplication.h"

#include "plugins/application/dsbpe/findBranchesDialog.h"

using namespace dunnart;


//! @brief  Plugin class that adds support for working with the Garuda platform.
//!
class DSBPEApplicationPlugin : public QObject, public ApplicationPluginInterface
{
    Q_OBJECT
    Q_INTERFACES (dunnart::ApplicationPluginInterface)

    public:
        DSBPEApplicationPlugin()
            : m_canvas_application(NULL)
        {
        }
        virtual void applicationMainWindowInitialised(
                CanvasApplication *canvasApplication)
        {
            m_canvas_application = canvasApplication;

            QAction *findBranchesAction = new QAction(tr("Find branches..."), this);
            connect(findBranchesAction, SIGNAL(triggered()), this,
                    SLOT(showFindBranchesDialog()));

            QMenu *dsbpeMenu = canvasApplication->mainWindow()->
                    menuBar()->addMenu("SB Pathways");
            dsbpeMenu->addAction(findBranchesAction);

        }
        virtual void applicationWillClose(CanvasApplication *canvasApplication)
        {
            Q_UNUSED (canvasApplication)
        }

    private slots:
        void showFindBranchesDialog()
        {
            Canvas *canvas = m_canvas_application->currentCanvas();
            QMainWindow *mainWindow = m_canvas_application->mainWindow();
            /*
            FindBranchesDialog *fbDialog = new FindBranchesDialog(
                        m_canvas_application->mainWindow().canvas(), m_canvas_application->mainWindow());
            */
            FindBranchesDialog *fbDialog = new FindBranchesDialog(canvas, mainWindow);
            fbDialog->show();
            fbDialog->raise();
            fbDialog->activateWindow();
        }

    private:
        CanvasApplication *m_canvas_application;
};

Q_EXPORT_PLUGIN2(application_dsbpe, DSBPEApplicationPlugin)

// Because there is no header file, we need to load the MOC file here to 
// cause Qt to generate it for us.
#include "plugin.moc"

// vim: filetype=cpp ts=4 sw=4 et tw=0 wm=0 cindent
