
/* libdata-gcf/src/libdata-gcf/000_TopHeader.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/

#ifndef HEADER_libdata_gcf
#define HEADER_libdata_gcf

/* standard includes, or includes needed for type declarations */
#include <stdint.h>
#include <gslutil.h>
#include <iso8601.h>

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-gcf/src/libdata-gcf/100_errstr.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup errstr Error reporting

When an error occurs during GCF parsing, it is reported through the function return code (see
individual functions for details). A human-readable error message is also printed into 
\ref gcf_errstr.

*/
/*!@{*/



/*! \brief Error message.

This variable holds a human-readable error message that refers to the last error which occurred in
any libdata-gcf library function.

*/
extern const char* gcf_errstr;



/*!@}*/



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-gcf/src/libdata-gcf/200_header.h
 *
 *  Copyright: ©2007–2013, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  Author: Kelly Dunlop <kdunlop@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup hdr Header parsing.
 *
 *  The gcf_parse_hdr() routine and associated data types are used to parse the header of a GCF
 *  block. This must be done before it is possible to extract the contents of the block itself.
 */

/*!@{*/



/*! \brief Possible block types.
 *
 *  This enumeration contains all the possible block types. It is set in the \a type field of the
 *  various different header structures.
 */
enum gcf_block_type_t {
    /*! \brief Data block. Uses struct gcf_data_hdr. */
    gcf_block_type_data,
    /*! \brief Status block. Uses struct gcf_status_hdr. */
    gcf_block_type_status,
    /*! \brief Unified status block. Uses struct gcf_status_hdr. */
    gcf_block_type_unified_status,
    /*! \brief Strong motion block. Uses struct gcf_status_hdr. */
    gcf_block_type_strong_motion,
    /*! \brief Continuous data status block. Uses struct gcf_generic_hdr. */
    gcf_block_type_cd_status
};



/*! \brief Possible digitiser types.
 *
 *  This enumeration lists the digitiser types that can be determined from a GCF block header. This
 *  information can be extracted if the extended sysid bit is set.
 */
enum gcf_digitiser_t {
    /*! \brief Unknown (probably, but not necessarily, CMG-DM24mk2). */
    gcf_digitiser_unknown,
    /*! \brief CMG-DM24mk3. */
    gcf_digitiser_dm24mk3,
    /*! \brief CMG-CD24. */
    gcf_digitiser_cd24,
    /*! \brief CMG-DAS (next generation digitiser). */
    gcf_digitiser_das,
};



/*! \brief Possible variable gain values
 *
 *  This enumeration lists the values for the variable gain setting.
 *  There may be no gain front end fitted in which case the value would
 *  be gcf_no_variable_gain otherwise it indicates the value.
 */
enum gcf_variable_gain_t {
    /*! \brief No gain front end fitted. */
    gcf_no_variable_gain,
    /*! \brief Variable gain times 1 */
    gcf_variable_gain_times_one,
    /*! \brief Variable gain times 2 */
    gcf_variable_gain_times_two,
    /*! \brief Variable gain times 4 */
    gcf_variable_gain_times_four,
    /*! \brief Variable gain times 8 */
    gcf_variable_gain_times_eight,
    /*! \brief Variable gain times 16 */
    gcf_variable_gain_times_sixteen,
    /*! \brief Variable gain times 32 */
    gcf_variable_gain_times_thirtytwo,
    /*! \brief Variable gain times 64 */
    gcf_variable_gain_times_sixtyfour
};



/*! \brief Generic header details shared by all GCF blocks.
 *
 *  This contains the header fields common to all types of GCF block: the system and stream ID, the
 *  timestamp, the type of block and the type of digitiser.
 */
struct gcf_generic_hdr {
    /*! \brief System ID. */
    char sysid[7];

    /*! \brief Stream ID. */
    char strid[7];

    /*! \brief Timestamp of packet (start time). */
    struct iso8601_date date;
    
