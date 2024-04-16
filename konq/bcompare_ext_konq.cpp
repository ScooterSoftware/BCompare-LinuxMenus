/*----------------------------------------------------------------------\
| Beyond Compare (tm)													|
| Copyright (c) 1996-2024 Scooter Software, Inc.						|
| All rights reserved.							www.scootersoftware.com	|
\----------------------------------------------------------------------*/
#include <Qt/qlabel.h>
#include <Qt/qstring.h>
#include <Qt/qwidget.h>
#include <Qt/qsettings.h>
#include <Qt/qdir.h>
#include <kactionmenu.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>
#include <kiconloader.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "bcompare_ext_konq.h"
#include "bcompare_ext_konq.moc"

#define DIR_PERM (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)


/*
 * NO_HINT_STRINGS define switcht
 *
 * older kde/qt versions (~qt 4.4 4.5) does not have setHelp
 * in the KAction objection.  This define was put in to turn
 * off hint strings while we need to compile for older distros
 * (KUbuntu 8.10, OpenSuse 11.3).  It is set in the Makefile.
 * Once this is not needed the define can be disabled and the
 * hint strings will appear.
 */

typedef KGenericFactory<BCompareKonq> factory;
K_EXPORT_COMPONENT_FACTORY(bcompare_ext_konq, factory("bcompare_ext_konq"))

/*************************************************************
 *
 * Utilities
 *
 *************************************************************/

void
BCompareKonq::clear_selections(void)
{
	LeftFile.setPath("");
	RightFile.setPath("");
	CenterFile.setPath("");

	unlink((const char *)LeftFileStorage.path().toUtf8().constData());
	unlink((const char *)CenterFileStorage.path().toUtf8().constData());
}

bool
BCompareKonq::file_is_dir(const char *path)
{
	bool isdir;
	int mcnt;
	struct stat stat_buf;

	stat(path, &stat_buf);
	isdir = S_ISDIR(stat_buf.st_mode);
	if (!isdir) {
		for (mcnt = 0; mcnt < Masks.count(); mcnt++) {
			isdir = isdir | QString(path).endsWith(Masks[mcnt].toUtf8().constData());
		}
	}

	return isdir;
}

/*************************************************************
 *
 * Action callbacks
 *
 *************************************************************/

void
BCompareKonq::select_left_action(void)
{
	FILE *fileptr;
	QDir storage;

	storage.mkpath(StorageDir.path());
	fileptr =
		fopen((const char *)LeftFileStorage.path().toUtf8().constData(), "w");
	if (fileptr != NULL) {
		fputs((const char *)RightFile.path().toUtf8().constData(), fileptr);
		fclose(fileptr);
	}
}

void
BCompareKonq::select_center_action(void)
{
	FILE *fileptr;
	QDir storage;

	storage.mkpath(StorageDir.path());
	fileptr =
		fopen((const char *)CenterFileStorage.path().toUtf8().constData(), "w");
	if (fileptr != NULL) {
		fputs((const char *)RightFile.path().toUtf8().constData(), fileptr);
		fclose(fileptr);
	}
}

void
BCompareKonq::edit_file_action(void)
{
	QStringList args;

	args << "-edit" << RightFile.path();
	KToolInvocation::kdeinitExec("bcompare", args);
	clear_selections();
}

void
BCompareKonq::compare_action(const QString &FileViewer)
{
	QStringList args;

	if (FileViewer.compare("") != 0) {
		args << QString("-fv=\"\"%1\"\"").arg(FileViewer);
	}
	args << LeftFile.path() << RightFile.path();
	KToolInvocation::kdeinitExec("bcompare", args);
	clear_selections();
}

void
BCompareKonq::sync_action(void)
{
	QStringList args;

	args << "-sync" << LeftFile.path() << RightFile.path();
	KToolInvocation::kdeinitExec("bcompare", args);
	clear_selections();
}

