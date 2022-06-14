#include <iostream>
#include "vulkan_program.hpp"

int main(void)
{
    try
    {
        // 初始化设置
        vulkan_program &app = *(new vulkan_program);
        app.glfwSetting.height = 300;
        app.glfwSetting.weight = 400;
        // 调试设置
        char *layname = (char *)"VK_LAYER_KHRONOS_validation";

        app.debugSetting.vkLayerCount = 1;
        app.debugSetting.vkLayerName = &layname;

        //运行时设定
        app.run();
        app.stop();

        //关闭应用
        delete &app;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
