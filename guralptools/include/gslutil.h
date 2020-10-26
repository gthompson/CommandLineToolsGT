
/* libgslutil/src/libgslutil/000_TopHeader.h
 * 
 *  Copyright: ©2009–2011, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */

#ifndef HEADER_libgslutil
#define HEADER_libgslutil

/* standard includes, or includes needed for type declarations */
#include <stdio.h>
#include <netdb.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* options for text editors
vim: expandtab:ts=4:sw=4
*/

/* libgslutil/src/libgslutil/001_enum.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/* Some enumerations are used before they are declared. Since we can't forward-declare an
 * enumeration, but we still want them to appear in the correct place in the Doxygen documentation,
 * such enumerations are forward-declared here with Doxygen group placement commands.
 */



/*! \brief Unix domain socket types.
\ingroup net_unix

Contains a list of the supported socket types in the Unix domain.

*/
enum unix_subproto_t {
    /*! \brief Connection-oriented streams. */
    unix_subproto_stream,
    /*! \brief Connectionless packets. */
    unix_subproto_packet,
    /*! \brief Connection-oriented packets. Only from linux-2.6.4. */
    unix_subproto_seqpacket
};



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/002_flags.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/* Flags that might be used in `flags' fields throughout the API. */



/* ==== I/O flags ==== */



/*! \brief Create file descriptor in non-blocking mode.
\ingroup io

This flag causes the created file descriptor to have non-blocking mode (O_NONBLOCK) set. This
flag has the same numerical value as O_NONBLOCK, but usage of the raw O_NONBLOCK define should
be deprecated.

*/
#define GSL_NONBLOCK (0x00000800)



/*! \brief Open serial port with control lines active.
\ingroup io

*/
#define GSL_SERIAL_PORT_CONTROL_LINES   (0x00000002)



/*! \brief Force IPv6 sockets to bind to only IPv6 addresses.
\ingroup net_ip

This flag causes IPv6 server sockets to bind \em only to IPv6 addresses, not to IPv4-mapped
addresses. Useful when you are binding to multiple wildcard addresses.

*/
#define GSL_IPV6ONLY (0x00000001)



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/050_cstructmacro.h
 * 
 *  Copyright: ©2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup cstructmacro C structure macros

A set of macros for simple, standardised manipulation of C structures.

These macros are taken from CCAN (http://ccodearchive.net/) and are licensed in
the public domain.

*/
/*!@{*/



/**
 * BUILD_ASSERT - assert a build-time dependency.
 * \param cond the compile-time condition which must be true.
 *
 * Your compile will fail if the condition isn't true, or can't be evaluated
 * by the compiler.  This can only be used within a function.
 *
 * Example:
 * <pre>
 *	#include <stddef.h>
 *	...
 *	static char *foo_to_char(struct foo *foo)
 *	{
 *		// This code needs string to be at start of foo.
 *		BUILD_ASSERT(offsetof(struct foo, string) == 0);
 *		return (char *)foo;
 *	}
 * </pre>
 */
#define BUILD_ASSERT(cond) \
	do { (void) sizeof(char [1 - 2*!(cond)]); } while(0)



/**
 * BUILD_ASSERT_OR_ZERO - assert a build-time dependency, as an expression.
 * \param cond the compile-time condition which must be true.
 *
 * Your compile will fail if the condition isn't true, or can't be evaluated
 * by the compiler.  This can be used in an expression: its value is "0".
 *
 * Example:
 * <pre>
 *	#define foo_to_char(foo)					\
 *		 ((char *)(foo)						\
 *		  + BUILD_ASSERT_OR_ZERO(offsetof(struct foo, string) == 0))
 * </pre>
 */
#define BUILD_ASSERT_OR_ZERO(cond) \
	(sizeof(char [1 - 2*!(cond)]) - 1)



/**
 * check_type - issue a warning or build failure if type is not correct.
 * \param expr the expression whose type we should check (not evaluated).
 * \param type the exact type we expect the expression to be.
 *
 * This macro is usually used within other macros to try to ensure that a macro
 * argument is of the expected type.  No type promotion of the expression is
 * done: an unsigned int is not the same as an int!
 *
 * check_type() always evaluates to 0.
 *
 * If your compiler does not support typeof, then the best we can do is fail
 * to compile if the sizes of the types are unequal (a less complete check).
 *
 * Example:
 * <pre>
 *	// They should always pass a 64-bit value to _set_some_value!
 *	#define set_some_value(expr)			\
 *		_set_some_value((check_type((expr), uint64_t), (expr)))
 * </pre>
 */
#define check_type(expr, type)			\
	((typeof(expr) *)0 != (type *)0)



/**
 * check_types_match - issue a warning or build failure if types are not same.
 * \param expr1 the first expression (not evaluated).
 * \param expr2 the second expression (not evaluated).
 *
 * This macro is usually used within other macros to try to ensure that
 * arguments are of identical types.  No type promotion of the expressions is
 * done: an unsigned int is not the same as an int!
 *
 * check_types_match() always evaluates to 0.
 *
 * If your compiler does not support typeof, then the best we can do is fail
 * to compile if the sizes of the types are unequal (a less complete check).
 *
 * Example:
 * <pre>
 *	// Do subtraction to get to enclosing type, but make sure that
 *	// pointer is of correct type for that member. 
 *	#define container_of(mbr_ptr, encl_type, mbr)			\
 *		(check_types_match((mbr_ptr), &((encl_type *)0)->mbr),	\
 *		 ((encl_type *)						\
 *		  ((char *)(mbr_ptr) - offsetof(enclosing_type, mbr))))
 * </pre>
 */
#define check_types_match(expr1, expr2)		\
	((typeof(expr1) *)0 != (typeof(expr2) *)0)



/**
 * ARRAY_SIZE - get the number of elements in a visible array
 * \param arr the array whose size you want.
 *
 * This does not work on pointers, or arrays declared as [], or
 * function parameters.  With correct compiler support, such usage
 * will cause a build error (see build_assert).
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + _array_size_chk(arr))

#ifndef DOXYGEN
/* Two gcc extensions.
 * &a[0] degrades to a pointer: a different type from an array */
#define _array_size_chk(arr)                                            \
        BUILD_ASSERT_OR_ZERO(!__builtin_types_compatible_p(typeof(arr), \
                                                        typeof(&(arr)[0])))
#endif



/**
 * container_of - get pointer to enclosing structure
 * \param member_ptr pointer to the structure member
 * \param containing_type the type this member is within
 * \param member the name of this member within the structure.
 *
 * Given a pointer to a member of a structure, this macro does pointer
 * subtraction to return the pointer to the enclosing type.
 *
 * Example:
 * <pre>
 *	struct foo {
 *		int fielda, fieldb;
 *		// ...
 *	};
 *	struct info {
 *		int some_other_field;
 *		struct foo my_foo;
 *	};
 *
 *	static struct info *foo_to_info(struct foo *foo)
 *	{
 *		return container_of(foo, struct info, my_foo);
 *	}
 * </pre>
 */
#define container_of(member_ptr, containing_type, member)		\
	 ((containing_type *)						\
	  ((char *)(member_ptr)						\
	   - container_off(containing_type, member))			\
	  + check_types_match(*(member_ptr), ((containing_type *)0)->member))



/**
 * container_off - get offset to enclosing structure
 * \param containing_type the type this member is within
 * \param member the name of this member within the structure.
 *
 * Given a pointer to a member of a structure, this macro does
 * typechecking and figures out the offset to the enclosing type.
 *
 * Example:
 * <pre>
 *	struct foo {
 *		int fielda, fieldb;
 *		// ...
 *	};
 *	struct info {
 *		int some_other_field;
 *		struct foo my_foo;
 *	};
 *
 *	static struct info *foo_to_info(struct foo *foo)
 *	{
 *		size_t off = container_off(struct info, my_foo);
 *		return (void *)((char *)foo - off);
 *	}
 * </pre>
 */
#define container_off(containing_type, member)	\
	offsetof(containing_type, member)



/**
 * container_of_var - get pointer to enclosing structure using a variable
 * \param member_ptr pointer to the structure member
 * \param container_var a pointer of same type as this member's container
 * \param member the name of this member within the structure.
 *
 * Given a pointer to a member of a structure, this macro does pointer
 * subtraction to return the pointer to the enclosing type.
 *
 * Example:
 * <pre>
 *	static struct info *foo_to_i(struct foo *foo)
 *	{
 *		struct info *i = container_of_var(foo, i, my_foo);
 *		return i;
 *	}
 * </pre>
 */
#define container_of_var(member_ptr, container_var, member) \
	container_of(member_ptr, typeof(*container_var), member)



/**
 * container_off_var - get offset of a field in enclosing structure
 * \param var a pointer to a container structure
 * \param member the name of a member within the structure.
 *
 * Given (any) pointer to a structure and a its member name, this
 * macro does pointer subtraction to return offset of member in a
 * structure memory layout.
 *
 */
#define container_off_var(var, member)		\
	container_off(typeof(*var), member)



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/100_safe_malloc.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup safe_malloc Safely allocate memory
 *
 *  This module contains a set of functions for safely allocating memory. This may involve zeroing
 *  the memory before it is returned and aborting if the allocation fails.
 */
/*!@{*/



/*! \brief Allocate memory.
 *
 *  \param amt Amount (in bytes) to allocate.
 *  \returns Valid pointer to new memory.
 *
 *  This function will allocate \a amt bytes of memory, returning a pointer to the new block. Should
 *  the allocation fail, it is logged to syslog and the program aborted with \c abort(). The newly
 *  allocated block is zeroed with \c memset(). Returns a unique pointer even if \a amt is zero.
 */
#ifdef DOXYGEN
void* safe_malloc(size_t amt);
#else
#define safe_malloc(amt) safe_malloc_(amt, __FUNCTION__, __LINE__)
void* safe_malloc_(size_t amt, const char* function, int line) 
    __attribute__((malloc,nonnull,warn_unused_result));
#endif



/*! \brief Dynamically resize allocated memory.
 *
 *  \param p Existing pointer (may be 0).
 *  \param amt New size to allocate.
 *  \returns Valid pointer to new memory.
 *
 *  This function will allocate \a amt bytes of memory, possibly by resizing the block at \a p (if
 *  it is not null), but in any case keeping any existing contents of \a p. Should the allocation
 *  fail, it is logged to syslog and the program aborted with \c abort(). Returns a unique pointer
 *  even if \a amt is zero.
 */
#ifdef DOXYGEN
void* safe_realloc(void* p, size_t amt);
#else
#define safe_realloc(p, amt) safe_realloc_(p, amt, __FUNCTION__, __LINE__)
void* safe_realloc_(void* p, size_t amt, const char* function, int line)
    __attribute__((nonnull(3),warn_unused_result));
#endif



/*! \brief Copy a string.
 *
 *  \param str Existing string (may not be 0).
 *  \returns Valid pointer to newly-allocated and copied string.
 *
 *  This function will copy \a str (which may not be 0), allocating the required amount of memory
 *  for it. Should the allocation fail, it is logged to syslog and the program aborted with 
 *  \c abort().
 */
#ifdef DOXYGEN
char* safe_strdup(const char* str);
#else
#define safe_strdup(str) safe_strdup_(str, __FUNCTION__, __LINE__)
char* safe_strdup_(const char* str, const char* function, int line)
    __attribute__((nonnull,malloc,warn_unused_result));
#endif



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/200_serialise/000_doxygen.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup serialise Serialised I/O functions
 *
 *  This module contains a set of functions for serialised I/O to memory. The functions are useful
 *  since they are architecture-independent. The \ref serialise_string (\c reads_* and \c writes_*
 *  functions) operate on raw sections of memory; \ref serialise_membuf (\c readm_* and \c writem_*
 *  functions) use a dynamic buffer object for convenience.
 */



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/200_serialise/100_string.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup serialise_string Serialised string functions
 *  \ingroup serialise
 *
 *  This set of functions is used to perform architecture-independent reads/writes from/to memory.
 *  The functions each take a pointer to memory (as \c char*), and return a pointer to the next
 *  location in memory after they have read their required data.
 */
/*!@{*/



/*! \brief Write integer into memory string (8-bit).
 *
 *  \param where Pointer to write position.
 *  \param i Integer to write.
 *  \returns Pointer to position at which next write should occur.
 *
 *  Writes an integer, in an architecture-independent manner, into the memory buffer at \a where. It
 *  is the caller's responsibility to ensure there is enough space.
 *
 *  \sa writes_i16(), writes_i24(), writes_i32(), writes_i64()
 */
char* writes_i8(char* where, uint8_t i) __attribute__((nonnull));

/*! \brief Write integer into memory string (16-bit). */
char* writes_i16(char* where, uint16_t i) __attribute__((nonnull));
/*! \brief Write integer into memory string (24-bit). */
char* writes_i24(char* where, uint32_t i) __attribute__((nonnull));
/*! \brief Write integer into memory string (32-bit). */
char* writes_i32(char* where, uint32_t i) __attribute__((nonnull));
/*! \brief Write integer into memory string (64-bit). */
char* writes_i64(char* where, uint64_t i) __attribute__((nonnull));

/*! \brief Write a 32-bit IEEE floating point. */
char* writes_f32(char* where, float f) __attribute__((nonnull));
/*! \brief Write a 64-bit IEEE floating point. */
char* writes_f64(char* where, double f) __attribute__((nonnull));



/*! \brief Write null-terminated string into memory.
 *
 *  \param where Pointer to write position.
 *  \param str String to write.
 *  \returns Pointer to position at which next write should occur.
 *
 *  Writes a string, in an architecture-independent manner, into the memory buffer at \a where. It
 *  is the caller's responsibility to ensure there is enough space (length of string + 4 bytes). The
 *  length of the string is written into memory as a prefix so that reading can be efficient. 
 *  Written as length (32-bit integer) followed by data, without terminating null.
 */
char* writes_str(char* where, const char* str) __attribute__((nonnull));



/*! \brief Write raw data into memory.
 *
 *  \param where Pointer to write position.
 *  \param mem The raw data to write.
 *  \param amt The number of bytes to write.
 *  \returns Pointer to position at which next write should occur.
 *
 *  Writes a set of bytes, in an architecture-independent manner, into the buffer at \a where. It is
 *  the caller's responsibility to ensure there is enough space (\a amt bytes). The length is not
 *  recorded, so it must either be written first or fixed in the reader and writer.
 */
char* writes_mem(char* where, const char* mem, uint32_t amt) __attribute__((nonnull));



/*! \brief Read integer from memory (8-bit).
 *
 *  \param where Position to read from.
 *  \param[out] i Integer value read.
 *  \returns Position at which next read should occur.
 *
 *  This function reads an integer written with writes_i8() or writem_i8(). It works in an
 *  architecture-independent manner. It returns a pointer to the next unused data in the buffer. It
 *  is the caller's responsibility to ensure the buffer contains enough data.
 *
 *  \sa read_i16(), read_i32(), read_i64()
 */
const char* reads_i8(const char* where, uint8_t* i) __attribute__((nonnull));

/*! \brief Read integer from memory (16-bit). */
const char* reads_i16(const char* where, uint16_t* i) __attribute__((nonnull));
/*! \brief Read integer from memory (24-bit). */
const char* reads_i24(const char* where, uint32_t* i) __attribute__((nonnull));
/*! \brief Read integer from memory (24-bit), and sign extend it to 32-bit. */
const char* reads_i24s(const char* where, int32_t* i) __attribute__((nonnull));
/*! \brief Read integer from memory (32-bit). */
const char* reads_i32(const char* where, uint32_t* i) __attribute__((nonnull));
/*! \brief Read integer from memory (64-bit). */
const char* reads_i64(const char* where, uint64_t* i) __attribute__((nonnull));

/*! \brief Read a 32-bit IEEE floating point from memory. */
const char* reads_f32(const char* where, float* f) __attribute__((nonnull));
/*! \brief Read a 64-bit IEEE floating point from memory. */
const char* reads_f64(const char* where, double* f) __attribute__((nonnull));



/*! \brief Read null-terminated string from memory.
 *
 *  \param where Position to read from.
 *  \param str Pointer to array into which to read string data.
 *  \param amt Number of bytes available in \a str.
 *  \returns Position at which next read should occur.
 *
 *  This function reads a string written with writes_str() or writem_str(). It works in an
 *  architecture-independent manner. It returns a pointer to the next unused data in the buffer. It
 *  is the caller's responsibility to ensure the buffer contains enough data.
 *
 *  If the string in the buffer is too large to fit into \a str, it is truncated. However, in any
 *  case, the output string will always be null terminated and the return value will always point to
 *  the correct location.
 */
const char* reads_str(const char* where, char* str, uint32_t amt) __attribute__((nonnull));



/*! \brief Read arbitrary data from memory.
 *
 *  \param where Position to read from.
 *  \param mem Pointer to array into which to read arbitrary data.
 *  \param amt Number of bytes to read.
 *  \returns Position at which next read should occur.
 *
 *  This function reads arbitrary data, possibly written with writes_mem() or writem_mem(). It works
 *  in an architecture-independent manner. It returns a pointer to the next unused data in the
 *  buffer. It is the caller's responsibility to ensure the buffer contains enough data.
 */
const char* reads_mem(const char* where, char* mem, uint32_t amt) __attribute__((nonnull));



/*! \brief Read null-terminated string from memory (allocate memory).
 *
 *  \param where Position to read from.
 *  \param[out] str Pointer to newly-allocated string.
 *  \param[out] amt Size of string (excluding terminating null, may be 0).
 *  \returns Position at which next read should occur.
 *
 *  Like reads_str(), only this version will allocate the required memory with safe_malloc().
 */
const char* reads_stra(const char* where, char** str, uint32_t* amt) __attribute__((nonnull));



/*! \brief Read arbitrary data from memory (allocate memory).
 *
 *  \param where Position to read from.
 *  \param[out] mem Pointer to newly-allocated array.
 *  \param amt Number of bytes to allocate and read.
 *  \returns Position at which next read should occur.
 *
 *  Like reads_mem(), only this version will allocate the required memory with safe_malloc().
 */
const char* reads_mema(const char* where, char** mem, uint32_t amt) __attribute__((nonnull));



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/200_serialise/200_membuf.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup serialise_membuf Serialised memory buffer functions
 *  \ingroup serialise
 *
 *  This set of functions is used to perform architecture-independent reads/writes from/to memory.
 *  An opaque type, <code>struct membuf</code>, is used. This must be allocated with membuf_new() or
 *  membuf_realloc() and freed with membuf_free().
 *
 *  The write functions will append to the buffer and
 *  automatically resize it as required (using safe_realloc()). The read functions will fail if
 *  there is not enough data in the buffer for the request to succeed; otherwise, the data will be
 *  removed from the buffer. This is implemented with efficient caching/resizing to minimise the
 *  number of calls to realloc() or memmove() that occur.
 */
/*!@{*/



/* !\brief Opaque structure used as a dynamic memory buffer object. */
struct membuf_t;



/*! \brief Allocate a new memory buffer. */
struct membuf_t* membuf_new(void) __attribute__((malloc,warn_unused_result));

/*! \brief Allocate new buffer or clear existing buffer. */
struct membuf_t* membuf_realloc(struct membuf_t* old) __attribute__((warn_unused_result));



/*! \brief Get details of memory buffer.
 *
 *  \param m Memory buffer object.
 *  \param[out] buf Pointer to data written here.
 *  \param[out] len Length of data in bytes written here.
 *
 *  Use this function once you have written some data into the memory buffer to get at the data. The
 *  returned values become invalid as soon as any more data is written into the buffer.
 */
void membuf_use(const struct membuf_t* m, const char** buf, uint32_t* len) __attribute__((nonnull));



/*! \brief Free used memory buffer. */
void membuf_free(struct membuf_t* m);



/*! \brief Mark some bytes in buffer as consumed.
 *
 *  \param m Memory buffer object.
 *  \param amt Number of bytes consumed.
 *
 *  This function will mark \a amt bytes in the buffer \a m as consumed. This does not involve
 *  moving any data in the buffer; moves only occur if required in one of the \c writem_* functions.
 *  Using this function will invalidate the results of any previous call to membuf_use().
 */
void membuf_consume(struct membuf_t* m, uint32_t amt) __attribute__((nonnull));



/*! \brief Truncate the memory buffer.

\param m Memory buffer object.
\param new_len The new length of the buffer.

This function will truncate the buffer so that it only contains \a new_len bytes. If the buffer
does not contain enough data to be affected, \a m will not be changed.

*/
void membuf_truncate(struct membuf_t* m, uint32_t new_len) __attribute__((nonnull));



/*! \brief Write integer to memory buffer (8-bit).
 *
 *  \param m Memory buffer object.
 *  \param i Integer to write.
 */
void writem_i8(struct membuf_t* m, uint8_t i) __attribute__((nonnull));

/*! \brief Write integer to memory buffer (16-bit). */
void writem_i16(struct membuf_t* m, uint16_t i) __attribute__((nonnull));
/*! \brief Write integer to memory buffer (24-bit). */
void writem_i24(struct membuf_t* m, uint32_t i) __attribute__((nonnull));
/*! \brief Write integer to memory buffer (32-bit). */
void writem_i32(struct membuf_t* m, uint32_t i) __attribute__((nonnull));
/*! \brief Write integer to memory buffer (64-bit). */
void writem_i64(struct membuf_t* m, uint64_t i) __attribute__((nonnull));

/*! \brief Write 32-bit IEEE float to memory buffer. */
void writem_f32(struct membuf_t* m, float f) __attribute__((nonnull));
/*! \brief Write 64-bit IEEE float to memory buffer. */
void writem_f64(struct membuf_t* m, double f) __attribute__((nonnull));



/*! \brief Write null-terminated string to memory buffer.
 *
 *  \param m Memory buffer object.
 *  \param str String to write.
 *
 *  The string is written as the length (32-bit integer) followed by the data, without the
 *  terminating null.
 */
void writem_str(struct membuf_t* m, const char* str) __attribute__((nonnull));



/*! \brief Write raw data to memory buffer.
 *
 *  \param m Memory buffer object.
 *  \param mem Pointer to bytes to write.
 *  \param amt Number of bytes to write.
 */
void writem_mem(struct membuf_t* m, const char* mem, uint32_t amt) __attribute__((nonnull));



/*! \brief Read 8-bit integer from memory buffer.
 *
 *  \param m Memory buffer to read from.
 *  \param[out] out Integer to write to.
 *  \retval 0 on success.
 *  \retval -1 on error (not enough data in buffer).
 *
 *  This function will read and consume an 8-bit integer from a memory buffer. If there is not
 *  enough data in the buffer, it will fail and return -1.
 *
 *  \sa readm_i16(), readm_i32(), readm_i64().
 */
int readm_i8(struct membuf_t* m, uint8_t* out) __attribute__((nonnull));
/*! \brief Read 16-bit integer from memory buffer. */
int readm_i16(struct membuf_t* m, uint16_t* out) __attribute__((nonnull));
/*! \brief Read 24-bit integer from memory buffer. */
int readm_i24(struct membuf_t* m, uint32_t* out) __attribute__((nonnull));
/*! \brief Read 24-bit integer from memory buffer, and sign-extend it. */
int readm_i24s(struct membuf_t* m, int32_t* out) __attribute__((nonnull));
/*! \brief Read 32-bit integer from memory buffer. */
int readm_i32(struct membuf_t* m, uint32_t* out) __attribute__((nonnull));
/*! \brief Read 64-bit integer from memory buffer. */
int readm_i64(struct membuf_t* m, uint64_t* out) __attribute__((nonnull));

/*! \brief Read a 32-bit float from memory buffer. */
int readm_f32(struct membuf_t* m, float* out) __attribute__((nonnull));
/*! \brief Read a 64-bit float from memory buffer. */
int readm_f64(struct membuf_t* m, double* out) __attribute__((nonnull));



/*! \brief Read string from memory buffer.
 *
 *  \param m Memory buffer to read from.
 *  \param str Pointer to allocated storage.
 *  \param str_size Number of bytes in \a str.
 *  \param[out] len_out Length of resultant string (may be 0).
 *  \retval 0 on success.
 *  \retval -1 on error (not enough data in buffer).
 *
 *  This function will read a string from a buffer. It first reads the 32-bit string length as
 *  written by writem_str() or writes_str(), and then the string data itself. If the string is
 *  larger than the \a str array, then \a str will contain a truncated version of the string.
 *  \a str will always be null terminated.
 *
 *  If there is not enough data in the buffer, it will fail and return -1.
 */
int readm_str(struct membuf_t* m, char* str, uint32_t str_size, uint32_t* len_out) __attribute__((nonnull(1,2)));



/*! \brief Read bytes from memory buffer.
 *
 *  \param m Memory buffer to read from.
 *  \param mem Pointer to allocated storage.
 *  \param mem_size Number of bytes to read into \a mem.
 *  \retval 0 on success.
 *  \retval -1 on error.
 *
 *  Reads \a mem_size bytes of data from \a m into \a mem. If there is not enough data in the 
 *  buffer, it will fail and return -1.
 */
int readm_mem(struct membuf_t* m, char* mem, uint32_t mem_size) __attribute__((nonnull));



/*! \brief Read string from memory buffer, allocating space for result. */
int readm_stra(struct membuf_t* m, char** str, uint32_t* amt) __attribute__((nonnull(2)));
/*! \brief Read bytes from memory buffer, allocating space for result. */
int readm_mema(struct membuf_t* m, char** mem, uint32_t amt) __attribute__((nonnull));



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/200_serialise/300_vint.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup serialise_vint Variable-length integer packing
\ingroup serialise

This module provides a set of functions for packing and unpacking integers
to/from an efficient form which contains only the bits of the number that are
actually used. The resulting packed objects are a variable number of bytes long
(from 1 to 10, for a full 64-bit number). This packing is most efficient for
integers of small numerical magnitude.

The first bit of each byte is the continuation bit. If it is set, then another
byte of data follows.  If it is unset, then this is the last byte of data. In a
multi-byte number, the last byte can never be 0x00; this means that there is no
ambiguity of representation.

For an unsigned number, the first byte contains the 7 least significant bits of
the number. Each byte thereafter contains the 7 next most significant bits.
Once all bits set in the original number have been stored in bytes, the packing
is complete.

Signed numbers are \em not transmitted as 2's complement (as this would defeat
the short packing scheme used above), but instead as a sign bit and magnitude.
The sign bit is transmitted along with the least significant byte in the
result, which means the first byte contains only the 6 least significant bits
of the number.  The sign bit is the least significant bit of the first byte.

