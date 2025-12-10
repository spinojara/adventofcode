#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

struct simplex {
	int m;
	int n;
	int *var;
	double **a;
	double *b;
	double *x;
	double *c;
	double y;
};

struct node {
	int m;
	int n;
	int k;
	int h;
	double xh;
	double ak;
	double bk;
	double *min;
	double *max;
	double **a;
	double *b;
	double *x;
	double *c;
	double z;

	struct node *next;
};

static const double eps = 1.0e-6;

void free_node(struct node *p, int free_p) {
	free(p->min);
	free(p->max);

	for (int i = 0; i < p->m + 1; i++)
		free(p->a[i]);
	free(p->a);

	free(p->b);
	free(p->x);
	free(p->c);


	if (free_p)
		free(p);
}

double cmpzero(double x) {
	return fabs(x) <= eps;
}

void simplex_print(const struct simplex *s) {
	int m = s->m;
	int n = s->n;
	double **a = s->a;
	double *b = s->b;
	double *c = s->c;
	printf("\n\nbasic : ");
	for (int i = n; i < n + m; i++)
		printf("%d, ", s->var[i]);
	printf("\nnonbasic : ");
	for (int i = 0; i < n; i++)
		printf("%d, ", s->var[i]);
	printf("\n\ny : %lf\n\n", s->y);

	printf("c: ");
	for (int i = 0; i < n; i++)
		printf("%lf, ", c[i]);
	printf("\n\na: ");

	for (int j = 0; j < m; j++) {
		for (int i = 0; i < n; i++)
			printf("%lf, ", a[j][i]);
		printf("\n   ");
	}
	printf("\nb: ");
	for (int i = 0; i < m; i++)
		printf("%lf, ", b[i]);
	printf("\n\n");
}

double xsimplex(int m, int n, double **a, double *b, double *c, double *x, double y, int *var, int h);

int init(struct simplex *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var) {
	int i, k;

	s->m = m;
	s->n = n;
	s->var = var;
	s->a = a;
	s->b = b;
	s->x = x;
	s->c = c;
	s->y = y;
	if (!s->var) {
		s->var = malloc((m + n + 1) * sizeof(*s->var));
		for (i = 0; i < m + n; i++)
			s->var[i] = i;
	}
	for (k = 0, i = 1; i < m; i++)
		if (b[i] < b[k])
			k = i;
	//printf("Smallest b: %d, %lf\n", k, b[k]);
	return k;
}

int select_nonbasic(const struct simplex *s) {
	//printf("Selecting nonbasic var\n");
	int i;
	for (i = 0; i < s->n; i++) {
		if (s->c[i] > eps) {
			//printf("Selected nonbasic %d\n\n", s->var[i]);
			return i;
		}
	}
	return -1;
}

void pivot(struct simplex *s, int row, int col) {
	int m = s->m;
	int n = s->n;
	double **a = s->a;
	double *b = s->b;
	double *c = s->c;
	int i, j, t;

	//simplex_print(s);

	/* Swap basic and nonbasic variable. */
	//printf("Swapping basic %d and nonbasic %d\n", s->var[n + row], s->var[col]);
	t = s->var[col];
	s->var[col] = s->var[n + row];
	s->var[n + row] = t;
	//printf("Adding %lf to y\n", c[col] * b[row] / a[row][col]);

	double ainv = 1.0 / a[row][col];

	s->y += c[col] * b[row] * ainv;

	for (i = 0; i < n; i++)
		if (i != col)
			c[i] -= c[col] * a[row][i] * ainv;
	c[col] = -c[col] * ainv;

	for (i = 0; i < m; i++)
		if (i != row)
			b[i] -= a[i][col] * b[row] * ainv;
	
	for (i = 0; i < m; i++)
		if (i != row)
			for (j = 0; j < n; j++)
				if (j != col)
					a[i][j] -= a[i][col] * a[row][j] * ainv;

	for (i = 0; i < m; i++)
		if (i != row)
			a[i][col] = -a[i][col] * ainv;

	for (i = 0; i < n; i++)
		if (i != col)
			a[row][i] = a[row][i] * ainv;

	b[row] = b[row] * ainv;
	a[row][col] = ainv;

	//simplex_print(s);
}

