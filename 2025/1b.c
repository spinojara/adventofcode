#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(void) {
	time_t t = clock();
	FILE *file = fopen("1.txt", "r");
	char buffer[16];
	int dial = 50;
	int zeros = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		int offset = (2 * (buffer[0] == 'L') - 1) * strtoll(&buffer[1], NULL, 10);
		int lastdial = dial;
		dial += offset;

		if (lastdial != 0 && dial < 0)
			zeros++;

		if (dial <= -100)
			zeros += (-dial) / 100;
		else if (dial >= 100)
			zeros += dial / 100;
		else if (dial == 0)
			zeros++;

		dial = dial % 100;
		if (dial < 0)
			dial += 100;
	}
	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%d\n", zeros);
	fclose(file);
	return 0;
}
