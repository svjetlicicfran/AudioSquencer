#ifndef STEPTASK_H
#define STEPTASK_H

#include "track.h"

#include <QObject>
#include <QRunnable>
#include <QTimer>

class StepTask : public QRunnable
{
public:
    StepTask();

    Track *track;
    QTimer trackTimer;

    void run() override {}
};

#endif // STEPTASK_H
