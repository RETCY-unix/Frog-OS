#include "../../Lib/include/fat12.h"
#include "../../Lib/include/ata.h"

#define FAT_START_SECTOR 1
#define ROOT_DIR_SECTOR 19
#define DATA_START_SECTOR 33
#define ROOT_DIR_SECTORS 14

// MOVED TO SMALLER TEMPORARY BUFFER - no huge static arrays
static unsigned char sector_buffer[SECTOR_SIZE];
static int fs_initialized = 0;

// Safe string length
static int str_len(const char* s) {
    int len = 0;
    while (s[len] && len < 256) len++;
    return len;
}

// Safe string compare
static int str_cmp(const char* s1, const char* s2) {
    int i = 0;
    while (i < 256 && s1[i] && s2[i]) {
        if (s1[i] != s2[i]) {
            return s1[i] - s2[i];
        }
        i++;
    }
    return s1[i] - s2[i];
}

// Convert filename to FAT12 format (8.3)
static void format_filename(const char* input, char* output) {
    int i, j = 0;
    
    // Clear output
    for (i = 0; i < 11; i++) {
        output[i] = ' ';
    }
    
    if (!input || input[0] == '\0') return;
    
    // Copy name (up to 8 chars)
    for (i = 0; i < 8 && j < 256 && input[j] && input[j] != '.'; i++, j++) {
        if (input[j] >= 'a' && input[j] <= 'z') {
            output[i] = input[j] - 32;
        } else {
            output[i] = input[j];
        }
    }
    
    // Skip to extension
    while (j < 256 && input[j] && input[j] != '.') j++;
    if (j < 256 && input[j] == '.') j++;
    
    // Copy extension (up to 3 chars)
    for (i = 8; i < 11 && j < 256 && input[j]; i++, j++) {
        if (input[j] >= 'a' && input[j] <= 'z') {
            output[i] = input[j] - 32;
        } else {
            output[i] = input[j];
        }
    }
}

// Initialize FAT12 - MINIMAL, non-blocking
void fat12_init() {
    // Just mark as initialized - don't actually read from disk yet
    fs_initialized = 1;
}

// List all files in root directory
int fat12_list_files(fat12_entry_t* entries, int max_entries) {
    if (!fs_initialized || !entries || max_entries <= 0) {
        return 0;
    }
    
    int count = 0;
    
    for (int sector = 0; sector < ROOT_DIR_SECTORS && count < max_entries; sector++) {
        if (ata_read_sector(ROOT_DIR_SECTOR + sector, sector_buffer) != 0) {
            break;
        }
        
        fat12_entry_t* dir = (fat12_entry_t*)sector_buffer;
        
        for (int i = 0; i < 16 && count < max_entries; i++) {
            // Check if entry is valid
            if (dir[i].filename[0] == 0x00) {
                return count;
            }
            
            if (dir[i].filename[0] == 0xE5 || dir[i].filename[0] == 0x2E) {
                continue;
            }
            
            if (dir[i].attributes & 0x08) {
                continue;
            }
            
            // Copy entry safely
            for (int j = 0; j < 11; j++) {
                entries[count].filename[j] = dir[i].filename[j];
            }
            entries[count].attributes = dir[i].attributes;
            entries[count].first_cluster = dir[i].first_cluster;
            entries[count].file_size = dir[i].file_size;
            count++;
        }
    }
    
    return count;
}

// Find a file in root directory
static int find_file(const char* filename, fat12_entry_t* entry, int* sector_out, int* index_out) {
    if (!fs_initialized || !filename) {
        return 0;
    }
    
    char formatted[11];
    format_filename(filename, formatted);
    
    for (int sector = 0; sector < ROOT_DIR_SECTORS; sector++) {
        if (ata_read_sector(ROOT_DIR_SECTOR + sector, sector_buffer) != 0) {
            return 0;
        }
        
        fat12_entry_t* dir = (fat12_entry_t*)sector_buffer;
        
        for (int i = 0; i < 16; i++) {
            if (dir[i].filename[0] == 0x00) {
                return 0;
            }
            
            if (dir[i].filename[0] == 0xE5) {
                continue;
            }
            
            // Compare filename
            int match = 1;
            for (int j = 0; j < 11; j++) {
                if (dir[i].filename[j] != formatted[j]) {
                    match = 0;
                    break;
                }
            }
            
            if (match) {
                if (entry) {
                    for (int j = 0; j < 32; j++) {
                        ((unsigned char*)entry)[j] = ((unsigned char*)&dir[i])[j];
                    }
                }
                if (sector_out) *sector_out = sector;
                if (index_out) *index_out = i;
                return 1;
            }
        }
    }
    
    return 0;
}

