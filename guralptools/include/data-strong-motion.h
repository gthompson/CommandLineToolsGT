
/* libdata-strong-motion/src/libdata-strong-motion/000_TopHeader.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/

#ifndef HEADER_libdata_strong_motion
#define HEADER_libdata_strong_motion

/* standard includes, or includes needed for type declarations */

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-strong-motion/src/libdata-strong-motion/300_tag.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup tag Tag interpretation.

Strong motion blocks contain tagged packets. Each packet has a 32-bit tag which gives details of the
packet type, the component or components it is from, and the number of data words. This mechanism
allows for forward compatibility and allows unknown packets to be skipped.

*/
/*!@{*/



/*! \brief Tag type. */
enum strong_motion_tag_t {
    /*! \brief Windowed minimum, maximum and average. */
    strong_motion_tag_windowed_mma,
    /*! \brief Peak ground acceleration. */
    strong_motion_tag_pga,
    /*! \brief Root mean square. */
    strong_motion_tag_rms,
    /*! \brief Cross-correlation. */
    strong_motion_tag_correlation,
    /*! \brief Spectral intensity. */
    strong_motion_tag_si
};



/*! \brief Source instrument. */
enum strong_motion_inst_t {
    /*! \brief Instrument 0. */
    strong_motion_inst_0,
    /*! \brief Instrument 1. */
    strong_motion_inst_1,
    /*! \brief Both instruments (i.e. cross-correlation). */
    strong_motion_inst_both
};



/*! \brief Special component: 2D (horizontal) resultant. */
#define STRONG_MOTION_HORIZONTAL_RESULTANT 'h'
/*! \brief Special component: 3D resultant. */
#define STRONG_MOTION_3D_RESULTANT 'a'



/*! \brief Interpret a 32-bit strong motion packet tag.

\param st Strong motion packet data.
\param[out] type Packet type represented by the tag is written here. May be 0.
\param[out] inst Which instrument the result comes from is written here. May be 0.
\param[out] component Which component the result comes from is written here, as a single character.
    May be 0.
\param[out] nwords Number of data words is written here.
\retval 0 on success.
\retval -1 on error (but \a nwords still valid).

Interprets a 32-bit tag at \a st, writing the results into \a type, \a inst, \a component and 
\a nwords (if they are not 0), returning 0 on success or -1 on error.

An error will occur if the tag type or component code is unrecognised, or if the tag type is 
recognised but the number of data words is not what is expected. In this case, the correct value 
will nevertheless be written to \a nwords (if it is not null). This allows unrecognised tags to be
skipped.

The \a component code will only be set if \a inst is not \ref strong_motion_inst_both. It is a
pointer to a single character, which will be set to '0'&ndash;'9' or 'A'&ndash;'Z' for the 
corresponding digitiser component (usually 'Z'/'N'/'E'), or to the special values 'h' or 'a'
(\ref STRONG_MOTION_HORIZONTAL_RESULTANT and \ref STRONG_MOTION_3D_RESULTANT respectively) for
resolved values.

*/
int strong_motion_read_tag(const char* st, enum strong_motion_tag_t* type,
    enum strong_motion_inst_t* inst, char* component, int* nwords);



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libdata-strong-motion/src/libdata-strong-motion/999_BottomHeader.h
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
