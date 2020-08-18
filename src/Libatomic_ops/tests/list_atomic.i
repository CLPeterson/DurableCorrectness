# 1 "list_atomic.c"
# 1 "/home/sdp/christina/durablecorrectness/src/libatomic_ops/tests//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "list_atomic.c"
# 1 "../src/atomic_ops.h" 1
# 27 "../src/atomic_ops.h"
# 1 "../src/atomic_ops/ao_version.h" 1
# 28 "../src/atomic_ops.h" 2



# 1 "/usr/include/assert.h" 1 3 4
# 35 "/usr/include/assert.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 424 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 1 3 4
# 442 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 443 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/long-double.h" 1 3 4
# 444 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 425 "/usr/include/features.h" 2 3 4
# 448 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs-64.h" 1 3 4
# 11 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 2 3 4
# 449 "/usr/include/features.h" 2 3 4
# 36 "/usr/include/assert.h" 2 3 4
# 66 "/usr/include/assert.h" 3 4




# 69 "/usr/include/assert.h" 3 4
extern void __assert_fail (const char *__assertion, const char *__file,
      unsigned int __line, const char *__function)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));


extern void __assert_perror_fail (int __errnum, const char *__file,
      unsigned int __line, const char *__function)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));




extern void __assert (const char *__assertion, const char *__file, int __line)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));



# 32 "../src/atomic_ops.h" 2
# 1 "/usr/lib/gcc/x86_64-linux-gnu/8/include/stddef.h" 1 3 4
# 149 "/usr/lib/gcc/x86_64-linux-gnu/8/include/stddef.h" 3 4
typedef long int ptrdiff_t;
# 216 "/usr/lib/gcc/x86_64-linux-gnu/8/include/stddef.h" 3 4
typedef long unsigned int size_t;
# 328 "/usr/lib/gcc/x86_64-linux-gnu/8/include/stddef.h" 3 4
typedef int wchar_t;
# 426 "/usr/lib/gcc/x86_64-linux-gnu/8/include/stddef.h" 3 4
typedef struct {
  long long __max_align_ll __attribute__((__aligned__(__alignof__(long long))));
  long double __max_align_ld __attribute__((__aligned__(__alignof__(long double))));
# 437 "/usr/lib/gcc/x86_64-linux-gnu/8/include/stddef.h" 3 4
} max_align_t;
# 33 "../src/atomic_ops.h" 2
# 294 "../src/atomic_ops.h"
# 1 "../src/atomic_ops/sysdeps/gcc/x86.h" 1
# 649 "../src/atomic_ops/sysdeps/gcc/x86.h"
# 1 "../src/atomic_ops/sysdeps/gcc/generic.h" 1
# 23 "../src/atomic_ops/sysdeps/gcc/generic.h"
# 1 "../src/atomic_ops/sysdeps/gcc/../test_and_set_t_is_char.h" 1
# 38 "../src/atomic_ops/sysdeps/gcc/../test_and_set_t_is_char.h"

# 38 "../src/atomic_ops/sysdeps/gcc/../test_and_set_t_is_char.h"
typedef enum {
  AO_BYTE_TS_clear = 0,
  AO_BYTE_TS_set = 1
} AO_BYTE_TS_val;
# 24 "../src/atomic_ops/sysdeps/gcc/generic.h" 2
# 62 "../src/atomic_ops/sysdeps/gcc/generic.h"
  static __inline void
  AO_nop_read(void)
  {
    __atomic_thread_fence(2);
  }



    static __inline void
    AO_nop_write(void)
    {
      __atomic_thread_fence(3);
    }



  static __inline void
  AO_nop_full(void)
  {

    __atomic_thread_fence(5);
  }



# 1 "../src/atomic_ops/sysdeps/gcc/generic-small.h" 1
# 20 "../src/atomic_ops/sysdeps/gcc/generic-small.h"
static __inline unsigned char
AO_char_load(const volatile unsigned char *addr)
{
  return __atomic_load_n(addr, 0);
}


static __inline unsigned char
AO_char_load_acquire(const volatile unsigned char *addr)
{
  return __atomic_load_n(addr, 2);
}
# 50 "../src/atomic_ops/sysdeps/gcc/generic-small.h"
  static __inline void
  AO_char_store(volatile unsigned char *addr, unsigned char value)
  {
    __atomic_store_n(addr, value, 0);
  }




  static __inline void
  AO_char_store_release(volatile unsigned char *addr, unsigned char value)
  {
    __atomic_store_n(addr, value, 3);
  }







  static __inline unsigned char
  AO_char_fetch_compare_and_swap(volatile unsigned char *addr,
                                  unsigned char old_val, unsigned char new_val)
  {
    (void)__atomic_compare_exchange_n(addr,
                                      &old_val ,
                                      new_val ,
                                      0 ,
                                      0 ,
                                      0 );
    return old_val;
  }


  static __inline unsigned char
  AO_char_fetch_compare_and_swap_acquire(volatile unsigned char *addr,
                                          unsigned char old_val, unsigned char new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      2, 2);
    return old_val;
  }


  static __inline unsigned char
  AO_char_fetch_compare_and_swap_release(volatile unsigned char *addr,
                                          unsigned char old_val, unsigned char new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      3,
                                      0 );
    return old_val;
  }


  static __inline unsigned char
  AO_char_fetch_compare_and_swap_full(volatile unsigned char *addr,
                                       unsigned char old_val, unsigned char new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      4,
                                      2 );
    return old_val;
  }



    static __inline int
    AO_char_compare_and_swap(volatile unsigned char *addr,
                              unsigned char old_val, unsigned char new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                        0, 0);
    }


    static __inline int
    AO_char_compare_and_swap_acquire(volatile unsigned char *addr,
                                      unsigned char old_val, unsigned char new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                        2, 2);
    }


    static __inline int
    AO_char_compare_and_swap_release(volatile unsigned char *addr,
                                      unsigned char old_val, unsigned char new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                              3,
                                              0 );
    }


    static __inline int
    AO_char_compare_and_swap_full(volatile unsigned char *addr,
                                   unsigned char old_val, unsigned char new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                              4,
                                              2 );
    }
# 178 "../src/atomic_ops/sysdeps/gcc/generic-small.h"
static __inline unsigned short
AO_short_load(const volatile unsigned short *addr)
{
  return __atomic_load_n(addr, 0);
}


static __inline unsigned short
AO_short_load_acquire(const volatile unsigned short *addr)
{
  return __atomic_load_n(addr, 2);
}
# 208 "../src/atomic_ops/sysdeps/gcc/generic-small.h"
  static __inline void
  AO_short_store(volatile unsigned short *addr, unsigned short value)
  {
    __atomic_store_n(addr, value, 0);
  }




  static __inline void
  AO_short_store_release(volatile unsigned short *addr, unsigned short value)
  {
    __atomic_store_n(addr, value, 3);
  }







  static __inline unsigned short
  AO_short_fetch_compare_and_swap(volatile unsigned short *addr,
                                  unsigned short old_val, unsigned short new_val)
  {
    (void)__atomic_compare_exchange_n(addr,
                                      &old_val ,
                                      new_val ,
                                      0 ,
                                      0 ,
                                      0 );
    return old_val;
  }


  static __inline unsigned short
  AO_short_fetch_compare_and_swap_acquire(volatile unsigned short *addr,
                                          unsigned short old_val, unsigned short new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      2, 2);
    return old_val;
  }


  static __inline unsigned short
  AO_short_fetch_compare_and_swap_release(volatile unsigned short *addr,
                                          unsigned short old_val, unsigned short new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      3,
                                      0 );
    return old_val;
  }


  static __inline unsigned short
  AO_short_fetch_compare_and_swap_full(volatile unsigned short *addr,
                                       unsigned short old_val, unsigned short new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      4,
                                      2 );
    return old_val;
  }



    static __inline int
    AO_short_compare_and_swap(volatile unsigned short *addr,
                              unsigned short old_val, unsigned short new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                        0, 0);
    }


    static __inline int
    AO_short_compare_and_swap_acquire(volatile unsigned short *addr,
                                      unsigned short old_val, unsigned short new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                        2, 2);
    }


    static __inline int
    AO_short_compare_and_swap_release(volatile unsigned short *addr,
                                      unsigned short old_val, unsigned short new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                              3,
                                              0 );
    }


    static __inline int
    AO_short_compare_and_swap_full(volatile unsigned short *addr,
                                   unsigned short old_val, unsigned short new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                              4,
                                              2 );
    }
# 336 "../src/atomic_ops/sysdeps/gcc/generic-small.h"
static __inline unsigned
AO_int_load(const volatile unsigned *addr)
{
  return __atomic_load_n(addr, 0);
}


static __inline unsigned
AO_int_load_acquire(const volatile unsigned *addr)
{
  return __atomic_load_n(addr, 2);
}
# 366 "../src/atomic_ops/sysdeps/gcc/generic-small.h"
  static __inline void
  AO_int_store(volatile unsigned *addr, unsigned value)
  {
    __atomic_store_n(addr, value, 0);
  }




  static __inline void
  AO_int_store_release(volatile unsigned *addr, unsigned value)
  {
    __atomic_store_n(addr, value, 3);
  }







  static __inline unsigned
  AO_int_fetch_compare_and_swap(volatile unsigned *addr,
                                  unsigned old_val, unsigned new_val)
  {
    (void)__atomic_compare_exchange_n(addr,
                                      &old_val ,
                                      new_val ,
                                      0 ,
                                      0 ,
                                      0 );
    return old_val;
  }


  static __inline unsigned
  AO_int_fetch_compare_and_swap_acquire(volatile unsigned *addr,
                                          unsigned old_val, unsigned new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      2, 2);
    return old_val;
  }


  static __inline unsigned
  AO_int_fetch_compare_and_swap_release(volatile unsigned *addr,
                                          unsigned old_val, unsigned new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      3,
                                      0 );
    return old_val;
  }


  static __inline unsigned
  AO_int_fetch_compare_and_swap_full(volatile unsigned *addr,
                                       unsigned old_val, unsigned new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      4,
                                      2 );
    return old_val;
  }



    static __inline int
    AO_int_compare_and_swap(volatile unsigned *addr,
                              unsigned old_val, unsigned new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                        0, 0);
    }


    static __inline int
    AO_int_compare_and_swap_acquire(volatile unsigned *addr,
                                      unsigned old_val, unsigned new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                        2, 2);
    }


    static __inline int
    AO_int_compare_and_swap_release(volatile unsigned *addr,
                                      unsigned old_val, unsigned new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                              3,
                                              0 );
    }


    static __inline int
    AO_int_compare_and_swap_full(volatile unsigned *addr,
                                   unsigned old_val, unsigned new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                              4,
                                              2 );
    }
# 494 "../src/atomic_ops/sysdeps/gcc/generic-small.h"
static __inline size_t
AO_load(const volatile size_t *addr)
{
  return __atomic_load_n(addr, 0);
}


static __inline size_t
AO_load_acquire(const volatile size_t *addr)
{
  return __atomic_load_n(addr, 2);
}
# 524 "../src/atomic_ops/sysdeps/gcc/generic-small.h"
  static __inline void
  AO_store(volatile size_t *addr, size_t value)
  {
    __atomic_store_n(addr, value, 0);
  }




  static __inline void
  AO_store_release(volatile size_t *addr, size_t value)
  {
    __atomic_store_n(addr, value, 3);
  }







  static __inline size_t
  AO_fetch_compare_and_swap(volatile size_t *addr,
                                  size_t old_val, size_t new_val)
  {
    (void)__atomic_compare_exchange_n(addr,
                                      &old_val ,
                                      new_val ,
                                      0 ,
                                      0 ,
                                      0 );
    return old_val;
  }


  static __inline size_t
  AO_fetch_compare_and_swap_acquire(volatile size_t *addr,
                                          size_t old_val, size_t new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      2, 2);
    return old_val;
  }


  static __inline size_t
  AO_fetch_compare_and_swap_release(volatile size_t *addr,
                                          size_t old_val, size_t new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      3,
                                      0 );
    return old_val;
  }


  static __inline size_t
  AO_fetch_compare_and_swap_full(volatile size_t *addr,
                                       size_t old_val, size_t new_val)
  {
    (void)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                      4,
                                      2 );
    return old_val;
  }



    static __inline int
    AO_compare_and_swap(volatile size_t *addr,
                              size_t old_val, size_t new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                        0, 0);
    }


    static __inline int
    AO_compare_and_swap_acquire(volatile size_t *addr,
                                      size_t old_val, size_t new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                        2, 2);
    }


    static __inline int
    AO_compare_and_swap_release(volatile size_t *addr,
                                      size_t old_val, size_t new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                              3,
                                              0 );
    }


    static __inline int
    AO_compare_and_swap_full(volatile size_t *addr,
                                   size_t old_val, size_t new_val)
    {
      return (int)__atomic_compare_exchange_n(addr, &old_val, new_val, 0,
                                              4,
                                              2 );
    }
# 88 "../src/atomic_ops/sysdeps/gcc/generic.h" 2


