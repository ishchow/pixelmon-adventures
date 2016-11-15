#ifndef ARDUINO_EXTRAS_H
#define ARDUINO_EXTRAS_H

#define __ASSERT_USE_STDERR

#include <Arduino.h>
#include <assert.h>

/** These are missing from the standard Arduino libraries and AVR libc */
void* operator new[](size_t bytes);
void  operator delete[](void *p);

#endif
