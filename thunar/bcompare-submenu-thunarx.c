/*----------------------------------------------------------------------\
| Beyond Compare (tm)													|
| Copyright (c) 1996-2015 Scooter Software, Inc.						|
| All rights reserved.							www.scootersoftware.com	|
\----------------------------------------------------------------------*/
#include "bcompare-submenu-thunarx.h"

#include <gtk/gtkmenu.h>
#include <gtk/gtkseparatormenuitem.h>
#include <gtk/gtkimagemenuitem.h>


struct _bcompare_submenu_action {
  GtkAction parent;
  GList* actions;
};

struct _bcompare_submenu_action_class {
  GtkActionClass parent_class;
};

static GObjectClass* parent_class = NULL;

GType
bcompare_submenu_action_get_type(void);

static void
submenu_action_class_init(
	bcompare_submenu_action_class* klass); 

static void 
submenu_action_init(
	BCompareSubMenu * action);


 
BCompareSubMenu * 
bcompare_submenu_new(
	const gchar *name, 
	const gchar *label, 
	const gchar *tooltip,
	const gchar *stock_id) 
{
  BCompareSubMenu * submenu;
  
  submenu = g_object_new(BCOMPARE_TYPE_SUBMENU_ACTION,
                        "name", name,
                        "label", label,
                        "tooltip", tooltip,
                        "stock_id", stock_id,
                        NULL);
  
  return submenu;
}

void 
bcompare_submenu_action_add(
	BCompareSubMenu * submenu, 
	GtkAction* subaction) 
{
  submenu->actions = g_list_append(submenu->actions, subaction);
}


GType
bcompare_submenu_action_get_type() 
{
  static GType type = 0;

  if(G_UNLIKELY(type == 0)) {
    const GTypeInfo our_info = {
      sizeof (bcompare_submenu_action_class),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) submenu_action_class_init,
      NULL,
      NULL, /* class_data */
      sizeof (BCompareSubMenu),
      0, /* n_preallocs */
      (GInstanceInitFunc)submenu_action_init,
      NULL,
    };

    type = g_type_register_static(
		GTK_TYPE_ACTION, "bcompare_submenu_action", &our_info, 0);
  }

  return type;
}

static GtkWidget *
create_menu_item(
	GtkAction* action) 
{
  GtkWidget* menu;
  GtkWidget* menu_item;
  GList* actions = BCOMPARE_SUBMENU_ACTION(action)->actions;

  menu = gtk_menu_new();
  
  while(actions) {
    GtkAction* a = GTK_ACTION(actions->data);
    
    if(a != NULL) {
      menu_item = gtk_action_create_menu_item(a);
      gtk_widget_show(menu_item);
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    } else {
      GtkWidget* s = gtk_separator_menu_item_new();
      gtk_widget_show(s);
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), s);
    }
    
    actions = g_list_next(actions);
  }

  gtk_widget_show(menu);

  menu_item = GTK_ACTION_CLASS(parent_class)->create_menu_item(action);

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);

  gtk_widget_show(menu_item);

  return menu_item;
}

static void
submenu_action_class_init(
	bcompare_submenu_action_class * klass) 
{
  GtkActionClass* action_class = GTK_ACTION_CLASS(klass);
  GObjectClass* gobject_class;

  gobject_class = G_OBJECT_CLASS(klass);

  parent_class = g_type_class_peek_parent(klass);

  action_class->menu_item_type = GTK_TYPE_IMAGE_MENU_ITEM;
  action_class->create_menu_item = create_menu_item;
}

static void 
submenu_action_init(
	BCompareSubMenu * action) 
{
}






