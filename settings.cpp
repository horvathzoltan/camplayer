#include "settings.h"
#include "filehelper.h"

#include <QDir>
#include <QMap>

const QString Settings::SETTINGS_FILENAME = QStringLiteral("settings.ini");
const QString Settings::KEY_lastOpenedFolder = QStringLiteral("lastOpenedFolder");

const QString Settings::KEY_videopath1 = QStringLiteral("videopath1");
const QString Settings::KEY_videopath2 = QStringLiteral("videopath2");
const QString Settings::KEY_videopath3 = QStringLiteral("videopath3");
const QString Settings::KEY_videopath4 = QStringLiteral("videopath4");
const QString Settings::KEY_fcspath = QStringLiteral("fcspath");
const QString Settings::KEY_tracking_radius = QStringLiteral("tracking_radius");
//const QString Settings::KEY_unfcspath = QStringLiteral("unfcspath");

Settings::Settings()
{
    lastOpenedFolder = QDir::homePath();
    videopath1 = QStringLiteral("front_cam");
    videopath2 = QStringLiteral("right_cam");
    videopath3 = QStringLiteral("back_cam");
    videopath4 = QStringLiteral("left_cam");
    fcspath = QStringLiteral("/mnt/shared/hwupdatefiles/a/fc_15_gene2");
    //unfcspath = QStringLiteral("/mnt/shared/hwupdatefiles/a/unfc_15_gene2");
    tracking_radius = 30;
}

auto Settings::Load() -> bool
{
    auto d = FileHelper::LoadIni(SETTINGS_FILENAME);
    if(d.isEmpty()) return false;
    if(d.contains(KEY_lastOpenedFolder)) lastOpenedFolder=d[KEY_lastOpenedFolder];

    if(d.contains(KEY_videopath1)) videopath1=d[KEY_videopath1];
    if(d.contains(KEY_videopath2)) videopath2=d[KEY_videopath2];
    if(d.contains(KEY_videopath3)) videopath3=d[KEY_videopath3];
    if(d.contains(KEY_videopath4)) videopath4=d[KEY_videopath4];

    if(d.contains(KEY_fcspath)) fcspath=d[KEY_fcspath];
    if(d.contains(KEY_tracking_radius)) tracking_radius=d[KEY_tracking_radius].toInt();

    return true;
}


auto Settings::Save() -> bool
{
    QMap<QString, QString> d;
    d.insert(KEY_lastOpenedFolder,lastOpenedFolder);
    d.insert(KEY_fcspath, fcspath);

    d.insert(KEY_videopath1, videopath1);
    d.insert(KEY_videopath2, videopath2);
    d.insert(KEY_videopath3, videopath3);
    d.insert(KEY_videopath4, videopath4);

    d.insert(KEY_tracking_radius, QString::number(tracking_radius));

    return FileHelper::SaveIni(SETTINGS_FILENAME, d);
}
