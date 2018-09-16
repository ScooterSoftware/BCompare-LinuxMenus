
#ifndef BCOMPARE_CONFIG_H
#define BCOMPARE_CONFIG_H

#include <QStringList>
#include <QString>
#include <QIcon>

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

    inline QStringList listViewer() const
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

    inline const QIcon& menuIconHalf() const
    {
        return m_iconHalf;
    }

    inline const QIcon& menuIconFull() const
    {
        return m_iconFull;
    }

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

    /** The icon to display in the menu in order to select the left file */
    QIcon m_iconHalf;

    /** The icon to display in the menu */
    QIcon m_iconFull;

    /** The storage path to save the left file path */
    QString m_pathSaveLeftFilePath;

    /** The storage path to save the center file path */
    QString m_pathSaveCenterFilePath;
};

#endif // BCOMPARE_CONFIG_H
