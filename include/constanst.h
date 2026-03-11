#ifndef CONSTANST_H
#define CONSTANST_H

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QString>

namespace Constants {
constexpr int SETP_BUTTON_WIDTH = 40;
constexpr int STEP_BUTTON_HEIGTH = 40;
const QColor accent = qApp->palette().color(QPalette::Highlight);
const QColor bg = qApp->palette().color(QPalette::Window);
constexpr int SETP_COUNT = 24;
} // namespace Constants

#endif //CONSTANST_H
