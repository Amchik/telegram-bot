/* VIM BEST EDITOR
 * vim: ft=c tabstop=2 shiftwidth=2
 *
 * matestr
 *
 * For more info check matestr.c (first comment)
 */
#ifndef __8zc1kg_matestr_h
#define __8zc1kg_matestr_h

#include <sys/types.h>

typedef char matechar;
#define __matecharlen strlen

/*
 * Mate string. Do not modify this struct
 */
typedef struct {
  /*
   * C-string instance
   */
  matechar* cstr;
  /*
   * Length of string
   */
  u_int32_t length;

  /*
   * Allocated bytes for the string
   */
  u_int32_t __allocated;
} matestr;

/*
 * Creates new instance of matestr.
 */
matestr matestralloc(size_t n);

/*
 * Creates new instance of matestr.
 * Calculates length of cstr, if cstr == 0,
 * passes empty allocated string
 * If len == 0 it will be ignored
 */
matestr matestr_new(const matechar* cstr, size_t len);
/*
 * Normalizes cstr in matestr
 */
void    matestr_normalize(matestr* self);
/*
 * Optimizes memory in matestr
 */
void    matestr_optimize(matestr* self);
/*
 * Copy string to matestr
 * If len == 0 it will be ignored
 */
void    matestr_append(matestr* self, const matechar* cstr, size_t len);
/*
 * Allocs matestr for n bytes
 */
void    matestr_appendm(matestr* self, size_t n);

/*
 * Replaces char in string
 */
void matestr_replacec(matestr* self, matechar oldc, matechar newc);
/*
 * Find index of char in string. Return (u_int32_t)-1 if fail
 */
u_int32_t matestr_find(matestr* self, matechar what, u_int32_t start);
/*
 * Find index of string in string. Return (u_int32_t)-1 if fail
 */
u_int32_t matestr_finds(matestr* self, matechar *what, u_int32_t start);

/*
 * Takes substring (creates new string).
 * Returns zero-allocated string if failed
 */
matestr matestr_take(matestr *self, u_int32_t start, u_int32_t end);

#endif
