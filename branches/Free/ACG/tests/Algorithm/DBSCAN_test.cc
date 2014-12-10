/*
 * DBSCAN_test.cpp
 *
 *  Created on: May 18, 2012
 *      Author: ebke
 */

#include <gtest/gtest.h>

#include <vector>
#include <map>

#include <cmath>
#include <cstring>

#include "../../Algorithm/DBSCANT.hh"

namespace {
const char * const test1_map[] = {
        "                                                                                ",
        "                                                .   b     b .                   ",
        "                                                                                ",
        "                         a                             b                        ",
        "                                                                                ",
        "                                                                                ",
        "                         a                       b  b  b                        ",
        "                         aa                  b   b   b                          ",
        "                       aaaa    .     .      b b b bbb   b                       ",
        "                       aa                    b b                                ",
        "            a a  a    a                                                         ",
        "          a  a a  a  a  a                                                       ",
        "            a a  a                                                              ",
        "                                                                                ",
        "                        aaa                                                     ",
        "                                                                                ",
        "                                                                                ",
        "                                                                                ",
        "                .   a   a   a   .                               cc              ",
        "                                                                cc              ",
        "                                                  ..                            ",
        "                                                   .                            ",
        "                                                                                ",
        0 };

const char * const test2_map[] = { "aaaaAAaaaa", 0 };

class Point {
    public:
        Point(double x, double y, char classifier, double weight = 1.0) : x(x), y(y), weight(weight), classifier(classifier) {}

        double length() const {
            return std::sqrt(x*x + y*y);
        }

        Point operator- (const Point &rhs) const {
            return Point(x-rhs.x, y-rhs.y, classifier, weight);
        }

        double dist(const Point &rhs) const {
            return operator-(rhs).length();
        }

        class DistanceFunc {
            public:
                double operator() (const Point &a, const Point &b) const {
                    return a.dist(b);
                }
        };

        class WeightFunc {
            public:
                double operator() (const Point &a) const {
                    return a.weight;
                }
        };

        double x, y, weight;
        char classifier;
};

template<class OSTREAM>
OSTREAM &operator<< (OSTREAM &stream, Point* point) {
    return stream << "(" << point->x << ", " << point->y << ", " << point->weight << ", " << "'" << point->classifier << "'" << ")";
}

template<class OUTPUT_ITERATOR>
void parse_points(const char * const * input, OUTPUT_ITERATOR points_out, double uc_weight = 1.0, double lc_weight = 1.0) {
    int y = 0;
    for (; *input != 0; ++input, ++y) {
        int x = 0;
        for (const char *it = *input; *it != 0; ++it, ++x) {
            if (!isspace(*it)) {
                const double weight = islower(*it) ? lc_weight : uc_weight;
                *points_out++ = Point(x, y, *it, weight);
            }
        }
    }
}

testing::AssertionResult checkClusterConsistency(const std::vector<Point> &points, const std::vector<int> &cluster_map) {
    std::map<int, char> cluster_2_classifier;

    std::vector<int>::const_iterator cluster_it = cluster_map.begin();
    for (std::vector<Point>::const_iterator point_it = points.begin(), point_it_end = points.end();
            point_it != point_it_end; ++point_it, ++cluster_it) {

        std::map<int, char>::const_iterator map_it = cluster_2_classifier.find(*cluster_it);
        if (map_it == cluster_2_classifier.end()) {
            cluster_2_classifier[*cluster_it] = point_it->classifier;

            if (point_it->classifier == '.' && *cluster_it != 0) {
                return testing::AssertionFailure() << "Noise point " << &(*point_it) << " was mapped to non-noise cluster " << *cluster_it << ".";
            }

            if (*cluster_it == 0 && point_it->classifier != '.') {
                return testing::AssertionFailure() << "Non-noise point " << &(*point_it) << " was mapped to noise cluster (0).";
            }
        } else {
            if (map_it->second != point_it->classifier) {
                return testing::AssertionFailure() << "Point " << &(*point_it) << " was mapped to cluster '" << map_it->second << "'.";
            }
        }
    }

    return testing::AssertionSuccess() << "All points were mapped to clusters as expected.";
}

template<class II_1, class II_2>
testing::AssertionResult checkCollectionEquivalence(II_1 first1, II_1 last1, II_2 first2, II_2 last2) {
    for (int index = 0; first1 != last1 && first2 != last2; ++first1, ++first2, ++index) {
        if (*first1 != *first2)
            return testing::AssertionFailure() << "Mismatch in element " <<  index << ".";
    }

    if (first1 != last1)
        return testing::AssertionFailure() << "Second collection longer than first one.";

    if (first2 != last2)
        return testing::AssertionFailure() << "First collection longer than second one.";

    return testing::AssertionSuccess() << "Collections are equivalent.";
}

TEST(DBSCAN, manual_test_1) {
    std::vector<Point> points;
    parse_points(test1_map, std::back_inserter(points));
    std::vector<int> clusters;

    EXPECT_EQ(3,
              ACG::Algorithm::DBSCAN(points.begin(), points.end(), Point::DistanceFunc(),
                                     std::back_inserter(clusters), 4.0001, 3.0));
    EXPECT_TRUE(checkClusterConsistency(points, clusters));

    // Call both versions of DBSCAN.
    EXPECT_EQ(3,
              ACG::Algorithm::DBSCAN(points.begin(), points.end(), Point::DistanceFunc(),
                                     std::back_inserter(clusters), 4.0001, 3.0,
                                     ACG::Algorithm::_DBSCAN_PRIVATE::constant_1<std::vector<Point>::iterator::value_type>()));
    EXPECT_TRUE(checkClusterConsistency(points, clusters));
}

TEST(DBSCAN, manual_test_2_a) {
    std::vector<Point> points;
    parse_points(test2_map, std::back_inserter(points), 1.0, 1.0);
    std::vector<int> clusters;
    EXPECT_EQ(1,
              ACG::Algorithm::DBSCAN(points.begin(), points.end(), Point::DistanceFunc(),
                                     std::back_inserter(clusters), 1.01, 1.2, Point::WeightFunc()));

    const int expected[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    EXPECT_TRUE(checkCollectionEquivalence(clusters.begin(), clusters.end(), expected, expected + 10));
}

TEST(DBSCAN, manual_test_2_b) {
    std::vector<Point> points;
    parse_points(test2_map, std::back_inserter(points), 1.0, .5);
    std::vector<int> clusters;
    EXPECT_EQ(1,
              ACG::Algorithm::DBSCAN(points.begin(), points.end(), Point::DistanceFunc(),
                                     std::back_inserter(clusters), 1.01, 1.2, Point::WeightFunc()));

    const int expected[] = { 0, 0, 1, 1, 1, 1, 1, 1, 0, 0 };
    EXPECT_TRUE(checkCollectionEquivalence(clusters.begin(), clusters.end(), expected, expected + 10));
}

}
