#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

int main(void) {
	time_t t = clock();
	FILE *file = fopen("7.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	unsigned long long *beams = NULL;
	unsigned long long *beamsold = NULL;
	size_t size = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		if (!beams) {
			size = strlen(buffer) - 1;
			beams = calloc(size, sizeof(*beams));
			beamsold = calloc(size, sizeof(*beamsold));
		}

		memset(beams, 0, size * sizeof(*beams));
		for (size_t i = 0; i < strlen(buffer) - 1; i++) {
			if (buffer[i] == 'S') {
				beams[i] = 1;
			}
			else if (buffer[i] == '^') {
				if (beamsold[i]) {
					/* Lazily not checking out of bound because it's not in the input. */
					beams[i - 1] = beams[i + 1] = beamsold[i];
					total++;
				}
			}
			else {
				beams[i] = beams[i] || beamsold[i];
			}
		}
		memcpy(beamsold, beams, size * sizeof(*beams));
	}
	free(beams);
	free(beamsold);
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
