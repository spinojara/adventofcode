#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

int main(void) {
	time_t t = clock();
	FILE *file = fopen("6.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	unsigned long long *numbers = NULL;
	char *signs = NULL;
	size_t arr_size = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		if (buffer[0] != '*' && buffer[0] != '+')
			continue;
		for (size_t i = 0; buffer[i]; i++) {
			if (buffer[i] == '*' || buffer[i] == '+') {
				signs = realloc(signs, ++arr_size);
				signs[arr_size - 1] = buffer[i];
			}
		}
	}
	numbers = malloc(arr_size * sizeof(*numbers));
	for (size_t i = 0; i < arr_size; i++) {
		if (signs[i] == '+')
			numbers[i] = 0;
		else
			numbers[i] = 1;
	}
	fseek(file, 0, SEEK_SET);
	while (fgets(buffer, sizeof(buffer), file)) {
		if (buffer[0] == '*' || buffer[0] == '+')
			break;

		char *bufptr = buffer;

		for (size_t i = 0; i < arr_size; i++) {
			char *endptr;
			errno = 0;
			unsigned long long d = strtoll(bufptr, &endptr, 10);
			if (errno)
				break;
			printf("%lld\n", d);
			numbers[i] = signs[i] == '+' ? (numbers[i] + d) : (numbers[i] * d);
			bufptr = endptr;
		}
	}
	for (size_t i = 0; i < arr_size; i++) {
		total += numbers[i];
	}
	free(numbers);
	free(signs);
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
