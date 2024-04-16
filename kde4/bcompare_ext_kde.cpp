/*----------------------------------------------------------------------\
| Beyond Compare (tm)													|
| Copyright (c) 1996-2024 Scooter Software, Inc.						|
| All rights reserved.							www.scootersoftware.com	|
\----------------------------------------------------------------------*/
#include <Qt/qsettings.h>
#include <Qt/qdir.h>
#include <QMenu>
#include <kgenericfactory.h>
#include <ktoolinvocation.h>
#include <kfileitem.h>
#include <unistd.h>

#include "bcompare_ext_kde.h"
#include "bcompare_ext_kde.moc"

K_PLUGIN_FACTORY(bcompare_kde_factory, registerPlugin<BCompareKde>();)
K_EXPORT_PLUGIN(bcompare_kde_factory("bcompare_ext_kde"))

/*************************************************************
 *
 * Utilities
 *
 *************************************************************/

void
BCompareKde::clear_selections(void)
{
	LeftFile.setPath("");
	RightFile.setPath("");
	CenterFile.setPath("");

	unlink((const char *)LeftFileStorage.path().toUtf8().constData());
	unlink((const char *)CenterFileStorage.path().toUtf8().constData());
}

bool
BCompareKde::file_is_dir(const char *path)
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
BCompareKde::select_left_action(void)
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
BCompareKde::select_center_action(void)
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
BCompareKde::edit_file_action(void)
{
	QStringList args;

	args << "-edit" << RightFile.path();
	KToolInvocation::kdeinitExec("bcompare", args);
	clear_selections();
}

void
BCompareKde::compare_action(const QString &FileViewer)
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
BCompareKde::sync_action(void)
{
	QStringList args;

	args << "-sync" << LeftFile.path() << RightFile.path();
	KToolInvocation::kdeinitExec("bcompare", args);
	clear_selections();
}

void
BCompareKde::merge_action(void)
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

QAction *
BCompareKde::select_left_mitem(bool IsDir)
{
	QAction *item;
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
	item = new QAction(this);
	item->setText(MenuStr);
	item->setToolTip(HintStr);
	item->setIcon(QIcon(QString("%1/bcomparehalf32").arg(IconPath)));

	connect(item, SIGNAL(triggered()), this, SLOT(select_left_action()));

	return item;
}

QAction *
BCompareKde::select_center_mitem(void)
{
	QAction *item;

	item = new QAction(this);
	item->setText(i18n("Select Center File"));
	item->setToolTip(i18n("Remembers selected item for later comparison using Beyond Compare. Right-click another item to start the comparison"));
	item->setIcon(QIcon(QString("%1/bcomparehalf32").arg(IconPath)));

	connect(item, SIGNAL(triggered()), this, SLOT(select_center_action()));
	return item;
}

QAction *
BCompareKde::edit_file_mitem(void)
{
	QAction *item;
	QString MenuStr;
	if (EditMenuType == MENU_SUBMENU)
		MenuStr = QString("Edit");
	else
		MenuStr = QString("Edit with Beyond Compare");

	item = new QAction(this);
	item->setText(MenuStr);
	item->setToolTip(i18n("Edit the file using Beyond Compare"));
	item->setIcon(QIcon(QString("%1/bcomparefull32").arg(IconPath)));

	connect(item, SIGNAL(triggered()), this, SLOT(edit_file_action()));
	return item;
}

QAction * 
BCompareKde::compare_mitem(
		QString &FileViewer,
		int SelectedCnt)
{
	QAction *item;
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

	item = new QAction(this);
	item->setText(MenuStr);
	item->setToolTip(HintStr);
	item->setIcon(QIcon(QString("%1/bcomparefull32").arg(IconPath)));

	mapper->setMapping(item, FileViewer);
	connect(item, SIGNAL(triggered()), mapper, SLOT(map()));

	return item;
}

QAction * 
BCompareKde::sync_mitem(
		int SelectedCnt)
{
	QAction *item;
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

	item = new QAction(this);
	item->setText(MenuStr);
	item->setToolTip(HintStr);
	item->setIcon(QIcon(QString("%1/bcomparefull32").arg(IconPath)));

	connect(item, SIGNAL(triggered()), this, SLOT(sync_action()));
	return item;
}

QAction * 
BCompareKde::merge_mitem(
		int SelectedCnt)
{
	QAction *item;
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

	item = new QAction(this);
	item->setText(MenuStr);
	item->setToolTip(HintStr);
	item->setIcon(QIcon(QString("%1/bcomparefull32").arg(IconPath)));

	connect(item, SIGNAL(triggered()), this, SLOT(merge_action()));
	return item;
}


/*************************************************************
 *
 * Menu Item creation
 *
 *************************************************************/

void
BCompareKde::bcompare_create_dir_menus(
		QList<QAction*> *items,
		int SelectedCnt,
		bool FirstIsDir,
		MenuTypes CurrentMenuType)
{

	QAction *item;
	QString nillers = QString("");

	if ((!FirstIsDir) || (SelectedCnt > 2)) return;

	if ((LeftIsDir) &&
			(!LeftFile.path().isEmpty()) && (!RightFile.path().isEmpty())) {
		if (CurrentMenuType == CompareMenuType) {
			item = compare_mitem(nillers, SelectedCnt);
			if (item != NULL) items->append(item);
		}
		if (CurrentMenuType == SyncMenuType) {
			item = sync_mitem(SelectedCnt);
			if (item != NULL) items->append(item);
		}
	}

	if (SelectedCnt == 1) {
		if (CurrentMenuType == CompareMenuType) {
			item = select_left_mitem(TRUE);
			if (item != NULL) items->append(item);
		}
	}
}

