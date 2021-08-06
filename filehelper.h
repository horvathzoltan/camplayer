#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QStringList>



class FileHelper
{
public:
    static QStringList LoadText(const QString& fn);
    static QMap<QString,QString> LoadIni(const QString &fn);
    static bool SaveIni(const QString &fn, const QMap<QString,QString>&);
    static bool SaveText(const QString &fn, const QStringList&);
};

#endif // FILEHELPER_H
