/***************************************************************************
 *   Copyright (C) 2007 by Tobias Koenig <tokoe@kde.org>                   *
 *   Copyright (C) 2008-2010 Matthias Fuchs <mat69@gmx.net>                *
 *   Copyright (C) 2015 Marco Martin <mart@kde.org>                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef COMICMODEL_H
#define COMICMODEL_H

#include <QtCore/QAbstractTableModel>

#include <Plasma/DataEngine>
#include <Plasma/DataEngineConsumer>

class ComicModel : public QAbstractTableModel, public Plasma::DataEngineConsumer
{
    Q_OBJECT

    public:
        ComicModel( Plasma::DataEngine *engine, const QString &source, const QStringList &usedComics, QObject *parent = nullptr );

        void setComics( const Plasma::DataEngine::Data &comics, const QStringList &usedComics );

        QHash<int, QByteArray> roleNames() const override;

        int rowCount( const QModelIndex &index = QModelIndex() ) const override;
        int columnCount( const QModelIndex &index = QModelIndex() ) const override;
        QVariant data( const QModelIndex &index, int role = Qt::CheckStateRole ) const override;
        Qt::ItemFlags flags( const QModelIndex &index ) const override;


    public Q_SLOTS:
        void dataUpdated( const QString &source, const Plasma::DataEngine::Data &data );

    private:
        Plasma::DataEngine::Data mComics;
        QStringList mUsedComics;
};

#endif
