/*
 *   Copyright (C) 2007 Ivan Cukic <ivan.cukic+kde@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser/Library General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser/Library General Public License for more details
 *
 *   You should have received a copy of the GNU Lesser/Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "BasicWidget.h"
#include <KDebug>
#include <cmath>
#include "Global.h"
#include <lancelot/lancelot.h>
#include <QtGui/QPainter>
#include <plasma/paintutils.h>

#define WIDGET_PADDING 8

#define max(A, B) ((A) >= (B)) ? (A) : (B)

// macro for setting the left coordinate of items
// relative to the parent and with alignment
// taken into consideration
#define setLeft(itemRect, parentRect, alignment) \
    if ((parentRect).width() > (itemRect).width()) { \
        if ((alignment) & Qt::AlignHCenter) \
        (itemRect).moveLeft(WIDGET_PADDING + ((parentRect).width() - (itemRect).width()) / 2); \
        else if ((alignment) & Qt::AlignRight) \
        (itemRect).moveLeft(WIDGET_PADDING + (parentRect).width() - (itemRect).width()); \
    } else { \
        (itemRect).setWidth((parentRect).width()); \
        (itemRect).moveLeft(WIDGET_PADDING); \
    };

namespace Lancelot
{

class BasicWidget::Private {
    public:
    Private(BasicWidget * parent, QString title = QString(), QString description = QString())
      : icon(QIcon()), iconSize(32, 32),
        innerOrientation(Qt::Horizontal), alignment(Qt::AlignCenter),
        title(title), description(description)
    {
        init(parent);
    }

    Private(BasicWidget * parent, QIcon icon, QString title, QString description)
      : icon(icon), iconSize(32, 32),
        innerOrientation(Qt::Horizontal), alignment(Qt::AlignCenter),
        title(title), description(description)
    {
        init(parent);
    }

    Private(BasicWidget * parent, const Plasma::Svg & icon, QString title, QString description)
      : icon(QIcon()), iconSize(32, 32),
        innerOrientation(Qt::Horizontal), alignment(Qt::AlignCenter),
        title(title), description(description)
    {
        iconInSvg.setImagePath(icon.imagePath());
        init(parent);
    }

    void init(BasicWidget * parent)
    {
        parent->setAcceptsHoverEvents(true);
        parent->resize(140, 38);
        parent->setGroupByName("BasicWidget");
    }

    int shortcutPosition(const QString & text)
    {
        int index = 0;
        while ((index = title.indexOf('&', index)) != -1) {
            if (index == title.size() - 1) {
                return -1;
            }

            if (title.at(index + 1) != '&') {
                return index + 1;
            }

            index += 2;
        }
        return -1;
    }

    QIcon icon;
    Plasma::Svg iconInSvg;
    QSize iconSize;
    Qt::Orientation innerOrientation;

    Qt::Alignment alignment;

    QString title;
    QString description;
};

BasicWidget::BasicWidget(QGraphicsItem * parent)
  : Widget(parent),
    d(new Private(this))
{
    L_WIDGET_SET_INITIALIZED;
}

BasicWidget::BasicWidget(QString title, QString description,
        QGraphicsItem * parent)
  : Widget(parent),
    d(new Private(this, title, description))
{
    L_WIDGET_SET_INITIALIZED;
}

BasicWidget::BasicWidget(QIcon icon, QString title,
        QString description, QGraphicsItem * parent)
  : Widget(parent),
    d(new Private(this, icon, title, description))
{
    L_WIDGET_SET_INITIALIZED;
}

BasicWidget::BasicWidget(const Plasma::Svg & icon, QString title,
        QString description, QGraphicsItem * parent)
  : Widget(parent),
    d(new Private(this, icon, title, description))
{
    L_WIDGET_SET_INITIALIZED;
}

BasicWidget::~BasicWidget()
{
    L_WIDGET_UNSET_INITIALIZED;
    delete d;
}

void BasicWidget::paint(QPainter * painter,
        const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    paintBackground(painter);
    paintForeground(painter);
}

void BasicWidget::paintForeground(QPainter * painter)
{
    if (!L_WIDGET_IS_INITIALIZED) return;

    QPainter * _painter = painter;

    QPixmap foreground(size().toSize().width(), size().toSize().height());
    foreground.fill(Qt::transparent);

    // Replacing painter with QImage painter
    QPainter fpainter(&foreground);
    painter = &fpainter;

    QColor fgColor;
    if (!isEnabled()) {
        fgColor = group()->foregroundColor()->disabled;
    } else if (isHovered()) {
        fgColor = group()->foregroundColor()->active;
    } else {
        fgColor = group()->foregroundColor()->normal;
    }
    painter->setPen(QPen(fgColor));

    QFont titleFont = painter->font();
    QFont descriptionFont = painter->font();
    descriptionFont.setPointSize(descriptionFont.pointSize() - 2);

    QRectF widgetRect       = QRectF(0, 0, size().width() - 2 * WIDGET_PADDING, size().height() - 2 * WIDGET_PADDING);
    QRectF iconRect;

    if (!d->icon.isNull() || d->iconInSvg.isValid()) {
        iconRect = QRectF(QPointF(), d->iconSize);
        if (iconRect.width() > geometry().width()) {
            iconRect.setWidth(geometry().width());
        }
        if (iconRect.height() > geometry().height()) {
            iconRect.setHeight(geometry().height());
        }
    }

    // painter->setFont(titleFont)); // NOT NEEDED
    QRectF titleRect        = painter->boundingRect(widgetRect,
            Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, d->title);

    painter->setFont(descriptionFont);
    QRectF descriptionRect  = painter->boundingRect(widgetRect,
            Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, d->description);

    if (d->innerOrientation == Qt::Vertical || (d->title.isEmpty() && d->description.isEmpty())) {

        // Modified setLeft macro for icon since we can not cut it if it's larger than needed
        // setLeft(iconRect, widgetRect, d->alignment);
        if (d->alignment & Qt::AlignHCenter) {
            iconRect.moveLeft(WIDGET_PADDING + (widgetRect.width() - iconRect.width()) / 2);
        } else if (d->alignment & Qt::AlignRight) {
            iconRect.moveLeft(WIDGET_PADDING + widgetRect.width() - iconRect.width());
        }

        setLeft(titleRect, widgetRect, d->alignment);
        setLeft(descriptionRect, widgetRect, d->alignment);

        float top = WIDGET_PADDING, height =
            iconRect.height() + titleRect.height() + descriptionRect.height();

        if ((!d->icon.isNull() || d->iconInSvg.isValid()) && !(d->title.isEmpty() && d->description.isEmpty()))
            height += WIDGET_PADDING;

        if (d->alignment & Qt::AlignVCenter)
            top = (widgetRect.height() - height) / 2 + WIDGET_PADDING;
        if (d->alignment & Qt::AlignBottom)
            top = widgetRect.height() - height + WIDGET_PADDING;

        if (!d->icon.isNull() || d->iconInSvg.isValid()) { // using real painter...
            iconRect.moveTop(top);
            QRect rect(QPoint(lround(iconRect.left()), lround(iconRect.top())), d->iconSize);
            if (!d->icon.isNull()) {
                d->icon.paint(_painter, rect);
            } else {
                d->iconInSvg.resize(rect.size());
                d->iconInSvg.paint(_painter, rect.left(), rect.top(), isHovered()?"active":"inactive");
            }
            top += d->iconSize.height() + WIDGET_PADDING;
        }

        if (!d->title.isEmpty()) {
            titleRect.moveTop(top);
            painter->setFont(titleFont);
            // painter->drawText(titleRect,
            //        Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine | Qt::ElideRight, d->title);
            drawText(painter, titleRect,
                   Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine | Qt::ElideRight, d->title, true);
            top += titleRect.height();
        }

        if (!d->description.isEmpty()) {
            descriptionRect.moveTop(top);

            painter->setFont(descriptionFont);
            // painter->drawText(descriptionRect,
            //         Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine | Qt::ElideRight, d->description);
            drawText(painter, descriptionRect,
                    Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine | Qt::ElideRight, d->description, false);
        }
    } else {
        float /*left = WIDGET_PADDING,*/ width =
            iconRect.width() + fmaxf(titleRect.width(), descriptionRect.width()) +
            WIDGET_PADDING;

        if (d->alignment & Qt::AlignTop) {
            iconRect.moveTop(WIDGET_PADDING);
            titleRect.moveTop(WIDGET_PADDING);
            descriptionRect.moveTop(titleRect.bottom());
        } else if (d->alignment & (Qt::AlignVCenter | Qt::AlignBottom)) {
            iconRect.moveTop(WIDGET_PADDING +
                    ((d->alignment & Qt::AlignVCenter) ? 0.5 : 1) * (widgetRect.height() - iconRect.height()));
            titleRect.moveTop(WIDGET_PADDING +
                    ((d->alignment & Qt::AlignVCenter) ? 0.5 : 1) * (widgetRect.height() -
                        ((d->description.isEmpty())?0:descriptionRect.height()) - titleRect.height()));
            descriptionRect.moveTop(titleRect.bottom());
        }

        if ((widgetRect.width() < width) || (d->alignment & Qt::AlignLeft)) {
            iconRect.moveLeft(WIDGET_PADDING);
            titleRect.setWidth(widgetRect.width() - ((!d->icon.isNull() || d->iconInSvg.isValid()) ? iconRect.width() + WIDGET_PADDING : 0));
            descriptionRect.setWidth(titleRect.width());
        } else if (d->alignment & Qt::AlignHCenter) {
            iconRect.moveLeft(WIDGET_PADDING + (widgetRect.width() - width) / 2);
        } else
            iconRect.moveLeft(WIDGET_PADDING + (widgetRect.width() - width));

        titleRect.moveLeft(WIDGET_PADDING + iconRect.right());
        descriptionRect.moveLeft(WIDGET_PADDING + iconRect.right());

        if (!d->icon.isNull() || d->iconInSvg.isValid()) {  // using real painter...
            QRect rect(QPoint(lround(iconRect.left()), lround(iconRect.top())), d->iconSize);
            if (!d->icon.isNull()) {
                QIcon::Mode mode;
                if (!isEnabled()) {
                    mode = QIcon::Disabled;
                } else if (isHovered()) {
                    mode = QIcon::Active;
                } else {
                    mode = QIcon::Normal;
                }

                d->icon.paint(_painter, rect, Qt::AlignCenter, mode, QIcon::Off);
            } else {
                d->iconInSvg.resize(d->iconSize);
                d->iconInSvg.paint(_painter, rect.left(), rect.top(), isHovered()?"active":"inactive"); //TODO: add disabled state
            }
        }

        if (!d->title.isEmpty()) {
            painter->setFont(titleFont);
            // painter->drawText(titleRect,
            //         Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, d->title);
            drawText(painter, titleRect,
                    Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, d->title, true);
        }

        if (!d->description.isEmpty()) {
            if (!isHovered()) {
                QPen pen = painter->pen();
                QColor clr = painter->pen().color();
                clr.setAlphaF(0.3);
                painter->setPen(QPen(clr));
            }
            painter->setFont(descriptionFont);
            // painter->drawText(descriptionRect,
            //         Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, d->description);
            drawText(painter, descriptionRect,
                    Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, d->description, false);
        }
    }

    QLinearGradient gradient = QLinearGradient(
            QPointF(size().width() - WIDGET_PADDING - 20, 0),
            QPointF(size().width() - WIDGET_PADDING, 0)
            );
    gradient.setColorAt(1, Qt::transparent);
    gradient.setColorAt(0, Qt::black);
    painter->setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter->fillRect(
            0, 0, (int)size().width(), (int)size().height(),
            gradient);
    //

    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    _painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    _painter->drawPixmap(0, 0, foreground);
}

