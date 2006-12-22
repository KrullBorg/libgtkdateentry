/*
 * GtkDateEntry widget for GTK+
 *
 * Copyright (C) 2005-2006 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GTK_DATE_ENTRY_H__
#define __GTK_DATE_ENTRY_H__

#include <time.h>

#include <gdk/gdk.h>
#include <gtk/gtkhbox.h>

G_BEGIN_DECLS


#define GTK_TYPE_DATE_ENTRY           (gtk_date_entry_get_type ())
#define GTK_DATE_ENTRY(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_DATE_ENTRY, GtkDateEntry))
#define GTK_DATE_ENTRY_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_DATE_ENTRY, GtkDateEntry))
#define GTK_IS_DATE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_DATE_ENTRY))
#define GTK_IS_DATE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_DATE_ENTRY))
#define GTK_DATE_ENTRY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_DATE_ENTRY, GtkDateEntry))


typedef struct _GtkDateEntry GtkDateEntry;
typedef struct _GtkDateEntryClass GtkDateEntryClass;


struct _GtkDateEntry
{
  GtkHBox hbox;

  GtkWidget *day,
            *btnCalendar,
            *wCalendar,
            *calendar;

  gchar separator,
        *format;
};

struct _GtkDateEntryClass
{
  GtkHBoxClass parent_class;
};


GType gtk_date_entry_get_type (void) G_GNUC_CONST;

GtkWidget *gtk_date_entry_new (const gchar *format,
                               const gchar separator,
                               gboolean calendar_button_is_visible);

void gtk_date_entry_set_separator (GtkDateEntry *date,
                                   const gchar separator);
void gtk_date_entry_set_format (GtkDateEntry *date,
                                const gchar *format);

const gchar *gtk_date_entry_get_text (GtkDateEntry *date);
const gchar *gtk_date_entry_get_strf (GtkDateEntry *date,
                                      const gchar *format,
                                      const gchar separator);

const struct tm *gtk_date_entry_get_tm (GtkDateEntry *date);
const GDate *gtk_date_entry_get_gdate (GtkDateEntry *date);

gboolean gtk_date_entry_set_date_strf (GtkDateEntry *date,
                                       const gchar *str,
                                       const gchar *format,
                                       const gchar separator);

void gtk_date_entry_set_date_tm (GtkDateEntry *date,
                                 const struct tm tmdate);
void gtk_date_entry_set_date_gdate (GtkDateEntry *date,
                                    const GDate *gdate);

gboolean gtk_date_entry_is_valid (GtkDateEntry *date);

void gtk_date_entry_set_editable (GtkDateEntry *date,
                                  gboolean is_editable);

void gtk_date_entry_set_calendar_button_visible (GtkDateEntry *date,
                                                 gboolean is_visible);


G_END_DECLS

#endif /* __GTK_DATE_ENTRY_H__ */
