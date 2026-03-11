#ifndef WAVEFORMWORKER_H
#define WAVEFORMWORKER_H

#include <QDebug>
#include <QFile>
#include <QObject>
#include <QString>
#include <QVector>
#include <QtMath>

class WaveformWorker : public QObject
{
    Q_OBJECT
  public:
    explicit WaveformWorker(const QString& filePath);

  public slots:
    void process();

  signals:
    void waveformReady(const QList<float>& peaks);
    void finished();

  private:
    QString filePath;
};

#endif // WAVEFORMWORKER_H
