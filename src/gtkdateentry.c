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

#include <string.h>

#include <gdk/gdkkeysyms.h>

#include <gtk/gtkhbox.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkcalendar.h>

#include <gtkmaskedentry.h>

#include "gtkdateentry.h"

static void gtk_date_entry_class_init (GtkDateEntryClass *klass);
static void gtk_date_entry_init (GtkDateEntry *date);

static void gtk_date_entry_change_mask (GtkDateEntry *date);

static void hide_popup (GtkWidget *date);
static gboolean popup_grab_on_window (GdkWindow *window,
                                      guint32 activate_time);
static gint delete_popup (GtkWidget *widget,
                          gpointer data);
static gint key_press_popup (GtkWidget *widget,
                             GdkEventKey *event,
                             gpointer data);
static gint button_press_popup (GtkWidget *widget,
                                GdkEventButton *event,
                                gpointer user_data);
static void btnCalendar_on_toggled (GtkToggleButton *togglebutton,
                                    gpointer user_data);
static void calendar_on_day_selected (GtkCalendar *calendar,
                                      gpointer user_data);
static void calendar_on_day_selected_double_click (GtkCalendar *calendar,
                                                   gpointer user_data);

static GtkWidgetClass *parent_class = NULL;

GType
gtk_date_entry_get_type (void)
{
	static GType date_type = 0;

	if (!date_type)
		{
			static const GTypeInfo date_info =
			{
				sizeof (GtkDateEntryClass),
				NULL,		/* base_init */
				NULL,		/* base_finalize */
				(GClassInitFunc) gtk_date_entry_class_init,
				NULL,		/* class_finalize */
				NULL,		/* class_data */
				sizeof (GtkDateEntry),
				0,		/* n_preallocs */
				(GInstanceInitFunc) gtk_date_entry_init,
			};

			date_type = g_type_register_static (GTK_TYPE_HBOX, "GtkDateEntry", &date_info, 0);
		}

	return date_type;
}

static void
gtk_date_entry_class_init (GtkDateEntryClass *class)
{
	GtkWidgetClass *widget_class;

	widget_class = (GtkWidgetClass*) class;
	parent_class = g_type_class_peek_parent (class);
}

static void
gtk_date_entry_init (GtkDateEntry *date)
{
	GtkWidget *arrow;

	/* TO DO
	 * read separator and format from locale settings
	 */
	date->separator = '/';
	date->format = g_strdup ("dmY");

	date->day = gtk_masked_entry_new ();
	gtk_date_entry_change_mask (date);
	gtk_box_pack_start (GTK_BOX (date), date->day, TRUE, TRUE, 0);
	gtk_widget_show (date->day);

	date->btnCalendar = gtk_toggle_button_new ();
	gtk_box_pack_start (GTK_BOX (date), date->btnCalendar, FALSE, FALSE, 0);
	gtk_widget_set_no_show_all (date->btnCalendar, TRUE);
	gtk_widget_show (date->btnCalendar);

	g_signal_connect (G_OBJECT (date->btnCalendar), "toggled",
										G_CALLBACK (btnCalendar_on_toggled), (gpointer)date);

	arrow = (GtkWidget *)gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_NONE);
	gtk_container_add (GTK_CONTAINER (date->btnCalendar), arrow);
	gtk_widget_show (arrow);

	date->wCalendar = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_window_set_resizable (GTK_WINDOW (date->wCalendar), FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (date->wCalendar), 3);

	gtk_widget_set_events (date->wCalendar,
	                       gtk_widget_get_events (date->wCalendar) | GDK_KEY_PRESS_MASK);

	g_signal_connect (date->wCalendar, "delete_event",
	                  G_CALLBACK (delete_popup), date);
	g_signal_connect (date->wCalendar, "key_press_event",
	                  G_CALLBACK (key_press_popup), date);
	g_signal_connect (date->wCalendar, "button_press_event",
	                  G_CALLBACK (button_press_popup), date);

	date->calendar = gtk_calendar_new ();
	gtk_container_add (GTK_CONTAINER (date->wCalendar), date->calendar);
	gtk_widget_show (date->calendar);

	g_signal_connect (G_OBJECT (date->calendar), "day-selected",
										G_CALLBACK (calendar_on_day_selected), (gpointer)date);
	g_signal_connect (G_OBJECT (date->calendar), "day-selected-double-click",
										G_CALLBACK (calendar_on_day_selected_double_click), (gpointer)date);
}