void prepare(struct simplex *s, int k) {
	int m = s->m;
	int n = s->n;
	int i;
	for (i = m + n; i > n; i--)
		s->var[i] = s->var[i - 1];
	s->var[n] = m + n;
	n++;
	for (i = 0; i < m; i++)
		s->a[i][n - 1] = -1;
	s->x = calloc(m + n, sizeof(*s->x));
	s->c = calloc(n, sizeof(*s->c));

	s->c[n - 1] = -1;
	s->n = n;

	pivot(s, k, n - 1);
}

int initial(struct simplex *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var) {
	int i, j, k;
	double w;
	k = init(s, m, n, a, b, c, x, y, var);
	if (b[k] >= 0) {
		//printf("b >= 0\n");
		return 1;
	}
	prepare(s, k);
	n = s->n;
	//printf("Solving P1\n");
	s->y = xsimplex(m, n, s->a, s->b, s->c, s->x, s->y, s->var, 1);
	//printf("Done solving P1\n");

	for (i = 0; i < m + n; i++) {
		if (s->var[i] == m + n - 1) {
			if(!cmpzero(s->x[i])) {
				free(s->x);
				free(s->c);
				return 0;
			}
			break;
		}
	}
	if (i >= n) {
		//printf("x_{n+m} is basic\n");
		for (j = k = 0; k < n; k++)
			if (fabs(s->a[i - n][k]) > fabs(s->a[i - n][j]))
				j = k;
		pivot(s, i - n, j);
		i = j;
	}
	if (i < n - 1) {
		//printf("x_{n+m} is nonbasic\n");
		//simplex_print(s);
		//printf("Swapping column of x_{n+m} and last column\n");
		k = s->var[i];
		s->var[i] = s->var[n - 1];
		s->var[n - 1] = k;
		/* Shouldn't we also swap c?
		 * No, look 36 lines below this.
		 */
		for (k = 0; k < m; k++) {
			w = s->a[k][n - 1];
			s->a[k][n - 1] = s->a[k][i];
			s->a[k][i] = w;
		}
		//simplex_print(s);
		//printf("Done swapping\n");
	}
	else {
		/* Nothing? */
	}
	free(s->c);
	s->c = c;
	s->y = y;
	for (k = n - 1; k < n + m - 1; k++)
		s->var[k] = s->var[k + 1];

	n = s->n = s->n - 1;
	double *t = calloc(n, sizeof(*t));
	for (k = 0; k < n; k++) {
		for (j = 0; j < n; j++) {
			if (k == s->var[j]) {
				t[j] += s->c[k];
				goto next_k;
			}
		}
		for (j = 0; j < m; j++)
			if (k == s->var[n + j])
				break;
		s->y += s->c[k] * s->b[j];
		for (i = 0; i < n; i++)
			t[i] -= s->c[k] * s->a[j][i];
next_k:;
	}
	for (i = 0; i < n; i++)
		s->c[i] = t[i];

	free(t);
	free(s->x);
	return 1;
}

double xsimplex(int m, int n, double **a, double *b, double *c, double *x, double y, int *var, int h) {
	struct simplex s;
	int i, row, col;
	if (!initial(&s, m, n, a, b, c, x, y, var)) {
		free(s.var);
		/* Use nan(const char *tagp)? */
		return NAN;
	}
	while ((col = select_nonbasic(&s)) >= 0) {
		//printf("Selecting basic to nonbasic %d\n", s.var[col]);
		row = -1;
		for (i = 0; i < m; i++) {
			/* The coefficient of the nonbasic variable has to be greater than 0.
			 * If row < 0 we have no selected row yet so we select this one.
			 * If we have already selected a row we check if this equation makes
			 * the nonbasic variable more restricted.
			 */
			if (a[i][col] > eps && (row < 0 || b[i] / a[i][col] < b[row] / a[row][col]))
				row = i;
		}
		if (row < 0) {
			free(s.var);
			return INFINITY;
		}
		//printf("Selected basic %d to nonbasic %d\n\n", s.var[n + row], s.var[col]);
		pivot(&s, row, col);
	}
	if (!h) {
		for (i = 0; i < n; i++)
			if (s.var[i] < n)
				x[s.var[i]] = 0;
		for (i = 0; i < m; i++)
			if (s.var[n + i] < n)
				x[s.var[n + i]] = s.b[i];
		free(s.var);
	}
	else {
		for (i = 0; i < n; i++)
			x[i] = 0;
		for (i = n; i < n + m; i++)
			x[i] = s.b[i - n];
	}
	/* free(s.var)? */
	return s.y;
}

