#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#include <stdint.h>
#include <stddef.h>

#include "./idt/idt.h"
#include "./io/io.h"
#include "./memory/heap/kheap.h"
#include "./memory/paging/paging.h"
#include "./string/string.h"
#include "./fs/file.h"
#include "./disk/disk.h"
#include "./fs/pparser.h"
#include "./disk/streamer.h"


#define BIRNEOS_MAX_PATH 108

void kernel_main();
void print(const char* str);
void panic(const char* msg);

#define ERROR(value) (void*)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

#endif