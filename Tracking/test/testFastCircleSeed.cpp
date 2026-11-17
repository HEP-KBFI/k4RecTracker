#include "FastCircleSeed.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
void require(bool condition, const char* message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

double dot(const Point2D_xy& a, const Point2D_xy& b) { return a.x * b.x + a.y * b.y; }

std::vector<Point2D_xy> arc(double start, double length, unsigned count) {
  std::vector<Point2D_xy> points;
  for (unsigned i = 0; i < count; ++i) {
    const double angle = start + length * i / (count - 1);
    points.push_back({1000. + 800. * std::cos(angle), 250. + 800. * std::sin(angle)});
  }
  return points;
}

void checkArc(double length) {
  const double pcaAngle = std::atan2(250., 1000.) + std::acos(-1.);
  auto points = arc(pcaAngle - .3, length, 80);
  FastCircleFit circle(points);
  const auto pca = circle.closestPointTo({0., 0.});
  const Point2D_xy expected{-std::sin(pcaAngle), std::cos(pcaAngle)};
  const auto tangent = circle.tangentAtPCA(pca, points);
  require(dot(tangent, expected) > .999999, "Ordered arc must give counterclockwise seed momentum");
  // The PCA lies ahead of the second measurement. It must not be used as
  // the start of the measured flight-direction segment.
  require(dot(circle.tangentAtPCA(pca, points[1]), tangent) < -.999999,
          "Regression fixture must expose the PCA-to-second-hit reversal");
  std::reverse(points.begin(), points.end());
  require(dot(circle.tangentAtPCA(pca, points), expected) < -.999999,
          "Reversing measurement order must reverse the seed momentum");
}
} // namespace

int main(int argc, char** argv) {
  checkArc(.6);
  checkArc(2. * std::acos(-1.) + .6);
  require(argc == 2, "Expected the measured-hit regression fixture path");
  std::ifstream input(argv[1]);
  require(input.good(), "Cannot open measured-hit regression fixture");
  std::vector<Point2D_xy> hits;
  std::string line;
  while (std::getline(input, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    Point2D_xy point;
    std::istringstream row(line);
    require(bool(row >> point.x >> point.y), "Invalid measured-hit fixture row");
    hits.push_back(point);
  }
  require(hits.size() > 20, "Regression fixture must contain the full ordered hit trajectory");
  FastCircleFit circle(hits);
  const auto pca = circle.closestPointTo({0., 0.});
  const Point2D_xy chord{hits.back().x - hits.front().x, hits.back().y - hits.front().y};
  require(dot(circle.tangentAtPCA(pca, hits[1]), chord) < 0.,
          "Original ttbar seed must point against the measured trajectory");
  require(dot(circle.tangentAtPCA(pca, hits), chord) > 0.,
          "The corrected seed must follow the measured trajectory without truth information");
  return 0;
}
