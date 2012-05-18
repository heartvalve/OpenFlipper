/*
 * DBSCAN.hh
 *
 *  Created on: May 18, 2012
 *      Author: ebke
 */

#ifndef DBSCAN_HH_
#define DBSCAN_HH_

#include <vector>
#include <iterator>
#include <algorithm>

/*
 * Private functions.
 */
#include "DBSCANT.cc"

namespace ACG {
namespace Algorithm {

/**
 * Implements the DBSCAN algorithm introduced in
 *
 * Ester, Martin, Hans-Peter Kriegel, Jörg S, and Xiaowei Xu. “A Density-based Algorithm for Discovering Clusters in Large Spatial Databases with Noise.” 226–231. AAAI Press, 1996.
 *
 * Classifies the sequence [first, last) into clusters. Outputs `int`s into the result sequence, one
 * for each element in the input sequence. `0` means noise, values greater than 0 specify a cluster index.
 *
 * Returned cluster indices are guaranteed to be a continuous range starting at 1.
 *
 * Result has to support the operation `*result++ = <int>`.
 *
 * @param Input iterator to the initial position of the data set.
 * @param Input iterator to the final position of the data set.
 * @param distance_func Binary function taking two elements as arguments. Returns the distance between these elements.
 * @param result Output iterator to the initial position of the result range. The range includes as many elements as [first, last).
 * @param epsilon The density-reachable neighborhood radius.
 * @param n_min The density-reachable count threshold.
 * @return The number of clusters found.
 */
template<typename INPUT_ITERATOR, typename DISTANCE_FUNC, typename OUTPUT_ITERATOR>
int DBSCAN(const INPUT_ITERATOR first, const INPUT_ITERATOR last, DISTANCE_FUNC distance_func,
           OUTPUT_ITERATOR result, const double epsilon, const int n_min) {

    const size_t input_size = std::distance(first, last);

    std::vector<int> id_cache(input_size, -1);

    int idx = 0;
    int current_cluster_id = 0;

    for (INPUT_ITERATOR it = first; it != last; ++it, ++idx) {

        // Visit every element only once.
        if (id_cache[idx] >= 0) continue;

        // Gather neighborhood.
        std::vector<INPUT_ITERATOR> neighborhood; neighborhood.reserve(input_size);
        _DBSCAN_PRIVATE::region_query(first, last, it, distance_func, std::back_inserter(neighborhood), epsilon);

        if ((int)neighborhood.size() < n_min) {
            // It's noise.
            id_cache[idx] = 0;
        } else {
            // It's the seed of a cluster.
            _DBSCAN_PRIVATE::expand_cluster(first, last, it, distance_func, epsilon, n_min, id_cache, ++current_cluster_id);
        }
    }

    std::copy(id_cache.begin(), id_cache.end(), result);

    return current_cluster_id;
}

} /* namespace Algorithm */
} /* namespace ACG */
#endif /* DBSCAN_HH_ */
