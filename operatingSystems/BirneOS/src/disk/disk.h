#ifndef DISK_H
#define DISK_H

#include "../io/io.h"
#include "../config.h"
#include "../status.h"
#include "../memory/memory.h"

typedef unsigned int BIRNEOS_DISK_TYPE;

#define BIRNEOS_DISK_TYPE_REAL 0

struct disk {
    BIRNEOS_DISK_TYPE type;
    int sector_size;
};

void disk_search_and_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif