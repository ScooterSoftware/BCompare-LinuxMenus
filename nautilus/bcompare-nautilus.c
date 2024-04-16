/*----------------------------------------------------------------------\
| Beyond Compare (tm)													|
| Copyright (c) 1996-2024 Scooter Software, Inc.						|
| All rights reserved.							www.scootersoftware.com	|
\----------------------------------------------------------------------*/
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <glib/gstdio.h>

#include <libnautilus-extension/nautilus-file-info.h>
#include <libnautilus-extension/nautilus-menu-provider.h>
#include <libnautilus-extension/nautilus-menu.h>

#define DIR_PERM (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define GBOOLEAN_TO_POINTER(i) (GINT_TO_POINTER ((i) ? 2 : 1))
#define GPOINTER_TO_BOOLEAN(i) ((gboolean) ((GPOINTER_TO_INT(i) == 2) ? TRUE : FALSE))

typedef NautilusMenuItem BcMenuItem;

typedef enum {
	MENU_NONE = 0,
	MENU_MAIN = 1,
	MENU_SUBMENU = 2
} MenuTypes;

typedef struct {
	GObject parent_slot;
	GtkWidget *Winder;
	gboolean Enabled;
	MenuTypes CompareMenuType;
	MenuTypes CompareUsingMenuType;
	MenuTypes MergeMenuType;
	MenuTypes SyncMenuType;
	MenuTypes EditMenuType;
	gchar **Masks;
	int MaskCnt;
	gchar **Viewers;
	int ViewerCnt;
	gboolean LeftIsDir;
	GString *LeftFile;
	GString *RightFile;
	GString *CenterFile;
	GString *StorageDir;
	GString *LeftFileStorage;
	GString *CenterFileStorage;
} BCompareExt;

typedef struct BCompareExtClass {
  GObjectClass parent_class;
} BCompareExt_class, BCompareExtClass;

static GType type_list[1];
static GObjectClass *parent_class;

static void bcompare_ext_register_type(GTypeModule *module);
void nautilus_module_initialize(GTypeModule *module);
void nautilus_module_shutdown(void);
void nautilus_module_list_types(const GType **types, gint *n_types);

/*************************************************************
 *
 * Utilities
 *
 *************************************************************/


static void setup_display(gpointer data)
{
	char* pDisplayName = (char*)data;

	if(strstr(pDisplayName, "wayland") != NULL)
		g_setenv("WAYLAND_DISPLAY",pDisplayName, TRUE);
	else
		g_setenv("DISPLAY", pDisplayName, TRUE);
}

static void spawn_bc(GtkWidget *window, char **argv)
{
	GdkScreen *screen = gtk_widget_get_screen(window);
	GError *error = NULL;
	char *display;

	if (screen != NULL) {
		display = gdk_screen_make_display_name(screen);
	} else {
		display = NULL;
	}

	if (g_spawn_async(NULL, argv, NULL,
			G_SPAWN_FILE_AND_ARGV_ZERO | G_SPAWN_SEARCH_PATH, 
			setup_display, display, NULL, &error) != TRUE) {
		GtkWindow *parent;
		GtkMessageDialog *dialog;
		gchar *cmd_line = g_strjoinv(" ", &argv[1]);

		parent = GTK_WINDOW(gtk_widget_get_ancestor(window, GTK_TYPE_WINDOW));

		dialog = (GtkMessageDialog *)gtk_message_dialog_new(parent,
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE,
					"Error(%d): %s\n\nUnable to run \"%s\"",
					error->code, error->message, cmd_line);

	/* Destroy the dialog when the user responds to it (e.g. clicks a button) */
		g_signal_connect_swapped(dialog, "response",
				G_CALLBACK(gtk_widget_destroy), dialog);
		gtk_widget_show_all(GTK_WIDGET(dialog));
		g_error_free(error);
	}
	g_free(display);
}

static void clear_selections(BCompareExt *bcobj)
{
	g_unlink(bcobj->LeftFileStorage->str);
	g_unlink(bcobj->CenterFileStorage->str);
}

static void alert_updated(BCompareExt *bcobj)
{
	g_signal_emit_by_name((NautilusMenuProvider *)bcobj, "items_updated");
}

