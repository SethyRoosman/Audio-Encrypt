#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void read_header(FILE *fptr, FILE *new_file) {
    char r = fgetc(fptr);
    char i = fgetc(fptr);
    char f1 = fgetc(fptr);
    char f2 = fgetc(fptr);
    printf("%c%c%c%c  ", r, i, f1, f2);
    putc(r, new_file);
    putc(i, new_file);
    putc(f1, new_file);
    putc(f2, new_file);
    // 4 bytes must be read as little-endian
    char chunkSize[4] = {fgetc(fptr), fgetc(fptr), fgetc(fptr), fgetc(fptr)};
    // chunk size MIGHT be above 2^32
    unsigned int chunkSum = (chunkSize[3] << 8) | chunkSize[2];
    chunkSum = (chunkSum << 8) | chunkSize[1];
    chunkSum = (chunkSum << 8) | chunkSize[0];
    printf(" | ChunkSize:     %10u", chunkSum); // little
    putc(chunkSize[0], new_file);
    putc(chunkSize[1], new_file);
    putc(chunkSize[2], new_file);
    putc(chunkSize[3], new_file);
    char w = fgetc(fptr);
    char a = fgetc(fptr);
    char v = fgetc(fptr);
    char e = fgetc(fptr);
    putc(w, new_file);
    putc(a, new_file);
    putc(v, new_file);
    putc(e, new_file);
    printf("\n%c%c%c%c   |", w, a, v, e);
    // file check
    //printf("\n%c%c%c%c\n", w, a, v, e);
    
    if (w != 'W' || a != 'A' || v != 'V' || e != 'E') {
        printf("\nnot wav32 file (perhaps wav64?)... exiting\n");
        exit(-1);
    }
    
    // start of fmt
    char f = fgetc(fptr);
    char m = fgetc(fptr);
    char t = fgetc(fptr);
    char blank = fgetc(fptr);
    printf("\n%c%c%c%c  ", f, m, t, blank);
    putc(f, new_file);
    putc(m, new_file);
    putc(t, new_file);
    putc(blank, new_file);
    // 4 bytes must be read as little-endian
    char subChunk1[4] = {fgetc(fptr), fgetc(fptr), fgetc(fptr), fgetc(fptr)};
    // chunk size MIGHT be above 2^32
    unsigned int subChunk1Sum = (subChunk1[3] << 8) | subChunk1[2];
    subChunk1Sum = (subChunk1Sum << 8) | subChunk1[1];
    subChunk1Sum = (subChunk1Sum << 8) | subChunk1[0];
    printf(" | SubChunk1Size: %10u", subChunk1Sum); // little
    putc(subChunk1[0], new_file);
    putc(subChunk1[1], new_file);
    putc(subChunk1[2], new_file);
    putc(subChunk1[3], new_file);
    // Audio format
    char audioFormat[2] = {fgetc(fptr), fgetc(fptr)};
    unsigned int audioFormatSum = (audioFormat[0] << 8) | audioFormat[1];
    printf(" | AudioFormat:   %10u\n", audioFormatSum); // TODO: fix
    putc(audioFormat[0], new_file);
    putc(audioFormat[1], new_file);
    // Channel numbers
    char channelNum[2] = {fgetc(fptr), fgetc(fptr)};
    unsigned int channelNumSum = (channelNum[0] << 8) | channelNum[1];
    printf("       | #ofChannels:   %10u | ", channelNumSum);
    putc(channelNum[0], new_file);
    putc(channelNum[1], new_file);
    // Sample rate
    char sampleRate[4] = {fgetc(fptr), fgetc(fptr), fgetc(fptr), fgetc(fptr)};
    unsigned int sampleRateNum = (sampleRate[3] << 8) | sampleRate[2];
    sampleRateNum = sampleRateNum << 8;
    sampleRateNum = sampleRateNum | sampleRate[1];
    sampleRateNum = sampleRateNum << 8;
    sampleRateNum = sampleRateNum | sampleRate[0];
    printf("SampleRate: %d\n", sampleRateNum);
    putc(sampleRate[0], new_file);
    putc(sampleRate[1], new_file);
    putc(sampleRate[2], new_file);
    putc(sampleRate[3], new_file);
    // Byte rate
    char byteRate[4] = {fgetc(fptr), fgetc(fptr), fgetc(fptr), fgetc(fptr)};
    unsigned int byteRateNum = (byteRate[3] << 8) | byteRate[2];
    byteRateNum = byteRateNum << 8;
    byteRateNum = byteRateNum | sampleRate[1];
    byteRateNum = byteRateNum << 8;
    byteRateNum = byteRateNum | byteRate[0];
    printf("       | ByteRate:      %10d | ", byteRateNum);
    putc(byteRate[0], new_file);
    putc(byteRate[1], new_file);
    putc(byteRate[2], new_file);
    putc(byteRate[3], new_file);
    // Block align
    char blockAlign[2] = {fgetc(fptr), fgetc(fptr)};
    unsigned int blockAlignNum = (blockAlign[1] << 8) | blockAlign[0];
    printf("block align: %d\n", blockAlignNum);
    putc(blockAlign[0], new_file);
    putc(blockAlign[1], new_file);
    // Bits per sample
    char bitsPerSample[2] = {fgetc(fptr), fgetc(fptr)};
    unsigned int bitsPerSampleNum = (bitsPerSample[1] << 8) | bitsPerSample[0];
    printf("       | bits per sample: %d\n", bitsPerSampleNum);
    putc(bitsPerSample[0], new_file);
    putc(bitsPerSample[1], new_file);
    // DATA
    char d = fgetc(fptr);
    char a1 = fgetc(fptr);
    char t1 = fgetc(fptr);
    char a2 = fgetc(fptr);

    // This whole thing can be avoided by checking if it's PCM
    if (d != 'd' || a1 !=  'a' || t1 != 't' || a2 != 'a') {
        // we enter this loop to read up to "data" (large subchunk 1 size)
        
        unsigned int extraParamSize = (a1 << 8) | d;
        printf("data did not match xtraParam: %d\n", extraParamSize);
        for (int i = 0; i < extraParamSize; i++) {
            a2 = t1;
            t1 = a1;
            a1 = d;
            d = fgetc(fptr);
            putc(d, new_file);
            putc(a1, new_file);
            putc(t1, new_file);
            putc(a2, new_file);
            if (d == 'd' && a1 == 'a' && t1 == 't' && a2 == 'a') {
                break;
            }
        }
        
    } else {
        // possibly fix (idk if needed)
        
        putc(d, new_file);
        putc(a1, new_file);
        putc(t1, new_file);
        putc(a2, new_file);
        
    }
    //printf("\nPASSED EXTRAPARAMSTUFF\n");
    //printf("%c%c%c%c   |\n", d & '_', a1 & '_', t1 & '_', a2 & '_'); // cool trick to make uppercase
    printf("%x %x %x %x   |\n", d, a1, t1, a2); // cool trick to make uppercase
    /*
    putc(d, new_file);
    putc(a1, new_file);
    putc(t1, new_file);
    putc(a2, new_file);
    */

}

