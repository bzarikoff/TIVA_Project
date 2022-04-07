// ------------------------------------------------------------
//
// This file contains modifications to the C language.
// It is included in ".h" files and at the very top of every
// ".c" file.
//
// This version is intended to support the TI 28035, TI 280049,
// TI CLA, and non-TI 32-bit and 64-bit compilers.
//
// Alpha Technologies
// Burnaby B.C. Canada
// V5J 5M4
//
// Copyright 2016, 2021 Alpha Technologies Ltd. Reproduction or disclosure of
// the file or its contents without the prior written consent of
// Alpha Technologies is prohibited.
//
// ---------------------------------------------------------------

#ifndef AAAA_H
#define AAAA_H

#ifdef __TI_COMPILER_VERSION__




#define HWREG(x)        (*((_Atomic volatile uint32_t *)(x)))

typedef unsigned char       __uint8_t;
typedef __uint8_t       uint8;

    // TI CLA compiler
//    #ifdef __TMS320C28XX_CLA__
//        typedef          short  int16_t;
//        typedef unsigned short  uint16_t;
//        typedef          int    int32_t;
//        typedef unsigned int    uint32_t;
//        typedef unsigned int    size_t;     // CLA pointers are 16 bits
//    #else
        typedef          int    int16_t;
        typedef unsigned int    uint16_t;
        typedef          long   int32_t;
        typedef unsigned long   uint32_t;
        //typedef unsigned long   size_t;
//    #endif
#else   // non-TI compilers, 32 or 64 bits wide
#include <stddef.h>
#include <stdint.h>
    typedef int64_t int64;
    typedef uint64_t uint64;

#endif

// Define numeric types
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int32_t q16;
typedef int32_t q15;    // deprecated, switching to q16
typedef float float32;
typedef char char8or16;  // 16 bits for TI 28x, 8 bits for most others
typedef signed char int8or16;  // 16 bits for TI 28x, 8 bits for most others
typedef unsigned char uint8or16;  // 16 bits for TI 28x, 8 bits for most others
#ifndef bool
#define bool uint16_t
#endif

// Scope modifiers (see coding guidelines)
#define global
#define package
#define local static

// Standard constants and macros
#ifndef false
#define false   0
#endif
#ifndef true
#define true    1
#endif
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifdef __TMS320C28XX_CLA__
// TI CLA compiler
// CLA CLIB routines
extern float32 CLAcos(float32 fAngleRad);
extern float32 CLAisqrt(float32 fVal);
extern float32 CLAsin(float32 fAngleRad);
extern float32 CLAsqrt(float32 fVal);
#endif // TI CLA compiler
//test

// TI compiler pragmas
#ifdef __TI_COMPILER_VERSION__
#define __AAAA_PRAGMA__(x) _Pragma(#x)

//
// Place the following function, which must have the same name as the parameter,
// into RAM. (Note that inlining takes precedence so if a function absolutely
// needs to be in RAM, use DO_NOT_INLINE_FUNCTION as well.)
//
#define RAM_FUNCTION(funcname) __AAAA_PRAGMA__(CODE_SECTION(funcname, ".TI.ramfunc"))

//
// Do not allow the following function to be inlined. Rarely required.
//
#define DO_NOT_INLINE_FUNCTION(funcname) __AAAA_PRAGMA__(FUNC_CANNOT_INLINE(funcname))

//
// Make the following function, which must have the same name as the parameter,
// an interrupt routine (saving all registers).
//
#define INTERRUPT(funcname) __AAAA_PRAGMA__(INTERRUPT(funcname)) __AAAA_PRAGMA__(FUNC_CANNOT_INLINE(funcname))

//
// Control MISRA -- only to be used when no other option -- see project lead
//
#define MISRA_CONTROL(misracontrol) __AAAA_PRAGMA__(CHECK_MISRA(misracontrol))

//
// Function call which must be first line of an interrupt routine
//
#ifdef __TMS320C28XX_CLA__
#define INTERRUPT_ENTRY() __meallow()
#else
#define INTERRUPT_ENTRY() __eallow()
#endif

//
// CLA intrinsics.
//
#ifdef __TMS320C28XX_CLA__
#define FLOAT_MIN(value_1, value_2) __mminf32((value_1), (value_2))

#define FLOAT_MAX(value_1, value_2) __mmaxf32((value_1), (value_2))

// estimate inverse square root to about 0.4%
#define FLOAT_EISQRT(value) __meisqrtf32(value)

// estimate inverse to about 0.4%
#define FLOAT_EINV(value) __meinvf32(value)