double simplex(int m, int n, double **a, double *b, double *c, double *x, double y) {
	return xsimplex(m, n, a, b, c, x, y, NULL, 0);
}

struct node *initial_node(int m, int n, double **a, double *b, double *c) {
	struct node *p = malloc(sizeof(*p));

	p->a = malloc((m + 1) * sizeof(*p->a));
	for (int i = 0; i < m + 1; i++)
		p->a[i] = calloc(n + 1, sizeof(**p->a));

	p->b = calloc(m + 1, sizeof(*p->b));
	p->c = calloc(n + 1, sizeof(*p->c));
	p->x = calloc(n + 1, sizeof(*p->x));

	p->min = calloc(n, sizeof(*p->min));
	p->max = calloc(n, sizeof(*p->max));

	p->m = m;
	p->n = n;

	/* Loop over i < m or i < m + 1? */
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			p->a[i][j] = a[i][j];
	for (int i = 0; i < m; i++)
		p->b[i] = b[i];
	for (int j = 0; j < n; j++)
		p->c[j] = c[j];

	/* Other way around? */
	for (int j = 0; j < n; j++) {
		p->min[j] = -INFINITY;
		p->max[j] = INFINITY;
	}

	p->next = NULL;
	return p;
}

struct node *extend(struct node *p, int m, int n, double **a, double *b, double *c, int k, double ak, double bk) {
	struct node *q = malloc(sizeof(*q));
	int i, j;
	q->k = k;
	q->ak = ak;
	q->bk = bk;
	if (ak > 0 && p->max[k] < INFINITY)
		q->m = p->m;
	else if (ak < 0 && p->min[k] > 0)
		q->m = p->m;
	else
		q->m = p->m + 1;
	q->n = p->n;
	q->h = -1;

	q->a = malloc((q->m + 1) * sizeof(*q->a));
	for (i = 0; i < q->m + 1; i++)
		q->a[i] = calloc(q->n + 1, sizeof(**q->a));

	q->b = calloc(q->m + 1, sizeof(*q->b));
	q->c = calloc(q->n + 1, sizeof(*q->c));
	q->x = calloc(q->n + 1, sizeof(*q->x));

	q->min = calloc(n, sizeof(*q->min));
	q->max = calloc(n, sizeof(*q->max));

	memcpy(q->min, p->min, n * sizeof(*q->min));
	memcpy(q->max, p->max, n * sizeof(*q->max));

	for (i = 0; i < m; i++)
		memcpy(q->a[i], a[i], n * sizeof(**q->a));
	memcpy(q->b, b, m * sizeof(*q->b));
	memcpy(q->c, c, n * sizeof(*q->c));

	if (ak > 0) {
		if (q->max[k] == INFINITY || bk < q->max[k])
			q->max[k] = bk;
	}
	else if (q->min[k] == -INFINITY || -bk > q->min[k])
		q->min[k] = -bk;

	for (i = m, j = 0; j < n; j++) {
		if (q->min[j] > -INFINITY) {
			q->a[i][j] = -1;
			q->b[i] = -q->min[j];
			i++;
		}
		if (q->max[j] < INFINITY) {
			q->a[i][j] = 1;
			q->b[i] = q->max[j];
			i++;
		}
	}
	return q;
}

int is_integer(double *xp) {
	double x = *xp;
	double r = round(x);
	if (fabs(r - x) < eps) {
		*xp = r;
		return 1;
	}
	return 0;
}

int integer(struct node *p) {
	for (int j = 0; j < p->n; j++)
		if (!is_integer(&p->x[j]))
			return 0;
	return 1;
}

void bound(struct node *p, struct node *h, double *zp, double *x) {
	if (p->z > *zp) {
		*zp = p->z;
		memcpy(x, p->x, p->n * sizeof(*x));
		/* Delete all nodes q in h with q->z < p->z. */
		for (struct node *q = h; q->next; q = q->next) {
			if (q->next->z < p->z) {
				struct node *r = q->next;
				q->next = q->next->next;
				free_node(r, 1);
				if (!q->next)
					break;
			}
		}
	}
}

int branch(struct node *q, double z) {
	double min, max;
	if (q->z < z)
		return 0;
	for (int h = 0; h < q->n; h++) {
		if (!is_integer(&q->x[h])) {
			if (q->min[h] == -INFINITY)
				min = 0;
			else
				min = q->min[h];
			max = q->max[h];

			if (floor(q->x[h]) < min || ceil(q->x[h]) > max)
				continue;
			q->h = h;
			q->xh = q->x[h];
			/* Free q's variables here? */
			return 1;
		}
	}
	return 0;
}

