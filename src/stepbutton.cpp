#include "stepbutton.h"

StepButton::StepButton(QWidget* parent) : QPushButton(parent)
{
    checked = false;
}

void StepButton::setChecked()
{
    checked = !checked;

    if (checked)
    {
        this->setStyleSheet("background-color: green;"
                            "border: 1px solid #888;"
                            "border-radius: 4px;");
    }
    else
    {
        this->setStyleSheet("background-color: transparent;"
                            "border: 1px solid #888;"
                            "border-radius: 4px;");
    }
    this->update();
}

bool StepButton::isChecked()
{
    return this->checked;
}

void StepButton::highlightStep()
{
    if (checked)
    {
        this->setStyleSheet("background-color: green;"
                            "border: 2px solid red;"
                            "border-radius: 4px;");
    }
    else
    {
        this->setStyleSheet("background-color: transparent;"
                            "border: 2px solid red;"
                            "border-radius: 4px;");
    }
    this->update();
}
void StepButton::unHighlightStep()
{
    if (checked)
    {
        this->setStyleSheet("background-color: green;"
                            "border: 1px solid #888;"
                            "border-radius: 4px;");
    }
    else
    {
        this->setStyleSheet("background-color: transparent;"
                            "border: 1px solid #888;"
                            "border-radius: 4px;");
    }
}
