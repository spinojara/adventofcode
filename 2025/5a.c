#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

int main(void) {
	time_t t = clock();
	FILE *file = fopen("5.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	int stage = 0;
	unsigned long long (*arr)[2] = NULL;
	size_t arr_size = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		if (buffer[0] == '\n') {
			stage = 1;
			continue;
		}
		if (stage == 0) {
			arr = realloc(arr, ++arr_size * sizeof(*arr));
			char *endptr;
			arr[arr_size - 1][0] = strtoll(buffer, &endptr, 10);
			arr[arr_size - 1][1] = strtoll(endptr + 1, NULL, 10);
			printf("%lld-%lld\n", arr[arr_size - 1][0], arr[arr_size - 1][1]);
		}
		else {
			unsigned long long id = strtoll(buffer, NULL, 10);
			int fresh = 0;
			for (size_t i = 0; i < arr_size && !fresh; i++) {
				if (arr[i][0] <= id && id <= arr[i][1])
					fresh = 1;
			}
			if (fresh)
				total++;
		}
	}
	free(arr);
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
