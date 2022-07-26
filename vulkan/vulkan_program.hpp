#ifndef VULKAN_PROGRAM
#define VULKAN_PROGRAM
#include "native.hpp"

#define RUN_DEBUG true

//自定义功能
#define GET_PRESENT_QUEUE (VkQueueFlagBits)0x0

// vk程序类
class vulkan_program
{
    //自定义变量
private:
#include "data_struct.hpp"

    //基础数据
    struct
    {
        GLFWwindow *window;
    } glfwData = {};

    struct
    {
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkSurfaceKHR surface;
    } vkData = {};

    //运行时数据
    struct
    {
        struct
        {
            /* data */
        } glfw;

        struct
        {
            std::vector<logicDevice> logicDevices;
        } vk;
    } runTimeData = {};

public:
    //外部传入接口
    vulkan_program *selfptr = nullptr; //如果已经给该类分配了内存，则应把内存地址传入
    struct
    {
        int height = 600;
        int weight = 800;
    } glfwSetting;

    struct
    {
        VkPhysicalDeviceFeatures logicDeviceUsingFeatures = {};
        std::set<VkQueueFlagBits> queueFamilyFlag = {};
        std::vector<const char *> deviceExtension = {};
    } vkInitSetting;

#if RUN_DEBUG
    struct
    {
        std::vector<const char *> vkLayerName;
    } debugSetting = {};
#endif

    //自定义函数
private:
    bool basicInit();
    bool checkDeviceExtensionSupport();
    bool createLogicDevice();
    bool getQueue();
    bool getQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo> &queueInfo, std::vector<queueFamily> &queueFamilies);
    int32_t getQueueFamilyIndex(VkQueueFlagBits flag);
    uint32_t getQueueIndex(VkPhysicalDevice device, uint8_t queueType, VkQueueFlagBits flagBit);
#if RUN_DEBUG
    void getLocalInfo();
#endif
    void stop(void);

public:
#if RUN_DEBUG
    std::string bugreport(void);
#endif

    uint32_t deviceMarkRule(VkPhysicalDevice device);
    void run(void);
};

#endif