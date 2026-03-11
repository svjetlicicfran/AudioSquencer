#ifndef XMLFILEREADER_H
#define XMLFILEREADER_H

#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QXmlStreamReader>

struct Sequence
{
    QString trackName;
    QString filePath;
    int steps;
};

class XMLFileReader
{
  public:
    bool load(const QString& filePath);
    void readFile();

    QList<Sequence>& sequences();

    XMLFileReader();

  private:
    QList<Sequence> songSequences;
    QFile file;
};

QString getXmlFileFromDialog();

#endif // XMLFILEREADER_H
