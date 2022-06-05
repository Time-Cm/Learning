#include <iostream>
#include "vulkan_program.hpp"

int main(void)
{
    try
    {
        //init setting
        vulkan_program &app = *(new vulkan_program);
        app.glfwSetting.height = 300;
        app.glfwSetting.weight = 400;
        //debug setting
        char *layname = (char *)"VK_LAYER_KHRONOS_validation";

        app.debugSetting.vkLayerCount = 1;
        app.debugSetting.vkLayerName = &layname;

        //running setting
        //app.getLocalInfo();
        app.run();
        app.stop();

        //close app
        delete &app;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}