void BasicWidget::setIconSize(QSize size)
{
    d->iconSize = size; update();
}

QSize BasicWidget::iconSize() const
{
    return d->iconSize;
}

void BasicWidget::setIcon(QIcon icon)
{
    d->icon = icon; update();
}

QIcon BasicWidget::icon() const
{
    return d->icon;
}

void BasicWidget::setIconInSvg(const Plasma::Svg & icon)
{
    d->iconInSvg.setImagePath(icon.imagePath());
    update();
}

Plasma::Svg & BasicWidget::iconInSvg() const
{
    return d->iconInSvg;
}

void BasicWidget::setTitle(const QString & title)
{
    d->title = title;

    int pos = d->shortcutPosition(title);
    if (pos > -1) {
        setShortcutKey(title.at(pos));
    }

    update();
}

void BasicWidget::setShortcutKey(const QString & key)
{

}

QString BasicWidget::title() const
{
    return d->title;
}

void BasicWidget::setDescription(const QString & description)
{
    d->description = description;
    update();
}

QString BasicWidget::description() const
{
    return d->description;
}

void BasicWidget::setInnerOrientation(Qt::Orientation position) {
    d->innerOrientation = position;
    update();
}

Qt::Orientation BasicWidget::innerOrientation() const
{
    return d->innerOrientation;
}

