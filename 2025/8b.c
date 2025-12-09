#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

unsigned long long dist(unsigned long long *arr1, unsigned long long *arr2) {
	unsigned long long r = 0;
	for (int i = 0; i < 3; i++) {
		/* Careful with underflow */
		unsigned long long d = arr1[i] > arr2[i] ? arr1[i] - arr2[i] : arr2[i] - arr1[i];
		r += d * d;
	}
	return r;
}

int cmp(const void *a, const void *b) {
	return *(const int *)b - *(const int *)a;
}

int main(void) {
	time_t t = clock();
	FILE *file = fopen("8.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	unsigned long long (*arr)[4] = NULL;
	size_t size = 0;
	int circuit = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		char *endptr = buffer;
		arr = realloc(arr, ++size * sizeof(*arr));
		arr[size - 1][0] = strtoll(endptr, &endptr, 10);
		arr[size - 1][1] = strtoll(endptr + 1, &endptr, 10);
		arr[size - 1][2] = strtoll(endptr + 1, &endptr, 10);
		arr[size - 1][3] = circuit++;
	}
	unsigned long long *connections = calloc(size * size, sizeof(*connections));
	for (int i = 0; ; i++) {
		unsigned long long min = ULLONG_MAX;
		size_t j0 = 0, k0 = 0;
		for (size_t j = 0; j < size; j++) {
			for (size_t k = j + 1; k < size; k++) {
				if (connections[j + size * k]) {
					printf("%ld, %ld (%lld, %lld), continue\n", j, k, arr[k][3], arr[j][3]);
					continue;
				}

				unsigned long long distance = dist(arr[j], arr[k]);
				if (distance < min) {
					min = distance;
					j0 = j;
					k0 = k;
				}
			}
		}
		unsigned long long int old = arr[j0][3];
		for (size_t j = 0; j < size; j++)
			if (arr[j][3] == old)
				arr[j][3] = arr[k0][3];
		connections[j0 + size * k0] = 1;
		size_t j;
		for (j = 0; j < size; j++)
			if (arr[j][3] != arr[0][3])
				break;
		if (j == size) {
			total = arr[j0][0] * arr[k0][0];
			break;
		}
	}
	int circuits[circuit];
	memset(circuits, 0, circuit * sizeof(*circuits));
	for (size_t j = 0; j < size; j++) {
		circuits[arr[j][3]]++;
	}
	qsort(circuits, circuit, sizeof(*circuits), cmp);
	for (int i = 0; i < circuit && circuits[i]; i++)
		printf("%d\n", circuits[i]);
	free(arr);
	free(connections);
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
