#include "waveformwidget.h"

#include <QPainter>

WaveformWidget::WaveformWidget(QWidget* parent) : QWidget(parent)
{
    this->setFixedWidth(200);
}

void WaveformWidget::setWaveform(const QList<float>& peaks)
{
    this->peaks = peaks;
    update();
}

void WaveformWidget::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (peaks.isEmpty())
        return;

    int w = width();
    int h = height();
    int count = peaks.size();
    float barWidth = 1;

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#66ff66"));

    for (int i = 0; i < count; i++)
    {
        float val = peaks[i];
        int barHeight = val * (h - 2);

        p.drawRect(i * barWidth, (h - barHeight) / 2, barWidth, barHeight);
    }
}
