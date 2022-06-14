#include "vulkan_program.hpp"

bool vulkan_program::init()
{
        // 全局变量
        uint32_t glfwExtensionCount;
        char **glfwExtensions;

        // glfw初始化
        {
                // create window
                glfwInit();
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                glfwData.window = glfwCreateWindow(glfwSetting.weight, glfwSetting.height, "testing", nullptr, nullptr);
                if (glfwData.window == nullptr)
                {
                        return false;
                }

                // extension
                glfwExtensions = (char **)glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        }

        // vulkan初始化
        {
                // 应用初始化
                vkData.appInfo.pApplicationName = "test";
                vkData.appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                vkData.appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);
                vkData.appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 3);
                vkData.appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 3);

                // 实例初始化
                vkData.instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                vkData.instanceInfo.pApplicationInfo = &vkData.appInfo;
#if DEBUG
                std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

                vkData.instanceInfo.enabledExtensionCount = extensions.size();
                vkData.instanceInfo.ppEnabledExtensionNames = extensions.data();
                extensions.clear();

                vkData.instanceInfo.enabledLayerCount = debugSetting.vkLayerCount;
                vkData.instanceInfo.ppEnabledLayerNames = debugSetting.vkLayerName;
#else
                vkData.instanceInfo.enabledExtensionCount = glfwExtensionCount;
                vkData.instanceInfo.ppEnabledExtensionNames = glfwExtensions;
                vkData.instanceInfo.enabledLayerCount = 0;
                vkData.instanceInfo.ppEnabledLayerNames = nullptr;
#endif
                if (vkCreateInstance(&vkData.instanceInfo, nullptr, &vkData.instance) != VK_SUCCESS)
                {
                        return false;
                }
        }

        //物理设备
        {
                uint32_t deviceCount;
                vkEnumeratePhysicalDevices(vkData.instance, &deviceCount, nullptr);

                VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
                bool haveFoundDevice = false;

                vkEnumeratePhysicalDevices(vkData.instance, &deviceCount, devices);
                for (uint32_t i = 0; i < deviceCount; i++)
                {
                        if (foundTarget(devices[i]) && haveFoundDevice == false)
                        {
                                vkData.targetDevice = devices[i];
                                haveFoundDevice = true;
                        }
                }

                delete[] devices;
                if (vkData.targetDevice == VK_NULL_HANDLE)
                {
                        using namespace std;
                        cerr << "Can't found the target GPU!" << endl;
                        return false;
                }
        }

        //逻辑设备
        {
        }
        return true;
}

bool vulkan_program::foundTarget(VkPhysicalDevice device)
{
        VkPhysicalDeviceProperties device_pro;
        vkGetPhysicalDeviceProperties(device, &device_pro);
#if DEBUG
        std::cout << device_pro.deviceName << "\n"
                  << "API version: "
                  << VK_VERSION_MAJOR(device_pro.apiVersion) << "."
                  << VK_VERSION_MINOR(device_pro.apiVersion) << "."
                  << VK_VERSION_PATCH(device_pro.apiVersion)
                  << std::endl;
#endif
        if (device_pro.apiVersion >= VK_MAKE_VERSION(1, 0, 100) && (device_pro.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || device_pro.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU))
        {
#if DEBUG
                std::cout << "Using device: " << device_pro.deviceName << std::endl;
#endif
                return true;
        }
        else if (device_pro.apiVersion >= VK_MAKE_VERSION(1, 0, 3) && device_pro.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
        {
#if DEBUG
                std::cout << "Using device: " << device_pro.deviceName << std::endl;
#endif
                return true;
        }

        return false;
}