#else
#define FLOAT_MIN(value_1, value_2) (((float32)(value_1) < (float32)(value_2)) ? (float32)(value_1) : (float32)(value_2))
#define FLOAT_MAX(value_1, value_2) (((float32)(value_1) > (float32)(value_2)) ? (float32)(value_1) : (float32)(value_2))
#endif

//
// Make the following function, which must have the same name as the parameter,
// as small as possible because it is only called at startup, not during real-time.
//
#define NOT_REAL_TIME(funcname) __AAAA_PRAGMA__(FUNCTION_OPTIONS(funcname, "-ms"))

//
// Place the following variable, which must have the same name as the parameter,
// into a specifc data section specified with a quoted string name. (This is
// typically used to access memory-mapped registers.)
//
#define DATA_SECTION(varname, sectionstring) __AAAA_PRAGMA__(DATA_SECTION(varname, sectionstring))

//
// Disable interrupts and return the old interrupt state and other ST1 states, especially
// register access allow (EALLOW), as a uint16.
// Other states include OBJMODE, AMODE, SPA, VMAP, PAGE0.
//
#define INTERRUPT_DISABLE() ((uint16)__disable_interrupts())

//
// Restore interrupts and other ST1 states, especially register access allow (EALLOW), to the
// state returned from INTERRUPT_DISABLE. Other states include OBJMODE, AMODE, SPA, VMAP, PAGE0.
//
#define INTERRUPT_RESTORE(statefromdisable) ((void)__restore_interrupts(statefromdisable))

//
// Wrap a 32-bit register name so that it will be accessed indivisibly as a 32-bit value.
// (e.g., for CAN registers) Otherwise, the compiler may optimize the access to
// be two 16-bit values.
//
#define REGISTER_32(regname) __byte_peripheral_32(&(regname))

//
// Store an 8-bit value at a specified octet index in an array with unknown element size (i.e. void *)
//
#define PACK_OCTET(arrayp, octetindex, val8)  do {__byte((void *)(arrayp),  (uint16)(octetindex)) = (uint16)(val8);} while(0)

//
// Fetch an 8-bit value from a specified octet index in an array with unknown element size (i.e. void *)
//
#define UNPACK_OCTET(arrayp, octetindex)  ((uint16)__byte((void *)(size_t)(arrayp), (uint16)(octetindex)))

//
// Multiply two Q16 numbers
//
#define Q16_MULTIPLY(factor1, factor2) __IQmpy((factor1), (factor2), 16)

//
// Adjust Q16 number to be between the minimum and maximum
//
#define Q16_MINMAX(value, min, max) __IQsat((value), (max), (min))

//
// Multiply two Q15 numbers -- deprecated, switching to Q16
//
#define Q15_MULTIPLY(factor1, factor2) __IQmpy((factor1), (factor2), 15)

//
// Adjust Q15 number to be between the minimum and maximum -- deprecated,
// switching to Q16
//
#define Q15_MINMAX(value, min, max) __IQsat((value), (max), (min))

//
//Take the absolute value of an int16.
//
#define INT16_ABSOLUTE_VALUE(value)    __abs16_sat(value)

