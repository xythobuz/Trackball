/* 
 * fat_disk.h
 */

#define DISK_BLOCK_COUNT 256
#define DISK_BLOCK_SIZE 512

void fat_disk_init(void);

uint8_t *fat_disk_get_sector(uint32_t sector);
