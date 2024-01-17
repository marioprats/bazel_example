#include <iostream>

#include "core.h"

#include "urdf_parser/urdf_parser.h"
#include "absl/strings/substitute.h"

int main(int argc, char** argv)
{
  urdf::ModelInterfaceSharedPtr urdf = urdf::parseURDFFile("data/pr2.urdf");
  if (!urdf)
  {
    std::cout << "Can't parse URDF." << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<urdf::LinkSharedPtr> links;
  urdf->getLinks(links);
  std::cout << absl::Substitute("Robot has $0 links.\n", links.size());

  auto chain = CreateChain(*urdf, "right_arm", "base_link", "r_wrist_roll_link");
  if (!chain.ok())
  {
    std::cout << "Couldn't create chain: " << chain.status().message() << std::endl;
  }
  std::cout << absl::Substitute("Chain created successfully, with $0 links and $1 active joints.\n",
                                chain->links.size(), chain->active_joint_indexes.size());

  Eigen::VectorXd joint_values = Eigen::VectorXd::Zero(chain->active_joint_indexes.size());
  auto link_poses = ChainForwardKinematics(*urdf, *chain, joint_values).value();

  for (int i = 0; i < link_poses.size(); i++)
  {
    std::cout << "Link " << i << ":\n" << link_poses[i].matrix() << std::endl << std::endl;
  }

  return 0;
}
