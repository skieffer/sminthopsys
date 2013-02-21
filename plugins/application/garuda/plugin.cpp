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
//! Plugin that adds support for working with the Garuda platform.

#include <QtGui>
#include <QtCore>

#include "libdunnartcanvas/applicationplugininterface.h"
#include "libdunnartcanvas/canvas.h"
#include "libdunnartcanvas/canvasapplication.h"

//#include "appgarudaclient.h"
#include "garudaclient.h"
#include "openincompatiblesoftwarewidget.h"

using namespace dunnart;


//! @brief  Plugin class that adds support for working with the Garuda platform.
//!
class GarudaApplicationPlugin : public QObject, public ApplicationPluginInterface
{
    Q_OBJECT
    Q_INTERFACES (dunnart::ApplicationPluginInterface)

    public:
        GarudaApplicationPlugin()
            : m_garuda_client(NULL),
              //m_app_garuda_client(NULL),
              m_canvas_application(NULL),
              m_compatible_software(NULL)
        {
        }
        virtual void applicationMainWindowInitialised(
                CanvasApplication *canvasApplication)
        {

            // Category list
            QList<QString> categoryList;
            categoryList.append("Layout");

            // Input files
            QList<QString> inputFiles;
            inputFiles.append("xml");
            inputFiles.append("sbml");

            // Output files
            QList<QString> outputFiles;
            outputFiles.append("xml");
            outputFiles.append("sbml");

            // Icon path and screenshot paths
            QDir pluginsDir = QDir(qApp->applicationDirPath());
        #if defined(Q_OS_MAC)
            if (pluginsDir.dirName() == "MacOS") {
                pluginsDir.cdUp();
            }
            pluginsDir.cd("Resources");
        #endif
            QString iconPath = pluginsDir.absoluteFilePath("DunnartIcon.png");

            QList<QString> screenshotPaths;
            screenshotPaths.append(pluginsDir.absoluteFilePath("DunnartScreen.png"));

            // Launch command
            QString launchCommand = "open " + QCoreApplication::applicationFilePath();


            // Construct the Garuda client
            m_garuda_client = new GarudaClient(this, "Dunnart", "593387e0-7183-11e2-bcfd-0800200c9a66",
                                               "MArVL, Monash University",
                                               "Dunnart is a prototype constraint-based diagram editor. It includes standard diagram editing capabilities, as well as advanced features such as constraint-based geometric placement tools (alignment, distribution, separation, non-overlap, and page containment), automatic object-avoiding poly-line connector routing, and continuous network layout.",
                                               categoryList, inputFiles, outputFiles,
                                               iconPath, screenshotPaths, launchCommand);

            //m_app_garuda_client = new AppGarudaClient(this);
            connect(m_garuda_client, SIGNAL(fileOpenRequest(QString)),
                    this, SLOT(fileOpenRequest(QString)));

            m_canvas_application = canvasApplication;

            if (m_garuda_client->connectedToServer())
            {
                QAction *showCompatibleSoftwareForFileAction =
                        new QAction(tr("Compatible Software for File..."), this);
                connect(showCompatibleSoftwareForFileAction, SIGNAL(triggered()),
                        this, SLOT(showCompatibleSoftwareRequest()));

                connect (m_garuda_client, SIGNAL(showCompatibleSoftwareResponse(QVariantMap)),
                         this, SLOT(showCompatibleSoftwareResponse(QVariantMap)));

                QMenu *garudaMenu = canvasApplication->mainWindow()->
                        menuBar()->addMenu("Garuda");
                garudaMenu->addAction(showCompatibleSoftwareForFileAction);
            }
        }
        virtual void applicationWillClose(CanvasApplication *canvasApplication)
        {
            Q_UNUSED (canvasApplication)

            //m_app_garuda_client->deregisterWithCore();
        }

    private slots:
        void showCompatibleSoftwareRequest()
        {
            m_garuda_client->showCompatibleSoftwareFor(
                "xml", "sbml");
        }

        void showCompatibleSoftwareResponse(const QVariantMap& response)
        {
            if (m_compatible_software)
            {
                m_compatible_software->setContentsForResponse(response);
                m_compatible_software->show();
            }
            else
            {
                m_compatible_software = new OpenInCompatibleSoftwareWidget(
                            m_canvas_application->currentCanvas());
                connect(m_canvas_application, SIGNAL(currentCanvasChanged(Canvas*)),
                        m_compatible_software, SLOT(changeCanvas(Canvas*)));

                m_compatible_software->setGarudaClient(m_garuda_client);
                m_compatible_software->setContentsForResponse(response);
                m_canvas_application->mainWindow()->addDockWidget(Qt::LeftDockWidgetArea, m_compatible_software);
                m_compatible_software->setFloating(true);
                m_compatible_software->show();
            }
        }
        void fileOpenRequest(const QString& filename)
        {
            m_canvas_application->openDiagram(QFileInfo(filename));
        }

    private:
        GarudaClient *m_garuda_client;
        //AppGarudaClient *m_app_garuda_client;
        CanvasApplication *m_canvas_application;
        OpenInCompatibleSoftwareWidget *m_compatible_software;
};

Q_EXPORT_PLUGIN2(application_garuda, GarudaApplicationPlugin)

// Because there is no header file, we need to load the MOC file here to 
// cause Qt to generate it for us.
#include "plugin.moc"

// vim: filetype=cpp ts=4 sw=4 et tw=0 wm=0 cindent
