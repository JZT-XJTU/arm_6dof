#include "nb_arm/path/trajectory_generator.hpp"
#include "kdl/path_line.hpp"                   // 具体实现需要 path_line
#include "kdl/rotational_interpolation_sa.hpp" // 具体实现需要 sa

/**
 * @brief 构造函数
 */
trajectory_generator::trajectory_generator()
{
    // 创建单轴旋转插值器，并存储为基类指针
    rot_interpolator = std::make_shared<KDL::RotationalInterpolation_SingleAxis>();

    // 如果无默认值就在这里修改
    // roundings_radius = 0.1;
    // equivalent_radius = 0.01;
    // float trap_max_vel = 0.5;
    // float trap_max_acc = 0.1;
}

/**
 * @brief 生成圆角路径
 * @param points 路径点 (std::vector)
 * @return KDL::Path_RoundedComposite 组合圆角路径
 */
std::unique_ptr<KDL::Path_RoundedComposite> trajectory_generator::generate_rounded_path(const std::vector<KDL::Frame> &points)
{
    auto path_round = std::make_unique<KDL::Path_RoundedComposite>(
        roundings_radius,
        equivalent_radius,
        rot_interpolator.get(),
        false);

    for (const auto &point : points)
    {
        path_round->Add(point);
    }
    path_round->Finish();
    std::cout << "Path length: " << path_round->PathLength() << std::endl;
    return path_round;
}

/**
 * @brief 生成直线路径
 * @param points 路径点 (std::vector)
 * @return KDL::Path_Composite 组合直线路径
 */
std::unique_ptr<KDL::Path_Composite> trajectory_generator::generate_line_path(const std::vector<KDL::Frame> &points)
{
    auto path_line_composite = std::make_unique<KDL::Path_Composite>();

    if (points.size() < 2)
    {
        return path_line_composite; // 路径点小于两个 返回空的复合路径
    }

    for (size_t i = 0; i < points.size() - 1; i++)
    {
        auto line_segment = std::make_unique<KDL::Path_Line>(points[i],
                                                             points[i + 1],
                                                             rot_interpolator.get(),
                                                             equivalent_radius,
                                                             false);
        path_line_composite->Add(line_segment.release(), true);
    }
    std::cout << "Path length: " << path_line_composite->PathLength() << std::endl;

    return path_line_composite;
}

/**
 * @brief 在给定的路径上生成轨迹规划
 * @param path 路径 is_trap 是否为梯形插值 spline_duration 样条插值时长
 * @return KDL::trajectory_seg 返回轨迹段
 */
std::unique_ptr<KDL::Trajectory> trajectory_generator::generate_trajectory_seg(std::unique_ptr<KDL::Path> path, bool is_trap, float spline_duration)
{
    // auto trajectory = std::make_unique<KDL::Trajectory>();
    std::unique_ptr<KDL::Trajectory> trajectory_seg;
    std::unique_ptr<KDL::VelocityProfile> velpref;
    if (is_trap)
    {
        velpref = std::make_unique<KDL::VelocityProfile_Trap>(trap_max_vel, trap_max_acc); // 梯形速度
        velpref->SetProfile(0.0, path->PathLength());
    }
    else
    {
        velpref = std::make_unique<KDL::VelocityProfile_Spline>();
        velpref->SetProfileDuration(0.0, path->PathLength(), spline_duration); // 样条插值速度，假设起止速度为 0
    }
    trajectory_seg = std::make_unique<KDL::Trajectory_Segment>(path.release(), velpref.release(), true);
    
    // 可以在这里添加多个轨迹段，然后组合成一个复合轨迹，这里暂时省略，使用时可在函数返回值中获取
    // trajectory_comp = std::make_unique<KDL::Trajectory_Composite>();
    // trajectory_comp->Add(trajectory_seg.release(), true);
    std::cout << "Trajectory duration: " << trajectory_seg->Duration() << std::endl;
    return trajectory_seg;
}
