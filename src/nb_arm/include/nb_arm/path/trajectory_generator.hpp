#ifndef TRAJECTORY_GENERATOR_HPP
#define TRAJECTORY_GENERATOR_HPP

// #pragma once

#include <vector>
#include <iostream>
#include <memory>

#include "kdl/frames.hpp"
#include "kdl/frames_io.hpp"
#include "kdl/path.hpp"
#include "kdl/path_composite.hpp"
#include "kdl/path_roundedcomposite.hpp"
#include "kdl/rotational_interpolation.hpp" // 包含基类即可
#include "kdl/velocityprofile.hpp"
#include "kdl/velocityprofile_spline.hpp"
#include "kdl/velocityprofile_trap.hpp"
#include "kdl/trajectory.hpp"
#include "kdl/trajectory_segment.hpp"
#include "kdl/trajectory_composite.hpp"


namespace KDL
{
    class RotationalInterpolation_SingleAxis;
}

class trajectory_generator
{
private:
    float roundings_radius = 0.05;
    float equivalent_radius = 0.1; // 等效旋转半径，可以简单理解为机械臂旋转速度，等效半径越大旋转的平均速度越慢
    float trap_max_vel = 0.5;
    float trap_max_acc = 0.2;
    std::shared_ptr<KDL::RotationalInterpolation> rot_interpolator; // 使用基类指针

public:
    trajectory_generator();

    ~trajectory_generator() = default;

    std::unique_ptr<KDL::Path_RoundedComposite> generate_rounded_path(const std::vector<KDL::Frame> &points);
    std::unique_ptr<KDL::Path_Composite> generate_line_path(const std::vector<KDL::Frame> &points);
    std::unique_ptr<KDL::Trajectory> generate_trajectory_seg(std::unique_ptr<KDL::Path> path, bool is_trap, float spline_duration);
};

#endif // TRAJECTORY_GENERATOR_HPP