The decoding algorithm for an unsigned number is as follows:

\li Initialise Y (output) to 0, and S (shift bits) to 0
\li Read byte of input into X
\li Store and strip off continuation bit from X
\li Left shift X by S
\li Bitwise OR X into Y
\li Increase S by 7
\li If continuation bit was set in X, go to step 2

The decoding algorithm for a signed number is as follows:

\li Initialise Y (output) to 0, and S (shift bits) to 0
\li Read byte of input into X
\li Store and strip off continuation bit from X
\li Store least significant bit of X as sign bit
\li Right shift X by 1
\li Store X into Y
\li Increase S by 6
\li If continuation bit was set, move to unsigned decoder
\li If sign bit was set, multiply result by -1

A diagram show the structure of the coded numbers is below:

<pre>Unsigned, only 7 bits
---------------------
byte    (MSB) bit meanings (LSB)
 0       [0]: continuation bit clear        [xxx xxxx]: data

Unsigned, &gt; 7 bits
------------------
byte    (MSB) bit meanings (LSB)
 0       [1]: continuation bit set          [xxx xxxx]: data least significant 7 bits
...      [1]: continuation bit set          [xxx xxxx]: data next most significant 7 bits
 n       [0]: continuation bit clear        [xxx xxxx]: data most significant 7 bits

Signed, only 6 bits
-------------------
byte    (MSB) bit meanings (LSB)
 0       [0]: continuation bit clear        [xxx xxx]: data     [s]: sign

Signed, > 6 bits
----------------
byte    (MSB) bit meanings (LSB)
 0       [1]: continuation bit set          [xxx xxx]: data     [s]: sign
...      [1]: continuation bit set          [xxx xxxx]: data next most significant 7 bits
 n       [0]: continuation bit clear        [xxx xxxx]: data most significant 7 bits</pre>

*/
/*!@{*/



/*! \brief Pack a 16-bit unsigned integer.

\param val Value to pack.
\param buf Buffer to write to (must have at least 3 bytes free).
\returns Number of bytes written.

*/
int vint_pack_u16(uint16_t val, char* buf) __attribute__((nonnull));



/*! \brief Pack a 16-bit signed integer.

\param val Value to pack.
\param buf Buffer to write to (must have at least 3 bytes free).
\returns Number of bytes written.

*/
int vint_pack_s16(int16_t val, char* buf) __attribute__((nonnull));



/*! \brief Pack a 32-bit unsigned integer.

\param val Value to pack.
\param buf Buffer to write to (must have at least 5 bytes free).
\returns Number of bytes written.

*/
int vint_pack_u32(uint32_t val, char* buf) __attribute__((nonnull));



/*! \brief Pack a 32-bit signed integer.

\param val Value to pack.
\param buf Buffer to write to (must have at least 5 bytes free).
\returns Number of bytes written.

*/
int vint_pack_s32(int32_t val, char* buf) __attribute__((nonnull));



/*! \brief Pack a 64-bit unsigned integer.

\param val Value to pack.
\param buf Buffer to write to (must have at least 10 bytes free).
\returns Number of bytes written.

*/
int vint_pack_u64(uint64_t val, char* buf) __attribute__((nonnull));



/*! \brief Pack a 64-bit signed integer.

\param val Value to pack.
\param buf Buffer to write to (must have at least 10 bytes free).
\returns Number of bytes written.

*/
int vint_pack_s64(int64_t val, char* buf) __attribute__((nonnull));



/*! \brief Unpack a 16-bit unsigned integer.

\param buf Buffer to read from.
\param buf_len Number of bytes in \a buf.
\param[out] val Retrieved value.
\returns Number of bytes read.
\retval -1 on error.

*/
int vint_unpack_u16(const char* buf, int buf_len, uint16_t* val) __attribute__((nonnull));



/*! \brief Unpack a 16-bit signed integer.

\param buf Buffer to read from.
\param buf_len Number of bytes in \a buf.
\param[out] val Retrieved value.
\returns Number of bytes read.
\retval -1 on error.

*/
int vint_unpack_s16(const char* buf, int buf_len, int16_t* val) __attribute__((nonnull));



/*! \brief Unpack a 32-bit unsigned integer.

\param buf Buffer to read from.
\param buf_len Number of bytes in \a buf.
\param[out] val Retrieved value.
\returns Number of bytes read.
\retval -1 on error.

*/
int vint_unpack_u32(const char* buf, int buf_len, uint32_t* val) __attribute__((nonnull));



/*! \brief Unpack a 32-bit signed integer.

\param buf Buffer to read from.
\param buf_len Number of bytes in \a buf.
\param[out] val Retrieved value.
\returns Number of bytes read.
\retval -1 on error.

*/
int vint_unpack_s32(const char* buf, int buf_len, int32_t* val) __attribute__((nonnull));



/*! \brief Unpack a 64-bit unsigned integer.

\param buf Buffer to read from.
\param buf_len Number of bytes in \a buf.
\param[out] val Retrieved value.
\returns Number of bytes read.
\retval -1 on error.

*/
int vint_unpack_u64(const char* buf, int buf_len, uint64_t* val) __attribute__((nonnull));



/*! \brief Unpack a 64-bit signed integer.

\param buf Buffer to read from.
\param buf_len Number of bytes in \a buf.
\param[out] val Retrieved value.
\returns Number of bytes read.
\retval -1 on error.

*/
int vint_unpack_s64(const char* buf, int buf_len, int64_t* val) __attribute__((nonnull));



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/200_serialise/400_template.h
 * 
 *  Copyright: ©2009–2011, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup serialise_template Read serialised string using template
\ingroup serialise

A set of high-level routines which use simple string templates to parse a serialised bitstring,
unpacking the results into a variable set of arguments passed using the stdarg mechanism.

\section serialise_template_fmt Template string format

Each character in the template string specifies a variable to be converted. Each character must
correspond to at least one argument in the variable argument list.

<table>
    <tr><td><b>Conversion character</b></td><td><b>Description</b></td><td><b>Arguments</b></td></tr>
    <tr><td>1</td><td>8-bit integer</td><td>uint8_t*</td></tr>
    <tr><td>2</td><td>16-bit integer</td><td>uint16_t*</td></tr>
    <tr><td>4</td><td>32-bit integer</td><td>uint32_t*</td></tr>
    <tr><td>8</td><td>64-bit integer</td><td>uint64_t*</td></tr>
    <tr><td>f</td><td>32-bit float</td><td>float*</td></tr>
    <tr><td>d</td><td>64-bit double</td><td>double*</td></tr>
    <tr><td>s</td><td>String</td><td>size_t*, char**</td></tr>
    <tr><td>S</td><td>Size-limited string</td><td>size_t, size_t*, char**</td></tr>
    <tr><td>m</td><td>Memory</td><td>size_t, void*</td></tr>
</table>

The basic scalar types are trivial; each character corresponds to one argument, which is a pointer
to the memory where the result should be stored after conversion.