void read_data(FILE *fptr, FILE *new_file) {
    // subchunk diagram: LeftSample, RightSample L1R1 L2R2
    int byte = 0; // arbritary value
    int counter = 0;
    int print_lim = 10;
    while (byte != EOF) {
        if (counter % 2 == 0 && counter < print_lim) {
            printf("\n%d | ", counter);
        }
        // *** this should make a file with only L audio ***
        // write this byte
        byte = fgetc(fptr);
        putc(byte, new_file);
        (counter < print_lim) ? printf("%02x ", byte) : 0;
        if (byte == EOF) {
            // janky fix but will work
            break;
        }
        // make this byte null
        byte = fgetc(fptr);
        putc(byte, new_file);
        (counter < print_lim) ? printf("%02x ", byte) : 0;
        // write this byte
        byte = fgetc(fptr);
        putc(byte, new_file);
        (counter < print_lim) ? printf("%02x ", byte) : 0;
        // make this byte null
        byte = fgetc(fptr);
        putc(byte, new_file);
        (counter < print_lim) ? printf("%02x ", byte) : 0;
        counter++;
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        // Everything goes in here
        FILE *fptr;
        if ((fptr = fopen(argv[1], "r")) != NULL) {
            char* new_fn = (char*) malloc(strlen(argv[1])+3); // + _e\0
            strncpy(new_fn, argv[1], strlen(argv[1]) - 4); // watch null char
            printf(":.. %d %d\n", strlen(argv[1]), strlen(new_fn));
            printf(".%s\n", new_fn);
            *(new_fn+strlen(new_fn)) = '_';
            *(new_fn+strlen(new_fn)) = 'e'; // strlen has increased
            strcpy((new_fn+strlen(new_fn)), (argv[1]+(strlen(argv[1])-4))); // may of made 1 thing too big
            printf(".%s %d\n", new_fn, strlen(new_fn));
            
            FILE *f_to_write = fopen(new_fn, "w");
            if (f_to_write == NULL) {
                printf("idk");
                exit(-1);
            }
            free(new_fn);
            // read n' display header contents
            read_header(fptr, f_to_write);
            read_data(fptr, f_to_write);
            fclose(fptr);
            fclose(f_to_write);
        } else {
            printf("Error opening file");
            return 0;
        }
    } else {
        printf("Wrong number of arguments");
        return 0;
    }
}