//
// Misra code review
//
#pragma CHECK_MISRA("4.1") // No non-ISO escape sequences
#pragma CHECK_MISRA("4.2") // No trigraphs
#pragma CHECK_MISRA("5.1") // Identifiers less than 31 characters
#pragma CHECK_MISRA("5.2") // No hiding identifiers in inner scope
#pragma CHECK_MISRA("5.3") // A typedef name shall be a unique identifier
#pragma CHECK_MISRA("5.5") // No reusing static storage identifier
#pragma CHECK_MISRA("6.3")  // Replace basic numerical types
#pragma CHECK_MISRA("6.4,6.5") // Bitfield rules, but we're not using bitfields
#pragma CHECK_MISRA("7.1") // No octal constants
#pragma CHECK_MISRA("8.2") // No non-explicit object or function types
#pragma CHECK_MISRA("8.3") // Prototypes must match functions
#pragma CHECK_MISRA("8.4") // External objects must be compatible type
#pragma CHECK_MISRA("8.5") // No objects in headers
#pragma CHECK_MISRA("8.6") // No functions inside blocks
#pragma CHECK_MISRA("8.8") // Objects are declared in only one file
#pragma CHECK_MISRA("8.9") // Objects are defined (allocated) in only one place
#pragma CHECK_MISRA("8.11") // Use static, not extern, for declarations
#pragma CHECK_MISRA("9.1") // No uninitialized locals
#pragma CHECK_MISRA("9.2") // Braces in initializers must match structure
#pragma CHECK_MISRA("9.3") // Only first or all enum values can be initialized
#pragma CHECK_MISRA("10.2") // No poor implicit float conversion
#pragma CHECK_MISRA("10.6") // No unsigned constants without 'U'
#pragma CHECK_MISRA("11.1") // No converting pointer to function to non-int type
#pragma CHECK_MISRA("11.2") // No converting pointer to another non-int type
#pragma CHECK_MISRA("11.5") // No "cast"ing away pointer's const or volatile
#pragma CHECK_MISRA("12.1") // Use parentheses for precedence of binary ops
// #pragma CHECK_MISRA("12.2") // No ambiguous expression order evaluation  #### ti-cgt-c2000_20.2.0.LTS BUG DISABLED
#pragma CHECK_MISRA("12.3") // No using sizeof on non-objects
#pragma CHECK_MISRA("12.4") // No side effects right of || or &&
#pragma CHECK_MISRA("12.5") // Operands of || and && must be primary expressions
// #pragma CHECK_MISRA("12.6") // Operands of logical operators are effectively Boolean  #### ti-cgt-c2000_20.2.0.LTS BUG DISABLED
#pragma CHECK_MISRA("12.8") // Shifts are between 0 and n-1 bits
#pragma CHECK_MISRA("12.9") // No negating unsigned value
#pragma CHECK_MISRA("12.10") // No comma operator
#pragma CHECK_MISRA("12.11") // Constants must fit in value
#pragma CHECK_MISRA("13.1") // No assignments in boolean expressions
#pragma CHECK_MISRA("13.2") // Zero tests should be explicit
#pragma CHECK_MISRA("13.3") // No checking floats for equality
#pragma CHECK_MISRA("13.4") // No floats in for loop expression
#pragma CHECK_MISRA("13.5") // No weird for loop expressions
#pragma CHECK_MISRA("13.6") // No modification of for loop vars in body
#pragma CHECK_MISRA("13.7") // No invariant boolean operations
#pragma CHECK_MISRA("14.1") // No unreachable code
#pragma CHECK_MISRA("14.2") // No ineffectual statements
#pragma CHECK_MISRA("14.3") // Null statement is first on line
#pragma CHECK_MISRA("14.4") // No goto
#pragma CHECK_MISRA("15.0") // Simple switch syntax
#pragma CHECK_MISRA("15.1") // Switch labels at block scope
#pragma CHECK_MISRA("15.2") // No dropping through cases without break
#pragma CHECK_MISRA("15.4") // No switch on boolean expression
#pragma CHECK_MISRA("15.5") // Switch must contain at least one case
#pragma CHECK_MISRA("16.1") // No function varargs
#pragma CHECK_MISRA("16.3") // Function prototype must contain identifiers
#pragma CHECK_MISRA("16.4") // Prototype identifiers must match function
#pragma CHECK_MISRA("16.5") // Empty function parameter list must contain "void"
#pragma CHECK_MISRA("16.6") // No calling functions with a different number of args
#pragma CHECK_MISRA("16.7") // Use pointer to const in function params if not modified
#pragma CHECK_MISRA("16.8") // No paths that don't return value in value functions
#pragma CHECK_MISRA("16.9") // No using function names as values (without &)
#pragma CHECK_MISRA("17.5") // No more than two level of indirection
#pragma CHECK_MISRA("18.1") // Struct and unions must be complete at end
#pragma CHECK_MISRA("19.1") // Includes must be at top
#pragma CHECK_MISRA("19.2") // No weird characters in include file names
#pragma CHECK_MISRA("19.3") // Include needs file name
#pragma CHECK_MISRA("19.5") // #defines only at file level
#pragma CHECK_MISRA("19.6") // #undef not allowed
#pragma CHECK_MISRA("19.8") // function-like macros must have all parameters in "call"
#pragma CHECK_MISRA("19.9") // No preprocessor directives in macro arguments
#pragma CHECK_MISRA("19.10") // In #define, argument usage must be parenthesized
#pragma CHECK_MISRA("19.11") // No undefined macros, except in #ifndef and defined()
#pragma CHECK_MISRA("19.13") // No # and ## in macro definitions
#pragma CHECK_MISRA("19.14") // No use of defined() on expressions
#pragma CHECK_MISRA("19.15") // Include check for multiple inclusion of header file
#pragma CHECK_MISRA("19.16") // Preprocessor syntax must always be valid
#pragma CHECK_MISRA("19.17") // No continuing #ifs across files
#pragma CHECK_MISRA("20.2") // No reuse of standard library names
#pragma CHECK_MISRA("20.4") // No heap allocation
#pragma CHECK_MISRA("20.6") // No offsetof
#pragma CHECK_MISRA("20.7") // No setjmp
#pragma CHECK_MISRA("20.8") // No signal.h
#pragma CHECK_MISRA("20.9") // No stido.h in production code
#pragma CHECK_MISRA("20.10") // No atof, etc.
#pragma CHECK_MISRA("20.11") // No abort, exit, etc.
#pragma CHECK_MISRA("20.12") // No time.h

