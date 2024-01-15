#include <iostream>

#include "urdf_parser/urdf_parser.h"
#include "absl/strings/substitute.h"
#include "Eigen/Geometry"

int main(int argc, char** argv)
{
  urdf::ModelInterfaceSharedPtr urdf = urdf::parseURDFFile("data/pr2.urdf");
  if (!urdf) {
    std::cout << "Can't parse URDF." << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<urdf::LinkSharedPtr> links;
  urdf->getLinks(links);

  std::cout << absl::Substitute("Robot has $0 links.\n", links.size());

  Eigen::Vector3d vector(0.0, 0.0, 0.0);
  std::cout << "Eigen vector: " << vector.transpose() << std::endl;
  return 0;
}
