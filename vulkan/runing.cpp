#include "vulkan_program.hpp"

void vulkan_program::stop(void)
{
    vkDestroySurfaceKHR(vkData.instance, vkData.surface, nullptr);
    for (const auto &logicDevice : runTimeData.vk.logicDevices)
    {
        vkDestroyDevice(logicDevice.device, nullptr);
    }

    vkDestroyInstance(vkData.instance, nullptr);
    glfwDestroyWindow(glfwData.window);
    glfwTerminate();
}

void vulkan_program::run(void)
{
    if (basicInit() == false)
    {
        using namespace std;
        cerr << "Failed to init" << endl;
        exit(1);
    }

    if (createLogicDevice() == false)
    {
        std::cerr << "Fail to create logic device" << std::endl;
        exit(1);
    }

#if RUN_DEBUG
    getLocalInfo();
#endif

    //以下为自定义行为区，可自由指定操作
    if (getQueue() == false)
    {
        std::cerr << "Fail to get queue" << std::endl;
        exit(1);
    }
    

    while (glfwWindowShouldClose(glfwData.window) == false)
    {
        glfwPollEvents();
        usleep(1000);
    }
    stop();
}