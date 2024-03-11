
#ifndef BCOMPARE_CONFIG_H
/*
 * Copyright (c) 2018 Benjamin ROBIN
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define BCOMPARE_CONFIG_H

#include <QStringList>
#include <QString>
#include <QIcon>

class BCompareIconCache;

class BCompareConfig
{
public:
    /** Get a reference to the global configuration. In order to reload the configuration,
     * Dolphin needs to be restarted */
    static const BCompareConfig& get();

    typedef enum {
        MENU_NONE = 0,
        MENU_MAIN = 1,
        MENU_SUBMENU = 2
    } MenuTypes;

    bool isFileArchive(const QString &pathFile) const;

    QString readPathLeftFile() const;
    void savePathLeftFile(const QString &path) const;
    void forgetLeftFile() const;

    QString readPathCenterFile() const;
    void savePathCenterFile(const QString &path) const;
    void forgetCenterFile() const;

    inline bool menuEnabled() const
    {
        return m_menuEnabled;
    }

    inline const QStringList& listViewer() const
    {
        return m_listViewer;
    }

    inline MenuTypes menuCompare() const
    {
        return m_menuCompare;
    }

    inline MenuTypes menuCompareUsing() const
    {
        return m_menuCompareUsing;
    }

    inline MenuTypes menuMerge() const
    {
        return m_menuMerge;
    }

    inline MenuTypes menuSync() const
    {
        return m_menuSync;
    }

    inline MenuTypes menuEdit() const
    {
        return m_menuEdit;
    }

    const QIcon& iconEdit() const;
    const QIcon& iconFull() const;
    const QIcon& iconHalf() const;
    const QIcon& iconMerge() const;
    const QIcon& iconSync() const;

private:
    BCompareConfig();

    /** Indicates if beyond compare is integrated in the file manager context menus */
    bool m_menuEnabled;

    /** The list of archive extensions */
    QStringList m_listArchiveExt;

    /** The beyond compare viewer types */
    QStringList m_listViewer;

    /** Indicates the location of the "Compare" menu */
    MenuTypes m_menuCompare;

    /** Indicates the location of the "Compare Using" menu */
    MenuTypes m_menuCompareUsing;

    /** Indicates the location of the "Merge" menu */
    MenuTypes m_menuMerge;

    /** Indicates the location of the "Sync" menu */
    MenuTypes m_menuSync;

    /** Indicates the location of the "Edit" menu */
    MenuTypes m_menuEdit;

    /** The storage path to save the left file path */
    QString m_pathSaveLeftFilePath;

    /** The storage path to save the center file path */
    QString m_pathSaveCenterFilePath;

    /** The icons cache */
    BCompareIconCache *m_icons;
};

#endif // BCOMPARE_CONFIG_H
