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
};

/* out is NULL */
struct node *node_by_name(const char *name, struct node *nodes, size_t n) {
	for (size_t i = 0; i < n; i++)
		if (!strcmp(name, nodes[i].name))
			return &nodes[i];
	return NULL;
}

unsigned long long paths(struct node *node) {
	if (!node)
		return 1;
	unsigned long long sum = 0;
	for (size_t i = 0; i < node->n; i++)
		sum += paths(node->next[i]);
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
		size_t len = strlen(buffer);
		buffer[3] = 0;
		struct node *node = node_by_name(buffer, nodes, n);
		node->n = strlen(buffer + 4) / 4;
		node->next = malloc(node->n * sizeof(*node->next));
		printf("adding '%s' with %zu children", buffer, node->n);
		for (size_t i = 0; i < node->n; i++) {
			buffer[4 * i + 8] = 0;
			printf(" '%s'", buffer + 4 * i + 5);
			node->next[i] = node_by_name(buffer + 4 * i + 5, nodes, n);
		}
		printf("\n");
	}

	total = paths(node_by_name("you", nodes, n));
	for (size_t i = 0; i < n; i++)
		free(nodes[i].next);
	free(nodes);

	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
