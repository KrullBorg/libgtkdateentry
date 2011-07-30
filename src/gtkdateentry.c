/*
 * GtkDateEntry widget for GTK+
 *
 * Copyright (C) 2005-2011 Andrea Zagli <azagli@libero.it>
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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#ifdef G_OS_WIN32
#include <windows.h>
#include <winnls.h>
#else
#include <langinfo.h>
#endif

#include <gtkmaskedentry.h>

#include "gtkdateentry.h"

enum
{
	PROP_0,
	PROP_SEPARATOR,
	PROP_TIME_SEPARATOR,
	PROP_FORMAT,
	PROP_EDITABLE_WITH_CALENDAR,
	PROP_CALENDAR_BUTTON_VISIBLE,
	PROP_TIME_VISIBLE,
	PROP_TIME_WITH_SECONDS
};

static void gtk_date_entry_class_init (GtkDateEntryClass *klass);
static void gtk_date_entry_init (GtkDateEntry *date);

static void gtk_date_entry_size_request (GtkWidget *widget,
                                    GtkRequisition *requisition);
static void gtk_date_entry_size_allocate (GtkWidget *widget,
                                     GtkAllocation *allocation);

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

static void gtk_date_entry_set_property (GObject *object,
                                           guint property_id,
                                           const GValue *value,
                                           GParamSpec *pspec);
static void gtk_date_entry_get_property (GObject *object,
                                           guint property_id,
                                           GValue *value,
                                           GParamSpec *pspec);

static gchar *gtk_date_entry_get_separator_from_locale ();
static gchar *gtk_date_entry_get_format_from_locale ();

static GtkWidgetClass *parent_class = NULL;


#define GTK_DATE_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_DATE_ENTRY, GtkDateEntryPrivate))

typedef struct _GtkDateEntryPrivate GtkDateEntryPrivate;
struct _GtkDateEntryPrivate
	{
		GtkWidget *hbox;
		GtkWidget *day;
		GtkWidget *btnCalendar;
		GtkWidget *wCalendar;
		GtkWidget *calendar;

		GtkWidget *spnHours;
		GtkWidget *lblMinutes;
		GtkWidget *spnMinutes;
		GtkWidget *lblSeconds;
		GtkWidget *spnSeconds;

		gchar *separator;
		gchar *time_separator;
		gchar *format;
		gboolean editable_with_calendar;

		gboolean time_is_visible;
		gboolean time_with_seconds;
	};

G_DEFINE_TYPE (GtkDateEntry, gtk_date_entry, GTK_TYPE_BIN)

static void
gtk_date_entry_class_init (GtkDateEntryClass *klass)
{
	GtkWidgetClass *widget_class;

	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (GtkDateEntryPrivate));

	widget_class = (GtkWidgetClass*) klass;
	parent_class = g_type_class_peek_parent (klass);

	object_class->set_property = gtk_date_entry_set_property;
	object_class->get_property = gtk_date_entry_get_property;

	widget_class->size_request = gtk_date_entry_size_request;
	widget_class->size_allocate = gtk_date_entry_size_allocate;

	g_object_class_install_property (object_class, PROP_SEPARATOR,
	                                 g_param_spec_string ("separator",
	                                                      "The separator",
	                                                      "The separator between day, month and year.",
	                                                      "",
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_TIME_SEPARATOR,
	                                 g_param_spec_string ("time-separator",
	                                                      "The time separator",
	                                                      "The separator between hours, minutes and seconds.",
	                                                      "",
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_FORMAT,
	                                 g_param_spec_string ("format",
	                                                      "The date's format",
	                                                      "The date's format.",
	                                                      "",
	                                                      G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_EDITABLE_WITH_CALENDAR,
	                                 g_param_spec_boolean ("editable-from-calendar",
	                                                       "TRUE if it is editable only from calendar",
	                                                       "Determines if the user can edit the text"
	                                                       " in the #GtkDateEntry widget only from the calendar or not.",
	                                                       FALSE,
	                                                       G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_CALENDAR_BUTTON_VISIBLE,
	                                 g_param_spec_boolean ("calendar-button-visible",
	                                                       "TRUE to show the calendar's button",
	                                                       "Determines if the calendar's button is visible or not.",
	                                                       TRUE,
	                                                       G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_TIME_VISIBLE,
	                                 g_param_spec_boolean ("time-visible",
	                                                       "TRUE to show the time part",
	                                                       "Determines if the time part of the widget is visible or not.",
	                                                       TRUE,
	                                                       G_PARAM_READWRITE));

	g_object_class_install_property (object_class, PROP_TIME_WITH_SECONDS,
	                                 g_param_spec_boolean ("time-with-seconds",
	                                                       "TRUE to show the seconds in time part",
	                                                       "Determines if the seconds in the time part of the widget are visible or not.",
	                                                       TRUE,
	                                                       G_PARAM_READWRITE));
}

static void
gtk_date_entry_init (GtkDateEntry *date)
{
	GtkWidget *arrow;

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	priv->separator = gtk_date_entry_get_separator_from_locale ();
	priv->time_separator = ":";
	priv->format = gtk_date_entry_get_format_from_locale ();

	priv->hbox = gtk_hbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (date), priv->hbox);
	gtk_widget_show (priv->hbox);

	priv->day = gtk_masked_entry_new ();
	gtk_date_entry_change_mask (date);
	gtk_entry_set_width_chars (GTK_ENTRY (priv->day), 10);
	gtk_box_pack_start (GTK_BOX (priv->hbox), priv->day, TRUE, TRUE, 0);
	gtk_widget_show (priv->day);

	priv->btnCalendar = gtk_toggle_button_new ();
	gtk_box_pack_start (GTK_BOX (priv->hbox), priv->btnCalendar, FALSE, FALSE, 0);
	gtk_widget_set_no_show_all (priv->btnCalendar, TRUE);
	gtk_widget_show (priv->btnCalendar);

	g_signal_connect (G_OBJECT (priv->btnCalendar), "toggled",
	                  G_CALLBACK (btnCalendar_on_toggled), (gpointer)date);

	arrow = (GtkWidget *)gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_NONE);
	gtk_container_add (GTK_CONTAINER (priv->btnCalendar), arrow);
	gtk_widget_show (arrow);

	priv->wCalendar = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_window_set_resizable (GTK_WINDOW (priv->wCalendar), FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (priv->wCalendar), 3);

	gtk_widget_set_events (priv->wCalendar,
	                       gtk_widget_get_events (priv->wCalendar) | GDK_KEY_PRESS_MASK);

	g_signal_connect (priv->wCalendar, "delete_event",
	                  G_CALLBACK (delete_popup), date);
	g_signal_connect (priv->wCalendar, "key_press_event",
	                  G_CALLBACK (key_press_popup), date);
	g_signal_connect (priv->wCalendar, "button_press_event",
	                  G_CALLBACK (button_press_popup), date);

	priv->calendar = gtk_calendar_new ();
	gtk_container_add (GTK_CONTAINER (priv->wCalendar), priv->calendar);
	gtk_widget_show (priv->calendar);

	g_signal_connect (G_OBJECT (priv->calendar), "day-selected",
	                  G_CALLBACK (calendar_on_day_selected), (gpointer)date);
	g_signal_connect (G_OBJECT (priv->calendar), "day-selected-double-click",
	                  G_CALLBACK (calendar_on_day_selected_double_click), (gpointer)date);

	priv->time_is_visible = TRUE;
	priv->time_with_seconds = TRUE;

	priv->spnHours = gtk_spin_button_new_with_range (0, 23, 1);
	gtk_entry_set_width_chars (GTK_ENTRY (priv->spnHours), 2);
	priv->lblMinutes = gtk_label_new (":");
	priv->spnMinutes = gtk_spin_button_new_with_range (0, 59, 1);
	gtk_entry_set_width_chars (GTK_ENTRY (priv->spnMinutes), 2);
	priv->lblSeconds = gtk_label_new (":");
	priv->spnSeconds = gtk_spin_button_new_with_range (0, 59, 1);
	gtk_entry_set_width_chars (GTK_ENTRY (priv->spnSeconds), 2);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON (priv->spnHours), 0);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON (priv->spnMinutes), 0);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON (priv->spnSeconds), 0);
	gtk_box_pack_start (GTK_BOX (priv->hbox), priv->spnHours, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (priv->hbox), priv->lblMinutes, FALSE, FALSE, 3);
	gtk_box_pack_start (GTK_BOX (priv->hbox), priv->spnMinutes, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (priv->hbox), priv->lblSeconds, FALSE, FALSE, 3);
	gtk_box_pack_start (GTK_BOX (priv->hbox), priv->spnSeconds, TRUE, TRUE, 0);
	gtk_widget_show (priv->spnHours);
	gtk_widget_show (priv->lblMinutes);
	gtk_widget_show (priv->spnMinutes);
	gtk_widget_show (priv->lblSeconds);
	gtk_widget_show (priv->spnSeconds);
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
 * Returns: The newly created #GtkDateEntry widget.
 */
