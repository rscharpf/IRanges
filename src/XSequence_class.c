/****************************************************************************
 *               Low-level manipulation of XSequence objects                *
 *                           Author: Herve Pages                            *
 ****************************************************************************/
#include "IRanges.h"

static int debug = 0;

SEXP debug_XSequence_class()
{
#ifdef DEBUG_IRANGES
	debug = !debug;
	Rprintf("Debug mode turned %s in file %s\n",
		debug ? "on" : "off", __FILE__);
#else
	Rprintf("Debug mode not available in file %s\n", __FILE__);
#endif
	return R_NilValue;
}


/****************************************************************************
 * C-level slot getters.
 *
 * Be careful that these functions do NOT duplicate the returned slot.
 * Thus they cannot be made .Call() entry points!
 */

static SEXP
	shared_symbol = NULL,
	offset_symbol = NULL,
	length_symbol = NULL;

SEXP _get_XSequence_shared(SEXP x)
{
	INIT_STATIC_SYMBOL(shared)
	return GET_SLOT(x, shared_symbol);
}

SEXP _get_XSequence_offset(SEXP x)
{
	INIT_STATIC_SYMBOL(offset)
	return GET_SLOT(x, offset_symbol);
}

SEXP _get_XSequence_length(SEXP x)
{
	INIT_STATIC_SYMBOL(length)
	return GET_SLOT(x, length_symbol);
}

/* Not a strict "slot getter" but very much like. */
SEXP _get_XSequence_tag(SEXP x)
{
	return _get_SharedVector_tag(_get_XSequence_shared(x));
}


/****************************************************************************
 * Caching.
 */

cachedCharSeq _cache_XRaw(SEXP x)
{
	cachedCharSeq cached_x;
	SEXP tag;
	int offset;

	tag = _get_XSequence_tag(x);
	offset = INTEGER(_get_XSequence_offset(x))[0];
	cached_x.seq = (const char *) (RAW(tag) + offset);
	cached_x.length = INTEGER(_get_XSequence_length(x))[0];
	return cached_x;
}


/****************************************************************************
 * C-level slot setters.
 *
 * Be careful that these functions do NOT duplicate the assigned value!
 */

static void set_XSequence_shared(SEXP x, SEXP value)
{
	INIT_STATIC_SYMBOL(shared)
	SET_SLOT(x, shared_symbol, value);
	return;
}

static void set_XSequence_offset(SEXP x, SEXP value)
{
	INIT_STATIC_SYMBOL(offset)
	SET_SLOT(x, offset_symbol, value);
	return;
}

static void set_XSequence_length(SEXP x, SEXP value)
{
	INIT_STATIC_SYMBOL(length)
	SET_SLOT(x, length_symbol, value);
	return;
}

static void set_XSequence_slots(SEXP x, SEXP shared, SEXP offset, SEXP length)
{
	set_XSequence_shared(x, shared);
	set_XSequence_offset(x, offset);
	set_XSequence_length(x, length);
}


/****************************************************************************
 * C-level constructors.
 *
 * Be careful that these functions do NOT duplicate their arguments before
 * putting them in the slots of the returned object.
 * Thus they cannot be made .Call() entry points!
 */

SEXP _new_XSequence(const char *classname, SEXP shared, int offset, int length)
{
	SEXP classdef, ans, ans_offset, ans_length;

	PROTECT(classdef = MAKE_CLASS(classname));
	PROTECT(ans = NEW_OBJECT(classdef));
	PROTECT(ans_offset = ScalarInteger(offset));
	PROTECT(ans_length = ScalarInteger(length));
	set_XSequence_slots(ans, shared, ans_offset, ans_length);
	UNPROTECT(4);
	return ans;
}

SEXP _new_XRaw_from_tag(const char *classname, SEXP tag)
{
	SEXP shared, ans;

	if (!IS_RAW(tag))
		error("IRanges internal error in _new_XRaw_from_tag(): "
		      "'tag' is not RAW");
	PROTECT(shared = _new_SharedVector("SharedRaw", tag));
	PROTECT(ans = _new_XSequence(classname, shared, 0, LENGTH(tag)));
	UNPROTECT(2);
	return ans;
}

SEXP _new_XInteger_from_tag(const char *classname, SEXP tag)
{
	SEXP shared, ans;

	if (!IS_INTEGER(tag))
		error("IRanges internal error in _new_XInteger_from_tag(): "
		      "'tag' is not INTEGER");
	PROTECT(shared = _new_SharedVector("SharedInteger", tag));
	PROTECT(ans = _new_XSequence(classname, shared, 0, LENGTH(tag)));
	UNPROTECT(2);
	return ans;
}

SEXP _new_XDouble_from_tag(const char *classname, SEXP tag)
{
	SEXP shared, ans;

	if (!IS_NUMERIC(tag))
		error("IRanges internal error in _new_XDouble_from_tag(): "
		      "'tag' is not NUMERIC");
	PROTECT(shared = _new_SharedVector("SharedDouble", tag));
	PROTECT(ans = _new_XSequence(classname, shared, 0, LENGTH(tag)));
	UNPROTECT(2);
	return ans;
}