The string type takes a pointer to a size_t argument, which is set to the length of the string
(excluding terminating null). The char** argument is set to point to a newly-allocated buffer which
contains the string (with an extra null terminator). The size-limited string type checks its first
size_t argument and does not perform the conversion if the recorded size string exceeds this value.

The memory type takes a size_t argument, which is the number of bytes to read, and a pointer to
a pre-allocated buffer into which the memory should be copied.

*/
/*!@{*/



/*! \brief Read serialised string using template.

\param ser Pointer to the serialised string.
\param len Length of serialised string, in bytes.
\param[out] used Set to the number of bytes read from \a ser.
\param tpl Template string (see \ref serialise_template_fmt "format").
\returns Number of items parsed.

This function uses the template string \a template to parse the serialised string \a ser. It will
never read more than \a len bytes from \a ser. It sets \a *used to the number of bytes actually
used during the read operation, and returns the number of items successfully read.

*/
int reads_template(const char* ser, size_t len, size_t* used, const char* tpl, ...);



/*! \brief Read serialised memory buffer using template.

\param buf Memory buffer.
\param tpl Template string (see \ref serialise_template_fmt "format").
\returns Number of items parsed.

This function uses the template string \a template to parse the serialised memory buffer \a buf. It
will consume any memory read from \a buf before returning. It returns the number of items
successfully read. Implemented as a simple wrapper around \ref reads_template().

*/
int readm_template(struct membuf_t* buf, const char* tpl, ...);



/*! \brief Read serialised string using template [varargs].

\param ser Pointer to the serialised string.
\param len Length of serialised string, in bytes.
\param[out] used Set to the number of bytes read from \a ser.
\param tpl Template string (see \ref serialise_template_fmt "format").
\param va Variable argument list.
\returns Number of items parsed.

This function uses the template string \a template to parse the serialised string \a ser. It will
never read more than \a len bytes from \a ser. It sets \a *used to the number of bytes actually
used during the read operation, and returns the number of items successfully read.

*/
int reads_templatev(const char* ser, size_t len, size_t* used, const char* tpl, va_list va);



/*!@}*/

/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/250_string/000_doxygen.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup string String manipulation, conversion and logging

Routines for dealing with strings (manipulating them, converting them between various formats, and
formatting them for display including logging).

*/



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/250_string/100_strconvert.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup strconvert String to data type conversion routines
\ingroup string

This set of routines provides conversion between strings and data types such as integers,
floating point numbers, and so on.

*/
/*!@{*/



/*! \brief Convert string to boolean.

\param data Scalar (string) value.
\param[out] out If the conversion is successful, the result is written into this value.
\retval 0 on success.
\retval non-0 on error.

Attempts to convert a scalar (string) value into a boolean. If the string value represents an
integer, then 0 is false and non-0 is true. Also recognised are the literals "false" and "true".
Trailing and leading whitespace are skipped. On success, the result is written into \a out and 0
is returned.

*/
int safe_strtobool(const char* data, int* out) __attribute__((nonnull));



/*! \brief Convert string to integer.

\param data Scalar (string) value.
\param[out] out If the conversion is successful, the result is written into this value.
\retval 0 on success.
\retval non-0 on error.

Attempts to convert a scalar (string) value into an integer. Trailing and leading whitespace are
skipped. On success, the result is written into \a out and 0 is returned.

*/
int safe_strtoint(const char* data, int* out) __attribute__((nonnull));



/*! \brief Convert string to double-precision floating point.

\param data Scalar (string) value.
\param[out] out If the conversion is successful, the result is written into this value.
\retval 0 on success.
\retval non-0 on error.

Attempts to convert a scalar (string) value into a double-precision floating point number. Trailing
and leading whitespace are skipped. On success, the result is written into \a out and 0 is returned.

*/
int safe_strtod(const char* data, double* out) __attribute__((nonnull));



/*! \brief Convert string to enumeration value.

\param data Scalar (string) value.
\param[out] out If the conversion is successful, the result is written into this value.
\param values A null-terminated array of choices; \a out is an index into this array.
\retval 0 on success.
\retval non-0 on error.

Attempts to convert a scalar (string) value into an index into an array of choices (\a values).
Trailing and leading whitespace are skipped. The conversion is case sensitive. The array \a values
must be null terminated. On success, the result is written into \a out and 0 is returned.

*/
int safe_strtoenum(const char* data, int* out, const char* const* values) __attribute__((nonnull));



/*! \brief Convert string to octal mode, suitable for filesystem.

\param data Scalar (string) value.
\param[out] out If the conversion is successful, the result is written into this value.
\param accept_mask A bitmask specifying the values that will be accepted.
\retval 0 on success.
\retval non-0 on error (and see \a errno).

Attempts to convert a string (scalar) value into an octal mode suitable for filesystem use. The
string itself does not have to be prefixed with "0" since we force octal conversion. If the 
conversion is successful, and the result does not contain any bits not set in \a accept_mask, the
result is written into \a out and 0 is returned.

*/
int safe_strtooctalmode(const char* data, int* out, int accept_mask) __attribute__((nonnull));



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/250_string/300_strlcpy.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup string_strlcpy Length-limited string operations.
\ingroup string

An implementation of strlcpy() and strlcat().

*/
/*!@{*/



/*! \brief Copy source to destination, limit by length.

\param dest Pointer to destination buffer.
\param src Pointer to source string.
\param dest_size Size of destination buffer in bytes.
\returns Length of source string excluding terminating null.

Like \c strcpy(3) except it will never write more than \a dest_size bytes to \a dest (i.e. the
result is truncated if it would not fit) and as long as \a dest_size is not zero then \a dest
will always be null terminated.

\warning Consider carefully the ramifications of source string truncation when using this function.

*/
size_t safe_strlcpy(char* dest, const char* src, size_t dest_size)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Catenate source onto destination, limit by length.

\param dest Pointer to destination buffer.
\param src Pointer to source string.
\param dest_size Size of destination buffer in bytes.
\returns Length of theoretical resultant string excluding terminating null.

Like \c strcat(3) except it will never write more than \a dest_size bytes to \a dest (i.e. the
result is truncated if it would not fit) and as long as \a dest_size is not zero then \a dest
will always be null terminated.

\warning Consider carefully the ramifications of source string truncation when using this function.

*/
size_t safe_strlcat(char* dest, const char* src, size_t dest_size)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/250_string/500_log.h
 *
 *  Copyright: ©2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup string_log Logging
\ingroup string

This module contains a set of functions for convenient interaction with syslog. The LOG() macro
is similar to \c syslog(), but it prepends line and function information. The LOG_RATE() macro
can be used to perform rate-limited logging.

It is possible to change the destination of log messages using the gslutil_log_destination()
messages.

The suggested uses for the various log levels are:
\li \c LOG_DEBUG - debugging, ignored by default (but see \ref gslutil_log_level()).
\li \c LOG_INFO - verbose informational messages, including user-requested debug dumps.
\li \c LOG_NOTICE - important but expected events (daemon startup/shutdown notifications, start of
    maintenance, etc.).
\li \c LOG_WARNING - a condition that the user might want to do something about, but not an actual
    system call error (e.g. low on disk space, name too long and was truncated, etc.).
\li \c LOG_ERR - system call error that doesn't cause an immediate exit().
\li \c LOG_CRIT - daemon quitting due to error condition.
\li \c LOG_ALERT - the user really needs to do something about this. Preferably we also generate
    a system event.
\li \c LOG_EMERG - we're probably going to have to reboot now. Generally only used in the watchdog
    code. Should generate a system event.

*/
/*!@{*/



/*! \brief Possible log destinations.
 *
 *  This list contains the possible destinations for the logfile.
 */
enum gslutil_log_destination_t {
    gslutil_log_destination_syslog,
    gslutil_log_destination_stderr,
    gslutil_log_destination_none
};



/*! \brief Change log destination.
 *
 *  \param dest Log destination.
 *
 *  This function changes the log destination.
 */
void gslutil_log_destination(enum gslutil_log_destination_t dest);



/*! \brief Get current log destination.
 *
 *  \returns Log destination as set by \ref gslutil_log_destination().
 */
enum gslutil_log_destination_t gslutil_get_log_destination(void);



/*! \brief Interpret log level names.

\param name Name to interpret.
\param[out] level On success, level is written to this argument.
\retval 0 on success.
\retval -1 on error.

This function interprets the string \a name, matching it to "LOG_DEBUG" etc., and on success stores
the associated log level value into \a level. If a match is found, \a level is set and 0 returned.
If no match is found, \a level is unchanged and -1 is returned.

*/
int gslutil_interpret_log_level(const char* name, int* level);



/*! \brief Change log level.

\param level The minimum required level for log messages.

This function allows the minimum required log level to be set. Any messages with a log level less
severe than \a level are ignored. By default, the log level is set to \c LOG_INFO, meaning messages
with a level of \c LOG_DEBUG are ignored. The test for the level is part of the \ref LOG macros, so
the format string and its arguments likely will not be evaluated at all if the minimum log level is
not met.

*/
void gslutil_log_level(int level);
#ifndef DOXYGEN
extern int _gslutil_min_log_level;
#endif



/*! \brief Query log level.

\returns Current log level, as set by \ref gslutil_log_level().

*/
int gslutil_get_log_level(void);



/*! \brief Log to syslog.
 *
 *  \param level The log level (\c LOG_INFO, \c LOG_WARNING, \c LOG_ERR, etc.).
 *  \param fmt Format string.
 *
 *  This macro is a simple wrapper around syslog() that also prints the function and line number.
 *  \a fmt must be a string literal.
 *
 *  \note \a level is evaluated more than once, so it should not be an expression with side
 *  effects. \a fmt must be a string literal.
 */
#define LOG(level, fmt, ...) do { \
    if((level) <= _gslutil_min_log_level) { \
        gslutil_log(level, "%s:%d: " fmt, __FUNCTION__, __LINE__ , ## __VA_ARGS__ ); \
    } \
}while(0)
#ifndef DOXYGEN
void gslutil_log(int level, const char* fmt, ...) __attribute__((format(printf,2,3)));
#endif



/*! \brief Rate-limiting log details.
 *
 *  This structure is used to maintain rate-limited logs. Although it doesn't malfunction in the
 *  case of non-monotonic time, it may reset the rate counter. The \a max and \a period members must
 *  be set to the maximum number of log entries per \a period seconds. \a num and \a start must be
 *  initialised to 0.
 *
 *  Whenever a log entry is written, the difference between \a start and the current time is 
 *  examined. If the difference is greater than \a period, then \a start is set to the current time,
 *  \a num is cleared, and the entry is sent to syslog. If the difference is less than \a period,
 *  then \a num is incremented, and the entry is only sent to syslog if \a num is less than \a max.
 */
struct log_ratelimit_t {
    /*! \brief Maximum number of entries per \a period. */
    int max;

    /*! \brief Period (in seconds) over which to ratelimit. Once elapsed, counters are reset. */
    int period;

    /*! \brief Number of entries so far this period. Stops at \a max. */
    int num;

    /*! \brief Absolute time at which first log entry this period was made. */
    time_t start;
};



/*! \brief Rate-limited logging.
 *
 *  \param rate The rate-limiting structure.
 *  \param level The log level (\c LOG_INFO, \c LOG_WARNING, \c LOG_ERR, etc.).
 *  \param fmt Format string.
 *
 *  Like LOG(), only uses the \a rate structure to perform rate limiting. \a fmt must be a string
 *  literal.
 */
#define LOG_RATE(rate, level, fmt, ...) do { \
    if(level <= _gslutil_min_log_level) { \
        log_rate(rate, level, "%s:%d: " fmt, __FUNCTION__, __LINE__ , ## __VA_ARGS__ ); \
    } \
}while(0)
#ifndef DOXYGEN
void log_rate(struct log_ratelimit_t* rate, int level, const char* fmt, ...) __attribute__((nonnull(1),format(printf,3,4)));
#endif



/*! \brief Debug support: log hexdump.

\param data Data to dump.
\param len Length of \a data, in bytes.
\param desc Format string for description.

This function logs a hexdump of \a data to syslog. It is logged with level \c LOG_DEBUG, so by
default calls to this macro will be ignored (but see \ref gslutil_log_level()).

\note \a len is evaluated more than once, so should not be an expression with side effects.

*/
#define LOG_HEXDUMP(data, len, desc, ...) do { \
    if(LOG_DEBUG <= _gslutil_min_log_level) { \
        gslutil_log(LOG_DEBUG, "%s:%d: hexdump of %u bytes: " desc, \
            __FUNCTION__, __LINE__, (unsigned)(len) , ## __VA_ARGS__ ); \
        gslutil_log_hexdump((data), (len)); \
    } \
}while(0)
#ifndef DOXYGEN
void gslutil_log_hexdump(const char* data, uint32_t len);
#endif



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/250_string/500_strdebug.h
 * 
 *  Copyright: ©2009–2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup strdebug String/memory debugging routines
\ingroup string

This set of routines allows printing of strings in various formats for debugging purposes.

*/
/*!@{*/



/*! \brief Serialise a string in C notation.

\param[out] buf Buffer to write string into.
\param data String to serialise.
\param size If negative, maximum number of bytes to convert; otherwise, exact length of string. 0
    to convert entire string regardless.

Serialises a string in C notation (i.e. replacing ASCII 10 in \a data with <code>\\n</code>, etc.).
Will escape whitespace chars (except space), double quotes and the backslash character. Other
control codes and non-ASCII characters will be represented in hex (i.e. <code>\\xNN</code>). Result
is appended to the buffer \a buf.

The \a size argument, if greater than zero, specifies the number of bytes in \a data to convert. It
does not require \a data to be null terminated, and will in fact encode embedded nulls as 
<code>\\0</code>. If \a size is exactly zero, then \a data must be null terminated and the entire
string will be serialised. If \a size is negative, then the string is assumed to be null terminated,
and conversion will stop just before the first null, but the magnitude \a -size is the maximum
number of bytes that will be converted.

*/
void writem_str_cnotation(struct membuf_t* buf, const char* data, int size) __attribute__((nonnull));



/*! \brief Perform a hex dump.

\param[out] buf Buffer to write dex dump into.
\param data Pointer to bytes.
\param size Number of bytes to dump.

Performs a hex dump, storing the textual result in \a buf. This is the canonical 16-bytes-per-line
dump. The result is appended to \a buf.

*/
void writem_str_hexdump(struct membuf_t* buf, const char* data, int size) __attribute__((nonnull));



/*! \brief Dump contents of buffer to FILE*.

\param buf Buffer to dump.
\param fp FILE* to dump to.

Writes the contents of \a buf to \a fp. The contents are not interpreted or encoded in any way;
they are simply written with \c fwrite(3). The buffer \a buf is then emptied.

\sa membuf_drain_output()

*/
void membuf_dump_to_file(struct membuf_t* buf, FILE* fp) __attribute__((nonnull));



/*! \brief Write string in hex notation.

\param buf Output buffer.
\param sz Size of output buffer.
\param str String to dump.
\param len Number of bytes to dump.

Writes the string \a str (of \a len bytes) bytewise in hex notation into \a buf, which will be
left null-terminated. The output may be truncated if \a sz is not large enough to contain the
complete result (which will require \a len * 3 bytes).

*/
void writes_str_hexnotation(char* buf, int sz, const char* str, int len) __attribute__((nonnull));



/*! \brief Open file for logging in /var/crash .

\param name Name of application (used for filename).
\param report If non-zero, reports the crash dump via \ref LOG().
\returns Opened file ready for writing.
\retval 0 on error (and see \a errno).

This function securely opens a file in /var/crash for writing a crash dump
into.  It must be passed \a name which is used as a prefix for the file. It
uses a random 2-hex-digit suffix which ensures this directory does not grow
indefinitely as older crash dumps will be overwritten.

*/
FILE* gsl_var_crash_open(const char* name, int report) __attribute__((nonnull));



/*! \brief Write simple crash log with hexdump in /var/crash.

\param name Name of application (used for filename).
\param desc Description of fault.
\param data Pointer to arbitrary block of data to hexdump.
\param len Length of data.
\param report If non-zero, reports the crash dump via \ref LOG().

This function can be used to easily write a bug or crash report with a short
description and a hex dump of a block of data.

Wrapper around \ref gsl_var_crash_open(), \ref writem_str_hexdump() and
\ref membuf_dump_to_file().

*/
void gsl_var_crash_dump(const char* name,
    const char* desc,
    const char* data,
    size_t len,
    int report)
    __attribute__((nonnull));



/*!@}*/

/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/300_io/000_doxygen.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup io I/O routines
 *
 *  A set of convenience functions for dealing with various aspects of I/O.
 */



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/300_io/100_utils.h
 * 
 *  Copyright: ©2009–2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup io_utils Simple I/O utilities
 *  \ingroup io
 *
 *  Routines for helping with generic posix I/O.
 */