GtkWidget
*gtk_date_entry_new (const gchar *format, const gchar *separator, gboolean calendar_button_is_visible)
{
	gchar *_format;
	gchar *_separator;

	GtkWidget *w = GTK_WIDGET (g_object_new (gtk_date_entry_get_type (), NULL));

	if (format == NULL)
		{
			_format = gtk_date_entry_get_format_from_locale ();
		}
	else
		{
			_format = g_strdup (format);
		}
	if (!gtk_date_entry_set_format (GTK_DATE_ENTRY (w), _format))
		{
			return NULL;
		}

	if (separator == NULL)
		{
			_separator = gtk_date_entry_get_separator_from_locale ();
		}
	else
		{
			_separator = g_strdup (separator);
		}
	if (!gtk_date_entry_set_separator (GTK_DATE_ENTRY (w), _separator))
		{
			return NULL;
		}

	gtk_date_entry_set_calendar_button_visible (GTK_DATE_ENTRY (w), calendar_button_is_visible);

	return w;
}

/**
 * gtk_date_entry_set_separator:
 * @date: a #GtkDateEntry object.
 * @separator: a #gchar that represents the separator between day, month and year.
 *
 * Set the separator between day, month and year.
 */
gboolean
gtk_date_entry_set_separator (GtkDateEntry *date, const gchar *separator)
{
	gchar *_separator;

	g_return_val_if_fail (GTK_IS_DATE_ENTRY (date), FALSE);

	GDate *gdate = gtk_date_entry_get_gdate (date);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	if (separator == NULL)
		{
			return FALSE;
		}
	_separator = g_strstrip (g_strdup (separator));
	if (strlen (_separator) != 1)
		{
			return FALSE;
		}

	priv->separator = g_strdup (_separator);
	gtk_date_entry_change_mask (date);
	gtk_date_entry_set_date_gdate (date, gdate);

	g_free (_separator);
}

/**
 * gtk_date_entry_set_time_separator:
 * @date: a #GtkDateEntry object.
 * @separator: a #gchar that represents the separator between hours, minutes and seconds.
 *
 * Set the separator between hours, minutes and seconds.
 */
gboolean
gtk_date_entry_set_time_separator (GtkDateEntry *date, const gchar *separator)
{
	gchar *_separator;

	g_return_val_if_fail (GTK_IS_DATE_ENTRY (date), FALSE);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	if (separator == NULL)
		{
			return FALSE;
		}
	_separator = g_strstrip (g_strdup (separator));
	if (strlen (_separator) != 1)
		{
			return FALSE;
		}

	priv->time_separator = g_strdup (_separator);
	gtk_label_set_text (GTK_LABEL (priv->lblMinutes), _separator);
	gtk_label_set_text (GTK_LABEL (priv->lblSeconds), _separator);

	g_free (_separator);
}

/**
 * gtk_date_entry_set_format:
 * @date: a #GtkDateEntry object.
 * @format: a #gchar which is the date's format.
 *
 * Sets the date's format.
 *
 * Returns: FALSE if @format isn't a valid date format.
 */
gboolean
gtk_date_entry_set_format (GtkDateEntry *date, const gchar *format)
{
	gint i;
	gboolean d = FALSE;
	gboolean m = FALSE;
	gboolean y = FALSE;
	gchar *format_;
	GDate *gdate;

	g_return_val_if_fail (GTK_IS_DATE_ENTRY (date), FALSE);
	g_return_val_if_fail (format != NULL, FALSE);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	format_ = g_strstrip (g_strdup (format));
	if (strlen (format_) != 3)
		{
			return FALSE;
		}

	for (i = 0; i < 3; i++)
		{
			switch (format_[i])
				{
					case 'd':
						if (d) return FALSE;
						d = TRUE;
						break;

					case 'm':
						if (m) return FALSE;
						m = TRUE;
						break;

					case 'Y':
						if (y) return FALSE;
						y = TRUE;
						break;

					default:
						return FALSE;
				}
		}
	if (!d || !m || !y) return FALSE; 

	gdate = gtk_date_entry_get_gdate (date);

	priv->format = g_strdup (format_);
	gtk_date_entry_change_mask (date);
	gtk_date_entry_set_date_gdate (date, gdate);

	g_free (format_);

	return TRUE;
}

/**
 * gtk_date_entry_get_text:
 * @date: a #GtkDateEntry object.
 *
 * Returns the @date's content as is.
 *
 * Returns: A pointer to the content of the widget as is.
 */
const gchar
*gtk_date_entry_get_text (GtkDateEntry *date)
{
	gchar *ret;

	GtkDateEntryPrivate *priv;

	g_return_val_if_fail (GTK_IS_DATE_ENTRY (date), NULL);

	priv = GTK_DATE_ENTRY_GET_PRIVATE (date);
	ret = (gchar *)gtk_entry_get_text (GTK_ENTRY (priv->day));

	if (gtk_widget_get_visible (priv->spnHours))
		{
			ret = g_strconcat (ret,
			                   g_strdup_printf (" %02d%s%02d%s%02d",
			                                    gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (priv->spnHours)),
			                                    priv->time_separator,
			                                    gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (priv->spnMinutes)),
			                                    priv->time_separator,
			                                    gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (priv->spnSeconds))),
			                   NULL);
		}

	return ret;
}