# 1 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h" 1
# 20 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned char
AO_char_fetch_and_add(volatile unsigned char *addr, unsigned char incr)
{
  return __atomic_fetch_add(addr, incr, 0);
}



  static __inline void
  AO_char_and(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_and_fetch(addr, value, 0);
  }




  static __inline void
  AO_char_or(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_or_fetch(addr, value, 0);
  }




  static __inline void
  AO_char_xor(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_xor_fetch(addr, value, 0);
  }
# 74 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned short
AO_short_fetch_and_add(volatile unsigned short *addr, unsigned short incr)
{
  return __atomic_fetch_add(addr, incr, 0);
}



  static __inline void
  AO_short_and(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_and_fetch(addr, value, 0);
  }




  static __inline void
  AO_short_or(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_or_fetch(addr, value, 0);
  }




  static __inline void
  AO_short_xor(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_xor_fetch(addr, value, 0);
  }
# 128 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned
AO_int_fetch_and_add(volatile unsigned *addr, unsigned incr)
{
  return __atomic_fetch_add(addr, incr, 0);
}



  static __inline void
  AO_int_and(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_and_fetch(addr, value, 0);
  }




  static __inline void
  AO_int_or(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_or_fetch(addr, value, 0);
  }




  static __inline void
  AO_int_xor(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_xor_fetch(addr, value, 0);
  }
# 182 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline size_t
AO_fetch_and_add(volatile size_t *addr, size_t incr)
{
  return __atomic_fetch_add(addr, incr, 0);
}



  static __inline void
  AO_and(volatile size_t *addr, size_t value)
  {
    (void)__atomic_and_fetch(addr, value, 0);
  }




  static __inline void
  AO_or(volatile size_t *addr, size_t value)
  {
    (void)__atomic_or_fetch(addr, value, 0);
  }




  static __inline void
  AO_xor(volatile size_t *addr, size_t value)
  {
    (void)__atomic_xor_fetch(addr, value, 0);
  }
# 236 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned char
AO_char_fetch_and_add_acquire(volatile unsigned char *addr, unsigned char incr)
{
  return __atomic_fetch_add(addr, incr, 2);
}



  static __inline void
  AO_char_and_acquire(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_and_fetch(addr, value, 2);
  }




  static __inline void
  AO_char_or_acquire(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_or_fetch(addr, value, 2);
  }




  static __inline void
  AO_char_xor_acquire(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_xor_fetch(addr, value, 2);
  }
# 290 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned short
AO_short_fetch_and_add_acquire(volatile unsigned short *addr, unsigned short incr)
{
  return __atomic_fetch_add(addr, incr, 2);
}



  static __inline void
  AO_short_and_acquire(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_and_fetch(addr, value, 2);
  }




  static __inline void
  AO_short_or_acquire(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_or_fetch(addr, value, 2);
  }




  static __inline void
  AO_short_xor_acquire(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_xor_fetch(addr, value, 2);
  }
# 344 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned
AO_int_fetch_and_add_acquire(volatile unsigned *addr, unsigned incr)
{
  return __atomic_fetch_add(addr, incr, 2);
}



  static __inline void
  AO_int_and_acquire(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_and_fetch(addr, value, 2);
  }




  static __inline void
  AO_int_or_acquire(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_or_fetch(addr, value, 2);
  }




  static __inline void
  AO_int_xor_acquire(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_xor_fetch(addr, value, 2);
  }
# 398 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline size_t
AO_fetch_and_add_acquire(volatile size_t *addr, size_t incr)
{
  return __atomic_fetch_add(addr, incr, 2);
}



  static __inline void
  AO_and_acquire(volatile size_t *addr, size_t value)
  {
    (void)__atomic_and_fetch(addr, value, 2);
  }




  static __inline void
  AO_or_acquire(volatile size_t *addr, size_t value)
  {
    (void)__atomic_or_fetch(addr, value, 2);
  }




  static __inline void
  AO_xor_acquire(volatile size_t *addr, size_t value)
  {
    (void)__atomic_xor_fetch(addr, value, 2);
  }
# 452 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned char
AO_char_fetch_and_add_release(volatile unsigned char *addr, unsigned char incr)
{
  return __atomic_fetch_add(addr, incr, 3);
}



  static __inline void
  AO_char_and_release(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_and_fetch(addr, value, 3);
  }




  static __inline void
  AO_char_or_release(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_or_fetch(addr, value, 3);
  }




  static __inline void
  AO_char_xor_release(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_xor_fetch(addr, value, 3);
  }
# 506 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned short
AO_short_fetch_and_add_release(volatile unsigned short *addr, unsigned short incr)
{
  return __atomic_fetch_add(addr, incr, 3);
}



  static __inline void
  AO_short_and_release(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_and_fetch(addr, value, 3);
  }




  static __inline void
  AO_short_or_release(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_or_fetch(addr, value, 3);
  }




  static __inline void
  AO_short_xor_release(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_xor_fetch(addr, value, 3);
  }
# 560 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned
AO_int_fetch_and_add_release(volatile unsigned *addr, unsigned incr)
{
  return __atomic_fetch_add(addr, incr, 3);
}



  static __inline void
  AO_int_and_release(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_and_fetch(addr, value, 3);
  }




  static __inline void
  AO_int_or_release(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_or_fetch(addr, value, 3);
  }




  static __inline void
  AO_int_xor_release(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_xor_fetch(addr, value, 3);
  }
# 614 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline size_t
AO_fetch_and_add_release(volatile size_t *addr, size_t incr)
{
  return __atomic_fetch_add(addr, incr, 3);
}



  static __inline void
  AO_and_release(volatile size_t *addr, size_t value)
  {
    (void)__atomic_and_fetch(addr, value, 3);
  }




  static __inline void
  AO_or_release(volatile size_t *addr, size_t value)
  {
    (void)__atomic_or_fetch(addr, value, 3);
  }




  static __inline void
  AO_xor_release(volatile size_t *addr, size_t value)
  {
    (void)__atomic_xor_fetch(addr, value, 3);
  }
# 668 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned char
AO_char_fetch_and_add_full(volatile unsigned char *addr, unsigned char incr)
{
  return __atomic_fetch_add(addr, incr, 5);
}



  static __inline void
  AO_char_and_full(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_and_fetch(addr, value, 5);
  }




  static __inline void
  AO_char_or_full(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_or_fetch(addr, value, 5);
  }




  static __inline void
  AO_char_xor_full(volatile unsigned char *addr, unsigned char value)
  {
    (void)__atomic_xor_fetch(addr, value, 5);
  }
# 722 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned short
AO_short_fetch_and_add_full(volatile unsigned short *addr, unsigned short incr)
{
  return __atomic_fetch_add(addr, incr, 5);
}



  static __inline void
  AO_short_and_full(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_and_fetch(addr, value, 5);
  }




  static __inline void
  AO_short_or_full(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_or_fetch(addr, value, 5);
  }




  static __inline void
  AO_short_xor_full(volatile unsigned short *addr, unsigned short value)
  {
    (void)__atomic_xor_fetch(addr, value, 5);
  }
# 776 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline unsigned
AO_int_fetch_and_add_full(volatile unsigned *addr, unsigned incr)
{
  return __atomic_fetch_add(addr, incr, 5);
}



  static __inline void
  AO_int_and_full(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_and_fetch(addr, value, 5);
  }




  static __inline void
  AO_int_or_full(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_or_fetch(addr, value, 5);
  }




  static __inline void
  AO_int_xor_full(volatile unsigned *addr, unsigned value)
  {
    (void)__atomic_xor_fetch(addr, value, 5);
  }
# 830 "../src/atomic_ops/sysdeps/gcc/generic-arithm.h"
static __inline size_t
AO_fetch_and_add_full(volatile size_t *addr, size_t incr)
{
  return __atomic_fetch_add(addr, incr, 5);
}



  static __inline void
  AO_and_full(volatile size_t *addr, size_t value)
  {
    (void)__atomic_and_fetch(addr, value, 5);
  }




  static __inline void
  AO_or_full(volatile size_t *addr, size_t value)
  {
    (void)__atomic_or_fetch(addr, value, 5);
  }




  static __inline void
  AO_xor_full(volatile size_t *addr, size_t value)
  {
    (void)__atomic_xor_fetch(addr, value, 5);
  }
# 91 "../src/atomic_ops/sysdeps/gcc/generic.h" 2




  static __inline AO_BYTE_TS_val
  AO_test_and_set(volatile unsigned char *addr)
  {
    return (AO_BYTE_TS_val)__atomic_test_and_set(addr, 0);
  }


  static __inline AO_BYTE_TS_val
  AO_test_and_set_acquire(volatile unsigned char *addr)
  {
    return (AO_BYTE_TS_val)__atomic_test_and_set(addr, 2);
  }


  static __inline AO_BYTE_TS_val
  AO_test_and_set_release(volatile unsigned char *addr)
  {
    return (AO_BYTE_TS_val)__atomic_test_and_set(addr, 3);
  }


  static __inline AO_BYTE_TS_val
  AO_test_and_set_full(volatile unsigned char *addr)
  {
    return (AO_BYTE_TS_val)__atomic_test_and_set(addr, 5);
  }
# 650 "../src/atomic_ops/sysdeps/gcc/x86.h" 2
# 295 "../src/atomic_ops.h" 2
# 443 "../src/atomic_ops.h"
# 1 "../src/atomic_ops/generalize.h" 1
# 178 "../src/atomic_ops/generalize.h"
  static __inline void AO_nop(void) {}
# 306 "../src/atomic_ops/generalize.h"
# 1 "../src/atomic_ops/generalize-small.h" 1
# 295 "../src/atomic_ops/generalize-small.h"
  static __inline unsigned char
  AO_char_load_read(const volatile unsigned char *addr)
  {
    unsigned char result = AO_char_load(addr);

    AO_nop_read();
    return result;
  }
# 823 "../src/atomic_ops/generalize-small.h"
  static __inline unsigned short
  AO_short_load_read(const volatile unsigned short *addr)
  {
    unsigned short result = AO_short_load(addr);

    AO_nop_read();
    return result;
  }
# 1351 "../src/atomic_ops/generalize-small.h"
  static __inline unsigned
  AO_int_load_read(const volatile unsigned *addr)
  {
    unsigned result = AO_int_load(addr);

    AO_nop_read();
    return result;
  }
# 1879 "../src/atomic_ops/generalize-small.h"
  static __inline size_t
  AO_load_read(const volatile size_t *addr)
  {
    size_t result = AO_load(addr);

    AO_nop_read();
    return result;
  }
# 307 "../src/atomic_ops/generalize.h" 2

# 1 "../src/atomic_ops/generalize-arithm.h" 1
# 309 "../src/atomic_ops/generalize.h" 2
# 444 "../src/atomic_ops.h" 2
# 2 "list_atomic.c" 2
# 15 "list_atomic.c"
# 1 "/usr/include/stdlib.h" 1 3 4
# 25 "/usr/include/stdlib.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 26 "/usr/include/stdlib.h" 2 3 4





# 1 "/usr/lib/gcc/x86_64-linux-gnu/8/include/stddef.h" 1 3 4
# 32 "/usr/include/stdlib.h" 2 3 4







# 1 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 1 3 4
# 52 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 3 4

# 52 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 3 4
typedef enum
{
  P_ALL,
  P_PID,
  P_PGID
} idtype_t;
# 40 "/usr/include/stdlib.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/waitstatus.h" 1 3 4
# 41 "/usr/include/stdlib.h" 2 3 4
# 55 "/usr/include/stdlib.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/floatn.h" 1 3 4
# 120 "/usr/include/x86_64-linux-gnu/bits/floatn.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/long-double.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 2 3 4
# 121 "/usr/include/x86_64-linux-gnu/bits/floatn.h" 2 3 4
# 56 "/usr/include/stdlib.h" 2 3 4


typedef struct
  {
    int quot;
    int rem;
  } div_t;



typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;





__extension__ typedef struct
  {
    long long int quot;
    long long int rem;
  } lldiv_t;
# 97 "/usr/include/stdlib.h" 3 4
extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__));



extern double atof (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));

extern int atoi (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));

extern long int atol (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));



__extension__ extern long long int atoll (const char *__nptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));



extern double strtod (const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



extern float strtof (const char *__restrict __nptr,
       char **__restrict __endptr) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern long double strtold (const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 176 "/usr/include/stdlib.h" 3 4
extern long int strtol (const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

extern unsigned long int strtoul (const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));



__extension__
extern long long int strtoq (const char *__restrict __nptr,
        char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

__extension__
extern unsigned long long int strtouq (const char *__restrict __nptr,
           char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));




__extension__
extern long long int strtoll (const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));

__extension__
extern unsigned long long int strtoull (const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 360 "/usr/include/stdlib.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__nothrow__ , __leaf__)) atoi (const char *__nptr)
{
  return (int) strtol (__nptr, (char **) ((void *)0), 10);
}
extern __inline __attribute__ ((__gnu_inline__)) long int
__attribute__ ((__nothrow__ , __leaf__)) atol (const char *__nptr)
{
  return strtol (__nptr, (char **) ((void *)0), 10);
}


__extension__ extern __inline __attribute__ ((__gnu_inline__)) long long int
__attribute__ ((__nothrow__ , __leaf__)) atoll (const char *__nptr)
{
  return strtoll (__nptr, (char **) ((void *)0), 10);
}
# 385 "/usr/include/stdlib.h" 3 4
extern char *l64a (long int __n) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__));