/*!@{*/



/*! \brief Set non-blocking mode on a file descriptor. Returns 0 on success, logs failures to syslog. */
int set_non_blocking(int fd);
/*! \brief Clear non-blocking mode on a file descriptor. Returns 0 on success, logs failures to syslog. */
int clear_non_blocking(int fd);



/*! \brief Set close-on-exec() mode on a file descriptor.

\param fd File descriptor.
\retval 0 on success.
\retval -1 on error (and see \a errno).

*/
int gsl_set_cloexec(int fd);



/*! \brief Clear close-on-exec() mode on a file descriptor.

\param fd File descriptor.
\retval 0 on success.
\retval -1 on error (and see \a errno).

*/
int gsl_clear_cloexec(int fd);



/*! \brief Set non-blocking mode on a file descriptor.

\param fd File descriptor.
\retval 0 on success.
\retval -1 on error (and see \a errno).

*/
int gsl_set_nonblocking(int fd);



/*! \brief Clear non-blocking mode on a file descriptor.

\param fd File descriptor.
\retval 0 on success.
\retval -1 on error (and see \a errno).

*/
int gsl_clear_nonblocking(int fd);



/*! \brief Read input from a file into a memory buffer.
 *
 *  \param m Memory buffer to read data into.
 *  \param fd File descriptor to read from.
 *  \returns Number of bytes available in buffer.
 *  \retval -1 on EOF or error (and see \a errno).
 *
 *  This function will read input from a file descriptor \a fd into the memory buffer \a m. It will
 *  attempt to drain as much input as possible from \a fd (which should be non-blocking). The return
 *  value is the number of bytes available in the buffer after the read (which will include any data
 *  already in the buffer).
 *
 *  If there is no data waiting to be collected and we get EAGAIN immediately, then we simply return
 *  the number of bytes remaining in the buffer (which may be 0 if the buffer was empty). We do not
 *  return -1 in this case.
 *
 *  If there is no data to read, and an EOF condition occurs (socket closed etc.) then -1 is 
 *  returned and \a errno is set to \c EPIPE. If an error occurs (but not \c EAGAIN) then -1 is
 *  returned and \a errno set as per read(). Errors (except \c EPIPE and \c EAGAIN) are logged to
 *  syslog.
 */
int membuf_drain_input(struct membuf_t* m, int fd) __attribute__((nonnull));



/*! \brief Drain output from a memory buffer to a file.
 *
 *  \param m Memory buffer to write data from.
 *  \param fd File descriptor to write data to.
 *  \returns Number of bytes remaining in the buffer.
 *  \retval -1 on error (and see \a errno).
 *
 *  This function will write data from a memory buffer \a m to the file descriptor \a fd. It will
 *  attempt to drain as much data as possible to \a fd (which may be non-blocking). The return value
 *  is the number of bytes remaining in the buffer after the write (zero if all output is written).
 *
 *  If an error occurs (but not \c EAGAIN), then it is logged to syslog and -1 is returned.
 */
int membuf_drain_output(struct membuf_t* m, int fd) __attribute__((nonnull));



/*! \brief Read fixed amount of data into memory.
 *
 *  \param fd File descriptor to read from.
 *  \param buf Buffer to read into.
 *  \param amt Amount to read.
 *  \returns Number of bytes read (should be \a amt normally, only shorter if EOF is reached).
 *  \retval -1 on error (and see \a errno).
 *
 *  Like read(), only we always read the amount that was requested. This won't work for non-blocking
 *  sockets, since we will return with EAGAIN and no way of knowing how much was read. Restarts if
 *  interrupted.
 */
ssize_t safe_read_fixed(int fd, char* buf, size_t amt) __attribute__((nonnull));



/*! \brief Read fixed amount of data into memory from position in file.
 *
 *  \param fd File descriptor to read from.
 *  \param buf Buffer to read into.
 *  \param amt Amount to read.
 *  \param offset Offset to seek to before reading.
 *  \returns Number of bytes read (should be \a amt normally, only shorter if EOF is reached).
 *  \retval -1 on error (and see \a errno).
 *
 *  Like pread(), only we always read the amount that was requested. This won't work for non-blocking
 *  sockets, since we will return with EAGAIN and no way of knowing how much was read. Restarts if
 *  interrupted. Doesn't affect file position pointer.
 */
ssize_t safe_pread_fixed(int fd, char* buf, size_t amt, int64_t offset) __attribute__((nonnull));



/*! \brief Write fixed amount of data from memory.
 *
 *  \param fd File descriptor to write to.
 *  \param buf Buffer to write from.
 *  \param amt Amount to write.
 *  \returns Number of bytes written (i.e. \a amt).
 *  \retval -1 on error (and see \a errno).
 *
 *  Like write(), only we always write the full amount that was requested. This won't work for
 *  non-blocking sockets, since we will return with EAGAIN and no way of knowing how much was
 *  written. Restarts if interrupted.
 */
ssize_t safe_write_fixed(int fd, const char* buf, size_t amt) __attribute__((nonnull));



/*! \brief Write fixed amount of data from memory to position in file.
 *
 *  \param fd File descriptor to write to.
 *  \param buf Buffer to write from.
 *  \param amt Amount to write.
 *  \param offset Offset to seek to before reading.
 *  \returns Number of bytes written (i.e. \a amt).
 *  \retval -1 on error (and see \a errno).
 *
 *  Like pwrite(), only we always write the full amount that was requested. This won't work for
 *  non-blocking sockets, since we will return with EAGAIN and no way of knowing how much was
 *  written. Restarts if interrupted. Doesn't affect file position pointer.
 */
ssize_t safe_pwrite_fixed(int fd, const char* buf, size_t amt, int64_t offset) __attribute__((nonnull));



/*! \brief Get poll(2) flags for file descriptor.

\param fd File descriptor to poll.
\returns Poll flags for the file descriptor \a fd.
\retval -1 on error (and see \a errno).

This function calls \c poll(2) with an immediate timeout on the file descriptor \a fd, returning the
flags that are set (or -1 on error). The result can be passed directly to \ref display_poll_flags().

*/
int poll_flags(int fd);



/*! \brief Display poll(2) flags in human-readable form.

\param flags Flags to display.
\returns Pointer to static null-terminated string buffer holding flag names.

This function prints a human-readable representation of the poll flag bitfield \a flags into an
internal buffer, returning a pointer to that buffer. If \a flags is -1, prints "Error: %m" into the
buffer instead.

*/
const char* display_poll_flags(int flags);



/*!@}*/

/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/300_io/200_gpiolib.h
 * 
 *  Copyright: ©2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup gpiolib GPIO via sysfs (gpiolib)
\ingroup io

Routines for manipulating GPIO lines via gpiolib (the kernel's sysfs-based
interface). GPIO lines are referred to by number.

A GPIO line must first be exported to user pace before it can be manipulated;
this can be done with \ref gsl_gpiolib_export(). From there it may be set to
input or output with \ref gsl_gpiolib_direction(). Its level may be read with
\ref gsl_gpiolib_read() and written with \ref gsl_gpiolib_write().

*/
/*!@{*/



/*! \brief GPIO direction.

Used to select whether a GPIO is an input or an output.

*/
enum gsl_gpiolib_dir {
    /*! \brief Input line (high-impedance). */
    gsl_gpiolib_dir_in,

    /*! \brief Output line (low-impedance). */
    gsl_gpiolib_dir_out,
};



/*! \brief Export GPIO to userspace.

\param gpio The GPIO line number.
\retval 0 on success.
\retval -1 on error (and see \a errno).

Requests that the kernel exports a GPIO line to userspace. If the line is
already exported, this function returns success. Otherwise, it attempts to
export the line, returning success or failure as per the kernel's report.

*/
int gsl_gpiolib_export(unsigned int gpio);



/*! \brief Set GPIO direction.

\param gpio The GPIO line number.
\param dir Direction to set.
\param value Initial value for output; ignored for input.
\retval 0 on success.
\retval -1 on error (and see \a errno).

Sets the direction of an exported GPIO line. If setting the line to be an
output, pass \a value as 0 for low or non-0 for high. If setting the line to
be an input, \a value is ignored.

*/
int gsl_gpiolib_direction(unsigned int gpio, enum gsl_gpiolib_dir dir,
    int value);



/*! \brief Read GPIO value.

\param gpio The GPIO line number.
\param[out] value The value of the GPIO line.
\retval 0 on success.
\retval -1 on error (and see \a errno).

Reads the current value of an exported GPIO line. \a value will be set to 0 if
low or 1 if high. On error, \a value is left unchanged.

*/
int gsl_gpiolib_read(unsigned int gpio, int* value)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Set GPIO value.

\param gpio The GPIO line number.
\param[out] value The value of the GPIO line.
\retval 0 on success.
\retval -1 on error (and see \a errno).

Sets the current value of an exported GPIO line, if it is an output. \a value
should be 0 to set the line low or 1 to set it high.

*/
int gsl_gpiolib_write(unsigned int gpio, int value);



/*!@}*/

/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/300_io/300_serial.h
 * 
 *  Copyright: ©2009–2012, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup serial_port Serial port routines
\ingroup io

Routines for opening and initialising serial ports.

\warning The original version of \a serial_port_open had a problem with its
    \a flags parameter. This was defined as taking a bitmask including
    \ref GSL_NONBLOCK or \c CLOCAL (undocumented, but widely used). However,
    these flags actually have the same bitmask! This has been resolved by
    defining a new \ref GSL_SERIAL_PORT_CONTROL_LINES bit, with a different
    value and a different sense compared to \c CLOCAL, but this of course
    requires an audit of all users to ensure the serial port is in the correct
    mode.

*/
/*!@{*/



/*! \brief Open serial port for I/O.

\param path Path to device node.
\param flags Flags to open with.
\param baud Baud rate.
\returns File descriptor.
\retval -1 on error

A convenience function that opens the serial port \a path O_RDWR | O_NOCTTY. \a
flags may contain \ref GSL_NONBLOCK to open for non-blocking I/O, and
\ref GSL_SERIAL_PORT_CONTROL_LINES to inhibit the \c CLOCAL bit (i.e. to honour
the serial port control lines). If setting non-blocking mode or setting the
requested \a baud rate fails, the file descriptor will be closed and -1
returned.

This automatically sets the port for raw serial mode, 8 data bits, no parity, 1
stop bit etc.

\sa set_non_blocking(), serial_port_setbaud()

*/
int serial_port_open(const char* path, int flags, int baud)
    __attribute__((nonnull));



/*! \brief Set baud rate of serial port.

\param fd Serial port file descriptor.
\param baud New baud rate.
\retval 0 on success.
\retval -1 on error.

Sets the baud rate of a serial port \a fd to the value in \a baud. This value is the actual
numerical baud rate, \em not a constant (as per \c cfsetspeed(3)). Passing a \c B... constant will
cause incorrect results.

If the baud rate is not a standard rate, this function will try to set the non-standard rate using
the divisor method. If the requested baud rate cannot be matched to within 5%, an error will be
returned. If the requested baud rate is not an integral divisor of the crystal frequency, two stop
bits will be turned on.

This will only change the baud rate and stop bits, not any of the other port settings.

*/
int serial_port_setbaud(int fd, int baud);



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/300_io/500_sleep.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup sleep Sleeping routines
\ingroup io

These routines are similar to \c sleep(3) but more convenient.

*/
/*!@{*/



/*! \brief Sleep for fixed period of time.

\param s Number of seconds to sleep for.
\param ns Number of nanoseconds to sleep for.
\retval 0 on success.
\retval -1 on error (and see \a errno).

This function performs a simple, fixed sleep. It is implemented with \c nanosleep(2) internally. It
will restart the sleep with the remainder of time in the case of signals. It is not very accurate.

It should only ever fail with \c EINVAL, and then only if \a s or \a ns is not in the valid range.

*/
int safe_sleep_fixed(int s, int ns);



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/300_io/900_generic_open.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup io_generic_open Generic open/connect routine
\ingroup io

A routine which allows any streaming device (Unix streams socket, serial port, TCP connection, etc.)
to be opened.

A TCP connection may be initiated with a device name of
<code>TCP:<em>host</em>,<em>service</em></code> where <em>host</em> is a host name to resolve or an
IP address, and <em>service</em> is a port number or service name (see \ref ipaddr_resolve()).

A Unix streams connection may be initiated with a device name of
<code>Unix:<em>/path/to/socket</em></code>.

A serial port may be opened with a device name of
<code>tty:<em>/path/to/device</em>,<em>baud</em></code>.

*/
/*!@{*/



/*! \brief Open a streaming device based on name.

\param flags Flags for open/connect.
\param name Device name.
\returns File descriptor.
\retval -1 on error (and see \a errno).

Opens a generic device (see \ref io_generic_open description for explanation of the format of
\a name). \a flags may contain \ref GSL_NONBLOCK to make the stream nonblocking.

On success, returns a file descriptor for the newly-opened device/socket. On failure, sets \a errno
and returns -1. If \a name does not begin with a recognised prefix, or cannot be parsed, \a errno is
set to \c EINVAL. If name resolution of a TCP connection fails, \a errno is set to \c EADDRNOTAVAIL.

*/
int generic_open(int flags, const char* name);



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/310_net/100_doxygen.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup net Network routines

A set of routines for networking.

*/



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/310_net/100_ip.h
 * 
 *  Copyright: ©2009–2011, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup net_ip IP networking
\ingroup net

ip_connect() instantiates and connects a client socket. ip_server() instantiates and binds a server
socket. ip_accept() accepts incoming connections on a server socket.

*/
/*!@{*/



/*! \brief IP-oriented connections: sub-protocol.

This contains the list of protocols that we know about over the IPv4 and IPv6 layers.

*/
enum ip_subproto_t {
    /*! \brief Any type acceptable */
    ip_subproto_any,
    /*! \brief TCP (transmission control protocol) */
    ip_subproto_tcp,
    /*! \brief UDP (user datagram protocol) */
    ip_subproto_udp
};



/*! \brief Connect to a remote IP server.

\param flags Flags for connection.
\param addr Address of remote host (perhaps from ip_resolve()).
\retval -1 on error (and see \a errno).
\returns file descriptor of new client on success.

This function attempts to create an IP client and connect it to a remote server. It will return -1
on error (\a errno will be set according to socket() or connect()).

\a flags may contain \ref GSL_NONBLOCK to specify non-blocking mode.

If \a addr contains more than one entry, each entry will be tried in turn to connect to the host
until one succeeds. In this case, -1 will be returned if none of the addresses succeed.

*/
int ip_connect(int flags, const struct addrinfo* addr) __attribute__((warn_unused_result,nonnull));



/*! \brief Create an IP server.

\param flags Flags for server socket.
\param addr Address to bind to.
\retval -1 on error (and see \a errno).
\returns file descriptor of new server on success.

This function attempts to create an IP server and bind it to the specified local address. It will
return -1 on error (\a errno will be set according to socket(), bind() or (if relevant) listen()).
The address will typically come from ip_resolve(), which can be called with a \a host argument of 0
to return an address that allows the server to listen on all network interfaces.

\a flags may contain \ref GSL_NONBLOCK to specify non-blocking mode (this will only affect the
server; newly-accepted clients may be either blocking or non-blocking). It may also contain
\ref GSL_IPV6ONLY to ensure a socket is not bound to an IPv4-in-IPv6 mapped address (ignored unless
\a addr is an IPv6 address).

\sa ip_accept().

*/
int ip_server(int flags, const struct addrinfo* addr) __attribute__((warn_unused_result,nonnull));



/*! \brief Accept a client from a connection-oriented IP server.

\param flags Flags for client socket.
\param fd Server's file descriptor.
\param[out] addr The client's remote address. May be 0.
\retval -1 on error (and see \a errno).
\returns file descriptor of new client on success.

This function attempts to accept() a connection from an IP server. It will retry on any error except
EAGAIN, which indicates that there are actually no more clients to accept. It should be called in a
loop until it does this. If a persistent error occurs, -1 will be returned and the last error 
recorded in \a errno.

The client's remote address may be retrieved by passing a pointer to a <code>struct addrinfo</code>
in \a addr (see sockaddr_to_addrinfo()).

\a flags may contain \ref GSL_NONBLOCK to specify that a newly-accepted client will be in non-blocking 
mode.

*/
int ip_accept(int flags, int fd, struct addrinfo* addr) __attribute__((warn_unused_result));