/**
 * gtk_date_entry_get_strf:
 * @date: a #GtkDateEntry object.
 * @format: a #gchar which is the date's format.
 * @separator: a #gchar which is the separator between day, month and year.
 *
 * Returns: A pointer to the content of the widget formatted as specified in 
 * @format with @separator.
 */
const gchar
*gtk_date_entry_get_strf (GtkDateEntry *date,
                          const gchar *format,
                          const gchar *separator,
                          const gchar *time_separator)
{
	gchar *fmt;
	gchar *sep;
	gchar *tsep;

	gchar *str_date;
	gchar *str_time;

	guint l;
	guint i;

	g_return_val_if_fail (GTK_IS_DATE_ENTRY (date), "");

	struct tm *tm_date = gtk_date_entry_get_tm (date);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	if (tm_date == NULL)
		{
			return "";
		}

	str_date = g_strdup ("");
	str_time = g_strdup ("");

	if (format == NULL)
		{
			fmt = priv->format;
		}
	else
		{
			fmt = g_strdup (format);
		}

	if (separator == NULL)
		{
			sep = priv->separator;
		}
	else
		{
			sep = (gchar *)separator;
		}

	if (time_separator == NULL)
		{
			tsep = priv->time_separator;
		}
	else
		{
			tsep = (gchar *)time_separator;
		}

	l = strlen (fmt);
	for (i = 0; i < l; i++)
		{
			switch (fmt[i])
				{
					case 'd':
						if (strlen (str_date) > 0)
							{
								str_date = g_strconcat (str_date, sep, NULL);
							}
						str_date = g_strconcat (str_date, g_strdup_printf ("%02d", tm_date->tm_mday, NULL));
						break;

					case 'm':
						if (strlen (str_date) > 0)
							{
								str_date = g_strconcat (str_date, sep, NULL);
							}
						str_date = g_strconcat (str_date, g_strdup_printf ("%02d", tm_date->tm_mon + 1, NULL));
						break;

					case 'Y':
						if (strlen (str_date) > 0)
							{
								str_date = g_strconcat (str_date, sep, NULL);
							}
						str_date = g_strconcat (str_date, g_strdup_printf ("%04d", tm_date->tm_year + 1900, NULL));
						break;

					case 'H':
						if (strlen (str_time) > 0)
							{
								str_time = g_strconcat (str_time, tsep, NULL);
							}
						str_time = g_strconcat (str_time, g_strdup_printf ("%02d", tm_date->tm_hour, NULL));
						break;

					case 'M':
						if (strlen (str_time) > 0)
							{
								str_time = g_strconcat (str_time, tsep, NULL);
							}
						str_time = g_strconcat (str_time, g_strdup_printf ("%02d", tm_date->tm_min, NULL));
						break;

					case 'S':
						if (strlen (str_time) > 0)
							{
								str_time = g_strconcat (str_time, tsep, NULL);
							}
						str_time = g_strconcat (str_time, g_strdup_printf ("%02d", tm_date->tm_sec, NULL));
						break;
				}
		}

	return (const gchar *)g_strstrip (g_strdup_printf ("%s %s", str_date, str_time));
}

/**
 * gtk_date_entry_get_tm:
 * @date: a #GtkDateEntry object.
 *
 * Returns: the @date's content as a struct tm.
 */
struct tm
*gtk_date_entry_get_tm (GtkDateEntry *date)
{
	struct tm tm;

	const GDate *gdate;
	GtkDateEntryPrivate *priv;

	g_return_val_if_fail (GTK_IS_DATE_ENTRY (date), NULL);

	priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	gdate = gtk_date_entry_get_gdate (date);

	if (gdate == NULL) return NULL;

	g_date_to_struct_tm (gdate, &tm);

	if (gtk_widget_get_visible (priv->spnHours))
		{
			tm.tm_hour = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (priv->spnHours));
			tm.tm_min = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (priv->spnMinutes));
			tm.tm_sec = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (priv->spnSeconds));
		}

	return (struct tm *)g_memdup ((gconstpointer)&tm, sizeof (struct tm));
}

/**
 * gtk_date_entry_get_gdate:
 * @date: a #GtkDateEntry object.
 *
 * Returns: the @date's content as a #GDate.
 */
