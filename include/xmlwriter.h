#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QObject>
#include <QString>
#include <QXmlStreamWriter>

#if defined(XMLFILESAVER_LIBRARY)
#define XMLFILESAVER_EXPORT Q_DECL_EXPORT
#else
#define XMLFILESAVER_EXPORT Q_DECL_IMPORT
#endif

class XMLFILESAVER_EXPORT XMLWriter
{
  public:
    XMLWriter(const QString& filePath);
    XMLWriter();
    ~XMLWriter();

    bool writeSequencesToXml(const QList<QStringList>& sequences);
    bool openFile(QString filePath);
    //bool openFile(QFile file);

  private:
    QFile xmlFile;
    QXmlStreamWriter xmlWriter;
};

QString createSaveFile();

QString createLogsSaveFile();

#endif
