/*
 *   Copyright 2010 by Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H

#include <QtGui/QGraphicsWidget>

#include <Plasma/Plasma>

class QGraphicsLinearLayout;

namespace Plasma {
    class ToolButton;
    class Animation;
}

class GridManager : public QGraphicsWidget
{
    Q_OBJECT
    public:
        GridManager(QGraphicsItem *parent);
        ~GridManager();

        void showAnimated();
        void hideAnimated();

        void checkLocation(const QPointF &pos);
        void setLocation(Plasma::Location);
        Plasma::Location location() const;

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget = 0);

    signals:
        void newClicked();
        void deleteClicked();

    private slots:
        void animationFinished();

    private:
        void place();

        QGraphicsLinearLayout *m_layout;
        Plasma::Location m_location;
        Plasma::ToolButton *m_newRowCol;
        Plasma::ToolButton *m_newRowCol2;
        Plasma::ToolButton *m_delRowCol;
        Plasma::ToolButton *m_delRowCol2;
        Plasma::Animation *m_fadeAnim;
        bool m_replace;
};

#endif
