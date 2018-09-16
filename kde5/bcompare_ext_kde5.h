
#ifndef BCOMPARE_EXT_KDE5_H
#define BCOMPARE_EXT_KDE5_H

#include <KAbstractFileItemActionPlugin>
#include <QList>
#include "bcompare_config.h"

class BCompareKde5 : public KAbstractFileItemActionPlugin
{
    Q_OBJECT
public:
    BCompareKde5(QObject *pParent, const QVariantList &args);
    virtual ~BCompareKde5();

    virtual QList<QAction*> actions(const KFileItemListProperties &fileItemInfos,
                                    QWidget *parentWidget) Q_DECL_OVERRIDE;

protected slots:
    /* Action callbacks */
    void cbSelectLeft();
    void cbSelectCenter();
    void cbEditFile();
    void cbCompare();
    void cbSync();
    void cbMerge();

protected:
    /* Utilities */
    bool isPathConsideredFolder(const QString &path) const;
    void clearSelections();

    /* Menu Items */
    QAction *createMenuItem(const QString &txt, const QString &tip, bool isIconHalf,
                            void (BCompareKde5::*callback)());

    struct CreateMenuCtx {
        bool isDir;
        int nbSelected;
        BCompareConfig::MenuTypes menuType;
    };

    QAction *createMenuItemSelectLeft(const CreateMenuCtx &ctx);
    QAction *createMenuItemSelectCenter(const CreateMenuCtx &ctx);
    QAction *createMenuItemEdit(const CreateMenuCtx &ctx);
    QAction *createMenuItemCompare(const CreateMenuCtx &ctx);
    QAction *createMenuItemCompareUsing(const QString &fileViewer, const CreateMenuCtx &ctx);
    QAction *createMenuItemSync(const CreateMenuCtx &ctx);
    QAction *createMenuItemMerge(const CreateMenuCtx &ctx);

    void createMenus(QList<QAction*> &items, const CreateMenuCtx &ctx);

protected:
    /** A reference to the global configuration */
    const BCompareConfig &m_config;

    /** The path of the selected left file */
    QString m_pathLeftFile;

    /** The path of the selected center file */
    QString m_pathCenterFile;

    /** The path of the selected right file */
    QString m_pathRightFile;
};

#endif // BCOMPARE_EXT_KDE5_H
