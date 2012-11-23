/*
 * Dunnart - Constraint-based Diagram Editor
 *
 * Copyright (C) 2011  Monash University
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
 *            Steven Kieffer  <http://skieffer.info>
*/

//! @file
//! Plugin class that adds support for loading and saving SBML
//! files.

#include <QtGui>
#include <QObject>
#include <QFileInfo>
#include <QDomDocument>
#include <QSvgGenerator>
#include <QMap>
#include <QList>

#include <string>
#include <math.h>

#include "sbml/SBMLTypes.h"

#include "libdunnartcanvas/fileioplugininterface.h"
#include "libdunnartcanvas/canvas.h"
#include "libdunnartcanvas/canvasitem.h"
#include "libdunnartcanvas/undo.h"
#include "libdunnartcanvas/pluginshapefactory.h"
#include "libdunnartcanvas/shape.h"

#include "libdsbpe/dsbspecies.h"
#include "libdsbpe/dsbreaction.h"
#include "libdsbpe/dsbcompartment.h"
#include "libdsbpe/dsbcell.h"

#include "plugins/shapes/sbgn/pdepn.h"

using namespace dunnart;

//! @brief  Plugin class that adds support for loading and saving SBML
//! files.
//!
class SBMLFileIOPlugin : public QObject, public FileIOPluginInterface
{
    Q_OBJECT
        Q_INTERFACES (dunnart::FileIOPluginInterface)

    public:
        SBMLFileIOPlugin()
        {
        }
        QStringList saveableFileExtensions(void) const
        {
            QStringList fileTypes;
            fileTypes << "sbml";
            return fileTypes;
        }
        QStringList loadableFileExtensions(void) const
        {
            QStringList fileTypes;
            fileTypes << "xml";
            fileTypes << "sbml";
            return fileTypes;
        }
        QString fileExtensionDescription(const QString& extension) const
        {
            if (extension == "sbml")
            {
                return "Systems Biology Markup Language";
            }
            else if (extension == "xml")
            {
                return "Systems Biology Markup Language";
            }
            return QString();
        }
        bool saveDiagramToFile(Canvas *canvas, const QFileInfo& fileInfo,
                QString& errorMessage)
        {
            // TODO
            return false;
        }
        // TODO: If not using this method, then delete it:
        static QString nodeToString(const QDomNode& node)
        {
            QString nodeString;
            QTextStream nodeTextStream(&nodeString);
            node.save(nodeTextStream, 4);

            return nodeString;
        }
        bool loadDiagramFromFile(Canvas *canvas, const QFileInfo& fileInfo,
                QString& errorMessage)
        {
            QString filename = fileInfo.absoluteFilePath();
            SBMLDocument *doc = readSBML(filename.toStdString().c_str());
            Model *model = doc->getModel();

            // Check for errors.
            unsigned int errors = doc->getNumErrors();
            if (errors > 0)
            {
                errorMessage = tr("Error reading SBML.");
                // TODO: Use doc->printErrors(stream=...) to write a description of
                // the errors to a std::ostream object. Then append this to
                // the errorMessage.
                return false;
            }

            // Get the species and reactions.
            ListOfSpecies *los = model->getListOfSpecies();
            unsigned int numSpecies = los->size();
            Species *spec;
            std::string id;

            // Build a map from species id's to internal objects representing those species.
            QMap<QString, DSBSpecies*> speciesMap;

            // Also a map from compartment names to DSBCompartment objects.
            QMap<QString, DSBCompartment*> compMap;

            for (unsigned int i = 0; i < numSpecies; i++)
            {
                spec = los->get(i);
                id = spec->getId();
                DSBSpecies *dsbspec = new DSBSpecies(spec);
                dsbspec->setCanvas(canvas);

                // Save it in the species map.
                speciesMap.insert(QString(id.c_str()), dsbspec);

                // If it belongs to a new compartment, then add that to the compartment map.
                QString compName = dsbspec->getCompartmentName();
                if (!compMap.contains(compName))
                {
                    DSBCompartment *comp = new DSBCompartment(compName);
                    compMap.insert(compName, comp);
                    comp->setCanvas(canvas);
                    qDebug() << "Canvas is: " << canvas;
                }
                // Now add the species to its compartment.
                DSBCompartment *comp = compMap.value(compName);
                comp->addSpecies(dsbspec);
                // And give it a reference to its compartment.
                dsbspec->setCompartment(comp);
            }

            // Now get the reactions.
            ListOfReactions *lor = model->getListOfReactions();
            unsigned int numReacs = lor->size();
            Reaction *reac;
            for (unsigned int i = 0; i < numReacs; i++)
            {
                reac = lor->get(i);
                id = reac->getId();
                DSBReaction *dsbreac = new DSBReaction(reac);
                dsbreac->doublyLink(speciesMap);
                dsbreac->setCanvas(canvas);

                // If it belongs to a new compartment, then add that to the compartment map.
                QString compName = dsbreac->getCompartmentName();
                if (!compMap.contains(compName))
                {
                    DSBCompartment *comp = new DSBCompartment(compName);
                    compMap.insert(compName, comp);
                    comp->setCanvas(canvas);
                    qDebug() << "Canvas is: " << canvas;
                }
                // Now add the reaction to its compartment.
                DSBCompartment *comp = compMap.value(compName);
                comp->addReaction(dsbreac);
                // And give it a reference to its compartment.
                dsbreac->setCompartment(comp);
            }

            // Do simple, square layout of each compartment, and
            // lay them out side by side.
            DSBCell *cell = new DSBCell();
            cell->setCompartments( compMap.values() );
            cell->rowLayout();
            cell->setRelPt(QPointF(0,0));
            cell->draw();
            canvas->interrupt_graph_layout();
            return true;
        }

};


Q_EXPORT_PLUGIN2(fileio_sbml, SBMLFileIOPlugin)

// Because there is no header file, we need to load the MOC file here to 
// cause Qt to generate it for us.
#include "plugin.moc"

// vim: filetype=cpp ts=4 sw=4 et tw=0 wm=0 cindent
