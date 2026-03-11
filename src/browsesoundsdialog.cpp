#include "browsesoundsdialog.h"

#include "ui_browsesoundsdialog.h"
#include <qurl.h>

BrowseSoundsDialog::BrowseSoundsDialog(QString _authToken, QUrl _serverUrl, QWidget* parent)
    : QDialog(parent), ui(new Ui::BrowseSoundsDialog), networkNamager(new QNetworkAccessManager(this)), authToken(_authToken),
      clientTcpSocket(new QTcpSocket(this)), udpSocket(new QUdpSocket(this)), serverUrl(_serverUrl)
{
    ui->setupUi(this);
    scrollAreaLayout = new QVBoxLayout(this->ui->scrollArea);

    this->setWindowTitle("Broswe sounds");

    serverUrl.setPath("/sound");

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleFormat(QAudioFormat::Int16);

    audioSink = new QAudioSink(QMediaDevices::defaultAudioOutput(), format, this);
    audioIODevice = audioSink->start();

    udpSocket->bind(QHostAddress::Any, 37548);

    QWidget* contentWidget = new QWidget(this);
    scrollAreaLayout = new QVBoxLayout(contentWidget);
    scrollAreaLayout->setAlignment(Qt::AlignTop);

    ui->scrollArea->setWidget(contentWidget);
    ui->scrollArea->setWidgetResizable(true);

    this->initSounds();

    this->connectToServer();

    connect(this->udpSocket, &QUdpSocket::readyRead, this, &BrowseSoundsDialog::handleUdp);

    connect(this->ui->genreSelectComboBox, &QComboBox::currentTextChanged, this, &BrowseSoundsDialog::filterSounds);
    connect(this->ui->typeSelectComboBox, &QComboBox::currentTextChanged, this, &BrowseSoundsDialog::filterSounds);
}

BrowseSoundsDialog::~BrowseSoundsDialog()
{
    clientTcpSocket->disconnect();
    delete ui;
}

void BrowseSoundsDialog::handleResponse(QNetworkReply* serverReply)
{
    if (serverReply->error() == QNetworkReply::NoError)
    {
        QJsonArray doc = QJsonDocument::fromJson(serverReply->readAll()).array();

        for (const QJsonValue& value : doc)
        {
            QJsonObject sound = value.toObject();
            QString soundName = sound.value("sound_name").toString();
            QString typeName = sound.value("type_name").toString();
            QString genreName = sound.value("genre_name").toString();
            int isPremium = sound.value("is_premium").toInt(); 

            typeSet.insert(typeName);
            genreSet.insert(genreName);

            SoundPrevieweWidget* soundPreviewe = new SoundPrevieweWidget(soundName, genreName, typeName, isPremium, this);

            connect(soundPreviewe, &SoundPrevieweWidget::playSound, this, &BrowseSoundsDialog::soundPrevieweClick);
            connect(soundPreviewe, &SoundPrevieweWidget::downloadSound, this, &BrowseSoundsDialog::downloadSoundClick);

            soundList.append(soundPreviewe);
            scrollAreaLayout->addWidget(soundPreviewe);
        }

        typeSet.insert("Any");
        genreSet.insert("Any");

        ui->genreSelectComboBox->addItems(QStringList(QList(this->genreSet.begin(), genreSet.end())));
        ui->typeSelectComboBox->addItems(QStringList(QList(this->typeSet.begin(), typeSet.end())));
    }
    else
    {
        QJsonObject jsonReply = QJsonDocument::fromJson(serverReply->readAll()).object();
        QMessageBox* errorMsgBox = new QMessageBox(QMessageBox::Warning, "Error", serverReply->errorString() + '\n' + jsonReply["error"].toString());
        errorMsgBox->show();
    }
}

void BrowseSoundsDialog::soundPrevieweClick(QString soundName)
{
    if (clientTcpSocket->state() == QAbstractSocket::ConnectedState)
    {
        clientTcpSocket->write(soundName.toUtf8());
        clientTcpSocket->flush();
    }
}

void BrowseSoundsDialog::handleUdp()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(), datagram.size());

        if (audioIODevice)
        {
            audioIODevice->write(datagram);
        }
    }   
}

void BrowseSoundsDialog::initSounds()
{
    QUrl url(this->serverUrl);

    QNetworkRequest soundsRequest(url);
    soundsRequest.setRawHeader(QString("Key").toUtf8(), authToken.toUtf8());

    QNetworkReply* serverReply = networkNamager->get(soundsRequest);

    connect(serverReply, &QNetworkReply::finished, this,
            [this, serverReply]()
            {
                this->handleResponse(serverReply);
            });
}

void BrowseSoundsDialog::downloadSoundClick(QString soundName)
{
    QUrl url = serverUrl.resolved(QUrl("/sound/download/" + QUrl::toPercentEncoding(soundName)));

    QNetworkRequest downloadRequest(url);
    downloadRequest.setRawHeader(QString("Key").toUtf8(), this->authToken.toUtf8());
    
    QNetworkReply* serverReply = this->networkNamager->get(downloadRequest);

    connect(serverReply, &QNetworkReply::finished, this,
            [this, serverReply]()
            {
                this->handleDownload(serverReply);
            });
}

void BrowseSoundsDialog::filterSounds()
{
    const QString selectedGenre = ui->genreSelectComboBox->currentText();
    const QString selectedType = ui->typeSelectComboBox->currentText();

    for (SoundPrevieweWidget* spw : soundList)
    {
        bool genreMismatch = (selectedGenre != "Any") && (spw->getSoundGenre() != selectedGenre);
        bool typeMismatch = (selectedType != "Any") && (spw->getSoundType() != selectedType);

        if (genreMismatch || typeMismatch)
        {
            spw->hide();
        }
        else
        {
            spw->show();
        }
    }
}

void BrowseSoundsDialog::handleDownload(QNetworkReply* serverReply)
{
    if (serverReply->error() == QNetworkReply::NoError)
    {
        QTemporaryFile* tempFile = new QTemporaryFile();
        if (!tempFile->open())
        {
            return;
            serverReply->deleteLater();
        }

        tempFile->write(serverReply->readAll());
        tempFile->flush();

        emit this->fileDownloaded(tempFile);
    }
    else
    {
        QJsonObject jsonReply = QJsonDocument::fromJson(serverReply->readAll()).object();
        QMessageBox* errorMsgBox = new QMessageBox(QMessageBox::Warning, "Error", serverReply->errorString() + '\n' + jsonReply["error"].toString() + 'n' + serverReply->errorString());
        errorMsgBox->show();
    }

    serverReply->deleteLater();

}

void BrowseSoundsDialog::connectToServer()
{
    if (clientTcpSocket->state() == QAbstractSocket::ConnectedState)
    {
        qInfo() << "Already connected to server!";
        return;
    }
    
    clientTcpSocket->connectToHost("192.168.1.19", 4000);
    if (!clientTcpSocket->waitForConnected(3000))
    {
        // qInfo() << "Connected to server!" << clientSocket->isValid();
        // qInfo() << "Server address: " << this->clientSocket->peerName();
        QMessageBox* errorMsgBox = new QMessageBox(QMessageBox::Warning, "Error", clientTcpSocket->errorString());
        errorMsgBox->show();
    }
}
