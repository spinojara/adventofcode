#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

int main(void) {
	time_t t = clock();
	FILE *file = fopen("4.txt", "r");
	char buffer[BUFSIZ];
	int total = 0;
	int lines = 0;
	int cols = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		cols = strlen(buffer) - 1;
		lines++;
	}
	lines += 2;
	cols += 2;
	char arr[lines][cols];
	memset(arr, '.', lines * cols);
	int line = 1;
	fseek(file, 0, SEEK_SET);
	while (fgets(buffer, sizeof(buffer), file)) {
		memcpy(&arr[line][1], buffer, cols - 2);
		line++;
	}
	for (line = 1; line < lines - 1; line++) {
		for (int col = 1; col < cols - 1; col++) {
			if (arr[line][col] != '@')
				continue;
			int count = 0;
			for (int i = -1; i <= 1; i++)
				for (int j = -1; j <= 1; j++)
					if (arr[line + i][col + j] == '@')
						count++;
			if (count <= 4)
				total++;
		}
	}
	for (int i = 0; i < lines; i++) {
		for (int j = 0; j < cols; j++) {
			printf("%c", arr[i][j]);
		}
		printf("\n");
	}

	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%d\n", total);
	fclose(file);
	return 0;
}
