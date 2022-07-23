#include"native.hpp"

struct queueFamily
{
    uint32_t queueFamilyIndex;
    uint16_t queueCount;
    std::vector<VkQueue> queue;
};

struct logicDevice
{
    VkDevice device;
    std::vector<queueFamily> queueFamilies;
};