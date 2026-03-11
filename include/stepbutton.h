#ifndef STEPBUTTON_H
#define STEPBUTTON_H

// #include "constanst.h"

#include <QObject>
#include <QPushButton>
#include <QWidget>

class StepButton : public QPushButton
{
    Q_OBJECT

  public:
    StepButton(QWidget* parent = nullptr);

    bool isChecked();
    void highlightStep();
    void unHighlightStep();

  public slots:
    void setChecked();

  private:
    bool checked;
};

#endif // STEPBUTTON_H