static gboolean file_is_dir(BCompareExt *bcobj, char *filepath)
{
	gboolean isdir;
	int mcnt;
	gchar *basename = g_path_get_basename(filepath);

	isdir = g_file_test(filepath, G_FILE_TEST_IS_DIR);
	if (!isdir) {
		for (mcnt = 0; mcnt < bcobj->MaskCnt; mcnt++) {
			isdir = isdir |
				g_str_has_suffix(basename, bcobj->Masks[mcnt]);
		}
	}

	g_free(basename);
	return isdir;
}

static gchar * nautilus_to_path(NautilusFileInfo* file)
{
	return g_filename_from_uri(nautilus_file_info_get_uri(file), NULL, NULL);
}

/*************************************************************
 *
 * Action callbacks
 *
 *************************************************************/
static void select_left_action(BcMenuItem *item, BCompareExt *bcobj)
{
	GString *left_file;
	FILE *fileptr;

	left_file =
		(GString *)g_object_get_data((GObject *)item, "bcext::left_path");
	g_mkdir_with_parents(bcobj->StorageDir->str, DIR_PERM);
	fileptr = g_fopen(bcobj->LeftFileStorage->str, "w");
	if (fileptr != NULL) {
		if (left_file != NULL)
			fputs(left_file->str, fileptr);
		else fputs("", fileptr);
		fclose(fileptr);
	}
	if (left_file != NULL) g_string_free(left_file, TRUE);

	alert_updated(bcobj);
}

static void select_center_action(BcMenuItem *item, BCompareExt *bcobj)
{
	GString *center_file;
	FILE *fileptr;

	center_file =
		(GString *)g_object_get_data((GObject *)item, "bcext::center_file");
	g_mkdir_with_parents(bcobj->StorageDir->str, DIR_PERM);
	fileptr = g_fopen(bcobj->CenterFileStorage->str, "w");
	if (fileptr != NULL) {
		if (center_file != NULL)
			fputs(center_file->str, fileptr);
		else fputs("", fileptr);
		fclose(fileptr);
	}
	if (center_file != NULL) g_string_free(center_file, TRUE);

	alert_updated(bcobj);
}

static void edit_file_action(BcMenuItem *item, BCompareExt *bcobj)
{
	GString *edit_file;
	char *argv[5];

	edit_file =
		(GString *)g_object_get_data((GObject *)item, "bcext::edit_file");
	argv[0] = "bcompare";
	argv[1] = "bcompare";
	argv[2] = "-edit";
	if (edit_file != NULL)
		argv[3] = edit_file->str;
	else argv[3] = "";
	argv[4] = 0;

	spawn_bc(bcobj->Winder, argv);
	clear_selections(bcobj);

	if (edit_file != NULL) g_string_free(edit_file, TRUE);
}

static void compare_action(BcMenuItem *item, BCompareExt *bcobj)
{
	GString *left_file, *right_file;
	GString *msg = g_string_new("");
	char *fileviewer;
	char *argv[6];
	int cnt = 0;

	left_file =
		(GString *)g_object_get_data((GObject *)item, "bcext::left_file");
	right_file =
		(GString *)g_object_get_data((GObject *)item, "bcext::right_file");
	fileviewer =
		(char *)g_object_get_data((GObject *)item, "bcext::fileviewer");

	argv[cnt++] = "bcompare";
	argv[cnt++] = "bcompare";
	if (strcmp(fileviewer, "") != 0) {
		g_string_printf(msg, "-fv=\"%s\"", fileviewer);
		argv[cnt++] = msg->str;
	}
	if (left_file != NULL)
		argv[cnt++] = left_file->str;
	else argv[cnt++] = "";
	if (right_file != NULL)
		argv[cnt++] = right_file->str;
	else argv[cnt++] = "";
	argv[cnt++] = 0;

	spawn_bc(bcobj->Winder, argv);
	clear_selections(bcobj);

	g_string_free(msg, TRUE);
	if (left_file != NULL) g_string_free(left_file, TRUE);
	if (right_file != NULL) g_string_free(right_file, TRUE);
}

