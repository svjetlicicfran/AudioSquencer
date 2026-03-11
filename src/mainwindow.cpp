#include "mainwindow.h"

#include "ui_mainwindow.h"
#include <qnamespace.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), srollAreaContainer(new QWidget(this)), scrollAreaLayout(new QVBoxLayout(srollAreaContainer)),
      sequencerTimer(new QTimer(this)), currStep(0), authToken("")
{
    ui->setupUi(this);
    this->setWindowTitle("Music sequencer");

    serverUrl.setHost("192.168.1.19");
    serverUrl.setScheme("http");
    serverUrl.setPort(3021);


    ui->actionEditDatabase->setVisible(false);
    ui->actionEditLogs->setVisible(false);

    srollAreaContainer->setLayout(this->scrollAreaLayout);
    ui->scrollArea->setWidget(srollAreaContainer);

    connect(ui->addSoundButton, &QPushButton::clicked, this, &MainWindow::addTrack);
    connect(ui->playPushButton, &QPushButton::clicked, this, &MainWindow::startPlayBack);
    connect(ui->stopPushButton, &QPushButton::clicked, this, &MainWindow::stopPlayBack);
    connect(sequencerTimer, &QTimer::timeout, this, &MainWindow::advanceStep);
    connect(ui->spinBoxTempo, &QSpinBox::valueChanged, this, &MainWindow::updateTempo);

    connect(this->ui->registerActionButton, &QAction::triggered, this,
            [this]()
            {
                RegisterDialog* rd = new RegisterDialog(serverUrl, this);
                rd->setAttribute(Qt::WA_DeleteOnClose);
                rd->show();
            });

    connect(this->ui->loginActionButton, &QAction::triggered, this,
            [this]()
            {
                LoginDialog* ld = new LoginDialog(serverUrl, this);
                connect(ld, &LoginDialog::loginSuccess, this,
                        [this](const QString authToken, bool isAdmin, const QString username)
                        {
                            ui->loggedInLabel->setText("Loged in as: " + username);
                            this->authToken = authToken;
                            ui->actionEditDatabase->setVisible(isAdmin);
                            ui->actionEditLogs->setVisible(isAdmin);
                        });
                ld->setAttribute(Qt::WA_DeleteOnClose);

                ld->show();

            });

    connect(this->ui->browseActionButton, &QAction::triggered, this,
            [this]()
            {
                if (this->authToken == "")
                {
                    QMessageBox* succesMsgBox = new QMessageBox(QMessageBox::Warning, "Error", QString("Not logged in"));
                    succesMsgBox->show();
                    return;
                }

                BrowseSoundsDialog* bsd = new BrowseSoundsDialog(authToken, serverUrl, this);

                bsd->setAttribute(Qt::WA_DeleteOnClose);
                bsd->show();

                connect(bsd, &BrowseSoundsDialog::fileDownloaded, this, &MainWindow::addTempTrack);
            });

    connect(this->ui->actionMoreSoundsButton, &QAction::triggered, this,
            [this]()
            {
                FreeSoundBrowser* fsb = new FreeSoundBrowser(this);

                fsb->setAttribute(Qt::WA_DeleteOnClose);
                fsb->show();

                connect(fsb, &FreeSoundBrowser::fileDownloaded, this, &MainWindow::addTempTrack);
                //connect(fsb, &QDialog::finished, this, []() { qInfo() << "finished"; });
            });

    connect(this->ui->actionEditDatabase, &QAction::triggered, this,
            [this]()
            {
                DatabaseEditor* dbe = new DatabaseEditor(serverUrl, authToken, this);

                dbe->setAttribute(Qt::WA_DeleteOnClose);
                dbe->show();
            });

    connect(ui->actionSaveButton, &QAction::triggered, this,
            [this]()
            {
                QString saveFile = createSaveFile();

                if (xmlWritter.openFile(saveFile))
                {
                    return;
                }

                QList<QStringList> trackList;

                for (int i = 0; i < tracks.size(); i++)
                {
                    QStringList trackInfo;

                    QString trackSoundEffectPath = tracks[i]->getSoundEffectPath().toString(QUrl::PreferLocalFile);
                    QString trackName = *(tracks[i]->getSoundEffectPath().toString().split(QDir::separator()).end() - 1);

                    QList<StepButton*> buttons = tracks[i]->getStepButtons();
                    int base = 0, sum = 0;
                    for (int j = 0; j < buttons.size(); j++)
                    {
                        sum += pow(2, base) * buttons[j]->isChecked();
                        base += 1;
                    }

                    trackInfo.append(trackName);
                    trackInfo.append(trackSoundEffectPath);
                    trackInfo.append(QString::number(sum));

                    trackList.append(trackInfo);
                }

                xmlWritter.writeSequencesToXml(trackList);
            });

    connect(ui->actionOpenButton, &QAction::triggered, this,
            [this]()
            {
                //qInfo() << "Radi";
                QString xmlPath = getXmlFileFromDialog();

                if (xmlPath == "")
                {
                    return;
                }
                if (!xmlReader.load(xmlPath))
                {
                    return;
                }

                xmlReader.readFile();

                for (QList<Sequence>::const_iterator it = xmlReader.sequences().begin(); it != xmlReader.sequences().end(); it++)
                {

                    // qInfo() << it->filePath << "  " << it->steps << " " << it->trackName;
                    addSavedTrack(it->filePath, it->steps);
                }
            });

    connect(ui->actionEditLogs, &QAction::triggered, this,
            [this]()
            {
                LogsEditor* logsEditor = new LogsEditor(authToken, this->serverUrl);
                
                logsEditor->setAttribute(Qt::WA_DeleteOnClose);
                logsEditor->show();

            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTrack()
{
    QString soundPath = this->getFromFileDialog();

    if (soundPath == "")
    {
        return;
    }

    Track* track = new Track(soundPath, this);

    this->scrollAreaLayout->addWidget(track);
    this->tracks.push_back(track);

    connect(track, &Track::deleteTrack, this,
            [this, track]()
            {
                this->tracks.removeOne(track);
                this->stopPlayBack();

                this->scrollAreaLayout->removeWidget(track);
                track->deleteLater();
            });
}

void MainWindow::addTempTrack(QTemporaryFile* tempFile)
{
    QString soundPath = tempFile->fileName();

    if (soundPath == "")
    {
        return;
    }

    Track* track = new Track(soundPath, this);

    this->scrollAreaLayout->addWidget(track);
    this->tracks.push_back(track);

    connect(track, &Track::deleteTrack, this,
            [this, track, tempFile]()
            {
                this->tracks.removeOne(track);
                this->stopPlayBack();

                this->scrollAreaLayout->removeWidget(track);
                tempFile->deleteLater();
                track->deleteLater();
            });
}

void MainWindow::addSavedTrack(const QString fileName, int mask)
{
    if (!QFile::exists(fileName))
    {
        return;
    }

    // qInfo() << fileName;

    Track* track = new Track(fileName, mask, this);

    this->scrollAreaLayout->addWidget(track);
    this->tracks.push_back(track);

    connect(track, &Track::deleteTrack, this,
            [this, track]()
            {
                this->tracks.removeOne(track);
                this->stopPlayBack();

                this->scrollAreaLayout->removeWidget(track);
                track->deleteLater();
            });
}

void MainWindow::advanceStep()
{
    if (currStep >= Constants::SETP_COUNT)
    {
        currStep = 0;
    }

    for (Track* t : this->tracks)
    {
        t->nextStep(currStep);
    }

    currStep++;
}

void MainWindow::updateTempo(int val)
{
    this->sequencerTimer->setInterval(val);
}

void MainWindow::startPlayBack()
{
    this->sequencerTimer->start(this->ui->spinBoxTempo->value());
}

void MainWindow::stopPlayBack()
{
    if (this->sequencerTimer->isActive())
    {
        this->sequencerTimer->stop();
    }
    this->currStep = 0;

    for (Track* t : this->tracks)
    {
        t->clearSteppButtonHighlights();
    }
}

QString MainWindow::getFromFileDialog()
{
    return QFileDialog::getOpenFileName(this, "Choose a sound file", QDir::homePath(), "Audio Files (*.wav)");
}


bool MainWindow::checkSoundDuration(const QString& filePath)
{
    QMediaPlayer* probe = new QMediaPlayer;

    bool soundTooLong = false;

    QMessageBox* msgBox = new QMessageBox();

    msgBox->resize(426, 240);

    probe->setSource(QUrl::fromLocalFile(filePath));

    connect(probe, &QMediaPlayer::durationChanged,
            [probe, &soundTooLong, &msgBox](qint64 duration)
            {
                if (duration > 4000)
                {
                    msgBox->setText(QString("Failed to load file: Duration %1 seconds is too long").arg(QString::number(duration / 1000.0)));
                    soundTooLong = true;
                }
                else
                {
                    msgBox->setText("File loaded succsessfuly.");
                    soundTooLong = false;
                }

                probe->deleteLater();
            });

    msgBox->exec();

    return soundTooLong;
}

// Foot
