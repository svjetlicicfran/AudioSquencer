#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "browsesoundsdialog.h"
#include "databaseeditor.h"
#include "freesoundbrowser.h"
#include "logindialog.h"
#include "registerdialog.h"
#include "track.h"
#include "xmlfilereader.h"
#include "xmlwriter.h"
#include "logseditor.h"

#include <QAudioDevice>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMediaDevices>
#include <QRadioButton>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <qurl.h>


// API key
// cN0MFGKwOo0suRNBOtAxDZrtnQRxBZrkP5bWSVln

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private slots:
    void addTrack();
    void addTempTrack(QTemporaryFile* tempFile);
    void addSavedTrack(const QString fileName, int mask);

    void startPlayBack();
    void stopPlayBack();
    void advanceStep();
    void updateTempo(int val);

  private:
    Ui::MainWindow* ui;
    QWidget* srollAreaContainer;
    QVBoxLayout* scrollAreaLayout;

    QUrl serverUrl;

    QString authToken;

    XMLWriter xmlWritter;
    XMLFileReader xmlReader;

    QTimer* sequencerTimer;
    int currStep;
    QList<Track*> tracks;

    bool checkSoundDuration(const QString& filePath);
    QString getFromFileDialog();
};
#endif // MAINWINDOW_H