static void sync_action(BcMenuItem *item, BCompareExt *bcobj)
{
	GString *left_folder, *right_folder;
	char *argv[6];

	left_folder =
		(GString *)g_object_get_data((GObject *)item, "bcext::left_folder");
	right_folder =
		(GString *)g_object_get_data((GObject *)item, "bcext::right_folder");

	argv[0] = "bcompare";
	argv[1] = "bcompare";
	argv[2] = "-sync";
	if (left_folder != NULL)
		argv[3] = left_folder->str;
	else argv[3] = "";
	if (right_folder != NULL)
		argv[4] = right_folder->str;
	else argv[4] = "";
	argv[5] = 0;

	spawn_bc(bcobj->Winder, argv);
	clear_selections(bcobj);

	if (left_folder != NULL) g_string_free(left_folder, TRUE);
	if (right_folder != NULL) g_string_free(right_folder, TRUE);
}

static void merge_action(BcMenuItem *item, BCompareExt *bcobj)
{
	GString *left_file, *right_file, *center_file;
	char *argv[7];

	left_file =
		(GString *)g_object_get_data((GObject *)item, "bcext::left_file");
	right_file =
		(GString *)g_object_get_data((GObject *)item, "bcext::right_file");
	center_file =
		(GString *)g_object_get_data((GObject *)item, "bcext::center_file");

	argv[0] = "bcompare";
	argv[1] = "bcompare";
	argv[2] = "-fv=\"\"Text Merge\"\"";
	if (left_file != NULL)
		argv[3] = left_file->str;
	else argv[3] = "";
	if (right_file != NULL)
		argv[4] = right_file->str;
	else argv[4] = "";
	if (center_file != NULL)
		argv[5] = center_file->str;
	else argv[5] = "";
	argv[6] = 0;

	spawn_bc(bcobj->Winder, argv);
	clear_selections(bcobj);

	if (left_file != NULL) g_string_free(left_file, TRUE);
	if (right_file != NULL) g_string_free(right_file, TRUE);
	if (center_file != NULL) g_string_free(center_file, TRUE);
}

/*************************************************************
 *
 * Menu Items
 *
 *************************************************************/

static BcMenuItem * select_left_mitem(
		BCompareExt *bcobj,
		gboolean IsDir)
{
	BcMenuItem *item;
	GString *MenuStr = g_string_new("");
	GString *HintStr = g_string_new("");
	GString *ItemStr = g_string_new("");

	if (IsDir) g_string_assign(ItemStr, "Folder");
	else g_string_assign(ItemStr, "File");

	if ((bcobj->CompareMenuType == MENU_SUBMENU) ||
		((bcobj->LeftFile != NULL) && (bcobj->LeftIsDir == IsDir))) {
		g_string_printf(MenuStr, "Select Left %s", ItemStr->str);
	}
	else if ((bcobj->MergeMenuType != MENU_NONE) && (!IsDir)) {
		g_string_printf(MenuStr, "Select Left %s for Compare/Merge", ItemStr->str);
	}
	else {
		g_string_printf(MenuStr, "Select Left %s for Compare", ItemStr->str);
	}

	g_string_assign(HintStr, "Remembers selected item for later comparison using Beyond Compare. Right-click another item to start the comparison");

	item = nautilus_menu_item_new("BCompareExt::select_left",
							   MenuStr->str,
							   HintStr->str,
							   "bcomparehalf32");

	g_signal_connect(item, "activate",
		G_CALLBACK (select_left_action), bcobj);
	g_object_set_data(
	(GObject*)item, "bcext::left_path", g_string_new(bcobj->RightFile->str));
	g_object_set_data((GObject*)item, "bcext::is_dir", GBOOLEAN_TO_POINTER(IsDir));

	g_string_free(MenuStr, TRUE);
	g_string_free(HintStr, TRUE);
	g_string_free(ItemStr, TRUE);

	return item;
}

static BcMenuItem * select_center_mitem(BCompareExt *bcobj)
{
	BcMenuItem *item;

	item = nautilus_menu_item_new("BCompareExt::select_center",
								  "Select Center File",
								  "Remembers selected item for later comparison using Beyond Compare. Right-click another item to start the comparison",
								  "bcomparehalf32" /* icon name */);

	g_signal_connect(item, "activate",
			G_CALLBACK (select_center_action), bcobj);
	g_object_set_data(
	(GObject*)item, "bcext::center_file", g_string_new(bcobj->RightFile->str));
	return item;
}

