#include "track.h"

Track::Track(const QString& filePath, QWidget* parent)
    : QGroupBox(parent), trackLayout(new QHBoxLayout(this)), trackSoundEffect(new QSoundEffect(this)), deleteBtn(new QPushButton(this)),
      playBtn(new QPushButton(this)), stopBtn(new QPushButton(this)), waveFormWidget(new WaveformWidget(this))
{
    this->trackSoundEffect->setSource(QUrl::fromLocalFile(filePath));
    this->setFixedHeight(160);
    this->setLayout(this->trackLayout);
    //this->setTitle(*(this->trackSoundEffect->source().url().split(QDir::separator()).end() - 1));
    this->setTitle(QFileInfo(filePath).fileName());
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

    this->trackLayout->addWidget(this->waveFormWidget);
    this->trackLayout->addWidget(this->setpButtonSetup());
    this->trackLayout->addWidget(this->controlButtonSetup());

    this->waveFormSetUp(filePath);

    connect(this->deleteBtn, &QPushButton::clicked, this, [this]() { emit this->deleteTrack(); });
    connect(this->playBtn, &QPushButton::clicked, this, [this]() { emit this->playTrack(); });
    connect(this->stopBtn, &QPushButton::clicked, this, [this]() { emit this->stopTrack(); });
}

Track::Track(const QString& filePath, int buttonMask, QWidget* parent)
    : QGroupBox(parent), trackLayout(new QHBoxLayout(this)), trackSoundEffect(new QSoundEffect(this)), deleteBtn(new QPushButton(this)),
      playBtn(new QPushButton(this)), stopBtn(new QPushButton(this)), waveFormWidget(new WaveformWidget(this))
{
    this->trackSoundEffect->setSource(QUrl::fromLocalFile(filePath));
    this->setFixedHeight(160);
    this->setLayout(this->trackLayout);

    this->setTitle(QFileInfo(filePath).fileName());
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

    this->trackLayout->addWidget(this->waveFormWidget);
    this->trackLayout->addWidget(this->setpMaskButtonSetup(buttonMask));
    this->trackLayout->addWidget(this->controlButtonSetup());

    this->waveFormSetUp(filePath);

    connect(this->deleteBtn, &QPushButton::clicked, this, [this]() { emit this->deleteTrack(); });
    connect(this->playBtn, &QPushButton::clicked, this, [this]() { emit this->playTrack(); });
    connect(this->stopBtn, &QPushButton::clicked, this, [this]() { emit this->stopTrack(); });
}
void Track::nextStep(int currStep)
{
    int prev = (currStep == 0 ? stepButtons.size() - 1 : currStep - 1);

    stepButtons[prev]->unHighlightStep();
    stepButtons[currStep]->highlightStep();

    if (this->stepButtons.at(currStep)->isChecked())
    {
        this->trackSoundEffect->play();
    }
}

void Track::clearSteppButtonHighlights(void)
{
    for (StepButton* b : stepButtons)
    {
        b->unHighlightStep();
    }
}

QList<StepButton*> Track::getStepButtons()
{
    return this->stepButtons;
}

QUrl Track::getSoundEffectPath()
{
    return this->trackSoundEffect->source();
}

QWidget* Track::controlButtonSetup()
{
    QWidget* container = new QWidget(this);
    QVBoxLayout* buttonLayout = new QVBoxLayout(container);
    container->setFixedWidth(100);

    this->deleteBtn->setText("Delete");
    this->deleteBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    this->playBtn->setText("Play");
    this->playBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    this->stopBtn->setText("Stop");
    this->stopBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    buttonLayout->addWidget(this->playBtn);
    buttonLayout->addWidget(this->stopBtn);
    buttonLayout->addWidget(this->deleteBtn);

    return container;
}

QWidget* Track::setpButtonSetup()
{
    QWidget* container = new QWidget(this);
    QHBoxLayout* stepTrack = new QHBoxLayout(container);

    for (int i = 0; i < Constants::SETP_COUNT; i++)
    {
        StepButton* button = new StepButton(this);
        button->setFixedSize(Constants::SETP_BUTTON_WIDTH, Constants::STEP_BUTTON_HEIGTH);

        connect(button, &StepButton::clicked, button, &StepButton::setChecked);

        stepButtons.push_back(button);
        stepTrack->addWidget(button);
    }

    return container;
}

QWidget* Track::setpMaskButtonSetup(int mask)
{
    QWidget* container = new QWidget(this);
    QHBoxLayout* stepTrack = new QHBoxLayout(container);

    for (int i = 0; i < Constants::SETP_COUNT; i++)
    {
        StepButton* button = new StepButton(this);
        button->setFixedSize(Constants::SETP_BUTTON_WIDTH, Constants::STEP_BUTTON_HEIGTH);

        if (mask % 2)
        {
            button->setChecked();
        }
        mask /= 2;

        connect(button, &StepButton::clicked, button, &StepButton::setChecked);

        stepButtons.push_back(button);
        stepTrack->addWidget(button);
    }

    return container;
}

void Track::waveFormSetUp(const QString& filePath)
{
    QThread* thread = new QThread();
    WaveformWorker* worker = new WaveformWorker(filePath);

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &WaveformWorker::process);
    connect(worker, &WaveformWorker::waveformReady, this->waveFormWidget, &WaveformWidget::setWaveform);
    connect(worker, &WaveformWorker::waveformReady, worker, &QObject::deleteLater);
    connect(worker, &WaveformWorker::waveformReady, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}
