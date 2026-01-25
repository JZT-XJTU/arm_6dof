#include "nb_arm/arm_main_node.hpp"
#include <csignal>

arm_node::arm_node() : Node("arm_node")
{
    this->declare_parameter<std::string>("urdf_file", "src/nb_arm/urdf/first_robot.urdf");
    this->declare_parameter<std::string>("control_mode", "Zero");

    uint16_t canid1 = 0x01;
    uint16_t mstid1 = 0x201;
    uint16_t canid2 = 0x02;
    uint16_t mstid2 = 0x202;
    uint16_t canid3 = 0x03;
    uint16_t mstid3 = 0x203;
    uint16_t canid4 = 0x04;
    uint16_t mstid4 = 0x204;
    uint16_t canid5 = 0x05;
    uint16_t mstid5 = 0x205;
    uint16_t canid6 = 0x06;
    uint16_t mstid6 = 0x206;
    uint32_t nom_baud = 1000000;
    uint32_t dat_baud = 2000000;
    std::vector<damiao::DmActData> init_data;
    init_data.push_back(damiao::DmActData{.motorType = damiao::DM4310,
                                          .mode = damiao::MIT_MODE,
                                          .can_id = canid1,
                                          .mst_id = mstid1});
    init_data.push_back(damiao::DmActData{.motorType = damiao::DM4310,
                                          .mode = damiao::MIT_MODE,
                                          .can_id = canid2,
                                          .mst_id = mstid2});
    init_data.push_back(damiao::DmActData{.motorType = damiao::DM4310,
                                          .mode = damiao::MIT_MODE,
                                          .can_id = canid3,
                                          .mst_id = mstid3});
    init_data.push_back(damiao::DmActData{.motorType = damiao::DM4310,
                                          .mode = damiao::MIT_MODE,
                                          .can_id = canid4,
                                          .mst_id = mstid4});
    init_data.push_back(damiao::DmActData{.motorType = damiao::DM4310,
                                          .mode = damiao::MIT_MODE,
                                          .can_id = canid5,
                                          .mst_id = mstid5});
    init_data.push_back(damiao::DmActData{.motorType = damiao::DM4310,
                                          .mode = damiao::MIT_MODE,
                                          .can_id = canid6,
                                          .mst_id = mstid6});
    motor = std::make_shared<damiao::Motor_Control>(nom_baud, dat_baud,
                                                    "9940F4E149D904A69924737E3DE6629F", &init_data);

    init();

    RCLCPP_INFO(this->get_logger(), "arm_node has been started.");
}

void arm_node::init()
{
    std::string urdf_file;
    this->get_parameter("urdf_file", urdf_file);
    arm_model.init(urdf_file);

    std::string control_mode_str;
    this->get_parameter("control_mode", control_mode_str);
    
    if (control_mode_str == "Position") {
        mode = State::Position;
        arm_controller.modeSwitch(State::Position);
    } else if (control_mode_str == "HybridForce") {
        mode = State::HybridForce;
        arm_controller.modeSwitch(State::HybridForce);
    } else if (control_mode_str == "Zero") {
        mode = State::Zero;
        arm_controller.modeSwitch(State::Zero);
    } else {
        RCLCPP_WARN(this->get_logger(), "Unknown control mode: %s. Defaulting to HybridForce.", control_mode_str.c_str());
        mode = State::HybridForce;
        arm_controller.modeSwitch(State::HybridForce);
    }
    
    // Start the control loop
    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(10), // 100Hz
        [this]() {
            arm_controller.run(arm_model, *motor);
        });
}

arm_node::~arm_node()
{
    node_instance->motor->disable_motor();
    RCLCPP_INFO(this->get_logger(), "arm_node is shutting down.");
}

arm_node *arm_node::node_instance = nullptr;
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<arm_node>();
    arm_node::node_instance = node.get();
    std::atexit(arm_node::stopArm);
    std::signal(SIGINT, arm_node::signalHandler);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}