static BcMenuItem * edit_file_mitem(BCompareExt *bcobj)
{
	BcMenuItem *item;
	GString *MenuStr = g_string_new("");

	if (bcobj->EditMenuType == MENU_SUBMENU)
		g_string_printf(MenuStr, "Edit");
	else
		g_string_printf(MenuStr, "Edit with Beyond Compare");

	item = nautilus_menu_item_new("BCompareExt::edit_file",
								  MenuStr->str,
				  "Edit the file using Beyond Compare",
								  "bcomparefull32" /* icon name */);

	g_signal_connect(item, "activate",
			G_CALLBACK (edit_file_action), bcobj);
	g_object_set_data(
	(GObject*) item, "bcext::edit_file", g_string_new(bcobj->RightFile->str));
	g_string_free(MenuStr, TRUE);
	return item;
}

static BcMenuItem * compare_mitem(
		BCompareExt *bcobj,
		gchar *fileviewer,
		int SelectedCnt)
{
	BcMenuItem *item;
	GString *MenuStr = g_string_new("");
	GString *HintStr = g_string_new("");
	GString *NameStr = g_string_new("");
	gchar *LeftFileName;

	if (strcmp(fileviewer, "") == 0) {
		if (SelectedCnt == 1) {
			LeftFileName = g_path_get_basename(bcobj->LeftFile->str);
			g_string_printf(MenuStr, "Compare to '%s'", LeftFileName);
			g_string_assign(HintStr,
					"Compare selected items using Beyond Compare");
			g_free(LeftFileName);
		}
		else {
			g_string_assign(MenuStr, "Compare");
			g_string_assign(HintStr,
					"Compare selected item with previously selected left item, using Beyond Compare");
		}
	}
	else {
		g_string_assign(MenuStr, fileviewer);
		g_string_printf(HintStr, "Compare files using the '%s' viewer",
				fileviewer);
	}

	g_string_printf(NameStr, "BCompareExt::compare_using %s", fileviewer);

	item = nautilus_menu_item_new(NameStr->str,
								  MenuStr->str,
								  HintStr->str,
								  "bcomparefull32" /* icon name */);
	g_signal_connect(item, "activate",
			G_CALLBACK (compare_action), bcobj);
	g_object_set_data(
	(GObject*)item, "bcext::left_file", g_string_new(bcobj->LeftFile->str));
	g_object_set_data(
	(GObject*)item, "bcext::right_file", g_string_new(bcobj->RightFile->str));
	g_object_set_data((GObject*)item, "bcext::fileviewer", fileviewer);

	g_string_free(MenuStr, TRUE);
	g_string_free(HintStr, TRUE);
	g_string_free(NameStr, TRUE);
	return item;
}

static BcMenuItem * sync_mitem(
		BCompareExt *bcobj,
		int SelectedCnt)
{
	BcMenuItem *item;
	GString *MenuStr = g_string_new("");
	GString *HintStr = g_string_new("");
	gchar *LeftFolderName;


	if (SelectedCnt == 1) {
		LeftFolderName = g_path_get_basename(bcobj->LeftFile->str);
		g_string_printf(MenuStr, "Sync with '%s'", LeftFolderName);
		g_string_assign(HintStr,
				"Sync to previously selected folder");
		g_free(LeftFolderName);
	}
	else {
		g_string_assign(MenuStr, "Sync");
		g_string_assign(HintStr,
				"Sync two selected folders");
	}

	item = nautilus_menu_item_new("BCompareExt::sync",
								  MenuStr->str,
								  HintStr->str,
								  "bcomparefull32" /* icon name */);
	g_signal_connect(item, "activate",
			G_CALLBACK (sync_action), bcobj);
	g_object_set_data(
			(GObject*)item, "bcext::left_folder", g_string_new(bcobj->LeftFile->str));
	g_object_set_data(
		(GObject*)item, "bcext::right_folder", g_string_new(bcobj->RightFile->str));

	g_string_free(MenuStr, TRUE);
	g_string_free(HintStr, TRUE);
	return item;

}

