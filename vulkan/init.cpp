#include "vulkan_program.hpp"

bool vulkan_program::basicInit()
{
        // 全局变量
        uint32_t glfwExtensionCount;
        char **glfwExtensions = nullptr;

        // glfw初始化
        {
                // 创建GLFW窗口
                glfwInit();
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                glfwData.window = glfwCreateWindow(glfwSetting.weight, glfwSetting.height, "testing", nullptr, nullptr);
                if (glfwData.window == nullptr && glfwVulkanSupported() == false)
                {
                        std::cerr << "Vulkan version is lower than minimum require or fail to create window." << std::endl;
                        return false;
                }

                // 特性
                glfwExtensions = (char **)glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
#if RUN_DEBUG
                std::cout << *glfwExtensions << std::endl;
#endif
        }
        // vulkan初始化
        {
                // 应用信息
                VkApplicationInfo appInfo = {};
                appInfo.pApplicationName = "test";
                appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);
                appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 3);
                appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 3);

                // 实例信息
                VkInstanceCreateInfo instanceInfo = {};
                instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                instanceInfo.pApplicationInfo = &appInfo;
#if RUN_DEBUG
                std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

                instanceInfo.enabledExtensionCount = extensions.size();
                instanceInfo.ppEnabledExtensionNames = extensions.data();

                instanceInfo.enabledLayerCount = debugSetting.vkLayerCount;
                instanceInfo.ppEnabledLayerNames = debugSetting.vkLayerName;
#else
                instanceInfo.enabledExtensionCount = glfwExtensionCount;
                instanceInfo.ppEnabledExtensionNames = glfwExtensions;
                instanceInfo.enabledLayerCount = 0;
                instanceInfo.ppEnabledLayerNames = nullptr;
#endif
                if (vkCreateInstance(&instanceInfo, nullptr, &vkData.instance) != VK_SUCCESS)
                {
                        return false;
                }
        }

        //物理设备
        {
                uint32_t deviceCount;
                vkEnumeratePhysicalDevices(vkData.instance, &deviceCount, nullptr);

                VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
                uint32_t target_num = 0;
                int32_t target_mark = 0;

                vkEnumeratePhysicalDevices(vkData.instance, &deviceCount, devices);

                //设备评分以及选择
                for (uint32_t i = 0; i < deviceCount; i++)
                {
                        int32_t dev_mark;
                        dev_mark = getMark(devices[i]);
                        if (dev_mark > target_mark)
                        {
                                target_num = i;
                                target_mark = dev_mark;
                        }
                }

                vkData.physicalDevice = devices[target_num];
#if RUN_DEBUG
                std::cout << "Using device: " << target_num << std::endl;
#endif

                delete[] devices;

                if (vkData.physicalDevice == VK_NULL_HANDLE)
                {
                        using namespace std;
                        cerr << "Can't found the target GPU!" << endl;
                        return false;
                }
        }

        //窗口表面
        {
                if (glfwCreateWindowSurface(vkData.instance, glfwData.window, nullptr, &vkData.surface) != VK_SUCCESS)
                {
                        std::cerr << "Can't create surface" << std::endl;
                        return false;
                }
        }
        return true;
}

bool vulkan_program::createLogicDevice()
{
        float priorities = 1.0f;

        //队列信息
        std::vector<VkDeviceQueueCreateInfo> queueInfo;
        logicDevice device = {};

        if (getQueueCreateInfo(queueInfo, device.queueFamilies) == false)
        {
                return false;
        }

        //逻辑设备信息
        VkDeviceCreateInfo devInfo = {};
        devInfo.queueCreateInfoCount = queueInfo.size();
        devInfo.pQueueCreateInfos = queueInfo.data();
        devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        //设备特性
        VkPhysicalDeviceFeatures devF = {};

        devInfo.pEnabledFeatures = &devF;
#if RUN_DEBUG
        devInfo.enabledLayerCount = debugSetting.vkLayerCount;
        devInfo.ppEnabledLayerNames = debugSetting.vkLayerName;
#else
        devInfo.enabledLayerCount = 0;
        devInfo.ppEnabledLayerNames = nullptr;
#endif

        if (vkCreateDevice(vkData.physicalDevice, &devInfo, nullptr, &device.device) != VK_SUCCESS)
        {
                return false;
        }

        runTimeData.vk.logicDevices.push_back(device);
        queueInfo.clear();

        return true;
}

uint32_t vulkan_program::getMark(VkPhysicalDevice device)
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

        //设备特性
        VkPhysicalDeviceFeatures device_fea;
        vkGetPhysicalDeviceFeatures(device, &device_fea);

        //评分规则
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

        return dev_mark;
}

bool vulkan_program::getQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo> &queueInfo, std::vector<queueFamily> &queueFamilies)
{

        for (const auto flag : vkInitSetting.queueFamilyFlag)
        {
                VkDeviceQueueCreateInfo info = {};
                float priorities = 1.0f;
                int32_t index = getQueueFamilyIndex(flag);
                if (index == -1)
                {
                        std::cerr << "Fail to get queue index" << std::endl;
                        return false;
                }

                info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                info.queueCount = 1;
                info.queueFamilyIndex = index;
                info.pQueuePriorities = &priorities;

                queueInfo.push_back(info);

                queueFamily family ={};
                family.queueFamilyIndex = index;

                queueFamilies.push_back(family);
        }
        return true;
}

int32_t vulkan_program::getQueueFamilyIndex(VkQueueFlagBits flag)
{
        int32_t targetQueueFamily = -1;

        uint32_t familyNumber;
        vkGetPhysicalDeviceQueueFamilyProperties(vkData.physicalDevice, &familyNumber, nullptr);
        VkQueueFamilyProperties *properties = new VkQueueFamilyProperties[familyNumber];
        vkGetPhysicalDeviceQueueFamilyProperties(vkData.physicalDevice, &familyNumber, properties);

        for (uint32_t i = 0; i < familyNumber; i++)
        {
                if (properties[i].queueCount > 0 && (properties[i].queueFlags & flag))
                {
                        targetQueueFamily = i;
                }
        }

        delete[] properties;
        return targetQueueFamily;
}

bool vulkan_program::getQueue()
{
        return true;
}