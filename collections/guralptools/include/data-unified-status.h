
/* libdata-unified-status/src/libdata-unified-status/000_TopHeader.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/

#ifndef HEADER_libdata_unified_status
#define HEADER_libdata_unified_status

/* standard includes, or includes needed for type declarations */
#include <stdint.h>
#include <gslutil.h>
#include <iso8601.h>

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-unified-status/src/libdata-unified-status/100_doxygen.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup tag_processing Tag processing

*/



/*! \defgroup status_parsing Status parsing

*/



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-unified-status/src/libdata-unified-status/300_extract.h
 * 
 *  Copyright: ©2009–2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup extract Tag extraction
\ingroup tag_processing

Allows the analysis of a tag. Will retrieve the tag type (allowing the tag to be fully decoded if
it is of a known type) and length. Since the length is an explicit part of the tag type, unknown
tags can simply be skipped when parsing a complete block.

*/
/*!@{*/



/*! \brief Possible tag types. */
enum unified_status_tag_t {
    /*! \brief Clock status (see \ref clock). */
    unified_status_tag_clock,

    /*! \brief GPS status (see \ref gps). */
    unified_status_tag_gps,

    /*! \brief Channel status (see \ref channel). */
    unified_status_tag_channel,

    /*! \brief Mass position (see \ref mass_pos). */
    unified_status_tag_mass_pos,

    /*! \brief Temperature (see \ref temp). */
    unified_status_tag_temp,

    /*! \brief Inclination (see \ref inclination). */
    unified_status_tag_inclination,
};



/*! \brief Read tag type and length.

\param st Pointer to position in status packet to read.
\param[out] tag_type The overall type of tag, excluding any specific data (e.g.
    which channel it applies to). May be 0.
\param[out] data_words The number of data words. May be 0.
\retval 0 on success.
\retval -1 on error (unrecognised tag).

Reads a 32-bit unified status tag; if the tag type is known, it will write the
type into \a tag_type. This can be used to intepret the data. If the tag type
is unknown, -1 will be returned, but \a data_words will still be set correctly.
This allows a reader to skip unknown tags. The \a data_words count excludes the
tag word itself.

*/
int unified_status_read_tag(const char* st,
    enum unified_status_tag_t* tag_type,
    int* data_words);



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-unified-status/src/libdata-unified-status/300_persistent.h
 * 
 *  Copyright: ©2009–2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup persistent Persistent storage
\ingroup tag_processing

A set of routines to maintain state-of-health information at a high level useful for data
processing. State of health information is held in a <code>struct soh_store_t</code> object, which
holds one object of each type that the library can \ref status_parsing "parse". Blocks of unified
status information are passed to \ref soh_store_update(), along with the generation timestamp. This
information is used to update the store, if relevant (i.e. if the timestamp is later).

\section persistent_compression Compression

Initial unified status support in Guralp Systems digitising equipment causes the emission of one
unified status packet per second. This is somewhat excessive for slow-changing data and can lead to
large bandwidth requirements when using fixed-size chunks to transfer data (i.e. Scream). It is
envisaged that future firmware updates may use a "compression" algorithm to only transmit
significant data. Such an algorithm is implemented by \ref soh_store_update(). Effectively, only
significant changes are transmitted. Changes are significant if:

\li it has been one minute since the last update; or
\li a bitfield entry (such as "clock locked") has changed; or
\li a continuous value entry (such as "clock differential") has changed by more than 10%.

\section persistent_channel Per-channel information

Some status information (\ref channel) is channel-specific. The persistent storage object is also
intended for channel-specific applications. An application must ensure that only the relevant
per-channel status words are passed to \ref soh_store_update().

When storing channel-specific information, the instrument number and component fields will be
stored but their meaning is never examined by this code. Thus if three per-channel updates from a
triaxial instrument were provided to the update function, whichever came last would be used (subject
to the compression rules above). This is clearly not the desirable behaviour, so the status must be
split before being provided to the update function.

