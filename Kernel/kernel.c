void kernel_main() {
    unsigned char *video = (unsigned char*)0xB8000;
    
    int i;
    for (i = 0; i < 80 * 25 * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0x00;
    }
    
    char block = 219;
    char space = ' ';
    
    char line1[] = {block,block,block,block,block,block,block,block,space,block,block,block,block,block,block,space,block,block,block,block,block,block,block,block,space,block,block,block,block,block,block,block,block,space,block,block,space,space,block,block,space,block,block,space,space,block,block,space,block,block,space,space,block,block,space,space,block,block,block,block,space,space,block,block,block,block,block,block,block,block,0};
    char line2[] = {block,block,space,space,space,space,space,space,space,block,block,space,space,space,space,space,block,block,space,space,space,space,block,block,space,block,block,space,space,space,space,block,block,space,block,block,space,space,block,block,space,block,block,space,block,block,space,space,block,block,space,space,block,block,space,block,block,space,space,block,block,space,block,block,space,space,space,space,space,space,0};
    char line3[] = {block,block,block,block,block,block,block,space,space,block,block,block,block,block,block,space,block,block,block,block,block,block,block,block,space,block,block,block,block,block,block,block,block,space,block,block,space,space,block,block,space,block,block,block,block,space,space,space,block,block,space,space,block,block,space,block,block,space,space,block,block,space,block,block,block,block,block,block,block,space,0};
    char line4[] = {space,space,space,space,space,block,block,space,space,block,block,space,space,space,space,space,block,block,space,space,space,space,space,space,space,block,block,space,space,space,space,space,space,space,block,block,space,space,block,block,space,block,block,space,block,block,space,space,block,block,space,space,block,block,space,block,block,space,space,block,block,space,space,space,space,space,space,block,block,0};
    char line5[] = {block,block,block,block,block,block,block,block,space,block,block,block,block,block,block,space,block,block,space,space,space,space,space,space,space,block,block,space,space,space,space,space,space,space,space,block,block,block,block,space,space,block,block,space,space,block,block,space,space,block,block,block,block,space,space,space,block,block,block,block,space,space,block,block,block,block,block,block,block,block,0};
    
    const char *subtitle = "I fucking hate this";
    
    int start_x = 5;
    
    i = 0;
    int offset = 80 * 2 * 9 + start_x * 2;
    while (line1[i] != 0) {
        video[offset + i * 2] = line1[i];
        video[offset + i * 2 + 1] = 0x04;
        i++;
    }
    
    i = 0;
    offset = 80 * 2 * 10 + start_x * 2;
    while (line2[i] != 0) {
        video[offset + i * 2] = line2[i];
        video[offset + i * 2 + 1] = 0x04;
        i++;
    }
    
    i = 0;
    offset = 80 * 2 * 11 + start_x * 2;
    while (line3[i] != 0) {
        video[offset + i * 2] = line3[i];
        video[offset + i * 2 + 1] = 0x04;
        i++;
    }
    
    i = 0;
    offset = 80 * 2 * 12 + start_x * 2;
    while (line4[i] != 0) {
        video[offset + i * 2] = line4[i];
        video[offset + i * 2 + 1] = 0x04;
        i++;
    }
    
    i = 0;
    offset = 80 * 2 * 13 + start_x * 2;
    while (line5[i] != 0) {
        video[offset + i * 2] = line5[i];
        video[offset + i * 2 + 1] = 0x04;
        i++;
    }
    
    i = 0;
    offset = 80 * 2 * 16 + 30 * 2;
    while (subtitle[i] != '\0') {
        video[offset + i * 2] = subtitle[i];
        video[offset + i * 2 + 1] = 0x08;
        i++;
    }
    
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