/**
 * gtk_date_entry_new:
 * @format: a #gchar which is the date's format.
 * Possible values are:
 * - d: day of the month with leading zeroes
 * - m: month of the year with leading zeroes
 * - Y: year with century.
 * @separator: a #gchar that represents the separator between day, month and year.
 * @calendar_button_is_visible: if calendar button is visible.
 *
 * Creates a new #GtkDateEntry.
 *
 * Return value: The newly created #GtkDateEntry widget.
 **/
GtkWidget
*gtk_date_entry_new (const gchar *format, const gchar separator, gboolean calendar_button_is_visible)
{
	GtkWidget *w = GTK_WIDGET (g_object_new (gtk_date_entry_get_type (), NULL));

	gtk_date_entry_set_format (GTK_DATE_ENTRY (w), format);
	gtk_date_entry_set_separator (GTK_DATE_ENTRY (w), separator);

	gtk_date_entry_set_calendar_button_visible (GTK_DATE_ENTRY (w), calendar_button_is_visible);

	return w;
}

/**
 * gtk_date_entry_set_separator:
 * @date: a #GtkDateEntry.
 * @separator: a #gchar that represents the separator between day, month and year.
 *
 * Set the separator between day, month and year.
 **/
void
gtk_date_entry_set_separator (GtkDateEntry *date, const gchar separator)
{
	date->separator = separator;
	gtk_date_entry_change_mask (date);
}

/**
 * gtk_date_entry_set_format:
 * @date: a #GtkDateEntry.
 * @format: a #gchar which is the date's format.
 *
 * Sets the date's format.
 **/
void
gtk_date_entry_set_format (GtkDateEntry *date, const gchar *format)
{
	gint i;
	gboolean d, m, y;

	if (strlen (format) != 3)
		{
			return;
		}

	for (i = 0; i < 3; i++)
		{
			switch (format[i])
				{
					case 'd':
						if (d) return;
						d = TRUE;
						break;

					case 'm':
						if (m) return;
						m = TRUE;
						break;

					case 'Y':
						if (y) return;
						y = TRUE;
						break;

					default:
						return;
				}
		}
	if (!d || !m || !y) return; 

  date->format = g_strdup (format);
	gtk_date_entry_change_mask (date);
}

/**
 * gtk_date_entry_get_text:
 * @date: a #GtkDateEntry.
 *
 * Returns the @date's content as is.
 *
 * Return value: A pointer to the content of the widget as is.
 **/
const gchar
*gtk_date_entry_get_text (GtkDateEntry *date)
{
  return gtk_entry_get_text (GTK_ENTRY (date->day));
}

/**
 * gtk_date_entry_get_strf:
 * @date: a #GtkDateEntry.
 * @format: a #gchar which is the date's format.
 * @separator: a #gchar which is the separator between day, month and year.
 *
 * Return value: A pointer to the content of the widget formatted as 
 * specified in @format with @separator.
 **/
const gchar
*gtk_date_entry_get_strf (GtkDateEntry *date,
                          const gchar *format,
                          const gchar separator)
{
	gchar *fmt, sep, *ret = "";
	gint i;
	GDate *gdate = (GDate *)gtk_date_entry_get_gdate (date);

	if (format == NULL)
		{
			fmt = date->format;
		}
	else
		{
			fmt = g_strdup (format);
		}
	if (separator == 0)
		{
			sep = date->separator;
		}
	else
		{
			sep = (gchar)separator;
		}

	for (i = 0; i < 3; i++)
		{
			switch (fmt[i])
				{
					case 'd':
						ret = g_strjoin (NULL, ret, g_strdup_printf ("%02d", (int)g_date_get_day (gdate)), NULL);
						break;

					case 'm':
						ret = g_strjoin (NULL, ret, g_strdup_printf ("%02d", (int)g_date_get_month (gdate)), NULL);
						break;

					case 'Y':
						ret = g_strjoin (NULL, ret, g_strdup_printf ("%04d", (int)g_date_get_year (gdate)), NULL);
						break;
				}

			if (i < 2)
				{
					ret = g_strjoin (NULL, ret, g_strdup_printf ("%c", sep), NULL);
				}
		}

	return (const gchar *)ret;
}

