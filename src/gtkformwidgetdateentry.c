/*
 * Copyright (C) 2010-2011 Andrea Zagli <azagli@libero.it>
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

#include <gtk/gtk.h>

#include "gtkdateentry.h"
#include "gtkformwidgetdateentry.h"

enum
{
	PROP_0
};

static void gtk_form_widget_date_entry_class_init (GtkFormWidgetDateEntryClass *klass);
static void gtk_form_widget_date_entry_init (GtkFormWidgetDateEntry *gtk_form_widget_date_entry);

static void gtk_form_widget_date_entry_set_property (GObject *object,
                                      guint property_id,
                                      const GValue *value,
                                      GParamSpec *pspec);
static void gtk_form_widget_date_entry_get_property (GObject *object,
                                      guint property_id,
                                      GValue *value,
                                      GParamSpec *pspec);

#define GTK_FORM_WIDGET_DATE_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_FORM_TYPE_WIDGET_DATE_ENTRY, GtkFormWidgetDateEntryPrivate))

typedef struct _GtkFormWidgetDateEntryPrivate GtkFormWidgetDateEntryPrivate;
struct _GtkFormWidgetDateEntryPrivate
	{
		gpointer foo;
	};


G_DEFINE_TYPE (GtkFormWidgetDateEntry, gtk_form_widget_date_entry, TYPE_GTK_FORM_WIDGET)

static void
gtk_form_widget_date_entry_class_init (GtkFormWidgetDateEntryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkFormWidgetClass *widget_class = GTK_FORM_WIDGET_CLASS (klass);

	object_class->set_property = gtk_form_widget_date_entry_set_property;
	object_class->get_property = gtk_form_widget_date_entry_get_property;

	widget_class->get_value_stringify = gtk_form_widget_date_entry_get_value_stringify;
	widget_class->set_value_stringify = gtk_form_widget_date_entry_set_value_stringify;
	widget_class->set_editable = gtk_form_widget_date_entry_set_editable;

	g_type_class_add_private (object_class, sizeof (GtkFormWidgetDateEntryPrivate));
}

static void
gtk_form_widget_date_entry_init (GtkFormWidgetDateEntry *gtk_form_widget_date_entry)
{
}

/**
 * gtk_form_widget_date_entry_new:
 *
 * Returns: the newly created #GtkFormWidgetDateEntry.
 */
GtkFormWidget
*gtk_form_widget_date_entry_new ()
{
	return g_object_new (gtk_form_widget_date_entry_get_type (), NULL);
}

/**
 * gtk_form_widget_date_entry_get_value_stringify:
 * @widget:
 *
 */
gchar
*gtk_form_widget_date_entry_get_value_stringify (GtkFormWidget *fwidget)
{
	GtkFormWidgetDateEntryPrivate *priv = GTK_FORM_WIDGET_DATE_ENTRY_GET_PRIVATE (fwidget);

	GtkWidget *w = gtk_form_widget_get_widget (fwidget);

	return g_strdup (gtk_date_entry_get_strf (GTK_DATE_ENTRY (w), "Ymd HMS", "-", ":"));
}

/**
 * gtk_form_widget_date_entry_set_value_stringify:
 * @fwidget:
 * @value:
 *
 */
gboolean
gtk_form_widget_date_entry_set_value_stringify (GtkFormWidget *fwidget, const gchar *value)
{
	gboolean ret = FALSE;
	GtkWidget *w;

	g_object_get (G_OBJECT (fwidget),
	              "widget", &w,
	              NULL);

	gtk_entry_set_text (GTK_ENTRY (w), value);

	ret = TRUE;

	return ret;
}

/**
 * gtk_form_widget_set_editable:
 * @fwidget:
 * @editable:
 *
 */
void
gtk_form_widget_date_entry_set_editable (GtkFormWidget *fwidget, gboolean editable)
{
	GtkWidget *w;

	g_object_get (G_OBJECT (fwidget),
	              "widget", &w,
	              NULL);

	gtk_editable_set_editable (GTK_EDITABLE (w), editable);
}

/* PRIVATE */
static void
gtk_form_widget_date_entry_set_property (GObject *object,
                                guint property_id,
                                const GValue *value,
                                GParamSpec *pspec)
{
	GtkFormWidgetDateEntry *widget_entry = (GtkFormWidgetDateEntry *)object;

	GtkFormWidgetDateEntryPrivate *priv = GTK_FORM_WIDGET_DATE_ENTRY_GET_PRIVATE (widget_entry);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
gtk_form_widget_date_entry_get_property (GObject *object,
                                guint property_id,
                                GValue *value,
                                GParamSpec *pspec)
{
	GtkFormWidgetDateEntry *widget_entry = (GtkFormWidgetDateEntry *)object;

	GtkFormWidgetDateEntryPrivate *priv = GTK_FORM_WIDGET_DATE_ENTRY_GET_PRIVATE (widget_entry);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
