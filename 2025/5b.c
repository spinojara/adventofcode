#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

unsigned long long max(unsigned long long a, unsigned long long b) {
	return a > b ? a : b;
}

int cmp(const void *ptr1, const void *ptr2) {
	const unsigned long long *arr1 = ptr1;
	const unsigned long long *arr2 = ptr2;
	return arr1[0] > arr2[0] ? 1 : arr1[0] == arr2[0] ? 0 : -1;
}

int main(void) {
	time_t t = clock();
	FILE *file = fopen("5.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	unsigned long long (*arr)[2] = NULL;
	size_t arr_size = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		if (buffer[0] == '\n')
			break;

		arr = realloc(arr, ++arr_size * sizeof(*arr));
		char *endptr;
		arr[arr_size - 1][0] = strtoll(buffer, &endptr, 10);
		arr[arr_size - 1][1] = strtoll(endptr + 1, NULL, 10);
		printf("%lld-%lld\n", arr[arr_size - 1][0], arr[arr_size - 1][1]);
	}
	qsort(arr, arr_size, sizeof(*arr), &cmp);
	unsigned long long previous = 0;
	for (size_t i = 0; i < arr_size; i++) {
		printf("%lld-%lld\n", arr[i][0], arr[i][1]);
		arr[i][0] = max(previous + 1, arr[i][0]);
		if (arr[i][0] <= arr[i][1]) {
			total += arr[i][1] + 1 - arr[i][0];
			previous = arr[i][1];
		}
	}
	free(arr);
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
