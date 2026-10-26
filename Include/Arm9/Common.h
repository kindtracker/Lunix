#pragma once

#define STRING_BUFFER_SIZE 512

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void Memcpy(void *Destination, void *Source, size_t Size);
