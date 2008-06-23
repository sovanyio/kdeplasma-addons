/***************************************************************************
 *   Copyright (C) 2007 Lukas Kropatschek <lukas.krop@kdemail.net>         *
 *   Copyright (C) 2008 Sebastian Kügler <sebas@kde.org>                   *
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

#include "notes.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsTextItem>

#include <KGlobalSettings>
#include <KConfigDialog>
#include <KConfigGroup>
#include <KTextEdit>

#include <Plasma/TextEdit>

Notes::Notes(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
    , m_notes_theme(this)
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    setAcceptDrops(true);
    setAcceptsHoverEvents(true);
    setBackgroundHints(Plasma::Applet::NoBackground);

    m_saveTimer.setSingleShot(true);
    connect(&m_saveTimer, SIGNAL(timeout()), this, SLOT(saveNote()));

    m_notes_theme.setImagePath("widgets/notes");
    m_notes_theme.setContainsMultipleImages(false);

    m_defaultText = i18n("Welcome to the Notes Plasmoid! Type your notes here...");
    m_textEdit = new Plasma::TextEdit();
    m_textEdit->setMinimumSize(QSize(0, 0));
    m_layout = new QGraphicsLinearLayout(this);
    m_textEdit->nativeWidget()->setFrameShape(QFrame::NoFrame);
    m_textEdit->nativeWidget()->setTextBackgroundColor(QColor(0,0,0,0));
    m_textEdit->nativeWidget()->viewport()->setAutoFillBackground(false);
    m_layout->addItem(m_textEdit);

    m_autoFont = false;

    resize(256, 256);
    updateTextGeometry();
}

Notes::~Notes()
{
    //FIXME is it really ok to save from here?
    saveNote();
    delete m_textEdit;
}

void Notes::init()
{
    KConfigGroup cg = config();

    QString text = cg.readEntry("autoSave", QString());
    if (! text.isEmpty()) {
        m_textEdit->nativeWidget()->setPlainText(text);
    } else {
        m_textEdit->nativeWidget()->setPlainText(m_defaultText);
    }
    m_font = cg.readEntry("font", KGlobalSettings::generalFont());
    m_autoFont = cg.readEntry("autoFont", true);
    m_autoFontPercent = cg.readEntry("autoFontPercent", 4);
    m_textColor = cg.readEntry("textcolor", QColor(Qt::black));
    m_textEdit->nativeWidget()->setTextColor(m_textColor);
    m_checkSpelling = cg.readEntry("checkSpelling", false);
    m_textEdit->nativeWidget()->setCheckSpellingEnabled(m_checkSpelling);
    updateTextGeometry();
    connect(m_textEdit, SIGNAL(textChanged()), this, SLOT(delayedSaveNote()));
}

void Notes::constraintsEvent(Plasma::Constraints constraints)
{
    //XXX why does everything break so horribly if I remove this line?
    setBackgroundHints(Plasma::Applet::NoBackground);
    if (constraints & Plasma::SizeConstraint) {
        updateTextGeometry();
    }
}

void Notes::updateTextGeometry()
{
    const qreal xpad = geometry().width() / 15;
    const qreal ypad = geometry().height() / 15;
    m_layout->setSpacing(xpad);
    m_layout->setContentsMargins(xpad, ypad, xpad, ypad);
    m_font.setPointSize(fontSize());
    m_textEdit->nativeWidget()->setFont(m_font);
}

int Notes::fontSize()
{
    if (m_autoFont) {
        int autosize = qRound(((geometry().width() + geometry().height())/2)*m_autoFontPercent/100);
        return qMax(KGlobalSettings::smallestReadableFont().pointSize(), autosize);
    } else {
        return m_font.pointSize();
    }
}

void Notes::delayedSaveNote()
{
    m_saveTimer.start(5000);
}

void Notes::saveNote()
{
    KConfigGroup cg = config();
    cg.writeEntry("autoSave", m_textEdit->nativeWidget()->toPlainText());
    //kDebug() << m_textEdit->nativeWidget()->toPlainText();
    emit configNeedsSaving();
}

void Notes::paintInterface(QPainter *p,
                           const QStyleOptionGraphicsItem *option,
                           const QRect &contentsRect)
{
    Q_UNUSED(option);

    m_notes_theme.resize(geometry().size());
    m_notes_theme.paint(p, contentsRect);
}

void Notes::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *widget = new QWidget(); // Do we need to delete this later?
    ui.setupUi(widget);
    parent->setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Apply );
    parent->addPage(widget, parent->windowTitle(), "notes");
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    ui.textColorButton->setColor(m_textColor);
    ui.textFontButton->setFont(m_textEdit->nativeWidget()->font());
    ui.autoFont->setChecked(m_autoFont);
    ui.autoFontPercent->setValue(m_autoFontPercent);
    ui.checkSpelling->setChecked(m_checkSpelling);
}

void Notes::configAccepted()
{
    KConfigGroup cg = config();
    bool changed = false;

    QFont newFont = ui.textFontButton->font();
    if (m_font != newFont) {
        changed = true;
        cg.writeEntry("font", newFont);
        m_font = newFont;
        m_font.setPointSize(fontSize());
        m_textEdit->nativeWidget()->setFont(newFont);
    }

    if (m_autoFont != ui.autoFont->isChecked()) {
        changed = true;
        m_autoFont = ui.autoFont->isChecked();
        cg.writeEntry("autoFont", m_autoFont);
        m_font.setPointSize(fontSize());
    }

    if (m_autoFontPercent != ui.autoFontPercent->value()) {
        changed = true;
        m_autoFontPercent = (ui.autoFontPercent->value());
        cg.writeEntry("autoFontPercent", m_autoFontPercent);
        if (m_autoFont) {
            m_font.setPointSize(fontSize());
        }
    }

    QColor newColor = ui.textColorButton->color();
    if (m_textColor != newColor) {
        changed = true;
        m_textColor = newColor;
        cg.writeEntry("textcolor", m_textColor);
        m_textEdit->nativeWidget()->setTextColor(m_textColor); // This doesn't seem to work, why?
    }

    bool spellCheck = ui.checkSpelling->isChecked();
    if (spellCheck != m_checkSpelling) {
        changed = true;
        m_checkSpelling = spellCheck;
        cg.writeEntry("checkSpelling", m_checkSpelling);
        m_textEdit->nativeWidget()->setCheckSpellingEnabled(m_checkSpelling);
    }

    if (changed) {
        updateTextGeometry();
        emit configNeedsSaving();
    }
}

#include "notes.moc"
