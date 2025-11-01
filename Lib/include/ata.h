#ifndef ATA_H
#define ATA_H

// ATA I/O ports
#define ATA_PRIMARY_DATA       0x1F0
#define ATA_PRIMARY_ERROR      0x1F1
#define ATA_PRIMARY_SECCOUNT   0x1F2
#define ATA_PRIMARY_LBA_LOW    0x1F3
#define ATA_PRIMARY_LBA_MID    0x1F4
#define ATA_PRIMARY_LBA_HIGH   0x1F5
#define ATA_PRIMARY_DRIVE      0x1F6
#define ATA_PRIMARY_STATUS     0x1F7
#define ATA_PRIMARY_COMMAND    0x1F7

// ATA Commands
#define ATA_CMD_READ_SECTORS   0x20
#define ATA_CMD_WRITE_SECTORS  0x30
#define ATA_CMD_IDENTIFY       0xEC

// ATA Status bits
#define ATA_STATUS_BSY   0x80
#define ATA_STATUS_DRDY  0x40
#define ATA_STATUS_DRQ   0x08
#define ATA_STATUS_ERR   0x01

// Function prototypes
void ata_init();
int ata_read_sector(unsigned int lba, unsigned char* buffer);
int ata_write_sector(unsigned int lba, unsigned char* buffer);

#endif