/**
 * gtk_date_entry_get_tm:
 * @date: a #GtkDateEntry
 *
 * Returns the @date's content as a tm struct.
 *
 * Return value: the @date's content as a tm struct.
 */
const struct tm
*gtk_date_entry_get_tm (GtkDateEntry *date)
{
	struct tm tm;

	const GDate *gdate = gtk_date_entry_get_gdate (date);

	if (gdate == NULL) return NULL;

	g_date_to_struct_tm (gdate, &tm);

	return (const struct tm *)g_memdup ((gconstpointer)&tm, sizeof (struct tm));
}

/**
 * gtk_date_entry_get_gdate:
 * @date: a #GtkDateEntry.
 *
 * Returns the @date's content as a #GDate.
 *
 * Return value: the @date's content as a #GDate.
 */
const GDate
*gtk_date_entry_get_gdate (GtkDateEntry *date)
{
	gint i, pos = 0, val;
	gchar *txt;
	GDate *gdate;

	txt = (gchar *)gtk_entry_get_text (GTK_ENTRY (date->day));
	gdate = g_date_new ();

	for (i = 0; i < 3; i++)
		{
			switch (date->format[i])
				{
					case 'd':
						val = atol (g_strndup (txt + pos, 2));
						if (g_date_valid_day ((GDateDay)val))
							{
								g_date_set_day (gdate, (GDateDay)val);
								pos += 3;
							}
						else
							{
								return NULL;
							}
						break;

					case 'm':
						val = atol (g_strndup (txt + pos, 2));
						if (g_date_valid_month ((GDateMonth)val))
							{
								g_date_set_month (gdate, (GDateMonth)val);
								pos += 3;
							}
						else
							{
								return NULL;
							}
						break;

					case 'Y':
						val = atol (g_strndup (txt + pos, 4));
						if (g_date_valid_year ((GDateYear)val))
							{
								g_date_set_year (gdate, (GDateYear)val);
								pos += 5;
							}
						else
							{
								return NULL;
							}
						break;
				}
		}

	return (const GDate *)gdate;
}

/**
 * gtk_date_entry_set_date_strf:
 * @date: a #GtkDateEntry.
 * @str: a #gchar which is the content to set.
 * @format: a #gchar which is the date's format.
 * @separator: a #gchar which is the separator between day, month and year.
 *
 * Sets @date's content from the @str string and based on @format and @separator, 
 * if it's a valid date.
 *
 * Return value: TRUE if @date's content is setted.
 */
gboolean
gtk_date_entry_set_date_strf (GtkDateEntry *date,
                              const gchar *str,
                              const gchar *format,
                              const gchar separator)
{
	gchar *fmt;
	gchar sep;
	GDateDay day;
	GDateMonth month;
	GDateYear year;
	gint i;
	gint pos = 0;

	if (format == NULL)
		{
			fmt = date->format;
		}
	else
		{
			fmt = g_strdup (format);
		}
	if (separator == 0)
		{
			sep = date->separator;
		}
	else
		{
			sep = (gchar)separator;
		}

	for (i = 0; i < 3; i++)
		{
			switch (fmt[i])
				{
					case 'd':
						if (!g_date_valid_day ((GDateDay)atol (g_strndup (str + pos, 2)))) return FALSE;
						/*txt = g_strjoin (NULL, txt, g_strndup (str + pos, 2), NULL);*/
						day = (GDateDay)atol (g_strndup (str + pos, 2));
						pos += 3;
						break;

					case 'm':
						if (!g_date_valid_month ((GDateMonth)atol (g_strndup (str + pos, 2)))) return FALSE;
						/*txt = g_strjoin (NULL, txt, g_strndup (str + pos, 2), NULL);*/
						month = (GDateMonth)atol (g_strndup (str + pos, 2));
						pos += 3;
						break;

					case 'Y':
						if (!g_date_valid_year ((GDateYear)atol (g_strndup (str + pos, 4)))) return FALSE;
						/*txt = g_strjoin (NULL, txt, g_strndup (str + pos, 4), NULL);*/
						year = (GDateYear)atol (g_strndup (str + pos, 4));
						pos += 5;
						break;
				}

			/*if (i < 2)
				{
					txt = g_strjoin (NULL, txt, g_strdup_printf ("%c", sep), NULL);
				}*/
		}

	/*gtk_entry_set_text (GTK_ENTRY (date->day), txt);*/
	gtk_date_entry_set_date_gdate (date, g_date_new_dmy (day, month, year));
	return TRUE;
}

