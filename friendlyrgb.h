#ifndef FRIENDLYRGB_H
#define FRIENDLYRGB_H

#include "ciede2000.h"
#include "global.h"
#include <QString>



class FriendlyRGB
{
public:
    byte r{};
    byte g{};
    byte b{};

    FriendlyRGB();
    FriendlyRGB(byte rr, byte gg, byte bb);
    FriendlyRGB(const QString& h);

    static const FriendlyRGB RYB_RED;
    static const FriendlyRGB RYB_ORANGE;
    static const FriendlyRGB RYB_YELLOW;

    static const FriendlyRGB RYB_GREEN;
    static const FriendlyRGB RYB_BLUE;
    static const FriendlyRGB RYB_PURPLE;

    static const FriendlyRGB RYB_RO;
    static const FriendlyRGB RYB_YO;
    static const FriendlyRGB RYB_YG;

    static const FriendlyRGB RYB_BG;
    static const FriendlyRGB RYB_BP;
    static const FriendlyRGB RYB_RP;

    static const FriendlyRGB WheelColorsRYB[];
    static const int WheelColorsRYBLen;
    static const QString WheelColorsRYBNames[];
    static const QString WheelColorsRYBHumNames[];

    enum CsvType {txt, hex};
    static FriendlyRGB FromCSV(QString txt, CsvType ltype, bool *isok);

    static FriendlyRGB FromInt(int i);
    static QString GetName(int i);
    static int GetRYBIxWheelN(byte r, byte g, byte b, qreal* d_min, int n);
    static CIEDE2000::LAB toLab(byte sR, byte sG, byte sB);
    static QString GetFileName(const QString &name, int i);
    static int ToFriendlyInt(byte r, byte g, byte b);
    int toFriendlyInt(){ return ToFriendlyInt(r,g,b); };

    QString toHexString() const{ return toHexString(r,g,b);};
    static QString toHexString(byte r, byte g, byte b){
        return QString("#%1%2%3").arg(r, 2, 16, QChar('0')).arg(g, 2, 16, QChar('0')).arg(b, 2, 16, QChar('0'));
    };

    QString toString() const{ return toString(r,g,b);};
    static QString toString(byte r, byte g, byte b){
        return QString("%1,%2,%3").arg(r).arg(g).arg(b);
    };

    static int ToFriendlyInt(int i);

    static FriendlyRGB FromFriendlyInt(int i);
    static QString GetFileNameFilter(const QString &name, int i);
};

#endif // FRIENDLYRGB_H
