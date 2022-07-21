#ifndef VULKAN_PROGRAM
#define VULKAN_PROGRAM

#define RUN_DEBUG true

//自定义功能
#define GET_PRESENT_QUEUE 0x0
#define GET_DRAW_QUEUE 0x1

//必要头文件
#include <iostream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <unistd.h>
#include <vector>
#include <set>

// vk程序类
class vulkan_program
{
    //自定义变量
private:
#include "data_struct.hpp"

    struct
    {
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkSurfaceKHR surface;
    } vkData = {};

    struct
    {
        GLFWwindow *window;
    } glfwData = {};

    struct
    {
        struct
        {
            std::vector<logicDevice> logicDevices;
        } vk;

        struct
        {
            /* data */
        } glfw;

    } runTimeData = {};

public:
    vulkan_program *selfptr = nullptr; //如果已经给该类分配了内存，则应把内存地址传入
    struct
    {
        int height = 600;
        int weight = 800;
    } glfwSetting;

    struct
    {
        std::set<VkQueueFlagBits> queueFamilyFlag = {};
    } vkInitSetting;

#if RUN_DEBUG
    struct
    {
        char **vkLayerName = nullptr;
        int vkLayerCount = 0;
    } debugSetting;
#endif

    //函数
private:
#if RUN_DEBUG
    void getLocalInfo();
#endif

    bool createLogicDevice();
    bool getQueue();
    bool basicInit();
    bool getQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo> &queueInfo, std::vector<queueFamily> &queueFamilies);
    uint32_t getMark(VkPhysicalDevice device);
    int32_t getQueueFamilyIndex(VkQueueFlagBits flag);
    uint32_t getQueueIndex(VkPhysicalDevice device, uint8_t queueType, VkQueueFlagBits flagBit);

public:
#if RUN_DEBUG
    std::string bugreport(void);
#endif

    void run(void);
    void stop(void);
};

#endif