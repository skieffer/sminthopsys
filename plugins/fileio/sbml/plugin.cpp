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
#include "libdsbpe/dsbabstractcontainer.h"
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
        bool loadDiagramFromFile(Canvas *canvas, const QFileInfo& fileInfo,
                QString& errorMessage)
        {
            QString filename = fileInfo.absoluteFilePath();
            SBMLDocument* doc = readSBML(filename.toStdString().c_str());

            // Check for errors.
            unsigned int errors = doc->getNumErrors();
            if (errors > 0)
            {
                errorMessage = tr("Error reading SBML.");
                //TODO: Use doc.printErrors(stream=...) to write a description of
                // the errors to a std::ostream object. Then append this to
                // the errorMessage.
                return false;
            }

            // Get the species and reactions.
            Model* model = doc->getModel();
            ListOfSpecies* los = model->getListOfSpecies();
            unsigned int numSpecies = los->size();
            Species* spec;
            std::string id;
            std::string name;
            // Will layout in a square array.
            int cols = ceil(sqrt(numSpecies)); // number of columns in array
            int u = 50; // unit of separation
            int sepUnits = 2; // separation between adjacent nodes, in units u
            int x0 = 0, y0 = 0, x, y;
            PluginShapeFactory *factory = sharedPluginShapeFactory();
            // Build a map from species id's to the internal objects representing those species.
            QMap<QString,dunnart::DSBSpecies> *speciesMap = new QMap<QString,dunnart::DSBSpecies>();
            // Also a map from container names to DSBAbstractContainer objects.
            QMap<QString,DSBAbstractContainer> *containerMap = new QMap<QString,DSBAbstractContainer>();
            for (unsigned int i = 0; i < numSpecies; i++)
            {
                // Get species information.
                spec = los->get(i);
                id = spec->getId();
                name = spec->getName();
                // Construct internal representation.
                dunnart::DSBSpecies *dsbspec = new dunnart::DSBSpecies(spec);
                // Save it in the species map.
                speciesMap->insert(QString(id.c_str()), *dsbspec);
                // If it belongs to a new container, then add it to the container map.
                QString conName = dsbspec->getCompartmentName();
                if (!containerMap->contains(conName))
                {
                    DSBAbstractContainer *con = new DSBAbstractContainer(conName);
                    containerMap->insert(conName,*con);
                }

                // Create shape.
                QString *type = new QString("org.sbgn.pd.00UnspecifiedEPN");
                ShapeObj *shape = factory->createShape(*type);

                //FIXME
                PDEPN *epn = (PDEPN*) &shape;
                //Why can't this be found at runtime?
                //epn->setSpecies(dsbspec);

                // Set its properties.
                // Size: leave as default.
                //QSizeF *size = new QSizeF(70,50);
                // Position
                x = x0 + sepUnits*u*(i%cols);
                y = y0 + sepUnits*u*(i/cols);
                QPointF *point = new QPointF(x,y);
                //shape->setPosAndSize(*point, *size);
                shape->setCentrePos(*point);
                // Label
                QString *label = new QString(name.c_str());
                shape->setLabel(*label);
                // Add it to the canvas.
                QUndoCommand *cmd = new CmdCanvasSceneAddItem(canvas, shape);
                canvas->currentUndoMacro()->addCommand(cmd);
            }

            // Now get the reactions.
            ListOfReactions *lor = model->getListOfReactions();
            unsigned int numReacs = lor->size();
            Reaction *reac;
            QMap<QString,dunnart::DSBReaction> *reactionMap = new QMap<QString,dunnart::DSBReaction>();
            for (unsigned int i = 0; i < numReacs; i++)
            {
                reac = lor->get(i);
                id = reac->getId();
                dunnart::DSBReaction *dsbreac = new dunnart::DSBReaction(reac);
                reactionMap->insert(QString(id.c_str()), *dsbreac);
                dsbreac->doublyLink(speciesMap);
            }

            return true;
        }

        // TODO: If not using this method, then delete it:
        static QString nodeToString(const QDomNode& node)
        {
            QString nodeString;
            QTextStream nodeTextStream(&nodeString);
            node.save(nodeTextStream, 4);

            return nodeString;
        }
};


Q_EXPORT_PLUGIN2(fileio_sbml, SBMLFileIOPlugin)

// Because there is no header file, we need to load the MOC file here to 
// cause Qt to generate it for us.
#include "plugin.moc"

// vim: filetype=cpp ts=4 sw=4 et tw=0 wm=0 cindent
