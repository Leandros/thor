// yvals.h internal header
#pragma once
#ifndef _YVALS
#define _YVALS
#ifndef RC_INVOKED

// _HAS_CXX17 directly controls:
// P0025R1 clamp()
// P0032R3 Homogeneous Interface For variant/any/optional
// P0077R2 is_callable, is_nothrow_callable
// P0088R3 <variant>
// P0163R0 shared_ptr::weak_type
// P0185R1 is_swappable, is_nothrow_swappable
// P0209R2 make_from_tuple()
// P0220R1 <any>, <optional>, <string_view>, apply(), sample()
//     (other components not yet implemented)
// P0254R2 Integrating string_view And std::string
// P0272R1 Non-const basic_string::data()
// P0307R2 Making Optional Greater Equal Again
// P0393R3 Making Variant Greater Equal
// P0504R0 Revisiting in_place_t/in_place_type_t<T>/in_place_index_t<I>
// P0510R0 Rejecting variants Of Nothing, Arrays, References, And Incomplete Types

// _HAS_CXX17 indirectly controls:
// N4190 Removing auto_ptr, random_shuffle(), And Old <functional> Stuff
// P0004R1 Removing Deprecated Iostreams Aliases
// LWG 2385 function::assign allocator argument doesn't make sense
// The non-Standard std::tr1 namespace and TR1-only machinery
// The non-Standard std::identity struct

 #ifndef _HAS_CXX17
  #if defined(_MSVC_LANG) && !(defined(__EDG__) && defined(__clang__))	// TRANSITION, VSO#273681
   #if _MSVC_LANG > 201402
    #define _HAS_CXX17	1
   #else /* _MSVC_LANG > 201402 */
    #define _HAS_CXX17	0
   #endif /* _MSVC_LANG > 201402 */
  #else /* _MSVC_LANG etc. */
   #if __cplusplus > 201402
    #define _HAS_CXX17	1
   #else /* __cplusplus > 201402 */
    #define _HAS_CXX17	0
   #endif /* __cplusplus > 201402 */
  #endif /* _MSVC_LANG etc. */
 #endif /* _HAS_CXX17 */

#include <xkeycheck.h>	// _HAS_CXX17 must be defined before including this
#include <crtdefs.h>

#ifndef _STL_WARNING_LEVEL
 #if defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4
  #define _STL_WARNING_LEVEL	4
 #else /* defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4 */
  #define _STL_WARNING_LEVEL	3
 #endif /* defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4 */
#endif /* _STL_WARNING_LEVEL */

#if _STL_WARNING_LEVEL < 3
 #error _STL_WARNING_LEVEL cannot be less than 3.
#endif /* _STL_WARNING_LEVEL < 3 */

#if _STL_WARNING_LEVEL > 4
 #error _STL_WARNING_LEVEL cannot be greater than 4.
#endif /* _STL_WARNING_LEVEL > 4 */

#ifndef _STL_EXTRA_DISABLED_WARNINGS
 #define _STL_EXTRA_DISABLED_WARNINGS
#endif /* _STL_EXTRA_DISABLED_WARNINGS */

// warning C4702: unreachable code
#ifndef _STL_DISABLED_WARNINGS
 #define _STL_DISABLED_WARNINGS	4702 _STL_EXTRA_DISABLED_WARNINGS
#endif /* _STL_DISABLED_WARNINGS */

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,_STL_WARNING_LEVEL)
#pragma warning(disable: _STL_DISABLED_WARNINGS)
#pragma push_macro("new")
#undef new

