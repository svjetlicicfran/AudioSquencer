#include "soundpreviewe.h"

SoundPrevieweWidget::SoundPrevieweWidget(QString _soundName, QString _soundGenre, QString _soundType, int _isPremium, QWidget* parent)
    : QGroupBox(parent), soundPrevieweLayout(new QHBoxLayout), soundName(new QLabel(this)), soundType(new QLabel(this)), soundGenre(new QLabel(this)),
      playButton(new QPushButton(this)), useButton(new QPushButton(this))
{
    setFixedHeight(150);
    setTitle(_soundName);

    soundName->setText(_soundName);
    soundGenre->setText(_soundGenre);
    soundType->setText(_soundType);

    setLayout(this->soundPrevieweLayout);

    soundPrevieweLayout->addWidget(soundName);
    soundPrevieweLayout->addWidget(soundGenre);
    soundPrevieweLayout->addWidget(soundType);

    soundPrevieweLayout->addWidget(this->initButton());

    if(_isPremium){
        this->setObjectName("parentWidget");
        this->setStyleSheet("#parentWidget { border: 1px solid #D5845C; border-radius: 5px }");
    }

}

QWidget* SoundPrevieweWidget::initButton()
{
    QWidget* container = new QWidget(this);
    QVBoxLayout* buttonLayout = new QVBoxLayout(container);

    playButton->setText("Play");
    playButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    useButton->setText("Use");
    useButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    connect(playButton, &QPushButton::clicked, this, [this]() { emit this->playSound(this->soundName->text()); });
    connect(useButton, &QPushButton::clicked, this, [this]() { emit this->downloadSound(this->soundName->text()); });

    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(useButton);

    return container;
}

QString SoundPrevieweWidget::getSoundGenre()
{
    return this->soundGenre->text();
}

QString SoundPrevieweWidget::getSoundType()
{
    return this->soundType->text();
}