/*! \brief Create a set of IP servers (advanced interface).

\param flags Flags for server socket.
\param addrs Address(es) to bind to, in linked list form as per
    \ref ipaddr_resolve or \c getaddrinfo(3).
\returns Pointer to newly-allocated -1-terminated array of file descriptors.
\retval 0 on error (and see \a errno).

This function creates a set of IP server sockets based on all the addresses
given in the list \a addrs.

Any IPv6 sockets created are bound with \c IPV6_V6ONLY (see \c ipv6(7)) in order
to avoid problems with "Address already in use" when binding to e.g. the
wildcard address.

If resolving or binding of any of the addresses fails, then any opened sockets
are closed and 0 is returned. \a errno will be set appropriately.

The returned array should be freed with \c free(3). The array has a terminating
value of -1, the invalid file descriptor.

*/
int* ip_servers2(int flags, const struct addrinfo* addrs)
#ifndef DOXYGEN
    __attribute__((malloc,nonnull))
#endif
;



/*! \brief Create a set of IP servers (basic interface).

\param flags Flags for server socket.
\param subproto IP subprotocol (TCP, UDP, any).
\param addrs Address(es) to bind to.
\returns Pointer to newly-allocated -1-terminated array of file descriptors.
\retval 0 on error (and see \a errno).

This function creates a set of IP server sockets based on all the addresses
given in the string \a addrs.

If \a subproto is specified as TCP or UDP, then only TCP or UDP sockets
(respectively) will be created. However, if \a subproto is specified as \ref
ip_subproto_any, then both TCP and UDP sockets may be created, depending on how
the service name is specified. A service name specified as a port number
resolves successfully as both TCP and UDP. String names depend upon
/etc/services to determine whether TCP, UDP or both are valid for each service.

The string \a addrs is a set of one or more whitespace-separated addresses.
Each address may consist of either <code>service</code> or
<code>bind,service</code> where <code>bind</code> is taken to be "any address"
if it is omitted as in the first form. Each address listed might return
multiple socket addresses (for instance, specifying just "domain" is likely to
return four sockets, TCP/IPv4, UDP/IPv4, TCP/IPv6 and UDP/IPv6).

If the string in \a addrs cannot be interpreted (usually because one of the
component names is too long), the function will fail (returning 0) and \a errno
will be set to \c EDOM. If resolving fails, errno will be set to \c
EADDRNOTAVAIL, and the error may be further found from \ref
ipaddr_resolve_errstr.

\sa \ref ip_servers2() for more details.

*/
int* ip_servers(int flags, enum ip_subproto_t subproto, const char* addrs)
#ifndef DOXYGEN
    __attribute__((malloc,nonnull))
#endif
;



/*!@}*/

/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/310_net/200_resolv.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup net_resolv Name and address resolution
\ingroup net

ipaddr_resolve() and ipaddr_copy() provide a convenient (but blocking) name resolution service; 
see <code>getaddrinfo(3)</code>.

*/
/*!@{*/



/*! \brief Resolve an IP address and port number for a given host/service.

\param host Hostname or IP address to resolve (may be 0 for 'any').
\param service Service name (e.g. 'smtp') or port number to resolve. May be 0.
\param ip_version IP version number (0 for unspecified, otherwise 4 or 6).
\param subproto IP sub-protocol.
\returns Address info structure (as per \c getaddrinfo(3)).
\retval 0 on error (and see \ref ipaddr_resolve_errstr).

This function will resolve a hostname/IP address string and a service string. It is a thin
wrapper around \c getaddrinfo(3). Either \a host or \a service may be 0 (but not both). The
returned structure is a linked list that must be freed with \c freeaddrinfo(3).

Leaving \a host as 0 will return the 'any' address. Servers binding to this address will accept
connections from any network interface, including loopback.

You can force the address family of the result to \c AF_INET by setting \a ip_version to 4, or to
\c AF_INET6 by setting \a ip_version to 6. Leaving \a ip_version as 0 will not force the address
family of the result.

On failure, a string describing the error may be found in `ipaddr_resolve_errstr'. This string will
only persist until the next call to ipaddr_resolve() (see gai_strerror()).

*/
struct addrinfo* ipaddr_resolve(const char* host, const char* service, int ip_version,
    enum ip_subproto_t subproto) __attribute__((malloc,warn_unused_result));



/*! \brief Resolve an IP address and port number for a given host and service.

\param address Comma-separated host and service addresses.
\param ip_version IP version number (0 for unspecified, otherwise 4 or 6).
\param subproto IP sub-protocol.
\returns Address info structure (as per \c getaddrinfo(3)).
\retval 0 on error (and see \ref ipaddr_resolve_errstr).

This function is a convenience wrapper for \ref ipaddr_resolve() . Rather than specifying the host
and service names separately, they may be specified together, separated by a comma.

*/
struct addrinfo* ipaddr_resolve2(const char* address, int ip_version,
    enum ip_subproto_t subproto) __attribute__((malloc,warn_unused_result));



/*! \brief String describing last error from ip_resolve(). */
extern const char* ipaddr_resolve_errstr;



/*! \brief Deep copy of an address info structure.

\param addr Address structure to copy.
\returns A newly-allocated address structure.

This function will perform a deep copy of an address info structure, allowing the original to
be freed. The entire linked list will be copied. Uses safe_malloc().

*/
struct addrinfo* ipaddr_copy(const struct addrinfo* addr) 
    __attribute__((malloc,warn_unused_result,nonnull));



/*! \brief Allocate an empty IP address structure.

\returns Pointer to new structure.

Allocates and returns a <code>struct addrinfo</code> suitable for ip_accept() or membuf_recvfrom().
The structure will initially be empty, though with enough space allocated to hold the client
address. The structure can be freed with freeaddrinfo().

*/
struct addrinfo* ipaddr_alloc_empty(void) __attribute__((malloc,warn_unused_result));



/*! \brief Convert a sockaddr structure to an addrinfo structure, with readable name.

\param from The <code>sockaddr</code> structure.
\param from_len Length of \a from, in bytes.
\param[out] addr The address info structure to write to, allocated with ipaddr_alloc_empty().

This function converts the address in \a from (a structure of \a from_len bytes) into an addrinfo
structure, \a addr. \a addr must already have been allocated with e.g. ipaddr_alloc_empty(). This
function will set \a addr->ai_family, \a addr->ai_addrlen and \a addr->ai_addr. It may also set
\a addr->ai_canonname.

If the address family is IPv4 or IPv6, \a addr->ai_canonname will be set to the string 
representation of the IP address, followed by a space, the (decimal) port number, and null
terminated. If the address is a Unix socket, \a addr->ai_canonname will be the source path. In any
other case, the string will be empty.

\sa addrinfo_set_canonname()

*/
void sockaddr_to_addrinfo(const struct sockaddr* __restrict__ from, socklen_t from_len, 
    struct addrinfo* __restrict__ addr) __attribute__((nonnull));



/*! \brief Sets canonname to consistent value.

\param ai The address info structure to operate on.

Sets the member variable \a ai->ai_canonname to be the string representation of the address used in
sockaddr_to_addrinfo(). Useful if you want to have a consistent string with which to label client
connections.

Requires that \a ai->ai_canonname have sufficient length if it is a valid pointer (i.e. from
ipaddr_alloc_empty()), or that it be null (in which case the required memory is allocated).

\sa sockaddr_to_addrinfo()

*/
void addrinfo_set_canonname(struct addrinfo* ai) __attribute__((nonnull));



/*! \brief Compare two address structures.

\param a1 First address structure.
\param a2 Second address structure.
\param flags Flags for comparison.
\retval 0 if identical
\retval non-0 if different

This function performs a comparison of two address structures \a a1 and \a a2.
The comparison may be controlled by the \a flags argument. This function takes
special care to deal with IPv4 mapped IPv6 addresses (i.e. it will consider
\verbatim ::ffff:192.168.0.1 \endverbatim and 192.168.0.1 to be equivalent).

This function can be used to compare Unix or other sorts of addresses, for
which it does just a simple memory comparison, but its primary purpose is to
compare IP addresses, for which its behaviour may be somewhat customised.

\a flags is a bitfield which may contain the following:
\li \ref IPADDR_COMPARE_PROTO consider the \a ai_socktype and \a ai_protocol
        fields of the two address structures

\li \ref IPADDR_COMPARE_PORTS consider TCP/UDP port numbers in addition to IP
        addresses

*/
int ipaddr_compare(const struct addrinfo* a1,
    const struct addrinfo* a2,
    int flags)
    __attribute__((nonnull));



/*! \brief Address structure comparison: consider socket type and protocol.

When comparing two address structures (<code>struct addrinfo</code>, see \c getaddrinfo(3)), also
consider the \a ai_socktype and \a ai_protocol fields. Depending on what filled out the address
info structure, these fields may be unset, and so are not considered by default.

*/
#define IPADDR_COMPARE_PROTO (0x00000001)



/*! \brief Address structure comparison: consider protocol ports.

When comparing two IP addresses, also consider the protocol port field (\a sin_port or
\a sin6_port). This has no effect if the address info structures do not contain IP addresses.

*/
#define IPADDR_COMPARE_PORTS (0x00000002)



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/310_net/210_ipfilter.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup net_ipfilter IP address filtering
\ingroup net

This set of routines provides a way to match an IP address against a list of accepted/rejected
addresses. Its purpose is similar to that of TCP Wrappers, although the implementation is more
limited. However, it is much more convenient for integration allowing fine-grained checks.

The filter's structure is a simple list of entries. Each entry consists of an IP address (IPv4 or
IPv6), the number of significant bits to test for, and the accept/reject policy for addresses
matching the entry. Entries are tested in the order they were added to the filter. If an address
matches an entry, its policy is used, and testing stops. If an address does not match any entry
in the filter, the filter's default policy is used.

An IP filter is constructed in a <code>struct ipfilter_t</code> object, which is opaque to the
library user.

*/
/*!@{*/



/* structure used for filtering */
struct ipfilter_t;



/*! \brief Default policy for IP filter.

If an IP address does not match any entry in the filter, the default policy is applied. This can
either accept or reject the unmatched address.

*/
enum ipfilter_policy_t {
    /*! \brief Accept unmatched addresses. */
    ipfilter_policy_accept,
    /*! \brief Reject unmatched addresses. */
    ipfilter_policy_reject,
};



/*! \brief Instantiate a new IP filter object.

\returns Pointer to newly-allocated object.

This creates an empty filter, which will by default reject all incoming connections. Its policy may
be changed with \ref ipfilter_policy_set().

*/
struct ipfilter_t* ipfilter_new(void) __attribute__((malloc,warn_unused_result));



/*! \brief Free an IP filter object.

\param ipf IP filter object. May be 0.

Frees all memory associated with an IP filter object.

*/
void ipfilter_free(struct ipfilter_t* ipf);



/*! \brief Filter address.

\param ipf IP filter object.
\param sa Address to filter.
\retval ipfilter_policy_accept if address is accepted.
\retval ipfilter_policy_reject if address is rejected.

Filters the address \a sa (either <code>struct sockaddr_in</code> or <code>struct 
sockaddr_in6</code> for IPv4/IPv6 respectively) through the IP filter object \a ipf.

If \a sa is not an IP address but some other address family, \a ipfilter_policy_reject is
returned.

*/
#ifdef DOXYGEN
enum ipfilter_policy_t ipfilter(const struct ipfilter_t* ipf, const struct sockaddr* sa);
#else
/* avoid having to cast to (struct sockaddr*) */
enum ipfilter_policy_t ipfilter(const struct ipfilter_t* ipf, const void* sa)
    __attribute__((nonnull));
#endif



/*! \brief Retrieve an IP filter's default accept/reject policy.

\param ipf IP filter object.
\returns Policy of filter.

Returns the default accept/reject policy of the IP filter \a ipf.

*/
enum ipfilter_policy_t ipfilter_policy_get(const struct ipfilter_t* ipf) __attribute__((nonnull));



/*! \brief Set an IP filter's default accept/reject policy.

\param ipf IP filter object.
\param policy New accept/reject policy.

Sets the default accept/reject policy of the IP filter \a ipf.

*/
void ipfilter_policy_set(struct ipfilter_t* ipf, enum ipfilter_policy_t policy)
    __attribute__((nonnull));



/*! \brief Add an entry to an IP filter.

\param ipf IP filter object.
\param sa Entry address.
\param bits Number of significant bits to test for.
\param policy Policy for matched addresses.
\retval 0 on success.
\retval -1 on error (and see \a errno).

This function adds an entry to the IP filter object \a ipf. The entry consists of an IP address
(either <code>struct sockaddr_in</code> or <code>struct sockaddr_in6</code> for IPv4/IPv6 
respectively), the number of significant bits in \a sa that will be tested, and the policy for
matched addresses.

If \a sa does not point to an IPv4 or IPv6 address, \a errno is set to \c EAFNOSUPPORT.

To match all addresses, it is legal to specify \a bits as 0.
To match an IPv4 address exactly, \a bits should be 32. If it is outside the range 0&ndash;32 for an
IPv4 address, the function will fail and set \a errno to \c EDOM. Similarly, IPv6 addresses have
128 bits, and \a bits must be between 0&ndash;128.

*/
#ifdef DOXYGEN
int ipfilter_add(struct ipfilter_t* ipf, const struct sockaddr* sa, int bits,
    enum ipfilter_policy_t policy);
#else
/* avoid having to cast to (struct sockaddr*) */
int ipfilter_add(struct ipfilter_t* ipf, const void* sa, int bits, enum ipfilter_policy_t policy)
    __attribute__((nonnull));
#endif



/*! \brief Add entry based on string.

\param ipf IP filter object.
\param str String to read entry from.
\param[out] endp Set to point to first character we stopped at.
\retval 0 on success.
\retval -1 on error (and see \a errno).

This function parses a string-based representation of an accept/reject entry, and adds the entry to
the IP filter object \a ipf. It returns 0 on success and -1 on error. \a errno will be set as per
\ref ipfilter_add() or to \a EINVAL if the input string is not valid.

This function will skip leading or trailing whitespace. If the entry string is parsed successfully,
\a endp will be set to the first non-whitespace character following the entry. If the entry string 
cannot be parsed, or \ref ipfilter_add() fails, \a endp will be set to the first
character that caused us to fail. \a endp may be 0 if this information is not required.

An entry string is formatted like this (case sensitive, no additional whitespace):

<pre>POLICY(ADDRESS[/NET])</pre>

\c POLICY may either be \c accept or \c reject . \c ADDRESS may be an IPv4 or IPv6 address in
numerical notation, or a hostname to be resolved. If \c NET and its leading slash is omitted, then
the exact address (/32 for IPv4 or /128 for IPv6) will be used. If \c NET is specified, it is the
number of significant bits of the address to use.

Some examples:

\li <code>accept(localhost)</code> &mdash; foo.
\li <code>accept(192.168.0.0/24)</code> &mdash; accept all IPv4 addresses on the 192.168.0.* subnet.
\li <code>accept(2001:41c8:1:5763::2/64)</code> &mdash; accept all IPv6 addresses with the
    2001:41c8:1:5763 prefix.
\li <code>reject(0.0.0.0/0)</code> &mdash; reject all IPv4 addresses.
\li <code>reject(::/0)</code> &mdash; reject all IPv6 addresses.

*/
int ipfilter_add_str(struct ipfilter_t* ipf, const char* str, const char** endp)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)))
#endif
;



/*! \brief Add entries based on comma-separated string.

\param ipf IP filter object.
\param str String to read entries from.
\param errp Pointer to location at which error occurred. May be 0.
\retval 0 on success.
\retval -1 on error (and see \a errno).

This function parses a string with multiple IP filter entries separated by commas. Each entry is in
the format specified by \ref ipfilter_add_str(), and that function is used to parse each component
of the string.

If all entries could be parsed, ipfilter_add_str() returns 0 and \a errp will point at the null
terminating \a str. If any error occurs, -1 will be returned and \a errp will be set to point at the
start of the first entry that failed. \a errp may be 0 if this information is not required.

Note that \a ipf may be changed even if an error occurs, as some of the filter string may have been
parsed successfully.

*/
int ipfilter_add_commastr(struct ipfilter_t* ipf, const char* str, const char** errp)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)))
#endif
;



/*! \brief Dump IP filter to string for debugging.

\param ipf IP filter object.
\param buf Pointer to buffer to dump to.
\param sz Size of buffer, in bytes.

Dumps the contents of the IP filter object \a ipf to the string \a buf. \a buf will never be overrun
and will always be null-terminated. The filter will be in the format described in
\ref ipfilter_add_commastr(), although any names will have been resolved to addresses.

*/
void ipfilter_debug_dump(const struct ipfilter_t* ipf, char* buf, int sz) __attribute__((nonnull));



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/310_net/300_unix.h
 *
 *  Copyright: ©2009–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup net_unix Unix networking routines
\ingroup net

unix_connect() instantiates and connects a client socket. unix_server() instantiates and binds a
server socket. unix_accept() accepts an incoming connection from a server socket.

*/
/*!@{*/



/* enum unix_subproto_t is declared in 001_enum.h */