    /*! \brief Type of packet. */
    enum gcf_block_type_t type;

    /*! \brief Type of digitiser. */
    enum gcf_digitiser_t digitiser;
};



/*! \brief Possible sample formats.
 *
 *  This enumeration lists the possible sample formats for a data block.
 */
enum gcf_sample_format_t {
    /*! \brief 32-bit differences. */
    gcf_sample_format_32bit_diff,
    /*! \brief 24-bit differences, modulo 0xFFFFFF. */
    gcf_sample_format_24bit_diff_mod,
    /*! \brief 16-bit differences. */
    gcf_sample_format_16bit_diff,
    /*! \brief 8-bit differences. */
    gcf_sample_format_8bit_diff
};



/*! \brief Header details for a data block.
 *
 *  This structure contains the generic header details from struct gcf_generic_hdr as well as the
 *  details used only for header blocks (sample rate, sample format and number of samples, as well
 *  as the tap table lookup field if present).
 */
struct gcf_data_hdr {
    /*! \brief System ID. */
    char sysid[7];

    /*! \brief Stream ID. */
    char strid[7];

    /*! \brief Timestamp of packet (start time). */
    struct iso8601_date date;
    
    /*! \brief Type of packet. */
    enum gcf_block_type_t type;

    /*! \brief Type of digitiser. */
    enum gcf_digitiser_t digitiser;

    /*! \brief Tap table lookup index. */
    int ttl;

    /*! \brief Variable gain value. */
    enum gcf_variable_gain_t variable_gain;

    /*! \brief Sample format. */
    enum gcf_sample_format_t samp_fmt;

    /*! \brief Sample rate.
     *
     *  The sample rate is 0 for status blocks (so you will not see that in a data header), positive
     *  for samples per second (e.g. a value of 100 would give a sample rate of 100Hz), and negative 
     *  for seconds per sample (e.g. a value of -4 would give 0.25Hz).
     */
    int32_t samp_rat;

    /*! \brief Number of samples in the block. */
    int samp_num;
};



/*! \brief Header details for a status block.
 *
 *  This structure contains the details for a status (or similar) block. This includes the generic 
 *  header details from struct gcf_generic_hdr and also has the length of the textual status 
 *  information in bytes, or number of words of packetised data in the case of strong motion and
 *  unified status.
 */
struct gcf_status_hdr {
    /*! \brief System ID. */
    char sysid[7];

    /*! \brief Stream ID. */
    char strid[7];

    /*! \brief Timestamp of packet (start time). */
    struct iso8601_date date;
    
    /*! \brief Type of packet. */
    enum gcf_block_type_t type;

    /*! \brief Type of digitiser. */
    enum gcf_digitiser_t digitiser;

    /*! \brief Number of bytes (or words) of data. */
    int status_len;
};



/*! \brief Header union type for parsing.
 *
 *  This type simply contains a union of all of the header types and is used for parsing a packet
 *  header (since the output type will not be known at that point). Once parsing is complete, the
 *  type can be determined by looking at \c generic.type.
 */
union gcf_union_hdr {
    /*! \brief Generic header shared by all block types. */
    struct gcf_generic_hdr generic;
    /*! \brief Header for data blocks. */
    struct gcf_data_hdr data;
    /*! \brief Header for status blocks. */
    struct gcf_status_hdr status;
};



/*! \brief Parse header.
 *
 *  \param data Block data.
 *  \param len Block length.
 *  \param[out] hdr Header union into which parsed header data will be written.
 *  \retval 0 on success.
 *  \retval -1 on error (and see \ref gcf_errstr).
 *
 *  This function will parse a GCF block header. It will only access the first 16 bytes of \a data,
 *  regardless of the specified \a len. \a len is used to determine if the data is in the special 
 *  24-bit difference mode used by the DM24mk3 over BRP; it is also used to ensure that the block
 *  contains enough data for the given number of samples. If the block was not transmitted using a
 *  protocol which truncates partially used blocks, then \a len should be set to 1024.
 *
 *  If the block header is invalid, then -1 is returned; otherwise, the parsed header data is valid
 *  and 0 is returned. On error, an informative message is written to \ref gcf_errstr.
 */
