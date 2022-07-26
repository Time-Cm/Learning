#include <iostream>

#include "vulkan_program.hpp"

int main(void)
{
        try
        {
                /*程序初始化*/
                vulkan_program &app = *(new vulkan_program);
                app.selfptr = &app;

                app.glfwSetting.height = 300;
                app.glfwSetting.weight = 400;

                app.vkInitSetting.queueFamilyFlag = {VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT, GET_PRESENT_QUEUE};
                //启用设备特性
                app.vkInitSetting.logicDeviceUsingFeatures.geometryShader = VK_TRUE;
                app.vkInitSetting.logicDeviceUsingFeatures.samplerAnisotropy = VK_TRUE;
                app.vkInitSetting.deviceExtension.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

                /*调试*/
                app.debugSetting.vkLayerName.push_back("VK_LAYER_KHRONOS_validation");

                /*运行*/
                app.run();

                /*关闭应用*/
                delete &app;
        }
        catch (const std::exception &e)
        {
                std::cerr << e.what() << std::endl;
        }

        return 0;
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

        if (getQueue() == false)
        {
                std::cerr << "Fail to get queue" << std::endl;
                exit(1);
        }
        //以下为自定义行为区，可自由操作
        /*************************************************************/
        while (glfwWindowShouldClose(glfwData.window) == false)
        {
                glfwPollEvents();
                usleep(1000);
        }
        /*************************************************************/
        stop();
}

uint32_t vulkan_program::deviceMarkRule(VkPhysicalDevice device)
{
        int32_t dev_mark = 0;
        //设备属性
        VkPhysicalDeviceProperties device_pro;
        vkGetPhysicalDeviceProperties(device, &device_pro);
#if RUN_DEBUG
        std::cout << device_pro.deviceName << "\n"
                  << "API version: "
                  << VK_VERSION_MAJOR(device_pro.apiVersion) << "."
                  << VK_VERSION_MINOR(device_pro.apiVersion) << "."
                  << VK_VERSION_PATCH(device_pro.apiVersion)
                  << std::endl;
#endif

        if (device_pro.apiVersion >= VK_MAKE_VERSION(1, 0, 100))
        {
                dev_mark += 2;
        }

        if (device_pro.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || device_pro.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU || device_pro.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
        {
                dev_mark += 5;
        }
        else if (device_pro.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
        {
                dev_mark += 2;
        }

        //设备特性
        /*与vkInitSetting.logicDeviceUsingFeatures对应使用，判断特性是否支持*/
        VkPhysicalDeviceFeatures device_fea;
        vkGetPhysicalDeviceFeatures(device, &device_fea);

        if (device_fea.geometryShader == false)
        {
                dev_mark = 0;
        }

        if (device_fea.samplerAnisotropy == false)
        {
                dev_mark = 0;
        }

        return dev_mark;
}
