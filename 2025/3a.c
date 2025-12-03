#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

int main(void) {
	time_t t = clock();
	FILE *file = fopen("3.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		int max_value = 0;
		size_t max_index = -1;
		for (size_t i = 0; i < strlen(buffer) - 2; i++) {
			if (buffer[i] - '0' > max_value) {
				max_value = buffer[i] - '0';
				max_index = i;
			}
		}
		int max_right_value = 0;
		for (size_t i = max_index + 1; i < strlen(buffer) - 1; i++)
			if (buffer[i] - '0' > max_right_value)
				max_right_value = buffer[i] - '0';

		printf("%d\n", 10 * max_value + max_right_value);
		total += 10 * max_value + max_right_value;
	}
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
