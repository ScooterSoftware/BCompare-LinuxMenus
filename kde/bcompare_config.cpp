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

#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QFile>
#include "bcompare_config.h"


class BCompareIconCache
{
public:
    const QIcon& iconEdit()
    {
        if (m_iconEdit.isNull())
        {
            m_iconEdit.addFile(QLatin1String(":/bcompare/icon/Edit32.png"));
        }
        return m_iconEdit;
    }

    const QIcon& iconFull()
    {
        if (m_iconFull.isNull())
        {
            m_iconFull.addFile(QLatin1String(":/bcompare/icon/Full32.png"));
        }
        return m_iconFull;
    }

    const QIcon& iconHalf()
    {
        if (m_iconHalf.isNull())
        {
            m_iconHalf.addFile(QLatin1String(":/bcompare/icon/Half32.png"));
        }
        return m_iconHalf;
    }

    const QIcon& iconMerge()
    {
        if (m_iconMerge.isNull())
        {
            m_iconMerge.addFile(QLatin1String(":/bcompare/icon/Merge32.png"));
        }
        return m_iconMerge;
    }

    const QIcon& iconSync()
    {
        if (m_iconSync.isNull())
        {
            m_iconSync.addFile(QLatin1String(":/bcompare/icon/Sync32.png"));
        }
        return m_iconSync;
    }

private:
    QIcon m_iconEdit;
    QIcon m_iconFull;
    QIcon m_iconHalf;
    QIcon m_iconMerge;
    QIcon m_iconSync;
};

/****************************************************/

BCompareConfig& BCompareConfig::get()
{
    static BCompareConfig m_cfg;
    return m_cfg;
}

static bool getBoolSetting(const QSettings &settings, const char *key, bool defVal)
{
    bool r = defVal;
    QVariant v = settings.value(QLatin1String(key));
    if (v.isValid())
    {
        QString str(v.toString());

        if (   (str.compare(QLatin1String("True"), Qt::CaseInsensitive) == 0)
            || (str == QLatin1String("1")) )
        {
            r = true;
        }
        else if (   (str.compare(QLatin1String("False"), Qt::CaseInsensitive) == 0)
                 || (str == QLatin1String("0")) )
        {
            r = false;
        }
    }

    return r;
}

static BCompareConfig::MenuTypes getMenuTypeSetting(const QSettings &settings, const char *key,
                                                    BCompareConfig::MenuTypes defVal)
{
    BCompareConfig::MenuTypes r = defVal;
    QVariant v = settings.value(QLatin1String(key));
    if (v.isValid())
    {
        int i = v.toInt();
        switch (i)
        {
            case BCompareConfig::MENU_NONE:
            case BCompareConfig::MENU_MAIN:
            case BCompareConfig::MENU_SUBMENU:
                r = static_cast<BCompareConfig::MenuTypes>(i);
                break;

            default:
                break;
        }
    }
    return r;
}

static QStringList archiveMaskToExt(const QStringList &listMask)
{
    QStringList listExt;

    for (const QString &mask : listMask)
    {
        if (mask.startsWith(QLatin1Char('*')))
        {
            listExt.append(mask.mid(1));
        }
    }

    return listExt;
}

static QDateTime getFileModifiedTime(const QString &p)
{
    QFileInfo menuIniInfo(p);
    menuIniInfo.refresh();

    return menuIniInfo.lastModified(
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        QTimeZone::UTC
#endif
    );
}

BCompareConfig::BCompareConfig() :
    m_menuEnabled(false), m_menuCompare(MENU_NONE), m_menuCompareUsing(MENU_NONE),
    m_menuMerge(MENU_NONE), m_menuSync(MENU_NONE), m_menuEdit(MENU_NONE),
    m_icons(new BCompareIconCache())
{
    findMenuConfigPath();
}

/*
 * Beyond Compare 4 looks for config in these directories in this order:
 *  - $HOME/.beyondcompare4
 *  - $HOME/.beyondcompare
 *  - $XDG_CONFIG_HOME/bcompare4
 *  - $XDG_CONFIG_HOME/bcompare (default)
 *
 * Beyond Compare 5 looks for config in these directories in this order:
 *  - $HOME/.beyondcompare5
 *  - $XDG_CONFIG_HOME/bcompare5 (default)
 *
 * There is no easy way to check which version of beyond compare is used/installed.
 * So first check for version 5, then check for version 4.
 */
