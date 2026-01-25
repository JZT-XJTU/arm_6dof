#include "nb_arm/controllers/arm_control_sim.hpp"

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("arm_control_sim_node");
    ArmControlSim sim(node);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