#define _CPPLIB_VER	650

 #if _HAS_EXCEPTIONS
  #define _NOEXCEPT	noexcept
  #define _NOEXCEPT_OP(x)	noexcept(x)
 #else /* _HAS_EXCEPTIONS */
  #define _NOEXCEPT	throw ()
  #define _NOEXCEPT_OP(x)
 #endif /* _HAS_EXCEPTIONS */

 #ifndef _HAS_CONSTEXPR14
  #if defined(__cpp_constexpr)
   #if __cpp_constexpr >= 201304
    #define _HAS_CONSTEXPR14 1
   #else /* ^^^ __cpp_constexpr >= 201304 / __cpp_constexpr < 201304 vvv */
    #define _HAS_CONSTEXPR14 0
   #endif /* __cpp_constexpr >= 201304 */
  #elif defined(_MSC_FULL_VER)
   #if _MSC_FULL_VER >= 190024318
    #define _HAS_CONSTEXPR14 1
   #else /* ^^^ _MSC_FULL_VER >= 190024318 / _MSC_FULL_VER < 190024318 vvv */
    #define _HAS_CONSTEXPR14 0
   #endif /* _MSC_FULL_VER >= 190024318 */
  #elif __cplusplus >= 201402
   #define _HAS_CONSTEXPR14 1
  #else /* ^^^ __cplusplus >= 201402 / __cplusplus < 201402 vvv */
   #define _HAS_CONSTEXPR14 0
  #endif /* __cpp_constexpr */
 #endif /* _HAS_CONSTEXPR14 */

 #if _HAS_CONSTEXPR14
  #define _CONSTEXPR14 constexpr
 #else /* ^^^ has relaxed constexpr ^^^ / vvv no relaxed constexpr vvv */
  #define _CONSTEXPR14 inline
 #endif /* _HAS_CONSTEXPR14 */

 // C++17 constexpr additions implementable without C++14 relaxed constexpr
 #if _HAS_CXX17
  #define _CONSTEXPR17_11 constexpr
 #else /* ^^^ has C++17 constexpr additions ^^^ / vvv no C++17 constexpr additions vvv */
  #define _CONSTEXPR17_11 inline
 #endif /* _HAS_CXX17 */

 // C++17 constexpr additions implementable with C++14 relaxed constexpr
 #if _HAS_CXX17 && _HAS_CONSTEXPR14
  #define _CONSTEXPR17_14 constexpr
 #else /* ^^^ has C++17 constexpr additions ^^^ / vvv no C++17 constexpr additions vvv */
  #define _CONSTEXPR17_14 inline
 #endif /* _HAS_CXX17 && _HAS_CONSTEXPR14 */

// N4190 Removing auto_ptr, random_shuffle(), And Old <functional> Stuff
 #ifndef _HAS_AUTO_PTR_ETC
  #if _HAS_CXX17
   #define _HAS_AUTO_PTR_ETC	0
  #else /* _HAS_CXX17 */
   #define _HAS_AUTO_PTR_ETC	1
  #endif /* _HAS_CXX17 */
 #endif /* _HAS_AUTO_PTR_ETC */

// P0004R1 Removing Deprecated Iostreams Aliases
 #ifndef _HAS_OLD_IOSTREAMS_MEMBERS
  #if _HAS_CXX17
   #define _HAS_OLD_IOSTREAMS_MEMBERS	0
  #else /* _HAS_CXX17 */
   #define _HAS_OLD_IOSTREAMS_MEMBERS	1
  #endif /* _HAS_CXX17 */
 #endif /* _HAS_OLD_IOSTREAMS_MEMBERS */

// LWG 2385 function::assign allocator argument doesn't make sense
 #ifndef _HAS_FUNCTION_ASSIGN
  #if _HAS_CXX17
   #define _HAS_FUNCTION_ASSIGN	0
  #else /* _HAS_CXX17 */
   #define _HAS_FUNCTION_ASSIGN	1
  #endif /* _HAS_CXX17 */
 #endif /* _HAS_FUNCTION_ASSIGN */

// The non-Standard std::tr1 namespace and TR1-only machinery
 #ifndef _HAS_TR1_NAMESPACE
  #if _HAS_CXX17
   #define _HAS_TR1_NAMESPACE	0
  #else /* _HAS_CXX17 */
   #define _HAS_TR1_NAMESPACE	1
  #endif /* _HAS_CXX17 */
 #endif /* _HAS_TR1_NAMESPACE */

