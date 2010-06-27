/*
 * Copyright (C) 2010 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __LIBGTK_FORM_WIDGET_ENTRY_H__
#define __LIBGTK_FORM_WIDGET_ENTRY_H__

#include <libgtkform/widget.h>


G_BEGIN_DECLS


#define GTK_FORM_TYPE_WIDGET_DATE_ENTRY                 (gtk_form_widget_date_entry_get_type ())
#define GTK_FORM_WIDGET_DATE_ENTRY(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_FORM_TYPE_WIDGET_DATE_ENTRY, GtkFormWidgetDateEntry))
#define GTK_FORM_WIDGET_DATE_ENTRY_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_FORM_TYPE_WIDGET_DATE_ENTRY, GtkFormWidgetDateEntryClass))
#define GTK_FORM_IS_WIDGET_DATE_ENTRY(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_FORM_TYPE_WIDGET_DATE_ENTRY))
#define GTK_FORM_IS_WIDGET_DATE_ENTRY_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_FORM_TYPE_WIDGET_DATE_ENTRY))
#define GTK_FORM_WIDGET_DATE_ENTRY_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_FORM_TYPE_WIDGET_DATE_ENTRY, GtkFormWidgetDateEntryClass))


typedef struct _GtkFormWidgetDateEntry GtkFormWidgetDateEntry;
typedef struct _GtkFormWidgetDateEntryClass GtkFormWidgetDateEntryClass;

struct _GtkFormWidgetDateEntry
	{
		GtkFormWidget parent;
	};

struct _GtkFormWidgetDateEntryClass
	{
		GtkFormWidgetClass parent_class;
	};

GType gtk_form_widget_date_entry_get_type (void) G_GNUC_CONST;

GtkFormWidget *gtk_form_widget_date_entry_new (void);

gchar *gtk_form_widget_date_entry_get_value_stringify (GtkFormWidget *widget);

gboolean gtk_form_widget_date_entry_set_value_stringify (GtkFormWidget *fwidget, const gchar *value);

void gtk_form_widget_date_entry_set_editable (GtkFormWidget *fwidget, gboolean editable);


G_END_DECLS


#endif /* __LIBGTK_FORM_WIDGET_ENTRY_H__ */
