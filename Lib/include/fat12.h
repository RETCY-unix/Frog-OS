#ifndef FAT12_H
#define FAT12_H

#define SECTOR_SIZE 512
#define MAX_FILENAME 11
#define MAX_FILES 224

// FAT12 Directory Entry (32 bytes)
typedef struct {
    char filename[11];        // 8.3 format
    unsigned char attributes;
    unsigned char reserved[10];
    unsigned short time;
    unsigned short date;
    unsigned short first_cluster;
    unsigned int file_size;
} __attribute__((packed)) fat12_entry_t;

// File handle for reading
typedef struct {
    char name[12];
    unsigned int size;
    unsigned int position;
    unsigned short cluster;
    unsigned char valid;
} file_handle_t;

// Function prototypes
void fat12_init();
int fat12_list_files(fat12_entry_t* entries, int max_entries);
int fat12_read_file(const char* filename, unsigned char* buffer, int max_size);
int fat12_write_file(const char* filename, unsigned char* data, int size);
int fat12_delete_file(const char* filename);
int fat12_file_exists(const char* filename);

#endif
