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

const BCompareConfig& BCompareConfig::get()
{
    static BCompareConfig m_cfg;
    return m_cfg;
}

static bool isFileExists(const QString &path)
{
    QFileInfo fInfo(path);
    return fInfo.exists() && fInfo.isFile();
}

static QStringList getListConfigDirToCheck()
{
    QStringList listDir;

    QDir homeDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    QDir configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

    QString configPath1 = configDir.absoluteFilePath(QLatin1String("bcompare"));
    QString configPath2 = homeDir.absoluteFilePath(QLatin1String(".config/bcompare"));

    listDir.append(homeDir.absoluteFilePath(QLatin1String(".beyondcompare")));
    listDir.append(configPath1);
    if (configPath1 != configPath2)
    {
        listDir.append(configPath2);
    }

    return listDir;
}

static QDir findMenuConfigPath(QString &menuIniPath)
{
    QStringList listDir = getListConfigDirToCheck();
    QString menuIniFileName(QLatin1String("menu.ini"));

    for (int i = 0; i < listDir.size(); ++i)
    {
        QDir cfgDir(listDir.at(i));
        if (cfgDir.exists())
        {
            QString cfgPath(cfgDir.absoluteFilePath(menuIniFileName));
            if (isFileExists(cfgPath))
            {
                menuIniPath = cfgPath;
                return cfgDir;
            }
        }
    }

    menuIniPath = QString();
    return QDir();
}

static bool getBoolSetting(const QSettings &settings, const char *key, bool defVal)
{
    bool r = defVal;
    QVariant v = settings.value(QLatin1String(key));
    if (v.isValid())
    {
        QString str(v.toString());

        if (   str.compare(QLatin1String("True"), Qt::CaseInsensitive) == 0
            || str == QLatin1String("1"))
        {
            r = true;
        }
        else if (!r || str == QLatin1String("0")
                 || str.compare(QLatin1String("False"), Qt::CaseInsensitive) == 0)
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

BCompareConfig::BCompareConfig() :
    m_menuEnabled(false), m_menuCompare(MENU_NONE), m_menuCompareUsing(MENU_NONE),
    m_menuMerge(MENU_NONE), m_menuSync(MENU_NONE), m_menuEdit(MENU_NONE)
{
    QString menuIniPath;
    QDir cfgDir = findMenuConfigPath(menuIniPath);

    if (!menuIniPath.isEmpty())
    {
        QSettings menuSettings(menuIniPath, QSettings::IniFormat);
        menuSettings.beginGroup(QLatin1String("ContextMenus"));

        m_menuEnabled = getBoolSetting(menuSettings, "Enabled", false);
        if (m_menuEnabled)
        {
            m_menuCompare      = getMenuTypeSetting(menuSettings, "Compare",      MENU_NONE);
            m_menuCompareUsing = getMenuTypeSetting(menuSettings, "CompareUsing", MENU_NONE);
            m_menuMerge        = getMenuTypeSetting(menuSettings, "Merge",        MENU_NONE);
            m_menuSync         = getMenuTypeSetting(menuSettings, "Sync",         MENU_NONE);
            m_menuEdit         = getMenuTypeSetting(menuSettings, "Edit",         MENU_NONE);

            m_listArchiveExt = menuSettings.value(QLatin1String("ArchiveMasks")).toStringList();
            m_listViewer     = menuSettings.value(QLatin1String("Viewers")).toStringList();

            QDir iconDir = menuSettings.value(QLatin1String("IconPath"),
                                              QLatin1String("/usr/share/pixmaps/")).toString();

            m_iconFull = QIcon(iconDir.absoluteFilePath(QLatin1String("bcomparefull32")));
            m_iconHalf = QIcon(iconDir.absoluteFilePath(QLatin1String("bcomparehalf32")));

            m_pathSaveLeftFilePath = cfgDir.absoluteFilePath(QLatin1String("left_file"));
            m_pathSaveCenterFilePath = cfgDir.absoluteFilePath(QLatin1String("center_file"));
        }

        menuSettings.endGroup();
    }
}

bool BCompareConfig::isFileArchive(const QString &pathFile) const
{
    for (int i = 0; i < m_listArchiveExt.size(); ++i)
    {
        if (pathFile.endsWith(m_listArchiveExt.at(i).midRef(1)))
        {
            return true;
        }
    }
    return false;
}

static QString readPathInFile(const QString &pathStorage)
{
    QString r;
    QFile f(pathStorage);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        r = QString::fromUtf8(f.readLine().trimmed());
        f.close();
    }

    return r;
}

static void savePathToFile(const QString &pathStorage, const QString &pathToSave)
{
    QFile f(pathStorage);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        f.write(pathToSave.toUtf8());
        f.write("\n");
        f.close();
    }
}

QString BCompareConfig::readPathLeftFile() const
{
    return readPathInFile(m_pathSaveLeftFilePath);
}

void BCompareConfig::savePathLeftFile(const QString &path) const
{
    savePathToFile(m_pathSaveLeftFilePath, path);
}

void BCompareConfig::forgetLeftFile() const
{
    QFile::remove(m_pathSaveLeftFilePath);
}

QString BCompareConfig::readPathCenterFile() const
{
    return readPathInFile(m_pathSaveCenterFilePath);
}

void BCompareConfig::savePathCenterFile(const QString &path) const
{
    savePathToFile(m_pathSaveCenterFilePath, path);
}

void BCompareConfig::forgetCenterFile() const
{
    QFile::remove(m_pathSaveCenterFilePath);
}
