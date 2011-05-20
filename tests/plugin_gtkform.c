/*
 * GtkDateEntry widget test - plugin for libgtkform
 * Copyright (C) 2010-2011 Andrea Zagli <azagli@libero.it>
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>

#include <gtk/gtk.h>

#include <libgtkform/form.h>

GtkBuilder *builder;
GtkForm *form;

static void
btn_insert_on_clicked (GtkButton *button,
                        gpointer user_data)
{
	GtkTextBuffer *buf;

	buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (gtk_builder_get_object (builder, "textview1")));
	gtk_text_buffer_set_text (buf, gtk_form_get_sql (form, GTK_FORM_SQL_INSERT), -1);
}

static void
btn_update_on_clicked (GtkButton *button,
                      gpointer user_data)
{
	GtkTextBuffer *buf;

	buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (gtk_builder_get_object (builder, "textview1")));
	gtk_text_buffer_set_text (buf, gtk_form_get_sql (form, GTK_FORM_SQL_UPDATE), -1);
}

int
main (int argc, char **argv)
{
	GtkWidget *w;
	GError *error;

	gtk_init (&argc, &argv);

#ifdef G_OS_WIN32
	gchar *libname = "libgtkmaskedentry-0.dll";
	g_module_open (g_build_filename (g_getenv ("LIBGTKFORM_MODULESDIR"), libname, NULL), G_MODULE_BIND_LAZY);
	libname = g_strdup ("libgtkdateentry-0.dll");
	g_module_open (g_build_filename (g_getenv ("LIBGTKFORM_MODULESDIR"), libname, NULL), G_MODULE_BIND_LAZY);
#endif

	error = NULL;
	builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, "plugin_gtkform.ui", &error);

	gtk_builder_connect_signals (builder, NULL);

	g_signal_connect (gtk_builder_get_object (builder, "button1"), "clicked", G_CALLBACK (btn_insert_on_clicked), NULL);
	g_signal_connect (gtk_builder_get_object (builder, "button2"), "clicked", G_CALLBACK (btn_update_on_clicked), NULL);

	form = gtk_form_new_from_file ("plugin_gtkform.form", builder);

	w = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
	gtk_widget_show (w);

	gtk_main ();

	return 0;
}
