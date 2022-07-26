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