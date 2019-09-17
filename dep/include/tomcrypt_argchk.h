/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 */

/* Defines the LTC_ARGCHK macro used within the library */
/* ARGTYPE is defined in tomcrypt_cfg.h */
#if ARGTYPE == 0

#include <signal.h>

/* this is the default LibTomCrypt macro  */
#if defined(__clang__) || defined(__GNUC_MINOR__)
#define NORETURN __attribute__ ((noreturn))
#else
#define NORETURN
#endif

void crypt_argchk(char *v, char *s, int d) NORETURN;
#define LTC_ARGCHK(x) do { if (!(x)) { crypt_argchk(#x, __FILE__, __LINE__); } }while(0)
#define LTC_ARGCHKVD(x) do { if (!(x)) { crypt_argchk(#x, __FILE__, __LINE__); } }while(0)

#elif ARGTYPE == 1

/* fatal type of error */
#define LTC_ARGCHK(x) assert((x))
#define LTC_ARGCHKVD(x) LTC_ARGCHK(x)

#elif ARGTYPE == 2

#define LTC_ARGCHK(x) if (!(x)) { fprintf(stderr, "\nwarning: ARGCHK failed at %s:%d\n", __FILE__, __LINE__); }
#define LTC_ARGCHKVD(x) LTC_ARGCHK(x)

#elif ARGTYPE == 3

#define LTC_ARGCHK(x)
#define LTC_ARGCHKVD(x) LTC_ARGCHK(x)

#elif ARGTYPE == 4

#define LTC_ARGCHK(x)   if (!(x)) return CRYPT_INVALID_ARG;
#define LTC_ARGCHKVD(x) if (!(x)) return;

#endif


/* ref:         tag: v1.18.0-rc3, release/1.18.0 */
/* git commit:  041b7aa5c0036ec0dabc7e6f47a3548387b365f1 */
/* commit time: 2017-08-23 22:35:35 +0200 */
