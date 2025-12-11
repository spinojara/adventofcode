#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

struct node {
	char name[4];
	size_t n;
	struct node **next;
	int init[4];
	unsigned long long sum[4];
};

/* out is NULL */
struct node *node_by_name(const char *name, struct node *nodes, size_t n) {
	for (size_t i = 0; i < n; i++)
		if (!strcmp(name, nodes[i].name))
			return &nodes[i];
	return NULL;
}

unsigned long long paths(struct node *node, int fft, int dac) {
	if (!node)
		return fft && dac;
	if (!strcmp(node->name, "fft"))
		fft = 1;
	else if (!strcmp(node->name, "dac"))
		dac = 1;
	int index = fft | (dac << 1);
	unsigned long long sum = 0;
	for (size_t i = 0; i < node->n; i++) {
		if (node->next[i] && node->next[i]->init[index])
			sum += node->next[i]->sum[index];
		else
			sum += paths(node->next[i], fft, dac);
	}
	node->init[index] = 1;
	node->sum[index] = sum;
	return sum;
}

int main(void) {
	time_t t = clock();
	FILE *file = fopen("11.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	struct node *nodes = NULL;
	size_t n = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		buffer[3] = 0;
		nodes = realloc(nodes, ++n * sizeof(*nodes));
		strcpy(nodes[n - 1].name, buffer);
	}
	fseek(file, 0, SEEK_SET);
	while (fgets(buffer, sizeof(buffer), file)) {
		buffer[3] = 0;
		struct node *node = node_by_name(buffer, nodes, n);
		node->n = strlen(buffer + 4) / 4;
		node->next = malloc(node->n * sizeof(*node->next));
		for (int i = 0; i < 4; i++)
			node->init[i] = 0;
		printf("adding '%s' with %zu children", buffer, node->n);
		for (size_t i = 0; i < node->n; i++) {
			buffer[4 * i + 8] = 0;
			printf(" '%s'", buffer + 4 * i + 5);
			node->next[i] = node_by_name(buffer + 4 * i + 5, nodes, n);
		}
		printf("\n");
	}

	total = paths(node_by_name("svr", nodes, n), 0, 0);
	for (size_t i = 0; i < n; i++)
		free(nodes[i].next);
	free(nodes);

	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
