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
#include <sstream>
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
                return "Systems Biology Markup Language (sbml)";
            }
            else if (extension == "xml")
            {
                return "Systems Biology Markup Language (xml)";
            }
            return QString();
        }
        bool saveDiagramToFile(Canvas *canvas, const QFileInfo& fileInfo,
                QString& errorMessage)
        {
            Q_UNUSED(canvas);
            Q_UNUSED(fileInfo);
            Q_UNUSED(errorMessage);
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

            // Check for errors.
            unsigned int errors = doc->getNumErrors();
            if (errors > 0)
            {
                std::stringstream ss;
                ss << "Error reading SBML.\n";
                doc->printErrors(ss);
                errorMessage = QString(ss.str().c_str());
                return false;
            }

            // Get the species and reactions.
            Model *model = doc->getModel();
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
                }
                // Now add the reaction to its compartment.
                DSBCompartment *comp = compMap.value(compName);
                comp->addReaction(dsbreac);
                // And give it a reference to its compartment.
                dsbreac->setCompartment(comp);
            }

            // Set all trivial clonings.
            QList<DSBCompartment*> comps = compMap.values();
            for (int i = 0; i < comps.size(); i++)
            {
                DSBCompartment *comp = comps.at(i);
                comp->setTrivialCloning();
            }
            // Turn on automatic graph layout.
            canvas->setOptAutomaticGraphLayout(true);
            // Put all compartments in a single container.
            DSBCompartment *comp = new DSBCompartment(QString("_root"));
            comp->setCanvas(canvas);
            comp->setBoundaryVisible(false);
            comp->addCompartments(compMap.values());
            comp->layout();
            comp->setRelPt(QPointF(0,0));
            comp->draw();
            return true;
        }

};


Q_EXPORT_PLUGIN2(fileio_sbml, SBMLFileIOPlugin)

// Because there is no header file, we need to load the MOC file here to 
// cause Qt to generate it for us.
#include "plugin.moc"

// vim: filetype=cpp ts=4 sw=4 et tw=0 wm=0 cindent
