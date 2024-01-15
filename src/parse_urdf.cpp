#include <iostream>

#include "urdf_parser/urdf_parser.h"
#include "absl/strings/substitute.h"

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

  return 0;
}
