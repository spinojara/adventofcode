#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

int main(void) {
	time_t t = clock();
	/* See <https://github.com/spinojara/fadjoint> */
	FILE *file = fopen("6.txt.adjoint", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	unsigned long long current = 0;
	char sign = '+';
	while (fgets(buffer, sizeof(buffer), file)) {
		int is_whitespace = 1;
		for (size_t i = 0; buffer[i]; i++)
			if (buffer[i] != ' ' && buffer[i] != '\n')
				is_whitespace = 0;
		if (is_whitespace) {
			total += current;
			continue;
		}

		unsigned long long d = strtoll(buffer, NULL, 10);
		if (strchr(buffer, '+')) {
			current = 0;
			sign = '+';
		}
		if (strchr(buffer, '*')) {
			current = 1;
			sign = '*';
		}
		current = sign == '+' ? (current + d) : (current * d);

	}
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