static BcMenuItem * merge_mitem(
		BCompareExt *bcobj,
		int SelectedCnt)
{
	BcMenuItem *item;
	GString *MenuStr = g_string_new("");
	GString *HintStr = g_string_new("");
	gchar *File1, *File2;

	if (SelectedCnt == 1){
		if (bcobj->CenterFile == NULL) {
			File1 = g_path_get_basename(bcobj->LeftFile->str);
			g_string_printf(MenuStr, "Merge with '%s'", File1);
			g_string_assign(HintStr,
				"Merge file with previously selected left file using Beyond Compare");
			g_free(File1);
		}
		else {
			File1 = g_path_get_basename(bcobj->LeftFile->str);
			File2 = g_path_get_basename(bcobj->CenterFile->str);
			g_string_printf(MenuStr, "Merge with '%s', '%s'", File1, File2);
			g_string_assign(HintStr,
				"Merge file with previously selected left and center files using Beyond Compare");
			g_free(File1);
			g_free(File2);
		}
	}
	else if (SelectedCnt == 2){
		if (bcobj->CenterFile == NULL) {
			g_string_assign(MenuStr, "Merge");
			g_string_assign(HintStr,
				"Merge selected files (left, right)");
		}
		else {
			File1 = g_path_get_basename(bcobj->CenterFile->str);
			g_string_printf(MenuStr, "Merge with '%s'", File1);
			g_string_assign(HintStr,
				"Merge selected files (left, right) with previously selected center file");
			g_free(File1);
		}
	}
	else if (SelectedCnt == 3) {
		g_string_assign(MenuStr, "Merge");
		g_string_assign(HintStr,
				"Merge selected files (left, right, center)");
	}
	else return NULL;


	item = nautilus_menu_item_new("BCompareExt::merge",
								  MenuStr->str,
								  HintStr->str,
								  "bcomparefull32" /* icon name */);
	g_signal_connect(item, "activate",
			G_CALLBACK(merge_action), bcobj);
	g_object_set_data(
			(GObject*)item, "bcext::left_file", g_string_new(bcobj->LeftFile->str));
	g_object_set_data(
			(GObject*)item, "bcext::right_file", g_string_new(bcobj->RightFile->str));
	if (bcobj->CenterFile != NULL) {
	g_object_set_data(
			(GObject*)item, "bcext::center_file", g_string_new(bcobj->CenterFile->str));
	}

	g_string_free(MenuStr, TRUE);
	g_string_free(HintStr, TRUE);

	return item;
}


/*************************************************************
 *
 * Menu Item creation
 *
 *************************************************************/

static GList *beyondcompare_create_dir_menus(
		BCompareExt *bcobj,
		int SelectedCnt,
		gboolean FirstIsDir,
		MenuTypes CurrentMenuType)
{
	GList *items = NULL;
	BcMenuItem *item;

	if ((!FirstIsDir) || (SelectedCnt > 2)) return NULL;

	if ((bcobj->LeftIsDir) &&
			(bcobj->LeftFile != NULL) && (bcobj->RightFile != NULL)) {
		if (CurrentMenuType == bcobj->CompareMenuType) {
			item = compare_mitem(bcobj, "", SelectedCnt);
			if (item != NULL) items = g_list_append(items, item);
		}
		if (CurrentMenuType == bcobj->SyncMenuType) {
			item = sync_mitem(bcobj, SelectedCnt);
			if (item != NULL) items = g_list_append(items, item);
		}
	}

	if (SelectedCnt == 1) {
		if (CurrentMenuType == bcobj->CompareMenuType) {
			item = select_left_mitem(bcobj, TRUE);
			if (item != NULL) items = g_list_append(items, item);
		}
	}

	return items;
}

