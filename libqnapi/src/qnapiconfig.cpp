/*****************************************************************************
** QNapi
** Copyright (C) 2008-2015 Piotr Krzemiński <pio.krzeminski@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*****************************************************************************/

#include "qnapiconfig.h"

QNapiConfig::QNapiConfig()
{
}

QNapiConfig::~QNapiConfig()
{
}

void QNapiConfig::load(QString appDirPath)
{
    if(settings) delete settings;

    QString localQNapiIniPath = appDirPath + QDir::separator() + "qnapi.ini";
    isPortableMode = QFileInfo(localQNapiIniPath).exists();

    if(isPortableMode)
    {
        settings = new QSettings(localQNapiIniPath, QSettings::IniFormat);
    }
    else
    {
        settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "qnapi");
    }

    performMigrations();

}

void QNapiConfig::save()
{
    settings->setValue("qnapi/version", QNAPI_VERSION);
    settings->setValue("qnapi/firstrun", false);
    settings->sync();
}

void QNapiConfig::performMigrations()
{
    if(!version().isEmpty() && version() <= "0.2.0") {
        // between 0.2.0 and 0.2.1 there was semantic change within search_policy
        // config item; SP_SEARCH_ALL and SP_BREAK_IF_FOUND were swapped

        if(searchPolicy() == SP_SEARCH_ALL)
            setSearchPolicy(SP_BREAK_IF_FOUND);
        else if(searchPolicy() == SP_BREAK_IF_FOUND)
            setSearchPolicy(SP_SEARCH_ALL);

        save();
    }
}


bool QNapiConfig::firstRun()
{
    return settings->value("qnapi/firstrun", true).toBool();
}

QString QNapiConfig::version()
{
    return settings->value("qnapi/version", "").toString();
}

QString QNapiConfig::p7zipPath()
{
    // Odczytujemy z konfiguracji
    QString p7z_path = settings->value("qnapi/7z_path", "").toString();
    if(!p7z_path.isEmpty() && QFileInfo(p7z_path).isExecutable())
        return p7z_path;

#ifdef Q_OS_WIN
    QString separator = ";";
#else
    QString separator = ":";
#endif

    // Przeszukiwanie sciezek systemowych i biezacego katalogu
    QString path = QProcess::systemEnvironment().filter(QRegExp("^PATH=(.*)$")).value(0);
    QStringList paths = path.mid(5).split(separator);
    paths.removeAll("");
    paths.append(".");

    if(paths.size() == 0)
        paths << "/bin" << "/usr/bin" << "/usr/local/bin";

#ifdef Q_OS_MAC
    // Pod MacOS X 7zip jest w zasobach aplikacji
    paths << QDir(QCoreApplication::applicationDirPath() + "/../Resources").canonicalPath();
#endif

    QStringList binaries;
    binaries << "7z" << "7za" << "7zr" << "p7zip";

    for(QStringList::iterator i = paths.begin(); i != paths.end(); i++)
    {
        for(QStringList::iterator j = binaries.begin(); j != binaries.end(); j++)
        {
            p7z_path = (*i) + "/" + (*j);
            if(QFileInfo(p7z_path).isExecutable())
                return QFileInfo(p7z_path).absoluteFilePath();
        }
    }


#ifdef Q_OS_WIN
    // Pod systemem Windows program 7za.exe musi byc w tym samym katalogu, co qnapi.exe
    p7z_path = QFileInfo(QCoreApplication::applicationDirPath() + "/7za.exe").absoluteFilePath();
    if(QFileInfo(p7z_path).isExecutable())
        return p7z_path;
#endif

    // Jesli wszystko inne zawiodlo...;)
    return "7z";
}

void QNapiConfig::setP7zipPath(const QString & path)
{
    settings->setValue("qnapi/7z_path", path);
}

QString QNapiConfig::tmpPath()
{
    QString tmpPath = settings->value("qnapi/tmp_path", QDir::tempPath()).toString();
    if(!QFileInfo(tmpPath).exists() || !QFileInfo(tmpPath).isWritable()) {
        if(isPortableMode) {
            tmpPath = QDir::tempPath();
        }
    }
    return tmpPath;
}

void QNapiConfig::setTmpPath(const QString & path)
{
    settings->setValue("qnapi/tmp_path", path);
}

QString QNapiConfig::nick(const QString & engine)
{
    QString nick = settings->value(engine + "/nick", "").toString();

    // Konwersja z konfigow <= 0.1.5
    if(nick.isEmpty() && (engine == "NapiProjekt"))
    {
        nick = settings->value("qnapi/nick", "").toString();

        if(!nick.isEmpty())
        {
            settings->remove("qnapi/nick");
            setNick("NapiProjekt", nick);
        }
    }

    return nick;
}

