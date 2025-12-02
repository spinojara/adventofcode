#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

int main(void) {
	time_t t = clock();
	FILE *file = fopen("2.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		long first, last;
		char *endptr = buffer;
		while (1) {
			first = strtoll(endptr, &endptr, 10);
			last = strtoll(endptr + 1, &endptr, 10);

			for (long x = first; x <= last; x++) {
				char str[BUFSIZ];
				sprintf(str, "%ld", x);
				int bad_id = 0;
				for (int repeats = 2; repeats <= strlen(str); repeats++) {
					if (strlen(str) % repeats)
						continue;
					size_t len = strlen(str) / repeats;
					int all_equal = 1;
					for (int i = 1; i < repeats && all_equal; i++)
						if (strncmp(str, str + len * i, len))
							all_equal = 0;
					if (all_equal) {
						bad_id = 1;
						break;
					}
				}
				if (bad_id) {
					total += x;
					printf("bad: %ld\n", x);
				}
			}
			endptr++;
			if (!*endptr || *endptr == '\n')
				break;
		}
	}
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
