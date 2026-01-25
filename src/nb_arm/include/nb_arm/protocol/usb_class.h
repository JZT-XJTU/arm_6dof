#ifndef USB_CLASS_H
#define USB_CLASS_H

#include <stdint.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <libusb-1.0/libusb.h>
#include <vector>
#include <string>
#include <atomic>
#include <cstring> 
#include <iomanip>
#include <functional>
#include <mutex>
#include "nb_arm/unit/crc.h"
#include <unistd.h>
#include <stdio.h>

//DM-FD-CAN的PID和VID
#define DM_USBD_VID           0x34B7
#define DM_USBD_PID           0x6877  

#define RX_LENGTH    (32*1024)

typedef struct
{
    // uint8_t     reserve0[2];        //保留2字节，
    uint8_t     step_id_l;
    uint8_t     step_id_h;          //id自增步长
    uint8_t     can_type:1;         //类型：2.0/fd
    uint8_t     fram_type:1;        //类型：远程/数据
    uint8_t     id_type:1;          //类型：标准/拓展
    uint8_t     fd_acc:1;           //类型：数据段加速
    uint8_t     id_increase:1;      //id递增
    uint8_t     data_increase:1;    //数据递增
    uint8_t     cmd:1;              //开始/停止
    uint8_t     reserve1;           //保留1bit
    uint8_t     dlc;                //数据包长度
    int32_t     send_time;           //发送次数
    // uint8_t     reserve3[4];        //保留4字节，
    uint32_t    stop_id;
    uint32_t    interval;           //时间间隔
    uint32_t    can_id;             //报文id
    uint8_t     data[64];           //数据负载
}can_tx_type;

typedef struct
{
    uint32_t  id;               //can id
    uint32_t  time_stamp;       //时间戳
    uint8_t   reserve[3];       //保留
    uint8_t   fram_type:1;      //类型：远程/数据
    uint8_t   can_type:1;       //类型：2.0/fd
    uint8_t   id_type:1;        //类型：标准/拓展
    uint8_t   dir:1;            //方向：rx/tx
    uint8_t   dlc:4;            //长度
}can_head_type;

typedef struct
{
    can_head_type head;
    uint8_t   data[64];         //数据区
}can_value_type;

//命令码
typedef enum{
    CMD_START_CAP   ,       
    CMD_STOP_CAP    ,     
    CMD_RESERVE1    ,     
    CMD_RESERVE2     ,     
    CMD_TEST_START  ,      
    CMD_SETUP_BUARD ,       
    CMD_ACK_HELLO   ,      
   
    CMD_GET_BAUD    ,
    CMD_RECOVERY_FACTORY,
    CMD_STOP_SEND   ,
}CMD_TYPE;

//响应码
typedef enum{
    ACK_PACK = 0x00             ,   
    //协议层错误
    NACK_FM_ERR                 ,       
    NACK_CRC_ERR                ,      
    NACK_ARG_ERR                ,      
    NACK_CALL_ERR               ,     
    //ACK报文有问题
    USB_ACK_FAILED                ,       
    //应用层错误
    FLASH_ERR                   ,       
    CAN_FD_SET_FAILED             ,      
    CAN_SET_FAILED                ,       
    RESERVE1                 ,     
    RESERVE2              ,     
    RESERVE3                ,
    RESERVE4                ,
    //USB传输层出错
    USB_CLEAR_FAILED              ,       
    USB_SEND_FAILED               ,   
    USB_RX_FAILED                 ,      
}ACK_TYPE;

class usb_class 
{  
using clock = std::chrono::steady_clock;
using duration = std::chrono::duration<float>;
using FrameCallback = std::function<void(can_value_type&)>;

public:
    usb_class(uint32_t nom_baud,uint32_t dat_baud,std::string sn);
    ~usb_class();

    void usb_clear();
    std::vector<std::string> usb_get_dm_device(int* num);
    int usb_open(std::string str);
    int can_pack_dlc_get(can_head_type* head);

    void fillFDCANFrame(std::vector<uint8_t>& data, can_tx_type& frame,uint32_t canId);
    void set_tx_frame(can_tx_type* frame);
    void send_data();
    void fdcanFrameSend(std::vector<uint8_t>& data, uint32_t canId);

    int usb_control_send(uint8_t *ptr , uint16_t length);
    uint8_t usb_control_ack(uint8_t cmd , uint16_t *length , uint8_t** data);
    uint8_t usb_contorl_transfer(uint8_t cmd , uint16_t length , uint8_t *pdata , 
                                    uint16_t *rx_len , uint8_t** rx_data);
    
    uint8_t USB_CMD_START_CAP();
    uint8_t USB_CMD_STOP_CAP();
    uint8_t USB_CMD_START_TEST();
    uint8_t USB_CMD_SETUP_BUARD(uint8_t canMode , uint8_t dbrp , uint8_t dbrp_fd , 
                                uint16_t can_seg1, uint16_t can_seg2 , 
                                uint16_t can_fd_seg1 , uint16_t can_fd_seg2);
    uint8_t USB_CMD_ACK_HELLO();
    uint8_t USB_CMD_TEST_ACK(uint16_t& len , uint8_t* &data);


    void USB_CMD_HEART();

    libusb_device_handle* getDeviceHandle() const {
        return dev_handle;
    }
    
    void setFrameCallback(FrameCallback cb);
    void print_can_value(can_value_type& can_value);
    int unpack_can_frame(uint8_t* pdat, size_t actual_len);
    int unpack_usb_frame(uint8_t *pdat , uint32_t length);
    void get_data_thread();
    void send_thread();
    void can_rev_thread();

private:

    libusb_device *usb_dev = NULL;//该结构体对于的u2canfd设备
    libusb_device_handle* dev_handle = NULL;//该结构体对于的u2canfd设备
    libusb_context *ctx = NULL;
    libusb_device **device_list = NULL;

    int num_devices = 0;
    int dm_cnt = 0;

    can_tx_type tx_frame;

    mutable std::mutex mutex_;
    FrameCallback frame_callback_;
    std::thread rec_thread;
    std::thread rec_thread2;
    std::thread rec_thread3;
    std::atomic<bool> stop_thread ;
    uint8_t winusb_read_buf[RX_LENGTH];
    uint8_t can_read_buf[RX_LENGTH];

    uint32_t dat_baud_;//数据域波特率
    std::string sn_;//设备的serial_number
};
#endif // USB_CLASS_H
