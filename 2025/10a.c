#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

int main(void) {
	time_t t = clock();
	FILE *file = fopen("10.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		unsigned long long minimum = ULLONG_MAX;
		unsigned final = 0;
		size_t i;
		for (i = 1; buffer[i] != ']'; i++)
			if (buffer[i] == '#')
				final |= 1 << (i - 1);

		char *endptr = buffer + i;
		unsigned *switches = NULL;
		size_t nswitches = 0;
		while (1) {
			if (endptr[2] != '(')
				break;
			endptr += 3;
			switches = realloc(switches, ++nswitches * sizeof(*switches));
			switches[nswitches - 1] = 0;
			while (1) {
				errno = 0;
				int x = strtoll(endptr, &endptr, 10);
				if (errno)
					break;
				switches[nswitches - 1] |= 1 << x;
				if (*endptr == ')')
					break;
				endptr++;
			}
		}

		for (unsigned k = 0; k < (1u << nswitches); k++) {
			unsigned tries = __builtin_popcount(k);
			if (tries >= minimum)
				continue;
			unsigned test = final;
			for (unsigned j = 0; j < nswitches; j++)
				if (k & (1u << j))
					test ^= switches[j];
			if (test)
				continue;
			minimum = tries;
		}
		total += minimum;
		free(switches);
	}

	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