void BasicWidget::setAlignment(Qt::Alignment alignment)
{
    d->alignment = alignment; update();
}

Qt::Alignment BasicWidget::alignment() const
{
    return d->alignment;
}

QSizeF BasicWidget::sizeHint(Qt::SizeHint which, const QSizeF & constraint) const
{
    QSizeF result = QSizeF();

    if (!L_WIDGET_IS_INITIALIZED) {
        return result;
    }

    switch (which) {
        case Qt::MinimumSize:
            result = d->iconSize;
            break;
        case Qt::MaximumSize:
            result = MAX_WIDGET_SIZE;
            break;
        default:
            // Do we need a more precise sizeHint?
            result = d->iconSize + QSizeF(2 * WIDGET_PADDING, 2 * WIDGET_PADDING);
            if (d->innerOrientation == Qt::Horizontal) {
                result.scale(5.0, 1.0, Qt::IgnoreAspectRatio);
            } else {
                result.scale(2.0, 2.0, Qt::IgnoreAspectRatio);
            }
    }
    if (constraint != QSizeF(-1, -1)) {
        result = result.boundedTo(constraint);
    }
    return result;
}

void BasicWidget::drawText(QPainter * painter, const QRectF & rectangle, int flags, const QString & txt, bool shortcutEnabled)
{
    if (txt.isEmpty()) {
        return;
    }

    QString text = txt;
    int shortcutPosition = d->shortcutPosition(text);
    if (shortcutPosition > -1 && shortcutEnabled) {
        text = text.remove(shortcutPosition - 1, 1);
    } else {
        shortcutEnabled = false;
    }

    static const int radius = 2;
    if (group()->hasProperty(BlurTextShadow)) {
        QColor textColor = painter->pen().color();
        QColor shadowColor;
        if (textColor.valueF() * textColor.alphaF() > 0.4) {
            shadowColor = Qt::black;
        } else {
            shadowColor = Qt::white;
        }

        QPixmap result = Plasma::PaintUtils::shadowText(
                text, textColor, shadowColor,
                QPoint(0, 0), radius);

        if (group()->hasProperty(TextColorBackground)) {
            QColor bgColor;
            if (!isEnabled()) {
                bgColor = group()->backgroundColor()->disabled;
            } else if (isHovered()) {
                bgColor = group()->backgroundColor()->active;
            } else {
                bgColor = group()->backgroundColor()->normal;
            }
            painter->setRenderHint(QPainter::Antialiasing);
            QRectF frect = QRectF(rectangle.topLeft(), result.size());
            painter->fillPath(
                    Plasma::PaintUtils::roundedRectangle(
                        frect, 2 * radius), QBrush(bgColor)
                    );
        }

        painter->drawPixmap(rectangle.topLeft(), result);

        if (shortcutEnabled) {
            int width = painter->boundingRect(
                    rectangle,
                    Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine,
                    text.left(shortcutPosition - 1)).width();
            QPixmap result = Plasma::PaintUtils::shadowText(
                    "_", textColor, shadowColor,
                    QPoint(width, 0), radius);
            painter->drawPixmap(rectangle.topLeft(), result);
            // painter->drawText(
            //         QRectF(rectangle.topLeft() + QPoint(width, 0), rectangle.size()),
            //         QString('_'));
        }
    } else {
        if (group()->hasProperty(TextColorBackground)) {
            QColor bgColor;
            if (!isEnabled()) {
                bgColor = group()->backgroundColor()->disabled;
            } else if (isHovered()) {
                bgColor = group()->backgroundColor()->active;
            } else {
                bgColor = group()->backgroundColor()->normal;
            }
            painter->setRenderHint(QPainter::Antialiasing);
            QRectF frect = painter->boundingRect(rectangle,
                    Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, text);
            frect.adjust(- radius, - radius, radius, radius);
            painter->fillPath(
                    Plasma::PaintUtils::roundedRectangle(
                     frect, 2*radius), QBrush(bgColor)
                    );
        }
        painter->drawText(rectangle,
                Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, text);

        if (shortcutEnabled) {
            int width = painter->boundingRect(
                    rectangle,
                    Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine,
                    text.left(shortcutPosition - 1)).width();
            painter->drawText(
                    QRectF(rectangle.topLeft() + QPoint(width, 0), rectangle.size()),
                    QString('_'));
        }
    }


}

} // namespace Lancelot

#include "BasicWidget.moc"

