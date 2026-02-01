#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
int main(int argc, char *argv[]) {
    unsigned char *bytes = NULL;
    size_t length = 0;
    int debug = 0;
    FILE *file = NULL;
    int index = 1;
    if (argc > 1 && strcmp(argv[1], "-debug") == 0) {
        debug = 1;
        index = 2;
    }
    if (argc > index) {
        file = fopen(argv[index], "rb");
        if (!file) {
            perror("Failed to open file");
            return 1;
        }
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);
        bytes = malloc(length);
        if (!bytes) {
            perror("Memory allocation failed");
            fclose(file);
            return 1;
        }
        fread(bytes, 1, length, file);
        fclose(file);
    } else {
        size_t capacity = 131072;
        bytes = malloc(capacity);
        if (!bytes) {
            perror("Memory allocation failed");
            return 1;
        }
        size_t totalRead = 0;
        size_t bytesRead;
        while ((bytesRead = fread(bytes + totalRead, 1, 131072, stdin)) > 0) {
            totalRead += bytesRead;
            if (totalRead + 131072 > capacity) {
                capacity *= 2;
                bytes = realloc(bytes, capacity);
                if (!bytes) {
                    perror("Memory allocation failed");
                    return 1;
                }
            }
        }
        length = totalRead;
    }
    int sum = 0;
    clock_t start = clock();
    if (debug) {
        printf("Starting\n");
    }
size_t i = 0;
for (; i + 255 < length; i += 256) {
    uint32_t x = 0;
    for (int j = 0; j < 256; j+=4) {
	uint32_t i_j = j+i;
	uint32_t s = i_j & 31;
	x ^= bytes[i_j] << s;
	x ^= bytes[i_j+1] << s;
	x ^= bytes[i_j+2] << s;
	x ^= bytes[i_j+3] << s;
    }
    sum ^= x;
}
for (; i < length; i++) {
	uint32_t x = 0;
	x ^= bytes[i] << i & 31;
	sum^=x;
}
    if (debug) {
        printf("Time: %ld ms\n", (clock() - start) * 1000 / CLOCKS_PER_SEC);
    }
    printf("%d\n", sum);
    free(bytes);
    return 0;
}