void
BCompareKonq::merge_action(void)
{
	QStringList args;

	args << "-fv=\"\"Text Merge\"\"" << LeftFile.path() << RightFile.path();
	if (!CenterFile.path().isEmpty()) {
		args << CenterFile.path();
	}
	KToolInvocation::kdeinitExec("bcompare", args);
	clear_selections();
}

/*************************************************************
 *
 * Menu Items
 *
 *************************************************************/

KAction *
BCompareKonq::select_left_mitem(KActionCollection* actionCollection, bool IsDir)
{
	KAction *item;
	QString MenuStr;
	QString HintStr;
	QString ItemStr;

	if (IsDir) ItemStr = QString("Folder");
	else ItemStr = QString("File");

	if ((CompareMenuType == MENU_SUBMENU) ||
		((LeftFile != NULL) && (LeftIsDir == IsDir))) {
		MenuStr = QString("Select Left %1").arg(ItemStr);
	}
	else if ((MergeMenuType != MENU_NONE) && (!IsDir)) {
		MenuStr = QString("Select Left %1 for Compare/Merge").arg(ItemStr);
	}
	else {
		MenuStr = QString("Select Left %1 for Compare").arg(ItemStr);
	}

	HintStr = QString("Remembers selected item for later comparison using Beyond Compare. Right-click another item to start the comparison");
	item = actionCollection->addAction("BCompareExt::select_left");
	item->setText(MenuStr);
#ifndef NO_HINT_STRINGS
	item->setHelpText(HintStr);
#endif
	item->setIcon(KIcon("bcomparehalf32"));

	connect(item, SIGNAL(triggered()), this, SLOT(select_left_action()));

	return item;
}

KAction *
BCompareKonq::select_center_mitem(KActionCollection* actionCollection)
{
	KAction *item;

	item = actionCollection->addAction("BCompareExt::select_center");
	item->setText(i18n("Select Center File"));
#ifndef NO_HINT_STRINGS
	item->setHelpText(i18n("Remembers selected item for later comparison using Beyond Compare. Right-click another item to start the comparison"));
#endif
	item->setIcon(KIcon("bcomparehalf32"));

	connect(item, SIGNAL(triggered()), this, SLOT(select_center_action()));
	return item;
}

KAction *
BCompareKonq::edit_file_mitem(KActionCollection* actionCollection)
{
	KAction *item;
	QString MenuStr;
	if (EditMenuType == MENU_SUBMENU)
		MenuStr = QString("Edit");
	else
		MenuStr = QString("Edit with Beyond Compare");

	item = actionCollection->addAction("BCompareExt::edit_file");
	item->setText(MenuStr);
#ifndef NO_HINT_STRINGS
	item->setHelpText(i18n("Edit the file using Beyond Compare"));
#endif
	item->setIcon(KIcon("bcomparefull32"));

	connect(item, SIGNAL(triggered()), this, SLOT(edit_file_action()));
	return item;
}

KAction *
BCompareKonq::compare_mitem(
		KActionCollection* actionCollection,
		QString &FileViewer,
		int SelectedCnt)
{
	KAction *item;
	QString MenuStr;
	QString HintStr;
	QString NameStr;


	if (FileViewer.compare("") == 0) {
		if (SelectedCnt == 1) {
			MenuStr = QString("Compare to %1").arg(LeftFile.fileName());
			HintStr = QString("Compare selected item with previously selected left item, using Beyond Compare");
		}
		else {
			MenuStr = QString("Compare");
			HintStr = QString("Compare selected items using Beyond Compare");
		}
	}
	else {
		MenuStr = FileViewer;
		HintStr =
			QString("Compare files using the '%1' viewer").arg(FileViewer);
	}

	NameStr = QString("BCompareExt::compare_using %1").arg(FileViewer);

	item = actionCollection->addAction(NameStr);
	item->setText(MenuStr);
#ifndef NO_HINT_STRINGS
	item->setHelpText(HintStr);
#endif
	item->setIcon(KIcon("bcomparefull32"));

	mapper->setMapping(item, FileViewer);
	connect(item, SIGNAL(triggered()), mapper, SLOT(map()));

	return item;
}

