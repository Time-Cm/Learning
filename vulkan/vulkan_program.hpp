#ifndef VULKAN_PROGRAM
#define VULKAN_PROGRAM

#define DEBUG true

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

//vk程序类
class vulkan_program
{
    //自定义变量
private:
    struct
    {
        VkInstanceCreateInfo instanceInfo;
        VkApplicationInfo appInfo;
        VkInstance instance;
        VkPhysicalDevice targetDevice = VK_NULL_HANDLE;
    } vkData;

    struct
    {
        GLFWwindow *window = nullptr;
    } glfwData;

public:
    vulkan_program *selfptr = nullptr; //如果已经给该类分配了内存，则应把内存地址传入
    struct
    {
        int height = 600;
        int weight = 800;
    } glfwSetting;

#if DEBUG
    struct
    {
        char **vkLayerName;
        int vkLayerCount;
    } debugSetting;
#endif

    //函数
private:
#if DEBUG
    void getLocalInfo();
#endif
    bool init();
    bool foundTarget(VkPhysicalDevice device);

public:
#if DEBUG
    std::string bugreport(void);
#endif

    void run(void);
    void stop(void);
};

#endif