int gcf_parse_hdr(const char* data, int len, union gcf_union_hdr* hdr);



/*!@}*/

/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-gcf/src/libdata-gcf/395_block_documentation.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup block Block parsing.
 *
 *  This module contains a set of routines for parsing various block types. Data blocks are decoded 
 *  into an array of integers representing the raw sample values, status blocks have their text
 *  extracted into a string, and CD or unified status packets have their data extracted into a
 *  specific structure.
 */



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-gcf/src/libdata-gcf/400_block_cd_status.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \brief CD status: GPS fix.
\ingroup block */
enum gcf_cd_gps_fix_t {
    /*! \brief No GPS unit attached. */
    gcf_cd_gps_fix_no_comms,
    /*! \brief No fix. */
    gcf_cd_gps_fix_none,
    /*! \brief 2D only fix. */
    gcf_cd_gps_fix_2d,
    /*! \brief Full 3D fix. */
    gcf_cd_gps_fix_3d
};



/*! \brief CD status: GPS control mode.
\ingroup block */
enum gcf_cd_gps_mode_t {
    /*! \brief Automatic. */
    gcf_cd_gps_mode_auto,
    /*! \brief Manual. */
    gcf_cd_gps_mode_manual
};



/*! \brief CD status packet.
 *  \ingroup block
 *
 *  The Continuous Data standard (from the CTBTO) requires some additional real-time status updates.
 *  These updates are sent in a CD status packet, which is decoded into a struct gcf_cd_status.
 */
struct gcf_cd_status {
    /*! \brief GPS fix. */
    enum gcf_cd_gps_fix_t gps_fix;
    /*! \brief GPS control mode. */
    enum gcf_cd_gps_mode_t gps_mode;
    /*! \brief Flag: set if clock is conditioned by GPS. */
    int gps_control;
    /*! \brief Flag: set if GPS is powered up. */
    int gps_power;
    /*! \brief Offset of internal clock from measured GPS time. */
    int32_t gps_offset_05us;
    /*! \brief Set when digitiser is busy with something (lock/unlock/calibrate?). */
    int32_t busy_ms;
    /*! \brief Flag: centring masses. */
    int mass_centring;
    /*! \brief Flag: locking masses. */
    int mass_locking;
    /*! \brief Flag: unlocking masses. */
    int mass_unlocking;
    /*! \brief Flag: calibrating channel 0. */
    int chan_0_calib;
    /*! \brief Flag: calibrating channel 1. */
    int chan_1_calib;
    /*! \brief Flag: calibrating channel 2. */
    int chan_2_calib;
};



/*! \brief Parse a CD status packet.
 *  \ingroup block
 * 
 *  \param hdr The already-parsed CD status header.
 *  \param block Pointer to the start of the block data (including 16-byte header).
 *  \param out Pointer to structure into which CD status information will be stored.
 *  \retval 0 on success.
 *  \retval -1 on error (and see \ref gcf_errstr).
 *
 *  Decodes the information contained in a CD status block, storing the result in \a out. If the
 *  information is in some way invalid, this is recorded in \ref gcf_errstr and -1 is
 *  returned. Otherwise, \a out contains the result and 0 is returned.
 */
int gcf_parse_cd_status(const struct gcf_generic_hdr* hdr, const char* block, struct gcf_cd_status* out);



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-gcf/src/libdata-gcf/400_block_data.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \brief Parse a data block into a fixed array.
\ingroup block

\param hdr The already-parsed data block header.
\param block Pointer to the start of the block data (including 16-byte header).
\param[out] out Array into which raw samples will be stored.
\retval 0 on success.
\retval -1 on error (and see \ref gcf_errstr, \a errno).

