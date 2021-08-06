#ifndef SETTINGS_H
#define SETTINGS_H

#include <QList>
#include <qstring.h>

class Settings
{
private:
    static const QString SETTINGS_FILENAME;
    static const QString KEY_lastOpenedFolder;

    static const QString KEY_videopath1;
    static const QString KEY_videopath2;
    static const QString KEY_videopath3;
    static const QString KEY_videopath4;
    static const QString KEY_fcspath;
    //static const QString KEY_unfcspath;
    static const QString KEY_tracking_radius;

public:        
    Settings();

    QString lastOpenedFolder;
    QString videopath1;
    QString videopath2;
    QString videopath3;
    QString videopath4;
    QString fcspath;
    //QString unfcspath;

    int tracking_radius;

    bool Load();
    bool Save();
};

#endif // SETTINGS_H
