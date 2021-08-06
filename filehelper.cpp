#include "filehelper.h"

#include <QFile>
#include <QTextStream>

auto FileHelper::LoadText(const QString& fn) -> QStringList
{
    if(fn.isEmpty()) return {};
    QFile file(fn);
    if(!file.exists()) return {};
    if(!file.open(QIODevice::ReadOnly| QFile::Text)) return {};

    QStringList e;

    QTextStream st(&file);
    st.setCodec("UTF-8");

    while (!st.atEnd())
    {
        e << st.readLine();
    }
    file.close();

    return e;
}

auto FileHelper::SaveText(const QString& fn, const QStringList& lines) -> bool
{
    if(fn.isEmpty()) return false;

    QFile f(fn);

    auto om = QIODevice::WriteOnly | QIODevice::Text;

    if (!f.open(om)) return false;

    QTextStream out(&f);
    for(auto&l:lines)
    {
        out << l<<Qt::endl;
    }

    f.close();

    return true;
}

auto FileHelper::LoadIni(const QString& fn) ->QMap<QString, QString>
{
    QMap<QString, QString> map;

    auto lines = FileHelper::LoadText(fn);
    if(lines.isEmpty()) return {};
    for(auto&l:lines)
    {
        if(l.isEmpty()) continue;
        if(l.startsWith('#')) continue;
        if(l.length()<3) continue;
        int ix = l.indexOf('=');
        if(ix<1) continue;
        if(ix>=l.length()-1) continue;
        QString key = l.left(ix).trimmed();
        QString value = l.mid(ix+1).trimmed();
        if(key.isEmpty()) continue;
        map.insert(key, value);
    }

    return map;
}

auto FileHelper::SaveIni(const QString &fn, const QMap<QString, QString> &d) -> bool
{
    QStringList lines;
    if(d.isEmpty()) return false;

    for(auto i = d.begin();i!=d.end();++i)
    {
        lines.append(i.key()+'='+i.value());
    }
    if(lines.isEmpty()) return true;

    bool isOk = SaveText(fn, lines);

    return isOk;
}
