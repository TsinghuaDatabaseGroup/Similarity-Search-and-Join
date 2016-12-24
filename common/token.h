#ifndef _TOKEN_H_
#define _TOKEN_H_

extern double THRESHOLD;

struct base_helper
{
	virtual int index_length(int l) = 0;
	virtual int probe_length(int l) = 0;
	virtual int require_overlap(int l1, int l2) = 0;
	virtual double calc_similarity(int l1, int l2, int overlap) = 0;
	virtual int max_possible_length(int l) = 0;
	virtual int min_possible_length(int l) = 0;
};

struct jaccard_helper : public base_helper
{
	virtual int index_length(int l)
	{
		return (1 - 2 * THRESHOLD / (1 + THRESHOLD)) * l + 1 + 1e-6;
	}

	virtual int probe_length(int l)
	{
		return (1 - THRESHOLD) * l + 1 + 1e-6;
	}

	virtual int require_overlap(int l1, int l2)
	{
		return ceil(THRESHOLD / (1 + THRESHOLD) * (double)(l1 + l2) - 1e-6);
	}

	virtual double calc_similarity(int l1, int l2, int overlap)
	{
		return (double)overlap / (double)(l1 + l2 - overlap) + 1e-6;
	}

	virtual int max_possible_length(int l)
	{
		return l / THRESHOLD + 1e-6;
	}

	virtual int min_possible_length(int l)
	{
		return ceil(l * THRESHOLD - 1e-6);
	}
};

struct cosine_helper : public base_helper
{
	virtual int index_length(int l)
	{
		return (1 - THRESHOLD) * l + 1 + 1e-6;
	}

	virtual int probe_length(int l)
	{
		return (1 - THRESHOLD * THRESHOLD) * l + 1 + 1e-6;
	}

	virtual int require_overlap(int l1, int l2)
	{
		return ceil(THRESHOLD * sqrt(l1) * sqrt(l2)  - 1e-6);
	}

	virtual double calc_similarity(int l1, int l2, int overlap)
	{
		return (double)overlap / (sqrt(l1) * sqrt(l2)) + 1e-6;
	}

	virtual int max_possible_length(int l)
	{
		return l / THRESHOLD / THRESHOLD + 1e-6;
	}

	virtual int min_possible_length(int l)
	{
		return ceil(l * THRESHOLD * THRESHOLD - 1e-6);
	}
};

struct dice_helper : public base_helper
{
	virtual int index_length(int l)
	{
		return (1 - THRESHOLD) * l + 1 + 1e-6;
	}

	virtual int probe_length(int l)
	{
		return (1 - THRESHOLD / (2 - THRESHOLD)) * l + 1 + 1e-6;
	}

	virtual int require_overlap(int l1, int l2)
	{
		return ceil((double)(l1 + l2) * THRESHOLD / 2.0 - 1e-6);
	}

	virtual double calc_similarity(int l1, int l2, int overlap)
	{
		return 2 * (double)overlap / (double)(l1 + l2) + 1e-6;
	}

	virtual int max_possible_length(int l)
	{
		return l / THRESHOLD * (2 - THRESHOLD) + 1e-6;
	}

	virtual int min_possible_length(int l)
	{
		return ceil(l * THRESHOLD / (2 - THRESHOLD) - 1e-6);
	}
};

#endif