// The non-Standard std::identity struct
 #ifndef _HAS_IDENTITY_STRUCT
  #if _HAS_CXX17
   #define _HAS_IDENTITY_STRUCT	0
  #else /* _HAS_CXX17 */
   #define _HAS_IDENTITY_STRUCT	1
  #endif /* _HAS_CXX17 */
 #endif /* _HAS_IDENTITY_STRUCT */


 #ifdef _RTC_CONVERSION_CHECKS_ENABLED
  #ifndef _ALLOW_RTCc_IN_STL
static_assert(false, "/RTCc rejects conformant code, "
	"so it isn't supported by the C++ Standard Library. "
	"Either remove this compiler option, or define _ALLOW_RTCc_IN_STL "
	"to acknowledge that you have received this warning.");
  #endif /* _ALLOW_RTCc_IN_STL */
 #endif /* _RTC_CONVERSION_CHECKS_ENABLED */

/* Note on use of "deprecate":
 * Various places in this header and other headers use __declspec(deprecate) or macros that
 * have the term DEPRECATE in them. We use deprecate here ONLY to signal the compiler to
 * emit a warning about these items. The use of deprecate should NOT be taken to imply that
 * any standard committee has deprecated these functions from the relevant standards.
 * In fact, these functions are NOT deprecated from the standard.
 *
 * Full details can be found in our documentation by searching for "Checked Iterators".
*/

#if defined(MRTDLL) && defined(_CRTBLD)
/*
process-global is the default for code built with /clr or /clr:oldSyntax.
appdomain-global is the default for code built with /clr:pure.
Code in MSVCM is built with /clr, but is used by user code built with /clr:pure
so it must conform to the expectations of /clr:pure clients.
Use __PURE_APPDOMAIN_GLOBAL when a global needs to be appdomain-global for pure
clients and process-global for mixed clients.
*/
#define __PURE_APPDOMAIN_GLOBAL   __declspec(appdomain)
#else
#define __PURE_APPDOMAIN_GLOBAL
#endif

		/* CURRENT DLL NAMES */
#ifndef _CRT_MSVCP_CURRENT
	#ifdef _CRT_WINDOWS
		/* Windows */
		#ifdef _DEBUG
			#define _CRT_MSVCP_CURRENT "msvcpd_win.dll"
		#else
			#define _CRT_MSVCP_CURRENT "msvcp_win.dll"
		#endif
	#else
		/* Visual Studio */
		#ifdef _DEBUG
			#define _CRT_MSVCP_CURRENT "msvcp140d.dll"
		#else
			#define _CRT_MSVCP_CURRENT "msvcp140.dll"
		#endif
	#endif
#endif

#ifdef _ITERATOR_DEBUG_LEVEL /* A. _ITERATOR_DEBUG_LEVEL is already defined. */

	/* A1. Validate _ITERATOR_DEBUG_LEVEL. */
	#if _ITERATOR_DEBUG_LEVEL > 2 && defined(_DEBUG)
		#error _ITERATOR_DEBUG_LEVEL > 2 is not supported in debug mode.
	#elif _ITERATOR_DEBUG_LEVEL > 1 && !defined(_DEBUG)
		#error _ITERATOR_DEBUG_LEVEL > 1 is not supported in release mode.
	#endif

	/* A2. Inspect _HAS_ITERATOR_DEBUGGING. */
	#ifdef _HAS_ITERATOR_DEBUGGING /* A2i. _HAS_ITERATOR_DEBUGGING is already defined, validate it. */
		#if _ITERATOR_DEBUG_LEVEL == 2 && _HAS_ITERATOR_DEBUGGING != 1
			#error _ITERATOR_DEBUG_LEVEL == 2 must imply _HAS_ITERATOR_DEBUGGING == 1 .
		#elif _ITERATOR_DEBUG_LEVEL < 2 && _HAS_ITERATOR_DEBUGGING != 0
			#error _ITERATOR_DEBUG_LEVEL < 2 must imply _HAS_ITERATOR_DEBUGGING == 0 .
		#endif
	#else /* A2ii. _HAS_ITERATOR_DEBUGGING is not yet defined, derive it. */
		#if _ITERATOR_DEBUG_LEVEL == 2
			#define _HAS_ITERATOR_DEBUGGING 1
		#else
			#define _HAS_ITERATOR_DEBUGGING 0
		#endif
	#endif /* _HAS_ITERATOR_DEBUGGING */

	/* A3. Inspect _SECURE_SCL. */
	#ifdef _SECURE_SCL /* A3i. _SECURE_SCL is already defined, validate it. */
		#if _ITERATOR_DEBUG_LEVEL > 0 && _SECURE_SCL != 1
			#error _ITERATOR_DEBUG_LEVEL > 0 must imply _SECURE_SCL == 1 .
		#elif _ITERATOR_DEBUG_LEVEL == 0 && _SECURE_SCL != 0
			#error _ITERATOR_DEBUG_LEVEL == 0 must imply _SECURE_SCL == 0 .
		#endif
	#else /* A3ii. _SECURE_SCL is not yet defined, derive it. */
		#if _ITERATOR_DEBUG_LEVEL > 0
			#define _SECURE_SCL 1
		#else
			#define _SECURE_SCL 0
		#endif
	#endif /* _SECURE_SCL */

