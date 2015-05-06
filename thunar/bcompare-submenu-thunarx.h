/*----------------------------------------------------------------------\
| Beyond Compare (tm)													|
| Copyright (c) 1996-2015 Scooter Software, Inc.						|
| All rights reserved.							www.scootersoftware.com	|
\----------------------------------------------------------------------*/
#ifndef __submenu_action__
#define __submenu_action__

#include <gtk/gtkaction.h>

G_BEGIN_DECLS

#define BCOMPARE_TYPE_SUBMENU_ACTION \
	(bcompare_submenu_action_get_type ())

#define BCOMPARE_SUBMENU_ACTION(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), \
	BCOMPARE_TYPE_SUBMENU_ACTION, \
	BCompareSubMenu))

#define BCOMPARE_SUBMENU_ACTION_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), \
	BCOMPARE_TYPE_SUBMENU_ACTION, \
	bcompare_submenu_action_class))

#define BCOMPARE_IS_SUBMENU_ACTION(obj) \
        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BCOMPARE_TYPE_SUBMENU_ACTION))

#define BCOMPARE_IS_SUBMENU_ACTION_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((obj), BCOMPARE_TYPE_SUBMENU_ACTION))

#define BCOMPARE_SUBMENU_ACTION_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS((obj), \
	BCOMPARE_TYPE_SUBMENU_ACTION, \
	bcompare_submenu_action_class))

typedef struct _bcompare_submenu_action BCompareSubMenu;
typedef struct _bcompare_submenu_action_class bcompare_submenu_action_class;

BCompareSubMenu * 
bcompare_submenu_new(
	const gchar* name, 
	const gchar* label, 
	const gchar* tooltip, 
	const gchar* stock_id);

void 
bcompare_submenu_action_add(
	BCompareSubMenu * submenu, 
	GtkAction* subaction); 

GType	
bcompare_submenu_action_get_type();

G_END_DECLS

#endif /* submenu_action */
