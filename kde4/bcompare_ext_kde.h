/*
 *  Use for KDE 4.6.1 and later
 */
#ifndef __BCOMPARE_KDE_H__
#define __BCOMPARE_KDE_H__

#include <Qt/qsignalmapper.h>
#include <kabstractfileitemactionplugin.h>
#include <kfileitemlistproperties.h>

typedef enum {
	MENU_NONE = 0,
	MENU_MAIN = 1,
	MENU_SUBMENU = 2
} MenuTypes;

class BCompareKde : public KAbstractFileItemActionPlugin {
  Q_OBJECT
  public:
    BCompareKde(QObject *pParent, const QVariantList &args);
    virtual ~BCompareKde();

    virtual QList<QAction*> actions(const KFileItemListProperties &fileItemInfos,
				QWidget *parentWidget);
  public slots:
	void select_left_action(void);
	void select_center_action(void);
	void edit_file_action(void);
	void compare_action(const QString &);
	void sync_action(void);
	void merge_action(void);

  protected:
	void clear_selections(void);
	bool file_is_dir(const char * path);
	void alert_updated(void);
	QAction *select_left_mitem(bool IsDir);
	QAction *select_center_mitem(void);
	QAction *edit_file_mitem(void);
	QAction *compare_mitem(QString &FileViewer, 
							int SelectedCnt);
	QAction *sync_mitem(int SelectedCnt);
	QAction *merge_mitem(int SelectedCnt);
	void bcompare_create_dir_menus(QList<QAction*> *items,
								   int SelectedCnt,
								   bool FirstIsDir,
								   MenuTypes CurrentMenuType);
	void bcompare_create_file_menus(QList<QAction*> *items,
									int SelectedCnt,
									bool FirstIsDir,
									MenuTypes CurrentMenuType);

  protected:
	bool Enabled;
	MenuTypes CompareMenuType;
	MenuTypes CompareUsingMenuType;
	MenuTypes MergeMenuType;
	MenuTypes SyncMenuType;
	MenuTypes EditMenuType;
	QStringList Viewers;
	QStringList Masks;
	QString IconPath;
	bool LeftIsDir;
	KUrl LeftFile;
	KUrl RightFile;
	KUrl CenterFile;
	KUrl StorageDir;
	KUrl LeftFileStorage;
	KUrl CenterFileStorage;
	QSignalMapper *mapper;
	QWidget *parent;

};

#endif //__BCOMPARE_KDE_H__
