#pragma once

#if DEBUG
#ifndef _DEBUG
#define _DEBUG  1
#endif // !_DEBUG
#elif _DEBUG
#ifndef DEBUG
#define DEBUG   1
#endif // !DEBUG
#elif NDEBUG
#ifndef _NDEBUG
#define _NDEBUG 1
#endif // !_NDEBUG
#elif _NDEBUG
#ifndef NDEBUG
#define NDEBUG  1
#endif // !NDEBUG
#else
#error "Configuration (DEBUG/NDEBUG) is not specified!"
#endif


#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(p)       ((void)(p))
#endif


#ifndef __in
#define __in
#endif // !__in

#ifndef __out
#define __out
#endif // !__out

#ifndef __inout
#define __inout
#endif // !__inout

#ifndef __in_opt
#define __in_opt
#endif // !__in_opt

#ifndef __out_opt
#define __out_opt
#endif // !__out_opt

#ifndef __inout_opt
#define __inout_opt
#endif // !__inout_opt


#if (defined _MSC_VER && !defined __INTEL_COMPILER)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif // _MSC_VER && !__INTEL_COMPILER

#if (defined _MSC_VER)
#define __thread            __declspec(thread)
#endif // _MSC_VER && !__INTEL_COMPILER

#if (!defined _MSC_VER && !defined __INTEL_COMPILER)
#define __assume(expr)
#endif // !_MSC_VER && !__INTEL_COMPILER


typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
