#ifndef WAVEFORMWIDGET_H
#define WAVEFORMWIDGET_H

#include <QObject>
#include <QPaintEvent>
#include <QVector>
#include <QWidget>

class WaveformWidget : public QWidget
{
    Q_OBJECT
  public:
    WaveformWidget(QWidget* parent = nullptr);

  public slots:
    void setWaveform(const QList<float>& peaks);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QList<float> peaks;
};

#endif // WAVEFORMWIDGET_H