KAction *
BCompareKonq::sync_mitem(
		KActionCollection* actionCollection,
		int SelectedCnt)
{
	KAction *item;
	QString MenuStr;
	QString HintStr;


	if (SelectedCnt == 1) {
		MenuStr = QString("Sync with '%1'").arg(LeftFile.fileName());
		HintStr = QString("Sync to previously selected folder");
	}
	else {
		MenuStr = QString("Sync");
		HintStr = QString("Sync two selected folders");
	}

	item = actionCollection->addAction("BCompareExt::sync");
	item->setText(MenuStr);
#ifndef NO_HINT_STRINGS
	item->setHelpText(HintStr);
#endif
	item->setIcon(KIcon("bcomparefull32"));

	connect(item, SIGNAL(triggered()), this, SLOT(sync_action()));
	return item;
}

KAction *
BCompareKonq::merge_mitem(
		KActionCollection* actionCollection,
		int SelectedCnt)
{
	KAction *item;
	QString MenuStr;
	QString HintStr;

	if (SelectedCnt == 1){
		if (CenterFile.path().isEmpty()) {
			MenuStr = QString("Merge with '%1'").arg(LeftFile.fileName());
			HintStr = QString("Merge file with previously selected left file using Beyond Compare");
		}
		else {
			MenuStr = QString("Merge with '%1', '%2'").arg(LeftFile.fileName(), CenterFile.fileName());
			HintStr = QString(
				"Merge file with previously selected left and center files using Beyond Compare");
		}
	}
	else if (SelectedCnt == 2){
		if (CenterFile.path().isEmpty()) {
			MenuStr = QString("Merge");
			HintStr = QString("Merge selected files (left, right)");
		}
		else {
			MenuStr = QString("Merge with '%1'").arg(CenterFile.fileName());
			HintStr = QString("Merge selected files (left, right) with previously selected center file");
		}
	}
	else if (SelectedCnt == 3) {
		MenuStr = QString("Merge");
		HintStr = QString("Merge selected files (left, right, center)");
	}
	else return NULL;


	item = actionCollection->addAction("BCompareExt::merge");
	item->setText(MenuStr);
#ifndef NO_HINT_STRINGS
	item->setHelpText(HintStr);
#endif
	item->setIcon(KIcon("bcomparefull32"));

	connect(item, SIGNAL(triggered()), this, SLOT(merge_action()));
	return item;
}


/*************************************************************
 *
 * Menu Item creation
 *
 *************************************************************/

void
BCompareKonq::bcompare_create_dir_menus(
		KActionCollection *actionCollection,
		QMenu *menu,
		int SelectedCnt,
		bool FirstIsDir,
		MenuTypes CurrentMenuType)
{
	KAction *item;
	QString nillers = QString("");

	if ((!FirstIsDir) || (SelectedCnt > 2)) return;

	if ((LeftIsDir) &&
			(!LeftFile.path().isEmpty()) && (!RightFile.path().isEmpty())) {
		if (CurrentMenuType == CompareMenuType) {
			item = compare_mitem(actionCollection, nillers, SelectedCnt);
			if (item != NULL) menu->addAction(item);
		}
		if (CurrentMenuType == SyncMenuType) {
			item = sync_mitem(actionCollection, SelectedCnt);
			if (item != NULL) menu->addAction(item);
		}
	}

	if (SelectedCnt == 1) {
		if (CurrentMenuType == CompareMenuType) {
			item = select_left_mitem(actionCollection, TRUE);
			if (item != NULL) menu->addAction(item);
		}
	}
}

