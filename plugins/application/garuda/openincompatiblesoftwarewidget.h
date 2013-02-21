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
 * Author: Michael Wybrow <mjwybrow@users.sourceforge.net>
*/



#ifndef OPENINCOMPATIBLESOFTWAREWIDGET_H
#define OPENINCOMPATIBLESOFTWAREWIDGET_H

#include <QDockWidget>
//#include "appgarudaclient.h"
#include "garudaclient.h"

namespace dunnart {
class Canvas;
}

namespace Ui {
class OpenInCompatibleSoftwareWidget;
}

using dunnart::Canvas;

class OpenInCompatibleSoftwareWidget : public QDockWidget
{
        Q_OBJECT
        
    public:
        explicit OpenInCompatibleSoftwareWidget(dunnart::Canvas *canvas,
                QWidget *parent = 0);
        ~OpenInCompatibleSoftwareWidget();

        void setContentsForResponse(const QVariantMap& response);
        void setGarudaClient(GarudaClient *garudaClient);
    public slots:
        void changeCanvas(Canvas *canvas);
    private slots:
        void cellWasDoubleClicked(int row, int column);
    private:
        Ui::OpenInCompatibleSoftwareWidget *ui;
        GarudaClient *m_garuda_client;
        Canvas *m_canvas;
};

#endif // OPENINCOMPATIBLESOFTWAREWIDGET_H
// vim: filetype=cpp ts=4 sw=4 et tw=0 wm=0 cindent
