#include <stdio.h> // for printf
#include <stdlib.h>
#include <string.h>

void read_header(FILE *fptr) {
    printf("Should be RIFF: ");
    for (int i = 0; i < 4; i++) {
        // R I F F
        printf("%c", (char) fgetc(fptr));
    }
    printf(" | ChunkSize: ");
    int chunkSum = 0;
    for (int k = 0; k < 4; k++) {
        // should this be a while loop; is chunksize variable?
        chunkSum += fgetc(fptr);
    }
    printf("%d | \nShould be WAVE: ", chunkSum);
    for (int l = 0; l < 4; l++) {
        // W A V E
        printf("%c", (char) fgetc(fptr));
    }

    // start of fmt
    printf(" | Should be fmt[]: ");
    for (int i = 0; i < 4; i++) {
        // fmt[]
        printf("%c", (char) fgetc(fptr));
    }
    printf("\nSubChunk1Size: ");
    for (int i = 0; i < 4; i++) {
        // R I F F
        printf("%x ", fgetc(fptr));
    }
    printf(" | AudioFormat: %x %x", fgetc(fptr), fgetc(fptr));
    printf("\n#ofChannels: %x %x | ", fgetc(fptr), fgetc(fptr));
    printf("SampleRate: ");
    for (int i = 0; i < 4; i++) {
        printf("%x", fgetc(fptr));
    }
    printf("\n");
    
    printf("ByteRate: ");
    for (int i = 0; i < 4; i++) {
        printf("%x", fgetc(fptr));
    }
    printf("\n");
    printf("block align: %x %x\n", fgetc(fptr), fgetc(fptr));
    printf("bits per sample: %x %x\n", fgetc(fptr), fgetc(fptr));
    printf("d a t a: ");
    for (int i = 0; i < 4; i++) {
        printf("%c", (char) fgetc(fptr));
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        // Everything goes in here
        FILE *fptr;
        if ((fptr = fopen(argv[1], "r")) != NULL) {
            // read n' display header contents
            read_header(fptr);
            char* new_fn = (char*) malloc(strlen(argv[1])+3); // + _e\0
            //printf("\n>strlen %lu  sizeof argv[1] %lu\n", strlen(argv[1]), sizeof(argv[1]));
            strncpy(new_fn, argv[1], strlen(argv[1]) - 4); // watch null char
            printf(":.. %d %d\n", strlen(argv[1]), strlen(new_fn));
            printf(".%s\n", new_fn);
            *(new_fn+strlen(new_fn)) = '_';
            *(new_fn+strlen(new_fn)) = 'e'; // strlen has increased
            // destination is end of new fn
            // src is
            strcpy((new_fn+strlen(new_fn)), (argv[1]+(strlen(argv[1])-4))); // may of made 1 thing too big
            printf(".%s %d\n", new_fn, strlen(new_fn));
            
            //FILE *f_to_write = fopen(argv[1]+"_e", "r");
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
                (counter < print_lim) ? printf("%02x ", byte) : 0;
                if (byte == EOF) {
                    // janky fix but will work
                    break;
                }
                // make this byte null
                byte = fgetc(fptr);
                (counter < print_lim) ? printf("%02x ", byte) : 0;
                // write this byte
                byte = fgetc(fptr);
                (counter < print_lim) ? printf("%02x ", byte) : 0;
                // make this byte null
                byte = fgetc(fptr);
                (counter < print_lim) ? printf("%02x ", byte) : 0;
                counter++;
            }

            fclose(fptr);
        } else {
            printf("Error opening file");
            return 0;
        }
    } else {
        printf("Wrong number of arguments");
        return 0;
    }
}