/**
 * gtk_date_entry_set_date_tm:
 * @date: a #GtkDateEntry.
 * @tmdate: a tm struct from which set @date's content.
 *
 * Sets @date's content from the tm struct @tmdate.
 **/
void
gtk_date_entry_set_date_tm (GtkDateEntry *date, const struct tm tmdate)
{
	GDate *gdate = g_date_new_dmy ((GDateDay)tmdate.tm_mday,
	                               (GDateMonth)tmdate.tm_mon + 1,
	                               (GDateYear)tmdate.tm_year + 1900);
	gtk_date_entry_set_date_gdate (date, (const GDate *)gdate);
}

/**
 * gtk_date_entry_set_date_gdate:
 * @date: a #GtkDateEntry.
 * @gdate: a #GDate from which set @date's content.
 *
 * Sets @date's content from a @gdate.
 **/
void
gtk_date_entry_set_date_gdate (GtkDateEntry *date, const GDate *gdate)
{
	gint i;
	gchar *txt = "";

	for (i = 0; i < 3; i++)
		{
			switch (date->format[i])
				{
					case 'd':
						txt = g_strjoin (NULL, txt, g_strdup_printf ("%02d", g_date_get_day (gdate)), NULL);
						break;

					case 'm':
						txt = g_strjoin (NULL, txt, g_strdup_printf ("%02d", g_date_get_month (gdate)), NULL);
						break;

					case 'Y':
						txt = g_strjoin (NULL, txt, g_strdup_printf ("%04d", g_date_get_year (gdate)), NULL);
						break;
				}

			if (i < 2)
				{
					txt = g_strjoin (NULL, txt, g_strdup_printf ("%c", date->separator), NULL);
				}
		}

	gtk_entry_set_text (GTK_ENTRY (date->day), txt);
}

/**
 * gtk_date_entry_is_valid:
 * @date: a #GtkDateEntry.
 *
 * Checks if @date's content is a valid date.
 *
 * Return value: TRUE if @date's content is a valid date.
 **/
gboolean
gtk_date_entry_is_valid (GtkDateEntry *date)
{
	const GDate *gdate = gtk_date_entry_get_gdate (date);
	if (gdate != NULL)
		{
			return g_date_valid (gdate);
		}
	else
		{
			return FALSE;
		}
}

/**
 * gtk_date_entry_set_editable:
 * @date: a #GtkDateEntry.
 * @is_editable: TRUE if the user is allowed to edit the text in the widget.
 *
 * Determines if the user can edit the text in the #GtkDateEntry widget or not.
 */
void
gtk_date_entry_set_editable (GtkDateEntry *date,
                             gboolean is_editable)
{
	gtk_editable_set_editable (GTK_EDITABLE (date->day), is_editable);
}

/**
 * gtk_date_entry_set_calendar_button_visible:
 * @date: a #GtkDateEntry.
 * @is_visible: TRUE if the calendar's button must be visible.
 * 
 * Determines if the calendar's button is visible or not.
 */
void
gtk_date_entry_set_calendar_button_visible (GtkDateEntry *date,
                                            gboolean is_visible)
{
	if (is_visible)
		{
			gtk_widget_show (date->btnCalendar);
		}
	else
		{
			gtk_widget_hide (date->btnCalendar);
		}
}

/* PRIVATE */
static void
gtk_date_entry_change_mask (GtkDateEntry *date)
{
	gchar *mask, *format[3];
	gint i;

	for (i = 0; i < 3; i++)
		{
			if (date->format[i] == 'd' || date->format[i] == 'm')
				{
					format[i] = g_strdup ("00");
				}
			else if (date->format[i] == 'Y')
				{
					format[i] = g_strdup ("0000");
				}
		}

	mask = g_strdup_printf ("%s%c%s%c%s",
	                        format[0],
	                        date->separator,
	                        format[1],
	                        date->separator,
	                        format[2]);
	gtk_masked_entry_set_mask (GTK_MASKED_ENTRY (date->day), mask);
}

/*
 * callbacks
 **/
static void
hide_popup (GtkWidget *date)
{
	GtkDateEntry *de = (GtkDateEntry *)date;
	gtk_widget_hide (de->wCalendar);
	gtk_grab_remove (de->wCalendar);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (de->btnCalendar), FALSE);
}

