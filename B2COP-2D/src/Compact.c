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
	//int tot = 0;
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
			//printf("Dist[%d][%d].nb_add = %d\n", k, i, dist[k][i].nb_add);
			//nb_paths[i][k] = dist[k][i].nb_add;
			//tot += nb_paths[i][k];
		}
	}

	//RESULTS("There are %d paths for %d nodes\n", tot, nbNodes);

	return nb_paths;
}

static int **get_nb_paths_per_segments_list(Dict_seglist_t **dist, int nbNodes, int iter)
{
	int **nb_paths = calloc(nbNodes, sizeof(int *));
	for (int i = 0; i < nbNodes; i++)
	{
		nb_paths[i] = calloc(iter, sizeof(int));
	}
	//int tot = 0;
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
			//printf("Dist[%d][%d].nb_add = %d\n", k, i, dist[k][i].nb_add);
			//nb_paths[i][k] = dist[k][i].nb_add;
			//tot += nb_paths[i][k];
		}
	}

	//RESULTS("There are %d paths for %d nodes\n", tot, nbNodes);

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

void Compact_free(compact_front *cp)
{
	for (int i = 0; i < cp->nbNodes; i++)
	{
		for (int j = 0; j < cp->iter; j++)
		{
			free(cp->paths[i][j]);
		}
		free(cp->paths[i]);
		free(cp->nb_path[i]);
	}
	free(cp->paths);
	free(cp->nb_path);
	free(cp);
}

