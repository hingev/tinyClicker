
#include <stdio.h>
#include <stdint.h>

void hexdump (uint8_t *data, size_t size) {
	int i,j;
	for (i=0; i < (size / 8) + 1; ++i) {
		for (j=0; j < 8 && i*8 + j < size; ++j) {
			printf ("%02X ", data[i*8 + j]);
		}
		printf ("\n");
	}
}
