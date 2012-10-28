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

#ifndef DSBSPECIES_H
#define DSBSPECIES_H

#include <QString>
#include <QList>

#include "libdsbpe/dsbreaction.h"

class Species;

namespace dunnart {

class DSBSpecies
{

public:
    DSBSpecies();
    DSBSpecies(Species *spec);
    QString getCompartmentName();
    void addReactionEntered(DSBReaction& reac);
    void addReactionExited(DSBReaction& reac);
    void addReactionModified(DSBReaction& reac);

private:
    Species *m_sbmlSpecies;
    QString m_name;
    QString m_id;
    QString m_compartmentName;
    QList<DSBReaction> m_reactionsEntered;
    QList<DSBReaction> m_reactionsExited;
    QList<DSBReaction> m_reactionsModified;
};

}

#endif // DSBSPECIES_H
