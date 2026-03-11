#include "waveformworker.h"

WaveformWorker::WaveformWorker(const QString& filePath) : filePath(filePath)
{
}

void WaveformWorker::process()
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
    {
        emit waveformReady({});
        return;
    }

    QByteArray audioData = f.readAll();
    f.close();

    QList<float> peaks;

    const int chunkSize = audioData.size() / 210; 

    // qInfo() << audioData.size() << " " << chunkSize;

    for (int i = 0; i < audioData.size(); i += chunkSize)
    {
        float peak = 0.0f;

        for (int j = i; j < i + chunkSize && j < audioData.size(); j++)
        {
            peak = qMax(peak, qAbs((float)audioData[j] / 128.0f));
        }

        peaks.push_back(peak);
    }

    emit waveformReady(peaks);
}