\section persistent_retrieve Retrieving stored values

See:
\li \ref soh_store_get_clock_status()
\li \ref soh_store_get_gps_status()
\li \ref soh_store_get_channel_status()

*/
/*!@{*/



/* opaque type */
struct soh_store_t;



/*! \brief Create a new persistent store.

\param warning Optional warning function. May be 0.
\returns Pointer to newly-allocated status storage object.

Allocates and initialises a new persistent status storage object. If desired, a warning function
may be passed as \a warning. This function will be called with a human-readable error message if a
tag cannot be parsed. If not provided, warnings are ignored.

*/
struct soh_store_t* soh_store_new(void (*warning)(const char* message))
#ifndef DOXYGEN
    __attribute__((malloc))
#endif
;



/*! \brief Free a persistent store object. */
void soh_store_free(struct soh_store_t* store);



/*! \brief Update a persistent store object.

\param store Persistent storage object.
\param timestamp Timestamp of latest status information.
\param st Status data.
\param st_words Length of status data, in units of 32-bit words.
\param comp_buf Output buffer into which compressed status is written. May be 0.

Uses the information in \a st to update the persistent store object \a store. The update will only
occur if \a timestamp is later than the last-received data in \a store (earlier timestamps are
ignored), and if the update is significant (see \ref persistent_compression).

If desired, a pointer to a memory buffer may be passed in \a comp_buf. If passed, then any update
after the compression process (i.e. any change to the \a store object) will be serialised, in
unified status words, and appended to \a comp_buf. This mode of operation allows the persistent
store to be used as a compressing filter.

\warning See section \ref persistent_channel.

*/
void soh_store_update(struct soh_store_t* store, const struct iso8601_date* timestamp,
    const char* st, int st_words, struct membuf_t* comp_buf)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2,3)))
#endif
;



/*! \brief Serialise persistent store into unified status words.

\param store Persistent storage object.
\param buf Buffer to write status into.

This function will serialise the status information in \a store and append unified status words
onto \a buf. If \a store was never updated, no information will be written. Otherwise, all the
latest information will be written.

*/
void soh_store_serialise(const struct soh_store_t* store, struct membuf_t* buf)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-unified-status/src/libdata-unified-status/500_tags/000000_clock.h
 * 
 *  Copyright: ©2009–2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup clock Clock status
\ingroup status_parsing

Per-instrument timing status.

*/
/*!@{*/



/*! \brief Maximum (or unknown) clock differential. */
#define UNIFIED_STATUS_MAX_CLOCK_DIFFERENTIAL (-0x7FFFFFFF)



/*! \brief Different types of clock source. */
enum unified_status_clock_source_t {
    unified_status_clock_source_internal_rtc,
    unified_status_clock_source_gps,
    unified_status_clock_source_stream_sync,
    unified_status_clock_source_ntp,
    unified_status_clock_source_accurate_rtc
};



/*! \brief Clock status details. */
struct unified_status_clock_t {
    /*! \brief Set to non-0 if external time source is locked. */
    int locked;
    /*! \brief Clock differential, in microseconds, or
        \ref UNIFIED_STATUS_MAX_CLOCK_DIFFERENTIAL. */
    int differential;
    /*! \brief Clock source. */
    enum unified_status_clock_source_t source;
    /*! \brief Timestamp at which clock was last locked. */
    struct iso8601_date last_locked;
};



/*! \brief Interpret a clock status tag.

\param st Pointer to status data (must coincide with start of tag).
\param[out] out Clock status written here.
\retval 0 on success.
\retval -1 on error.

Interprets clock status tag at \a st, storing the result in \a out.

*/
int unified_status_read_clock(const char* st,
    struct unified_status_clock_t* out);



/*! \brief Compose a clock status tag.

\param buf Buffer to write result into.
\param in Clock status.

Encodes the clock status in \a in, appending the result onto \a buf.

*/
void unified_status_write_clock(struct membuf_t* buf,
    const struct unified_status_clock_t* in);



