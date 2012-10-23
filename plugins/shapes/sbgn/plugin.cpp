/*
 * Dunnart - Constraint-based Diagram Editor
 *
 * Copyright (C) 2010-2011  Monash University
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
 * Author(s): Sarah Boyd  <Sarah.Boyd@monash.edu>
*/


#include <QtGui>
#include <QObject>

#include "libdunnartcanvas/shapeplugininterface.h"
#include "libdunnartcanvas/shape.h"

using namespace dunnart;

#include "pdepn.h"
#include "pdunspecifiedepn.h"
#include "pdsimplechemepn.h"
#include "pdmacromolepn.h"
#include "pdnucleicepn.h"
#include "pdperturbingepn.h"
#include "pdsourcesink.h"
#include "pdcomplexepn.h"
#include "pdprocessnode.h"
#include "pdphenotypeprocessnode.h"

class SBGNShapesPlugin : public QObject, public ShapePluginInterface
{
    Q_OBJECT
        Q_INTERFACES (dunnart::ShapePluginInterface)

    public:
        SBGNShapesPlugin()
        {
        }
        QString shapesClassLabel(void) const
        {
            return "SBGN";
        }
        QStringList producableShapeTypes() const
        {
            QStringList shapes;
            shapes << "org.sbgn.pd.00UnspecifiedEPN";
            shapes << "org.sbgn.pd.01SimpleChemEPN";
            shapes << "org.sbgn.pd.02MacromolEPN";
            shapes << "org.sbgn.pd.03NucleicAcidEPN";
            shapes << "org.sbgn.pd.04PerturbingEPN";
            shapes << "org.sbgn.pd.05SourceOrSink";
            shapes << "org.sbgn.pd.06ComplexEPN";
            shapes << "org.sbgn.pd.ProcessNodeVertical";
            shapes << "org.sbgn.pd.ProcessNodeHorizontal";
            shapes << "org.sbgn.pd.UnknownProcessNode";
            shapes << "org.sbgn.pd.OmittedProcessNode";
            shapes << "org.sbgn.pd.AssociationProcessNode";
            shapes << "org.sbgn.pd.DissociationProcessNode";
            shapes << "org.sbgn.pd.PhenotypeProcessNode";
            return shapes;
        }
        ShapeObj *generateShapeOfType(QString shapeType)
        {
            if (shapeType == "org.sbgn.pd.00UnspecifiedEPN")
            {
                return new UnspecifiedEPN("LABEL", false);
            }
            else if (shapeType == "org.sbgn.pd.01SimpleChemEPN")
            {
                return new SimpleChemEPN("LABEL", false, false);
            }
            else if (shapeType == "org.sbgn.pd.02MacromolEPN")
            {
                return new MacromolEPN("LABEL", false, "", false);
            }
            else if (shapeType == "org.sbgn.pd.03NucleicAcidEPN")
            {
                return new NucleicAcidEPN("LABEL", false, "", false);
            }
            else if (shapeType == "org.sbgn.pd.04PerturbingEPN")
            {
                return new PerturbingEPN("LABEL", false);
            }
            else if (shapeType == "org.sbgn.pd.05SourceOrSink")
            {
                return new SourceOrSink();
            }
            else if (shapeType == "org.sbgn.pd.06ComplexEPN")
            {
                return new ComplexEPN("LABEL", false, "", false);
            }
            else if (shapeType == "org.sbgn.pd.ProcessNodeVertical")
            {
                return new ProcessNode(Qt::Vertical, ProcessNode::PROCESS);
            }
            else if (shapeType == "org.sbgn.pd.ProcessNodeHorizontal")
            {
                return new ProcessNode(Qt::Horizontal, ProcessNode::PROCESS);
            }
            else if (shapeType == "org.sbgn.pd.UnknownProcessNode")
            {
                return new ProcessNode(Qt::Vertical, ProcessNode::UNCERTAIN);
            }
            else if (shapeType == "org.sbgn.pd.OmittedProcessNode")
            {
                return new ProcessNode(Qt::Horizontal, ProcessNode::OMITTED);
            }
            else if (shapeType == "org.sbgn.pd.AssociationProcessNode")
            {
                return new ProcessNode(Qt::Horizontal, ProcessNode::ASSOCIATION);
            }
            else if (shapeType == "org.sbgn.pd.DissociationProcessNode")
            {
                return new ProcessNode(Qt::Horizontal, ProcessNode::DISSOCIATION);
            }
            else if (shapeType == "org.sbgn.pd.PhenotypeProcessNode")
            {
                return new PhenotypeProcessNode("LABEL", false);
            }
            return NULL;
        }
};

Q_EXPORT_PLUGIN2(shapes_sbgn, SBGNShapesPlugin)

// Because there is no header file, we need to load the MOC file here to 
// cause Qt to generate it for us.
#include "plugin.moc"

// vim: filetype=cpp ts=4 sw=4 et tw=0 wm=0 cindent