#else /* B. _ITERATOR_DEBUG_LEVEL is not yet defined. */

	/* B1. Inspect _HAS_ITERATOR_DEBUGGING. */
	#ifdef _HAS_ITERATOR_DEBUGGING /* B1i. _HAS_ITERATOR_DEBUGGING is already defined, validate it. */
		#if _HAS_ITERATOR_DEBUGGING > 1
			#error _HAS_ITERATOR_DEBUGGING must be either 0 or 1 .
		#elif _HAS_ITERATOR_DEBUGGING == 1 && !defined(_DEBUG)
			#error _HAS_ITERATOR_DEBUGGING == 1 is not supported in release mode.
		#endif
	#else /* B1ii. _HAS_ITERATOR_DEBUGGING is not yet defined, default it. */
		#ifdef _DEBUG
			#define _HAS_ITERATOR_DEBUGGING 1
		#else
			#define _HAS_ITERATOR_DEBUGGING 0
		#endif
	#endif /* _HAS_ITERATOR_DEBUGGING */

	/* B2. Inspect _SECURE_SCL. */
	#ifdef _SECURE_SCL /* B2i. _SECURE_SCL is already defined, validate it. */
		#if _SECURE_SCL > 1
			#error _SECURE_SCL must be either 0 or 1 .
		#endif
	#else /* B2ii. _SECURE_SCL is not yet defined, default it. */
		#if _HAS_ITERATOR_DEBUGGING == 1
			#define _SECURE_SCL 1
		#else
			#define _SECURE_SCL 0
		#endif
	#endif /* _SECURE_SCL */

	/* B3. Derive _ITERATOR_DEBUG_LEVEL. */
	#if _HAS_ITERATOR_DEBUGGING
		#define _ITERATOR_DEBUG_LEVEL 2
	#elif _SECURE_SCL
		#define _ITERATOR_DEBUG_LEVEL 1
	#else
		#define _ITERATOR_DEBUG_LEVEL 0
	#endif

#endif /* _ITERATOR_DEBUG_LEVEL */

#define _STRINGIZEX(x) #x
#define _STRINGIZE(x) _STRINGIZEX(x)

#ifdef __cplusplus
	#ifndef _ALLOW_MSC_VER_MISMATCH
		#pragma detect_mismatch("_MSC_VER", "1900")
	#endif /* _ALLOW_MSC_VER_MISMATCH */

	#ifndef _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
		#pragma detect_mismatch("_ITERATOR_DEBUG_LEVEL", _STRINGIZE(_ITERATOR_DEBUG_LEVEL))
	#endif /* _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH */

	#ifndef _ALLOW_RUNTIME_LIBRARY_MISMATCH
		#if !defined(_DLL) && !defined(_DEBUG)
			#pragma detect_mismatch("RuntimeLibrary", "MT_StaticRelease")
		#elif !defined(_DLL) && defined(_DEBUG)
			#pragma detect_mismatch("RuntimeLibrary", "MTd_StaticDebug")
		#elif defined(_DLL) && !defined(_DEBUG)
			#pragma detect_mismatch("RuntimeLibrary", "MD_DynamicRelease")
		#elif defined(_DLL) && defined(_DEBUG)
			#pragma detect_mismatch("RuntimeLibrary", "MDd_DynamicDebug")
		#endif /* defined(_DLL) etc. */
	#endif /* _ALLOW_RUNTIME_LIBRARY_MISMATCH */
