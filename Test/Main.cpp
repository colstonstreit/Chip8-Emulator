#include <stdio.h>

#include "Main.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))

int main(int argc, char** argv) {
	printf("Num arguments: %d\n", argc);
	for (int i = 1; i < argc; i++) {
		printf("Argument %d: %s\n", i, argv[i]);
	}

	Data d1;
	Data d2 = d1;
	d1.s = "Bruh";
	printf("D1: %d %d %f %s\n", d1.x, d1.y, d1.d, d1.s);
	printf("D2: %d %d %f %s\n", d2.x, d2.y, d2.d, d2.s);
} /* main() */