void QNapiConfig::setNick(const QString & engine, const QString & nick)
{
    settings->setValue(engine + "/nick", nick);
}

QString QNapiConfig::pass(const QString & engine)
{
    QString pass = settings->value(engine + "/pass", "").toString();

    if(pass.isEmpty() && (engine == "NapiProjekt"))
    {
        pass = settings->value("qnapi/pass", "").toString();

        if(!pass.isEmpty())
        {
            settings->remove("qnapi/pass");
            setPass("NapiProjekt", pass);
        }
    }

    return pass;
}

void QNapiConfig::setPass(const QString & engine, const QString & pass)
{
    settings->setValue(engine + "/pass", pass);
}

QString QNapiConfig::language()
{
    return settings->value("qnapi/language", "pl").toString();
}

void QNapiConfig::setLanguage(const QString & language)
{
    settings->setValue("qnapi/language", language);
}

QString QNapiConfig::languageBackup()
{
    return settings->value("qnapi/language_backup", "en").toString();
}

void QNapiConfig::setLanguageBackup(const QString & language)
{
    settings->setValue("qnapi/language_backup", language);
}

bool QNapiConfig::noBackup()
{
    return settings->value("qnapi/no_backup", false).toBool();
}

void QNapiConfig::setNoBackup(bool noBackup)
{
    settings->setValue("qnapi/no_backup", noBackup);
}

bool QNapiConfig::quietBatch()
{
#ifdef Q_OS_MAC
    return false;
#else
    return settings->value("qnapi/quiet_batch", false).toBool();
#endif
}

void QNapiConfig::setQuietBatch(bool quietBatch)
{
#ifndef Q_OS_MAC
    settings->setValue("qnapi/quiet_batch", quietBatch);
#else
    Q_UNUSED(quietBatch)
#endif
}

#ifdef Q_OS_MAC
bool QNapiConfig::showDockIcon()
{
    const bool show_default = true;
    
    QString infoPlistPath = QFileInfo(QCoreApplication::applicationDirPath() + "/../Info.plist").canonicalFilePath();

    QFile plistFile(infoPlistPath);
    
    QDomDocument doc;
    if(!doc.setContent(&plistFile))
        return show_default;

    if(!doc.hasChildNodes())
        return show_default;

    QDomNodeList nodes = doc.childNodes();
    
    QDomNode node;
    int i;
    for(i = 0; i < nodes.size(); ++i)
    {
        node = nodes.at(i);
        if(node.nodeName() == "plist")
            break;
    }
    
    if(i == nodes.size())
        return show_default;

    if(!node.hasChildNodes())
        return show_default;

    nodes = node.childNodes();
    
    for(i = 0; i < nodes.size(); ++i)
    {
        node = nodes.at(i);
        if(node.nodeName() == "dict")
            break;
    }

    if(i == nodes.size())
        return show_default;
    
    if(!node.hasChildNodes())
        return show_default;
    
    nodes = node.childNodes();

    for(i = 0; i < nodes.size(); ++i)
    {
        node = nodes.at(i);

        QString subText;

        if(node.hasChildNodes())
        {
            subText = node.childNodes().at(0).toText().data();
        }

        if(subText == "LSUIElement")
            break;
    }

    if(i < nodes.size())
    {
        node = node.nextSibling();
        return (node.nodeName() != "true");
    }

    return show_default;
    
}

