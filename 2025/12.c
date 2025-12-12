#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

struct gift {
	unsigned n;
	unsigned total;
	uint64_t rotation[8][3];
};

void rotate(uint64_t in[3], uint64_t out[3], unsigned rotation) {
	/* Make rotation number 7 the most essential rotation */
	if (rotation != 3)
		rotation ^= 3;

	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
	for (unsigned i = 0; i < 3; i++) {
		if (!((1 << i) & rotation))
			continue;
		switch (i) {
		case 0:;
			uint64_t t = out[0];
			out[0] = out[2];
			out[2] = t;
			break;
		case 1:
			for (int j = 0; j < 3; j++)
				out[j] = ((out[j] & 1) << 2) | (out[j] & 2) | ((out[j] & 4) >> 2);
			break;
		case 2:;
			uint64_t a, b, c;
			a =  (out[0] & 1)       | ((out[1] & 1) << 1) | ((out[2] & 1) << 2);
			b = ((out[0] & 2) >> 1) |  (out[1] & 2)       | ((out[2] & 2) << 1);
			c = ((out[0] & 4) >> 2) | ((out[1] & 4) >> 1) |  (out[2] & 4);
			out[0] = a;
			out[1] = b;
			out[2] = c;
			break;
		}
	}
}

void print_gift(uint64_t rotation[3]) {
	for (int i = 0; i < 3; i++)
		printf("%c%c%c\n", (rotation[i] & 1) ? '#' : '.', (rotation[i] & 2) ? '#' : '.', (rotation[i] & 4) ? '#' : '.');
}

int recurse(uint64_t *board, int x0, int y0, struct gift *gifts, int n_gifts, int *n) {
	int i;
	for (i = 0; i < n_gifts; i++)
		if (n[i])
			break;
	if (i == n_gifts)
		return 1;

	for (i = 0; i < n_gifts; i++) {
		if (!n[i])
			continue;
		n[i]--;
		for (int y = 0; y < y0 - 2; y++) {
			for (int x = 0; x < x0 - 2; x++) {
				for (unsigned r = 0; r < gifts[i].n; r++) {
					int k;
					for (k = 0; k < 3; k++)
						if (board[y + k] & (gifts[i].rotation[r][k] << x))
							break;
					if (k != 3)
						continue;

					for (k = 0; k < 3; k++)
						board[y + k] ^= gifts[i].rotation[r][k] << x;

					if (recurse(board, x0, y0, gifts, n_gifts, n))
						return 1;

					for (k = 0; k < 3; k++)
						board[y + k] ^= gifts[i].rotation[r][k] << x;
				}
			}
		}
		n[i]++;
		return 0;
	}
	return 0;
}

int main(void) {
	time_t t = clock();
	FILE *file = fopen("12.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	struct gift gifts[6] = { 0 };
	int n_gifts = 0;
	int row = 0;

	while (n_gifts < 6) {
		fgets(buffer, sizeof(buffer), file);
		if (buffer[0] == '\n') {
			n_gifts++;
			row = 0;
			continue;
		}

		if (buffer[0] != '.' && buffer[0] != '#')
			continue;

		gifts[n_gifts].rotation[0][row] = (buffer[0] == '#') | ((buffer[1] == '#') << 1) | ((buffer[2] == '#') << 2);
		gifts[n_gifts].n = 1;
		gifts[n_gifts].total += __builtin_popcountll(gifts[n_gifts].rotation[0][row]);

		row++;
	}

	for (int i = 0; i < n_gifts; i++) {
		print_gift(gifts[i].rotation[0]);
		/* We have to start with rotation 7 because it is the most essential rotation which can never be skipped. */
		for (int j = 7; j >= 1; j--) {
			uint64_t rotation[3] = { 0 };
			//printf("\nrotation %d:\n", j);
			rotate(gifts[i].rotation[0], rotation, j);
			//print_gift(rotation);
			unsigned k;
			for (k = 0; k < gifts[i].n; k++)
				if (gifts[i].rotation[k][0] == rotation[0] && gifts[i].rotation[k][1] == rotation[1] && gifts[i].rotation[k][2] == rotation[2])
					break;
			if (k != gifts[i].n) {
				//printf("skipping (%d)\n", k);
				continue;
			}
			memcpy(gifts[i].rotation[gifts[i].n], rotation, 3 * sizeof(uint64_t));
			gifts[i].n++;
		}
		//printf("gift %d has %u rotations and %d squares\n", i, gifts[i].n, gifts[i].total);
		//printf("\n");
	}

	while (fgets(buffer, sizeof(buffer), file)) {
		char *endptr;
		int x = strtoll(buffer, &endptr, 10);
		int y = strtoll(endptr + 1, &endptr, 10);
		if (x < y) {
			int z = x;
			x = y;
			y = z;
		}
		uint64_t board[y];
		memset(board, 0, sizeof(board));
		int n[n_gifts];
		endptr++;
		unsigned long long total_n = 0;
		for (int i = 0; i < n_gifts; i++) {
			n[i] = strtoll(endptr, &endptr, 10);
			total_n += n[i] * gifts[i].total;
		}
		if (total_n > x * y) {
			printf("not even trying this one\n");
			continue;
		}

		int r = recurse(board, x, y, gifts, n_gifts, n);
		total += r;
		printf("recurse returned %d\n", r);
	}

	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
