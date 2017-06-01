/*
 *   Copyright (C) 2007 Tobias Koenig <tokoe@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
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

#ifndef CACHEDPROVIDER_H
#define CACHEDPROVIDER_H

#include "comicprovider.h"

#include <QtCore/QHash>

/**
 * This class provides comics from the local cache.
 */
class CachedProvider : public ComicProvider
{
    Q_OBJECT

    public:
        /**
         * Creates a new cached provider.
         *
         * @param identifier The identifier of the cached comic.
         * @param parent The parent object.
         */
        explicit CachedProvider(QObject *parent, const QVariantList &args = QVariantList());

        /**
         * Destroys the cached provider.
         */
        ~CachedProvider();

        /**
         * Returns the identifier type.
         *
         * Is always StringIdentifier here.
         */
        IdentifierType identifierType() const Q_DECL_OVERRIDE;

        /**
         * Returns the type of identifier that is used by this
         * comic provider.
         */
        QString suffixType() const Q_DECL_OVERRIDE;

        /**
         * Returns the requested image.
         *
         * Note: This method returns only a valid image after the
         *       finished() signal has been emitted.
         */
        QImage image() const Q_DECL_OVERRIDE;

        /**
         * Returns the identifier of the comic request (name + date).
         */
        QString identifier() const Q_DECL_OVERRIDE;

        /**
         * Returns the identifier suffix of the next comic.
         */
        QString nextIdentifier() const Q_DECL_OVERRIDE;

        /**
         * Returns the identifier suffix of the previous comic.
         */
        QString previousIdentifier() const Q_DECL_OVERRIDE;

        /**
         * Returns the identifier of the first strip.
         */
        QString firstStripIdentifier() const Q_DECL_OVERRIDE;

        /**
         * Returns the identifier of the last cached strip.
         */
        QString lastCachedStripIdentifier() const;

        /**
         * Returns the title of the strip.
         */
        QString stripTitle() const Q_DECL_OVERRIDE;

        /**
         * Returns the author of the comic.
         */
        QString comicAuthor() const Q_DECL_OVERRIDE;

        /**
         * Returns additionalText of the comic.
         */
        QString additionalText() const Q_DECL_OVERRIDE;

        /**
         * Returns the name for the comic
         */
        QString name() const Q_DECL_OVERRIDE;

        /**
        * Returns wether the comic is leftToRight or not
        */
        bool isLeftToRight() const Q_DECL_OVERRIDE;

        /**
        * Returns wether the comic is topToBottom or not
        */
        bool isTopToBottom() const Q_DECL_OVERRIDE;

        /**
         * Returns whether a comic with the given @p identifier is cached.
         */
        static bool isCached(const QString &identifier);

        /**
         * Map of keys and values to store in the config file for an individual identifier
         */
        typedef QHash<QString, QString> Settings;

        /**
         * Stores the given @p comic with the given @p identifier in the cache.
         */
        static bool storeInCache(const QString &identifier, const QImage &comic, const Settings &info = Settings());

        /**
         * Returns the website of the comic.
         */
        QUrl websiteUrl() const Q_DECL_OVERRIDE;

        QUrl imageUrl() const Q_DECL_OVERRIDE;

        /**
         * Returns the shop website of the comic.
         */
        QUrl shopUrl() const Q_DECL_OVERRIDE;

        /**
          * Returns the maximum number of cached strips per comic, -1 means that there is no limit
          * @note defaulte is -1
          */
        static int maxComicLimit();

        /**
          * Sets the maximum number of cached strips per comic, -1 means that there is no limit
          */
        static void setMaxComicLimit(int limit);

    private Q_SLOTS:
        void triggerFinished();

    private:
        static const int CACHE_DEFAULT;
};

#endif
