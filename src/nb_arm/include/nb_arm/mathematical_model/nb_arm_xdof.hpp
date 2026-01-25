#ifndef NB_ARM_XDOF_HPP
#define NB_ARM_XDOF_HPP

#include "math.h"
#include <vector>
#include <iostream>
#include <cstdint>

#include "rclcpp/rclcpp.hpp"
#include "kdl/frames.hpp"
#include "kdl/frames_io.hpp"
#include "kdl/chaindynparam.hpp"
#include "kdl/chainfksolverpos_recursive.hpp"
#include "kdl/chainiksolver.hpp"
#include "kdl/chainiksolverpos_lma.hpp"
#include "kdl/chainiksolvervel_wdls.hpp"
#include "kdl/chainfksolvervel_recursive.hpp"
#include "kdl/chainjnttojacsolver.hpp"
#include "kdl/chainjnttojacdotsolver.hpp"
#include <Eigen/Core>
#include <Eigen/QR>
#include <kdl/jacobian.hpp>
#include <kdl/jntarray.hpp>
#include "kdl/tree.hpp"
#include <kdl_parser/kdl_parser.hpp>

#include <urdf/model.h>

#include <cmath>

#define ARM_DOF 6

class nb_arm_xdof
{
private:
public:
    nb_arm_xdof();
    ~nb_arm_xdof();
    void init(const std::string &urdf_file); // Add init method
    std::shared_ptr<KDL::ChainFkSolverPos_recursive> fk_solver_pos;
    std::shared_ptr<KDL::ChainFkSolverVel_recursive> fk_solver_vel;
    std::shared_ptr<KDL::ChainIkSolverPos_LMA> ik_solver_pos_lma;
    std::shared_ptr<KDL::ChainIkSolverVel_wdls> ik_solver_vel_wdls;
    std::shared_ptr<KDL::ChainJntToJacSolver> jac_solver;
    std::shared_ptr<KDL::ChainJntToJacDotSolver> ik_solver_acc;
    std::shared_ptr<KDL::ChainDynParam> dyn_solver;
    std::shared_ptr<KDL::ChainIdSolver_RNE> id_solver;

    KDL::Chain kdl_chain;
    Eigen::Matrix<double, 6, 1> _L = Eigen::Matrix<double, 6, 1>::Ones();
    KDL::JntArray current_joint_positions;
    KDL::JntArray current_joint_velocities;
    KDL::JntArray current_joint_torques;
    KDL::JntArray current_joint_acceleration;
    KDL::JntArrayVel current_q_pos_vel; // 用于计算逆加速度函数的输入

    KDL::JntArray desire_joint_positions;
    KDL::JntArray desire_joint_velocities;
    KDL::JntArray desire_joint_torques;
    KDL::JntArray desire_joint_acceleration;
    KDL::JntArray forward_joint_torques;
    KDL::JntArray gravity_joint_torque;
    KDL::JntArrayVel desire_q_pos_vel;

    KDL::JntArray home_joint_positions;

    KDL::Jacobian current_jacobian;
    KDL::Jacobian desire_jacobian;

    KDL::Frame current_end_effector_frame;
    KDL::Frame desire_end_effector_frame;

    KDL::Twist desire_end_effector_dot;
    KDL::Twist desire_end_effector_ddot;
    KDL::Wrench desire_end_effector_wrench;
    KDL::Twist current_end_effector_dot;
    KDL::Twist current_end_effector_ddot;
    KDL::Wrench current_end_effector_wrench;

    std::vector<float> arm_link_length;
    float desire_joint_pos[ARM_DOF] = {0.0};
    float desire_joint_vel[ARM_DOF] = {0.0};
    float desire_joint_acc[ARM_DOF] = {0.0};
    float total_arm_length;
    float desire_end_effector_pos_default[ARM_DOF] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float external_force[ARM_DOF] = {0.0};
    float motor_direction[ARM_DOF] = {1, 1, 1, 1, 1, 1};
    float end_force_inv[6] = {0, 0, 1, 0, 0, 0}; // 用于力位混合控制指定力控的方向
    float end_force_value = -3;                  // 用于力位混合控制指定力控的数值
    float joint_hybrid_tor[ARM_DOF] = {0.0};
    float joint_ctrl_tor[ARM_DOF] = {0.0};

    // float current_joint_pos[ARM_DOF] = {0};
    // float current_joint_vel[ARM_DOF] = {0};
    // float current_joint_tor[ARM_DOF] = {0};

    float desire_joint_pos_protect_max[ARM_DOF] = {1.4, 1.55, 1.50, 1.56, 1.4, 1.5};
    float desire_joint_pos_protect_min[ARM_DOF] = {-1.4, -1.5, -1.5, -1.56, -1.5, -1.5};

    void joint2end_torque();
    void joint2end_vel();
    void computeInverseDynamics(const KDL::JntArray &q, const KDL::JntArray &q_dot, const KDL::JntArray &q_dotdot, KDL::JntArray &torques);
    void compute_joint_acceleration(const KDL::JntArrayVel &q_pos_vel, KDL::Twist &end_effector_ddot, KDL::JntArray &q_dotdot);
    void joint2motor(float q[ARM_DOF], float dq[ARM_DOF], float ddq[ARM_DOF], float tor_joint[ARM_DOF], float motor_pos[ARM_DOF], float motor_vel[ARM_DOF], float motor_acc[ARM_DOF], float tor_motor[ARM_DOF]);
    void update_armState(float motor_pos[ARM_DOF], float motor_vel[ARM_DOF], float motor_tau[ARM_DOF]);
    bool comput_ik(KDL::Frame &frame);
    void updateLinkMass(const std::string &link_name, float new_mass);
};

#endif // NB_ARM_XDOF_HPP