extern long int a64l (const char *__s)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));




# 1 "/usr/include/x86_64-linux-gnu/sys/types.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/types.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4


typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;






typedef __int8_t __int_least8_t;
typedef __uint8_t __uint_least8_t;
typedef __int16_t __int_least16_t;
typedef __uint16_t __uint_least16_t;
typedef __int32_t __int_least32_t;
typedef __uint32_t __uint_least32_t;
typedef __int64_t __int_least64_t;
typedef __uint64_t __uint_least64_t;



typedef long int __quad_t;
typedef unsigned long int __u_quad_t;







typedef long int __intmax_t;
typedef unsigned long int __uintmax_t;
# 140 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/typesizes.h" 1 3 4
# 141 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4


typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned long int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
typedef struct { int __val[2]; } __fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;

typedef int __daddr_t;
typedef int __key_t;


typedef int __clockid_t;


typedef void * __timer_t;


typedef long int __blksize_t;




typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;


typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;


typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;


typedef long int __fsword_t;

typedef long int __ssize_t;


typedef long int __syscall_slong_t;

typedef unsigned long int __syscall_ulong_t;



typedef __off64_t __loff_t;
typedef char *__caddr_t;


typedef long int __intptr_t;


typedef unsigned int __socklen_t;




typedef int __sig_atomic_t;
# 30 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;


typedef __loff_t loff_t;




typedef __ino_t ino_t;
# 59 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;





typedef __off_t off_t;
# 97 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __pid_t pid_t;





typedef __id_t id_t;




typedef __ssize_t ssize_t;





typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;




# 1 "/usr/include/x86_64-linux-gnu/bits/types/clock_t.h" 1 3 4






typedef __clock_t clock_t;
# 127 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/types/clockid_t.h" 1 3 4






typedef __clockid_t clockid_t;
# 129 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/time_t.h" 1 3 4






typedef __time_t time_t;
# 130 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/timer_t.h" 1 3 4






typedef __timer_t timer_t;
# 131 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 144 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/8/include/stddef.h" 1 3 4
# 145 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;




# 1 "/usr/include/x86_64-linux-gnu/bits/stdint-intn.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/stdint-intn.h" 3 4
typedef __int8_t int8_t;
typedef __int16_t int16_t;
typedef __int32_t int32_t;
typedef __int64_t int64_t;
# 156 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 177 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));
# 193 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/include/endian.h" 1 3 4
# 36 "/usr/include/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/endian.h" 1 3 4
# 37 "/usr/include/endian.h" 2 3 4
# 60 "/usr/include/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 1 3 4
# 33 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
static __inline __uint16_t
__bswap_16 (__uint16_t __bsx)
{

  return __builtin_bswap16 (__bsx);



}






static __inline __uint32_t
__bswap_32 (__uint32_t __bsx)
{

  return __builtin_bswap32 (__bsx);



}
# 69 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
__extension__ static __inline __uint64_t
__bswap_64 (__uint64_t __bsx)
{

  return __builtin_bswap64 (__bsx);



}
# 61 "/usr/include/endian.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/uintn-identity.h" 1 3 4
# 32 "/usr/include/x86_64-linux-gnu/bits/uintn-identity.h" 3 4
static __inline __uint16_t
__uint16_identity (__uint16_t __x)
{
  return __x;
}

static __inline __uint32_t
__uint32_identity (__uint32_t __x)
{
  return __x;
}

static __inline __uint64_t
__uint64_identity (__uint64_t __x)
{
  return __x;
}
# 62 "/usr/include/endian.h" 2 3 4
# 194 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/sys/select.h" 1 3 4
# 30 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/select.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/select.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/select.h" 2 3 4
# 31 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/types/sigset_t.h" 1 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/types/__sigset_t.h" 1 3 4




typedef struct
{
  unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
} __sigset_t;
# 5 "/usr/include/x86_64-linux-gnu/bits/types/sigset_t.h" 2 3 4


typedef __sigset_t sigset_t;
# 34 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_timeval.h" 1 3 4







struct timeval
{
  __time_t tv_sec;
  __suseconds_t tv_usec;
};
# 38 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_timespec.h" 1 3 4
# 9 "/usr/include/x86_64-linux-gnu/bits/types/struct_timespec.h" 3 4
struct timespec
{
  __time_t tv_sec;
  __syscall_slong_t tv_nsec;
};
# 40 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4



typedef __suseconds_t suseconds_t;





typedef long int __fd_mask;
# 59 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
typedef struct
  {






    __fd_mask __fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];


  } fd_set;






typedef __fd_mask fd_mask;
# 91 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4

# 101 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
# 113 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);





# 1 "/usr/include/x86_64-linux-gnu/bits/select2.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/select2.h" 3 4
extern long int __fdelt_chk (long int __d);
extern long int __fdelt_warn (long int __d)
  __attribute__((__warning__ ("bit outside of fd_set selected")));
# 124 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4



# 197 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4





typedef __blksize_t blksize_t;






typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 244 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 1 3 4
# 77 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 1 3 4
# 21 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 2 3 4
# 65 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 3 4
struct __pthread_rwlock_arch_t
{
  unsigned int __readers;
  unsigned int __writers;
  unsigned int __wrphase_futex;
  unsigned int __writers_futex;
  unsigned int __pad3;
  unsigned int __pad4;

  int __cur_writer;
  int __shared;
  signed char __rwelision;




  unsigned char __pad1[7];


  unsigned long int __pad2;


  unsigned int __flags;
# 99 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 3 4
};
# 78 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 2 3 4




typedef struct __pthread_internal_list
{
  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;
# 118 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 3 4
struct __pthread_mutex_s
{
  int __lock ;
  unsigned int __count;
  int __owner;

  unsigned int __nusers;



  int __kind;
 




  short __spins; short __elision;
  __pthread_list_t __list;
# 145 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 3 4
 
};




struct __pthread_cond_s
{
  __extension__ union
  {
    __extension__ unsigned long long int __wseq;
    struct
    {
      unsigned int __low;
      unsigned int __high;
    } __wseq32;
  };
  __extension__ union
  {
    __extension__ unsigned long long int __g1_start;
    struct
    {
      unsigned int __low;
      unsigned int __high;
    } __g1_start32;
  };
  unsigned int __g_refs[2] ;
  unsigned int __g_size[2];
  unsigned int __g1_orig_size;
  unsigned int __wrefs;
  unsigned int __g_signals[2];
};
# 24 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 2 3 4



typedef unsigned long int pthread_t;




typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;




typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;



typedef unsigned int pthread_key_t;



typedef int pthread_once_t;


union pthread_attr_t
{
  char __size[56];
  long int __align;
};

typedef union pthread_attr_t pthread_attr_t;




typedef union
{
  struct __pthread_mutex_s __data;
  char __size[40];
  long int __align;
} pthread_mutex_t;


typedef union
{
  struct __pthread_cond_s __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;





typedef union
{
  struct __pthread_rwlock_arch_t __data;
  char __size[56];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;





typedef volatile int pthread_spinlock_t;




typedef union
{
  char __size[32];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;
# 245 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



# 395 "/usr/include/stdlib.h" 2 3 4






extern long int random (void) __attribute__ ((__nothrow__ , __leaf__));


extern void srandom (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));





extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));



extern char *setstate (char *__statebuf) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));







struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };

extern int random_r (struct random_data *__restrict __buf,
       int32_t *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
   size_t __statelen,
   struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2, 4)));

extern int setstate_r (char *__restrict __statebuf,
         struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));





extern int rand (void) __attribute__ ((__nothrow__ , __leaf__));

extern void srand (unsigned int __seed) __attribute__ ((__nothrow__ , __leaf__));



extern int rand_r (unsigned int *__seed) __attribute__ ((__nothrow__ , __leaf__));







extern double drand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern double erand48 (unsigned short int __xsubi[3]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int lrand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern long int nrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern long int mrand48 (void) __attribute__ ((__nothrow__ , __leaf__));
extern long int jrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));


extern void srand48 (long int __seedval) __attribute__ ((__nothrow__ , __leaf__));
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    __extension__ unsigned long long int __a;

  };


extern int drand48_r (struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int lrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int mrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));


extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));

extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));

extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2)));




extern void *malloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__warn_unused_result__));

extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__warn_unused_result__));






extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__));
# 563 "/usr/include/stdlib.h" 3 4
extern void free (void *__ptr) __attribute__ ((__nothrow__ , __leaf__));


# 1 "/usr/include/alloca.h" 1 3 4
# 24 "/usr/include/alloca.h" 3 4
# 1 "/usr/lib/gcc/x86_64-linux-gnu/8/include/stddef.h" 1 3 4
# 25 "/usr/include/alloca.h" 2 3 4







extern void *alloca (size_t __size) __attribute__ ((__nothrow__ , __leaf__));






# 567 "/usr/include/stdlib.h" 2 3 4





extern void *valloc (size_t __size) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__warn_unused_result__));




extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));




extern void *aligned_alloc (size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__malloc__)) __attribute__ ((__alloc_size__ (2))) __attribute__ ((__warn_unused_result__));



extern void abort (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));







extern int at_quick_exit (void (*__func) (void)) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern void exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));





extern void quick_exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));





extern void _Exit (int __status) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));




extern char *getenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));
# 644 "/usr/include/stdlib.h" 3 4
extern int putenv (char *__string) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));





extern int setenv (const char *__name, const char *__value, int __replace)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));


extern int unsetenv (const char *__name) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));






extern int clearenv (void) __attribute__ ((__nothrow__ , __leaf__));
# 672 "/usr/include/stdlib.h" 3 4
extern char *mktemp (char *__template) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 685 "/usr/include/stdlib.h" 3 4
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));
# 707 "/usr/include/stdlib.h" 3 4
extern int mkstemps (char *__template, int __suffixlen) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));
# 728 "/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));
# 781 "/usr/include/stdlib.h" 3 4
extern int system (const char *__command) __attribute__ ((__warn_unused_result__));
# 797 "/usr/include/stdlib.h" 3 4
extern char *realpath (const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__));






typedef int (*__compar_fn_t) (const void *, const void *);
# 817 "/usr/include/stdlib.h" 3 4
extern void *bsearch (const void *__key, const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) __attribute__ ((__warn_unused_result__));


# 1 "/usr/include/x86_64-linux-gnu/bits/stdlib-bsearch.h" 1 3 4
# 19 "/usr/include/x86_64-linux-gnu/bits/stdlib-bsearch.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) void *
bsearch (const void *__key, const void *__base, size_t __nmemb, size_t __size,
  __compar_fn_t __compar)
{
  size_t __l, __u, __idx;
  const void *__p;
  int __comparison;

  __l = 0;
  __u = __nmemb;
  while (__l < __u)
    {
      __idx = (__l + __u) / 2;
      __p = (void *) (((const char *) __base) + (__idx * __size));
      __comparison = (*__compar) (__key, __p);
      if (__comparison < 0)
 __u = __idx;
      else if (__comparison > 0)
 __l = __idx + 1;
      else
 return (void *) __p;
    }

  return ((void *)0);
}
# 823 "/usr/include/stdlib.h" 2 3 4




extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));
# 837 "/usr/include/stdlib.h" 3 4
extern int abs (int __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) __attribute__ ((__warn_unused_result__));
extern long int labs (long int __x) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) __attribute__ ((__warn_unused_result__));


__extension__ extern long long int llabs (long long int __x)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) __attribute__ ((__warn_unused_result__));






extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) __attribute__ ((__warn_unused_result__));
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) __attribute__ ((__warn_unused_result__));


__extension__ extern lldiv_t lldiv (long long int __numer,
        long long int __denom)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__)) __attribute__ ((__warn_unused_result__));
# 869 "/usr/include/stdlib.h" 3 4
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) __attribute__ ((__warn_unused_result__));




extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) __attribute__ ((__warn_unused_result__));




extern char *gcvt (double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3))) __attribute__ ((__warn_unused_result__));




extern char *qecvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) __attribute__ ((__warn_unused_result__));
extern char *qfcvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4))) __attribute__ ((__warn_unused_result__));
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3))) __attribute__ ((__warn_unused_result__));




extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));

extern int qecvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (3, 4, 5)));





extern int mblen (const char *__s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));


extern int mbtowc (wchar_t *__restrict __pwc,
     const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));


extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__ , __leaf__));



extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ , __leaf__));

extern size_t wcstombs (char *__restrict __s,
   const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__ , __leaf__));