GDate
*gtk_date_entry_get_gdate (GtkDateEntry *date)
{
	gint i, pos = 0, val;
	gchar *txt;
	GDate *gdate;
	GDateDay gday;
	GDateMonth gmon;
	GDateYear gyear;

	g_return_val_if_fail (GTK_IS_DATE_ENTRY (date), NULL);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	gday = G_DATE_BAD_DAY;
	gmon = G_DATE_BAD_MONTH;
	gyear = G_DATE_BAD_YEAR;

	txt = (gchar *)gtk_entry_get_text (GTK_ENTRY (priv->day));

	for (i = 0; i < 3; i++)
		{
			switch (priv->format[i])
				{
					case 'd':
						val = strtol (g_strndup (txt + pos, 2), NULL, 10);
						if (g_date_valid_day ((GDateDay)val))
							{
								gday = (GDateDay)val;
								pos += 3;
							}
						else
							{
								return NULL;
							}
						break;

					case 'm':
						val = strtol (g_strndup (txt + pos, 2), NULL, 10);
						if (g_date_valid_month ((GDateMonth)val))
							{
								gmon = (GDateMonth)val;
								pos += 3;
							}
						else
							{
								return NULL;
							}
						break;

					case 'Y':
						val = strtol (g_strndup (txt + pos, 4), NULL, 10);
						if (g_date_valid_year ((GDateYear)val))
							{
								gyear = (GDateYear)val;
								pos += 5;
							}
						else
							{
								return NULL;
							}
						break;
				}
		}

	if (g_date_valid_day (gday)
		&& g_date_valid_month (gmon)
		&& g_date_valid_year (gyear))
		{
			return g_date_new_dmy (gday, gmon, gyear);
		}
	else
		{
			return NULL;
		}
}

/**
 * gtk_date_entry_get_gdatetime:
 * @date: a #GtkDateEntry object.
 *
 * Returns: the @date's content as a #GDateTime.
 */
GDateTime
*gtk_date_entry_get_gdatetime (GtkDateEntry *date)
{
	GDateTime *ret;
	GDate *gdate;

	gint hour;
	gint minute;
	gdouble seconds;

	g_return_val_if_fail (GTK_IS_DATE_ENTRY (date), NULL);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	gdate = gtk_date_entry_get_gdate (date);
	if (gdate == NULL)
		{
			return NULL;
		}

	hour = 0;
	minute = 0;
	seconds = 0.0;
	if (priv->time_is_visible)
		{
			hour = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (priv->spnHours));
			minute = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (priv->spnMinutes));
			if (priv->time_with_seconds)
				{
					seconds = gtk_spin_button_get_value (GTK_SPIN_BUTTON (priv->spnSeconds));
				}
		}

	ret = g_date_time_new_local (g_date_get_year (gdate),
	                             g_date_get_month (gdate),
	                             g_date_get_day (gdate),
	                             hour,
	                             minute,
	                             seconds);

	return ret;
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
 * Returns: TRUE if @date's content is setted.
 */
gboolean
gtk_date_entry_set_date_strf (GtkDateEntry *date,
                              const gchar *str,
                              const gchar *format)
{
	gchar *fmt;
	GDateDay day;
	GDateMonth month;
	GDateYear year;
	gint i;
	gint pos = 0;

	g_return_val_if_fail (GTK_IS_DATE_ENTRY (date), FALSE);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	if (format == NULL)
		{
			fmt = priv->format;
		}
	else
		{
			fmt = g_strdup (format);
		}

	for (i = 0; i < 3; i++)
		{
			switch (fmt[i])
				{
					case 'd':
						if (!g_date_valid_day ((GDateDay)strtol (g_strndup (str + pos, 2), NULL, 10))) return FALSE;
						day = (GDateDay)strtol (g_strndup (str + pos, 2), NULL, 10);
						pos += 3;
						break;

					case 'm':
						if (!g_date_valid_month ((GDateMonth)strtol (g_strndup (str + pos, 2), NULL, 10))) return FALSE;
						month = (GDateMonth)strtol (g_strndup (str + pos, 2), NULL, 10);
						pos += 3;
						break;

					case 'Y':
						if (!g_date_valid_year ((GDateYear)strtol (g_strndup (str + pos, 4), NULL, 10))) return FALSE;
						year = (GDateYear)strtol (g_strndup (str + pos, 4), NULL, 10);
						pos += 5;
						break;
				}
		}

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
	GtkDateEntryPrivate *priv;

	g_return_if_fail (GTK_IS_DATE_ENTRY (date));

	priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	GDate *gdate = g_date_new_dmy ((GDateDay)tmdate.tm_mday,
	                               (GDateMonth)tmdate.tm_mon + 1,
	                               (GDateYear)tmdate.tm_year + 1900);
	gtk_date_entry_set_date_gdate (date, (const GDate *)gdate);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (priv->spnHours), tmdate.tm_hour);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (priv->spnMinutes), tmdate.tm_min);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (priv->spnSeconds), tmdate.tm_sec);
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
	gchar *txt;

	g_return_if_fail (GTK_IS_DATE_ENTRY (date));
	g_return_if_fail (gdate != NULL && g_date_valid (gdate));

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	txt = g_strdup ("");
	for (i = 0; i < 3; i++)
		{
			switch (priv->format[i])
				{
					case 'd':
						txt = g_strconcat (txt, g_strdup_printf ("%02d", g_date_get_day (gdate)), NULL);
						break;

					case 'm':
						txt = g_strconcat (txt, g_strdup_printf ("%02d", g_date_get_month (gdate)), NULL);
						break;

					case 'Y':
						txt = g_strconcat (txt, g_strdup_printf ("%04d", g_date_get_year (gdate)), NULL);
						break;
				}

			if (i < 2)
				{
					txt = g_strconcat (txt, priv->separator, NULL);
				}
		}

	gtk_editable_set_position (GTK_EDITABLE (priv->day), 0);
	gtk_entry_set_text (GTK_ENTRY (priv->day), txt);
}

