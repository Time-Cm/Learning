#include <iostream>
#include "App.hpp"

int main(void)
{
    App *apps = new App;
    App &myapp = *apps;

    std::cout << std::fixed << myapp.pi(apps) << std::endl;
    delete apps;

    return 0;
}
