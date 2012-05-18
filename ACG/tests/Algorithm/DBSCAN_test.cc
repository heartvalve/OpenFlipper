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
        "                                                          .                     ",
        "                                                                                ",
        "                         a                             b                        ",
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
        "                    .   a                                       cc              ",
        "                                                                cc              ",
        "                                                  ..                            ",
        "                                                   .                            ",
        "                                                                                ",
        0 };

class Point {
    public:
        Point(double x, double y, char classifier) : x(x), y(y), classifier(classifier) {}

        double length() const {
            return std::sqrt(x*x + y*y);
        }

        Point operator- (const Point &rhs) const {
            return Point(x-rhs.x, y-rhs.y, classifier);
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

        double x, y;
        char classifier;
};

template<class OSTREAM>
OSTREAM &operator<< (OSTREAM &stream, const Point &point) {
    return stream << "(" << point.x << ", " << point.y << ", " << "'" << point.classifier << "'" << ")";
}

template<class OUTPUT_ITERATOR>
void parse_points(const char * const * input, OUTPUT_ITERATOR points_out) {
    int y = 0;
    for (; *input != 0; ++input, ++y) {
        int x = 0;
        for (const char *it = *input; *it != 0; ++it, ++x) {
            if (!isspace(*it)) {
                *points_out++ = Point(x, y, *it);
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
                return testing::AssertionFailure() << "Noise point " << *point_it << " was mapped to non-noise cluster " << *cluster_it << ".";
            }

            if (*cluster_it == 0 && point_it->classifier != '.') {
                return testing::AssertionFailure() << "Non-noise point " << *point_it << " was mapped to noise cluster (0).";
            }
        } else {
            if (map_it->second != point_it->classifier) {
                return testing::AssertionFailure() << "Point " << *point_it << " was mapped to cluster '" << map_it->second << "'.";
            }
        }
    }

    return testing::AssertionSuccess() << "All points were mapped to clusters as expected.";
}


TEST(DBSCAN, manual_test_1) {
    std::vector<Point> points;
    parse_points(test1_map, std::back_inserter(points));
    std::vector<int> clusters;
    EXPECT_EQ(3,
              ACG::Algorithm::DBSCAN(points.begin(), points.end(), Point::DistanceFunc(),
                                     std::back_inserter(clusters), 4.0001, 3));
    EXPECT_TRUE(checkClusterConsistency(points, clusters));
}

}
