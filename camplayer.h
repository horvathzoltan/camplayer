#ifndef CAMPLAYER_H
#define CAMPLAYER_H

#include "ciede2000.h"
#include "friendlyrgb.h"
#include <QDateTime>
#include <QMap>
#include <QSet>
#include <QString>
#include <QWidget>

#define BALL_LENGTH 16

typedef unsigned char byte;

class CamPlayer
{
public:
    static bool isInited;
    static void init();
    enum FilterMode:int{ Copy=0, IsFriendly, AllFriendly};

    struct TrackingColor{
        QColor color;
        int friendly_int;
        FriendlyRGB fc;
    };

    struct TrackingData{
        int vix = -1;
        int fix = -1;
        int bix = -1;
        int fcix = -1;

        int x=-1;
        int y=-1;

        FilterMode filtermode = FilterMode::IsFriendly;

        bool isValid(){return vix!=-1&&(bix!=-1||x>=0||y>=0);}

        QImage image;

        TrackingColor trackingcolor;

        QString toString(){
            QString tracking_txt = QString::number(vix);
            tracking_txt+=' '+((bix>-1)?QString::number(bix):QStringLiteral("?"));
            tracking_txt += ' '+QString::number(fcix);
            if(isValid()) tracking_txt+=(bix!=-1)?'*':'+';
            return tracking_txt;
        }

        QSet<int> ffcs;
        //QSet<int> fcs;
    };
    static TrackingData trackingdata;

    struct FriendlyColorData{
        QString fileName;
        int ix;
        QSet<int> fcs;
    };

    static const int _fcs_length = 6;

    static FriendlyColorData _fcs[_fcs_length];
    static FriendlyColorData _unfcs[_fcs_length]; // minuszos
    static FriendlyColorData _fcs_extra[_fcs_length]; // plusszos

    struct BallData{
        QString txt;

        int id; //0
        bool is_valid_for_tracking;//1
        QString shortname;//2
        qreal x;//3
        qreal y;//4
        int diameter;//5
        int fpxs;//6
        int connected;//7
        int partitions;//8
        int partitions_too_small;//9
        int partitions_too_large;//10
        int partitions_ok;//11
        int found_total;//12
        int not_found_ftotal;//13
        int not_found_last_not_ok;//14
        int found_last_ok;//15
//        qreal x;//16
//        qreal y;//17
        int quality;//18fcsd
        // kalibráció esetén nincs Q mivel azt az interpoláció hozza létre
        // Q = 0, ha nincs találat, 1 ha van, 2, 3 ha interpolált (lineáris, illetve bezier)
        // itt a kamerában kell megoldani, hogy get_pic esetén is legyen Q, ami találat esetén 1, interpoláció ugyanúgy nincs -
        // ha trackelhető és (volt találat vagy ha nem, interpoláció volt)

        bool isVisible() const{
            return is_valid_for_tracking && (found_last_ok > 0 || quality > 0);
        }
    };

    struct FrameMetaData{        
        QString txt;
        int frameix;
        QDateTime timestamp;
        int balls;
        QMap<int,BallData> balldata;
        int videoix;
    };

    struct FrameData
    {
        QString fileName;
        QImage image;
        bool balls[BALL_LENGTH];
        FrameMetaData metadata;
        int videoix;
    };

    struct VideoData{
        int ix;
        QString folder;
        int maxframeix;
        QMap<int, FrameData> frames;
    };

    static QString videoFolder;
    static QString fcsFolder;
    static VideoData video1;
    static VideoData video2;
    static VideoData video3;
    static VideoData video4;

    struct LoadR
    {
        QString folderName;

    };
    static LoadR Load(QWidget*);

    struct SettingsR
    {
        QString video_folderName;
        QString fcs_folderName;
    };

    static SettingsR Settings();
    static bool LoadExam(const QString&);
    static bool LoadVideoData(const QString&, const QString&, VideoData&);

    static int frameix;
    static int maxframeix;
    struct ShowFrameR
    {
        const FrameData* framedata1;
        const FrameData* framedata2;
        const FrameData* framedata3;
        const FrameData* framedata4;
        int frameix;


    };

    static VideoData* GetVideoData(int ix);
    static ShowFrameR NextFrame();
    static ShowFrameR PrevFrame();
    static ShowFrameR ShowFrame(int ix);

    static QString GetMetaData(const QImage&);
    static FrameMetaData ParseMeta(const QString&);

    static void DrawMetaData(QPainter&, const FrameMetaData&, QSize );
    static ShowFrameR GotoFrame(int);

    static QColor GetColor(int videoix, int frameix, int x, int y);
    static QString toHexString(const QColor&);


    static FrameData *GetFrameData(VideoData *videodata, int ix);
    static QString toString(const QColor &pix);

    static int GetColorIx(const QColor &color);
    static QString GetColorName(const QColor& color);

    static QColor GetColor(int quality);
    static QImage GetImage(int videoix, int fix, int x, int y, int r);
    static QImage GetImage(VideoData* videodata, int fix, int x, int y, int r);

    static QImage Filter(const QImage&, int fcsix, FilterMode);

    struct LoadFcsR{
        QString folderName;
        int fcs_count;
        int unfcs_count;
    };

    static LoadFcsR LoadFcs();
    static bool LoadFcs2(const QString&, int i);
    static bool LoadUnfcs2(const QString&, int i);

    struct SaveFcsR{
        QString folderName;
        int fcs_count;
        int unfcs_count;
    };

    static SaveFcsR SaveUnfcs();
    static bool SaveUnfcs2(const QString&, int i);

    static int GetBallIx(int vix, int fix, double x, double y, double d_max);

    struct SetTrackingR{
        bool fcix_changed;
        bool isValid;
    };

    static SetTrackingR SetTracking(int vix, int fix, int bix, int fcix, int x, int y);
    static void SetTracking(FilterMode mode);

    struct ShowTrackingR{
        QImage image;
        QImage image_filtered;
    };

    static ShowTrackingR ShowTracking();
    static QString ShowTrackingTxt();


    static FilterMode GetTrackingFilterMode();
    static QColor GetTrackingColor(const QPoint& p, const QSize& s);

    static void SetTrackingColor(const QColor& c);
    static TrackingColor GetTrackingColor();

    struct AddUnfcsR{
        FriendlyRGB fc;
        bool isok;
    };

    static AddUnfcsR AddUnfcs();


    struct AddFcsExtraR{
        FriendlyRGB fc;
        bool isok;
    };

    static AddFcsExtraR AddFcs();

    struct DelUnfcsR{
        FriendlyRGB fc;
        bool isok;
    };

    static DelUnfcsR DelUnfcs(const QString& txt);

   struct GetTrackingUnfcR{
       QStringList unfcshex;
       QString name;
   };

    static GetTrackingUnfcR GetTrackingUnfc();


    struct TrackingFcsExtraR{
        QStringList unfcshex;
        QString name;
    };

    static TrackingFcsExtraR GetTrackingFcsExtraR();

    static void SetVideoData();
    static void AdjoinFFcs();

    struct DelFcsExtraR{
        FriendlyRGB fc;
        bool isok;
    };

    static DelFcsExtraR DelFcsExtra(const QString& txt);
};

#endif // CAMPLAYER_H