#endif /* __cplusplus */

#ifdef _ITERATOR_DEBUG_ARRAY_OVERLOADS
	#if _ITERATOR_DEBUG_ARRAY_OVERLOADS != 0 && _ITERATOR_DEBUG_ARRAY_OVERLOADS != 1
		#error _ITERATOR_DEBUG_ARRAY_OVERLOADS must be either 0 or 1 .
	#elif _ITERATOR_DEBUG_LEVEL == 0 && _ITERATOR_DEBUG_ARRAY_OVERLOADS == 1
		#error _ITERATOR_DEBUG_LEVEL == 0 must imply _ITERATOR_DEBUG_ARRAY_OVERLOADS == 0 .
	#endif
#else /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */
	#if _ITERATOR_DEBUG_LEVEL == 0
		#define _ITERATOR_DEBUG_ARRAY_OVERLOADS 0
	#else
		#define _ITERATOR_DEBUG_ARRAY_OVERLOADS 1
	#endif
#endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

/* See note on use of deprecate at the top of this file */
#if !defined(_SCL_SECURE_NO_WARNINGS) && defined(_SCL_SECURE_NO_DEPRECATE)
#define _SCL_SECURE_NO_WARNINGS
#endif

#if !defined (_SECURE_SCL_DEPRECATE)
#if defined(_SCL_SECURE_NO_WARNINGS)
#define _SECURE_SCL_DEPRECATE 0
#else
#define _SECURE_SCL_DEPRECATE 1
#endif
#endif

/* _SECURE_SCL switches: helper macros */
/* See note on use of deprecate at the top of this file */

#if _ITERATOR_DEBUG_LEVEL > 0 && _SECURE_SCL_DEPRECATE
#define _SCL_INSECURE_DEPRECATE_FN(_Func) \
	_CRT_DEPRECATE_TEXT( \
		"Call to 'std::" #_Func "' with parameters that may be unsafe - " \
		"this call relies on the caller to check that the passed values are correct. " \
		"To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. " \
		"See documentation on how to use Visual C++ 'Checked Iterators'")
#else
#define _SCL_INSECURE_DEPRECATE_FN(_Func)
#endif

#ifndef _SCL_SECURE_INVALID_PARAMETER
 #define _SCL_SECURE_INVALID_PARAMETER(expr) _CRT_SECURE_INVALID_PARAMETER(expr)
#endif

 #define _SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT		_SCL_SECURE_INVALID_PARAMETER("invalid argument")
 #define _SCL_SECURE_OUT_OF_RANGE_NO_ASSERT			_SCL_SECURE_INVALID_PARAMETER("out of range")

 #define _SCL_SECURE_ALWAYS_VALIDATE(cond)				\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE(#cond && 0);						\
			_SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT;		\
		}												\
		_Analysis_assume_(cond);						\
	}

 #define _SCL_SECURE_CRT_VALIDATE(cond, retvalue)		\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE(#cond && 0);						\
			_SCL_SECURE_INVALID_PARAMETER(cond);		\
			return (retvalue);							\
		}												\
	}

 #if _ITERATOR_DEBUG_LEVEL > 0

 #define _SCL_SECURE_VALIDATE(cond)						\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE(#cond && 0);						\
			_SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT;		\
		}												\
		_Analysis_assume_(cond);						\
	}

 #define _SCL_SECURE_VALIDATE_RANGE(cond)				\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE(#cond && 0);						\
			_SCL_SECURE_OUT_OF_RANGE_NO_ASSERT;			\
		}												\
		_Analysis_assume_(cond);						\
	}

 #else /* _ITERATOR_DEBUG_LEVEL > 0 */