static GList *beyondcompare_create_file_menus(
		BCompareExt *bcobj,
		int SelectedCnt,
		gboolean FirstIsDir,
		MenuTypes CurrentMenuType)
{
	GList *items = NULL;
	BcMenuItem *item;
	NautilusMenu *SubMenu;
	int Cnt;

	if ((FirstIsDir) || (SelectedCnt > 3)) return NULL;

	if ((!bcobj->LeftIsDir) &&
			(bcobj->LeftFile != NULL) && (bcobj->RightFile != NULL)) {
		if (bcobj->MergeMenuType == CurrentMenuType ) {
			item = merge_mitem(bcobj, SelectedCnt);
			if (item != NULL) items = g_list_append(items, item);
		}
		if (SelectedCnt < 3) {
			if (bcobj->CompareMenuType == CurrentMenuType) {
				item = compare_mitem(bcobj, "", SelectedCnt);
				if (item != NULL) items = g_list_append(items, item);
			}
			if (bcobj->CompareUsingMenuType == CurrentMenuType &&
				(bcobj->ViewerCnt > 0)) {

				item = nautilus_menu_item_new("BeyondCompareExt::compareusing",
										"Compare using",
										"Select viewer type for compare",
										"bcomparefull32");
				SubMenu = nautilus_menu_new();
				nautilus_menu_item_set_submenu(item, SubMenu);

				for (Cnt = 0; Cnt < bcobj->ViewerCnt; Cnt++) {
					nautilus_menu_append_item(SubMenu,
						compare_mitem(bcobj, bcobj->Viewers[Cnt], SelectedCnt));
				}
				items = g_list_append(items, item);
			}
		}
	}

	if (SelectedCnt == 1) {
		if (bcobj->CompareMenuType == CurrentMenuType) {
			item = select_left_mitem(bcobj, FALSE);
			if (item != NULL) items = g_list_append(items, item);
			if ((!bcobj->LeftIsDir) && (bcobj->LeftFile != NULL)) {
				item = select_center_mitem(bcobj);
				if (item != NULL) items = g_list_append(items, item);
			}
		}
		if (bcobj->EditMenuType == CurrentMenuType) {
			item = edit_file_mitem(bcobj);
			if (item != NULL) items = g_list_append(items, item);
		}
	}

	return items;
}

/*************************************************************
 *
 * Extension functions for XXXMenuProvider Interface
 *
 *************************************************************/