// take the pfront and put it in an array, indexed first on the destination node,
// than on the number of segments.
// In other words, it lists, for a given node, all paths using a given
// number of segments
compact_front *compact_to_array_2D(Pfront_t **pf, Dict_t **dist, int iter, int nbNodes, struct segment_list ***sl)
{
	compact_front *cf = calloc(1, sizeof(compact_front));
	path ***compact_pf;
	UNUSED(pf);
	compact_pf = calloc(nbNodes, sizeof(path **));

	//mark end of array

	int **nb_paths_per_segments = get_nb_paths_per_segments(dist, nbNodes, iter);
	cf->nb_path = nb_paths_per_segments;
	for (int i = 0; i < nbNodes; i++)
	{
		compact_pf[i] = calloc((iter + 2), sizeof(path *));
		compact_pf[i][iter + 1] = NULL;
		for (int seg = 0; seg < iter; seg++)
		{
			// get nb of paths composed of seg segments
			int nb_paths = nb_paths_per_segments[i][seg];
			compact_pf[i][seg] = calloc((nb_paths + 1), sizeof(path));
			path end = {.cost = -1};
			int index = 0;
			for (int delay = 0; delay < dist[seg][i].size ; delay++)
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
	cf->iter = iter;
	cf->paths = compact_pf;

	return cf;
}

compact_front *dict_seglist_to_compact(Pfront_t **pf, Dict_seglist_t **dist, int iter, int nbNodes)
{
	compact_front *cf = calloc(1, sizeof(compact_front));
	path ***compact_pf;
	UNUSED(pf);
	compact_pf = calloc(nbNodes, sizeof(path **));

	//mark end of array
	//compact_pf[nbNodes] = NULL;

	int **nb_paths_per_segments = get_nb_paths_per_segments_list(dist, nbNodes, iter);
	cf->nb_path = nb_paths_per_segments;
	for (int i = 0; i < nbNodes; i++)
	{
		compact_pf[i] = calloc((iter + 2), sizeof(path *));
		compact_pf[i][iter + 1] = NULL;
		for (int seg = 0; seg < iter; seg++)
		{
			// get nb of paths composed of seg segments
			int nb_paths = nb_paths_per_segments[i][seg];
			compact_pf[i][seg] = calloc((nb_paths + 1), sizeof(path));
			path end = {.cost = -1};
			int index = 0;
			for (int delay = 0; delay < dist[seg][i].size; delay++)
			{
				if (dist[seg][i].paths[delay] != INF)
				{
					struct segment_list sl2 = dist[seg][i].seg_list[delay];
					path p = {.cost = dist[seg][i].paths[delay], .delay = delay, .sl = sl2};
					compact_pf[i][seg][index++] = p;
				}
			}
			// end of sub-array
			compact_pf[i][seg][index] = end;
		}
	}

	cf->nbNodes = nbNodes;
	cf->iter = iter;
	cf->paths = compact_pf;

	for (int i = 0; i < nbNodes; i++)
	{
		free(nb_paths_per_segments[i]);
	}

	free(nb_paths_per_segments);
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

static struct segment_list merge_and_correct_sl(struct segment_list sl1, struct segment_list sl2,
												compact_front *pf_area_abr1, compact_front *pf_area_abr2, SrGraph_t *sr_bb,
												int other_abr, Topology_t *topo_bb, Topology_t *topo_area, int src)
{
	UNUSED(topo_bb);
	UNUSED(topo_area);
	struct segment_list sl3;
	memset(&sl3, 0, sizeof(struct segment_list));
	sl3.size = sl1.size + sl2.size;
	int i = 0;
	for (i = sl1.size - 1; i >= 0; i--)
	{
		sl3.seg[sl1.size - i - 1] = sl1.seg[i];
		sl3.seg_type[sl1.size - i - 1] = sl1.seg_type[i];
	}
	sl3.abr_index = sl1.size - 1;
	// sl2 -> sl3
	for (int j = sl2.size - 1; j >= 0; j--)
	{
		sl3.seg[j + (sl1.size)] = sl2.seg[sl2.size - j - 1];
		sl3.seg_type[j + (sl1.size)] = sl2.seg_type[sl2.size - j - 1];
	}

	if ((sl3.seg_type[sl1.size - i - 1] == ADJACENCY_SEGMENT || sl3.seg_type[sl1.size - i] == ADJACENCY_SEGMENT) || sl3.size < 2 || !sl2.size)
	{
		return sl3;
	}

	short abr1 = sl3.seg[sl1.size - 1];
	short af_abr = sl3.seg[sl1.size];
	short bf_abr = 0;
	if (sl1.size < 2)
	{
		bf_abr = src;
	}
	else
	{
		bf_abr = sl3.seg[sl1.size - 2];
	}

	int cost_bf_abr1 = sr_bb->m2dists[bf_abr][abr1];
	int delay_bf_abr1 = sr_bb->m1dists[bf_abr][abr1];

	int cost_bf_abr2 = sr_bb->m2dists[bf_abr][other_abr];
	int delay_bf_abr2 = sr_bb->m1dists[bf_abr][other_abr];

	my_m1 cost_af_abr1 = pf_area_abr1->paths[af_abr][1][pf_area_abr1->nb_path[af_abr][1] - 1].cost;
	my_m2 delay_af_abr1 = pf_area_abr1->paths[af_abr][1][pf_area_abr1->nb_path[af_abr][1] - 1].delay;

	my_m1 cost_af_abr2 = pf_area_abr2->paths[af_abr][1][pf_area_abr2->nb_path[af_abr][1] - 1].cost;
	my_m2 delay_af_abr2 = pf_area_abr2->paths[af_abr][1][pf_area_abr2->nb_path[af_abr][1] - 1].delay;

	int cost_via_abr1 = cost_bf_abr1 + cost_af_abr1;
	int cost_via_abr2 = cost_bf_abr2 + cost_af_abr2;
	int delay_via_abr1 = delay_bf_abr1 + delay_af_abr1;
	int delay_via_abr2 = delay_bf_abr2 + delay_af_abr2;

	if ((cost_via_abr1 < cost_via_abr2) || (cost_via_abr1 == cost_via_abr2 && delay_via_abr1 == delay_via_abr2))
	{
		for (int i = sl3.abr_index; i < sl3.size - 1; i++)
		{
			sl3.seg[i] = sl3.seg[i + 1];
			sl3.seg_type[i] = sl3.seg_type[i + 1];
		}
		sl3.size--;
		sl3.abr_index--;
	}

	return sl3;
}

Dict_seglist_t **cart(compact_front *pf1, compact_front *pf2, compact_front *pf2bis,
					  int c1, int ABR, int other_ABR, SrGraph_t *sr_bb, Topology_t *topo_bb,
					  Topology_t *topo_area, int src, bool cop)
{
	// pf1 = dist to ABR
	Dict_seglist_t **pf3 = NULL;
	int maxIter = SEG_MAX;
	if (cop)
	{
		maxIter = 10 * SEG_MAX;
	}
	pf3 = calloc(maxIter + 1, sizeof(Dict_t *));
	ASSERT(pf3, NULL, maxIter + 1);

	for (int i = 0; i <= maxIter; i++)
	{
		pf3[i] = malloc(pf2->nbNodes * sizeof(Dict_t));
		ASSERT(pf3[i], NULL, pf2->nbNodes);

		for (int j = 0; j < pf2->nbNodes; j++)
		{
			Dict_seglist_init(pf3[i] + j, 1000);
		}
	}

	#pragma omp parallel for schedule(dynamic)
	for (int out_node = 0; out_node < pf2->nbNodes; out_node++)
	{

		// all nb seg to ABR (s1_index = nbseg I thing)
		for (int s1_index = 0; pf1->paths[ABR][s1_index]; s1_index++)
		{
			//printf("s1_index = %d\n", s1_index);
			// all delay to ABR
			for (int d1_index = 0; pf1->paths[ABR][s1_index][d1_index].cost != -1; d1_index++)
			{
				int delay1 = pf1->paths[ABR][s1_index][d1_index].delay;

				// all other nodes

				// all nb seg to other node
				for (int s2_index = 0; pf2->paths[out_node][s2_index]; s2_index++)
				{
					// break if above MSD
					if ((s1_index + s2_index > 11) && !cop)
					{
						break;
					}
					// all delay to other nodes
					for (int d2_index = 0; pf2->paths[out_node][s2_index][d2_index].cost != -1; d2_index++)
					{
						int delay2 = pf2->paths[out_node][s2_index][d2_index].delay;
						// break if above delay constraint
						if (delay1 + delay2 > c1)
						{
							break;
						}

						int cost3 = pf1->paths[ABR][s1_index][d1_index].cost + pf2->paths[out_node][s2_index][d2_index].cost;
						int delay3 = delay1 + delay2;

						struct segment_list sl3 =
							merge_and_correct_sl(pf1->paths[ABR][s1_index][d1_index].sl,
												 pf2->paths[out_node][s2_index][d2_index].sl, pf2,
												 pf2bis, sr_bb, other_ABR, topo_bb, topo_area, src);

						if ((sl3.size > 10) && !cop)
						{
							continue;
						}

						//printf("To %s: %d, %d, %d\n", topo_area->labels->node[out_node].name, sl3.size, delay3, cost3);

						Dict_seglist_add(&pf3[sl3.size][out_node], delay3, cost3, sl3);
					}
				}
			}
		}
	}
	return pf3;
}

Dict_seglist_t **compact_pareto_front_ify(Dict_seglist_t **merged[2], int nbNodes, bool analyse)
{
	Dict_seglist_t **pf3 = NULL;
	Dict_t *dist = malloc(nbNodes * sizeof(Dict_t));
	ASSERT(dist, NULL, nbNodes);
	int maxIter = SEG_MAX;
	if (analyse)
	{
		maxIter = SEG_MULT * SEG_MAX;
	}

	for (int i = 0; i < nbNodes; i++)
	{
		Dict_init(&dist[i], 1000);
	}

	pf3 = malloc((maxIter + 1) * sizeof(Dict_seglist_t *));
	for (int i = 0; i <= maxIter; i++)
	{
		pf3[i] = malloc(nbNodes * sizeof(Dict_seglist_t));
		ASSERT(pf3[i], NULL, nbNodes);

		for (int j = 0; j < nbNodes; j++)
		{
			Dict_seglist_init(pf3[i] + j, 1000);
		}
	}

	my_m2 last_m2 = INF;
	my_m2 min = INF;
	my_m2 pfcand[1000];
	//Dict_init(&pfcand, 1000);

	for (int i = 0; i <= maxIter; i++)
	{
		for (int j = 0; j < nbNodes; j++)
		{
			last_m2 = INF;

			//Dict_reset(&pfcand);
			memset(pfcand, 0xff, 1000 * sizeof(my_m2));
			for (int k = 0; k < 1000; k++)
			{
				//Min IGP via both ABR
				min = MIN(merged[0][i][j].paths[k], merged[1][i][j].paths[k]);
				if (min < dist[j].paths[k])
				{
					//Dict_add(&pfcand, k, min, 0);
					pfcand[k] = min;
					Dict_add(&dist[j], k, min, 0);
				}
			}

			for (int k = 0; k < 1000; k++)
			{
				if (dist[j].paths[k] < last_m2)
				{
					last_m2 = dist[j].paths[k];
					if (pfcand[k] != INF)
					{
						if (dist[j].paths[k] == merged[0][i][j].paths[k])
						{
							Dict_seglist_add(&pf3[i][j], k, dist[j].paths[k], merged[0][i][j].seg_list[k]);
						}
						else
						{
							Dict_seglist_add(&pf3[i][j], k, dist[j].paths[k], merged[1][i][j].seg_list[k]);
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < nbNodes; i++)
	{
		Dict_free(&dist[i]);
	}

	free(dist);

	//Dict_free(&pfcand);

	return pf3;
}


// Dict_seglist_t **compact_pareto_front_ify(Dict_seglist_t **merged[2], int nbNodes, bool analyse)
// {
// 	Dict_seglist_t **pf3 = NULL;
// 	Dict_t *dist = malloc(nbNodes * sizeof(Dict_t));
// 	ASSERT(dist, NULL, nbNodes);
// 	int maxIter = SEG_MAX;
// 	if (analyse)
// 	{
// 		maxIter = SEG_MULT * SEG_MAX;
// 	}

// 	for (int i = 0; i < nbNodes; i++)
// 	{
// 		Dict_init(&dist[i], 1000);
// 	}

// 	pf3 = malloc((maxIter + 1) * sizeof(Dict_seglist_t *));
// 	for (int i = 0; i <= maxIter; i++)
// 	{
// 		pf3[i] = malloc(nbNodes * sizeof(Dict_seglist_t));
// 		ASSERT(pf3[i], NULL, nbNodes);

// 		for (int j = 0; j < nbNodes; j++)
// 		{
// 			Dict_seglist_init(pf3[i] + j, 1000);
// 		}
// 	}

// 	my_m2 last_m2 = INF;
// 	my_m2 min = INF;
// 	Dict_t pfcand;
// 	Dict_init(&pfcand, 1000);

// 	for (int i = 0; i <= maxIter; i++)
// 	{
// 		for (int j = 0; j < nbNodes; j++)
// 		{
// 			last_m2 = INF;

// 			Dict_reset(&pfcand);
// 			for (int k = 0; k < 1000; k++)
// 			{
// 				//Min IGP via both ABR
// 				min = MIN(merged[0][i][j].paths[k], merged[1][i][j].paths[k]);
// 				if (min < dist[j].paths[k])
// 				{
// 					Dict_add(&pfcand, k, min, 0);
// 					Dict_add(&dist[j], k, min, 0);
// 				}
// 			}

// 			for (int k = 0; k < 1000; k++)
// 			{
// 				if (dist[j].paths[k] < last_m2)
// 				{
// 					last_m2 = dist[j].paths[k];
// 					if (pfcand.paths[k] != INF)
// 					{
// 						if (dist[j].paths[k] == merged[0][i][j].paths[k])
// 						{
// 							Dict_seglist_add(&pf3[i][j], k, dist[j].paths[k], merged[0][i][j].seg_list[k]);
// 						}
// 						else
// 						{
// 							Dict_seglist_add(&pf3[i][j], k, dist[j].paths[k], merged[1][i][j].seg_list[k]);
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}

// 	for (int i = 0; i < nbNodes; i++)
// 	{
// 		Dict_free(&dist[i]);
// 	}

// 	free(dist);

// 	Dict_free(&pfcand);

// 	return pf3;
// }

Dict_seglist_t **compact_pareto_front_ify_3D(Dict_seglist_t ***merged, int nbNodes, bool analyse)
{
	Dict_seglist_t **pf3 = NULL;
	Dict_t *dist = malloc(nbNodes * sizeof(Dict_t));
	ASSERT(dist, NULL, nbNodes);
	int maxIter = SEG_MAX;

	if (analyse)
	{
		maxIter = SEG_MULT * SEG_MAX;
	}

	for (int i = 0; i < nbNodes; i++)
	{
		Dict_init(&dist[i], 1000);
	}

	pf3 = malloc((maxIter + 1) * sizeof(Dict_seglist_t *));
	for (int i = 0; i <= maxIter; i++)
	{
		pf3[i] = malloc(nbNodes * sizeof(Dict_seglist_t));
		ASSERT(pf3[i], NULL, nbNodes);

		for (int j = 0; j < nbNodes; j++)
		{
			Dict_seglist_init(pf3[i] + j, 1000);
		}
	}

	my_m2 last_m2 = INF;
	my_m2 min = INF;
	Dict_t pfcand;
	Dict_init(&pfcand, 1000);
	for (int i = 0; i <= maxIter; i++)
	{
		for (int j = 0; j < nbNodes; j++)
		{
			last_m2 = INF;

			Dict_reset(&pfcand);
			for (int k = 0; k < merged[0][i][j].size; k++)
			{
				//Min IGP via both ABR
				min = MIN(merged[0][i][j].paths[k], merged[1][i][j].paths[k]);
				if (min < dist[j].paths[k])
				{
					Dict_add(&pfcand, k, min, 0);
					Dict_add(&dist[j], k, min, 0);
				}
			}

			for (int k = 0; k < merged[0][i][j].size; k++)
			{
				if (dist[j].paths[k] < last_m2)
				{
					last_m2 = dist[j].paths[k];
					if (pfcand.paths[k] != INF)
					{
						if (dist[j].paths[k] == merged[0][i][j].paths[k])
						{
							Dict_seglist_add(&pf3[i][j], k, dist[j].paths[k], merged[0][i][j].seg_list[k]);
						}
						else
						{
							Dict_seglist_add(&pf3[i][j], k, dist[j].paths[k], merged[1][i][j].seg_list[k]);
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < nbNodes; i++)
	{
		Dict_free(&dist[i]);
	}

	free(dist);

	return pf3;
}
