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
            tracking_txt+=' '
                +((bix>-1)
                       ?QString::number(bix)
                       :QStringLiteral("?"));
            tracking_txt += ' '+QString::number(fcix);
            if(isValid()) tracking_txt+=(bix!=-1)?'*':'+';
            return tracking_txt;
        }

        QSet<int> ffcs;
        //QSet<int> fcs;


        struct IgnoreData{
            int vix, fcix, /*x, y,*/ ix;

            QString toString() {
                return QString::number(vix)+' '
                    + QString::number(fcix)+' '
//                    + QString::number(x)+' '
//                    + QString::number(y)+' '
                    + QString::number(ix);
            }

            void setIx(int _ix){ ix=_ix;}
        };
        IgnoreData ignoreData(){return {vix,fcix,/*x,y,*/-1};}
        QPoint mpoint(){return {x,y};}

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
        // kalibr??ci?? eset??n nincs Q mivel azt az interpol??ci?? hozza l??tre
        // Q = 0, ha nincs tal??lat, 1 ha van, 2, 3 ha interpol??lt (line??ris,
        //illetve bezier) itt a kamer??ban kell megoldani, hogy get_pic eset??n
        //is legyen Q, ami tal??lat eset??n 1, interpol??ci?? ugyan??gy nincs -
        // ha trackelhet?? ??s (volt tal??lat vagy ha nem, interpol??ci?? volt)

        bool isVisible() const{
            return is_valid_for_tracking
                && (found_last_ok > 0 || quality > 0);
        }
    };

    struct FrameMetaData{        
        QString txt;
        int frameix;
        QDateTime timestamp = QDateTime();
        int balls=0;
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
        //int pixel_counter;//filtered
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
        bool isCanceled;
    };
    static LoadR Load(QWidget*);

    struct SettingsR
    {
        QString video_folderName;
        QString fcs_folderName;
    };

    static SettingsR Settings();
    static bool LoadExam(const QString&);
    static bool LoadVideoData(const QString&,
                              const QString&,
                              VideoData&);

    static int frameix;
    static int maxframeix;
    struct ShowFrameR
    {
        const FrameData* framedata1;
        const FrameData* framedata2;
        const FrameData* framedata3;
        const FrameData* framedata4;
        int frameix;
        bool hasNext;
        bool hasPrev;
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
    //static QString GetColorName(const QColor& color);

    static QColor GetColor(int quality);
    static QImage GetImage(int videoix,
                           int fix,
                           int x,
                           int y,
                           int r);
    static QImage GetImage(VideoData* videodata,
                           int fix,
                           int x,
                           int y,
                           int r);

    static QImage Filter(const QImage&,
                         int fcsix,
                         FilterMode,
                         int *counter);

    static const int FILTER_W = 40;
    struct FilterIx{
        int w, h;
        int rw;
        int w2;
        int h2;
        FilterIx(QSize s, int _rw)
        {
            h = s.height();
            w = s.width();
            rw = _rw;
            w2 = w/rw;
            h2 = h/rw;
        }
        int ix(int _p) const {return ((_p/w)/rw)*w2+((_p%w)/rw);}
        int length(){return w2*h2;}
        quint64 pixelcount() const {return w*h;}
        int ix(int _x, int _y) const {return _y*w2+_x;}
        int ix(QPoint _p) const {return ix(_p.x(), _p.y());}
        QPoint mpoint(QPoint _p){return {_p.x()/rw, _p.y()/rw};}
        QPoint mpoint(int _p){return {_p/w2, _p%w2};}
    };

    struct FilterStatR{
        FilterStatR(QSize s) : fix{s, w} {
            pix_count = 0;
            p.resize(fix.length());
            for(auto& i:p) i=0;
        }
        //bool isinited = false;
        int pix_count;
        int w = FILTER_W;
        FilterIx fix;
        QVarLengthArray<int> p;
        int get(int x, int y) const
        {
            return p[fix.ix(x,y)];
        }
    };

    static FilterStatR FilterStat(const QImage&,
                                  FilterStatR* ignoretab);

    struct LoadFcsR{
        QString folderName;
        int fcs_count;
        int unfcs_count;
        bool isCanceled;
    };

    static LoadFcsR LoadFcs(QWidget *parent);
    static bool LoadFcs2(const QString&, int i);
    static bool LoadUnfcs2(const QString&, int i);

    struct SaveFcsR{
        QString folderName;
        int fcs_count;
        int unfcs_count;
    };

    static SaveFcsR SaveUnfcs();
    static bool SaveUnfcs2(const QString&, int i);

    static int GetBallIx(int vix,
                         int fix,
                         double x,
                         double y,
                         double d_max);

    struct SetTrackingR{
        bool fcix_changed;
        bool isValid;
        int ffcs_count;
    };

    static SetTrackingR SetTracking(int vix,
                                    int fix,
                                    int bix,
                                    int fcix,
                                    int x,
                                    int y);
    static void SetTracking(FilterMode mode);

    struct ShowTrackingR{
        QImage image;
        QImage image_filtered;
        int fcs_count;
        int fpixel_count;
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
    static SetTrackingR SetTrackingFcix(int fcix);
    //static QSize trackingdata_image_size();
    static void DeleteTracking();
    static void DrawFilterStat(QImage *img, const FilterStatR& r);
    static void DrawFilterStat2(QImage *img,
                                FilterStatR* r,
                                const QColor& c,
                                bool isEnabled);
    static int filterIx();


    static void SaveToVideoFolder(const QString &fn, const QStringList &lines);
    static QMap<int,QStringList> LoadFromVideoFolder(const QString &fn);
};

#endif // CAMPLAYER_H

