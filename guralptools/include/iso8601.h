
/* libiso8601/src/libiso8601/000_TopHeader.h
 *
 *  (c)2006-2010, Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv3. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/

#ifndef HEADER_libiso8601
#define HEADER_libiso8601

#ifdef __cplusplus
extern "C" {
#endif

/* standard includes, or includes needed for type declarations */
#include <time.h>
#include <stdint.h>
#include <sys/time.h>

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libiso8601/src/libiso8601/100_types.h
 *
 *  (c)2006-2010, Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv3. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \brief Date/time point.

This structure contains the details to represent a specific instant on the UTC
timescale. It uses Jan 1, year 0000 as the origin (when \a day will be 0). \a
sec is the number of seconds elapsed since start of day, and \a nsec is the
number of nanoseconds elapsed since the start of the current second.

We correctly deal with leap seconds by encoding 23:59:60 as having a \a sec
field of 86400.

*/
struct iso8601_date {
    /*! \brief Number of nanoseconds elapsed since start of second. */
    int32_t nsec;

    /*! \brief Number of days elapsed since Jan 1, year 0000. May be negative.*/
    int32_t day;

    /*! \brief Number of seconds elapsed since start of day. */
    int32_t sec;
};



/*! \brief Date (day portion) precision.
\ingroup parser

This enumeration will record how precisely the date was specified, as well as
the format in use. It allows the library to determine the earliest and latest
dates that could possibly be represented with the given input and also allows
the output format to match the input format.

*/
enum iso8601_date_prec {
    /*! \brief Only year specified. */
    iso8601_prec_year,

    /*! \brief Year and month specified (calendar format). */
    iso8601_prec_month,

    /*! \brief Year, month and day specified (calendar format). */
    iso8601_prec_day,

    /*! \brief Year and ordinal day specified (ordinal format). */
    iso8601_prec_ord,

    /*! \brief Year and week specified (week format). */
    iso8601_prec_week,

    /*! \brief Year, week and weekday specified (week format). */
    iso8601_prec_wday
};



/*! \brief Time precision.
\ingroup parser

This enumeration records how precisely the time was specified as well as its
format. The fractional format will record whether it was the hour, minute or
second that was specified with a fractional part, allowing a processed
date/time to be presented to the user in the format it was originally
encountered.

*/
enum iso8601_time_prec {
    /*! \brief Don't display date. */
    iso8601_prec_none,
    /*! \brief Display integer part of hour. */
    iso8601_prec_hour,
    /*! \brief Display hour and integer part of minute. */
    iso8601_prec_min,
    /*! \brief Display hour, minute and integer part of second. */
    iso8601_prec_sec,
    /*! \brief Display hour and fractional part of hour. */
    iso8601_prec_hourfrac,
    /*! \brief Display hour, minute and fractional part of minute. */
    iso8601_prec_minfrac,
    /*! \brief Display hour, minute, second and nanoseconds. */
    iso8601_prec_secfrac
};



/*! \brief Date/time formatting details.

This structure simply records details related to the formatting (and precision)
of a date/time structure. The structure can be filled out by the parser so that
a program's output can match the format of its input. Alternatively it can be
controlled by the program to provide a consistent output format.

*/
struct iso8601_details {
    /*! \brief Date precision (\ref iso8601_date_prec). */
    uint8_t date_prec;

    /*! \brief Time precision (\ref iso8601_time_prec). */
    uint8_t time_prec;

    /*! \brief Flag: non-zero if extended format should be used. */
    uint8_t extended;

    /*! \brief Time zone offset in seconds. */
    int32_t tz_sec;
};



/*! \brief Short period elapsed time.

This structure contains the details of an elapsed time period, split into
seconds and nanoseconds.  None of its components can ever be negative. This is
only capable of representing short (compared to struct iso8601_date's range)
periods of up to approximately 136 years.

*/
struct iso8601_elapsed {
    /*! \brief Number of seconds. */
    uint32_t sec;
    /*! \brief Number of nanoseconds (0 to 999999999). */
    uint32_t nsec;
};



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libiso8601/src/libiso8601/200_parser.h
 *
 *  (c)2006-2010, Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv3. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup parser Parsing, printing and validation routines.

These routines are used for parsing an ISO8601 date/time string into the
internal structure used to represent them, and for validating such dates/times.

*/
/*!@{*/



/*! \brief Parse ISO8601 date/time.

\param str The input string. Whitespace will be stripped.
\param[out] earliest The earliest possible time the string could represent. May
    be 0.
\param[out] latest The latest possible time the string could represent. May be
    0.
\param[out] details Stores details such as the precision to which the time/date
    were specified. May be 0.
\retval -1 on error.
\retval 0 on success.

Parses a string containing the ISO8601 date/time. Deals with any format of
date, optionally storing the details in \a details. The time may be partial, in
which case this function returns the earliest and latest times that could
possibly be represented by the string.

Note that this function will accept leap seconds (23:59:60) on days on which
they occurred.

*/
int iso8601_parse(const char* str, struct iso8601_date* earliest,
    struct iso8601_date* latest, struct iso8601_details* details)