This function will extract the raw data samples from a data block. The raw samples are
signed 32-bit values representing the absolute value recorded by the digitiser's ADC. The array
\a out must be large enough to hold all of the decompressed samples.

On success, 0 is returned. On failure, -1 is returned and \a errno and \ref gcf_errstr are set. On
failure, the value of \a errno determines whether anything was written to \a out:
\li \c ENOTSUP &mdash; The encoding format is not understood. \a out will be unmodified.
\li \c EOVERFLOW &mdash; The last sample does not match the RIC (reverse integrating constant).
    \a out will contain samples, but their values may be incorrect (effectively a checksum error).
\li \c EDOM &mdash; The first difference is not zero. \a out will contain samples, but their values
    may be incorrect (the first difference is ignored).

*/
int gcf_parse_data(const struct gcf_data_hdr* hdr, const char* block, int32_t* out)
    __attribute__((nonnull));



/*! \brief Parse a data block into a memory buffer.
\ingroup block

\param hdr The already-parsed data block header.
\param block Pointer to the start of the block data (including 16-byte header).
\param[out] buf Memory buffer into which the samples will be written.
\retval 0 on success.
\retval -1 on error (and see \ref gcf_errstr, \a errno).

This function will extract the raw data samples from a data block. The raw samples are
signed 32-bit values representing the absolute value recorded by the digitiser's ADC.

On success, 0 is returned. On failure, -1 is returned and \a errno and \ref gcf_errstr are set. On
failure, the value of \a errno determines whether anything was written to \a buf:
\li \c ENOTSUP &mdash; The encoding format is not understood. \a buf will be unmodified.
\li \c EOVERFLOW &mdash; The last sample does not match the RIC (reverse integrating constant).
    \a buf will contain samples, but their values may be incorrect (effectively a checksum error).
\li \c EDOM &mdash; The first difference is not zero. \a buf will contain samples, but their values
    may be incorrect (the first difference is ignored).

*/
int gcf_parse_data_membuf(const struct gcf_data_hdr* hdr, const char* block, struct membuf_t* buf)
    __attribute__((nonnull));



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libdata-gcf/src/libdata-gcf/400_block_status.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \brief Parse a status block.
 *  \ingroup block
 *
 *  \param hdr The already-parsed status block header.
 *  \param block Pointer to the start of the block data (including 16-byte header).
 *  \param[out] out String buffer into which the status information will be stored.
 *  \retval 0 on success (cannot fail).
 *  
 *  This function will extract the status information from a status block. The information is simply
 *  copied into the output buffer \a out, and then null terminated. The buffer \a out must be large
 *  enough to hold all the status information and the terminating null byte. In general, it must be
 *  at least 1009 bytes large.
 *
 *  \note This function can also be used to parse byte pipe and infoblock blocks.
 */
int gcf_parse_status(const struct gcf_status_hdr* hdr, const char* block, char* out);



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-gcf/src/libdata-gcf/800_base36.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup base36 Base 36 routines
 *
 *  A small set of routines for manipulating the base 36 IDs used in GCF block headers.
 */

/*!@{*/



/*! \brief Decode a base36 integer into a string.
 *
 *  \param code The integer value to decode.
 *  \param[out] id Buffer into which the string (null-terminated) is written.
 *  \retval 0 on success.
 *  \retval -1 on error (and see \a gcf_errstr).
 *
 *  Decodes the base-36 encoded string in \a code, storing the result in \a id. The buffer \a id
 *  must have at least 7 bytes free (6 for the decoded ID, and one for the terminating null). The
 *  operation can only fail if the topmost bit of \a code is set (since the ID is defined as being
 *  31-bit).
 */
int gcf_base36_decode(uint32_t code, char* id);