/*! \brief Connect to a remote Unix domain server.

\param flags Flags for connection.
\param path Path to the server socket. If the first character of the string is an ASCII null, then
    the server socket refers to the abstract namespace.
\param subproto Unix domain socket type.
\retval -1 on error (and see \a errno).
\retval file descriptor of new client on success.

Creates a Unix domain socket and connects it to a remote server using the socket type specified in
\a subproto.

\a flags may contain \ref GSL_NONBLOCK to specify non-blocking mode.

*/
int unix_connect(int flags, const char* path, enum unix_subproto_t subproto) __attribute__((warn_unused_result,nonnull));



/*! \brief Create a Unix domain server socket.

\param flags Flags for server socket.
\param path Path to the server socket. If the first character of the string is an ASCII null, then
    the server socket refers to the abstract namespace.
\param owner Owner name for chown (may be 0).
\param group Group name for chgrp (may be 0).
\param mode Mode number for chmod (-1 means 0666 - umask).
\param subproto Unix domain socket type.
\retval -1 on error (and see \a errno).
\retval file descriptor of new server on success.

Creates a Unix domain socket and binds it to a socket file using the details in
\a path and, optionally, \a owner, \a group and \a mode (see \ref
gsl_set_perms()). Note the process's umask is temporarily altered during
creation of the socket.

\a flags may contain \ref GSL_NONBLOCK to specify non-blocking mode (this will
only affect the server; newly-accepted clients may be either blocking or
non-blocking).

\sa unix_accept().

*/
int unix_server(int flags,
    const char* path,
    const char* owner,
    const char* group,
    int mode,
    enum unix_subproto_t subproto)
#ifndef DOXYGEN
__attribute__((warn_unused_result,nonnull(2)))
#endif
;



/*! \brief Accept a client from a connection-oriented Unix domain server socket.

\param flags Flags for client socket.
\param fd Server's file descriptor.
\retval -1 on error (and see \a errno).
\returns file descriptor of new client on success.

This function attempts to accept() a connection from Unix domain socket server. It will retry on any
error except EAGAIN, which indicates that there are actually no more clients to accept. It should be
called in a loop until it does this. If a persistent error occurs, -1 will be returned and the last
error recorded in \a errno.

\a flags may contain \ref GSL_NONBLOCK to specify that a newly-accepted client will be in non-blocking 
mode.

*/
int unix_accept(int flags, int fd) __attribute__((warn_unused_result));



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/310_net/500_io.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup net_io Network I/O routines
\ingroup net

Routines for I/O on network connections.

*/
/*!@{*/



/*! \brief Read a packet into a membuf [non-blocking].

\param fd File descriptor of the packet socket (may be blocking or non-blocking).
\param[in,out] m Membuf to read into (may dereference to 0).
\param[out] addr Pointer to <code>struct addrinfo</code>. Source address written here. May be 0.
\retval 0 on success.
\retval -1 on error (and see \a errno).

This function will read a single packet into a memory buffer, which it will first clear (and 
possibly allocate) using membuf_realloc(). On error, -1 is returned and \a errno set as per 
recvfrom().

The client's remote address may be retrieved by passing a pointer to a <code>struct addrinfo</code>
in \a addr (see sockaddr_to_addrinfo()).

\note This function will return with \c EAGAIN if there are no packets available, even if the socket
    is in blocking mode. See \ref membuf_recv_wait() for an alternative solution that blocks, with
    an optional timeout.

*/
int membuf_recv(int fd, struct membuf_t** m, struct addrinfo* addr) __attribute__((nonnull(2)));



/*! \brief Read a packet into a membuf [blocking].

\param fd File descriptor of the packet socket (may be blocking or non-blocking).
\param[in,out] m Membuf to read into (may dereference to 0).
\param[out] addr Pointer to <code>struct addrinfo</code>. Source address written here. May be 0.
\param timeout Timeout, in milliseconds, as per \c poll(2).
\retval 0 on success.
\retval -1 on error (and see \a errno).

This function will read a single packet into a memory buffer, which it will first clear (and
possibly allocate) using membuf_realloc(). On error, -1 is returned and \a errno set as per
recvfrom().

The client's remote address may be retrieved by passing a pointer to a <code>struct addrinfo</code>
in \a addr (see sockaddr_to_addrinfo()).

This function uses \c poll(2) internally so it will block even if the socket is set to non-blocking
mode. The \a timeout argument may be negative for no timeout, 0 for instant timeout, or a positive
number for that many milliseconds. On a timeout, \a errno will be set to \c EAGAIN.

*/
int membuf_recv_wait(int fd, struct membuf_t** m, struct addrinfo* addr, int timeout)
    __attribute__((nonnull(2)));



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/310_net/900_nfsmount.h
 * 
 *  Copyright: ©2011, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup net_nfsmount NFS mount helpers
\ingroup net

A simple wrapper function around \c mount(2) that help to mount an NFS
filesystem. It can be used to programatically mount an NFS filesystem on
demand, much like calling <code>mount -t nfs</code>. The wrapper function
provide two things beyond \c mount(2): DNS lookups and policy defaults.

Technically this requires a 2.6.23 kernel, as it uses the more advanced
textual version of the opaque data pointer. Its policy defaults are:
\li \c hard
\li \c ac
\li \c nolock
\li \c cto

It will perform a DNS lookup to fill out the \c addr field of the \c mount(2)
data.

*/
/*!@{*/



/*! \brief Mount an NFS filesystem.

\param src Source address, in form <code>hostname:/directory</code>.
\param target Target mountpoint, e.g. <code>/mnt</code>.
\param mountflags Flags for \c mount(2).
\param opts Options from \c nfs(5). May be 0.
\param err_msg Buffer for error message. May be 0.
\param err_msg_len Size of error message buffer, in bytes.
\retval -1 on error (and see \a err_msg).
\retval 0 on success.

A wrapper function around \c mount(2) which helps perform an NFS mount. It
performs a DNS lookup on the hostname in \a src and builds an option string
suitable for passing to the kernel (\c fs/nfs/super.c), and then actually calls
\c mount(2).

On any error, this function returns -1, and writes a descriptive error message
in to \a err_msg (unless it is null, in which case the error cannot be
recovered). Don't rely on \a errno, since some of the types of error are from
\c getaddrinfo(3) and others are from internal checks in the function.

*/
int
gslutil_nfs_mount(const char* src,
    const char* target,
    unsigned long mountflags,
    const char* opts,
    char* err_msg,
    int err_msg_len)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)))
#endif
;



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/320_hash/000_doxygen.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup hash Hash and CRC functions

A set of functions for computing hashes and CRCs, and a hash table implementation.

*/



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/320_hash/100_hash.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup hash_hash Hash/CRC implementations
\ingroup hash

A set of hash functions and CRC computations.

*/
/*!@{*/



/*! \brief Compute string hash

\param data Pointer to data or C string.
\param len Size of data in bytes, or -1 for C strings.
\returns 32-bit hash of \a data

This function computes a string hash using the LOOKUP3 hash from Bob Jenkins.
This hash is intended as a fairly collision-resistant way of hashing ASCII
strings.  It is also a fast hash for arbitrary binary data. The 32-bit return
value may be truncated to a smaller length as necessary.

If \a data is a C string, pass -1 in \a len to compute over the length of the
string. Otherwise, pass the number of bytes to compute the hash over in \a len.

*/
uint32_t gsl_string_hash(const char* data, int len)
#ifndef DOXYGEN
    __attribute__((nonnull));
#endif
;



/*! \brief Compute memory vector hash

\param datv Pointer to array of memory regions.
\param datvcnt Number of entries in \a datv.
\returns 32-bit hash of \a data

This function computes a hash using the LOOKUP3 hash from Bob Jenkins. The
32-bit return value may be truncated to a smaller length as necessary.

\sa \ref gsl_string_hash() for a simpler interface.

*/
uint32_t gsl_memory_hash(const struct iovec* datv, int datvcnt)
#ifndef DOXYGEN
    __attribute__((nonnull));
#endif
;



/*! \brief Compute CRC32 checksum.

\param seed Initial seed value.
\param data Pointer to data.
\param len Size of data in bytes.
\returns CRC32 of \a data.

This function computes the CRC32 of the given data. You can perform incremental CRC by passing
the result of one CRC operation as the \a seed value of the next.

When \a seed starts at 0xFFFFFFFF and the final result is XORed with 0xFFFFFFFF, this implementation
is compatible with CRC-32-IEEE 802.3 (used in Ethernet et al.).

*/
uint32_t gsl_crc32(uint32_t seed, const char* data, uint32_t len) __attribute__((nonnull));



/*! \brief Compute SHA-256 hash.

\param data Pointer to data.
\param len Size of data in bytes.
\param sha A 32-byte buffer into which the digest will be written.

This function computes the SHA-256 hash of the given data.

*/
void gsl_sha256(const char* data, uint32_t len, char* sha) __attribute__((nonnull));



/*! \brief Allocate SHA-256 incremental hash object. */
struct gsl_sha256_ctx* gsl_sha256_alloc(void) __attribute__((malloc));

/*! \brief Perform incremental hash of data block.

\param ctx SHA-256 incremental hash object.
\param data Pointer to data.
\param len Size of data in bytes.

*/
void gsl_sha256_incr_hash(struct gsl_sha256_ctx* ctx, const char* data, uint32_t len)
    __attribute__((nonnull));

/*! \brief Finalise incremental hash, free hash object.

\param ctx SHA-256 incremental hash object. Will be freed.
\param sha A 32-byte buffer into which the digest will be written.

*/
void gsl_sha256_finish(struct gsl_sha256_ctx* ctx, char* sha) __attribute__((nonnull));



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/320_hash/150_md5.h
 * 
 *  Copyright: ©2011, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup hash_md5 MD5 implementation
\ingroup hash

The standard MD5 hash based on RFC1321. Note that MD5 hashes are deprecated for
security/cryptographic purposes; use SHA instead for new applications. This
implementation is public domain with a very thin wrapper layer for libgslutil.
It uses the opaque type <code>struct gsl_md5</code> for a context object.

*/
/*!@{*/



/* opaque structure */
struct gsl_md5;



/*! \brief Create MD5 context.

\returns Pointer to newly-allocated context object.

Pass the context object to \ref gsl_md5_update() to process data and free it
with \ref gsl_md5_finish() when done.

*/
struct gsl_md5* gsl_md5_start(void);



/*! \brief Process data with existing MD5 context.

\param p MD5 context.
\param data Pointer to data bytes.
\param len Number of bytes of data to process.

*/
void gsl_md5_update(struct gsl_md5* p, const void* data, size_t len);



/*! \brief Complete MD5 processing, returning sum, and free context.

\param p MD5 context.
\param[out] out Pointer to output buffer (at least 16 bytes). May be 0.

Writes the raw hash into \a out, if it was not null. Frees the MD5 context
object \a p.

*/
void gsl_md5_finish(struct gsl_md5* p, uint8_t* out);



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/320_hash/200_strtable.h
 *
 *  Copyright: ©2009–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup hash_table_str Dynamic hash table (string keys)
\ingroup hash

A generic hash table implementation. The key of the hash table is a
null-terminated C string. The hash table is automatically resized when
necessary, in order to reduce hash collisions and keep lookup operations as
O(1). The maximum number of bins is 2^31, though there is no limit to the
number of entries.

This is a wrapper around the \ref hash_table_mem module.

*/
/*!@{*/



/* opaque hash table type */
struct hash_table;



/*! \brief Create an empty hash table.

\param size Initial guess at size of hash table. May be 0.
\returns Pointer to newly-allocated opaque hash table object.

Creates and initialises an empty hash table object. The initial number of bins
to allocate may be specified in \a size, which is the base-2 logarithm of the
total number (i.e. passing \a size as 4 gets you 16 bins). Passing anything
less than 4 will still allocate 16 bins.

*/
struct hash_table* hash_table_new(int size)
#ifndef DOXYGEN
    __attribute__((malloc))
#endif
;



/*! \brief Free a hash table.

\param ht Hash table. May be 0.

Frees a hash table, but does not alter the objects pointed to by the table. In
order to free memory associated with entries, call \ref hash_table_for_each().
If \a ht is 0, no action is taken.

*/
void hash_table_free(struct hash_table* ht);



/*! \brief Retrieve an entry.

\param ht Hash table.
\param key Key of entry to look up.
\returns Value associated with key.
\retval 0 if there is no associated value.

Retrieves the value associated with the entry with key \a key. If there is no
such entry, then 0 is returned.

*/
void* hash_table_lookup(const struct hash_table* ht, const char* key)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Add, update or remove an entry.

\param ht Hash table.
\param key Key of entry.
\param value Value to store. 0 to delete.
\returns Previous value.
\retval 0 if there was no previous value.

Associates the value \a value with an entry with key \a key. If \a key had a
previous value associated with it, this previous value is returned. Passing a
value of 0 will clear the entry, freeing the memory associated with the entry.
If \a key had no previous value, 0 will be returned, and the new value will be
stored (or no action will be taken if \a value is 0).

Note that \a key is copied when creating a new entry, so the memory it points
to need not persist past the function call.

*/
void* hash_table_update(struct hash_table* ht, const char* key, void* value)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)))
#endif
;



/*! \brief Iterate through all entries.

\param ht Hash table.
\param func Callback function for each entry.
\retval 0 if \a func always returned 0.
\returns First non-zero value returned from \a func otherwise.

Iterates through each entry in the hash table \a ht and calls \a func for each.
\a func takes two arguments: the key and the value. If \a func ever returns
non-zero, the iteration is aborted and the value returned from \a func is
returned.

*/
void* hash_table_for_each(struct hash_table* ht,
    void* (*func)(const char* key, void* value))
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Iterate through all entries with user pointer.

\param ht Hash table.
\param func Callback function for each entry.
\param user Opaque user pointer to pass to callback.
\retval 0 if \a func always returned 0.
\returns First non-zero value returned from \a func otherwise.

Iterates through each entry in the hash table \a ht and calls \a func for each.
\a func takes three arguments: the key, the value and an opaque user pointer \a
user. If \a func ever returns non-zero, the iteration is aborted and the value
returned from \a func is returned.

*/
void* hash_table_for_each2(struct hash_table* ht,
    void* (*func)(const char* key, void* value, void* user),
    void* user)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)))
#endif
;



/*! \brief Delete all entries matching predicate.

\param ht Hash table.
\param pred Predicate function (returning 0 to keep or non-0 to delete).
\param user Opaque pointer to pass to callbacks (may be 0).
\returns Number of entries removed.

For each entry in the hash table \a ht, this function calls \a pred. This
callback is similar to that of \ref hash_table_for_each(), except that it
returns an integer, with 0 meaning ‘keep element’ and non-0 meaning ‘delete
element’.

If any cleanup of the element needs to be done (e.g. freeing of memory), this
can be done in \a pred before it returns. Other hash table functions, with the
exception of lookups, should not be called on \a ht from within the callback.

*/
int hash_table_delete_if(struct hash_table* ht,
    int (*pred)(const char* key, void* value, void* user),
    void* user)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)))
#endif
;



/*! \brief Debugging: dump contents of hash table to memory buffer.

\param ht Hash table.
\param buf Memory buffer to dump to.

A debugging function which will dump the contents of the hash table \a ht (i.e.
its keys and the pointer value associated with them) to the memory buffer \a
buf. The dump is in human-readable form and will consist only of printable
characters. It is terminated with a newline.

*/
void hash_table_debug_dump(const struct hash_table* ht, struct membuf_t* buf)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/320_hash/300_memtable.h
 *
 *  Copyright: ©2009–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup hash_table_mem Dynamic hash table (memory keys)
\ingroup hash

A generic hash table implementation. The key of the hash table is a single
block of memory. The hash table is automatically resized when necessary, in
order to reduce hash collisions and keep lookup operations as O(1). The maximum
number of bins is 2^31, though there is no limit to the number of entries.

*/
/*!@{*/



/* opaque hash table type */
struct mhash_table;



/*! \brief Create an empty hash table.

\param size Initial guess at size of hash table. May be 0.
\returns Pointer to newly-allocated opaque hash table object.

Creates and initialises an empty hash table object. The initial number of bins
to allocate may be specified in \a size, which is the base-2 logarithm of the
total number (i.e. passing \a size as 4 gets you 16 bins). Passing anything
less than 4 will still allocate 16 bins.

*/
struct mhash_table* mhash_table_new(int size)
#ifndef DOXYGEN
    __attribute__((malloc))
#endif
;



/*! \brief Free a hash table.

\param ht Hash table. May be 0.

Frees a hash table, but does not alter the objects pointed to by the table. In
order to free memory associated with entries, call \ref mhash_table_for_each().
If \a ht is 0, no action is taken.

*/
void mhash_table_free(struct mhash_table* ht);



