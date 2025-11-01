#include "../../Lib/include/ata.h"

// Port I/O helpers
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void inw_buffer(unsigned short port, unsigned short* buffer, int count) {
    __asm__ __volatile__("rep insw" : "+D"(buffer), "+c"(count) : "d"(port) : "memory");
}

static inline void outw_buffer(unsigned short port, unsigned short* buffer, int count) {
    __asm__ __volatile__("rep outsw" : "+S"(buffer), "+c"(count) : "d"(port) : "memory");
}

// Wait for disk to be ready with timeout
static int ata_wait_bsy() {
    int timeout = 100000;
    while ((inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY) && timeout > 0) {
        timeout--;
    }
    return timeout > 0 ? 0 : -1; // Return 0 on success, -1 on timeout
}

static int ata_wait_drq() {
    int timeout = 100000;
    while (!(inb(ATA_PRIMARY_STATUS) & ATA_STATUS_DRQ) && timeout > 0) {
        timeout--;
    }
    return timeout > 0 ? 0 : -1;
}

// Initialize ATA driver
void ata_init() {
    // Select master drive
    outb(ATA_PRIMARY_DRIVE, 0xA0);
    
    // Small delay
    for (volatile int i = 0; i < 1000; i++);
    
    // Don't wait forever - if disk not ready, continue anyway
    ata_wait_bsy();
}

// Read a sector from disk
int ata_read_sector(unsigned int lba, unsigned char* buffer) {
    if (ata_wait_bsy() != 0) {
        return -1; // Timeout
    }
    
    // Select master drive and set LBA mode
    outb(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Set sector count
    outb(ATA_PRIMARY_SECCOUNT, 1);
    
    // Set LBA
    outb(ATA_PRIMARY_LBA_LOW, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HIGH, (lba >> 16) & 0xFF);
    
    // Send read command
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_READ_SECTORS);
    
    // Wait for data
    if (ata_wait_drq() != 0) {
        return -1; // Timeout
    }
    
    // Read data (256 words = 512 bytes)
    inw_buffer(ATA_PRIMARY_DATA, (unsigned short*)buffer, 256);
    
    return 0;
}

// Write a sector to disk
int ata_write_sector(unsigned int lba, unsigned char* buffer) {
    if (ata_wait_bsy() != 0) {
        return -1;
    }
    
    // Select master drive and set LBA mode
    outb(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Set sector count
    outb(ATA_PRIMARY_SECCOUNT, 1);
    
    // Set LBA
    outb(ATA_PRIMARY_LBA_LOW, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HIGH, (lba >> 16) & 0xFF);
    
    // Send write command
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_WRITE_SECTORS);
    
    // Wait for ready
    if (ata_wait_drq() != 0) {
        return -1;
    }
    
    // Write data (256 words = 512 bytes)
    outw_buffer(ATA_PRIMARY_DATA, (unsigned short*)buffer, 256);
    
    // Flush cache
    ata_wait_bsy();
    
    return 0;
}