void QNapiConfig::setShowDockIcon(bool show)
{
    QString infoPlistPath = QFileInfo(QCoreApplication::applicationDirPath() + "/../Info.plist").canonicalFilePath();
    
    QFile plistFile(infoPlistPath);
    
    QDomDocument doc;
    if(!doc.setContent(&plistFile) || !doc.hasChildNodes())
        return;
    
    QDomNodeList nodes = doc.childNodes();
    
    QDomNode node;
    int i;
    for(i = 0; i < nodes.size(); ++i)
    {
        node = nodes.at(i);
        if(node.nodeName() == "plist")
            break;
    }
    
    if((i == nodes.size()) || !node.hasChildNodes())
        return;
    
    nodes = node.childNodes();
    
    for(i = 0; i < nodes.size(); ++i)
    {
        node = nodes.at(i);
        if(node.nodeName() == "dict")
            break;
    }
    
    if((i == nodes.size()) || !node.hasChildNodes())
        return;
    
    nodes = node.childNodes();
    
    for(i = 0; i < nodes.size(); ++i)
    {
        node = nodes.at(i);
        
        QString subText;
        
        if(node.hasChildNodes())
        {
            subText = node.childNodes().at(0).toText().data();
        }
        
        if(subText == "LSUIElement")
            break;
    }
    
    if(i >= nodes.size())
        return;
    
    node = node.nextSibling();
    
    node.toElement().setTagName(show ? "false" : "true");

    QString modifiedContent = doc.toString(4);

    plistFile.close();
    
    if(!plistFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    QTextStream plistStream(&plistFile);
    
    plistStream << modifiedContent;
    
    plistFile.close();
    
}

#endif

QList<QPair<QString, bool> > QNapiConfig::engines()
{
    QList<QVariant> inList = settings->value("qnapi/engines").toList();
    QList<QPair<QString, bool> > map;
    QSet<QString> names;

    foreach(QVariant v, inList)
    {
        QStringList sl = v.toStringList();
        if(sl.size() != 2) continue;

        QString key = sl.at(0);
        bool value = (sl.at(1) == "1");
        map << qMakePair(key, value);
        names << key;
    }

    QStringList allNames;
    allNames << "NapiProjekt" << "Napisy24" << "OpenSubtitles";

    QString name;
    foreach(name, allNames) {
        if(!names.contains(name)) {
          map << qMakePair(name, true);
        }
    }

    return map;
}

QStringList QNapiConfig::enginesList()
{
    QList<QPair<QString, bool> > map = engines();
    QStringList list;

    for(int i = 0; i < map.size(); ++i)
    {
        QPair<QString,bool> e = map.at(i);
        if(e.second) list << e.first;
    }

    if(list.isEmpty())
    {
        list << "NapiProjekt" << "OpenSubtitles" << "Napisy24";
    }

    return list;
}

void QNapiConfig::setEngines(QList<QPair<QString, bool> > engines)
{
    QList<QVariant> outList;
    for(int i = 0; i < engines.size(); ++i)
    {
        QPair<QString, bool> e = engines.at(i);

        QStringList sl;
        sl << e.first << (e.second ? "1" : "0");
        outList << sl;
    }

    settings->setValue("qnapi/engines", outList);
}

SearchPolicy QNapiConfig::searchPolicy()
{
    return (SearchPolicy) settings->value("qnapi/search_policy", 0).toInt();
}

void QNapiConfig::setSearchPolicy(SearchPolicy policy)
{
    settings->setValue("qnapi/search_policy", policy);
}

DownloadPolicy QNapiConfig::downloadPolicy()
{
    return (DownloadPolicy) settings->value("qnapi/download_policy", 1).toInt();
}

void QNapiConfig::setDownloadPolicy(DownloadPolicy policy)
{
    settings->setValue("qnapi/download_policy", policy);
}

bool QNapiConfig::ppEnabled()
{
    return settings->value("qnapi/post_processing", false).toBool();
}

void QNapiConfig::setPpEnabled(bool enable)
{
    settings->setValue("qnapi/post_processing", enable);
}

ChangeEncodingMethod QNapiConfig::ppEncodingMethod() {
    if(settings->contains("qnapi/change_encoding")) {
        bool encodingEnabled = settings->value("qnapi/change_encoding", false).toBool();
        settings->remove("qnapi/change_encoding");

        ChangeEncodingMethod method = encodingEnabled ? CEM_CHANGE : CEM_ORIGINAL;

        setPpEncodingMethod(method);
        return method;
    }

    return (ChangeEncodingMethod) settings->value("qnapi/encoding_method", 0).toInt();
}


void QNapiConfig::setPpEncodingMethod(ChangeEncodingMethod method) {
    settings->setValue("qnapi/encoding_method", method);
}

bool QNapiConfig::ppAutoDetectEncoding()
{
    return settings->value("qnapi/auto_detect_encoding", true).toBool();
}

void QNapiConfig::setPpAutoDetectEncoding(bool change)
{
    settings->setValue("qnapi/auto_detect_encoding", change);
}

QString QNapiConfig::ppEncodingFrom()
{
    return settings->value("qnapi/enc_from", "windows-1250").toString();
}

void QNapiConfig::setPpEncodingFrom(const QString & encoding)
{
    settings->setValue("qnapi/enc_from", encoding);
}

QString QNapiConfig::ppEncodingTo()
{
    return settings->value("qnapi/enc_to", "UTF-8").toString();
}

void QNapiConfig::setPpEncodingTo(const QString & encoding)
{
    settings->setValue("qnapi/enc_to", encoding);
}

bool QNapiConfig::ppShowAllEncodings()
{
    return settings->value("qnapi/show_all_encodings", false).toBool();
}

void QNapiConfig::setPpShowAllEncodings(bool show)
{
    settings->setValue("qnapi/show_all_encodings", show);
}

bool QNapiConfig::ppRemoveLines()
{
    return settings->value("qnapi/remove_lines", false).toBool();
}

void QNapiConfig::setPpRemoveLines(bool remove)
{
    settings->setValue("qnapi/remove_lines", remove);
}

QString QNapiConfig::ppSubFormat()
{
    return settings->value("qnapi/sub_format", "").toString();
}

void QNapiConfig::setPpSubFormat(const QString & subFormat)
{
    settings->setValue("qnapi/sub_format", subFormat);
}

QString QNapiConfig::ppSubExtension()
{
    return settings->value("qnapi/sub_ext", "").toString();
}

void QNapiConfig::setPpSubExtension(const QString & subExtension)
{
    settings->setValue("qnapi/sub_ext", subExtension);
}

bool QNapiConfig::ppSkipConvertAds()
{
    return settings->value("qnapi/skip_convert_ads", false).toBool();
}

void QNapiConfig::setPpSkipConvertAds(bool skip)
{
    settings->setValue("qnapi/skip_convert_ads", skip);
}

QStringList QNapiConfig::ppRemoveWords()
{
    QStringList defaultRemoveWords;
    defaultRemoveWords << "movie info" << "synchro ";

    return settings->value("qnapi/remove_words", defaultRemoveWords).toStringList();
}

void QNapiConfig::setPpRemoveWords(const QStringList & words)
{
    settings->setValue("qnapi/remove_words", words);
}

bool QNapiConfig::changePermissions()
{
    return settings->value("qnapi/change_permissions", false).toBool();
}

void QNapiConfig::setChangePermissions(bool change)
{
    settings->setValue("qnapi/change_permissions", change);
}

QString QNapiConfig::changePermissionsTo()
{
    bool ok;
    int perm = settings->value("qnapi/permissions", 644).toInt(&ok);
    if(!ok || perm > 777)
        perm = 644;
    QString str = QString("%1").arg(perm);
    while(str.size() < 3)
        str = QString("0") + str;
    return str;
}

void QNapiConfig::setChangePermissionsTo(const QString & permissions)
{
    bool ok;
    int perm = permissions.toInt(&ok);
    if(ok && perm <= 777)
    {
        QString str = QString("%1").arg(perm);
        while(str.size() < 3)
            str = QString("0") + str;
        settings->setValue("qnapi/permissions", str);
    }
}

QString QNapiConfig::previousDialogPath()
{
    QString path = settings->value("qnapi/prev_dialog_path", "").toString();
    return (QDir(path).exists() ? path : QDir::homePath());
}

void QNapiConfig::setPreviousDialogPath(const QString & path)
{
    settings->setValue("qnapi/prev_dialog_path", path);
}

QStringList QNapiConfig::scanFilters()
{
    QStringList defaultScanFilters;

    defaultScanFilters << movieExtensionsFilter() << "*.*";

    return settings->value("scan/filters", defaultScanFilters).toStringList();
}

void QNapiConfig::setScanFilters(const QStringList & filters)
{
    settings->setValue("scan/filters", filters);
}

QString QNapiConfig::scanSkipFilters()
{
    return settings->value("scan/skip_filters", "PL dubbing").toString();
}

void QNapiConfig::setScanSkipFilters(const QString & filters)
{
    settings->setValue("scan/skip_filters", filters);
}

bool QNapiConfig::scanSkipIfSubtitlesExists()
{
    return settings->value("scan/skip_if_subtitles_exists", false).toBool();
}

void QNapiConfig::setScanSkipIfSubtitlesExists(bool skip)
{
    settings->setValue("scan/skip_if_subtitles_exists", skip);
}

QString QNapiConfig::lastScanDir()
{
    return settings->value("scan/last_scan_dir", "").toString();
}

void QNapiConfig::setLastScanDir(const QString & dir)
{
    settings->setValue("scan/last_scan_dir", dir);
}

QStringList QNapiConfig::movieExtensions()
{
    QStringList exts = {
        "avi", "asf", "divx", "mkv", "mov", "mp4", "mpeg", "mpg", "ogm", "rm", "rmvb", "wmv"
    };
    return exts;
}

QString QNapiConfig::movieExtensionsFilter()
{
    QStringList moviePatterns;
    foreach(QString ext, movieExtensions())
    {
        moviePatterns << "*." + ext;
    }
    return moviePatterns.join(" ");
}


QStringList QNapiConfig::subtitleExtensions()
{
    QStringList exts = {
        "srt", "sub", "txt"
    };
    return exts;
}

QString QNapiConfig::subtitleExtensionsFilter()
{
    QStringList subtitlePatterns;
    foreach(QString ext, subtitleExtensions())
    {
        subtitlePatterns << "*." + ext;
    }
    return subtitlePatterns.join(" ");
}


QNapiConfig & GlobalConfig()
{
    static QNapiConfig cfg;
    return cfg;
}