// Check if file exists
int fat12_file_exists(const char* filename) {
    if (!fs_initialized) return 0;
    return find_file(filename, 0, 0, 0);
}

// Read file contents
int fat12_read_file(const char* filename, unsigned char* buffer, int max_size) {
    if (!fs_initialized || !filename || !buffer || max_size <= 0) {
        return -1;
    }
    
    fat12_entry_t entry;
    
    if (!find_file(filename, &entry, 0, 0)) {
        return -1;
    }
    
    int bytes_read = 0;
    int bytes_to_read = entry.file_size;
    if (bytes_to_read > max_size) {
        bytes_to_read = max_size;
    }
    
    if (bytes_to_read == 0) {
        return 0;
    }
    
    unsigned short cluster = entry.first_cluster;
    
    // Safety: only read first cluster for now
    if (cluster >= 2 && cluster < 0xFF8) {
        int sector = DATA_START_SECTOR + cluster - 2;
        if (ata_read_sector(sector, sector_buffer) == 0) {
            int to_copy = bytes_to_read;
            if (to_copy > SECTOR_SIZE) {
                to_copy = SECTOR_SIZE;
            }
            
            for (int i = 0; i < to_copy; i++) {
                buffer[i] = sector_buffer[i];
            }
            bytes_read = to_copy;
        }
    }
    
    return bytes_read;
}

// Write file
int fat12_write_file(const char* filename, unsigned char* data, int size) {
    if (!fs_initialized || !filename || !data || size < 0 || size > SECTOR_SIZE) {
        return -1;
    }
    
    // Find free directory entry
    int free_sector = -1, free_index = -1;
    
    for (int sector = 0; sector < ROOT_DIR_SECTORS; sector++) {
        if (ata_read_sector(ROOT_DIR_SECTOR + sector, sector_buffer) != 0) {
            continue;
        }
        
        fat12_entry_t* dir = (fat12_entry_t*)sector_buffer;
        
        for (int i = 0; i < 16; i++) {
            if (dir[i].filename[0] == 0x00 || dir[i].filename[0] == 0xE5) {
                free_sector = sector;
                free_index = i;
                break;
            }
        }
        if (free_sector != -1) break;
    }
    
    if (free_sector == -1) {
        return -1;
    }
    
    // Use cluster 2 for simplicity
    unsigned short cluster = 2;
    
    // Read directory sector
    if (ata_read_sector(ROOT_DIR_SECTOR + free_sector, sector_buffer) != 0) {
        return -1;
    }
    
    fat12_entry_t* dir = (fat12_entry_t*)sector_buffer;
    
    // Create entry
    format_filename(filename, dir[free_index].filename);
    dir[free_index].attributes = 0x20;
    dir[free_index].first_cluster = cluster;
    dir[free_index].file_size = size;
    dir[free_index].time = 0;
    dir[free_index].date = 0;
    
    // Write directory
    if (ata_write_sector(ROOT_DIR_SECTOR + free_sector, sector_buffer) != 0) {
        return -1;
    }
    
    // Write data
    for (int i = 0; i < SECTOR_SIZE; i++) {
        sector_buffer[i] = 0;
    }
    
    for (int i = 0; i < size && i < SECTOR_SIZE; i++) {
        sector_buffer[i] = data[i];
    }
    
    int data_sector = DATA_START_SECTOR + cluster - 2;
    if (ata_write_sector(data_sector, sector_buffer) != 0) {
        return -1;
    }
    
    return size;
}

// Delete file
int fat12_delete_file(const char* filename) {
    if (!fs_initialized || !filename) {
        return -1;
    }
    
    int sector, index;
    
    if (!find_file(filename, 0, &sector, &index)) {
        return -1;
    }
    
    if (ata_read_sector(ROOT_DIR_SECTOR + sector, sector_buffer) != 0) {
        return -1;
    }
    
    sector_buffer[index * 32] = 0xE5;
    
    if (ata_write_sector(ROOT_DIR_SECTOR + sector, sector_buffer) != 0) {
        return -1;
    }
    
    return 0;
}
