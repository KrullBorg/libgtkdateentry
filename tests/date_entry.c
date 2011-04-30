/*
 * GtkDateEntry widget for GTK+
 *
 * Copyright (C) 2005-2009 Andrea Zagli <azagli@libero.it>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>

#include <gtkdateentry.h>

GtkWidget *window;
GtkWidget *table;
GtkWidget *date;
GtkWidget *separator;
GtkWidget *btnSeparator;
GtkWidget *format;
GtkWidget *btnFormat;
GtkWidget *txtSetStrf;
GtkWidget *txtSetStrfFormat;
GtkWidget *txtSetStrfSep;
GtkWidget *btnSetStrf;
GtkWidget *tbtnEditable;
GtkWidget *tbtnEditableWithCalendar;
GtkWidget *tbtnSensitive;

static void
on_btnSeparator_clicked (GtkButton *button,
                         gpointer user_data)
{
	gtk_date_entry_set_separator (GTK_DATE_ENTRY (date),
	                              gtk_entry_get_text (GTK_ENTRY (separator)));
}

static void
on_btnFormat_clicked (GtkButton *button,
                      gpointer user_data)
{
	const gchar *str_format = gtk_entry_get_text (GTK_ENTRY (format));

	if (!gtk_date_entry_set_format (GTK_DATE_ENTRY (date), str_format))
		{
			g_warning ("Error on gtk_date_entry_set_format (%s).", str_format);
		}
}

static void
on_btnSetStrf_clicked (GtkButton *button,
                       gpointer user_data)
{
	gtk_date_entry_set_date_strf (GTK_DATE_ENTRY (date),
	                              (const gchar *)gtk_entry_get_text (GTK_ENTRY (txtSetStrf)),
                                NULL, 0);
}

static void
on_tbtnEditable_toggled (GtkToggleButton *togglebutton,
                         gpointer user_data)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tbtnEditable)))
		{
			gtk_date_entry_set_editable (GTK_DATE_ENTRY (date), FALSE);
			gtk_button_set_label (GTK_BUTTON (tbtnEditable), "Not Editable");
		}
	else
		{
			gtk_date_entry_set_editable (GTK_DATE_ENTRY (date), TRUE);
			gtk_button_set_label (GTK_BUTTON (tbtnEditable), "Editable");
		}
}

static void
on_tbtnEditableWithCalendar_toggled (GtkToggleButton *togglebutton,
                         gpointer user_data)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tbtnEditableWithCalendar)))
		{
			gtk_date_entry_set_editable_with_calendar (GTK_DATE_ENTRY (date), TRUE);
			gtk_button_set_label (GTK_BUTTON (tbtnEditableWithCalendar), "Editable only from calendar");
		}
	else
		{
			gtk_date_entry_set_editable_with_calendar (GTK_DATE_ENTRY (date), FALSE);
			gtk_button_set_label (GTK_BUTTON (tbtnEditableWithCalendar), "Editable also from calendar");
		}
}

static void
on_tbtnSensitive_toggled (GtkToggleButton *togglebutton,
                          gpointer user_data)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tbtnSensitive)))
		{
			gtk_widget_set_sensitive (date, FALSE);
			gtk_button_set_label (GTK_BUTTON (tbtnSensitive), "Not Sensitive");
		}
	else
		{
			gtk_widget_set_sensitive (date, TRUE);
			gtk_button_set_label (GTK_BUTTON (tbtnSensitive), "Sensitive");
		}
}

int
main (int argc, char **argv)
{
	GtkWidget *label;

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title (GTK_WINDOW (window), "GtkDateEntry Test");

	g_signal_connect (G_OBJECT (window), "destroy",
		        G_CALLBACK (gtk_main_quit), NULL);

	table = gtk_table_new (5, 3, FALSE);
	gtk_container_add (GTK_CONTAINER (window), table);
	gtk_widget_show (table);

	label = gtk_label_new ("GtkDateEntry");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (label);

	date = gtk_date_entry_new (NULL, NULL, TRUE);
	gtk_table_attach (GTK_TABLE (table), date, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (date);

	label = gtk_label_new ("Separator");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (label);

	separator = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (separator), "/");
	gtk_entry_set_max_length (GTK_ENTRY (separator), 1);
	gtk_table_attach (GTK_TABLE (table), separator, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (separator);

	btnSeparator = gtk_button_new_with_label ("Set separator");
	gtk_table_attach (GTK_TABLE (table), btnSeparator, 2, 3, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (btnSeparator);

	g_signal_connect (G_OBJECT (btnSeparator), "clicked",
		      G_CALLBACK (on_btnSeparator_clicked), NULL);

	label = gtk_label_new ("Format");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (label);

	format = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (format), "dmY");
	gtk_entry_set_max_length (GTK_ENTRY (format), 3);
	gtk_table_attach (GTK_TABLE (table), format, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (format);

	btnFormat = gtk_button_new_with_label ("Set format");
	gtk_table_attach (GTK_TABLE (table), btnFormat, 2, 3, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (btnFormat);

	g_signal_connect (G_OBJECT (btnFormat), "clicked",
	                  G_CALLBACK (on_btnFormat_clicked), NULL);

	label = gtk_label_new ("Strf");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (label);

	txtSetStrf = gtk_entry_new ();
	gtk_table_attach (GTK_TABLE (table), txtSetStrf, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (txtSetStrf);

	btnSetStrf = gtk_button_new_with_label ("set_date_strf");
	gtk_table_attach (GTK_TABLE (table), btnSetStrf, 2, 3, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (btnSetStrf);

	g_signal_connect (G_OBJECT (btnSetStrf), "clicked",
	                  G_CALLBACK (on_btnSetStrf_clicked), NULL);

	tbtnEditable = gtk_toggle_button_new_with_label ("Editable");
	gtk_table_attach (GTK_TABLE (table), tbtnEditable, 0, 1, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (tbtnEditable);

	g_signal_connect (G_OBJECT (tbtnEditable), "toggled",
	                  G_CALLBACK (on_tbtnEditable_toggled), NULL);

	tbtnEditableWithCalendar = gtk_toggle_button_new_with_label ("Editable also from calendar");
	gtk_table_attach (GTK_TABLE (table), tbtnEditableWithCalendar, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (tbtnEditableWithCalendar);

	g_signal_connect (G_OBJECT (tbtnEditableWithCalendar), "toggled",
	                  G_CALLBACK (on_tbtnEditableWithCalendar_toggled), NULL);

	tbtnSensitive = gtk_toggle_button_new_with_label ("Sensitive");
	gtk_table_attach (GTK_TABLE (table), tbtnSensitive, 2, 3, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show (tbtnSensitive);

	g_signal_connect (G_OBJECT (tbtnSensitive), "toggled",
	                  G_CALLBACK (on_tbtnSensitive_toggled), NULL);

	gtk_widget_show (window);
  
	gtk_main ();

	return 0;
}
