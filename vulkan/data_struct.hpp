struct queueFamily
{
    uint32_t queueFamilyIndex;
    std::vector<uint32_t> queue;
};

struct logicDevice
{
    VkDevice device;
    std::vector<queueFamily> queueFamilies;
};