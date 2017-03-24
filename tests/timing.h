/*
 * This file is borrowed from the KeccakCodePackage and adapted slightly to work
 * with 64-bit processors and avoid the measurement macro to finish the
 * execution of the function using it.
 *
 * The original code belongs to Doug Whiting, who shared it in hash-forum at
 * nist.gov.
 *
 *
-------- Forwarded Message --------
Subject:    RE: Time Trouble
Date:   Mon, 28 Jul 2008 08:07:47 -0400
From:   Doug Whiting <DWhiting at hifn.com>
Reply-To:   hash-forum at nist.gov
To:     Multiple recipients of list <hash-forum at nist.gov>

Sorry for the earlier empty email. I pushed send by mistake while starting my message.

Yes, it's a real shame that C doesn't have a standard way to do this. Below is some code that you are free to copy if you wish I have used variants of this function for years, all the way back to AES days, and the code is entirely mine, so I hereby release it to the public domain. If you keep reading below, I also give some concrete suggestions on how to use it.

This code works on x86 family CPUs (32-big and 64-bit), under MSVC, gcc, and BorlandC, including older compiler versions where the __rdtsc() function is not defined. It also checks for ANSI compiles (i.e., -ansi using gcc, /Za using MSVC, and -A using Borland) and disables the call, to avoid compile-time warnings/errors. The function HiResTime() currently returns only 32 bits, mostly for historical reasons. However, that's enough to do most timing measurements, and you could easily enhance it to return 64 bits if desired. I normally compile with multiple compilers -- e.g., three versions of MSVC (v4.2, v6.0 and v9.0), at least two versions of gcc, plus Borland -- and take performance measurements on all of them.

[…]

*/

#include <stdint.h>
#include <stdio.h>

/************** Timing routine (for performance measurements) ***********/
/* By Doug Whiting */
/* unfortunately, this is generally assembly code and not very portable */
#if defined(_M_IX86) || defined(__i386) || defined(_i386) || defined(__i386__) || defined(i386) || \
    defined(_X86_)   || defined(__x86_64__) || defined(_M_X64) || defined(__x86_64)
#define _Is_X86_    1
#endif

#if  defined(_Is_X86_) && (!defined(__STRICT_ANSI__)) && (defined(__GNUC__) || !defined(__STDC__)) && \
    (defined(__BORLANDC__) || defined(_MSC_VER) || defined(__MINGW_H) || defined(__GNUC__))
#define HI_RES_CLK_OK         1          /* it's ok to use RDTSC opcode */

#if defined(_MSC_VER) /* && defined(_M_X64) */
#include <intrin.h>
#pragma intrinsic(__rdtsc)         /* use MSVC rdtsc call where defined */
#endif

#endif

/* return the current value of time stamp counter */
static uint64_t HiResTime(void) {
#if defined(HI_RES_CLK_OK)
    uint64_t x[2];
#if   defined(__BORLANDC__)
#define COMPILER_ID "BCC"
    __emit__(0x0F,0x31); /* RDTSC instruction */
    _asm { mov x[0],eax };
#elif defined(_MSC_VER)
#define COMPILER_ID "MSC"
#if defined(_MSC_VER) /* && defined(_M_X64) */
    x[0] = (uint64_t) __rdtsc();
#else
    _asm { _emit 0fh }; _asm { _emit 031h };
    _asm { mov x[0],eax };
#endif
#elif defined(__MINGW_H) || defined(__GNUC__)
#define COMPILER_ID "GCC"
    /*
     * The time stamp counter is contained in a 64-bit MSR. The high-order 32
     * bits of the MSR are loaded into the EDX register, and the low-order 32
     * bits are loaded into the EAX register.
     */
    __asm__ __volatile__("rdtsc" : "=a"(x[0]), "=d"(x[1]));
#else
#error  "HI_RES_CLK_OK -- but no assembler code for this platform (?)"
#endif
    /*
     * Modified for 64-bit architectures, might need tweaking if run on
     * 32-bit processors.
     */
    return (x[0] | (x[1] << 32));
#else
    /* avoid annoying MSVC 9.0 compiler warning #4720 in ANSI mode! */
#if (!defined(_MSC_VER)) || (!defined(__STDC__)) || (_MSC_VER < 1300)
    FatalError("No support for RDTSC on this CPU platform\n");
#endif
    return 0;
#endif /* defined(HI_RES_CLK_OK) */
    }

#define TIMER_SAMPLE_CNT (10)

static uint64_t calibrate()
{
    /* adapted to 64-bit */
    uint64_t dtMin = 0xFFFFFFFFFFFFFFFF; /* big number to start */
    uint64_t t0,t1,i;

    for (i=0;i < TIMER_SAMPLE_CNT;i++) /* calibrate the overhead for measuring time */
        {
        t0 = HiResTime();
        t1 = HiResTime();
        if (dtMin > t1-t0) /* keep only the minimum time */
            dtMin = t1-t0;
        }
    return dtMin;
}

#define measureTimingBegin \
    /* adapted to 64-bit */ \
    uint64_t tMin = 0xFFFFFFFFFFFFFFFF; \
    uint64_t t0,t1,i; \
    for (i=0;i < TIMER_SAMPLE_CNT;i++) \
        { \
        t0 = HiResTime();

#define measureTimingEnd \
        t1 = HiResTime(); \
        if (tMin > t1-t0 - dtMin) \
            tMin = t1-t0 - dtMin; \
    }
