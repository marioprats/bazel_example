#pragma once

#include <Eigen/Geometry>

#include <absl/status/statusor.h>
#include <absl/strings/substitute.h>
#include <urdf_parser/urdf_parser.h>

struct KinematicChain
{
  std::string name;

  std::vector<urdf::LinkConstSharedPtr> links;
  std::vector<urdf::JointConstSharedPtr> joints;

  std::vector<int> active_joint_indexes;
};

// Returns an error status if a chain between root_link_name and tip_link_name doesn't exist.
absl::StatusOr<KinematicChain> CreateChain(const urdf::ModelInterface& model, const std::string& chain_name,
                                           const std::string& root_link_name, const std::string& tip_link_name);

// Computes the transform for a joint, given the joint value.
absl::StatusOr<Eigen::Isometry3d> CalculateJointTransform(const urdf::ModelInterface& model, const urdf::Joint& joint,
                                                          double joint_value);

// Computes the chain forward kinematics.
absl::StatusOr<std::vector<Eigen::Isometry3d>> ChainForwardKinematics(const urdf::ModelInterface& model,
                                                                      const KinematicChain& chain,
                                                                      const Eigen::VectorXd& joint_values);