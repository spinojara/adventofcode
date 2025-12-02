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
				if (strlen(str) % 2)
					continue;
				size_t len = strlen(str) / 2;
				if (!strncmp(str, str + len, len))
					total += x;
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
