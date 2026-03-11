#ifndef SOUNDPREVIEWE_H
#define SOUNDPREVIEWE_H

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QWidget>

class SoundPrevieweWidget : public QGroupBox
{
    Q_OBJECT
  public:
    SoundPrevieweWidget(QString _soundName, QString _soundGenre, QString _soundType, int _isPremium, QWidget* parent = nullptr);

  private:
    QLabel* soundName;
    QLabel* soundGenre;
    QLabel* soundType;

    QHBoxLayout* soundPrevieweLayout;

    QPushButton* playButton;
    QPushButton* useButton;

    QWidget* initButton();

  signals:
    void playSound(QString name);
    void downloadSound(QString name);

  public:
    QString getSoundGenre();
    QString getSoundType();
};

#endif // SOUNDPREVIEWE_H
