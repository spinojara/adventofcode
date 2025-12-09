#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

unsigned long long sizex, sizey;

size_t arr_index(size_t k, size_t size) {
	return k % size;
}

size_t index(unsigned long long x, unsigned long long y) {
	return x + sizex * y;
}

int main(void) {
	time_t t = clock();
	FILE *file = fopen("9.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long maximum = 0;
	unsigned long long (*arr)[2] = NULL;
	unsigned long long (*path)[2] = NULL;
	size_t size = 0;
	size_t path_size = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		char *endptr;
		arr = realloc(arr, ++size * sizeof(*arr));
		arr[size - 1][0] = strtoll(buffer, &endptr, 10) + 1;
		arr[size - 1][1] = strtoll(endptr + 1, &endptr, 10) + 1;
	}

	for (size_t i = 0; i < size; i++) {
		size_t next = (i + 1) % size;
		unsigned long long x0 = arr[i][0];
		unsigned long long y0 = arr[i][1];
		unsigned long long x1 = arr[next][0];
		unsigned long long y1 = arr[next][1];

		if (x0 == x1) {
			for (unsigned long long y = y0; y != y1; y += (y1 > y0) - (y0 > y1)) {
				path = realloc(path, ++path_size * sizeof(*path));
				path[path_size - 1][0] = x0;
				path[path_size - 1][1] = y;
			}
		}
		else {
			for (unsigned long long x = x0; x != x1; x += (x1 > x0) - (x0 > x1)) {
				path = realloc(path, ++path_size * sizeof(*path));
				path[path_size - 1][0] = x;
				path[path_size - 1][1] = y0;
			}
		}
	}

	for (size_t i = 0; i < size; i++) {
		for (size_t j = i + 1; j < size; j++) {
			unsigned long long x0 = arr[j][0] > arr[i][0] ? arr[i][0] : arr[j][0];
			unsigned long long y0 = arr[j][1] > arr[i][1] ? arr[i][1] : arr[j][1];
			unsigned long long x1 = arr[j][0] > arr[i][0] ? arr[j][0] : arr[i][0];
			unsigned long long y1 = arr[j][1] > arr[i][1] ? arr[j][1] : arr[i][1];
			unsigned long long dx = x1 - x0 + 1;
			unsigned long long dy = y1 - y0 + 1;
			unsigned long long area = dx * dy;
			if (maximum >= area)
				continue;

			size_t k;
			for (k = 0; k < path_size; k++) {
				if (x0 < path[k][0] && path[k][0] < x1 && y0 < path[k][1] && path[k][1] < y1)
					break;
			}
			if (k != path_size)
				continue;
			maximum = area;
			printf("max is now %lld\n", maximum);
		}
	}

	free(arr);
	free(path);
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", maximum);
	fclose(file);
	return 0;
}