/**
 * gtk_date_entry_set_date_gdatetime:
 * @date: a #GtkDateEntry.
 * @gdatetime: a #GDateTime from which set @date's content.
 *
 * Sets @date's content from a @gdatetime.
 **/
void
gtk_date_entry_set_date_gdatetime (GtkDateEntry *date, const GDateTime *gdatetime)
{
	GDate *gdate;

	g_return_if_fail (GTK_IS_DATE_ENTRY (date));

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	gdate = g_date_new_dmy (g_date_time_get_year ((GDateTime *)gdatetime),
	                        g_date_time_get_month ((GDateTime *)gdatetime),
	                        g_date_time_get_day_of_month ((GDateTime *)gdatetime));

	gtk_date_entry_set_date_gdate (date, gdate);

	if (priv->time_is_visible)
		{
			gtk_spin_button_set_value (GTK_SPIN_BUTTON (priv->spnHours),
			                           (gdouble)g_date_time_get_hour ((GDateTime *)gdatetime));
			gtk_spin_button_set_value (GTK_SPIN_BUTTON (priv->spnMinutes),
			                           (gdouble)g_date_time_get_minute ((GDateTime *)gdatetime));
			if (priv->time_with_seconds)
				{
					gtk_spin_button_set_value (GTK_SPIN_BUTTON (priv->spnHours),
					                           g_date_time_get_seconds ((GDateTime *)gdatetime));
				}
		}
}

/**
 * gtk_date_entry_is_valid:
 * @date: a #GtkDateEntry.
 *
 * Checks if @date's content is a valid date.
 *
 * Returns: TRUE if @date's content is a valid date.
 **/