/* when users disable _SECURE_SCL to get performance, we don't want analysis warnings from SCL headers */
#if _ITERATOR_DEBUG_LEVEL == 2
 #define _SCL_SECURE_VALIDATE(cond)			_Analysis_assume_(cond)
 #define _SCL_SECURE_VALIDATE_RANGE(cond)	_Analysis_assume_(cond)
#else
 #define _SCL_SECURE_VALIDATE(cond)
 #define _SCL_SECURE_VALIDATE_RANGE(cond)
#endif

 #endif /* _ITERATOR_DEBUG_LEVEL > 0 */

#if __STDC_WANT_SECURE_LIB__
#define _CRT_SECURE_MEMCPY(dest, destsize, source, count) ::memcpy_s((dest), (destsize), (source), (count))
#define _CRT_SECURE_WMEMCPY(dest, destsize, source, count) ::wmemcpy_s((dest), (destsize), (source), (count))
#else
#define _CRT_SECURE_MEMCPY(dest, destsize, source, count) ::memcpy((dest), (source), (count))
#define _CRT_SECURE_WMEMCPY(dest, destsize, source, count) ::wmemcpy((dest), (source), (count))
#endif

#include <use_ansi.h>

#if defined(_M_CEE) && defined(_STATIC_CPPLIB)
#error _STATIC_CPPLIB is not supported while building with /clr or /clr:pure
#endif

#if defined(_DLL) && defined(_STATIC_CPPLIB) && !defined(_DISABLE_DEPRECATE_STATIC_CPPLIB)
#pragma _CRT_WARNING("_STATIC_CPPLIB is deprecated")
#endif

/* Define _CRTIMP2 */
 #ifndef _CRTIMP2
  #if defined(CRTDLL2) && defined(_CRTBLD)
   #define _CRTIMP2	__declspec(dllexport)
  #else   /* ndef CRTDLL2 && _CRTBLD */

   #if defined(_DLL) && !defined(_STATIC_CPPLIB)
    #define _CRTIMP2	__declspec(dllimport)

   #else   /* ndef _DLL && !STATIC_CPPLIB */
    #define _CRTIMP2
   #endif  /* _DLL && !STATIC_CPPLIB */

  #endif  /* CRTDLL2 && _CRTBLD */
 #endif  /* _CRTIMP2 */

/* Define _CRTIMP2_NCEEPURE */
 #ifndef _CRTIMP2_NCEEPURE
  #if defined(_M_CEE_PURE)
   #define _CRTIMP2_NCEEPURE
  #else
   #define _CRTIMP2_NCEEPURE _CRTIMP2
  #endif
 #endif

 #ifndef _MRTIMP2_PURE
  #if defined(_M_CEE_PURE)
   #define _MRTIMP2_PURE
  #else
   #define _MRTIMP2_PURE _MRTIMP2
  #endif
 #endif

 #ifndef _MRTIMP2_PURE_NPURE
  #if defined(_M_CEE_PURE)
   #define _MRTIMP2_PURE_NPURE
  #else
   #define _MRTIMP2_PURE_NPURE _MRTIMP2_NPURE
  #endif
 #endif

/* Define _MRTIMP2_NPURE */
 #ifndef _MRTIMP2_NPURE
  #if   defined(MRTDLL) && defined(_CRTBLD)
    #if !defined(_M_CEE_PURE)
      #define _MRTIMP2_NPURE __declspec(dllexport)
    #else
      #define _MRTIMP2_NPURE
    #endif
  #else   /* ndef MRTDLL && _CRTBLD */

   #if defined(_DLL) && defined(_M_CEE_PURE)
    #define _MRTIMP2_NPURE	__declspec(dllimport)

   #else
    #define _MRTIMP2_NPURE
   #endif

  #endif  /* MRTDLL && _CRTBLD */
 #endif  /* _MRTIMP2_NPURE */

 #if defined(_DLL) && !defined(_STATIC_CPPLIB) && !defined(_M_CEE_PURE)
  #define _DLL_CPPLIB
 #endif

 #ifndef _CRTIMP2_PURE
  #if   defined(MRTDLL) && defined(_CRTBLD)
   #define _CRTIMP2_PURE
  #else
   #ifdef  _M_CEE_PURE
     #define _CRTIMP2_PURE
   #else
     #define _CRTIMP2_PURE _CRTIMP2
   #endif
  #endif
 #endif

 #ifdef _CRTBLD
  #if !defined(_CRTDATA2)
   #if !defined(MRTDLL)
    #define _CRTDATA2 _CRTIMP2
   #else
    #define _CRTDATA2
   #endif
  #endif

