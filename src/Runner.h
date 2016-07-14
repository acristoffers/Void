#ifndef RUNNER_H
#define RUNNER_H

#include <functional>

#include <QRunnable>
#include <QThreadPool>

class Runner : public QRunnable
{
public:
    Runner(std::function<void (void)> );
    void run() override;

private:
    std::function<void (void)> func;
};

#endif // RUNNER_H
