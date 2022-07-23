#include <iostream>

#include "vulkan_program.hpp"

int main(void)
{
    try
    {
        // init
        vulkan_program &app = *(new vulkan_program);
        app.selfptr = &app;
        app.glfwSetting.height = 300;
        app.glfwSetting.weight = 400;
        app.vkInitSetting.queueFamilyFlag = {VK_QUEUE_GRAPHICS_BIT,GET_PRESENT_QUEUE};

        // debug

        app.debugSetting.vkLayerName.push_back((char*)"VK_LAYER_KHRONOS_validation");
        app.debugSetting.vkLayerName.push_back((char*)"VK_LAYER_MANGOHUD_overlay");

        //运行时设定
        app.run();

        //关应用
        delete &app;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}