void
BCompareKonq::bcompare_create_file_menus(
		KActionCollection *actionCollection,
		QMenu *menu,
		int SelectedCnt,
		bool FirstIsDir,
		MenuTypes CurrentMenuType)
{
	KAction *item;
	KActionMenu *SubMenu;
	int Cnt;
	QString nillers = QString("");

	if ((FirstIsDir) || (SelectedCnt > 3)) return;

	if ((!LeftIsDir) &&
			(!LeftFile.path().isEmpty()) && (!RightFile.path().isEmpty())) {
		if (MergeMenuType == CurrentMenuType ) {
			item = merge_mitem(actionCollection, SelectedCnt);
			if (item != NULL) menu->addAction(item);
		}
		if (SelectedCnt < 3) {
			if (CompareMenuType == CurrentMenuType) {
				item = compare_mitem(actionCollection, nillers, SelectedCnt);
				if (item != NULL) menu->addAction(item);
			}
			if ((CompareUsingMenuType == CurrentMenuType) &&
				(Viewers.count() > 0)) {

				SubMenu =
					new KActionMenu(i18n("BeyondCompareExt::compareusing"),
									actionCollection);
				SubMenu->setText(i18n("Compare using"));
#ifndef NO_HINT_STRINGS
				SubMenu->setHelpText(i18n("Select viewer type for compare"));
#endif
				SubMenu->setIcon(KIcon("bcomparefull32"));

				for (Cnt = 0; Cnt < Viewers.count(); Cnt++) {
					SubMenu->addAction(
						compare_mitem(actionCollection,
							Viewers[Cnt], SelectedCnt));
				}
				menu->addAction(SubMenu);
			}
		}
	}

	if (SelectedCnt == 1) {
		if (CompareMenuType == CurrentMenuType) {
			item = select_left_mitem(actionCollection, FALSE);
			if (item != NULL) menu->addAction(item);
			if ((!LeftIsDir) && (!LeftFile.path().isEmpty())) {
				item = select_center_mitem(actionCollection);
				if (item != NULL) menu->addAction(item);
			}
		}
		if (EditMenuType == CurrentMenuType) {
			item = edit_file_mitem(actionCollection);
			if (item != NULL) menu->addAction(item);
		}
	}
}

/*************************************************************
 *
 * Extension functions for XXXMenuProvider Interface
 *
 *************************************************************/

void
BCompareKonq::setup(KActionCollection *actionCollection,
				   const KonqPopupMenuInformation &popupMenuInfo,
				   QMenu *menu)
{
	KFileItemList selected = popupMenuInfo.items();
	int SelectedCnt = selected.count();
	bool FirstIsDir;
	char leftfilepath[1024];
	char centerfilepath[1024];
	char *leftfileptr, *centerfileptr;
	FILE *filestrptr;
	KActionMenu *SubMenu;

	if ((actionCollection == NULL) ||
			(!Enabled) ||
			(SelectedCnt == 0)) {
		return;
	}

	FirstIsDir = file_is_dir(selected.first().url().path().toUtf8().constData());

	if (SelectedCnt > 1) {
		for (KFileItemList::iterator i = selected.begin();
				i != selected.end(); i++) {
			if (FirstIsDir != file_is_dir((*i).url().path().toUtf8().constData())) {
				return;
			}
		}
	}

	leftfileptr = NULL;
	filestrptr =
		fopen((const char *)LeftFileStorage.path().toUtf8().constData(), "r");
	if (filestrptr != NULL) {
		leftfileptr = fgets(leftfilepath, 255, filestrptr);
		fclose(filestrptr);
	}

	centerfileptr = NULL;
	filestrptr =
		fopen((const char *)CenterFileStorage.path().toUtf8().constData(), "r");
	if (filestrptr != NULL) {
		centerfileptr = fgets(centerfilepath, 255, filestrptr);
		fclose(filestrptr);
	}

	if (SelectedCnt == 3) {
		CenterFile = selected.at(2).url();
	}
	if (SelectedCnt >= 2) {
		LeftFile = selected.at(0).url();
		RightFile = selected.at(1).url();
	}
	if (SelectedCnt == 1) {

		if (leftfileptr != NULL) LeftFile.setPath(leftfileptr);
		else LeftFile.setPath("");

		RightFile = selected.at(0).url();

		if (centerfileptr != NULL) CenterFile.setPath(centerfileptr);
		else CenterFile.setPath("");
	}

	if (!LeftFile.path().isEmpty()) {
		LeftIsDir = file_is_dir(LeftFile.path().toUtf8().constData());
	}

	connect(mapper, SIGNAL(mapped(const QString &)),
			this, SLOT(compare_action(const QString &)));

	menu->addSeparator();

	SubMenu = new KActionMenu(i18n("BeyondCompareExt::Top"),
									actionCollection);
	SubMenu->setText(i18n("Beyond Compare"));
#ifndef NO_HINT_STRINGS
	SubMenu->setHelpText(i18n("Beyond Compare funcitons"));
#endif
	SubMenu->setIcon(KIcon("bcomparefull32"));

	bcompare_create_dir_menus(actionCollection, (QMenu *)SubMenu->menu(),
			SelectedCnt, FirstIsDir, MENU_SUBMENU);
	bcompare_create_file_menus(actionCollection, (QMenu *)SubMenu->menu(),
			SelectedCnt, FirstIsDir, MENU_SUBMENU);

	if (actionCollection->count() > 0) menu->addAction(SubMenu);
	else delete SubMenu;

	bcompare_create_dir_menus(actionCollection, menu,
					SelectedCnt, FirstIsDir, MENU_MAIN);
	bcompare_create_file_menus(actionCollection, menu,
					SelectedCnt, FirstIsDir, MENU_MAIN);

	menu->addSeparator();

}

