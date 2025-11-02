// Safe stub functions for file system
// These do nothing but satisfy the linker

void ata_init(void) {
    // Do nothing for now
}

void fat12_init(void) {
    // Do nothing for now
}

int fat12_list_files(void* entries, int max_entries) {
    return 0; // No files
}

int fat12_read_file(const char* filename, unsigned char* buffer, int max_size) {
    return -1; // Error
}

int fat12_write_file(const char* filename, unsigned char* data, int size) {
    return -1; // Error
}

int fat12_delete_file(const char* filename) {
    return -1; // Error
}
