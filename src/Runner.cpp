#include "Runner.h"

Runner::Runner(std::function<void (void)> func)
{
    this->func = func;
}

void Runner::run()
{
    func();
}