/* These functions are for enabling STATIC_CPPLIB functionality */
  #define _cpp_stdin  (__acrt_iob_func(0))
  #define _cpp_stdout (__acrt_iob_func(1))
  #define _cpp_stderr (__acrt_iob_func(2))
  #define _cpp_isleadbyte(c) (__pctype_func()[(unsigned char)(c)] & _LEADBYTE)
 #else  /* _CRTBLD */
  #if !defined(_CRTDATA2)
   #define _CRTDATA2 _CRTIMP2
  #endif
 #endif  /* _CRTBLD */

		/* NAMESPACE */
#define _STD_BEGIN	namespace std {
#define _STD_END	}
#define _STD	::std::

/*
We use the stdext (standard extension) namespace to contain extensions that are not part of the current standard
*/
#define _STDEXT_BEGIN	namespace stdext {
#define _STDEXT_END		}
#define _STDEXT	        ::stdext::

#ifdef __cplusplus
 #define _CSTD	::

 #define _EXTERN_C			extern "C" {
 #define _END_EXTERN_C		}
#else /* ^^^ __cplusplus ^^^ // vvv !__cplusplus vvv */
 #define _CSTD

 #define _EXTERN_C
 #define _END_EXTERN_C
#endif /* __cplusplus */

		/* INTEGER PROPERTIES */
#define _MAX_EXP_DIG	8	/* for parsing numerics */
#define _MAX_INT_DIG	32
#define _MAX_SIG_DIG_V1	36
#define _MAX_SIG_DIG_V2	768

		/* STDIO PROPERTIES */
#define _Filet FILE

#define _IOBASE	_base
#define _IOPTR	_ptr
#define _IOCNT	_cnt

		/* MULTITHREAD PROPERTIES */
		/* LOCK MACROS */
#define _LOCK_LOCALE			0
#define _LOCK_MALLOC			1
#define _LOCK_STREAM			2
#define _LOCK_DEBUG				3
#define _LOCK_AT_THREAD_EXIT	4

 #ifdef __cplusplus
_STD_BEGIN
enum _Uninitialized
	{	// tag for suppressing initialization
	_Noinit
	};

		// CLASS _Lockit
class _CRTIMP2_PURE _Lockit
	{	// lock while object in existence -- MUST NEST
public:
 #if defined(_M_CEE_PURE) || defined(MRTDLL)
	__CLR_OR_THIS_CALL _Lockit()
		: _Locktype(0)
		{	// default construct
		_Lockit_ctor(this);
		}

	explicit __CLR_OR_THIS_CALL _Lockit(int _Kind)
		{	// set the lock
		_Lockit_ctor(this, _Kind);
		}

	__CLR_OR_THIS_CALL ~_Lockit() _NOEXCEPT
		{	// clear the lock
		_Lockit_dtor(this);
		}

 #else /* defined(_M_CEE_PURE) || defined(MRTDLL) */
	__thiscall _Lockit();	// default construct
	explicit __thiscall _Lockit(int);	// set the lock
	__thiscall ~_Lockit() _NOEXCEPT;	// clear the lock
 #endif /* defined(_M_CEE_PURE) || defined(MRTDLL) */

	static _MRTIMP2_NPURE void __cdecl _Lockit_ctor(int);
	static _MRTIMP2_NPURE void __cdecl _Lockit_dtor(int);

private:
	static _MRTIMP2_NPURE void __cdecl _Lockit_ctor(_Lockit *);
	static _MRTIMP2_NPURE void __cdecl _Lockit_ctor(_Lockit *, int);
	static _MRTIMP2_NPURE void __cdecl _Lockit_dtor(_Lockit *);

public:
	__CLR_OR_THIS_CALL _Lockit(const _Lockit&) = delete;
	_Lockit& __CLR_OR_THIS_CALL operator=(const _Lockit&) = delete;

private:
	int _Locktype;
	};

 #ifdef _M_CEE
