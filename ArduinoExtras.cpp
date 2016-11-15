#include "ArduinoExtras.h"

void* operator new[](size_t bytes) { return malloc(bytes); }
void  operator delete[](void *p)   { free(p); }

/** Required for assert(). */
void __assert(
  const char *func, const char *file, int line, const char *expr)
{
  Serial.print(file); Serial.print(":");
  Serial.print(line); Serial.print(": assertion failed: ");
  Serial.println(expr);
  Serial.flush();

  while (true) { }
}
