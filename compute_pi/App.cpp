#include "App.hpp"

double App::pi(App *running)
{
    using namespace std;

    double result = 0;
    int inCount = 0;         //命中总次数
    int *data[THREADS] = {}; //用于线程数据交流的内存块
    thread *thPtr[THREADS] = {};

    for (int i = 0; i < THREADS; i++)
    {
        int *input = new int;
        *input = i;

        thPtr[i] = new thread(&App::compute, running, input);

        data[i] = input;
    }

    for (int i = 0; i < THREADS; i++)
    {
        thread &th = *thPtr[i];
        th.join();
    }

    for (int i = 0; i < THREADS; i++)
    {
        inCount += *data[i];
        delete data[i];
        delete thPtr[i];
    }

    result = 4.0 * (inCount / (double)COUNTS);
    return result;
}

void App::compute(int *inputData)
{
    using namespace std;

    int &inCount = *inputData;
    int threadsIndex = inCount; //保存传入的数据块中包含的线程信息

    srand(time(nullptr) / (threadsIndex + 1));
    for (int i = (COUNTS / THREADS) * threadsIndex; i < (COUNTS / THREADS) * (threadsIndex + 1); i++)
    {
        double x, y;

        x = rand() % POSITION_MAP;
        y = rand() % POSITION_MAP;

        if ((x * x + y * y) <= (POSITION_MAP * POSITION_MAP))
        {
            inCount += 1;
            srand((((rand() % time(nullptr)) + rand() / i) % rand())%POSITION_MAP);
        }
    }
}