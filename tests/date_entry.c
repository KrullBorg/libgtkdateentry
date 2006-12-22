#include <gtk/gtk.h>

#include "gtkdateentry.h"

GtkWidget *window,
          *table,
          *date,
          *separator,
          *btnSeparator,
          *format,
          *btnFormat,
					*txtSetStrf,
					*txtSetStrfFormat,
					*txtSetStrfSep,
					*btnSetStrf;

static void
on_btnSeparator_clicked (GtkButton *button,
                         gpointer user_data)
{
	gtk_date_entry_set_separator (GTK_DATE_ENTRY (date),
	                              (const gchar)(gtk_entry_get_text (GTK_ENTRY (separator))[0]));
}

static void
on_btnFormat_clicked (GtkButton *button,
                      gpointer user_data)
{
	gtk_date_entry_set_format (GTK_DATE_ENTRY (date),
	                           (const gchar *)gtk_entry_get_text (GTK_ENTRY (format)));
}

static void
on_btnSetStrf_clicked (GtkButton *button,
                       gpointer user_data)
{
	gtk_date_entry_set_date_strf (GTK_DATE_ENTRY (date),
	                              (const gchar *)gtk_entry_get_text (GTK_ENTRY (txtSetStrf)),
                                NULL, 0);
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

  table = gtk_table_new (4, 3, FALSE);
  gtk_container_add (GTK_CONTAINER (window), table);
  gtk_widget_show (table);

	label = gtk_label_new ("GtkDateEntry");
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_widget_show (label);

  date = gtk_date_entry_new ("dmY", '/', TRUE);
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

	gtk_widget_show (window);
  
  gtk_main ();
  
  return 0;
}