static GList * beyondcompare_get_file_items(
					NautilusMenuProvider *provider,
					GtkWidget *window,
					GList *files)
{
	BCompareExt *bcobj = (BCompareExt *)provider;
	NautilusMenu *SubMenu;
	BcMenuItem *item;
	GList *ret = NULL;
	GList *tmp = NULL;
	gchar leftfilepath[256];
	gchar centerfilepath[256];
	char *leftfileptr, *centerfileptr;
	FILE *filestrptr;
	int Cnt;
	gboolean FirstIsDir;
	int SelectedCnt;

	if ((provider == NULL) || (files == NULL) || (!bcobj->Enabled)) return NULL;
	g_return_val_if_fail(GTK_IS_WIDGET(window), NULL);

	FirstIsDir =
		file_is_dir(bcobj, nautilus_to_path((NautilusFileInfo *)files->data));

	SelectedCnt = g_list_length(files);
	if (SelectedCnt > 1) {
		for (Cnt = 1; Cnt < g_list_length(files); Cnt++) {
			if (FirstIsDir != file_is_dir(bcobj,
					   nautilus_to_path((NautilusFileInfo *)(g_list_nth_data(files, Cnt))))) {
				return NULL;
			}
		}
	}

	leftfileptr = NULL;
	filestrptr = fopen(bcobj->LeftFileStorage->str, "r");
	if (filestrptr != NULL) {
		leftfileptr = fgets(leftfilepath, 255, filestrptr);
		fclose(filestrptr);
	}

	centerfileptr = NULL;
	filestrptr = fopen(bcobj->CenterFileStorage->str, "r");
	if (filestrptr != NULL) {
		centerfileptr = fgets(centerfilepath, 255, filestrptr);
		fclose(filestrptr);
	}

	if (SelectedCnt == 3) {
		if (bcobj->CenterFile != NULL)
			g_string_free(bcobj->CenterFile, TRUE);
		bcobj->CenterFile = g_string_new(nautilus_to_path(
			(NautilusFileInfo *)g_list_nth_data(files, 2)));
	}
	if (SelectedCnt >= 2) {
		if (bcobj->LeftFile != NULL)
			g_string_free(bcobj->LeftFile, TRUE);
		bcobj->LeftFile = g_string_new(nautilus_to_path(
			(NautilusFileInfo *)g_list_nth_data(files, 0)));

		bcobj->LeftIsDir = FirstIsDir;

		if (bcobj->RightFile != NULL)
			g_string_free(bcobj->RightFile, TRUE);
		bcobj->RightFile = g_string_new(nautilus_to_path(
			(NautilusFileInfo *)g_list_nth_data(files, 1)));
	}
	if (SelectedCnt == 1) {
		if (leftfileptr != NULL) {
			if (bcobj->LeftFile != NULL)
				g_string_free(bcobj->LeftFile, TRUE);
			bcobj->LeftFile = g_string_new(leftfileptr);
			bcobj->LeftIsDir =
			  file_is_dir(bcobj, bcobj->LeftFile->str);
		}

		if (bcobj->RightFile != NULL)
			g_string_free(bcobj->RightFile, TRUE);
		bcobj->RightFile = g_string_new(nautilus_to_path(
			(NautilusFileInfo *)g_list_nth_data(files, 0)));

		if (centerfileptr != NULL) {
			if (bcobj->CenterFile != NULL)
				g_string_free(bcobj->CenterFile, TRUE);
			bcobj->CenterFile = g_string_new(centerfileptr);
		}
	}

	bcobj->Winder = window;
	ret = beyondcompare_create_dir_menus(
			bcobj, SelectedCnt, FirstIsDir, MENU_SUBMENU);
	tmp = beyondcompare_create_file_menus(
			bcobj, SelectedCnt, FirstIsDir, MENU_SUBMENU);
	ret = g_list_concat(ret, tmp);

	if (ret != NULL) {
		item = nautilus_menu_item_new("BeyondCompareExt::Top",
								  "Beyond Compare",
								  "Beyond Compare functions",
								  "bcomparefull32");
		SubMenu = nautilus_menu_new();
		nautilus_menu_item_set_submenu(item, SubMenu);

		for(; ret != NULL; ret = ret->next) {
			nautilus_menu_append_item(
				SubMenu, (BcMenuItem *)(ret->data));
		}

		g_list_free(ret);
		ret = NULL;
		ret = g_list_append(ret, item);
	}

	tmp = beyondcompare_create_dir_menus(
			bcobj, SelectedCnt, FirstIsDir, MENU_MAIN);
	ret = g_list_concat(ret, tmp);
	tmp = beyondcompare_create_file_menus(
			bcobj, SelectedCnt, FirstIsDir, MENU_MAIN);
	ret = g_list_concat(ret, tmp);

	if (bcobj->LeftFile != NULL) g_string_free(bcobj->LeftFile, TRUE);
	if (bcobj->RightFile != NULL) g_string_free(bcobj->RightFile, TRUE);
	if (bcobj->CenterFile != NULL) g_string_free(bcobj->CenterFile, TRUE);

	bcobj->LeftFile = NULL;
	bcobj->RightFile = NULL;
	bcobj->CenterFile = NULL;

	return ret;
}

/*************************************************************
 *
 * Beyond Compare Extension Management Functions
 *
 *************************************************************/

