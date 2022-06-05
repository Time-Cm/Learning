#include "vulkan_program.hpp"
#if DEBUG

using namespace std;

string vulkan_program::bugreport(void)
{
}

void vulkan_program::getLocalInfo()
{
    //vulkan info
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
        cout << "Using: " << debugSetting.vkLayerName << endl;
    }
}
#endif