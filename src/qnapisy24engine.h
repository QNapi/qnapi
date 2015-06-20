#ifndef QNAPISY24ENGINE_H
#define QNAPISY24ENGINE_H

#include "qnapiabstractengine.h"
#include "synchttp.h"
#include <QUrl>
#include <QUrlQuery>

class QNapisy24Engine : public QNapiAbstractEngine
{
public:
    QNapisy24Engine(const QString & movieFile = "", const QString & subtitlesFile = "");
    ~QNapisy24Engine();

    QString engineName();
    QString engineInfo();
    QIcon engineIcon();
    bool isConfigurable();
    void configure(QWidget * parent);

    QString checksum(QString filename = "");

    bool lookForSubtitles(QString lang);
    QList<QNapiSubtitleInfo> listSubtitles();
    bool download(int idx);
    bool unpack();
    void cleanup();

private:
    quint64 fileSize;
    QString p7zipPath, lang, tmpPackedFile, subFileName, fileName;
    QList<QNapiSubtitleInfo> subtitlesList;
};

#endif // QNAPISY24ENGINE_H
