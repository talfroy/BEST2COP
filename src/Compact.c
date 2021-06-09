#include "Compact.h"

// Get the total number of paths for each destination
int *get_nb_paths_per_dest(Dict_t **dist, int nbNodes, int iter)
{
	int *nb_paths = calloc(nbNodes, sizeof(int));
	for (int i = 0; i < nbNodes; i++)
	{
		for (int k = 0; k < iter; k++)
		{
			for (int j = 0; j < dist[k][i].size; j++)
			{
				if (dist[k][i].paths[j] != INF)
				{
					nb_paths[i] += 1;
				}
			}
		}
	}
	return nb_paths;
}

// Get the number of path for a given delay
// e.g., there are 10 paths with a delay X
int **get_nb_paths_per_delay(Dict_t **dist, int nbNodes, int iter)
{
	int **nb_paths = calloc(nbNodes, sizeof(int *));
	for (int i = 0; i < nbNodes; i++)
	{
		nb_paths[i] = calloc(1000, sizeof(int));
	}
	for (int i = 0; i < nbNodes; i++)
	{
		for (int k = 0; k < iter; k++)
		{
			for (int j = 0; j < dist[k][i].size; j++)
			{
				if (dist[k][i].paths[j] != INF)
				{
					nb_paths[i][j] += 1;
				}
			}
		}
	}
	return nb_paths;
}

static int **get_nb_paths_per_segments(Dict_t **dist, int nbNodes, int iter)
{
	int **nb_paths = calloc(nbNodes, sizeof(int *));
	for (int i = 0; i < nbNodes; i++)
	{
		nb_paths[i] = calloc(iter, sizeof(int));
	}
	for (int i = 0; i < nbNodes; i++)
	{
		for (int k = 0; k < iter; k++)
		{
			for (int j = 0; j < dist[k][i].size; j++)
			{
				if (dist[k][i].paths[j] != INF)
				{
					nb_paths[i][k] += 1;
				}
			}
		}
	}
	return nb_paths;
}

// Print the 1D version on the compact array
void print_compact_array_1D(path **compact_pf)
{
	int i = 0;
	while (compact_pf[i] != NULL)
	{
		int j = 0;
		while (compact_pf[i][j].cost != -1)
		{
			printf("Nodes %d : %d %d %d\n", i, compact_pf[i][j].delay, compact_pf[i][j].sl.size, compact_pf[i][j].cost);
			j++;
		}
		i++;
	}
}

// Print the 2D version of the compact array, index on the number of segments
void print_compact_array_2D(compact_front *compact_pf)
{
	int node = 0;
	while (node < compact_pf->nbNodes)
	{
		int seg = 0;
		while (compact_pf->paths[node][seg])
		{
			int delay = 0;
			while (compact_pf->paths[node][seg][delay].cost != -1)
			{
				int c = compact_pf->paths[node][seg][delay].cost;
				int d = compact_pf->paths[node][seg][delay].delay;
				int nb_seg = compact_pf->paths[node][seg][delay].sl.size;
				printf("To %d : (%d; %d; %d)\n", node, nb_seg, d, c);
				delay++;
			}
			seg++;
		}
		node++;
	}
}

// take the pfront and put it in an array, indexed first on the destination node,
// than on the number of segments.
// In other words, it lists, for a given node, all paths using a given
// number of segments
compact_front* compact_to_array_2D(Pfront_t **pf, Dict_t **dist, int iter, int nbNodes, struct segment_list ***sl)
{
	compact_front *cf = calloc(1, sizeof(compact_front));
	path ***compact_pf;
	UNUSED(pf);
	compact_pf = calloc(nbNodes, sizeof(path **));
	//mark end of array
	//compact_pf[nbNodes] = NULL;

	int **nb_paths_per_segments = get_nb_paths_per_segments(dist, nbNodes, iter);

	for (int i = 0; i < nbNodes; i++)
	{
		compact_pf[i] = calloc(iter + 2, sizeof(path *));
		compact_pf[i][iter + 1] = NULL;
		for (int seg = 0; seg < iter; seg++)
		{
			// get nb of paths composed of seg segments
			int nb_paths = nb_paths_per_segments[i][seg];
			compact_pf[i][seg] = calloc(nb_paths + 1, sizeof(path));
			path end = {.cost = -1};
			int index = 0;
			for (int delay = 0; delay < dist[seg][i].size; delay++)
			{
				if (dist[seg][i].paths[delay] != INF)
				{
					struct segment_list sl2 = sl[seg][i][delay];
					path p = {.cost = dist[seg][i].paths[delay], .delay = delay, .sl = sl2};
					compact_pf[i][seg][index++] = p;
				}
			}
			// end of sub-array
			compact_pf[i][seg][index] = end;
		}
	}
	cf->nbNodes = nbNodes;
	cf->paths = compact_pf;
	return cf;
}