static gboolean
popup_grab_on_window (GdkWindow *window,
                      guint32 activate_time)
{
	if ((gdk_pointer_grab (window, TRUE,
	                       GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
	                       GDK_POINTER_MOTION_MASK,
	                       NULL, NULL, activate_time) == 0))
		{
			if (gdk_keyboard_grab (window, TRUE, activate_time) == 0)
				{
					return TRUE;
				}
			else
				{
					gdk_pointer_ungrab (activate_time);
					return FALSE;
				}
		}

	return FALSE;
}

static gint
delete_popup (GtkWidget *widget,
              gpointer data)
{
	hide_popup ((GtkWidget *)data);

	return TRUE;
}

static gint
key_press_popup (GtkWidget *widget,
                 GdkEventKey *event,
                 gpointer data)
{
	switch (event->keyval)
		{
			case GDK_Escape:
				break;

			case GDK_Return:
	    case GDK_KP_Enter:
				/* TO DO */
				break;

			default:
				return FALSE;
		}

	g_signal_stop_emission_by_name (widget, "key_press_event");
	hide_popup ((GtkWidget *)data);

	return TRUE;
}

/* This function is yanked from gtkcombo.c */
static gint
button_press_popup (GtkWidget *widget,
                    GdkEventButton *event,
                    gpointer user_data)
{
	GtkDateEntry *date = (GtkDateEntry *)user_data;
	GtkWidget *child = gtk_get_event_widget ((GdkEvent *)event);

	/* We don't ask for button press events on the grab widget, so
	 *  if an event is reported directly to the grab widget, it must
	 *  be on a window outside the application (and thus we remove
	 *  the popup window). Otherwise, we check if the widget is a child
	 *  of the grab widget, and only remove the popup window if it
	 *  is not.
	 */
	if (child != widget) {
		while (child) {
			if (child == widget) return FALSE;
			child = child->parent;
		}
	}

	hide_popup (user_data);

	return TRUE;
}

static void
btnCalendar_on_toggled (GtkToggleButton *togglebutton,
                        gpointer user_data)
{
	if (gtk_toggle_button_get_active (togglebutton))
		{
			gint x, y, bwidth, bheight;
			GtkRequisition req;
			GtkWidget *btn = ((GtkDateEntry *)user_data)->btnCalendar,
			          *wCalendar = ((GtkDateEntry *)user_data)->wCalendar;

			/* sets current date */
			const GDate *gdate = gtk_date_entry_get_gdate ((GtkDateEntry *)user_data);
			if (gdate != NULL)
				{
					gtk_calendar_select_month (GTK_CALENDAR (((GtkDateEntry *)user_data)->calendar),
					                           (guint)g_date_get_month (gdate) - 1,
					                           (guint)g_date_get_year (gdate));
					gtk_calendar_select_day (GTK_CALENDAR (((GtkDateEntry *)user_data)->calendar),
					                         (guint)g_date_get_day (gdate));
				}

			/* show calendar */
			gtk_widget_size_request (wCalendar, &req);
			gdk_window_get_origin (GDK_WINDOW (btn->window), &x, &y);
			x += btn->allocation.x;
			y += btn->allocation.y;
			bwidth = btn->allocation.width;
			bheight = btn->allocation.height;
			x += bwidth - req.width;
			y += bheight;
			if (x < 0) x = 0;
			if (y < 0) y = 0;

			gtk_grab_add (wCalendar);
			gtk_window_move (GTK_WINDOW (wCalendar), x, y);
      gtk_widget_show (wCalendar);
			gtk_widget_grab_focus (((GtkDateEntry *)user_data)->calendar);
			popup_grab_on_window (wCalendar->window, gtk_get_current_event_time ());
		}
}

static void
calendar_on_day_selected (GtkCalendar *calendar,
                          gpointer user_data)
{
	guint day, month, year;

	gtk_calendar_get_date (calendar, &year, &month, &day);
	gtk_date_entry_set_date_gdate ((GtkDateEntry *)user_data,
                           g_date_new_dmy ((GDateDay)day,
                                           (GDateMonth)(month + 1),
                                           (GDateYear)year));
}

static void
calendar_on_day_selected_double_click (GtkCalendar *calendar,
                                       gpointer user_data)
{
	hide_popup ((GtkWidget *)user_data);
}