/*! \brief Retrieve an entry.

\param ht Hash table.
\param key Key of entry to look up.
\param key_len Length of key, in bytes.
\returns Value associated with key.
\retval 0 if there is no associated value.

Retrieves the value associated with the entry with key \a key. If there is no
such entry, then 0 is returned.

*/
void* mhash_table_lookup(const struct mhash_table* ht, const char* key,
    size_t key_len)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Retrieve an entry (vector key).

\param ht Hash table.
\param keyv Pointer to array of memory regions which comprise the key.
\param keyvcnt Number of entries in \a keyv.
\returns Value associated with key.
\retval 0 if there is no associated value.

Retrieves the value associated with the entry with key \a keyv. If there is no
such entry, then 0 is returned.

The key is a set of bytes which may be discontiguous in memory. 1 or more
regions of memory are pointed to by \a keyv, and the key is the sequence of
bytes that would be encountered by stepping byte-by-byte through each region in
turn. Note in particular that the keys "a","bc" and "ab","c" would be
considered identical.

*/
void* mhash_table_lookupv(const struct mhash_table* ht,
    const struct iovec* keyv, int keyvcnt)
#ifndef DOXYGEN
    __attribute__((nonnull));
#endif
;



/*! \brief Add, update or remove an entry.

\param ht Hash table.
\param key Key of entry.
\param key_len Length of key, in bytes.
\param value Value to store. 0 to delete.
\returns Previous value.
\retval 0 if there was no previous value.

Associates the value \a value with an entry with key \a key. If \a key had a
previous value associated with it, this previous value is returned. Passing a
value of 0 will clear the entry, freeing the memory associated with the entry.
If \a key had no previous value, 0 will be returned, and the new value will be
stored (or no action will be taken if \a value is 0).

Note that \a key is copied when creating a new entry, so the memory it points
to need not persist past the function call.

*/
void* mhash_table_update(struct mhash_table* ht, const char* key,
    size_t key_len, void* value)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)))
#endif
;



/*! \brief Add, update or remove an entry (vector key).

\param ht Hash table.
\param keyv Pointer to array of memory regions which comprise the key.
\param keyvcnt Number of entries in \a keyv.
\param value Value to store. 0 to delete.
\returns Previous value.
\retval 0 if there was no previous value.

Associates the value \a value with an entry with key \a keyv. If \a keyv had a
previous value associated with it, this previous value is returned. Passing a
value of 0 will clear the entry, freeing the memory associated with the entry.
If \a keyv had no previous value, 0 will be returned, and the new value will be
stored (or no action will be taken if \a value is 0).

The key is a set of bytes which may be discontiguous in memory. 1 or more
regions of memory are pointed to by \a keyv, and the key is the sequence of
bytes that would be encountered by stepping byte-by-byte through each region in
turn. Note in particular that the keys "a","bc" and "ab","c" would be
considered identical. The memory regions comprising the key are copied
internally (forming a single contiguous memory region) when a new entry is
created.

*/
void* mhash_table_updatev(struct mhash_table* ht, const struct iovec* keyv,
    int keyvcnt, void* value)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)));
#endif
;



/*! \brief Iterate through all entries.

\param ht Hash table.
\param func Callback function for each entry.
\retval 0 if \a func always returned 0.
\returns First non-zero value returned from \a func otherwise.

Iterates through each entry in the hash table \a ht and calls \a func for each.
\a func takes three arguments: the key and its length, and the value. If \a
func ever returns non-zero, the iteration is aborted and the value returned
from \a func is returned.

*/
void* mhash_table_for_each(struct mhash_table* ht,
    void* (*func)(const char* key, size_t key_len, void* value))
#ifndef DOXYGEN
    __attribute__((nonnull));
#endif
;



/*! \brief Iterate through all entries.

\param ht Hash table.
\param func Callback function for each entry.
\param user Opaque user pointer to pass to callback.
\retval 0 if \a func always returned 0.
\returns First non-zero value returned from \a func otherwise.

Iterates through each entry in the hash table \a ht and calls \a func for each.
\a func takes four arguments: the key and its length, the value and an opaque
pointer \a user. If \a func ever returns non-zero, the iteration is aborted and
the value returned from \a func is returned.

*/
void* mhash_table_for_each2(struct mhash_table* ht,
    void* (*func)(const char* key, size_t key_len, void* value, void* user),
    void* user)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)));
#endif
;



/*! \brief Delete all entries matching predicate.

\param ht Hash table.
\param pred Predicate function (returning 0 to keep or non-0 to delete).
\param user Opaque pointer to pass to callbacks (may be 0).
\returns Number of entries removed.

For each entry in the hash table \a ht, this function calls \a pred. This
callback is similar to that of \ref mhash_table_for_each(), except that it
returns an integer, with 0 meaning ‘keep element’ and non-0 meaning ‘delete
element’.

If any cleanup of the element needs to be done (e.g. freeing of memory), this
can be done in \a pred before it returns. Other hash table functions, with the
exception of lookups, should not be called on \a ht from within the callback.

*/
int mhash_table_delete_if(struct mhash_table* ht,
    int (*pred)(const char* key, size_t key_len, void* value, void* user),
    void* user)
#ifndef DOXYGEN
    __attribute__((nonnull(1,2)))
#endif
;



/*! \brief Debugging: dump contents of hash table to memory buffer.

\param ht Hash table.
\param buf Memory buffer to dump to.

A debugging function which will dump the contents of the hash table \a ht (i.e.
its keys and the pointer value associated with them) to the memory buffer \a
buf. The dump is in human-readable form and will consist only of printable
characters. It is terminated with a newline.

*/
void mhash_table_debug_dump(const struct mhash_table* ht, struct membuf_t* buf)
#ifndef DOXYGEN
    __attribute__((nonnull));
#endif
;



/*!@}*/
/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/350_string/000_doxygen.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup cffile Configuration files

Routines for loading and saving configuration files.

Simple flat "name = value" style configuration files are supported (see \ref cffile_orig).
Configuration files with sections are also supported (see \ref cffile2). If your configuration file
format needs to be any more complicated, you should consider using Streamed XML.

*/



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/350_string/100_cffile.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup cffile_orig Configuration file manipulation
\ingroup cffile

A set of routines for dealing with simple, flat configuration files consisting of a number of string
tuples. gslutil_cffile_load() loads a set of these tuples into memory. gslutil_cffile_save() writes
a set of tuples to disk, attempting to keep the structure of the previous file intact. Any errors 
encountered will be described by the string gslutil_cffile_error.

The configuration file consists of a set of tuples, in the format:

<pre># comment
name = value # comment</pre>

\c name is a single string token, without whitespace. \c value may not be blank, but may contain
whitespace. Any whitespace before or after \c name is stripped. The `=' symbol may or may not be
surrounded by whitespace. Any whitespace before or after (but not within) \c value is stripped.

Any characters after a `#' character (including the character itself) are considered to be comments
and are ignored. Blank lines, or lines consisting only of whitespace, are skipped. Tuples may not
be repeated (i.e. the name must be unique). Tuples are sorted lexically by name within the config
structure.

When saving, any tuples in the new data with names matching tuples in the original file are simply
updated so that the values match the new data. New tuples are appended to the end of the file.
Tuples which exist on disk but not in the new data are commented out. The save routine tries to
leave comments intact. Any syntactical errors in the original are commented out.

*/
/*!@{*/



/*! \brief Configuration file error reporting.

If any error is encountered while loading or saving a configuration file, this string will be set
to hold a verbose description of the error.

*/
extern const char* const gslutil_cffile_error;



/*! \brief Configuration file tuple.

The value be manipulated by gslutil_cffile_change_tuple(). Direct changes to the elements are not
permitted.

*/
struct gslutil_cffile_tuple {
    /*! \brief Tuple name (unique). */
    char* name;
    /*! \brief Tuple value. */
    char* value;
};



/*! \brief Simple configuration file structure.

Can be manipulated by gslutil_cffile_add_tuple(), gslutil_cffile_remove_tuple_by_name() and
gslutil_cffile_remove_tuple_by_pos(). Can be allocated with gslutil_cffile_alloc() (or as a result
of gslutil_cffile_load()) and freed with gslutil_cffile_free(). You can search for a specific tuple
with gslutil_cffile_find_tuple().

*/
struct gslutil_cffile_t {
    /*! \brief List of tuples, sorted lexically by name. */
    struct gslutil_cffile_tuple* tuples;
    /*! \brief Number of valid tuples. */
    int num;
    /*! \brief Size of tuple array. */
    int size;
};



/*! \brief Load configuration file.

\param fname Filename.
\returns Pointer to newly-allocated configuration file structure.
\retval 0 on error (and see \ref gslutil_cffile_error).

Loads tuples from a configuration file named \a fname. If the loading succeeds, a new 
gslutil_cffile_t is allocated and returned as the result. Pass this to gslutil_cffile_free() when
finished to free any memory associated with it.

On error, no structure is allocated, 0 is returned, and a verbose human-readable error message is
written into \ref gslutil_cffile_error.

*/
struct gslutil_cffile_t* gslutil_cffile_load(const char* fname)
    __attribute__((warn_unused_result,nonnull,malloc));



/*! \brief Save configuration file.

\param fname Filename.
\param cf Configuration tuple structure.
\retval 0 on success.
\retval -1 on error (and see \ref gslutil_cffile_error).

Saves the list of configuration tuples \a cf into the file name \a fname. If the file already 
exists, it is edited in a non-destructive manner (see \ref cffile). In any case, the new file is
written to a dotfile in the same directory first before being renamed with \c rename(2).

The only errors that can occur are disk I/O errors, which will be reported through the 
\ref gslutil_cffile_error mechanism.

*/
int gslutil_cffile_save(const char* fname, const struct gslutil_cffile_t* cf)
    __attribute__((nonnull));



/*! \brief Change value of configuration file tuple.

\param tuple Pointer to tuple object to change.
\param new_value The new value to set.

Changes the value of the referenced \a tuple to be \a new_value. Will create a copy of the string,
so the original need not persist. Uses safe_malloc(), so always succeeds.

*/
void gslutil_cffile_change_tuple(struct gslutil_cffile_tuple* tuple, const char* new_value)
    __attribute__((nonnull));



/*! \brief Allocate a new, empty configuration file structure.

\returns Pointer to new structure.

Allocates a new, empty configuration file structure. Uses safe_malloc(), so always succeeds.

*/
struct gslutil_cffile_t* gslutil_cffile_alloc(void) __attribute__((warn_unused_result,malloc));



/*! \brief Free a configuration file structure.

\param cf Configuration tuple structure.

Frees the configuration tuple structure \a cf and all of its constituent tuples. Accepts null values
for \a cf.

*/
void gslutil_cffile_free(struct gslutil_cffile_t* cf);



/*! \brief Add a new tuple to a configuration file structure.

\param cf Configuration tuple structure.
\param name Name of new tuple (must be unique).
\param value Value of new tuple.
\retval 0 on success.
\retval -1 on error (duplicate entry).

Adds a new tuple (\a name and \a value) to the structure \a cf. \a name must be unique; if a tuple
already exists in \a cf with the same name, -1 will be returned. \a name and \a value are both
copied, so the original string need not persist.

*/
int gslutil_cffile_add_tuple(struct gslutil_cffile_t* cf, const char* name, const char* value)
    __attribute__((nonnull));



/*! \brief Remove a tuple from a configuration file structure.

\param cf Configuration tuple structure.
\param name Name of tuple to remove.
\retval 0 on success.
\retval -1 on error (no such tuple).

Removes a tuple with name \a name from \a cf, returning 0 if it was found or -1 if not.

*/
int gslutil_cffile_remove_tuple_by_name(struct gslutil_cffile_t* cf, const char* name)
    __attribute__((nonnull));



/*! \brief Remove a tuple from a configuration file structure.

\param cf Configuration tuple structure.
\param pos Position within array (not checked).

Removes the tuple from array position \a pos from \a cf. \a pos is not bounds-checked. Passing an
invalid value will crash your program.

*/
void gslutil_cffile_remove_tuple_by_pos(struct gslutil_cffile_t* cf, int pos)
    __attribute__((nonnull));



/*! \brief Find a tuple by name.

\param cf Configuration tuple structure.
\param name Name of tuple to find.
\returns Pointer to tuple.
\retval 0 if not found.

Searches for a tuple by name. Does a binary search. If no tuple with a matching name is found,
returns 0.

*/
struct gslutil_cffile_tuple* gslutil_cffile_find_tuple(struct gslutil_cffile_t* cf,
    const char* name) __attribute__((nonnull));



/*! \brief Get value from file.

\param cf Configuration tuple structure.
\param name Name of tuple to find.
\returns Pointer to value.
\retval 0 if not found.

Searches for a tuple by name. Does a binary search. If no tuple with a matching name is found,
returns 0. Otherwise, returns a pointer to its value.

*/
char* gslutil_cffile_find_value(const struct gslutil_cffile_t* cf,
    const char* name) __attribute__((nonnull));



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/350_string/200_cffile2.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Laurence Withers, <l@lwithers.me.uk>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
*/



/*! \defgroup cffile2 Sectioned configuration file manipulation
\ingroup cffile

A set of routines for dealing with flat configuration files consisting of a number of string tuples
divided into sections. gslutil_cffile2_load() loads a set of these tuples into memory, categorised
by section. gslutil_cffile2_save() writes these sets of tuples to disk. Any errors encountered will
be described by the string gslutil_cffile_error.

See \ref cffile_orig for the tuple format. Subsections are indicated in the file
by a string marked with square brackets. The string is the subsection title, and must be unique
within the configuration file. For example:

<pre>name = value         # stored as global
[subsection1]
name = value
[subsection2]
name = value</pre>

When parsed with gslutil_cffile2_load, this will result in a struct gslutil_cffile2_t with a single
global tuple and two named subsections each with a single tuple.

*/
/*!@{*/



/*! \brief Structure for subsection of a configuration file.

This structure holds the data in a [marked] subsection of a configuration file. The subsection
title is held in \a name, and the tuples within it in \a values. \a values can be manipulated as
desired.

*/
struct gslutil_cffile2_subsection {
    /*! \brief Name of [marked] subsection. */
    char* name;
    /*! \brief Values associated with subsection. */
    struct gslutil_cffile_t* values;
};



/*! \brief Structure for configuration file with subsections.

This structure is used to hold a parsed configuration file with sections. Anything in the global
section (i.e. name = value pairs before any [marked] subsections) is stored in the \a global
member.

Can be allocated with gslutil_cffile2_alloc(), loaded with gslutil_cffile2_load() and freed with
gslutil_cffile2_free(). Subsections can be added with gslutil_cffile2_add_subsection().

*/
struct gslutil_cffile2_t {
    /*! \brief Values occurring before any [marked] subsections. */
    struct gslutil_cffile_t* global;
    /*! \brief List of subsections, in order they were encountered. */
    struct gslutil_cffile2_subsection* subsections;
    /*! \brief Number of subsections. */
    int num_sub;
    /*! \brief Size of subsections array. */
    int size_sub;
};



/*! \brief Load configuration file with sections.

\param fname Filename.
\returns Pointer to newly-allocated configuration file structure.
\retval 0 on error (and see \ref gslutil_cffile_error).

Loads tuples from a configuration file named \a fname. If the loading succeeds, a new
gslutil_cffile_t is allocated and returned as the result. Pass this to gslutil_cffile2_free() when
finished to free any memory associated with it.

On error, no structure is allocated, 0 is returned, and a verbose human-readable error message is
written into \ref gslutil_cffile_error.

*/
struct gslutil_cffile2_t* gslutil_cffile2_load(const char* fname)
    __attribute__((warn_unused_result,nonnull,malloc));



/*! \brief Save configuration file with sections.

\param fname Filename.
\param cf2 Configuration structure.
\retval 0 on success.
\retval -1 on error (and see \ref gslutil_cffile_error).

Saves the configuration file \a cf2 to \a fname. On error, leaves `fname' unchanged and returns -1,
setting \ref gslutil_cffile_error.

\warning This saving routine is less satisfactory than \ref gslutil_cffile_save, as it does not make
    any effort to retain the original comments or structure.

*/
int gslutil_cffile2_save(const char* fname, const struct gslutil_cffile2_t* cf2)
    __attribute__((nonnull));



/*! \brief Allocate new, empty configuration file structure with sections.

\returns Pointer to new structure.

Allocates a new, empty configuration file structure. Uses safe_malloc(), so always succeeds.

*/
struct gslutil_cffile2_t* gslutil_cffile2_alloc(void) __attribute__((warn_unused_result,malloc));



/*! \brief Free a configuration file structure with sections.

\param cf Configuration file structure.

Frees the configuration file structure \a cf and all of its constituent tuples and subsections.
Accepts null values for \a cf.

*/
void gslutil_cffile2_free(struct gslutil_cffile2_t* cf);



