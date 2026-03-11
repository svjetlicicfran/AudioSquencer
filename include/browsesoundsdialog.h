#ifndef BROWSESOUNDSDIALOG_H
#define BROWSESOUNDSDIALOG_H

#include "soundpreviewe.h"

#include <QAudioFormat>
#include <QAudioSink>
#include <QDebug>
#include <QDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMediaDevices>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTcpSocket>
#include <QTemporaryFile>
#include <QThread>
#include <QUdpSocket>
#include <QVBoxLayout>
#include <qurl.h>

namespace Ui
{
class BrowseSoundsDialog;
}

class BrowseSoundsDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit BrowseSoundsDialog(QString _authToken, QUrl _serverUrl, QWidget* parent = nullptr);
    ~BrowseSoundsDialog();

  private slots:
    void handleResponse(QNetworkReply* serverReply);
    void handleDownload(QNetworkReply* serverReply);
    void soundPrevieweClick(QString soundName);
    void downloadSoundClick(QString soundName);
    void filterSounds();

  signals:
    void fileDownloaded(QTemporaryFile* tempFile);

  private:
    Ui::BrowseSoundsDialog* ui;
    const QString authToken;

    QUrl serverUrl;
    QNetworkAccessManager* networkNamager;

    QTcpSocket* clientTcpSocket;
    QUdpSocket* udpSocket;

    QVBoxLayout* scrollAreaLayout;
    QList<SoundPrevieweWidget*> soundList;

    QAudioSink* audioSink;
    QIODevice* audioIODevice;

    QSet<QString> genreSet;
    QSet<QString> typeSet;

    void handleUdp();
    void initSounds();
    void connectToServer();
};

#endif // BROWSESOUNDSDIALOG_H