/*! \brief Retrieve clock status from persistent store object.

\param store Persistent storage object.
\param[out] timestamp Timestamp of last update. May be 0.
\param[out] clock Clock information structure. May be 0.
\retval 0 on success.
\retval -1 on error (no status information seen).

*/
int soh_store_get_clock_status(const struct soh_store_t* store,
    struct iso8601_date* timestamp,
    struct unified_status_clock_t* clock)
#ifndef DOXYGEN
    __attribute__((warn_unused_result,nonnull(1)))
#endif
;



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-unified-status/src/libdata-unified-status/500_tags/000001_gps.h
 * 
 *  Copyright: ©2009–2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup gps GPS receiver status
\ingroup status_parsing

Per-instrument GPS receiver state.

*/
/*!@{*/



/*! \brief GPS receiver fix. */
enum unified_status_gps_fix_t {
    /*! \brief GPS receiver is powered off by the digitiser. */
    unified_status_gps_fix_powered_off = 0x0,
    /*! \brief GPS receiver is powered on, but there is no serial data from it. */
    unified_status_gps_fix_no_comms = 0x1,
    /*! \brief GPS receiver has lost its fix. */
    unified_status_gps_fix_none = 0x2,
    /*! \brief GPS receiver has a 2D fix. */
    unified_status_gps_fix_2D = 0x3,
    /*! \brief GPS receiver has a 3D fix. */
    unified_status_gps_fix_3D = 0x4
};



/*! \brief GPS receiver status details. */
struct unified_status_gps_t {
    /*! \brief GPS fix. */
    enum unified_status_gps_fix_t fix;
    /*! \brief Latitude, in millionths of a degree (negative is south). */
    int32_t latitude;
    /*! \brief Longitude, in millionths of a degree (negative is west). */
    int32_t longitude;
    /*! \brief Elevation, in millimetres. */
    int32_t elevation;
};



/*! \brief Interpret a GPS receiver status tag.

\param st Pointer to status data (must coincide with start of tag).
\param[out] out GPS receiver status written here.
\retval 0 on success.
\retval -1 on error.

Interprets GPS receiver status tag at \a st, storing the result in \a out.

*/
int unified_status_read_gps(const char* st, struct unified_status_gps_t* out);



/*! \brief Compose a GPS receiver status tag.

\param buf Buffer to write result into.
\param in GPS receiver status.

Encodes the GPS receiver status in \a in, appending the result onto \a buf.

*/
void unified_status_write_gps(struct membuf_t* buf,
    const struct unified_status_gps_t* in);



/*! \brief Retrieve GPS receiver status from persistent store object.

\param store Persistent storage object.
\param[out] timestamp Timestamp of last update. May be 0.
\param[out] gps GPS receiver information structure. May be 0.
\retval 0 on success.
\retval -1 on error (no status information seen).

*/
int soh_store_get_gps_status(const struct soh_store_t* store,
    struct iso8601_date* timestamp,
    struct unified_status_gps_t* gps)
#ifndef DOXYGEN
    __attribute__((warn_unused_result,nonnull(1)))
#endif
;



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-unified-status/src/libdata-unified-status/500_tags/000002_temp.h
 * 
 *  Copyright: ©2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup temp Temperature
\ingroup status_parsing

Temperature reading. Temperature is expressed as a value in Kelvin.

*/
/*!@{*/



/*! \brief Temperature details. */
struct unified_status_temp_t {
    /*! \brief Temperature in Kelvin. */
    float temp_K;
};



/*! \brief Interpret temperature tag.

\param st Pointer to status data (must coincide with start of tag).
\param[out] out Temperature written here.
\retval 0 on success.
\retval -1 on error.

Interprets temperature tag at \a st, storing the result in \a out.

*/
int unified_status_read_temp(const char* st,
    struct unified_status_temp_t* out);