#ifndef DOXYGEN
    __attribute__((nonnull(1),warn_unused_result))
#endif
;



/*! \brief Print ISO8601 date/time into string.

\param str Pointer to buffer into which result is written.
\param amt Number of bytes in \a str.
\param date Date to print.
\param details Formatting details (may be 0).
\returns Pointer to buffer (\a str).

Formats and prints an ISO8601 date, optionally using the details in \a details.
Will always return a null-terminated result, even if that means truncating the
output to fit the buffer.

*/
char* iso8601_print(char* str, int amt, const struct iso8601_date* date, 
    const struct iso8601_details* details)
#ifndef DOXYGEN
    __attribute__((nonnull(1,3)))
#endif
;



/*! \brief Validate ISO8601 date/time.

\param date The date to validate.
\retval -1 if not valid.
\retval 0 if valid.

Checks the details of \a date to ensure that they are sensible. This involves
checking that \a sec is in the range 0 to 86399 (or 86400 if there is a leap
second), and that \a nsec is in the range 0 to 999999999.

*/
int iso8601_invalid(const struct iso8601_date* date)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libiso8601/src/libiso8601/400_calc.h
 *
 *  (c)2006-2010, Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv3. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup calc Conversion and calculation routines.

This set of functions is useful for converting dates and times into formats
understood by humans, and vice versa.

*/
/*!@{*/



/*! \brief Is year a leap year?

\param year Year to examine.
\retval 0 if not leap year.
\retval non-0 if leap year.

Returns non-0 if \a year is a leap year.

*/
int iso8601_isleap(int year);



/*! \brief Convert date to calendar format.

\param[out] year Calendar year.
\param[out] month Calendar month (1 to 12).
\param[out] day Calendar day (starting from 1).
\param date Date to convert.

*/
void iso8601_to_cal(int* year, int* month, int* day,
    const struct iso8601_date* date)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Convert date from calendar format.

\param[out] date Output date.
\param year Calendar year.
\param month Calendar month (1 to 12).
\param day Calenday day (starting from 1).
\retval 0 on success.
\retval -1 on error (and see \a errno).

Converts the date specified in \a year, \a month and \a day into \a date.

\note Does not touch the \a sec or \a nsec time members of \a date. This means
    they will be unchanged after a call to this function. If this will lead to
    unexpected results, initialise the structure to 0 first.

*/
int iso8601_from_cal(struct iso8601_date* date, int year, int month, int day)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Convert date to ordinal format.

\param[out] year Calendar year.
\param[out] oday Ordinal day (from 1).
\param date Date to convert.

*/
void iso8601_to_ord(int* year, int* oday, const struct iso8601_date* date)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Convert date from ordinal format.

\param[out] date Output date.
\param year Calendar year.
\param oday Ordinal day (from 1).
\retval 0 on success.
\retval -1 on error (and see \a errno).

Converts the date specified into \a year and \a oday into \a date.

\note Does not touch the \a sec or \a nsec time members of \a date. This means
    they will be unchanged after a call to this functoin. If this will lead to
    unexpected results, initialise the structure to 0 first.

*/
int iso8601_from_ord(struct iso8601_date* date, int year, int oday)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Convert date to week format.

\param[out] year Calendar year.
\param[out] week ISO week number (from 1).
\param[out] wday ISO week day number (1 = Monday, 7 = Sunday).
\param date Date to convert.

*/
void iso8601_to_week(int* year, int* week, int* wday,
    const struct iso8601_date* date)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Convert date from week format.

\param [out] date Output date.
\param year Calendar year.
\param week ISO week number (from 1).
\param wday ISO week day number (1 = Monday, 7 = Sunday).
\retval 0 on success.
\retval -1 on error (and see \a errno).

Converts the date specified into \a year, \a week and \a wday into \a date.

\note Does not touch the \a sec or \a nsec time members of \a date. This means
    they will be unchanged after a call to this functoin. If this will lead to
    unexpected results, initialise the structure to 0 first.

*/
int iso8601_from_week(struct iso8601_date* date, int year, int week, int wday)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Convert time to wall clock format.

\param[out] hour Output hour.
\param[out] min Output minute.
\param[out] sec Output second.
\param date Date/time to convert.

Converts the time stored in \a date into wall clock format, storing the result
in \a hour, \a min and \a sec.

