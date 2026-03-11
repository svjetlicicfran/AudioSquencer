#include "freesoundbrowser.h"

#include "ui_freesoundbrowser.h"

FreeSoundBrowser::FreeSoundBrowser(QWidget* parent)
    : QDialog(parent), ui(new Ui::FreeSoundBrowser), networkNamager(new QNetworkAccessManager(this)), player(new QMediaPlayer(this)),
      audioOutput(new QAudioOutput(this))
{
    ui->setupUi(this);

    serverUrl.setScheme("http");
    serverUrl.setHost("freesound.org");
    serverUrl.setPath("/apiv2/search/text");

    ui->soundTableWidget->setRowCount(8);
    ui->soundTableWidget->setColumnCount(3);
    ui->soundTableWidget->setHorizontalHeaderLabels({"Name", "Play", "Download"});

    ui->soundTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->soundTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->soundTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->soundTableWidget->setAlternatingRowColors(true);
    ui->soundTableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    ui->downloadProgressBar->setValue(0);

    player->setAudioOutput(audioOutput);

    connect(ui->searchButton, &QPushButton::clicked, this, &FreeSoundBrowser::sendSearchRequest);

}

FreeSoundBrowser::~FreeSoundBrowser()
{
    delete ui;
}

void FreeSoundBrowser::sendSearchRequest()
{
    QUrlQuery query;
    query.addQueryItem("query", ui->searchQuerryLineEdit->text());
    query.addQueryItem("page_size", "8");
    query.addQueryItem("fields", "name,previews");
    query.addQueryItem("duration", "[3 TO 15]");

    QUrl searchRequestUrl(serverUrl);
    searchRequestUrl.setQuery(query);

    QNetworkRequest searchRequest(searchRequestUrl);
    searchRequest.setRawHeader("Authorization", TOKEN.toUtf8());

    QNetworkReply* searchReply = this->networkNamager->get(searchRequest);

    connect(searchReply, &QNetworkReply::finished, this, [this, searchReply]() { this->handleSearchResponse(searchReply); });
}

void FreeSoundBrowser::sendDownloadRequest(QString url)
{
    QUrl downloadUrl(url);

    QNetworkRequest downloadSoundRequest(downloadUrl);
    QNetworkReply* downloadReply = this->networkNamager->get(downloadSoundRequest);

    connect(downloadReply, &QNetworkReply::finished, this, [this, downloadReply]() { this->handleDownloadResponse(downloadReply); });

    connect(downloadReply, &QNetworkReply::downloadProgress, this,
            [this](qint64 received, qint64 total)
            {
                if (total <= 0)
                    return;

                ui->downloadProgressBar->setRange(0, 100);
                ui->downloadProgressBar->setValue(int((received * 100) / total));
            });
}

void FreeSoundBrowser::sendPlayRequest(QString url)
{
    player->stop();
    player->setSource(QUrl(url));
    player->play();
}

void FreeSoundBrowser::handleSearchResponse(QNetworkReply* serverReply)
{
    if (serverReply->error() != QNetworkReply::NoError)
    {
        QMessageBox* errorMsgBox = new QMessageBox(QMessageBox::Warning, "Error", serverReply->errorString());
        errorMsgBox->show();
        serverReply->deleteLater();
        return;
    }

    const QByteArray data = serverReply->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(data);

    QJsonObject root = doc.object();
    QJsonArray results = root.value("results").toArray();

    for (int i = 0; i < results.size(); i++)
    {
        QJsonObject soundObj = results.at(i).toObject();

        QJsonObject previews = soundObj.value("previews").toObject();

        QString name = soundObj.value("name").toString();
        QString previewMp3 = previews.value("preview-hq-mp3").toString();

        ui->soundTableWidget->setItem(i, 0, new QTableWidgetItem(name));

        QPushButton* playButton = new QPushButton(this);
        QPushButton* downloadButton = new QPushButton(this);
        playButton->setText("Play");
        downloadButton->setText("Download");
        ui->soundTableWidget->setCellWidget(i, 1, playButton);
        ui->soundTableWidget->setCellWidget(i, 2, downloadButton);

        connect(playButton, &QPushButton::clicked, this, [this, previewMp3]() { this->sendPlayRequest(previewMp3); });
        connect(downloadButton, &QPushButton::clicked, this, [this, previewMp3]() { this->sendDownloadRequest(previewMp3); });
    }

    serverReply->deleteLater();
}

void FreeSoundBrowser::handleDownloadResponse(QNetworkReply* serverReply)
{
    if (serverReply->error() != QNetworkReply::NoError)
    {
        QMessageBox* errorMsgBox = new QMessageBox(QMessageBox::Warning, "Error", serverReply->errorString());
        errorMsgBox->show();
        serverReply->deleteLater();
        return;
    }

    QTemporaryFile* tempFile = new QTemporaryFile();
    if (!tempFile->open())
    {
        return;
    }

    tempFile->write(serverReply->readAll());
    tempFile->flush();

    convertMp3ToWav(tempFile);

    serverReply->deleteLater();
}

void FreeSoundBrowser::handlePlayResponse(QNetworkReply* serverReply)
{
}

void FreeSoundBrowser::convertMp3ToWav(QTemporaryFile* mp3TempFile)
{
    QProcess* ffmpeg = new QProcess(this);
    QTemporaryFile* wavTempFile = new QTemporaryFile();

    if (!wavTempFile->open())
    {
        ffmpeg->deleteLater();
        wavTempFile->deleteLater();
        mp3TempFile->deleteLater();
        return;
    }

    wavTempFile->rename(wavTempFile->fileName() + ".wav");

    connect(ffmpeg, &QProcess::finished, this,
            [this, ffmpeg, mp3TempFile, wavTempFile](int exitCode, QProcess::ExitStatus status)
            {
                if (status == QProcess::NormalExit && exitCode == 0)
                {
                    emit this->fileDownloaded(wavTempFile);
                }
                else
                {
                    qInfo() << "FFmpeg failed";
                }

                ffmpeg->deleteLater();
            });

    ffmpeg->start("ffmpeg", {"-y", "-i", mp3TempFile->fileName(), wavTempFile->fileName()});
}
