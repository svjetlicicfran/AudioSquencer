#ifndef TRACK_H
#define TRACK_H

#include "constanst.h"
#include "stepbutton.h"
#include "waveformwidget.h"
#include "waveformworker.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QObject>
#include <QSizePolicy>
#include <QSoundEffect>
#include <QStyle>
#include <QThread>
#include <QTimer>
#include <QUrl>
#include <QFileInfo>
#include <QWidget>

class Track : public QGroupBox
{
    Q_OBJECT
  public:
    Track(const QString& filePath, QWidget* parent = nullptr);

    Track(const QString& filePath, int buttonMask, QWidget* parent = nullptr);

    // static QTimer globalTimer;   // shared among all tracks
    // static void startSequencer(int intervalMs);
    static void startTimer(const int msec);
    static void stopTimer();

    void nextStep(int currStep);
    void clearSteppButtonHighlights(void);

    QList<StepButton*> getStepButtons();

    QUrl getSoundEffectPath();

  signals:
    void deleteTrack(void);
    void stopTrack(void);
    void playTrack(void);

  private:
    QWidget* controlButtonSetup(void);
    QWidget* setpButtonSetup(void);
    QWidget* setpMaskButtonSetup(int mask);

    void waveFormSetUp(const QString& filePath);

    QSoundEffect* trackSoundEffect;
    QList<StepButton*> stepButtons;

    WaveformWidget* waveFormWidget;
    // WaveformWorker waveFormWorker;

    QHBoxLayout* trackLayout;

    QPushButton* deleteBtn;
    QPushButton* playBtn;
    QPushButton* stopBtn;
};

#endif // TRACK_H