/*! \brief Add subsection to configuration file structure.

\param cf Configuration file structure.
\param name Name of subsection.
\retval 0 on success.
\retval -1 on error (duplicate subsection name).

Adds an empty subsection to the configuration file structure \a cf. The new subsection will have a
name copied from the string \a name. If a
subsection with a matching name already exists, \a cf is not altered and -1 is returned.

*/
int gslutil_cffile2_add_subsection(struct gslutil_cffile2_t* cf, const char* name)
    __attribute__((nonnull));



/*! \brief Get pointer to subsection by name.

\param cf Configuration file structure.
\param name Name of subsection.
\returns Pointer to subsection.
\retval 0 if not found.

Gets a pointer to the subsection of \a cf named \a name. If not found, returns 0. This is a linear
lookup.

*/
struct gslutil_cffile_t* gslutil_cffile2_get_subsection(struct gslutil_cffile2_t* cf,
    const char* name) __attribute__((nonnull));



/*! \brief Get tuple from subsection.

\param cf Configuration file structure.
\param section_name Name of subsection.
\param entry_name Name of configuration entry.
\returns Pointer to tuple.
\retval 0 if not found.

Gets a pointer to the tuple named \a entry_name in the subsection of \a cf named \a section_name.
If not found, returns 0.

*/
struct gslutil_cffile_tuple* gslutil_cffile2_get_tuple(struct gslutil_cffile2_t* cf,
    const char* section_name, const char* entry_name) __attribute__((nonnull(1,3)));



/*! \brief Get value from subsection.

\param cf Configuration file structure.
\param section_name Name of subsection.
\param entry_name Name of configuration entry.
\returns Pointer to value.
\retval 0 if not found.

Gets a pointer to the value of the tuple named \a entry_name in the subsection of \a cf named
\a section_name. If not found, returns 0.

If \a section_name is 0 then the \a global section is searched.

*/
char* gslutil_cffile2_get_value(const struct gslutil_cffile2_t* cf,
    const char* section_name, const char* entry_name) __attribute__((nonnull(1,3)));



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/400_usergroup/000_doxygen.h
 *
 *  Copyright: ©2010, Güralp Systems Ltd.
 *  Author: Laurence Withers, <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup usergroup User and group handling
 *
 *  Convenience functions for programs which deal with users and groups,
 *  including name lookups and setting file permissions etc.
 */



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/400_usergroup/100_lookup.h
 *
 *  Copyright: ©2010, Güralp Systems Ltd.
 *  Author: Laurence Withers, <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup usergroup_lookup User and group ID lookups
\ingroup usergroup

Fast, caching lookup functions for user and group IDs. These actually scan
the system database once and store the results into a hash table so they can
be retrieved at O(1) when required. On a simple system with a flat file
store containing a few entries this pays off after 2–3 lookups.

Initialisation of the user and/or group hash table is automatic when the
corresponding lookup function is first called.

\note No mechanism currently exists to re-scan /etc/passwd or /etc/group after
  first use but one would be trivial to add if this feature was deemed to be
  necessary.

*/

/*!@{*/



/*! \brief Look up uid by name.

\param name User name to look up.
\returns uid
\retval \c (uid_t)-1 on error, e.g. no such user

*/
uid_t gsl_lookup_uid(const char* name)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Look up gid by name.

\param name Group name to look up.
\returns gid
\retval \c (gid_t)-1 on error, e.g. no such group

*/
gid_t gsl_lookup_gid(const char* name)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/400_usergroup/200_files.h
 *
 *  Copyright: ©2010, Güralp Systems Ltd.
 *  Author: Laurence Withers, <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup usergroup_files File ownership and permissions
\ingroup usergroup

Utility functions for manipulating file ownerships and permissions.

*/

/*!@{*/



/*! \brief Set file ownership and mode.

\param path Pathname of file to operate on.
\param owner Name of user that will own file; pass 0 for unchanged
\param group Name of group that will own file; pass 0 for unchanged
\param mode New mode for file; pass -1 for unchanged
\retval 0 on success
\retval -1 on error (and see \a errno)

Changes the ownership, group and/or permissions of a file (or directory etc.;
does not operate on symlinks but always dereferences them). Any errors are
logged at \c LOG_WARNING.

If lookup of \a owner or \a group fails, that parameter is ignored, \a errno is
set to \c EINVAL, and the function continues. When it completes it will return
-1 to signal error.

The function then changes the ownership with \c chown(2). If this fails,
\a errno is left set and the function continues. When the function completes it
will return -1.

Next the function changes the file mode with \c chmod(2). If this fails,
\a errno is left set (overriding any earlier error) and the function returns -1.
If any other errors occurred earlier, the function also returns -1.

Only if all operations succeeded completely will the function return 0.

*/
int gsl_set_perms(const char* path,
    const char* owner,
    const char* group,
    mode_t mode)
#ifndef DOXYGEN
    __attribute__((nonnull(1)))
#endif
;



/*! \brief Create a directory.

\param path Pathname of directory to create
\param owner Name of user that will own directory; pass 0 for unchanged
\param group Name of group for directory; pass 0 for unchanged
\param mode Octal mode for new directory, pass -1 for unchanged
\retval 0 on success
\retval -1 on error (and see \a errno)

Creates a directory at \a path. If the directory cannot be created then returns
-1, leaving \a errno set as per \c mkdir(2) (unless the error is \c EEXIST, in
which case the function continues). Note the directory is created with a mode of
0700 less umask.

If the directory is created (or existed), then \ref gsl_set_perms() is called on
it and the function returns 0 (leaving \a errno set as per \ref gsl_set_perms()
but ignoring that function's return value).

*/
int gsl_mkdir(const char* path,
    const char* owner,
    const char* group,
    mode_t mode)
#ifndef DOXYGEN
    __attribute__((nonnull(1)))
#endif
;



/*! \brief Recursively unlink a file or directory.

\param path Path of file or directory to delete.
\retval 0 on success.
\retval -1 on error (and see \a errno).

Attempts to unlink a file or remove a directory. If the directory has contents,
they are recursively unlinked/removed as well.

If an error occurs, the process continues, unlinking as much as possible.
\a errno will be set as per the very first error to occur, which should give
an indication of why the unlink failed.

This is basically the equivalent of the shell command <code>rm -rf path</code>.

*/
int gsl_recursive_unlink(const char* path)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/400_usergroup/300_cgi.h
 *
 *  Copyright: ©2011, Güralp Systems Ltd.
 *  Author: Laurence Withers, <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup usergroup_cgi CGI program support routine
\ingroup usergroup

Support routines for manipulating user/group ID within CGI programs. Relies on
an environment variable (\c REMOTE_USER) set by the web server as per the CGI
standard, and sets the user/group ID as appropriate (falling back to \c
nobody). Intended for running CGI programs as the logged-in user.

*/
/*!@{*/



/*! \brief Set user/group for CGI program.

\retval -1 on error (and see \a errno).
\retval 0 on success.

Reads the environment variable \c REMOTE_USER and sets the process's UID, GID
and auxiliary groups based on its value. If \c REMOTE_USER is not set, then
it uses \c nobody.

*/
int usergroup_cgi(void)
#ifndef DOXYGEN
    __attribute__((warn_unused_result))
#endif
;



/*! \brief Set user/group for CGI program, die on failure.

As per \ref usergroup_cgi(), but always succeeds (or aborts the program).

*/
void usergroup_cgi_or_die(void);



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/410_processes/000_doxygen.h
 *
 *  Copyright: ©2010, Güralp Systems Ltd.
 *  Author: Laurence Withers, <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup processes Process creation and manipulation
 *
 *  Convenience functions for starting child processes and manipulating other
 *  processes running on the system.
 */



/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/500_exec.h
 *
 *  Copyright: ©2009–2010, Güralp Systems Ltd.
 *  Author: Laurence Withers, <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup processes_exec Process creation
 * \ingroup processes
 *
 *  A set of routines for easy process creation and management.
 */

/*!@{*/



/*! \brief Close all open file descriptors.

\param leave_std_open Pass 0 to close \a stdin, \a stdout and \a stderr or non-0 to keep them open.

This function attempts to close all file descriptors that the process has open. It calls
\c opendir("/proc/self/fd"), closing each file descriptor in turn. If a non-0 value is passed for
\a leave_std_open, then file descriptors 0, 1 and 2 (conventionally stdin, stdout and stderr) will
be left open.

*/
void close_all_file_descriptors(int leave_std_open);



/*! \brief Fork a child process and use it to execute another executable.
 *
 *  \param filename Full pathname (possibly relative) to executable.
 *  \param argv Argument vector (arg 0 is filename as perceived by executable).
 *  \param envp Environment vector (may be 0 to inherit current environment).
 *  \param[out] stdin File descriptor for stdin write-pipe stored here, may be 0.
 *  \param[out] stdout File descriptor for stdout read-pipe stored here, may be 0.
 *  \param[out] stderr File descriptor for stderr read-pipe stored here, may be 0.
 *  \retval (pid_t)-1 on error (logged -- and see \a errno).
 *  \returns PID of child process on success.
 *
 *  Forks a child process and uses the child to execute another executable. You must specify the
 *  executable's full \a filename and its argument vector \a argv. You may optionally specify an
 *  environment with \a envp (but leaving this as zero will cause the child to inherit its parent's
 *  environment).
 *
 *  The child will have only three file descriptors open: 0 (stdin), 1 (stdout) and 2 (stderr). If
 *  you want to connect these to the parent with pipes, pass in the address of an integer where the
 *  pipe file descriptor can be stored. Otherwise, the child's file descriptors will point at
 *  \c /dev/null.
 *
 *  The child process will not have any blocked signals.
 *
 *  This call may fail if \c pipe() fails or if \c fork() fails. Either case is logged to syslog. If
 *  \c execve() fails, the child will exit with a status of 1, but that will not be indicated as an
 *  error through the return value of this function. It will be logged to syslog, however.
 */
pid_t fork_and_execve(const char* filename, char* const argv[], char* const envp[],
    int* stdin, int* stdout, int* stderr) __attribute__((nonnull(1,2)));



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/200_stale.h
 *
 *  Copyright: ©2010, Güralp Systems Ltd.
 *  Author: Laurence Withers, <lwithers@guralp.com>
 *  License: GPLv3
*/



/*! \defgroup processes_stale Stale process checking.
 * \ingroup processes
 *
 *  A set of routines which allow checking for stale processes. The function
 *  \ref gsl_stale_get_tokens() is first called in the target process (the one
 *  being checked), and the resulting tokens are written into the file/resource
 *  that requires the check. Consumer processes then read these tokens and pass
 *  them to \ref gsl_is_stale(), which will return 0 if the associated process
 *  is fresh or non-0 if stale.
 */

/*!@{*/



/*! \brief Get tokens to signify current process is fresh.
 *
 *  \param[out] tok_a Set to first token.
 *  \param[out] tok_b Set to second token.
 *
 *  This function returns two tokens for the current process which can be used
 *  by any other process on the system to determine if this process is still
 *  running (fresh) or has exited (stale).
 *
 *  The two tokens returned may be written to a file etc. and any other process
 *  on the same system can use them to determine stale/fresh information at any
 *  time.
 */
void
gsl_stale_get_tokens(uint32_t* tok_a, uint32_t* tok_b)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Get tokens to signify no process associated with resource.
 *
 *  \param[out] tok_a Set to first token.
 *  \param[out] tok_b Set to second token.
 *
 *  A degenerate case of \ref gsl_stale_get_tokens(), this returns two special
 *  tokens which can be used to denote a file or resource is not associated with
 *  a running process (and thus if passed to \ref gsl_is_stale() the result will
 *  always indicate a fresh file/resource).
 */
void
gsl_stale_get_nonprocess_tokens(uint32_t* tok_a, uint32_t* tok_b)
#ifndef DOXYGEN
    __attribute__((nonnull))
#endif
;



/*! \brief Check if process associated with tokens is stale.
 *
 *  \param tok_a First token.
 *  \param tok_b Second token.
 *  \retval 0 on success (process is fresh).
 *  \retval 1 on fail (process is stale).
 *
 *  Given two tokens originally returned from \ref gsl_stale_get_tokens(),
 *  checks to see if the process that originally created those tokens is still
 *  fresh or not.
 *
 *  If the process is still fresh, returns 0; otherwise, returns 1 (stale).
 */
int
gsl_is_stale(uint32_t tok_a, uint32_t tok_b);



/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/600_random.h
 *
 *  (c)2009, Guralp Systems Ltd. Written by Kelly Dunlop, <kdunlop@guralp.com>.
 *  Released under the GNU GPLv2. See file COPYING or
 *  http://www.gnu.org/copyleft/gpl.html for details.
 */


/*! \defgroup random Generate random numbers
 *
 *  This module contains a set of functions for generating random numbers
 *  using /dev/urandom.  If it fails to open this it falls back to using
 *  the random() system call.
 *  Logic should be call \a random_begin_reading() before using \a random_read()
 *  and call \a random_end_reading() after to tidy up.
 */
/*!@{*/

/*! \brief Name of urandom device
 */
#define URANDOM_FILENAME   "/dev/urandom"


/*! \brief Open /dev/urandom
 *
 *  \returns 0 on success or -1 on failure
 *
 *  Open URANDOM_FILENAME - more efficient to do it first and then
 *  use it although should work if opened each time it's used.
 */
int random_begin_reading( void );


/*! \brief Read random number from system.
 *
 *  \param buf Pointer to buffer for data.
 *  \param count Number of bytes to read.
 *
 *  Reads \a count bytes from \a URANDOM_FILENAME.   If it hasn't already
 *  been opened (by calling \a random_begin_reading()) then tries to open it.
 *  Note it's more efficient to call \a random_begin_reading() first else
 *  \a URANDOM_FILENAME is opened (and closed) each time \a random_read() is
 *  called.
 */
void random_read( u_char *buf, int count );


/*! \brief Tidy up after getting random numbers.
 *
 *  Closes the file descriptor for \a URANDOM_FILENAME if it is still open.
 */
void random_end_reading( void );


/* Useful wrapper functions */


/*!  \brief Get an 8 bit random number.
 *
 *   \returns 8 bit random number.
 *
 *   Calls \a random_read() with 1 as the count.
 */
uint8_t random_8( void );


/*!  \brief Get a 16 bit random number.
 *
 *   \returns 16 bit random number.
 *
 *   Calls \a random_read() with 2 as the count.
 */
uint16_t random_16( void );


/*!  \brief Get a 32 bit random number.
 *
 *   \returns 32 bit random number.
 *
 *   Calls \a random_read() with 4 as the count.
 */
uint32_t random_32( void );


/*!  \brief Get a 64 bit random number.
 *
 *   \returns 64 bit random number.
 *
 *   Calls \a random_read() with 8 as the count.
 */
uint64_t random_64( void );


/*!@}*/

/* options for text editors
kate: replace-trailing-space-save true; space-indent true; tab-width 4;
vim: expandtab:ts=4:sw=4
*/

/* libgslutil/src/libgslutil/650_machine_id.h
 * 
 *  Copyright: ©2011, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */



/*! \defgroup machine_id Machine ID

\c dbus and, more latterly, \c systemd define a useful concept: a machine ID.
Underneath the hood this is a UUID.

\sa http://0pointer.de/public/systemd-man/machine-id.html .

*/
/*!@{*/



/*! \brief Length for null-terminated machine ID string. */
#define GSL_MACHINE_ID_LEN              (33)

/*! \brief Length for raw machine ID bytes. */
#define GSL_MACHINE_UUID_LEN            (16)



/*! \brief Read machine ID.

\param[out] strbuf String buffer, containing ASCII machine ID.
\param[out] uuid Raw hex digits of UUID.
\param[out] was_generated True if result generated by this function.

Reads the machine ID from \c /etc/machine-id (if present). If not present, or
the contents of the file are invalid, generates a machine ID locally. Such
local generation will result in a different UUID on each call, and will result
in \a *was_generated being set to a non-zero value (if not null), and the UUID
will be a v4 (random) UUID.

If \a strbuf is not null, it must point to a buffer of at least
\ref GSL_MACHINE_ID_LEN bytes (which includes space for a terminating null).
If \a uuid is not null, it must point to a buffer of \ref GSL_MACHINE_UUID_LEN
bytes.

*/
void gsl_get_machine_id(char* strbuf, uint8_t* uuid, int* was_generated);



/*!@}*/
/* options for text editors
vim: expandtab:ts=4:sw=4:syntax=ch.doxygen
*/

/* libgslutil/src/libgslutil/999_BottomHeader.h
 * 
 *  Copyright: ©2009–2011, Güralp Systems Ltd.
 *  Author: Laurence Withers <lwithers@guralp.com>
 *  License: GPLv3
 */

#ifdef __cplusplus
}
#endif

#endif

/* options for text editors
vim: expandtab:ts=4:sw=4
*/