void succ(struct node *p, struct node *h, int m, int n, double **a, double *b, double *c, int k, double ak, double bk, double *zp, double *x) {
	struct node *q = extend(p, m, n, a, b, c, k, ak, bk);
	if (!q)
		return;
	q->z = simplex(q->m, q->n, q->a, q->b, q->c, q->x, 0);
	if (isfinite(q->z)) {
		if (integer(q))
			bound(q, h, zp, x);
		else if (branch(q, *zp)) {
			/* Add q to h. */
			q->next = NULL;
			for (; h->next; h = h->next);
			h->next = q;
			return;
		}
	}
	/* Delete q. */
	free_node(q, 1);
}

double intopt(int m, int n, double **a, double *b, double *c, double *x) {
	struct node *h = initial_node(m, n, a, b, c);

	/* h = {p} */
	double z = -INFINITY;

	h->z = simplex(h->m, h->n, h->a, h->b, h->c, h->x, 0);
	//printf("Max from simplex: %lf\n", h->z);
	if (integer(h) || !isfinite(h->z)) {
		z = h->z;
		if (integer(h))
			memcpy(x, h->x, n * sizeof(*x));
		/* Delete p. */
		free_node(h, 1);
		return z;
	}
	branch(h, z);
	/* while h contains something. */
	while (h) {
		/* Take p from h. */
		struct node *p = h;
		succ(p, h, m, n, a, b, c, p->h, 1, floor(p->xh), &z, x);
		succ(p, h, m, n, a, b, c, p->h, -1, -ceil(p->xh), &z, x);
		h = h->next;
		/* Delete p. */
		free_node(p, 1);
	}
	if (isinf(z) == -1)
		return NAN;
	return z;
}

int count_chars(const char *str, char c) {
	int count = 0;
	for (const char *ptr = str; *ptr; ptr++)
		if (*ptr == c)
			count++;
	return count;
}

int main(void) {
	time_t t = clock();
	FILE *file = fopen("10.txt", "r");
	char buffer[BUFSIZ];
	unsigned long long total = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		size_t switches = count_chars(buffer, '.') + count_chars(buffer, '#');
		size_t combinations = count_chars(buffer, '(');
		double **a = malloc(2 * switches * sizeof(*a));
		for (size_t s = 0; s < 2 * switches; s++)
			a[s] = calloc(combinations, sizeof(**a));
		double *b = malloc(2 * switches * sizeof(*b));
		double *c = malloc(combinations * sizeof(*c));
		for (size_t i = 0; i < combinations; i++)
			c[i] = -1.0;

		char *endptr = buffer + switches + 1;
		size_t combination = 0;
		while (1) {
			if (endptr[2] != '(')
				break;
			endptr += 3;
			while (1) {
				errno = 0;
				int x = strtoll(endptr, &endptr, 10);
				if (errno)
					break;
				a[x][combination] = 1;
				if (*endptr == ')')
					break;
				endptr++;
			}
			combination++;
		}

		endptr += 3;
		for (size_t s = 0; s < switches; s++) {
			errno = 0;
			int x = strtoll(endptr, &endptr, 10);
			b[s] = x;
			endptr++;
		}
		for (size_t s = switches; s < 2 * switches; s++) {
			for (size_t i = 0; i < combinations; i++)
				a[s][i] = -a[s - switches][i];
			b[s] = -b[s - switches];
		}

#if 0
		for (size_t j = 0; j < switches; j++) {
			for (size_t i = 0; i < combinations ; i++) {
				printf("%d ", (int)a[j][i]);
			}
			printf("= %d\n", (int)b[j]);
			printf("\n");
		}
#endif
		double *x = malloc((combinations + 1) * sizeof(*x));
		int minimum = (int)round(-intopt(2 * switches, combinations, a, b, c, x));
		printf("minimum: %d\n", minimum);

		total += minimum;
		free(x);
		free(c);
		free(b);
		for (size_t s = 0; s < 2 * switches; s++)
			free(a[s]);
		free(a);
	}

	printf("%f\n", (double)(clock() - t) / CLOCKS_PER_SEC);
	printf("%lld\n", total);
	fclose(file);
	return 0;
}
