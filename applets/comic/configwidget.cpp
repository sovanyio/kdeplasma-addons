/***************************************************************************
 *   Copyright (C) 2007 by Tobias Koenig <tokoe@kde.org>                   *
 *   Copyright (C) 2008 Matthias Fuchs <mat69@gmx.net>                     *
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

#include "configwidget.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QTimer>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSortFilterProxyModel>

#include <KLocale>
#include <KIcon>
#include <KNS/Engine>

class ComicModel : public QAbstractListModel
{
    public:
        ComicModel( const Plasma::DataEngine::Data &comics, QObject *parent = 0 )
            : QAbstractListModel( parent )
        {
            setComics( comics );
        }

        void setComics( const Plasma::DataEngine::Data &comics )
        {
            mComics = comics;
            reset();
        }

        virtual int rowCount( const QModelIndex &index = QModelIndex() ) const
        {
            if ( !index.isValid() )
                return mComics.count();
            else
                return 0;
        }

        virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const
        {
            if ( !index.isValid() || index.row() >= mComics.keys().count() )
                return QVariant();

            if ( role == Qt::DisplayRole ) {
                return mComics[ mComics.keys()[ index.row() ] ].toStringList()[ 0 ];
            } else if ( role == Qt::DecorationRole ) {
                return KIcon( mComics[ mComics.keys()[ index.row() ] ].toStringList()[ 1 ] );
            } else if ( role == Qt::UserRole ) {
                return mComics.keys()[ index.row() ];
            } else {
                return QVariant();
            }
        }

    private:
        Plasma::DataEngine::Data mComics;
};


ConfigWidget::ConfigWidget( Plasma::DataEngine *engine, QWidget *parent )
    : QWidget( parent ), mEngine( engine )
{
    ui.setupUi( this );
    connect( ui.pushButton_Size, SIGNAL( clicked() ), this, SIGNAL( maxSizeClicked() ) );
    connect( ui.pushButton_GHNS, SIGNAL( clicked() ), this, SLOT( getNewStuff() ) );

    mModel = new ComicModel( mEngine->query( "providers" ), this );
    mProxyModel = new QSortFilterProxyModel( this );
    mProxyModel->setSourceModel( mModel );
    mProxyModel->sort( 0, Qt::AscendingOrder );
    ui.comboBox_comic->setModel( mProxyModel );
    if ( mModel->rowCount() < 1 ) {
        QTimer::singleShot( 0, this, SLOT( getNewStuff() ) );
    }
}

ConfigWidget::~ConfigWidget()
{
}

void ConfigWidget::getNewStuff()
{
    KNS::Engine engine( this );
    if ( engine.init( "comic.knsrc" ) ) {
        KNS::Entry::List entries = engine.downloadDialogModal( this );
        if ( entries.size() > 0 ) {
            QString tmp = comicIdentifier();
            mModel->setComics( mEngine->query( "providers" ) );
            setComicIdentifier( tmp );
        }
    }
}

void ConfigWidget::setComicIdentifier( const QString &comic )
{
    for ( int i = 0; i < mProxyModel->rowCount(); ++i ) {
        const QModelIndex index = mProxyModel->index( i, 0 );
        if ( index.data( Qt::UserRole ).toString() == comic ) {
            ui.comboBox_comic->setCurrentIndex( i );
            break;
        }
    }
}

QString ConfigWidget::comicIdentifier() const
{
    const QModelIndex index = mProxyModel->index( ui.comboBox_comic->currentIndex(), 0 );
    return index.data( Qt::UserRole ).toString();
}

void ConfigWidget::setShowComicUrl( bool show )
{
    ui.checkBox_url->setChecked( show );
}

bool ConfigWidget::showComicUrl() const
{
    return ui.checkBox_url->isChecked();
}

void ConfigWidget::setShowComicAuthor( bool show )
{
    ui.checkBox_author->setChecked( show );
}

bool ConfigWidget::showComicAuthor() const
{
    return ui.checkBox_author->isChecked();
}

void ConfigWidget::setShowComicTitle( bool show )
{
    ui.checkBox_title->setChecked( show );
}

bool ConfigWidget::showComicTitle() const
{
    return ui.checkBox_title->isChecked();
}

void ConfigWidget::setShowComicIdentifier( bool show )
{
    ui.checkBox_identifier->setChecked( show );
}

bool ConfigWidget::showComicIdentifier() const
{
    return ui.checkBox_identifier->isChecked();
}

void ConfigWidget::setArrowsOnHover( bool arrows )
{
    return ui.checkBox_arrows->setChecked( arrows );
}

bool ConfigWidget::arrowsOnHover() const
{
    return ui.checkBox_arrows->isChecked();
}

#include "configwidget.moc"