void
BCompareKde::bcompare_create_file_menus(
		QList<QAction*> *items,
		int SelectedCnt,
		bool FirstIsDir,
		MenuTypes CurrentMenuType)
{
    QAction *item;
	QMenu *SubMenu;
	QAction *SubMenuAction;
	int Cnt;
	QString nillers = QString("");

	if ((FirstIsDir) || (SelectedCnt > 3)) return;

	if ((!LeftIsDir) &&
			(!LeftFile.path().isEmpty()) && (!RightFile.path().isEmpty())) {
		if (MergeMenuType == CurrentMenuType ) {
			item = merge_mitem(SelectedCnt);
			if (item != NULL) items->append(item);
		}
		if (SelectedCnt < 3) {
			if (CompareMenuType == CurrentMenuType) {
				item = compare_mitem(nillers, SelectedCnt);
				if (item != NULL) items->append(item);
			}
			if ((CompareUsingMenuType == CurrentMenuType) &&
				(Viewers.count() > 0)) {

				SubMenu = new QMenu();
				SubMenuAction = new QAction(this);
				SubMenuAction->setMenu(SubMenu);
				SubMenu->setTitle(i18n("Compare Using"));
				SubMenu->setIcon(QIcon(QString("%1/bcomparefull32").arg(IconPath)));

				for (Cnt = 0; Cnt < Viewers.count(); Cnt++) {
					SubMenu->addAction(compare_mitem(Viewers[Cnt], SelectedCnt));
				}
				items->append(SubMenuAction);
			}
		}
	}

	if (SelectedCnt == 1) {
		if (CompareMenuType == CurrentMenuType) {
			item = select_left_mitem(FALSE);
			if (item != NULL) items->append(item);
			if ((!LeftIsDir) && (!LeftFile.path().isEmpty())) {
				item = select_center_mitem();
				if (item != NULL) items->append(item);
			}
		}
		if (EditMenuType == CurrentMenuType) {
			item = edit_file_mitem();
			if (item != NULL) items->append(item);
		}
	}
}

/*************************************************************
 *
 * Extension functions for XXXMenuProvider Interface
 *
 *************************************************************/

QList<QAction*>
BCompareKde::actions(const KFileItemListProperties &fileItemInfos,
                   	 QWidget *parentWidget)
{
	QList<QAction*> items;
	KFileItemList selected = fileItemInfos.items();
	int SelectedCnt = selected.count();
	bool FirstIsDir;
	char leftfilepath[1024];
	char centerfilepath[1024];
	char *leftfileptr, *centerfileptr;
	FILE *filestrptr;
	QMenu *SubMenu;
	QAction *SubMenuAction;
	int Cnt;

	parent = parentWidget;

	if ((!Enabled) || (SelectedCnt == 0)) {
		return items;
	}

	FirstIsDir = file_is_dir(selected.first().url().path().toUtf8().constData());

	if (SelectedCnt > 1) {
		for (KFileItemList::iterator i = selected.begin();
				i != selected.end(); i++) {
			if (FirstIsDir != file_is_dir((*i).url().path().toUtf8().constData())) {
				return items;
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

	bcompare_create_dir_menus(&items, SelectedCnt, FirstIsDir, MENU_SUBMENU);
	bcompare_create_file_menus(&items, SelectedCnt, FirstIsDir, MENU_SUBMENU);

	if (items.count() > 0) {

		SubMenu = new QMenu();
		SubMenu->setTitle(i18n("Beyond Compare"));
		SubMenu->setIcon(QIcon(QString("%1/bcomparefull32").arg(IconPath)));
		SubMenuAction = new QAction(this);
		SubMenuAction->setMenu(SubMenu);

		for (Cnt = 0; Cnt < items.count(); Cnt++) {
				SubMenu->addAction(items[Cnt]);
		}
		items.clear();
		items.append(SubMenuAction);
	}
	bcompare_create_dir_menus(&items, SelectedCnt, FirstIsDir, MENU_MAIN);
	bcompare_create_file_menus(&items, SelectedCnt, FirstIsDir, MENU_MAIN);

	return items;
}

BCompareKde::BCompareKde(QObject *pParent, const QVariantList & /*args*/) 
	: KAbstractFileItemActionPlugin(pParent)
{
	int cnt;
	QString enb;
	char *env;
	QString configdir;
   
	env = getenv("HOME");
	configdir = QString("%1/.beyondcompare").arg(env);
	if (!QDir(configdir).exists()) {
		env = getenv("XDG_CONFIG_HOME");
		if (env == NULL) {
			env = getenv("HOME");
			configdir = QString("%1/.config/bcompare").arg(env);
		}
		else configdir = QString("%1/bcompare").arg(env);
	}

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

	IconPath = MenuIni.value("IconPath", "/usr/share/pixmaps/").toString();
	MenuIni.endGroup();

	StorageDir = KUrl::fromPath(QString("%1/").arg(configdir));
	LeftFileStorage =
		KUrl::fromPath(QString("%1/left_file").arg(configdir));
	CenterFileStorage =
		KUrl::fromPath(QString("%1/center_file").arg(configdir));

	mapper = new QSignalMapper(this);
}

BCompareKde::~BCompareKde() 
{
	delete mapper;
}