void BCompareConfig::findMenuConfigPath()
{
    QStringList listDir;
    QDir homeDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    QDir configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

    listDir.append(homeDir.absoluteFilePath(QLatin1String(".beyondcompare5")));
    listDir.append(configDir.absoluteFilePath(QLatin1String("bcompare5")));

    listDir.append(homeDir.absoluteFilePath(QLatin1String(".beyondcompare4")));
    listDir.append(homeDir.absoluteFilePath(QLatin1String(".beyondcompare")));
    listDir.append(configDir.absoluteFilePath(QLatin1String("bcompare4")));
    listDir.append(configDir.absoluteFilePath(QLatin1String("bcompare")));

    QString menuIniFileName(QLatin1String("menu.ini"));
    QMutexLocker lock(&m_mutex);

    for (const QString &d : listDir)
    {
        QDir cfgDir(d);
        if (cfgDir.exists())
        {
            QString cfgPath(cfgDir.absoluteFilePath(menuIniFileName));
            if (QFileInfo::exists(cfgPath))
            {
                m_menuIniPath = cfgPath;
                m_leftFileSavePath = cfgDir.absoluteFilePath(QLatin1String("left_file"));
                m_centerFileSavePath = cfgDir.absoluteFilePath(QLatin1String("center_file"));
            }

            break;
        }
    }
}

void BCompareConfig::reloadMenuConfig()
{
    QMutexLocker lock(&m_mutex);

    if (m_menuIniPath.isEmpty())
    {
        return;
    }

    QDateTime menuIniModTime = getFileModifiedTime(m_menuIniPath);
    if (menuIniModTime == m_menuIniDateTime)
    {
        return;
    }

    m_menuIniDateTime = menuIniModTime;

    QSettings menuSettings(m_menuIniPath, QSettings::IniFormat);
    menuSettings.beginGroup(QLatin1String("ContextMenus"));

    m_menuEnabled = getBoolSetting(menuSettings, "Enabled", false);
    if (m_menuEnabled)
    {
        m_menuCompare      = getMenuTypeSetting(menuSettings, "Compare",      MENU_NONE);
        m_menuCompareUsing = getMenuTypeSetting(menuSettings, "CompareUsing", MENU_NONE);
        m_menuMerge        = getMenuTypeSetting(menuSettings, "Merge",        MENU_NONE);
        m_menuSync         = getMenuTypeSetting(menuSettings, "Sync",         MENU_NONE);
        m_menuEdit         = getMenuTypeSetting(menuSettings, "Edit",         MENU_NONE);

        m_listArchiveExt = archiveMaskToExt(
            menuSettings.value(QLatin1String("ArchiveMasks")).toStringList());

        m_listViewer = menuSettings.value(QLatin1String("Viewers")).toStringList();
    }

    menuSettings.endGroup();
}

bool BCompareConfig::isFileArchive(const QString &pathFile) const
{
    for (const QString &ext : m_listArchiveExt)
    {
        if (pathFile.endsWith(ext))
        {
            return true;
        }
    }
    return false;
}

static QString readPathInFile(const QString &pathStorage)
{
    QString r;

    if (!pathStorage.isEmpty())
    {
        QFile f(pathStorage);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            r = QString::fromUtf8(f.readLine().trimmed());
            f.close();
        }
    }

    return r;
}

static void savePathToFile(const QString &pathStorage, const QString &pathToSave)
{
    if (!pathStorage.isEmpty())
    {
        QFile f(pathStorage);
        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            f.write(pathToSave.toUtf8());
            f.write("\n");
            f.close();
        }
    }
}

QString BCompareConfig::readPathLeftFile() const
{
    return readPathInFile(m_leftFileSavePath);
}

void BCompareConfig::savePathLeftFile(const QString &path) const
{
    savePathToFile(m_leftFileSavePath, path);
}

void BCompareConfig::forgetLeftFile() const
{
    QFile::remove(m_leftFileSavePath);
}

QString BCompareConfig::readPathCenterFile() const
{
    return readPathInFile(m_centerFileSavePath);
}

void BCompareConfig::savePathCenterFile(const QString &path) const
{
    savePathToFile(m_centerFileSavePath, path);
}

void BCompareConfig::forgetCenterFile() const
{
    QFile::remove(m_centerFileSavePath);
}

const QIcon& BCompareConfig::iconEdit() const
{
    return m_icons->iconEdit();
}

const QIcon& BCompareConfig::iconFull() const
{
    return m_icons->iconFull();
}

const QIcon& BCompareConfig::iconHalf() const
{
    return m_icons->iconHalf();
}

const QIcon& BCompareConfig::iconMerge() const
{
    return m_icons->iconMerge();
}

const QIcon& BCompareConfig::iconSync() const
{
    return m_icons->iconSync();
}