extern int rpmatch (const char *__response) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1))) __attribute__ ((__warn_unused_result__));
# 954 "/usr/include/stdlib.h" 3 4
extern int getsubopt (char **__restrict __optionp,
        char *const *__restrict __tokens,
        char **__restrict __valuep)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1, 2, 3))) __attribute__ ((__warn_unused_result__));
# 1000 "/usr/include/stdlib.h" 3 4
extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (1)));
# 1010 "/usr/include/stdlib.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stdlib-float.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/stdlib-float.h" 3 4
extern __inline __attribute__ ((__gnu_inline__)) double
__attribute__ ((__nothrow__ , __leaf__)) atof (const char *__nptr)
{
  return strtod (__nptr, (char **) ((void *)0));
}
# 1011 "/usr/include/stdlib.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/stdlib.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/stdlib.h" 3 4
extern char *__realpath_chk (const char *__restrict __name,
        char *__restrict __resolved,
        size_t __resolvedlen) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__));
extern char *__realpath_alias (const char *__restrict __name, char *__restrict __resolved) __asm__ ("" "realpath") __attribute__ ((__nothrow__ , __leaf__))

                                                 __attribute__ ((__warn_unused_result__));
extern char *__realpath_chk_warn (const char *__restrict __name, char *__restrict __resolved, size_t __resolvedlen) __asm__ ("" "__realpath_chk") __attribute__ ((__nothrow__ , __leaf__))


                                                __attribute__ ((__warn_unused_result__))
     __attribute__((__warning__ ("second argument of realpath must be either NULL or at " "least PATH_MAX bytes long buffer")))
                                      ;

extern __inline __attribute__ ((__always_inline__)) __attribute__ ((__gnu_inline__)) __attribute__ ((__artificial__)) __attribute__ ((__warn_unused_result__)) char *
__attribute__ ((__nothrow__ , __leaf__)) realpath (const char *__restrict __name, char *__restrict __resolved)
{
  if (__builtin_object_size (__resolved, 2 > 1) != (size_t) -1)
    {




      return __realpath_chk (__name, __resolved, __builtin_object_size (__resolved, 2 > 1));
    }

  return __realpath_alias (__name, __resolved);
}


extern int __ptsname_r_chk (int __fd, char *__buf, size_t __buflen,
       size_t __nreal) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__nonnull__ (2)));
extern int __ptsname_r_alias (int __fd, char *__buf, size_t __buflen) __asm__ ("" "ptsname_r") __attribute__ ((__nothrow__ , __leaf__))

     __attribute__ ((__nonnull__ (2)));
extern int __ptsname_r_chk_warn (int __fd, char *__buf, size_t __buflen, size_t __nreal) __asm__ ("" "__ptsname_r_chk") __attribute__ ((__nothrow__ , __leaf__))


     __attribute__ ((__nonnull__ (2))) __attribute__((__warning__ ("ptsname_r called with buflen bigger than " "size of buf")))
                   ;

extern __inline __attribute__ ((__always_inline__)) __attribute__ ((__gnu_inline__)) __attribute__ ((__artificial__)) int
__attribute__ ((__nothrow__ , __leaf__)) ptsname_r (int __fd, char *__buf, size_t __buflen)
{
  if (__builtin_object_size (__buf, 2 > 1) != (size_t) -1)
    {
      if (!__builtin_constant_p (__buflen))
 return __ptsname_r_chk (__fd, __buf, __buflen, __builtin_object_size (__buf, 2 > 1));
      if (__buflen > __builtin_object_size (__buf, 2 > 1))
 return __ptsname_r_chk_warn (__fd, __buf, __buflen, __builtin_object_size (__buf, 2 > 1));
    }
  return __ptsname_r_alias (__fd, __buf, __buflen);
}


extern int __wctomb_chk (char *__s, wchar_t __wchar, size_t __buflen)
  __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__warn_unused_result__));
extern int __wctomb_alias (char *__s, wchar_t __wchar) __asm__ ("" "wctomb") __attribute__ ((__nothrow__ , __leaf__))
              __attribute__ ((__warn_unused_result__));

extern __inline __attribute__ ((__always_inline__)) __attribute__ ((__gnu_inline__)) __attribute__ ((__artificial__)) __attribute__ ((__warn_unused_result__)) int
__attribute__ ((__nothrow__ , __leaf__)) wctomb (char *__s, wchar_t __wchar)
{







  if (__builtin_object_size (__s, 2 > 1) != (size_t) -1 && 16 > __builtin_object_size (__s, 2 > 1))
    return __wctomb_chk (__s, __wchar, __builtin_object_size (__s, 2 > 1));
  return __wctomb_alias (__s, __wchar);
}


extern size_t __mbstowcs_chk (wchar_t *__restrict __dst,
         const char *__restrict __src,
         size_t __len, size_t __dstlen) __attribute__ ((__nothrow__ , __leaf__));
extern size_t __mbstowcs_alias (wchar_t *__restrict __dst, const char *__restrict __src, size_t __len) __asm__ ("" "mbstowcs") __attribute__ ((__nothrow__ , __leaf__))


                                  ;
extern size_t __mbstowcs_chk_warn (wchar_t *__restrict __dst, const char *__restrict __src, size_t __len, size_t __dstlen) __asm__ ("" "__mbstowcs_chk") __attribute__ ((__nothrow__ , __leaf__))



     __attribute__((__warning__ ("mbstowcs called with dst buffer smaller than len " "* sizeof (wchar_t)")))
                        ;

extern __inline __attribute__ ((__always_inline__)) __attribute__ ((__gnu_inline__)) __attribute__ ((__artificial__)) size_t
__attribute__ ((__nothrow__ , __leaf__)) mbstowcs (wchar_t *__restrict __dst, const char *__restrict __src, size_t __len)

{
  if (__builtin_object_size (__dst, 2 > 1) != (size_t) -1)
    {
      if (!__builtin_constant_p (__len))
 return __mbstowcs_chk (__dst, __src, __len,
          __builtin_object_size (__dst, 2 > 1) / sizeof (wchar_t));

      if (__len > __builtin_object_size (__dst, 2 > 1) / sizeof (wchar_t))
 return __mbstowcs_chk_warn (__dst, __src, __len,
         __builtin_object_size (__dst, 2 > 1) / sizeof (wchar_t));
    }
  return __mbstowcs_alias (__dst, __src, __len);
}


extern size_t __wcstombs_chk (char *__restrict __dst,
         const wchar_t *__restrict __src,
         size_t __len, size_t __dstlen) __attribute__ ((__nothrow__ , __leaf__));
extern size_t __wcstombs_alias (char *__restrict __dst, const wchar_t *__restrict __src, size_t __len) __asm__ ("" "wcstombs") __attribute__ ((__nothrow__ , __leaf__))


                                  ;
extern size_t __wcstombs_chk_warn (char *__restrict __dst, const wchar_t *__restrict __src, size_t __len, size_t __dstlen) __asm__ ("" "__wcstombs_chk") __attribute__ ((__nothrow__ , __leaf__))



     __attribute__((__warning__ ("wcstombs called with dst buffer smaller than len")));

extern __inline __attribute__ ((__always_inline__)) __attribute__ ((__gnu_inline__)) __attribute__ ((__artificial__)) size_t
__attribute__ ((__nothrow__ , __leaf__)) wcstombs (char *__restrict __dst, const wchar_t *__restrict __src, size_t __len)

{
  if (__builtin_object_size (__dst, 2 > 1) != (size_t) -1)
    {
      if (!__builtin_constant_p (__len))
 return __wcstombs_chk (__dst, __src, __len, __builtin_object_size (__dst, 2 > 1));
      if (__len > __builtin_object_size (__dst, 2 > 1))
 return __wcstombs_chk_warn (__dst, __src, __len, __builtin_object_size (__dst, 2 > 1));
    }
  return __wcstombs_alias (__dst, __src, __len);
}
# 1015 "/usr/include/stdlib.h" 2 3 4






# 16 "list_atomic.c" 2