/*! \brief Compose a temperature tag.

\param buf Buffer to write result into.
\param in Temperature.

Encodes the temperature in \a in, appending the result onto \a buf.

*/
void unified_status_write_temp(struct membuf_t* buf,
    const struct unified_status_temp_t* in);



/*! \brief Retrieve temperature from persistent store object.

\param store Persistent storage object.
\param[out] timestamp Timestamp of last update. May be 0.
\param[out] temp Temperature structure. May be 0.
\retval 0 on success.
\retval -1 on error (no status information seen).

*/
int soh_store_get_temp(const struct soh_store_t* store,
    struct iso8601_date* timestamp,
    struct unified_status_temp_t* temp)
#ifndef DOXYGEN
    __attribute__((warn_unused_result,nonnull(1)))
#endif
;



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-unified-status/src/libdata-unified-status/500_tags/000003_inclination.h
 * 
 *  Copyright: ©2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup inclination Inclination
\ingroup status_parsing

Inclinometer reading. An inclinometer's reading is expressed in radians, and
will be in the range [-M_PI/2,M_PI/2]. As the value is the result of an analog
to digital conversion, it may lie slightly outside this interval. To be useful,
an inclinometer has two axes: X and Y.

*/
/*!@{*/



/*! \brief Inclination details. */
struct unified_status_inclination_t {
    /*! \brief X-axis inclination in radians. */
    float x_inc;

    /*! \brief Y-axis inclination in radians. */
    float y_inc;
};



/*! \brief Interpret inclination tag.

\param st Pointer to status data (must coincide with start of tag).
\param[out] out Inclination written here.
\retval 0 on success.
\retval -1 on error.

Interprets inclination tag at \a st, storing the result in \a out.

*/
int unified_status_read_inclination(const char* st,
    struct unified_status_inclination_t* out);



/*! \brief Compose a inclination tag.

\param buf Buffer to write result into.
\param in Inclination.

Encodes the inclination in \a in, appending the result onto \a buf.

*/
void unified_status_write_inclination(struct membuf_t* buf,
    const struct unified_status_inclination_t* in);



/*! \brief Retrieve inclination from persistent store object.

\param store Persistent storage object.
\param[out] timestamp Timestamp of last update. May be 0.
\param[out] inclination Inclination structure. May be 0.
\retval 0 on success.
\retval -1 on error (no status information seen).

*/
int soh_store_get_inclination(const struct soh_store_t* store,
    struct iso8601_date* timestamp,
    struct unified_status_inclination_t* inclination)
#ifndef DOXYGEN
    __attribute__((warn_unused_result,nonnull(1)))
#endif
;



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-unified-status/src/libdata-unified-status/500_tags/0001xx_channel.h
 * 
 *  Copyright: ©2009–2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup channel Channel status
\ingroup status_parsing

Per-channel status flags.

*/
/*!@{*/


/*! \brief Channel status flag: sensor is dead. */
#define UNIFIED_STATUS_CHANNEL_DEAD_SENSOR              (0x00000001)
/*! \brief Channel status flag: data is zeroed. */
#define UNIFIED_STATUS_CHANNEL_ZEROED_DATA              (0x00000002)
/*! \brief Channel status flag: digitiser clipped. */
#define UNIFIED_STATUS_CHANNEL_DIGITISER_CLIPPING       (0x00000004)
/*! \brief Channel status flag: calibration signal active. */
#define UNIFIED_STATUS_CHANNEL_CALIBRATING              (0x00000008)
/*! \brief Channel status flag: analogue input shorted. */
#define UNIFIED_STATUS_CHANNEL_INPUT_SHORTED            (0x00000010)
/*! \brief Channel status flag: amplifier is saturated. */
#define UNIFIED_STATUS_CHANNEL_AMPLIFIER_SATURATED      (0x00000080)
/*! \brief Channel status flag: glitches (missing samples) on signal detected. */
#define UNIFIED_STATUS_CHANNEL_GLITCHES                 (0x00000100)
/*! \brief Channel status flag: spikes on signal detected. */
#define UNIFIED_STATUS_CHANNEL_SPIKES                   (0x00000200)
/*! \brief Channel status flag: data is missing or padded. */
#define UNIFIED_STATUS_CHANNEL_MISSING_OR_PADDED        (0x00000400)
/*! \brief Channel status flag: a FIR filter may be charging. */
#define UNIFIED_STATUS_CHANNEL_FIR_CHARGING             (0x00000800)
/*! \brief Channel status flag: mass positions are centring. */
#define UNIFIED_STATUS_CHANNEL_MASS_CENTRE              (0x00001000)