//
// Disallowed types and intrinsics
//
#define char char_type_is_not_allowed // because bytes are 16 bits in a TMS320F28035
#define int int_type_is_not_allowed // because int length varies between compilers
#define long long_type_is_not_allowed // because long length varies between compilers
#define float float_type_is_not_allowed // because MISRA complains about float, use float32
#define double double_type_is_not_allowed // because doubles are single precision
#define typedef typedef_is_not_allowed // typedef is not allowed for production code
#define asm asm_is_not_allowed // using asm is not allowed for production code
#define __asm asm_is_not_allowed // using asm is not allowed for production code

#else  // not TI compiler

//
// Place the following function, which must have the same name as the parameter,
// into RAM.
//
#define RAM_FUNCTION(funcname)

//
// Do not allow the following function to be inlined. Rarely required.
//
#define DO_NOT_INLINE_FUNCTION(funcname)

//
// Make the following function, which must have the same name as the parameter,
// an interrupt routine (saving all registers).
//
#define INTERRUPT(funcname)

//
// Control MISRA -- only to be used when no other option -- see project lead
//
#define MISRA_CONTROL(misracontrol)

//
// Function call which must be first line of an interrupt routine
//
#define INTERRUPT_ENTRY()

//
// Make the following function, which must have the same name as the parameter,
// as small as possible because it is only called at startup, not during real-time.
//
#define NOT_REAL_TIME(funcname)

//
// Disable interrupts and return the old interrupt state and other ST1 states, especially
// register access allow (EALLOW), as a uint16.
// Other states include OBJMODE, AMODE, SPA, VMAP, PAGE0.
//
#define INTERRUPT_DISABLE()  ((uint16)0u)

//
// Restore interrupts and other ST1 states, especially register access allow (EALLOW), to the
// state returned from INTERRUPT_DISABLE. Other states include OBJMODE, AMODE, SPA, VMAP, PAGE0.
//
#define INTERRUPT_RESTORE(statefromdisable)

//
// Wrap a 32-bit register name so that it will be accessed indivisibly as a 32-bit value.
// (e.g., for CAN registers) Otherwise, the compiler may optimize the access to
// be two 16-bit values.
//
#define REGISTER_32(regname) (regname)

//
// Store an 8-bit value at a specified octet index in an array with unknown element size (i.e. void *)
//
typedef unsigned char __octet;
#define PACK_OCTET(arrayp, octetindex, val8)  do {((__octet *)(arrayp))[octetindex] = (__octet)(val8);} while(0)

//
// Fetch an 8-bit value from a specified octet index in an array with unknown element size (i.e. void *)
//
#define UNPACK_OCTET(arrayp, octetindex)  ((__octet)((const __octet *)(arrayp))[octetindex])

//
// Place the following variable, which must have the same name as the parameter,
// into a specifc data section specified with a quoted string name.
//
#define DATA_SECTION(varname, sectionstring)


//
// Multiply two Q16 numbers -- deprecated, switching to Q16
//
#define Q16_MULTIPLY(factor1, factor2) ((int32)(((int64)(factor1) * (int64)(factor2)) / 65536L))

//
// Adjust Q16 number to be between the minimum and maximum
//
#define Q16_MINMAX(value, min, max)  ((value) > (max) ? (max) : ((value) < (min) ? (min) : (value)))

//
// Multiply two Q15 numbers
//
#define Q15_MULTIPLY(factor1, factor2) ((int32)(((int64)(factor1) * (int64)(factor2)) / 32768L))

//
// Adjust Q15 number to be between the minimum and maximum -- deprecated, switching to Q16
//
#define Q15_MINMAX(value, min, max)  ((value) > (max) ? (max) : ((value) < (min) ? (min) : (value)))

//
//Take the absolute value of an int16.
//
#define INT16_ABSOLUTE_VALUE(value)    ((value) > 0 ? (value) : ((value) * -1))

#define FLOAT_MIN(value_1, value_2) (((float32)(value_1) < (float32)(value_2)) ? (float32)(value_1) : (float32)(value_2))

#define FLOAT_MAX(value_1, value_2) (((float32)(value_1) > (float32)(value_2)) ? (float32)(value_1) : (float32)(value_2))

// estimate inverse square root to about .4%
#define FLOAT_EISQRT(value) ((float32)0.996/(float32)sqrt((float32)(value)))

// estimate inverse  to about .4%
#define FLOAT_EINV(value) ((float32)0.996/(float32)(value))


#endif  // not TI compiler
#endif // AAAA_H
