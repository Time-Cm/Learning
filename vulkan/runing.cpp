#include "vulkan_program.hpp"

void vulkan_program::run(void)
{
    if (init() == false)
    {
        using namespace std;
        cerr << "Failed to init" << endl;
        exit(1);
    }

#if DEBUG
    getLocalInfo();
#endif

    while (glfwWindowShouldClose(glfwData.window) == false)
    {
        glfwPollEvents();
        usleep(1000);
    }
}

void vulkan_program::stop(void)
{
    vkDestroyInstance(vkData.instance, nullptr);
    glfwDestroyWindow(glfwData.window);
    glfwTerminate();
}