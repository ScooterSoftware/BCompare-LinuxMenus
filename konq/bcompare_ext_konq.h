/*----------------------------------------------------------------------\
| Beyond Compare (tm)													|
| Copyright (c) 1996-2015 Scooter Software, Inc.						|
| All rights reserved.							www.scootersoftware.com	|
\----------------------------------------------------------------------*/
#ifndef __BCOMPARE_KONQ_H__
#define __BCOMPARE_KONQ_H__

#include <Qt/qstring.h>
#include <Qt/qsignalmapper.h>
#include <konq_popupmenu.h>
#include <konq_popupmenuinformation.h>
#include <konq_popupmenuplugin.h>
#include <kconfig.h>
#include <kfileitem.h>

typedef enum {
	MENU_NONE = 0,
	MENU_MAIN = 1,
	MENU_SUBMENU = 2
} MenuTypes;

class BCompareKonq : public KonqPopupMenuPlugin {
  Q_OBJECT
  public:
	BCompareKonq(QObject *popupMenu, const QStringList &list);
	virtual ~BCompareKonq();

	virtual void setup(KActionCollection *actionCollection,
					   const KonqPopupMenuInformation &popupMenuInfo,
					   QMenu *menu);

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
	KAction *select_left_mitem(KActionCollection *actionCollection, bool IsDir);
	KAction *select_center_mitem(KActionCollection *actionCollection);
	KAction *edit_file_mitem(KActionCollection *actionCollection);
	KAction *compare_mitem(KActionCollection *actionCollection,
							QString &FileViewer,
							int SelectedCnt);
	KAction *sync_mitem(KActionCollection *actionCollection,
						int SelectedCnt);
	KAction *merge_mitem(KActionCollection *actionCollection,
						 int SelectedCnt);
	void bcompare_create_dir_menus(KActionCollection *actionCollection,
								   QMenu *menu,
								   int SelectedCnt,
								   bool FirstIsDir,
								   MenuTypes CurrentMenuType);
	void bcompare_create_file_menus(KActionCollection *actionCollection,
									QMenu *menu,
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
	bool LeftIsDir;
	KUrl LeftFile;
	KUrl RightFile;
	KUrl CenterFile;
	KUrl StorageDir;
	KUrl LeftFileStorage;
	KUrl CenterFileStorage;
	QSignalMapper *mapper;

};

#endif //__BCOMPARE_KONQ_H__