/*! \brief Encode a string to a base36 integer.
 *
 *  \param id The string to encode.
 *  \param[out] code The encoded base 36 integer.
 *  \retval 0 on success.
 *  \retval -1 on error (and see \a gcf_errstr).
 *
 *  Encodes the string \a id into a base 36 integer, which is stored in \a code. The operation will
 *  fail if the string is too long or contains characters not in the set A-Z or 0-9. The result will
 *  only ever use 31 bits. The operation is not case sensitive. If the string cannot be encoded, -1
 *  will be returned, nothing will be written to \a code.
 */
int gcf_base36_encode(const char* id, uint32_t* code);



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-gcf/src/libdata-gcf/800_date.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup date Date manipulation routines

A small set of routines for manipulating the dates used in GCF block headers. GCF dates are encoded
into a 32-bit field, the top 15 bits of which represent the number of days elasped since 1989-11-17,
and the bottom 17 bits of which represent the number of seconds elapsed that day since 00:00:00. 
Both ISO8601 and GCF time deal correctly with leap seconds.

*/

/*!@{*/



/*! \brief Decode a GCF date.

\param[out] iso_date The ISO8601 date.
\param gcf_date The date (32-bit encoded value) to decode.
\retval 0 on success.
\retval -1 on error (and see \ref gcf_errstr).

This function will decode a GCF encoded date \a gcf_date and store the result in \a iso_date, also
validating it.

*/
int gcf_date_decode(struct iso8601_date* iso_date, uint32_t gcf_date);



/*! \brief Encode a GCF date.

\param[out] gcf_date The 32-bit encoded date is written here.
\param iso_date The ISO8601 date to encode.
\retval 0 on success.
\retval -1 on error (and see \ref gcf_errstr).

This function will encode an ISO8601 date \a iso_date, storing the result in \a gcf_date. It will
fail if the date in \a iso_date is outside the range that can be represented by GCF dates. Any
nanosecond component of \a iso_date is ignored.

*/
int gcf_date_encode(uint32_t* gcf_date, const struct iso8601_date* iso_date);



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-gcf/src/libdata-gcf/800_transcode.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup transcode Transcode blocks between difference formats.
 *
 *  This module contains a routine which transcodes the 24-bit modulo compression scheme used by a
 *  DM24mk3 over BRP into 32-bit differences used elsewhere. The 24-bit scheme cannot be a primary
 *  encoding because it relies on the block length being available. BRP will preserve the block
 *  length but disk files and Scream! will not.
 */

/*!@{*/



/*! \brief Transcode 24-bit differences to 32-bit differences.
 *
 *  \param[in,out] hdr The header of the data block.
 *  \param[in,out] block The raw block data.
 *  \retval 0 on success.
 *  \retval -1 on error (and see \ref gcf_errstr).
 *
 *  This function will transcode the 24-bit modulo difference compression scheme used by a DM24mk3
 *  over BRP into standard 32-bit differences. It must be passed a valid 24-bit block. It may fail
 *  if there is a checksum error, in which case it will return -1. Otherwise, the data pointed to by
 *  \a block and \a hdr is modified and 0 is returned.
 */
int gcf_transcode_24_to_32(struct gcf_data_hdr* hdr, char* block);



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-gcf/src/libdata-gcf/900_display.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup display Diagnostic display.

A set of routines for displaying blocks for diagnostic purposes.

*/
/*!@{*/



/*! \brief Display block header.

\param hdr Pointer to ready-parsed block header.
\returns Pointer to display information.

Returns a pointer to a line of information about a given block header. The line is null terminated
but does not have a newline character.

*/
const char* gcf_display_header(const union gcf_union_hdr* hdr);



/*! \brief Display block details (including status/samples).

\param block Pointer to block data.
\param len Length of block data (1024 if unknown).
\retval 0 on error (could not parse block; see \a ref gcf_errstr).
\returns Pointer to display information.

Returns a pointer to several lines of information about a given block. The information is null
terminated and contains embedded newlines, but is not followed by a newline character.

*/
const char* gcf_display_block(const char* block, int len);



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-gcf/src/libdata-gcf/999_BottomHeader.h
 *
 *  Copyright: ©2007–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/

#endif

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/