BCompareKonq::BCompareKonq(QObject *popupMenu, const QStringList &list)
	: KonqPopupMenuPlugin(popupMenu)
{
	int cnt;
	QString enb;
	char *env;
	QString configdir;
   
	env = getenv("XDG_CONFIG_HOME");
	if (env == NULL) {
		env = getenv("HOME");
		configdir = QString("%1/.config/bcompare5").arg(env);
	}
	else configdir = QString("%1/bcompare5").arg(env);

	QSettings MenuIni(QString("%1/menu.ini").arg(configdir),
						QSettings::IniFormat);

	MenuIni.beginGroup("ContextMenus");

	Enabled = FALSE;
	enb = MenuIni.value("Enabled", "TRUE").toString();
	if ((enb.compare("true") == 0) || (enb.compare("TRUE") == 0) ||
			(enb.compare("True") == 0)) {
		Enabled = TRUE;
	}

	CompareMenuType = (MenuTypes)MenuIni.value("Compare", MENU_MAIN).toInt();
	CompareUsingMenuType =
			(MenuTypes)MenuIni.value("CompareUsing", MENU_MAIN).toInt();
	MergeMenuType = (MenuTypes)MenuIni.value("Merge", MENU_MAIN).toInt();
	SyncMenuType = (MenuTypes)MenuIni.value("Sync", MENU_MAIN).toInt();
	EditMenuType = (MenuTypes)MenuIni.value("Edit", MENU_MAIN).toInt();

	Viewers = MenuIni.value("Viewers", "").toStringList();
	Masks = MenuIni.value("ArchiveMasks", "").toStringList();
	for (cnt = 0; cnt < Masks.count(); cnt++) {
		Masks[cnt].remove(0,1);
	}

	LeftFile.setPath("");
	LeftFile.setProtocol("");
	LeftIsDir = false;
	RightFile.setPath("");
	CenterFile.setPath("");

	MenuIni.endGroup();

	StorageDir = KUrl::fromPath(QString("%1/").arg(configdir));
	LeftFileStorage =
		KUrl::fromPath(QString("%1/left_file").arg(configdir));
	CenterFileStorage =
		KUrl::fromPath(QString("%1/center_file").arg(configdir));

	mapper = new QSignalMapper(this);
}

BCompareKonq::~BCompareKonq()
{
	delete mapper;
}

