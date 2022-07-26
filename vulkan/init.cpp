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
        }
        // vulkan初始化
        {
                // 应用信息
                VkApplicationInfo appInfo = {};
                appInfo.pApplicationName = "test";
                appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                appInfo.apiVersion = VK_API_VERSION_1_1;
                appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 3);
                appInfo.engineVersion = VK_API_VERSION_1_1;

                // 实例信息
                VkInstanceCreateInfo instanceInfo = {};
                instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                instanceInfo.pApplicationInfo = &appInfo;
#if RUN_DEBUG
                std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

                instanceInfo.enabledExtensionCount = extensions.size();
                instanceInfo.ppEnabledExtensionNames = extensions.data();

                instanceInfo.enabledLayerCount = debugSetting.vkLayerName.size();
                instanceInfo.ppEnabledLayerNames = debugSetting.vkLayerName.data();
                getLocalInfo();

                std::cout << "Using extensions:" << std::endl;
                for (const auto &extension : extensions)
                {
                        std::cout << extension << std::endl;
                }
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
#if RUN_DEBUG
                extensions.clear();
#endif
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
                        dev_mark = deviceMarkRule(devices[i]);
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

        if (getQueueCreateInfo(queueInfo, device.queueFamilies) == false || checkDeviceExtensionSupport() == false)
        {
                return false;
        }        

        //逻辑设备信息
        VkDeviceCreateInfo devInfo = {};
        devInfo.queueCreateInfoCount = queueInfo.size();
        devInfo.pQueueCreateInfos = queueInfo.data();
        devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        devInfo.pEnabledFeatures = &vkInitSetting.logicDeviceUsingFeatures;
        devInfo.enabledExtensionCount = vkInitSetting.deviceExtension.size();
        devInfo.ppEnabledExtensionNames = vkInitSetting.deviceExtension.data();
#if RUN_DEBUG
        devInfo.enabledLayerCount = debugSetting.vkLayerName.size();
        devInfo.ppEnabledLayerNames = debugSetting.vkLayerName.data();
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

bool vulkan_program::getQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo> &queueInfo, std::vector<queueFamily> &queueFamilies)
{

        for (const auto &flag : vkInitSetting.queueFamilyFlag)
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

                //队列创建信息记录
                queueInfo.push_back(info);

                //队列信息记录
                queueFamily family = {};
                family.queueFamilyIndex = index;
                family.queueCount = info.queueCount;

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

                if (flag == GET_PRESENT_QUEUE)
                {
                        VkBool32 support = false;
                        vkGetPhysicalDeviceSurfaceSupportKHR(vkData.physicalDevice, i, vkData.surface, &support);

                        if (support)
                        {
                                targetQueueFamily = i;
                        }
                }
        }

        if (targetQueueFamily == -1)
        {
                std::cerr << std::hex << "Flag(0x" << flag << ") isn't supported." << std::endl;
        }

        delete[] properties;
        return targetQueueFamily;
}

bool vulkan_program::getQueue()
{
        for (auto &device : runTimeData.vk.logicDevices)
        {
                for (auto &queueFamiles : device.queueFamilies)
                {
                        for (uint16_t i = 0; i < queueFamiles.queueCount; i++)
                        {
                                VkQueue queue;
                                vkGetDeviceQueue(device.device, queueFamiles.queueFamilyIndex, i, &queue);
                                if (queue == VK_NULL_HANDLE)
                                {
                                        return false;
                                }

                                queueFamiles.queue.push_back(queue);
                        }
                }
        }
        return true;
}

bool vulkan_program::checkDeviceExtensionSupport()
{
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(vkData.physicalDevice, nullptr, &extensionCount, nullptr);
        VkExtensionProperties *physicalDeviceExtension = new VkExtensionProperties[extensionCount];
        vkEnumerateDeviceExtensionProperties(vkData.physicalDevice, nullptr, &extensionCount, physicalDeviceExtension);

        std::set<std::string> extension(vkInitSetting.deviceExtension.begin(), vkInitSetting.deviceExtension.end());

#if RUN_DEBUG
        std::cout << "Supported Extensions:" << std::endl;
#endif

        for (uint32_t i = 0; i < extensionCount; i++)
        {
                extension.erase(physicalDeviceExtension[i].extensionName);
#if RUN_DEBUG
                std::cout << physicalDeviceExtension[i].extensionName << std::endl;
#endif
        }
        delete[] physicalDeviceExtension;

        return extension.empty();
}