static void
bcompare_ext_init(BCompareExt *object)
{
	GKeyFile *MenuIni;
	GError *gerr = NULL;
	gchar *enb;
	gchar *list;
	const gchar *env;
	gchar configdir[256];
	gchar pathname[256];

	env = g_getenv("HOME");
	g_snprintf(configdir, 256, "%s/.beyondcompare", env);
	if (!g_file_test(configdir, G_FILE_TEST_IS_DIR)) {
		env = g_getenv("XDG_CONFIG_HOME");
		if (env == NULL) {
			env = g_getenv("HOME");
			g_snprintf(configdir, 256, "%s/.config/bcompare", env);
		}
		else g_snprintf(configdir, 256, "%s/bcompare", env);
	}

	object->Enabled = FALSE;
	object->ViewerCnt = 0;
	object->MaskCnt = 0;

	MenuIni = g_key_file_new();
	g_snprintf(pathname, 256, "%s/menu.ini", configdir);
	g_key_file_load_from_file(MenuIni,
				  pathname,
				  (G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS),
				  &gerr);

	gerr = NULL;
	g_key_file_set_list_separator(MenuIni, ',');
	enb = g_key_file_get_value(MenuIni, "ContextMenus",
						"Enabled", &gerr);
	if (enb == NULL) object->Enabled = TRUE;
	else if ((strcmp(enb, "true") == 0) || (strcmp(enb, "TRUE") == 0) ||
			(strcmp(enb, "True") == 0)) {
		object->Enabled = TRUE;
	}

	gerr = NULL;
	object->CompareMenuType = (MenuTypes)g_key_file_get_integer(MenuIni,
						"ContextMenus", "Compare", &gerr);
	if (gerr != NULL) object->CompareMenuType = MENU_MAIN;

	gerr = NULL;
	object->CompareUsingMenuType = (MenuTypes)g_key_file_get_integer(MenuIni,
						"ContextMenus", "CompareUsing", &gerr);
	if (gerr != NULL) object->CompareUsingMenuType = MENU_MAIN;

	gerr = NULL;
	object->MergeMenuType = (MenuTypes)g_key_file_get_integer(MenuIni,
						"ContextMenus", "Merge", &gerr);
	if (gerr != NULL) object->MergeMenuType = MENU_MAIN;

	gerr = NULL;
	object->SyncMenuType = (MenuTypes)g_key_file_get_integer(MenuIni,
						"ContextMenus", "Sync", &gerr);
	if (gerr != NULL) object->SyncMenuType = MENU_MAIN;

	gerr = NULL;
	object->EditMenuType = (MenuTypes)g_key_file_get_integer(MenuIni,
						"ContextMenus", "Edit", &gerr);
	if (gerr != NULL) object->EditMenuType = MENU_MAIN;

	gerr = NULL;
	list = g_key_file_get_string(MenuIni, "ContextMenus","Viewers", &gerr);
	if (list != NULL) {
		object->Viewers = g_strsplit(list, ",", 255);
		while((void *)object->Viewers[object->ViewerCnt] != 0) {
			object->ViewerCnt++;
		}
		g_free(list);
	}

	gerr = NULL;
	list = g_key_file_get_string(
	  MenuIni,"ContextMenus", "ArchiveMasks", &gerr);
	if (list != NULL) {
		object->Masks = g_strsplit(list, ",", 255);
		while((void *)object->Masks[object->MaskCnt] != 0) {
			g_strchug(
			  g_strdelimit(
				object->Masks[object->MaskCnt],"*", ' '));
			object->MaskCnt++;
		}
		g_free(list);
	}

	g_key_file_free(MenuIni);

	object->LeftFile = NULL;
	object->RightFile = NULL;
	object->CenterFile = NULL;

	object->StorageDir = g_string_new("");
	g_string_printf(object->StorageDir, "%s", configdir);

	object->LeftFileStorage = g_string_new("");
	g_string_printf(object->LeftFileStorage, "%s/left_file", configdir);

	object->CenterFileStorage = g_string_new("");
	g_string_printf(object->CenterFileStorage, "%s/center_file", configdir);
}

static void
bcompare_ext_class_init(BCompareExtClass *class)
{
	parent_class = g_type_class_peek_parent(class);
}

/* Interface Init function */
static void
bcompare_menu_provider_init(
		NautilusMenuProviderIface *iface)
{
	iface->get_file_items = beyondcompare_get_file_items;
}

/* Registration function */
static void bcompare_ext_register_type(GTypeModule *module)
{
	static const GTypeInfo info = {
		sizeof (BCompareExtClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) bcompare_ext_class_init,
		NULL,
		NULL,
		sizeof (BCompareExt),
		0,
		(GInstanceInitFunc) bcompare_ext_init,
	};

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) bcompare_menu_provider_init,
		NULL,
		NULL
	};

	type_list[0] = g_type_module_register_type(module,
										G_TYPE_OBJECT,
										"BeyondCompareExt",
										&info, 0);

	g_type_module_add_interface(module,
					 type_list[0],
					 NAUTILUS_TYPE_MENU_PROVIDER,
					 &menu_provider_iface_info);

}

/*************************************************************
 *
 * Nautilus Extension Interface functions
 *
 *************************************************************/

void
nautilus_module_initialize(GTypeModule *module)
{
	bcompare_ext_register_type(module);
}

void
nautilus_module_shutdown(void)
{
 g_message("Shutting down bcompare-ext extension");
}

void
nautilus_module_list_types(const GType **types, gint *n_types)
{
	*types = type_list;
	*n_types = G_N_ELEMENTS(type_list);
}

