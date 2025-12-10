#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

int count_chars(const char *str, char c) {
	int count = 0;
	for (const char *ptr = str; *ptr; ptr++)
		if (*ptr == c)
			count++;
	return count;
}

int main(void) {
	time_t t = clock();
	FILE *file = fopen("10.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		size_t switches = count_chars(buffer, '.') + count_chars(buffer, '#');
		size_t combinations = count_chars(buffer, '(');
		int matrix[switches][combinations];
		int result[switches];
		memset(matrix, 0, switches * combinations * sizeof(matrix[0][0]));
		memset(result, 0, switches * sizeof(result[0]));

		char *endptr = buffer + switches + 1;
		size_t combination = 0;
		while (1) {
			if (endptr[2] != '(')
				break;
			endptr += 3;
			while (1) {
				errno = 0;
				int x = strtoll(endptr, &endptr, 10);
				if (errno)
					break;
				matrix[x][combination] = 1;
				if (*endptr == ')')
					break;
				endptr++;
			}
			combination++;
		}

		endptr += 3;
		for (size_t s = 0; s < switches; s++) {
			errno = 0;
			int x = strtoll(endptr, &endptr, 10);
			result[s] = x;
			endptr++;
		}
		for (size_t j = 0; j < switches; j++) {
			for (size_t i = 0; i < combinations ; i++) {
				printf("%d ", matrix[j][i]);
			}
			printf("= %d\n", result[j]);
			printf("\n");
		}
		int minimum;
		int done = 0;
		for (minimum = 0; !done; ) {
			minimum++;
			int chosen[combinations];
			memset(chosen, 0, combinations * sizeof(*chosen));
			chosen[combinations - 1] = minimum;
			while (1) {
				size_t i;
#if 0
				for (i = 0; i < combinations; i++)
					printf("%d ", chosen[i]);
				printf("\n");
#endif
				for (i = 0; i < switches; i++) {
					int sum = 0;
					for (size_t j = 0; j < combinations; j++)
						if (matrix[i][j])
							sum += chosen[j];
					if (sum != result[i])
						break;
				}

				if (i == switches) {
					done = 1;
					break;
				}

				for (i = combinations - 1; i > 0; i--) {
					if (!chosen[i])
						continue;
					chosen[i]--;
					chosen[i - 1]++;
					for (size_t j = i; j < combinations - 1; j++) {
						chosen[combinations - 1] += chosen[j];
						chosen[j] = 0;
					}
					break;
				}
				if (!i)
					break;
			}
		}
		printf("found minimum %d\n", minimum);

		total += minimum;
	}

	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