*/
void iso8601_to_clocktime(int* hour, int* min, int* sec,
    const struct iso8601_date* date)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Convert time from wall clock format.

\param[out] date Output time.
\param hour Hour.
\param min Minute.
\param sec Second.
\retval 0 on success.
\retval -1 on error (and see \a errno).

Converts the time as specified by \a hour, \a min and \a sec, storing the
result in \a date.

\note Does not touch the \a day (date) member of \a date. This means it will be
    unchanged after a call to this function. If this will lead to unexpected
    results, initialise the structure to 0 first.

*/
int iso8601_from_clocktime(struct iso8601_date* date, int hour, int min,
    int sec)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libiso8601/src/libiso8601/400_c_library.h
 *
 *  (c)2006-2010, Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv3. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup c_library C library integration.

These functions enable integration with the C library (system call wrappers and
conversion).

*/
/*!@{*/



/*! \brief Retrieve the current time.

\param[out] date Current date/time (may be 0), in UTC. May be 0.
\param[out] details Details (may be 0), including timezone. May be 0.

Retrieves the current time from the system clock, storing it into \a date and
\a details (both parameters optional).

*/
void iso8601_now(struct iso8601_date* date, struct iso8601_details* details);



/*! \brief Set the system clock.

\param date Date to set.
\retval 0 on success.
\retval -1 on error (and see \a errno).

Attempts to set the system clock using \c clock_settime(), falling back to \c
settimeofday(). The user will need the \c CAP_SYS_TIME capability to set the
clock, otherwise \a errno will be set to \c EPERM.

*/
int iso8601_set_sysclock(const struct iso8601_date* date)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Convert from a struct timespec. */
void iso8601_from_ts(struct iso8601_date* date, const struct timespec* ts)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;

/*! \brief Convert to a struct timespec. */
void iso8601_to_ts(struct timespec* ts, const struct iso8601_date* date)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;

/*! \brief Convert from a struct timeval. */
void iso8601_from_tv(struct iso8601_date* date, const struct timeval* tv)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;

/*! \brief Convert to a struct timeval. */
void iso8601_to_tv(struct timeval* tv, const struct iso8601_date* date)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;

/*! \brief Convert from a time_t. */
void iso8601_from_time_t(struct iso8601_date* date, const time_t* t)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;

/*! \brief Convert to a time_t. */
void iso8601_to_time_t(time_t* t, const struct iso8601_date* date)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libiso8601/src/libiso8601/400_leap.h
 *
 *  (c)2006-2010, Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv3. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup leap Leap second support.

A set of functions for explicitly dealing with leap seconds. All library
functions are implicitly leap second aware, but there are occasions when leap
seconds must be dealt with explicitly. These functions permit this.

Internally, leap seconds are represented as a table of day numbers; each day
number present in the table means that the corresponding day has 86401 seconds
(a leap second).

\note The library does not (yet) have support for negative leap seconds. This
    support will be added should a day with a negative leap second ever come.
    This will need to addressed in the API as the current API does not support
    loading a table of negative leap seconds.

It is possible to create a disk file containing an updated table and to load
that file using \ref iso8601_leap_table_load (or to explicitly use an existing
in-memory table with \ref iso8601_leap_table_set). The format of the on-disk
file is:

<pre># integers are stored big-endian
[8*char] signature, "/O9PdPZI"
[uint32] number of entries, 'n'

# entries are repeated 'n' times, and stored lowest day number first
[uint32] day number of entry</pre>

*/
/*!@{*/



/*! \brief Return number of seconds in day, taking leap seconds into account.

\param date The date to return the number of seconds for.
\retval 86399 day with a negative leap second.
\retval 86400 day with no leap second.
\retval 86401 day with one leap second.

Returns the duration of a day \a date, in seconds. This function takes leap
seconds into account and may be used to determine if a day contains a leap
second or not.

\note There have not yet been any days requiring a negative leap second, so at
    present 86399 will never be returned.

*/
int iso8601_seconds_leap(const struct iso8601_date* date)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Return number of leap seconds elapsed between two days.

\param start The start date.
\param end The end date.
\returns Number of leap seconds elapsed.

Computes the number of leap seconds that have elapsed between two days. Note
that this is the sum of such leap seconds, so it will be 0 if (for example)
there is one positive leap second and one negative leap second. The ordering of
the dates is important; if \a start is after \a end, then the value returned
will be negative (for positive leap seconds).

*/
int iso8601_leap_elapsed(const struct iso8601_date* start,
    const struct iso8601_date* end)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Update table of leap seconds.

\param new_table Array of day numbers on which leap seconds occur.
\param new_size Number of entries in array.