# 17 "list_atomic.c"
void list_atomic(void)
{






    static volatile size_t val ;



    static size_t oldval ;




    static size_t newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static size_t incr ;



    (void)"AO_nop(): ";
    AO_nop();





    (void)"AO_load(&val):";
    (void)AO_load(&val);




    (void)"AO_store(&val, newval):";
    AO_store(&val, newval);




    (void)"AO_fetch_and_add(&val, incr):";
    (void)AO_fetch_and_add(&val, incr);




    (void)"AO_fetch_and_add1(&val):";
    (void)AO_fetch_and_add(&val, 1);




    (void)"AO_fetch_and_sub1(&val):";
    (void)AO_fetch_and_add(&val, (size_t)(-1));




    (void)"AO_and(&val, incr):";
    AO_and(&val, incr);




    (void)"AO_or(&val, incr):";
    AO_or(&val, incr);




    (void)"AO_xor(&val, incr):";
    AO_xor(&val, incr);




    (void)"AO_compare_and_swap(&val, oldval, newval):";
    if (!AO_compare_and_swap(&val, oldval, newval))
      exit(1);






    (void)"AO_fetch_compare_and_swap(&val, oldval, newval):";
    if (AO_fetch_compare_and_swap(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set(&ts):";
    (void)AO_test_and_set(&ts);



}
# 138 "list_atomic.c"
void list_atomic_release(void)
{






    static volatile size_t val ;



    static size_t oldval ;




    static size_t newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static size_t incr ;






    (void)"No AO_nop_release";






    (void)"No AO_load_release";


    (void)"AO_store_release(&val, newval):";
    AO_store_release(&val, newval);




    (void)"AO_fetch_and_add_release(&val, incr):";
    (void)AO_fetch_and_add_release(&val, incr);




    (void)"AO_fetch_and_add1_release(&val):";
    (void)AO_fetch_and_add_release(&val, 1);




    (void)"AO_fetch_and_sub1_release(&val):";
    (void)AO_fetch_and_add_release(&val, (size_t)(-1));




    (void)"AO_and_release(&val, incr):";
    AO_and_release(&val, incr);




    (void)"AO_or_release(&val, incr):";
    AO_or_release(&val, incr);




    (void)"AO_xor_release(&val, incr):";
    AO_xor_release(&val, incr);




    (void)"AO_compare_and_swap_release(&val, oldval, newval):";
    if (!AO_compare_and_swap_release(&val, oldval, newval))
      exit(1);






    (void)"AO_fetch_compare_and_swap_release(&val, oldval, newval):";
    if (AO_fetch_compare_and_swap_release(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_release(&ts):";
    (void)AO_test_and_set_release(&ts);



}
# 259 "list_atomic.c"
void list_atomic_acquire(void)
{






    static volatile size_t val ;



    static size_t oldval ;




    static size_t newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static size_t incr ;






    (void)"No AO_nop_acquire";



    (void)"AO_load_acquire(&val):";
    (void)AO_load_acquire(&val);







    (void)"No AO_store_acquire";


    (void)"AO_fetch_and_add_acquire(&val, incr):";
    (void)AO_fetch_and_add_acquire(&val, incr);




    (void)"AO_fetch_and_add1_acquire(&val):";
    (void)AO_fetch_and_add_acquire(&val, 1);




    (void)"AO_fetch_and_sub1_acquire(&val):";
    (void)AO_fetch_and_add_acquire(&val, (size_t)(-1));




    (void)"AO_and_acquire(&val, incr):";
    AO_and_acquire(&val, incr);




    (void)"AO_or_acquire(&val, incr):";
    AO_or_acquire(&val, incr);




    (void)"AO_xor_acquire(&val, incr):";
    AO_xor_acquire(&val, incr);




    (void)"AO_compare_and_swap_acquire(&val, oldval, newval):";
    if (!AO_compare_and_swap_acquire(&val, oldval, newval))
      exit(1);






    (void)"AO_fetch_compare_and_swap_acquire(&val, oldval, newval):";
    if (AO_fetch_compare_and_swap_acquire(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_acquire(&ts):";
    (void)AO_test_and_set_acquire(&ts);



}
# 380 "list_atomic.c"
void list_atomic_read(void)
{






    static volatile size_t val ;



    static size_t oldval ;




    static size_t newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static size_t incr ;



    (void)"AO_nop_read(): ";
    AO_nop_read();





    (void)"AO_load_read(&val):";
    (void)AO_load_read(&val);







    (void)"No AO_store_read";


    (void)"AO_fetch_and_add_read(&val, incr):";
    (void)AO_fetch_and_add_full(&val, incr);




    (void)"AO_fetch_and_add1_read(&val):";
    (void)AO_fetch_and_add_full(&val, 1);




    (void)"AO_fetch_and_sub1_read(&val):";
    (void)AO_fetch_and_add_full(&val, (size_t)(-1));




    (void)"AO_and_read(&val, incr):";
    AO_and_full(&val, incr);




    (void)"AO_or_read(&val, incr):";
    AO_or_full(&val, incr);




    (void)"AO_xor_read(&val, incr):";
    AO_xor_full(&val, incr);




    (void)"AO_compare_and_swap_read(&val, oldval, newval):";
    if (!AO_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_fetch_compare_and_swap_read(&val, oldval, newval):";
    if (AO_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_read(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 501 "list_atomic.c"
void list_atomic_write(void)
{






    static volatile size_t val ;



    static size_t oldval ;




    static size_t newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static size_t incr ;



    (void)"AO_nop_write(): ";
    AO_nop_write();
# 539 "list_atomic.c"
    (void)"No AO_load_write";


    (void)"AO_store_write(&val, newval):";
    (AO_nop_write(), AO_store(&val, newval));




    (void)"AO_fetch_and_add_write(&val, incr):";
    (void)AO_fetch_and_add_full(&val, incr);




    (void)"AO_fetch_and_add1_write(&val):";
    (void)AO_fetch_and_add_full(&val, 1);




    (void)"AO_fetch_and_sub1_write(&val):";
    (void)AO_fetch_and_add_full(&val, (size_t)(-1));




    (void)"AO_and_write(&val, incr):";
    AO_and_full(&val, incr);




    (void)"AO_or_write(&val, incr):";
    AO_or_full(&val, incr);




    (void)"AO_xor_write(&val, incr):";
    AO_xor_full(&val, incr);




    (void)"AO_compare_and_swap_write(&val, oldval, newval):";
    if (!AO_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_fetch_compare_and_swap_write(&val, oldval, newval):";
    if (AO_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_write(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 622 "list_atomic.c"
void list_atomic_full(void)
{






    static volatile size_t val ;



    static size_t oldval ;




    static size_t newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static size_t incr ;



    (void)"AO_nop_full(): ";
    AO_nop_full();





    (void)"AO_load_full(&val):";
    (void)(AO_nop_full(), AO_load_acquire(&val));




    (void)"AO_store_full(&val, newval):";
    (AO_store_release(&val, newval), AO_nop_full());




    (void)"AO_fetch_and_add_full(&val, incr):";
    (void)AO_fetch_and_add_full(&val, incr);




    (void)"AO_fetch_and_add1_full(&val):";
    (void)AO_fetch_and_add_full(&val, 1);




    (void)"AO_fetch_and_sub1_full(&val):";
    (void)AO_fetch_and_add_full(&val, (size_t)(-1));




    (void)"AO_and_full(&val, incr):";
    AO_and_full(&val, incr);




    (void)"AO_or_full(&val, incr):";
    AO_or_full(&val, incr);




    (void)"AO_xor_full(&val, incr):";
    AO_xor_full(&val, incr);




    (void)"AO_compare_and_swap_full(&val, oldval, newval):";
    if (!AO_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_fetch_compare_and_swap_full(&val, oldval, newval):";
    if (AO_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_full(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 743 "list_atomic.c"
void list_atomic_release_write(void)
{






    static volatile size_t val ;



    static size_t oldval ;




    static size_t newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static size_t incr ;






    (void)"No AO_nop_release_write";






    (void)"No AO_load_release_write";


    (void)"AO_store_release_write(&val, newval):";
    AO_store_release(&val, newval);




    (void)"AO_fetch_and_add_release_write(&val, incr):";
    (void)AO_fetch_and_add_full(&val, incr);




    (void)"AO_fetch_and_add1_release_write(&val):";
    (void)AO_fetch_and_add_full(&val, 1);




    (void)"AO_fetch_and_sub1_release_write(&val):";
    (void)AO_fetch_and_add_full(&val, (size_t)(-1));




    (void)"AO_and_release_write(&val, incr):";
    AO_and_full(&val, incr);




    (void)"AO_or_release_write(&val, incr):";
    AO_or_full(&val, incr);




    (void)"AO_xor_release_write(&val, incr):";
    AO_xor_full(&val, incr);




    (void)"AO_compare_and_swap_release_write(&val, oldval, newval):";
    if (!AO_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_fetch_compare_and_swap_release_write(&val, oldval, newval):";
    if (AO_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_release_write(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 864 "list_atomic.c"
void list_atomic_acquire_read(void)
{






    static volatile size_t val ;



    static size_t oldval ;




    static size_t newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static size_t incr ;






    (void)"No AO_nop_acquire_read";



    (void)"AO_load_acquire_read(&val):";
    (void)AO_load_acquire(&val);







    (void)"No AO_store_acquire_read";


    (void)"AO_fetch_and_add_acquire_read(&val, incr):";
    (void)AO_fetch_and_add_full(&val, incr);




    (void)"AO_fetch_and_add1_acquire_read(&val):";
    (void)AO_fetch_and_add_full(&val, 1);




    (void)"AO_fetch_and_sub1_acquire_read(&val):";
    (void)AO_fetch_and_add_full(&val, (size_t)(-1));




    (void)"AO_and_acquire_read(&val, incr):";
    AO_and_full(&val, incr);




    (void)"AO_or_acquire_read(&val, incr):";
    AO_or_full(&val, incr);




    (void)"AO_xor_acquire_read(&val, incr):";
    AO_xor_full(&val, incr);




    (void)"AO_compare_and_swap_acquire_read(&val, oldval, newval):";
    if (!AO_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_fetch_compare_and_swap_acquire_read(&val, oldval, newval):";
    if (AO_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_acquire_read(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 985 "list_atomic.c"
void list_atomic_dd_acquire_read(void)
{






    static volatile size_t val ;



    static size_t oldval ;




    static size_t newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static size_t incr ;






    (void)"No AO_nop_dd_acquire_read";



    (void)"AO_load_dd_acquire_read(&val):";
    (void)AO_load(&val);







    (void)"No AO_store_dd_acquire_read";


    (void)"AO_fetch_and_add_dd_acquire_read(&val, incr):";
    (void)AO_fetch_and_add(&val, incr);




    (void)"AO_fetch_and_add1_dd_acquire_read(&val):";
    (void)AO_fetch_and_add(&val, 1);




    (void)"AO_fetch_and_sub1_dd_acquire_read(&val):";
    (void)AO_fetch_and_add(&val, (size_t)(-1));







    (void)"No AO_and_dd_acquire_read";





    (void)"No AO_or_dd_acquire_read";





    (void)"No AO_xor_dd_acquire_read";


    (void)"AO_compare_and_swap_dd_acquire_read(&val, oldval, newval):";
    if (!AO_compare_and_swap(&val, oldval, newval))
      exit(1);






    (void)"AO_fetch_compare_and_swap_dd_acquire_read(&val, oldval, newval):";
    if (AO_fetch_compare_and_swap(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_dd_acquire_read(&ts):";
    (void)AO_test_and_set(&ts);



}
# 1106 "list_atomic.c"
void char_list_atomic(void)
{






    static volatile unsigned char val ;



    static unsigned char oldval ;




    static unsigned char newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned char incr ;



    (void)"AO_nop(): ";
    AO_nop();





    (void)"AO_char_load(&val):";
    (void)AO_char_load(&val);




    (void)"AO_char_store(&val, newval):";
    AO_char_store(&val, newval);




    (void)"AO_char_fetch_and_add(&val, incr):";
    (void)AO_char_fetch_and_add(&val, incr);




    (void)"AO_char_fetch_and_add1(&val):";
    (void)AO_char_fetch_and_add(&val, 1);




    (void)"AO_char_fetch_and_sub1(&val):";
    (void)AO_char_fetch_and_add(&val, (unsigned char)(-1));




    (void)"AO_char_and(&val, incr):";
    AO_char_and(&val, incr);




    (void)"AO_char_or(&val, incr):";
    AO_char_or(&val, incr);




    (void)"AO_char_xor(&val, incr):";
    AO_char_xor(&val, incr);




    (void)"AO_char_compare_and_swap(&val, oldval, newval):";
    if (!AO_char_compare_and_swap(&val, oldval, newval))
      exit(1);






    (void)"AO_char_fetch_compare_and_swap(&val, oldval, newval):";
    if (AO_char_fetch_compare_and_swap(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set(&ts):";
    (void)AO_test_and_set(&ts);



}
# 1227 "list_atomic.c"
void char_list_atomic_release(void)
{






    static volatile unsigned char val ;



    static unsigned char oldval ;




    static unsigned char newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned char incr ;






    (void)"No AO_nop_release";






    (void)"No AO_char_load_release";


    (void)"AO_char_store_release(&val, newval):";
    AO_char_store_release(&val, newval);




    (void)"AO_char_fetch_and_add_release(&val, incr):";
    (void)AO_char_fetch_and_add_release(&val, incr);




    (void)"AO_char_fetch_and_add1_release(&val):";
    (void)AO_char_fetch_and_add_release(&val, 1);




    (void)"AO_char_fetch_and_sub1_release(&val):";
    (void)AO_char_fetch_and_add_release(&val, (unsigned char)(-1));




    (void)"AO_char_and_release(&val, incr):";
    AO_char_and_release(&val, incr);




    (void)"AO_char_or_release(&val, incr):";
    AO_char_or_release(&val, incr);




    (void)"AO_char_xor_release(&val, incr):";
    AO_char_xor_release(&val, incr);




    (void)"AO_char_compare_and_swap_release(&val, oldval, newval):";
    if (!AO_char_compare_and_swap_release(&val, oldval, newval))
      exit(1);






    (void)"AO_char_fetch_compare_and_swap_release(&val, oldval, newval):";
    if (AO_char_fetch_compare_and_swap_release(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_release(&ts):";
    (void)AO_test_and_set_release(&ts);



}
# 1348 "list_atomic.c"
void char_list_atomic_acquire(void)
{






    static volatile unsigned char val ;



    static unsigned char oldval ;




    static unsigned char newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned char incr ;






    (void)"No AO_nop_acquire";



    (void)"AO_char_load_acquire(&val):";
    (void)AO_char_load_acquire(&val);







    (void)"No AO_char_store_acquire";


    (void)"AO_char_fetch_and_add_acquire(&val, incr):";
    (void)AO_char_fetch_and_add_acquire(&val, incr);




    (void)"AO_char_fetch_and_add1_acquire(&val):";
    (void)AO_char_fetch_and_add_acquire(&val, 1);




    (void)"AO_char_fetch_and_sub1_acquire(&val):";
    (void)AO_char_fetch_and_add_acquire(&val, (unsigned char)(-1));




    (void)"AO_char_and_acquire(&val, incr):";
    AO_char_and_acquire(&val, incr);




    (void)"AO_char_or_acquire(&val, incr):";
    AO_char_or_acquire(&val, incr);




    (void)"AO_char_xor_acquire(&val, incr):";
    AO_char_xor_acquire(&val, incr);




    (void)"AO_char_compare_and_swap_acquire(&val, oldval, newval):";
    if (!AO_char_compare_and_swap_acquire(&val, oldval, newval))
      exit(1);






    (void)"AO_char_fetch_compare_and_swap_acquire(&val, oldval, newval):";
    if (AO_char_fetch_compare_and_swap_acquire(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_acquire(&ts):";
    (void)AO_test_and_set_acquire(&ts);



}
# 1469 "list_atomic.c"
void char_list_atomic_read(void)
{






    static volatile unsigned char val ;



    static unsigned char oldval ;




    static unsigned char newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned char incr ;



    (void)"AO_nop_read(): ";
    AO_nop_read();





    (void)"AO_char_load_read(&val):";
    (void)AO_char_load_read(&val);







    (void)"No AO_char_store_read";


    (void)"AO_char_fetch_and_add_read(&val, incr):";
    (void)AO_char_fetch_and_add_full(&val, incr);




    (void)"AO_char_fetch_and_add1_read(&val):";
    (void)AO_char_fetch_and_add_full(&val, 1);




    (void)"AO_char_fetch_and_sub1_read(&val):";
    (void)AO_char_fetch_and_add_full(&val, (unsigned char)(-1));




    (void)"AO_char_and_read(&val, incr):";
    AO_char_and_full(&val, incr);




    (void)"AO_char_or_read(&val, incr):";
    AO_char_or_full(&val, incr);




    (void)"AO_char_xor_read(&val, incr):";
    AO_char_xor_full(&val, incr);




    (void)"AO_char_compare_and_swap_read(&val, oldval, newval):";
    if (!AO_char_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_char_fetch_compare_and_swap_read(&val, oldval, newval):";
    if (AO_char_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_read(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 1590 "list_atomic.c"
void char_list_atomic_write(void)
{






    static volatile unsigned char val ;



    static unsigned char oldval ;




    static unsigned char newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned char incr ;



    (void)"AO_nop_write(): ";
    AO_nop_write();
# 1628 "list_atomic.c"
    (void)"No AO_char_load_write";


    (void)"AO_char_store_write(&val, newval):";
    (AO_nop_write(), AO_char_store(&val, newval));




    (void)"AO_char_fetch_and_add_write(&val, incr):";
    (void)AO_char_fetch_and_add_full(&val, incr);




    (void)"AO_char_fetch_and_add1_write(&val):";
    (void)AO_char_fetch_and_add_full(&val, 1);




    (void)"AO_char_fetch_and_sub1_write(&val):";
    (void)AO_char_fetch_and_add_full(&val, (unsigned char)(-1));




    (void)"AO_char_and_write(&val, incr):";
    AO_char_and_full(&val, incr);




    (void)"AO_char_or_write(&val, incr):";
    AO_char_or_full(&val, incr);




    (void)"AO_char_xor_write(&val, incr):";
    AO_char_xor_full(&val, incr);




    (void)"AO_char_compare_and_swap_write(&val, oldval, newval):";
    if (!AO_char_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_char_fetch_compare_and_swap_write(&val, oldval, newval):";
    if (AO_char_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_write(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 1711 "list_atomic.c"
void char_list_atomic_full(void)
{






    static volatile unsigned char val ;



    static unsigned char oldval ;




    static unsigned char newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned char incr ;



    (void)"AO_nop_full(): ";
    AO_nop_full();





    (void)"AO_char_load_full(&val):";
    (void)(AO_nop_full(), AO_char_load_acquire(&val));




    (void)"AO_char_store_full(&val, newval):";
    (AO_char_store_release(&val, newval), AO_nop_full());




    (void)"AO_char_fetch_and_add_full(&val, incr):";
    (void)AO_char_fetch_and_add_full(&val, incr);




    (void)"AO_char_fetch_and_add1_full(&val):";
    (void)AO_char_fetch_and_add_full(&val, 1);




    (void)"AO_char_fetch_and_sub1_full(&val):";
    (void)AO_char_fetch_and_add_full(&val, (unsigned char)(-1));




    (void)"AO_char_and_full(&val, incr):";
    AO_char_and_full(&val, incr);




    (void)"AO_char_or_full(&val, incr):";
    AO_char_or_full(&val, incr);




    (void)"AO_char_xor_full(&val, incr):";
    AO_char_xor_full(&val, incr);




    (void)"AO_char_compare_and_swap_full(&val, oldval, newval):";
    if (!AO_char_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_char_fetch_compare_and_swap_full(&val, oldval, newval):";
    if (AO_char_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_full(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 1832 "list_atomic.c"
void char_list_atomic_release_write(void)
{






    static volatile unsigned char val ;



    static unsigned char oldval ;




    static unsigned char newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned char incr ;






    (void)"No AO_nop_release_write";






    (void)"No AO_char_load_release_write";


    (void)"AO_char_store_release_write(&val, newval):";
    AO_char_store_release(&val, newval);




    (void)"AO_char_fetch_and_add_release_write(&val, incr):";
    (void)AO_char_fetch_and_add_full(&val, incr);




    (void)"AO_char_fetch_and_add1_release_write(&val):";
    (void)AO_char_fetch_and_add_full(&val, 1);




    (void)"AO_char_fetch_and_sub1_release_write(&val):";
    (void)AO_char_fetch_and_add_full(&val, (unsigned char)(-1));




    (void)"AO_char_and_release_write(&val, incr):";
    AO_char_and_full(&val, incr);




    (void)"AO_char_or_release_write(&val, incr):";
    AO_char_or_full(&val, incr);




    (void)"AO_char_xor_release_write(&val, incr):";
    AO_char_xor_full(&val, incr);




    (void)"AO_char_compare_and_swap_release_write(&val, oldval, newval):";
    if (!AO_char_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_char_fetch_compare_and_swap_release_write(&val, oldval, newval):";
    if (AO_char_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_release_write(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 1953 "list_atomic.c"
void char_list_atomic_acquire_read(void)
{






    static volatile unsigned char val ;



    static unsigned char oldval ;




    static unsigned char newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned char incr ;






    (void)"No AO_nop_acquire_read";



    (void)"AO_char_load_acquire_read(&val):";
    (void)AO_char_load_acquire(&val);







    (void)"No AO_char_store_acquire_read";


    (void)"AO_char_fetch_and_add_acquire_read(&val, incr):";
    (void)AO_char_fetch_and_add_full(&val, incr);




    (void)"AO_char_fetch_and_add1_acquire_read(&val):";
    (void)AO_char_fetch_and_add_full(&val, 1);




    (void)"AO_char_fetch_and_sub1_acquire_read(&val):";
    (void)AO_char_fetch_and_add_full(&val, (unsigned char)(-1));




    (void)"AO_char_and_acquire_read(&val, incr):";
    AO_char_and_full(&val, incr);




    (void)"AO_char_or_acquire_read(&val, incr):";
    AO_char_or_full(&val, incr);




    (void)"AO_char_xor_acquire_read(&val, incr):";
    AO_char_xor_full(&val, incr);




    (void)"AO_char_compare_and_swap_acquire_read(&val, oldval, newval):";
    if (!AO_char_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_char_fetch_compare_and_swap_acquire_read(&val, oldval, newval):";
    if (AO_char_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_acquire_read(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 2074 "list_atomic.c"
void char_list_atomic_dd_acquire_read(void)
{






    static volatile unsigned char val ;



    static unsigned char oldval ;




    static unsigned char newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned char incr ;






    (void)"No AO_nop_dd_acquire_read";



    (void)"AO_char_load_dd_acquire_read(&val):";
    (void)AO_char_load(&val);







    (void)"No AO_char_store_dd_acquire_read";


    (void)"AO_char_fetch_and_add_dd_acquire_read(&val, incr):";
    (void)AO_char_fetch_and_add(&val, incr);




    (void)"AO_char_fetch_and_add1_dd_acquire_read(&val):";
    (void)AO_char_fetch_and_add(&val, 1);




    (void)"AO_char_fetch_and_sub1_dd_acquire_read(&val):";
    (void)AO_char_fetch_and_add(&val, (unsigned char)(-1));







    (void)"No AO_char_and_dd_acquire_read";





    (void)"No AO_char_or_dd_acquire_read";





    (void)"No AO_char_xor_dd_acquire_read";


    (void)"AO_char_compare_and_swap_dd_acquire_read(&val, oldval, newval):";
    if (!AO_char_compare_and_swap(&val, oldval, newval))
      exit(1);






    (void)"AO_char_fetch_compare_and_swap_dd_acquire_read(&val, oldval, newval):";
    if (AO_char_fetch_compare_and_swap(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_dd_acquire_read(&ts):";
    (void)AO_test_and_set(&ts);



}
# 2195 "list_atomic.c"
void short_list_atomic(void)
{






    static volatile unsigned short val ;



    static unsigned short oldval ;




    static unsigned short newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned short incr ;



    (void)"AO_nop(): ";
    AO_nop();





    (void)"AO_short_load(&val):";
    (void)AO_short_load(&val);




    (void)"AO_short_store(&val, newval):";
    AO_short_store(&val, newval);




    (void)"AO_short_fetch_and_add(&val, incr):";
    (void)AO_short_fetch_and_add(&val, incr);




    (void)"AO_short_fetch_and_add1(&val):";
    (void)AO_short_fetch_and_add(&val, 1);




    (void)"AO_short_fetch_and_sub1(&val):";
    (void)AO_short_fetch_and_add(&val, (unsigned short)(-1));




    (void)"AO_short_and(&val, incr):";
    AO_short_and(&val, incr);




    (void)"AO_short_or(&val, incr):";
    AO_short_or(&val, incr);




    (void)"AO_short_xor(&val, incr):";
    AO_short_xor(&val, incr);




    (void)"AO_short_compare_and_swap(&val, oldval, newval):";
    if (!AO_short_compare_and_swap(&val, oldval, newval))
      exit(1);






    (void)"AO_short_fetch_compare_and_swap(&val, oldval, newval):";
    if (AO_short_fetch_compare_and_swap(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set(&ts):";
    (void)AO_test_and_set(&ts);



}
# 2316 "list_atomic.c"
void short_list_atomic_release(void)
{






    static volatile unsigned short val ;



    static unsigned short oldval ;




    static unsigned short newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned short incr ;






    (void)"No AO_nop_release";






    (void)"No AO_short_load_release";


    (void)"AO_short_store_release(&val, newval):";
    AO_short_store_release(&val, newval);




    (void)"AO_short_fetch_and_add_release(&val, incr):";
    (void)AO_short_fetch_and_add_release(&val, incr);




    (void)"AO_short_fetch_and_add1_release(&val):";
    (void)AO_short_fetch_and_add_release(&val, 1);




    (void)"AO_short_fetch_and_sub1_release(&val):";
    (void)AO_short_fetch_and_add_release(&val, (unsigned short)(-1));




    (void)"AO_short_and_release(&val, incr):";
    AO_short_and_release(&val, incr);




    (void)"AO_short_or_release(&val, incr):";
    AO_short_or_release(&val, incr);




    (void)"AO_short_xor_release(&val, incr):";
    AO_short_xor_release(&val, incr);




    (void)"AO_short_compare_and_swap_release(&val, oldval, newval):";
    if (!AO_short_compare_and_swap_release(&val, oldval, newval))
      exit(1);






    (void)"AO_short_fetch_compare_and_swap_release(&val, oldval, newval):";
    if (AO_short_fetch_compare_and_swap_release(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_release(&ts):";
    (void)AO_test_and_set_release(&ts);



}
# 2437 "list_atomic.c"
void short_list_atomic_acquire(void)
{






    static volatile unsigned short val ;



    static unsigned short oldval ;




    static unsigned short newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned short incr ;






    (void)"No AO_nop_acquire";



    (void)"AO_short_load_acquire(&val):";
    (void)AO_short_load_acquire(&val);







    (void)"No AO_short_store_acquire";


    (void)"AO_short_fetch_and_add_acquire(&val, incr):";
    (void)AO_short_fetch_and_add_acquire(&val, incr);




    (void)"AO_short_fetch_and_add1_acquire(&val):";
    (void)AO_short_fetch_and_add_acquire(&val, 1);




    (void)"AO_short_fetch_and_sub1_acquire(&val):";
    (void)AO_short_fetch_and_add_acquire(&val, (unsigned short)(-1));




    (void)"AO_short_and_acquire(&val, incr):";
    AO_short_and_acquire(&val, incr);




    (void)"AO_short_or_acquire(&val, incr):";
    AO_short_or_acquire(&val, incr);




    (void)"AO_short_xor_acquire(&val, incr):";
    AO_short_xor_acquire(&val, incr);




    (void)"AO_short_compare_and_swap_acquire(&val, oldval, newval):";
    if (!AO_short_compare_and_swap_acquire(&val, oldval, newval))
      exit(1);






    (void)"AO_short_fetch_compare_and_swap_acquire(&val, oldval, newval):";
    if (AO_short_fetch_compare_and_swap_acquire(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_acquire(&ts):";
    (void)AO_test_and_set_acquire(&ts);



}
# 2558 "list_atomic.c"
void short_list_atomic_read(void)
{






    static volatile unsigned short val ;



    static unsigned short oldval ;




    static unsigned short newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned short incr ;



    (void)"AO_nop_read(): ";
    AO_nop_read();





    (void)"AO_short_load_read(&val):";
    (void)AO_short_load_read(&val);







    (void)"No AO_short_store_read";


    (void)"AO_short_fetch_and_add_read(&val, incr):";
    (void)AO_short_fetch_and_add_full(&val, incr);




    (void)"AO_short_fetch_and_add1_read(&val):";
    (void)AO_short_fetch_and_add_full(&val, 1);




    (void)"AO_short_fetch_and_sub1_read(&val):";
    (void)AO_short_fetch_and_add_full(&val, (unsigned short)(-1));




    (void)"AO_short_and_read(&val, incr):";
    AO_short_and_full(&val, incr);




    (void)"AO_short_or_read(&val, incr):";
    AO_short_or_full(&val, incr);




    (void)"AO_short_xor_read(&val, incr):";
    AO_short_xor_full(&val, incr);




    (void)"AO_short_compare_and_swap_read(&val, oldval, newval):";
    if (!AO_short_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_short_fetch_compare_and_swap_read(&val, oldval, newval):";
    if (AO_short_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_read(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 2679 "list_atomic.c"
void short_list_atomic_write(void)
{






    static volatile unsigned short val ;



    static unsigned short oldval ;




    static unsigned short newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned short incr ;



    (void)"AO_nop_write(): ";
    AO_nop_write();
# 2717 "list_atomic.c"
    (void)"No AO_short_load_write";


    (void)"AO_short_store_write(&val, newval):";
    (AO_nop_write(), AO_short_store(&val, newval));




    (void)"AO_short_fetch_and_add_write(&val, incr):";
    (void)AO_short_fetch_and_add_full(&val, incr);




    (void)"AO_short_fetch_and_add1_write(&val):";
    (void)AO_short_fetch_and_add_full(&val, 1);




    (void)"AO_short_fetch_and_sub1_write(&val):";
    (void)AO_short_fetch_and_add_full(&val, (unsigned short)(-1));




    (void)"AO_short_and_write(&val, incr):";
    AO_short_and_full(&val, incr);




    (void)"AO_short_or_write(&val, incr):";
    AO_short_or_full(&val, incr);




    (void)"AO_short_xor_write(&val, incr):";
    AO_short_xor_full(&val, incr);




    (void)"AO_short_compare_and_swap_write(&val, oldval, newval):";
    if (!AO_short_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_short_fetch_compare_and_swap_write(&val, oldval, newval):";
    if (AO_short_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_write(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 2800 "list_atomic.c"
void short_list_atomic_full(void)
{






    static volatile unsigned short val ;



    static unsigned short oldval ;




    static unsigned short newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned short incr ;



    (void)"AO_nop_full(): ";
    AO_nop_full();





    (void)"AO_short_load_full(&val):";
    (void)(AO_nop_full(), AO_short_load_acquire(&val));




    (void)"AO_short_store_full(&val, newval):";
    (AO_short_store_release(&val, newval), AO_nop_full());




    (void)"AO_short_fetch_and_add_full(&val, incr):";
    (void)AO_short_fetch_and_add_full(&val, incr);




    (void)"AO_short_fetch_and_add1_full(&val):";
    (void)AO_short_fetch_and_add_full(&val, 1);




    (void)"AO_short_fetch_and_sub1_full(&val):";
    (void)AO_short_fetch_and_add_full(&val, (unsigned short)(-1));




    (void)"AO_short_and_full(&val, incr):";
    AO_short_and_full(&val, incr);




    (void)"AO_short_or_full(&val, incr):";
    AO_short_or_full(&val, incr);




    (void)"AO_short_xor_full(&val, incr):";
    AO_short_xor_full(&val, incr);




    (void)"AO_short_compare_and_swap_full(&val, oldval, newval):";
    if (!AO_short_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_short_fetch_compare_and_swap_full(&val, oldval, newval):";
    if (AO_short_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_full(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 2921 "list_atomic.c"
void short_list_atomic_release_write(void)
{






    static volatile unsigned short val ;



    static unsigned short oldval ;




    static unsigned short newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned short incr ;






    (void)"No AO_nop_release_write";






    (void)"No AO_short_load_release_write";


    (void)"AO_short_store_release_write(&val, newval):";
    AO_short_store_release(&val, newval);




    (void)"AO_short_fetch_and_add_release_write(&val, incr):";
    (void)AO_short_fetch_and_add_full(&val, incr);




    (void)"AO_short_fetch_and_add1_release_write(&val):";
    (void)AO_short_fetch_and_add_full(&val, 1);




    (void)"AO_short_fetch_and_sub1_release_write(&val):";
    (void)AO_short_fetch_and_add_full(&val, (unsigned short)(-1));




    (void)"AO_short_and_release_write(&val, incr):";
    AO_short_and_full(&val, incr);




    (void)"AO_short_or_release_write(&val, incr):";
    AO_short_or_full(&val, incr);




    (void)"AO_short_xor_release_write(&val, incr):";
    AO_short_xor_full(&val, incr);




    (void)"AO_short_compare_and_swap_release_write(&val, oldval, newval):";
    if (!AO_short_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_short_fetch_compare_and_swap_release_write(&val, oldval, newval):";
    if (AO_short_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_release_write(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 3042 "list_atomic.c"
void short_list_atomic_acquire_read(void)
{






    static volatile unsigned short val ;



    static unsigned short oldval ;




    static unsigned short newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned short incr ;






    (void)"No AO_nop_acquire_read";



    (void)"AO_short_load_acquire_read(&val):";
    (void)AO_short_load_acquire(&val);







    (void)"No AO_short_store_acquire_read";


    (void)"AO_short_fetch_and_add_acquire_read(&val, incr):";
    (void)AO_short_fetch_and_add_full(&val, incr);




    (void)"AO_short_fetch_and_add1_acquire_read(&val):";
    (void)AO_short_fetch_and_add_full(&val, 1);




    (void)"AO_short_fetch_and_sub1_acquire_read(&val):";
    (void)AO_short_fetch_and_add_full(&val, (unsigned short)(-1));




    (void)"AO_short_and_acquire_read(&val, incr):";
    AO_short_and_full(&val, incr);




    (void)"AO_short_or_acquire_read(&val, incr):";
    AO_short_or_full(&val, incr);




    (void)"AO_short_xor_acquire_read(&val, incr):";
    AO_short_xor_full(&val, incr);




    (void)"AO_short_compare_and_swap_acquire_read(&val, oldval, newval):";
    if (!AO_short_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_short_fetch_compare_and_swap_acquire_read(&val, oldval, newval):";
    if (AO_short_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_acquire_read(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 3163 "list_atomic.c"
void short_list_atomic_dd_acquire_read(void)
{






    static volatile unsigned short val ;



    static unsigned short oldval ;




    static unsigned short newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned short incr ;






    (void)"No AO_nop_dd_acquire_read";



    (void)"AO_short_load_dd_acquire_read(&val):";
    (void)AO_short_load(&val);







    (void)"No AO_short_store_dd_acquire_read";


    (void)"AO_short_fetch_and_add_dd_acquire_read(&val, incr):";
    (void)AO_short_fetch_and_add(&val, incr);




    (void)"AO_short_fetch_and_add1_dd_acquire_read(&val):";
    (void)AO_short_fetch_and_add(&val, 1);




    (void)"AO_short_fetch_and_sub1_dd_acquire_read(&val):";
    (void)AO_short_fetch_and_add(&val, (unsigned short)(-1));







    (void)"No AO_short_and_dd_acquire_read";





    (void)"No AO_short_or_dd_acquire_read";





    (void)"No AO_short_xor_dd_acquire_read";


    (void)"AO_short_compare_and_swap_dd_acquire_read(&val, oldval, newval):";
    if (!AO_short_compare_and_swap(&val, oldval, newval))
      exit(1);






    (void)"AO_short_fetch_compare_and_swap_dd_acquire_read(&val, oldval, newval):";
    if (AO_short_fetch_compare_and_swap(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_dd_acquire_read(&ts):";
    (void)AO_test_and_set(&ts);



}
# 3284 "list_atomic.c"
void int_list_atomic(void)
{






    static volatile unsigned val ;



    static unsigned oldval ;




    static unsigned newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned incr ;



    (void)"AO_nop(): ";
    AO_nop();





    (void)"AO_int_load(&val):";
    (void)AO_int_load(&val);




    (void)"AO_int_store(&val, newval):";
    AO_int_store(&val, newval);




    (void)"AO_int_fetch_and_add(&val, incr):";
    (void)AO_int_fetch_and_add(&val, incr);




    (void)"AO_int_fetch_and_add1(&val):";
    (void)AO_int_fetch_and_add(&val, 1);




    (void)"AO_int_fetch_and_sub1(&val):";
    (void)AO_int_fetch_and_add(&val, (unsigned)(-1));




    (void)"AO_int_and(&val, incr):";
    AO_int_and(&val, incr);




    (void)"AO_int_or(&val, incr):";
    AO_int_or(&val, incr);




    (void)"AO_int_xor(&val, incr):";
    AO_int_xor(&val, incr);




    (void)"AO_int_compare_and_swap(&val, oldval, newval):";
    if (!AO_int_compare_and_swap(&val, oldval, newval))
      exit(1);






    (void)"AO_int_fetch_compare_and_swap(&val, oldval, newval):";
    if (AO_int_fetch_compare_and_swap(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set(&ts):";
    (void)AO_test_and_set(&ts);



}
# 3405 "list_atomic.c"
void int_list_atomic_release(void)
{






    static volatile unsigned val ;



    static unsigned oldval ;




    static unsigned newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned incr ;






    (void)"No AO_nop_release";






    (void)"No AO_int_load_release";


    (void)"AO_int_store_release(&val, newval):";
    AO_int_store_release(&val, newval);




    (void)"AO_int_fetch_and_add_release(&val, incr):";
    (void)AO_int_fetch_and_add_release(&val, incr);




    (void)"AO_int_fetch_and_add1_release(&val):";
    (void)AO_int_fetch_and_add_release(&val, 1);




    (void)"AO_int_fetch_and_sub1_release(&val):";
    (void)AO_int_fetch_and_add_release(&val, (unsigned)(-1));




    (void)"AO_int_and_release(&val, incr):";
    AO_int_and_release(&val, incr);




    (void)"AO_int_or_release(&val, incr):";
    AO_int_or_release(&val, incr);




    (void)"AO_int_xor_release(&val, incr):";
    AO_int_xor_release(&val, incr);




    (void)"AO_int_compare_and_swap_release(&val, oldval, newval):";
    if (!AO_int_compare_and_swap_release(&val, oldval, newval))
      exit(1);






    (void)"AO_int_fetch_compare_and_swap_release(&val, oldval, newval):";
    if (AO_int_fetch_compare_and_swap_release(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_release(&ts):";
    (void)AO_test_and_set_release(&ts);



}
# 3526 "list_atomic.c"
void int_list_atomic_acquire(void)
{






    static volatile unsigned val ;



    static unsigned oldval ;




    static unsigned newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned incr ;






    (void)"No AO_nop_acquire";



    (void)"AO_int_load_acquire(&val):";
    (void)AO_int_load_acquire(&val);







    (void)"No AO_int_store_acquire";


    (void)"AO_int_fetch_and_add_acquire(&val, incr):";
    (void)AO_int_fetch_and_add_acquire(&val, incr);




    (void)"AO_int_fetch_and_add1_acquire(&val):";
    (void)AO_int_fetch_and_add_acquire(&val, 1);




    (void)"AO_int_fetch_and_sub1_acquire(&val):";
    (void)AO_int_fetch_and_add_acquire(&val, (unsigned)(-1));




    (void)"AO_int_and_acquire(&val, incr):";
    AO_int_and_acquire(&val, incr);




    (void)"AO_int_or_acquire(&val, incr):";
    AO_int_or_acquire(&val, incr);




    (void)"AO_int_xor_acquire(&val, incr):";
    AO_int_xor_acquire(&val, incr);




    (void)"AO_int_compare_and_swap_acquire(&val, oldval, newval):";
    if (!AO_int_compare_and_swap_acquire(&val, oldval, newval))
      exit(1);






    (void)"AO_int_fetch_compare_and_swap_acquire(&val, oldval, newval):";
    if (AO_int_fetch_compare_and_swap_acquire(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_acquire(&ts):";
    (void)AO_test_and_set_acquire(&ts);



}
# 3647 "list_atomic.c"
void int_list_atomic_read(void)
{






    static volatile unsigned val ;



    static unsigned oldval ;




    static unsigned newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned incr ;



    (void)"AO_nop_read(): ";
    AO_nop_read();





    (void)"AO_int_load_read(&val):";
    (void)AO_int_load_read(&val);







    (void)"No AO_int_store_read";


    (void)"AO_int_fetch_and_add_read(&val, incr):";
    (void)AO_int_fetch_and_add_full(&val, incr);




    (void)"AO_int_fetch_and_add1_read(&val):";
    (void)AO_int_fetch_and_add_full(&val, 1);




    (void)"AO_int_fetch_and_sub1_read(&val):";
    (void)AO_int_fetch_and_add_full(&val, (unsigned)(-1));




    (void)"AO_int_and_read(&val, incr):";
    AO_int_and_full(&val, incr);




    (void)"AO_int_or_read(&val, incr):";
    AO_int_or_full(&val, incr);




    (void)"AO_int_xor_read(&val, incr):";
    AO_int_xor_full(&val, incr);




    (void)"AO_int_compare_and_swap_read(&val, oldval, newval):";
    if (!AO_int_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_int_fetch_compare_and_swap_read(&val, oldval, newval):";
    if (AO_int_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_read(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 3768 "list_atomic.c"
void int_list_atomic_write(void)
{






    static volatile unsigned val ;



    static unsigned oldval ;




    static unsigned newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned incr ;



    (void)"AO_nop_write(): ";
    AO_nop_write();
# 3806 "list_atomic.c"
    (void)"No AO_int_load_write";


    (void)"AO_int_store_write(&val, newval):";
    (AO_nop_write(), AO_int_store(&val, newval));




    (void)"AO_int_fetch_and_add_write(&val, incr):";
    (void)AO_int_fetch_and_add_full(&val, incr);




    (void)"AO_int_fetch_and_add1_write(&val):";
    (void)AO_int_fetch_and_add_full(&val, 1);




    (void)"AO_int_fetch_and_sub1_write(&val):";
    (void)AO_int_fetch_and_add_full(&val, (unsigned)(-1));




    (void)"AO_int_and_write(&val, incr):";
    AO_int_and_full(&val, incr);




    (void)"AO_int_or_write(&val, incr):";
    AO_int_or_full(&val, incr);




    (void)"AO_int_xor_write(&val, incr):";
    AO_int_xor_full(&val, incr);




    (void)"AO_int_compare_and_swap_write(&val, oldval, newval):";
    if (!AO_int_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_int_fetch_compare_and_swap_write(&val, oldval, newval):";
    if (AO_int_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_write(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 3889 "list_atomic.c"
void int_list_atomic_full(void)
{






    static volatile unsigned val ;



    static unsigned oldval ;




    static unsigned newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned incr ;



    (void)"AO_nop_full(): ";
    AO_nop_full();





    (void)"AO_int_load_full(&val):";
    (void)(AO_nop_full(), AO_int_load_acquire(&val));




    (void)"AO_int_store_full(&val, newval):";
    (AO_int_store_release(&val, newval), AO_nop_full());




    (void)"AO_int_fetch_and_add_full(&val, incr):";
    (void)AO_int_fetch_and_add_full(&val, incr);




    (void)"AO_int_fetch_and_add1_full(&val):";
    (void)AO_int_fetch_and_add_full(&val, 1);




    (void)"AO_int_fetch_and_sub1_full(&val):";
    (void)AO_int_fetch_and_add_full(&val, (unsigned)(-1));




    (void)"AO_int_and_full(&val, incr):";
    AO_int_and_full(&val, incr);




    (void)"AO_int_or_full(&val, incr):";
    AO_int_or_full(&val, incr);




    (void)"AO_int_xor_full(&val, incr):";
    AO_int_xor_full(&val, incr);




    (void)"AO_int_compare_and_swap_full(&val, oldval, newval):";
    if (!AO_int_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_int_fetch_compare_and_swap_full(&val, oldval, newval):";
    if (AO_int_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_full(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 4010 "list_atomic.c"
void int_list_atomic_release_write(void)
{






    static volatile unsigned val ;



    static unsigned oldval ;




    static unsigned newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned incr ;






    (void)"No AO_nop_release_write";






    (void)"No AO_int_load_release_write";


    (void)"AO_int_store_release_write(&val, newval):";
    AO_int_store_release(&val, newval);




    (void)"AO_int_fetch_and_add_release_write(&val, incr):";
    (void)AO_int_fetch_and_add_full(&val, incr);




    (void)"AO_int_fetch_and_add1_release_write(&val):";
    (void)AO_int_fetch_and_add_full(&val, 1);




    (void)"AO_int_fetch_and_sub1_release_write(&val):";
    (void)AO_int_fetch_and_add_full(&val, (unsigned)(-1));




    (void)"AO_int_and_release_write(&val, incr):";
    AO_int_and_full(&val, incr);




    (void)"AO_int_or_release_write(&val, incr):";
    AO_int_or_full(&val, incr);




    (void)"AO_int_xor_release_write(&val, incr):";
    AO_int_xor_full(&val, incr);




    (void)"AO_int_compare_and_swap_release_write(&val, oldval, newval):";
    if (!AO_int_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_int_fetch_compare_and_swap_release_write(&val, oldval, newval):";
    if (AO_int_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_release_write(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 4131 "list_atomic.c"
void int_list_atomic_acquire_read(void)
{






    static volatile unsigned val ;



    static unsigned oldval ;




    static unsigned newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned incr ;






    (void)"No AO_nop_acquire_read";



    (void)"AO_int_load_acquire_read(&val):";
    (void)AO_int_load_acquire(&val);







    (void)"No AO_int_store_acquire_read";


    (void)"AO_int_fetch_and_add_acquire_read(&val, incr):";
    (void)AO_int_fetch_and_add_full(&val, incr);




    (void)"AO_int_fetch_and_add1_acquire_read(&val):";
    (void)AO_int_fetch_and_add_full(&val, 1);




    (void)"AO_int_fetch_and_sub1_acquire_read(&val):";
    (void)AO_int_fetch_and_add_full(&val, (unsigned)(-1));




    (void)"AO_int_and_acquire_read(&val, incr):";
    AO_int_and_full(&val, incr);




    (void)"AO_int_or_acquire_read(&val, incr):";
    AO_int_or_full(&val, incr);




    (void)"AO_int_xor_acquire_read(&val, incr):";
    AO_int_xor_full(&val, incr);




    (void)"AO_int_compare_and_swap_acquire_read(&val, oldval, newval):";
    if (!AO_int_compare_and_swap_full(&val, oldval, newval))
      exit(1);






    (void)"AO_int_fetch_compare_and_swap_acquire_read(&val, oldval, newval):";
    if (AO_int_fetch_compare_and_swap_full(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_acquire_read(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 4252 "list_atomic.c"
void int_list_atomic_dd_acquire_read(void)
{






    static volatile unsigned val ;



    static unsigned oldval ;




    static unsigned newval ;


    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);



    static unsigned incr ;






    (void)"No AO_nop_dd_acquire_read";



    (void)"AO_int_load_dd_acquire_read(&val):";
    (void)AO_int_load(&val);







    (void)"No AO_int_store_dd_acquire_read";


    (void)"AO_int_fetch_and_add_dd_acquire_read(&val, incr):";
    (void)AO_int_fetch_and_add(&val, incr);




    (void)"AO_int_fetch_and_add1_dd_acquire_read(&val):";
    (void)AO_int_fetch_and_add(&val, 1);




    (void)"AO_int_fetch_and_sub1_dd_acquire_read(&val):";
    (void)AO_int_fetch_and_add(&val, (unsigned)(-1));







    (void)"No AO_int_and_dd_acquire_read";





    (void)"No AO_int_or_dd_acquire_read";





    (void)"No AO_int_xor_dd_acquire_read";


    (void)"AO_int_compare_and_swap_dd_acquire_read(&val, oldval, newval):";
    if (!AO_int_compare_and_swap(&val, oldval, newval))
      exit(1);






    (void)"AO_int_fetch_compare_and_swap_dd_acquire_read(&val, oldval, newval):";
    if (AO_int_fetch_compare_and_swap(&val, oldval, newval) != oldval)
      exit(1);





    (void)"AO_test_and_set_dd_acquire_read(&ts):";
    (void)AO_test_and_set(&ts);



}
# 4373 "list_atomic.c"
void double_list_atomic(void)
{
# 4393 "list_atomic.c"
    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);







    (void)"AO_nop(): ";
    AO_nop();
# 4411 "list_atomic.c"
    (void)"No AO_double_load";





    (void)"No AO_double_store";





    (void)"No AO_double_fetch_and_add";





    (void)"No AO_double_fetch_and_add1";





    (void)"No AO_double_fetch_and_sub1";





    (void)"No AO_double_and";





    (void)"No AO_double_or";





    (void)"No AO_double_xor";






    (void)"No AO_double_compare_and_swap";
# 4469 "list_atomic.c"
    (void)"No AO_double_fetch_compare_and_swap";



    (void)"AO_test_and_set(&ts):";
    (void)AO_test_and_set(&ts);



}
# 4494 "list_atomic.c"
void double_list_atomic_release(void)
{
# 4514 "list_atomic.c"
    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);
# 4525 "list_atomic.c"
    (void)"No AO_nop_release";






    (void)"No AO_double_load_release";





    (void)"No AO_double_store_release";





    (void)"No AO_double_fetch_and_add_release";





    (void)"No AO_double_fetch_and_add1_release";





    (void)"No AO_double_fetch_and_sub1_release";





    (void)"No AO_double_and_release";





    (void)"No AO_double_or_release";





    (void)"No AO_double_xor_release";






    (void)"No AO_double_compare_and_swap_release";
# 4590 "list_atomic.c"
    (void)"No AO_double_fetch_compare_and_swap_release";



    (void)"AO_test_and_set_release(&ts):";
    (void)AO_test_and_set_release(&ts);



}
# 4615 "list_atomic.c"
void double_list_atomic_acquire(void)
{
# 4635 "list_atomic.c"
    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);
# 4646 "list_atomic.c"
    (void)"No AO_nop_acquire";






    (void)"No AO_double_load_acquire";





    (void)"No AO_double_store_acquire";





    (void)"No AO_double_fetch_and_add_acquire";





    (void)"No AO_double_fetch_and_add1_acquire";





    (void)"No AO_double_fetch_and_sub1_acquire";





    (void)"No AO_double_and_acquire";





    (void)"No AO_double_or_acquire";





    (void)"No AO_double_xor_acquire";






    (void)"No AO_double_compare_and_swap_acquire";
# 4711 "list_atomic.c"
    (void)"No AO_double_fetch_compare_and_swap_acquire";



    (void)"AO_test_and_set_acquire(&ts):";
    (void)AO_test_and_set_acquire(&ts);



}
# 4736 "list_atomic.c"
void double_list_atomic_read(void)
{
# 4756 "list_atomic.c"
    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);







    (void)"AO_nop_read(): ";
    AO_nop_read();
# 4774 "list_atomic.c"
    (void)"No AO_double_load_read";





    (void)"No AO_double_store_read";





    (void)"No AO_double_fetch_and_add_read";





    (void)"No AO_double_fetch_and_add1_read";





    (void)"No AO_double_fetch_and_sub1_read";





    (void)"No AO_double_and_read";





    (void)"No AO_double_or_read";





    (void)"No AO_double_xor_read";






    (void)"No AO_double_compare_and_swap_read";
# 4832 "list_atomic.c"
    (void)"No AO_double_fetch_compare_and_swap_read";



    (void)"AO_test_and_set_read(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 4857 "list_atomic.c"
void double_list_atomic_write(void)
{
# 4877 "list_atomic.c"
    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);







    (void)"AO_nop_write(): ";
    AO_nop_write();
# 4895 "list_atomic.c"
    (void)"No AO_double_load_write";





    (void)"No AO_double_store_write";





    (void)"No AO_double_fetch_and_add_write";





    (void)"No AO_double_fetch_and_add1_write";





    (void)"No AO_double_fetch_and_sub1_write";





    (void)"No AO_double_and_write";





    (void)"No AO_double_or_write";





    (void)"No AO_double_xor_write";






    (void)"No AO_double_compare_and_swap_write";
# 4953 "list_atomic.c"
    (void)"No AO_double_fetch_compare_and_swap_write";



    (void)"AO_test_and_set_write(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 4978 "list_atomic.c"
void double_list_atomic_full(void)
{
# 4998 "list_atomic.c"
    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);







    (void)"AO_nop_full(): ";
    AO_nop_full();
# 5016 "list_atomic.c"
    (void)"No AO_double_load_full";





    (void)"No AO_double_store_full";





    (void)"No AO_double_fetch_and_add_full";





    (void)"No AO_double_fetch_and_add1_full";





    (void)"No AO_double_fetch_and_sub1_full";





    (void)"No AO_double_and_full";





    (void)"No AO_double_or_full";





    (void)"No AO_double_xor_full";






    (void)"No AO_double_compare_and_swap_full";
# 5074 "list_atomic.c"
    (void)"No AO_double_fetch_compare_and_swap_full";



    (void)"AO_test_and_set_full(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 5099 "list_atomic.c"
void double_list_atomic_release_write(void)
{
# 5119 "list_atomic.c"
    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);
# 5130 "list_atomic.c"
    (void)"No AO_nop_release_write";






    (void)"No AO_double_load_release_write";





    (void)"No AO_double_store_release_write";





    (void)"No AO_double_fetch_and_add_release_write";





    (void)"No AO_double_fetch_and_add1_release_write";





    (void)"No AO_double_fetch_and_sub1_release_write";





    (void)"No AO_double_and_release_write";





    (void)"No AO_double_or_release_write";





    (void)"No AO_double_xor_release_write";






    (void)"No AO_double_compare_and_swap_release_write";
# 5195 "list_atomic.c"
    (void)"No AO_double_fetch_compare_and_swap_release_write";



    (void)"AO_test_and_set_release_write(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 5220 "list_atomic.c"
void double_list_atomic_acquire_read(void)
{
# 5240 "list_atomic.c"
    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);
# 5251 "list_atomic.c"
    (void)"No AO_nop_acquire_read";






    (void)"No AO_double_load_acquire_read";





    (void)"No AO_double_store_acquire_read";





    (void)"No AO_double_fetch_and_add_acquire_read";





    (void)"No AO_double_fetch_and_add1_acquire_read";





    (void)"No AO_double_fetch_and_sub1_acquire_read";





    (void)"No AO_double_and_acquire_read";





    (void)"No AO_double_or_acquire_read";





    (void)"No AO_double_xor_acquire_read";






    (void)"No AO_double_compare_and_swap_acquire_read";
# 5316 "list_atomic.c"
    (void)"No AO_double_fetch_compare_and_swap_acquire_read";



    (void)"AO_test_and_set_acquire_read(&ts):";
    (void)AO_test_and_set_full(&ts);



}
# 5341 "list_atomic.c"
void double_list_atomic_dd_acquire_read(void)
{
# 5361 "list_atomic.c"
    unsigned char ts = ((unsigned char)AO_BYTE_TS_clear);
# 5372 "list_atomic.c"
    (void)"No AO_nop_dd_acquire_read";






    (void)"No AO_double_load_dd_acquire_read";





    (void)"No AO_double_store_dd_acquire_read";





    (void)"No AO_double_fetch_and_add_dd_acquire_read";





    (void)"No AO_double_fetch_and_add1_dd_acquire_read";





    (void)"No AO_double_fetch_and_sub1_dd_acquire_read";





    (void)"No AO_double_and_dd_acquire_read";





    (void)"No AO_double_or_dd_acquire_read";





    (void)"No AO_double_xor_dd_acquire_read";






    (void)"No AO_double_compare_and_swap_dd_acquire_read";
# 5437 "list_atomic.c"
    (void)"No AO_double_fetch_compare_and_swap_dd_acquire_read";



    (void)"AO_test_and_set_dd_acquire_read(&ts):";
    (void)AO_test_and_set(&ts);



}