/*! \brief Channel status details. */
struct unified_status_channel_t {
    /*! \brief Instrument number (counting from 0). */
    uint8_t inst_num;
    /*! \brief Channel component name, in ASCII. */
    char component;
    /*! \brief Channel status flags. */
    uint32_t flags;
};



/*! \brief Interpret a channel status tag.

\param st Pointer to status data (must coincide with start of tag).
\param[out] out Channel status written here.
\retval 0 on success.
\retval -1 on error.

Interprets channel status tag at \a st, storing the result in \a out.

*/
int unified_status_read_channel(const char* st,
    struct unified_status_channel_t* out);



/*! \brief Compose a channel status tag.

\param buf Buffer to write result into.
\param in Channel status.

Encodes the channel status in \a in, appending the result onto \a buf.

*/
void unified_status_write_channel(struct membuf_t* buf,
    const struct unified_status_channel_t* in);



/*! \brief Retrieve channel status from persistent store object.

\param store Persistent storage object.
\param[out] timestamp Timestamp of last update. May be 0.
\param[out] channel Channel information structure. May be 0.
\retval 0 on success.
\retval -1 on error (no status information seen).

*/
int soh_store_get_channel_status(const struct soh_store_t* store,
    struct iso8601_date* timestamp,
    struct unified_status_channel_t* channel)
#ifndef DOXYGEN
    __attribute__((warn_unused_result,nonnull(1)))
#endif
;



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-unified-status/src/libdata-unified-status/500_tags/0002xx_mass_pos.h
 * 
 *  Copyright: ©2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup mass_pos Mass position
\ingroup status_parsing

Per-channel mass position reading. The mass position is expressed as a ratio of
full scale deflection. Since the value is the result of analog to digital
conversion, it is possible that the the result could be slightly outside of the
interval [-1.0,1.0].

*/
/*!@{*/



/*! \brief Mass position details. */
struct unified_status_mass_pos_t {
    /*! \brief Channel component name, in ASCII. */
    char component;

    /*! \brief Mass position from -1.0 to +1.0. */
    float pos;
};



/*! \brief Interpret mass position tag.

\param st Pointer to status data (must coincide with start of tag).
\param[out] out Mass position written here.
\retval 0 on success.
\retval -1 on error.

Interprets mass position tag at \a st, storing the result in \a out.

*/
int unified_status_read_mass_pos(const char* st,
    struct unified_status_mass_pos_t* out);



/*! \brief Compose a mass position tag.

\param buf Buffer to write result into.
\param in Mass position.

Encodes the mass position in \a in, appending the result onto \a buf.

*/
void unified_status_write_mass_pos(struct membuf_t* buf,
    const struct unified_status_mass_pos_t* in);



/*! \brief Retrieve mass position from persistent store object.

\param store Persistent storage object.
\param[out] timestamp Timestamp of last update. May be 0.
\param[out] mass_pos Mass position structure. May be 0.
\retval 0 on success.
\retval -1 on error (no status information seen).

*/
int soh_store_get_mass_pos(const struct soh_store_t* store,
    struct iso8601_date* timestamp,
    struct unified_status_mass_pos_t* mass_pos)
#ifndef DOXYGEN
    __attribute__((warn_unused_result,nonnull(1)))
#endif
;



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-unified-status/src/libdata-unified-status/999_BottomHeader.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/

#endif

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/