This function can be used to update the table of leap seconds at runtime. The
\a new_table argument points to an array of integers, each entry being the day
number containing a leap second.  The array must be sorted so that lower day
numbers appear towards the start of the array. \a new_size gives the number of
entries in the array. \a new_table must persist in memory as long as library
functions are in use.

\warning If negative leap seconds are ever used, this function will not support
    them. There may need to be an ABI change in future to solve this problem.

*/
void iso8601_leap_table_set(int* new_table, int new_size)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Load new table of leap seconds from disk.

\param fname Filename. May be 0 for system default.
\retval 0 on success.
\retval -1 on error (and see \a errno).

This function attempts to load a new table of leap seconds from disk, using the
filename \a fname.  If \a fname is not specified, the system default (set at
compile time) will be used.

If the table is loaded successfully, it will be set with \ref
iso8601_leap_table_set(). On any error, -1 will be returned, and \a errno will
be set appropriately. If \a errno is \c EINVAL, then the file does not contain
a valid leap second table.

*/
int iso8601_leap_table_load(const char* fname);



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libiso8601/src/libiso8601/400_manip.h
 *
 *  (c)2006-2010, Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv3. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup manip Manipulation routines.

This set of functions is useful for performing arithmetic etc. on dates. It
uses struct \ref iso8601_elapsed to represent the magnitude of time differences.

*/
/*!@{*/



/*! \brief Comparison (less than).

\param d1 First date to compare.
\param d2 Second date to compare.
\retval non-0 if \a d1 &lt; \a d2
\retval 0 otherwise

*/
int iso8601_lt(const struct iso8601_date* d1, const struct iso8601_date* d2)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Comparison (less than or equal to).

\param d1 First date to compare.
\param d2 Second date to compare.
\retval non-0 if \a d1 &lt;= \a d2
\retval 0 otherwise

*/
int iso8601_lte(const struct iso8601_date* d1, const struct iso8601_date* d2)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Comparison (equality).

\param d1 First date to compare.
\param d2 Second date to compare.
\retval non-0 if \a d1 == \a d2
\retval 0 otherwise

*/
int iso8601_eq(const struct iso8601_date* d1, const struct iso8601_date* d2)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Comparison (for qsort et al.).

\param d1 First date to compare.
\param d2 Second date to compare.
\retval -1 if \a d1 &lt; \a d2
\retval 0 if \a d1 == \a d2
\retval 1 if \a d1 &gt; \a d2

*/
int iso8601_cmp(const struct iso8601_date* d1, const struct iso8601_date* d2)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Add a period to a date.

\param[in,out] date Date to modify.
\param per Period to advance date/time by.

*/
void iso8601_add_elapsed(struct iso8601_date* date,
    const struct iso8601_elapsed* per)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Subtract a period from a date.

\param[in,out] date Date to modify.
\param per Period to regress date/time by.

*/
void iso8601_subtract_elapsed(struct iso8601_date* date,
    const struct iso8601_elapsed* per)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Add a multiple of a period to a date.

\param[in,out] date Date to modify.
\param per Period to advance date/time by.
\param n Multiple of \a per.

Adds \a n multiples of \a per to \a date. \a n may be 0 or negative. The result
is stored in \a date. This is an efficient implementation which avoids loops,
but it does use 64-bit arithmetic.

*/
void iso8601_add_multiple(struct iso8601_date* date,
    const struct iso8601_elapsed* per, int n)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Find difference between dates.

\param d1 First date.
\param d2 Second date.
\param[out] per Magnitude of period elapsed between two dates. Pointer may be 0.
\param[out] sign Set to sign of difference (-1 or +1). Pointer may be 0.

This function will perform the calculation <code>|d1 - d2|</code>, storing the
result in \a per (if it is not a null pointer). The sign of the result is
stored in \a sign (if it is not a null pointer), i.e. -1 if \a d2 &gt; \a d1 or
+1 if \a d2 &lt;= \a d1.

*/
void iso8601_difference(const struct iso8601_date* d1,
    const struct iso8601_date* d2, struct iso8601_elapsed* per, int* sign)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)))
#endif
;



/*! \brief Divide one period by another.

\param num Numerator.
\param denom Denominator (divisor).
\param[out] remain Remainder. May be 0.
\returns Number of times \a denom divides into \a num.

This function computes the number of times that \a denom can be divided into \a
num, returning that number. If desired, the remaining period which could not be
divided can be written into \a remain.  Uses 64-bit arithmetic internally.

*/
int iso8601_elapsed_div(const struct iso8601_elapsed* num,
    const struct iso8601_elapsed* denom, struct iso8601_elapsed* remain)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)));
#endif
;



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libiso8601/src/libiso8601/999_BottomHeader.h
 *
 *  (c)2006-2010, Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv3. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/

#ifdef __cplusplus
}
#endif

#endif

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/
