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
*/

//! @file
//! Plugin that adds support for reading and writing Dunnart's native
//! annotated SVG file format.

#include <QtGui>
#include <QObject>
#include <QFileInfo>
#include <QDomDocument>
#include <QSvgGenerator>

#include <string>
#include <math.h>
#include "sbml/SBMLTypes.h"

#include "libdunnartcanvas/fileioplugininterface.h"
#include "libdunnartcanvas/canvas.h"
#include "libdunnartcanvas/canvasitem.h"
#include "libdunnartcanvas/undo.h"
#include "libdunnartcanvas/pluginshapefactory.h"
#include "libdunnartcanvas/shape.h"

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
            int cols = ceil(sqrt(numSpecies));
            int x0 = 0, y0 = 0;
            int u = 30; int sepUnits = 2;
            int x, y;
            for (unsigned int i = 0; i < numSpecies; i++) {
                spec = los->get(i);
                id = spec->getId();
                name = spec->getName();
                x = x0 + sepUnits*u*(i%cols);
                y = y0 + sepUnits*u*(i/cols);
                CanvasItem *newObj = NULL;
                ShapeObj *shape = NULL;
                QString *type = new QString("org.sbgn.pd.01SimpleChemEPN");

                PluginShapeFactory *factory = sharedPluginShapeFactory();
                shape = factory->createShape(*type);
                QSizeF *size = new QSizeF(u,u);
                QPointF *point = new QPointF(x,y);
                shape->setPosAndSize(*point, *size);
                QString *label = new QString(name.c_str());
                shape->setLabel(*label);
                newObj = (CanvasItem*) shape;

                QUndoCommand *cmd = new CmdCanvasSceneAddItem(canvas, newObj);
                canvas->currentUndoMacro()->addCommand(cmd);
            }


            return true;
        }
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
