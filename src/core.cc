#include "core.h"

absl::StatusOr<Eigen::Isometry3d> CalculateJointTransform(const urdf::ModelInterface& model, const urdf::Joint& joint,
                                                          double joint_value)
{
  switch (joint.type)
  {
    case urdf::Joint::CONTINUOUS:
      ABSL_FALLTHROUGH_INTENDED;
    case urdf::Joint::REVOLUTE: {
      Eigen::Quaterniond rotation(
          Eigen::AngleAxisd(joint_value, Eigen::Vector3d(joint.axis.x, joint.axis.y, joint.axis.z)));
      return Eigen::Isometry3d(rotation);
    }
    case urdf::Joint::PRISMATIC:
      return Eigen::Isometry3d(
          Eigen::Translation3d(joint_value * joint.axis.x, joint_value * joint.axis.y, joint_value * joint.axis.z));
    case urdf::Joint::FIXED:
      return Eigen::Isometry3d::Identity();
    default:
      return absl::UnimplementedError("Joint type not supported.");
  };
}

absl::StatusOr<KinematicChain> CreateChain(const urdf::ModelInterface& model, const std::string& chain_name,
                                           const std::string& root_link_name, const std::string& tip_link_name)
{
  const auto root_link = model.links_.find(root_link_name);
  if (root_link == model.links_.end())
  {
    return absl::NotFoundError(absl::Substitute("Root link $0 not found in model.", root_link_name));
  }

  const auto tip_link = model.links_.find(tip_link_name);
  if (tip_link == model.links_.end())
  {
    return absl::NotFoundError(absl::Substitute("Tip link not found in model.", tip_link_name));
  }

  urdf::JointSharedPtr tip_joint = tip_link->second->parent_joint;
  if (!tip_joint)
  {
    return absl::NotFoundError(absl::Substitute("Tip link $0 doesn't have a parent joint.", tip_link_name));
  }

  std::vector<urdf::LinkConstSharedPtr> links;
  std::vector<urdf::JointConstSharedPtr> joints = { tip_joint };
  std::vector<int> active_joint_indexes;
  // TODO: This shouldn't be a while true.
  while (true)
  {
    const auto last_joint_type = joints.back()->type;
    if (last_joint_type != urdf::Joint::REVOLUTE && last_joint_type != urdf::Joint::CONTINUOUS &&
        last_joint_type != urdf::Joint::PRISMATIC && last_joint_type != urdf::Joint::FIXED)
    {
      std::cout << "Joint type: " << last_joint_type << std::endl;
      return absl::InvalidArgumentError("Can't create a chain with multi-dof joint types.");
    }

    if (last_joint_type != urdf::Joint::FIXED)
    {
      active_joint_indexes.push_back(joints.size() - 1);
    }

    links.push_back(model.getLink(joints.back()->parent_link_name));
    if (links.back()->name == root_link_name)
    {
      break;
    }

    // TODO: detect loops.

    if (links.back()->parent_joint == nullptr)
    {
      return absl::NotFoundError("Chain not found from tip to root links");
    }
    joints.push_back(links.back()->parent_joint);
  }

  std::reverse(links.begin(), links.end());
  std::reverse(joints.begin(), joints.end());
  std::reverse(active_joint_indexes.begin(), active_joint_indexes.end());

  return KinematicChain{ .name = chain_name,
                         .links = std::move(links),
                         .joints = std::move(joints),
                         .active_joint_indexes = std::move(active_joint_indexes) };
}

absl::StatusOr<std::vector<Eigen::Isometry3d>> ChainForwardKinematics(const urdf::ModelInterface& model,
                                                                      const KinematicChain& chain,
                                                                      const Eigen::VectorXd& joint_values)
{
  std::vector<Eigen::Isometry3d> link_poses;
  link_poses.reserve(chain.links.size());
  link_poses.push_back(Eigen::Isometry3d::Identity());

  int joint_id = 0;
  for (int i = 1; i < chain.links.size(); i++)
  {
    const auto link = chain.links[i];
    const auto parent_joint = link->parent_joint;

    Eigen::Isometry3d joint_offset = Eigen::Quaterniond(parent_joint->parent_to_joint_origin_transform.rotation.w,
                                                        parent_joint->parent_to_joint_origin_transform.rotation.x,
                                                        parent_joint->parent_to_joint_origin_transform.rotation.y,
                                                        parent_joint->parent_to_joint_origin_transform.rotation.z) *
                                     Eigen::Translation3d(parent_joint->parent_to_joint_origin_transform.position.x,
                                                          parent_joint->parent_to_joint_origin_transform.position.y,
                                                          parent_joint->parent_to_joint_origin_transform.position.z);

    if (parent_joint->type == urdf::Joint::FIXED)
    {
      link_poses.push_back(link_poses.back() * joint_offset);
    }
    else
    {
      auto transform = CalculateJointTransform(model, *parent_joint, joint_values[joint_id]);
      if (!transform.ok())
      {
        return transform.status();
      }
      link_poses.push_back(link_poses.back() * joint_offset *
                           CalculateJointTransform(model, *parent_joint, joint_values[joint_id]).value());
      joint_id++;
    }
  }
  return link_poses;
}