class _CRTIMP2_PURE _EmptyLockit
	{	// empty lock class used for bin compat
private:
	int _Locktype;
	};

  #if defined(__cplusplus_cli)
   #define _M_CEE_FINALLY finally
  #else /* defined(__cplusplus_cli) */
   #define _M_CEE_FINALLY __finally
  #endif /* defined(__cplusplus_cli) */

  #define _BEGIN_LOCK(_Kind) \
	{ \
		typedef int _TmpTestType; \
		__if_exists(_TmpTestType::ToString) \
		{ \
		bool _MustReleaseLock = false; \
		int _LockKind = _Kind; \
		System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions(); \
		try \
		} \
		{ \
			__if_exists(_TmpTestType::ToString) \
			{ \
			System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions(); \
			try { } _M_CEE_FINALLY \
			{ \
				_STD _Lockit::_Lockit_ctor(_LockKind); \
				_MustReleaseLock = true; \
			} \
			} \
			__if_not_exists(_TmpTestType::ToString) \
			{ \
			_STD _Lockit _Lock(_Kind); \
			}

  #define _END_LOCK() \
		} \
		__if_exists(_TmpTestType::ToString) \
		{ \
		_M_CEE_FINALLY \
		{ \
			if (_MustReleaseLock) \
			{ \
				_STD _Lockit::_Lockit_dtor(_LockKind); \
			} \
		} \
		} \
	}

  #define _BEGIN_LOCINFO(_VarName) \
	_BEGIN_LOCK(_LOCK_LOCALE) \
	_Locinfo _VarName;

  #define _END_LOCINFO() \
	_END_LOCK()

  #define _RELIABILITY_CONTRACT \
	[System::Runtime::ConstrainedExecution::ReliabilityContract( \
		System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState, \
		System::Runtime::ConstrainedExecution::Cer::Success)]

 #else /* _M_CEE */
  #define _BEGIN_LOCK(_Kind) \
	{ \
		_STD _Lockit _Lock(_Kind);

  #define _END_LOCK() \
	}

  #define _BEGIN_LOCINFO(_VarName) \
	{ \
		_Locinfo _VarName;

  #define _END_LOCINFO() \
	}

  #define _RELIABILITY_CONTRACT
 #endif /* _M_CEE */

class _CRTIMP2_PURE _Init_locks
	{	// initialize mutexes
public:
 #if defined(_M_CEE_PURE) || defined(MRTDLL)
	__CLR_OR_THIS_CALL _Init_locks()
		{	// default construct
		_Init_locks_ctor(this);
		}

	__CLR_OR_THIS_CALL ~_Init_locks() _NOEXCEPT
		{	// destroy the object
		_Init_locks_dtor(this);
		}

 #else /* defined(_M_CEE_PURE) || defined(MRTDLL) */
	__thiscall _Init_locks();
	__thiscall ~_Init_locks() _NOEXCEPT;
 #endif /* defined(_M_CEE_PURE) || defined(MRTDLL) */

private:
	static _MRTIMP2_NPURE void __cdecl _Init_locks_ctor(_Init_locks *);
	static _MRTIMP2_NPURE void __cdecl _Init_locks_dtor(_Init_locks *);
	};

_STD_END
 #endif /* __cplusplus */

#ifndef _RELIABILITY_CONTRACT
 #define _RELIABILITY_CONTRACT
#endif /* _RELIABILITY_CONTRACT */

		/* MISCELLANEOUS MACROS AND TYPES */
_MRTIMP2 void __cdecl _Atexit(void (__cdecl *)(void));

typedef unsigned long _Uint32t;

#define _Mbstinit(x)	mbstate_t x = {}

 #pragma pop_macro("new")
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _YVALS */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
