#ifndef FREESOUNDBROWSER_H
#define FREESOUNDBROWSER_H

#include "sum.h"

#include <QAudioOutput>
#include <QDebug>
#include <QDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QSoundEffect>
#include <QTemporaryFile>
#include <QUrl>
#include <QUrlQuery>

namespace Ui
{
class FreeSoundBrowser;
}

class FreeSoundBrowser : public QDialog
{
    Q_OBJECT

  public:
    explicit FreeSoundBrowser(QWidget* parent = nullptr);
    ~FreeSoundBrowser();
  private slots:
    void sendSearchRequest();
    void sendDownloadRequest(QString url);
    void sendPlayRequest(QString url);

    void handleSearchResponse(QNetworkReply* serverReply);
    void handleDownloadResponse(QNetworkReply* serverReply);
    void handlePlayResponse(QNetworkReply* serverReply);

  signals:
    void fileDownloaded(QTemporaryFile* tempFile);

  private:
    void convertMp3ToWav(QTemporaryFile* tempFile);

    Ui::FreeSoundBrowser* ui;
    const QString TOKEN = "";

    QUrl serverUrl;
    QNetworkAccessManager* networkNamager;

    QMediaPlayer* player;
    QAudioOutput* audioOutput;
};

#endif // FREESOUNDBROWSER_H