gboolean
gtk_date_entry_is_valid (GtkDateEntry *date)
{
	GDateTime *gdatetime;

	g_return_val_if_fail (GTK_IS_DATE_ENTRY (date), FALSE);

	gdatetime = gtk_date_entry_get_gdatetime (date);
	if (gdatetime != NULL)
		{
			return TRUE;
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
	g_return_if_fail (GTK_IS_DATE_ENTRY (date));

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	gtk_editable_set_editable (GTK_EDITABLE (priv->day), is_editable);
	gtk_widget_set_sensitive (priv->btnCalendar, is_editable);
	gtk_widget_set_sensitive (priv->spnHours, is_editable);
	gtk_widget_set_sensitive (priv->spnMinutes, is_editable);
	gtk_widget_set_sensitive (priv->spnSeconds, is_editable);
}

/**
 * gtk_date_entry_set_editable_with_calendar:
 * @date:  a #GtkDateEntry.
 * @is_editable_with_calendar: TRUE if the user is allowed to edit the text
 * in the widget only from the calendar.
 *
 * Determines if the user can edit the text in the #GtkDateEntry widget only
 * from the calendar or not.
 */
void
gtk_date_entry_set_editable_with_calendar (GtkDateEntry *date,
                                  gboolean is_editable_with_calendar)
{
	g_return_if_fail (GTK_IS_DATE_ENTRY (date));

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	gtk_editable_set_editable (GTK_EDITABLE (priv->day), !is_editable_with_calendar);
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
	g_return_if_fail (GTK_IS_DATE_ENTRY (date));

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	if (is_visible)
		{
			gtk_widget_show (priv->btnCalendar);
		}
	else
		{
			gtk_widget_hide (priv->btnCalendar);
		}
}

/**
 * gtk_date_entry_set_time_visible:
 * @date: a #GtkDateEntry.
 * @is_visible: TRUE if the time must be visible.
 * 
 * Determines if the time is visible or not.
 */
void
gtk_date_entry_set_time_visible (GtkDateEntry *date,
                                 gboolean is_visible)
{
	g_return_if_fail (GTK_IS_DATE_ENTRY (date));

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	priv->time_is_visible = is_visible;
	if (priv->time_is_visible)
		{
			gtk_widget_show (priv->spnHours);
			gtk_widget_show (priv->lblMinutes);
			gtk_widget_show (priv->spnMinutes);
			if (priv->time_with_seconds)
				{
					gtk_widget_show (priv->lblSeconds);
					gtk_widget_show (priv->spnSeconds);
				}
			else
				{
					gtk_widget_hide (priv->lblSeconds);
					gtk_widget_hide (priv->spnSeconds);

					gtk_spin_button_set_value (GTK_SPIN_BUTTON (priv->spnSeconds), 0.0);
				}
		}
	else
		{
			gtk_widget_hide (priv->spnHours);
			gtk_widget_hide (priv->lblMinutes);
			gtk_widget_hide (priv->spnMinutes);
			gtk_widget_hide (priv->lblSeconds);
			gtk_widget_hide (priv->spnSeconds);

			gtk_spin_button_set_value (GTK_SPIN_BUTTON (priv->spnHours), 0.0);
			gtk_spin_button_set_value (GTK_SPIN_BUTTON (priv->spnMinutes), 0.0);
			gtk_spin_button_set_value (GTK_SPIN_BUTTON (priv->spnSeconds), 0.0);
		}
}

/* PRIVATE */
static void
gtk_date_entry_change_mask (GtkDateEntry *date)
{
	gchar *mask, *format[3];
	gint i;

	g_return_if_fail (GTK_IS_DATE_ENTRY (date));

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	for (i = 0; i < 3; i++)
		{
			if (priv->format[i] == 'd' || priv->format[i] == 'm')
				{
					format[i] = g_strdup ("00");
				}
			else if (priv->format[i] == 'Y')
				{
					format[i] = g_strdup ("0000");
				}
		}

	mask = g_strdup_printf ("%s%s%s%s%s",
	                        format[0],
	                        priv->separator,
	                        format[1],
	                        priv->separator,
	                        format[2]);
	gtk_masked_entry_set_mask (GTK_MASKED_ENTRY (priv->day), mask);
}

/*
 * callbacks
 **/
static void
hide_popup (GtkWidget *date)
{
	g_return_if_fail (GTK_IS_DATE_ENTRY (date));

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE ((GtkDateEntry *)date);

	gtk_widget_hide (priv->wCalendar);
	gtk_grab_remove (priv->wCalendar);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->btnCalendar), FALSE);
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
			GtkDateEntry *date = (GtkDateEntry *)user_data;
			GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);
		
			gint x, y, bwidth, bheight;
			GtkRequisition req;
			GtkWidget *btn = priv->btnCalendar,
			          *wCalendar = priv->wCalendar;

			/* sets current date */
			const GDate *gdate = gtk_date_entry_get_gdate (date);
			if (gdate != NULL)
				{
					gtk_calendar_select_month (GTK_CALENDAR (priv->calendar),
					                           (guint)g_date_get_month (gdate) - 1,
					                           (guint)g_date_get_year (gdate));
					gtk_calendar_select_day (GTK_CALENDAR (priv->calendar),
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

			gtk_editable_set_position (GTK_EDITABLE (priv->day), 0);

			gtk_grab_add (wCalendar);
			gtk_window_move (GTK_WINDOW (wCalendar), x, y);
			gtk_widget_show (wCalendar);
			gtk_widget_grab_focus (priv->calendar);
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

static void
gtk_date_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GtkDateEntry *date_entry = GTK_DATE_ENTRY (object);
	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date_entry);

	switch (property_id)
		{
			case PROP_SEPARATOR:
				gtk_date_entry_set_separator (date_entry, g_value_get_string (value));
				break;

			case PROP_TIME_SEPARATOR:
				gtk_date_entry_set_time_separator (date_entry, g_value_get_string (value));
				break;

			case PROP_FORMAT:
				gtk_date_entry_set_format (date_entry, g_value_get_string (value));
				break;

			case PROP_EDITABLE_WITH_CALENDAR:
				gtk_date_entry_set_editable_with_calendar (date_entry, g_value_get_boolean (value));
				break;

			case PROP_CALENDAR_BUTTON_VISIBLE:
				gtk_date_entry_set_calendar_button_visible (date_entry, g_value_get_boolean (value));
				break;

			case PROP_TIME_VISIBLE:
				gtk_date_entry_set_time_visible (date_entry, g_value_get_boolean (value));
				break;

			case PROP_TIME_WITH_SECONDS:
				priv->time_with_seconds = g_value_get_boolean (value);
				gtk_date_entry_set_time_visible (date_entry, priv->time_is_visible);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
gtk_date_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GtkDateEntry *date_entry = GTK_DATE_ENTRY (object);
	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date_entry);

	switch (property_id)
		{
			case PROP_SEPARATOR:
				g_value_set_string (value, priv->separator);
				break;

			case PROP_TIME_SEPARATOR:
				g_value_set_string (value, priv->time_separator);
				break;

			case PROP_FORMAT:
				g_value_set_string (value, priv->format);
				break;

			case PROP_EDITABLE_WITH_CALENDAR:
				g_value_set_boolean (value, priv->editable_with_calendar);
				break;

			case PROP_CALENDAR_BUTTON_VISIBLE:
				g_value_set_boolean (value, gtk_widget_get_visible (priv->btnCalendar));
				break;

			case PROP_TIME_VISIBLE:
				g_value_set_boolean (value, priv->time_is_visible);
				break;

			case PROP_TIME_WITH_SECONDS:
				g_value_set_boolean (value, priv->time_with_seconds);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
gtk_date_entry_size_request (GtkWidget *widget,
                        GtkRequisition *requisition)
{
	GtkDateEntry *date_entry;
	GtkBin *bin;
	GtkRequisition child_requisition;

	guint border_width;

	g_return_if_fail (GTK_IS_DATE_ENTRY (widget));
	g_return_if_fail (requisition != NULL);

	date_entry = GTK_DATE_ENTRY (widget);
	bin = GTK_BIN (date_entry);

	requisition->width = 0;
	requisition->height = 0;

	if (bin->child && GTK_WIDGET_VISIBLE (bin->child))
		{
			gtk_widget_size_request (bin->child, &child_requisition);
			requisition->width += child_requisition.width;
			requisition->height += child_requisition.height;
		}

	border_width = gtk_container_get_border_width (GTK_CONTAINER (widget));
	requisition->width += (border_width * 2);
	requisition->height += (border_width * 2);
}

static void
gtk_date_entry_size_allocate (GtkWidget *widget,
                         GtkAllocation *allocation)
{
	GtkDateEntry *date_entry;
	GtkBin *bin;
	GtkAllocation relative_allocation;
	GtkAllocation child_allocation;

	guint border_width;

	g_return_if_fail (GTK_IS_DATE_ENTRY (widget));
	g_return_if_fail (allocation != NULL);

	date_entry = GTK_DATE_ENTRY (widget);
	bin = GTK_BIN (date_entry);

	gtk_widget_set_allocation (widget, allocation);

	border_width = gtk_container_get_border_width (GTK_CONTAINER (widget));
	relative_allocation.x = border_width;
	relative_allocation.y = border_width;
	relative_allocation.width = MAX (1, (gint)widget->allocation.width - relative_allocation.x * 2);
	relative_allocation.height = MAX (1, (gint)widget->allocation.height - relative_allocation.y * 2);

	if (bin->child && GTK_WIDGET_VISIBLE (bin->child))
		{
			child_allocation.x = relative_allocation.x + allocation->x;
			child_allocation.y = relative_allocation.y + allocation->y;
			child_allocation.width = relative_allocation.width;
			child_allocation.height = relative_allocation.height;
			gtk_widget_size_allocate (bin->child, &child_allocation);
		}
}

static gchar
*gtk_date_entry_get_separator_from_locale ()
{
	gchar *fmt;

	gchar *lfmt;
	guint l;
	guint i;

	fmt = NULL;

#ifdef G_OS_WIN32

	gchar lpLCData[30];

	if (GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, (LPTSTR)&lpLCData, 30) == 0)
		{
			g_warning ("Unable to get locale information");
			return NULL;
		}
	lfmt = g_strdup ((const gchar *)&lpLCData);

#else

	lfmt = nl_langinfo (D_FMT);

#endif

	l = strlen (lfmt);
	for (i = 0; i < l; i++)
		{
			switch (lfmt[i])
				{
					case 'd':
					case 'm':
					case 'y':
					case 'Y':
					case '%':
						break;

					default:
						fmt = g_strdup_printf ("%c", lfmt[i]);
						i = l;
						break;
				}
		}

	return fmt;
}

static gchar
*gtk_date_entry_get_format_from_locale ()
{
	gchar *fmt;

	gchar *lfmt;
	guint l;
	guint i;

	fmt = NULL;

#ifdef G_OS_WIN32

	gchar lpLCData[30];

	if (GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, (LPTSTR)&lpLCData, 30) == 0)
		{
			g_warning ("Unable to get locale information");
			return NULL;
		}
	lfmt = g_strdup ((const gchar *)&lpLCData);

#else

	lfmt = nl_langinfo (D_FMT);

#endif

	l = strlen (lfmt);
	for (i = 0; i < l; i++)
		{
			switch (lfmt[i])
				{
					case 'd':
					case 'D':
						fmt = g_strconcat (fmt == NULL ? "" : fmt, fmt != NULL && strchr (fmt, 'd') != 0 ? "" : "d", NULL);
						break;

					case 'm':
					case 'M':
						fmt = g_strconcat (fmt == NULL ? "" : fmt, fmt != NULL && strchr (fmt, 'm') != 0 ? "" : "m", NULL);
						break;

					case 'y':
					case 'Y':
						fmt = g_strconcat (fmt == NULL ? "" : fmt, fmt != NULL && strchr (fmt, 'Y') != 0 ? "" : "Y", NULL);
						break;
				}
		}

	return fmt;
}
