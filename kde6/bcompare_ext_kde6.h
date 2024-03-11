/*
 * Copyright (c) 2024 Benjamin ROBIN
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

#ifndef BCOMPARE_EXT_KDE6_H
#define BCOMPARE_EXT_KDE6_H

#include <KAbstractFileItemActionPlugin>
#include <QList>
#include "bcompare_config.h"

class BCompareKde6 : public KAbstractFileItemActionPlugin
{
    Q_OBJECT
public:
    BCompareKde6(QObject *pParent, const QVariantList &args);
    virtual ~BCompareKde6();

    virtual QList<QAction*> actions(const KFileItemListProperties &fileItemInfos,
                                    QWidget *parentWidget) Q_DECL_OVERRIDE;

protected:
    /* Action callbacks */
    void cbSelectLeft();
    void cbSelectCenter();
    void cbEditFile();
    void cbCompare();
    void cbSync();
    void cbMerge();

    /* Utilities */
    bool isPathConsideredFolder(const QString &path) const;
    void clearSelections();

    /* Menu Items */
    QAction *createMenuItem(const QString &txt, const QString &hint,
                            const QIcon &icon, void (BCompareKde6::*callback)());

    struct CreateMenuCtx {
        bool isDir;
        int nbSelected;
        BCompareConfig::MenuTypes menuType;
    };

    QAction *createMenuItemSelectLeft(const CreateMenuCtx &ctx);
    QAction *createMenuItemSelectCenter(const CreateMenuCtx &ctx);
    QAction *createMenuItemEdit(const CreateMenuCtx &ctx);
    QAction *createMenuItemCompare(const CreateMenuCtx &ctx);
    QAction *createSubMenuItemCompareUsing(const QString &fileViewer, const CreateMenuCtx &ctx);
    QAction *createMenuItemCompareUsing(const CreateMenuCtx &ctx);
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

#endif // BCOMPARE_EXT_KDE6_H