// Put all paths to a given node in an array
path **compact_to_array_1D(Dict_t **dist, int *nb_paths, int iter, int nbNodes, struct segment_list ***sl)
{
	path **compact_pf;
	compact_pf = calloc(nbNodes + 1, sizeof(path *));
	for (int i = 0; i < nbNodes; i++)
	{
		compact_pf[i] = calloc(nb_paths[i] + 1, sizeof(path));
		int index = 0;
		for (int seg = 0; seg < iter; seg++)
		{
			for (int delay = 0; delay < dist[seg][i].size; delay++)
			{
				if (dist[seg][i].paths[delay] != INF)
				{
					struct segment_list sl2 = sl[seg][i][delay];
					path p = {.cost = dist[seg][i].paths[delay], .delay = delay, .sl = sl2};
					compact_pf[i][index] = p;
					index++;
				}
			}
		}
		path end = {.cost = -1};
		compact_pf[i][nb_paths[i]] = end;
	}
	compact_pf[nbNodes] = NULL;
	return compact_pf;
}

struct segment_list merge_and_correct_sl(struct segment_list sl1, struct segment_list sl2)
{
	struct segment_list sl3;

	// TODO: First check if we can remove ABR from sl1

	sl3.size = sl1.size + sl2.size;
	// sl1 -> sl3
	int i;
	for (i = 0; i < sl1.size; i++)
	{
		sl3.seg[i] = sl1.seg[i];
		sl3.seg_type[i] = sl1.seg_type[i];
	}
	// sl2 -> sl3
	for (int j = 0; j < sl2.size; j++)
	{
		sl3.seg[j + i] = sl2.seg[j];
		sl3.seg_type[j + i] = sl2.seg_type[j];
	}

	return sl3;
}

Dict_seglist_t **cart(compact_front *pf1, compact_front *pf2, int c1, int ABR)
{
	// pf1 = dist to ABR
	Dict_seglist_t **pf3 = NULL;
	pf3 = calloc(11, sizeof(Dict_t*));
	ASSERT(pf3, NULL);

	for (int i = 0; i <= 10; i++)
	{
		pf3[i] = malloc(pf2->nbNodes * sizeof(Dict_t));
		ASSERT(pf3[i], NULL);

		for (int j = 0; j < pf2->nbNodes; j++)
		{
			Dict_seglist_init(pf3[i] + j, 1000);
		}
	}

	// all nb seg to ABR (s1_index = nbseg I thing)
	for (int s1_index = 0; pf1->paths[ABR][s1_index]; s1_index++)
	{
		//printf("s1_index = %d\n", s1_index);
		// all delay to ABR
		for (int d1_index = 0; pf1->paths[ABR][s1_index][d1_index].cost != -1; d1_index++)
		{
			int delay1 = pf1->paths[ABR][s1_index][d1_index].delay;
			//printf("Delay : %d\n", delay1);

			// all other nodes
			for (int out_node = 0; out_node < pf2->nbNodes; out_node++)
			{
				// all nb seg to other node
				for (int s2_index = 0; pf2->paths[out_node][s2_index]; s2_index++)
				{
					// break if above MSD
					if (s1_index + s2_index > 11)
					{
						break;
					}
					// all delay to other nodes
					for (int d2_index = 0; pf2->paths[out_node][s2_index][d2_index].cost != -1; d2_index++)
					{
						int delay2 = pf2->paths[out_node][s2_index][d2_index].delay;
						//printf("Delay 2 : %d\n", delay2);
						// break if above delay constraint
						if (delay1 + delay2 > c1)
						{
							break;
						}
						struct segment_list sl3 =
							merge_and_correct_sl(pf1->paths[ABR][s1_index][d1_index].sl,
												 pf2->paths[out_node][s2_index][d2_index].sl);

						if (sl3.size > 10)
						{
							continue;
						}
						int cost3 = pf1->paths[ABR][s1_index][d1_index].cost + pf2->paths[out_node][s2_index][d2_index].cost;
						int delay3 = delay1 + delay2;
						printf("To %d: %d, %d, %d\n", out_node, sl3.size, delay3, cost3);
						Dict_seglist_add(&pf3[sl3.size][out_node], delay3, cost3, sl3);
					}
				}
			}
		}
	}
	return pf3;
}
