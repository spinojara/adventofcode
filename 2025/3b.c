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
		unsigned long long x = 0;
		size_t max_index = -1;
		for (int j = 0; j < 12; j++) {
			int max_value = 0;
			for (size_t i = max_index + 1; i < strlen(buffer) - 12 + j; i++) {
				if (buffer[i] - '0' > max_value) {
					max_value = buffer[i] - '0';
					max_index = i;
				}
			}
			if (j)
				x = x * 10;
			x += max_value;
		}
		printf("%lld\n", x);
		total += x;
	}
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
