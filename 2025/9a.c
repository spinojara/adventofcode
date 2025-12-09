#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

int main(void) {
	time_t t = clock();
	FILE *file = fopen("9.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long maximum = 0;
	unsigned long long (*arr)[2] = NULL;
	size_t size = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		char *endptr;
		arr = realloc(arr, ++size * sizeof(*arr));
		unsigned long long x, y;
		x = arr[size - 1][0] = strtoll(buffer, &endptr, 10);
		y = arr[size - 1][1] = strtoll(endptr + 1, &endptr, 10);
		for (size_t i = 0; i < size - 1; i++) {
			unsigned long long dx = x > arr[i][0] ? x - arr[i][0] : arr[i][0] - x;
			unsigned long long dy = y > arr[i][1] ? y - arr[i][1] : arr[i][1] - y;
			unsigned long long area = (dx + 1) * (dy + 1);
			if (area > maximum)
				maximum = area;
		}
	}
	free(arr);
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", maximum);
	fclose(file);
	return 0;
}
