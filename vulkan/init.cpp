#include "vulkan_program.hpp"

bool vulkan_program::init()
{
        //global data
        uint32_t glfwExtensionCount;
        char **glfwExtensions;

        //glfw init
        {
                //create window
                glfwInit();
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                glfwData.window = glfwCreateWindow(glfwSetting.weight, glfwSetting.height, "testing", nullptr, nullptr);
                if (glfwData.window == nullptr)
                {
                        return false;
                }

                //extension
                glfwExtensions = (char **)glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        }
        //vulkan init
        {
                //application init
                vkData.appInfo.pApplicationName = "test";
                vkData.appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                vkData.appInfo.apiVersion = VK_API_VERSION_1_2;
                vkData.appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 162);
                vkData.appInfo.engineVersion = VK_MAKE_VERSION(1, 2, 162);

                //instance init
                vkData.instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                vkData.instanceInfo.pApplicationInfo = &vkData.appInfo;
#if DEBUG
                std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

                vkData.instanceInfo.enabledExtensionCount = extensions.size();
                vkData.instanceInfo.ppEnabledExtensionNames = extensions.data();
                vkData.instanceInfo.enabledLayerCount = debugSetting.vkLayerCount;
                vkData.instanceInfo.ppEnabledLayerNames = debugSetting.vkLayerName;
#else
                vkData.instanceInfo.enabledExtensionCount = glfwExtensionCount;
                vkData.instanceInfo.ppEnabledExtensionNames = glfwExtensions;
                vkData.instanceInfo.enabledLayerCount = 0;
                vkData.instanceInfo.ppEnabledLayerNames = nullptr;
#endif

                vkCreateInstance(&vkData.instanceInfo, nullptr, &vkData.instance);
        }

        //physical devices
        {
                uint32_t deviceCount;
                vkEnumeratePhysicalDevices(vkData.instance, &deviceCount, nullptr);

                VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];

                vkEnumeratePhysicalDevices(vkData.instance, &deviceCount, devices);
                for (uint32_t i = 0; i < deviceCount; i++)
                {
                        if (foundTarget(devices[i]))
                        {
                                vkData.targetDevice=devices[i];
                        }
                }

                delete[] devices;
                if (vkData.targetDevice == VK_NULL_HANDLE)
                {
                        using namespace std;
                        cerr << "Can't found the target GPU!" << endl;
                }
                
        }
        return true;
}

bool vulkan_program::foundTarget(VkPhysicalDevice device)
{
        if (true)
        {
                return true;
        }
        
        return false;
}