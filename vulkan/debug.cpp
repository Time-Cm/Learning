#include "vulkan_program.hpp"
#if RUN_DEBUG

using namespace std;

string vulkan_program::bugreport(void)
{
    string report;
    return report;
}

void vulkan_program::getLocalInfo()
{
    // vulkan信息
    {
        using namespace std;

        uint32_t laycount;
        vkEnumerateInstanceLayerProperties(&laycount, nullptr);
        VkLayerProperties *properties = new VkLayerProperties[laycount];
        vkEnumerateInstanceLayerProperties(&laycount, properties);

        for (uint32_t i = 0; i < laycount; i++)
        {
            cout << properties[i].layerName << endl;
        }
        delete[] properties;
        cout << "Using layer:" << *debugSetting.vkLayerName << endl;
    }
}
#endif