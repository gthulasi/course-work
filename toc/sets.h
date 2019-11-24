#define MAX_BITS 64

uint64_t CLEAR_LSBIT(uint64_t x)
{
	if (x == 0) return 0;
	return (x & (x - 1));
}

int LSBIT(uint64_t x)
{
	int pos = 0;
	while (pos < 64)
	{
		if (x & (1ULL << pos))
			return pos;
		pos++;
	}
	return 64;
}

/* Assumption sets are mutually exclusive */
int find_set(uint64_t set[], int x)
{
	int i;

	if (x >= MAX_BITS) return 64;

	for (i=0; i<MAX_BITS; i++)
	{
		if (!set[i]) continue;
		else if (set[i] & (1ULL << x)) return i;
	}
	return 64;
}

/* Assumption sets are mutually exclusive 
 * remove x from set[set_no] and add it new set
 */
void split_set(uint64_t set[], int set_no, int x)
{
	int i;

	if (x >= MAX_BITS) return;

	set[set_no] ^= (1ULL << x);

	for (i=0; i<MAX_BITS; i++) {
		if (!set[i]) {
			set[i] = (1ULL << x);
			return;
		}
	}

}
