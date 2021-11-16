#include "global.h"
#include "camplayer.h"
#include "settings.h"
#include "friendlyrgb.h"
#include "filehelper.h"
#include <QFileDialog>
#include <QFontMetrics>
#include <QPainter>
#include <QRegularExpression>
#include <QVarLengthArray>
#include <QtMath>

extern Settings settings;

int CamPlayer::frameix = -1;
int CamPlayer::maxframeix = -1;

QString CamPlayer::videoFolder = QLatin1String("");
QString CamPlayer::fcsFolder = QLatin1String("");

CamPlayer::VideoData CamPlayer::video1 = {};
CamPlayer::VideoData CamPlayer::video2 = {};
CamPlayer::VideoData CamPlayer::video3 = {};
CamPlayer::VideoData CamPlayer::video4 = {};
CamPlayer::FriendlyColorData CamPlayer::_fcs[_fcs_length]; //NOLINT
CamPlayer::FriendlyColorData CamPlayer::_unfcs[_fcs_length]; //NOLINT
CamPlayer::FriendlyColorData CamPlayer::_fcs_extra[_fcs_length]; //NOLINT
bool CamPlayer::isInited=false;

CamPlayer::TrackingData CamPlayer::trackingdata;

void CamPlayer::init()
{
    isInited = false;
    SetVideoData();
    isInited = true;
}

auto CamPlayer::Load(QWidget *parent) -> CamPlayer::LoadR
{
    videoFolder = QFileDialog::getExistingDirectory(
        parent,
        QStringLiteral("Open Folder"),
        settings.lastOpenedFolder,
        QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks
        );
    if(videoFolder.isEmpty()) return{{},true};
    settings.lastOpenedFolder = videoFolder;
    LoadExam(videoFolder);
    return {videoFolder, false};
}

void CamPlayer::SaveToVideoFolder(const QString& fn, const QStringList& lines)
{
    QString path = settings.lastOpenedFolder;

    auto d = QDir(path);
    if(!d.exists()) return;
    QString fullpath = d.filePath(fn);

    FileHelper::SaveText(fullpath, lines);
}

auto  CamPlayer::LoadFromVideoFolder(const QString& fn) -> QMap<int,QStringList>
{
    QString path = settings.lastOpenedFolder;

    QDir d(path);
    //{"*.txt"}
    auto filenames = d.entryList({fn}, QDir::Files, QDir::Name);
    QRegularExpression re(QStringLiteral("(\\d+)"));
    bool isok;
    QMap<int, QStringList> map;
    for(auto&filename:filenames)
    {
        QString fullpath = d.filePath(filename);
        if(!QFileInfo(fullpath).exists()) continue;

        auto m = re.match(filename);
        if(!m.hasMatch()) continue;

        QString a = m.captured();
        int frameix = a.toInt(&isok);
        if(!isok) continue;

        auto lll = FileHelper::LoadText(fullpath);
        map.insert(frameix, lll);
    }
    return map;
}

auto CamPlayer::LoadFcs(QWidget *parent) -> LoadFcsR{
    fcsFolder = QFileDialog::getExistingDirectory(
            parent,
            QStringLiteral("Open Folder"),
            settings.fcspath,
            QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks
            );
    if(fcsFolder.isEmpty()) return{{}, 0, 0, true};
    settings.fcspath = fcsFolder;

    int n=0,m=0;
    for(int i=0 ;i<_fcs_length;i++){
        bool isok = LoadFcs2(fcsFolder, i);
        if(isok) n++;
        isok = LoadUnfcs2(fcsFolder, i);
        if(isok) m++;
    }


    AdjoinFFcs();
    return {fcsFolder,n, m, false};
}

auto CamPlayer::LoadFcs2(const QString& folder, int ix)->bool{
    CamPlayer::FriendlyColorData& data = _fcs[ix];//NOLINT
    //if(!data) return false;

    auto d = QDir(folder);
    if(!d.exists()) return false;

    QString fn = FriendlyRGB::GetFileName(QStringLiteral("fc"), ix);

    QString fullpath = d.filePath(fn);

    data.fcs.clear();
    data.fileName = fullpath;
    data.ix=ix;

    if(!QFileInfo(fullpath).exists()) return false;
    auto lines = FileHelper::LoadText(fullpath);
    if(lines.isEmpty()) return false;
    for(auto&line:lines){
        if(line.isEmpty()) continue;
        bool ok;
        auto fc = FriendlyRGB::FromCSV(line, FriendlyRGB::CsvType::hex, &ok);
        auto fi = FriendlyRGB::ToFriendlyInt(fc.r, fc.g, fc.b);
        data.fcs.insert(fi);
    }
    return true;
}

auto CamPlayer::LoadUnfcs2(const QString& folder, int ix)->bool{
    CamPlayer::FriendlyColorData& data = _unfcs[ix];//NOLINT
    CamPlayer::FriendlyColorData& data2 = _fcs_extra[ix];//NOLINT
    //if(!data) return false;

    auto d = QDir(folder);
    if(!d.exists()) return false;
    QString fn = FriendlyRGB::GetFileName(QStringLiteral("ufc"), ix);

    //QString fullpath = d.filePath(fn);

    QString fn_filter = FriendlyRGB::GetFileNameFilter(QStringLiteral("ufc")
                                                           ,ix);

    auto files = d.entryList({fn_filter},QDir::Files);

    data.fcs.clear();
    data.fileName = d.filePath(fn_filter);
    data.ix=ix;

    QStringList lines;
    for(auto& fff:files){
        QString fullpath = d.filePath(fff);
        if(!QFileInfo(fullpath).exists()) continue;
        auto lll = FileHelper::LoadText(fullpath);
        lines.append(lll);
    }
    if(lines.isEmpty()) return false;
    for(auto&line:lines){
        if(line.isEmpty()) continue;
        bool isExtra=false;
        QString line2 = line;
        if(line2.endsWith('+')){
            line2= line2.left(line2.length()-1);
            isExtra = true;
        }

        bool ok;        
        auto fc = FriendlyRGB::FromCSV(line2, FriendlyRGB::CsvType::hex, &ok);
        auto fi = FriendlyRGB::ToFriendlyInt(fc.r, fc.g, fc.b);
        if(isExtra){
            data2.fcs.insert(fi);
        } else {
            data.fcs.insert(fi);
        }
    }
    return true;
}

auto CamPlayer::SaveUnfcs() -> SaveFcsR{
    QDir u(settings.fcspath);
    if(!u.exists()){
        fcsFolder = QFileDialog::getExistingDirectory(
            nullptr,
            QStringLiteral("Open Folder"),
            settings.fcspath,
            QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks
            );
        settings.fcspath = fcsFolder;
    }

    int n=0,m=0;
    for(int i=0 ;i<_fcs_length;i++){
        bool isok;
        isok = SaveUnfcs2(settings.fcspath, i);
        if(isok) m++;
    }

    return {fcsFolder,n, m};
}

auto CamPlayer::SaveUnfcs2(const QString& folder, int ix)->bool{
    CamPlayer::FriendlyColorData& data = _unfcs[ix]; //NOLINT
    CamPlayer::FriendlyColorData& data2 = _fcs_extra[ix]; //NOLINT

    auto d = QDir(folder);
    if(!d.exists()) return false;

    QString fn = FriendlyRGB::GetFileName(QStringLiteral("ufc"), ix);

    QString fullpath = d.filePath(fn);

    data.fileName = fullpath;

    QStringList lines;
    for(auto&i:data.fcs){
        auto fc = FriendlyRGB::FromFriendlyInt(i);
        lines.append(fc.toHexString());
    }
    for(auto&i:data2.fcs){
        auto fc = FriendlyRGB::FromFriendlyInt(i);
        lines.append(fc.toHexString()+"+");
    }
    FileHelper::SaveText(fullpath, lines);
    return true;
}

auto CamPlayer::GetBallIx(int vix,
                          int fix,
                          double x0,
                          double y0,
                          double d_max) -> int
{
    if(d_max<=0) d_max=settings.tracking_radius;
    auto videodata = GetVideoData(vix);
    auto framedata = GetFrameData(videodata, fix==-1?frameix:fix);
    auto s = QSizeF(framedata->image.size());
    double d_min = std::numeric_limits<double>::max();
    int ix_min=-1;
    for (auto i = framedata->metadata.balldata.begin();
         i != framedata->metadata.balldata.end();
         i++ )
    {
        auto b = i.value();
        if(!b.isVisible()) continue;

        double x = b.x*s.width();
        double y = b.y*s.height();

        auto d = std::hypotf(x-x0,y-y0);
        if(d>d_max) continue;
        if(d<d_min){
            d_min=d;
            ix_min = i.key();
        }
    }
    return ix_min;
}

auto CamPlayer::SetTrackingFcix(int fcix)->SetTrackingR
{
    SetTrackingR r{};

    if(fcix!=-1){
        r.fcix_changed = trackingdata.fcix!=fcix;
        trackingdata.fcix=fcix;
    }

    r.isValid = trackingdata.isValid();

    if(r.fcix_changed) AdjoinFFcs();    
    r.ffcs_count = trackingdata.ffcs.count();
    return r;
}

//QSize CamPlayer::trackingdata_image_size()
//{
//    return trackingdata.image.size();
//}
void CamPlayer::DeleteTracking(){
    trackingdata.vix = trackingdata.bix = trackingdata.x = trackingdata.y = -1;
}

auto CamPlayer::SetTracking(int vix,
                            int fix,
                            int bix,
                            int fcix,
                            int x,
                            int y) -> SetTrackingR
{
    SetTrackingR r{};
    trackingdata.vix=vix;
    trackingdata.fix=fix;
    trackingdata.bix=bix;
    if(fcix!=-1){
        r.fcix_changed = trackingdata.fcix!=fcix;
        trackingdata.fcix=fcix;
    }
    trackingdata.x = x;
    trackingdata.y = y;
    r.isValid = trackingdata.isValid();

    if(r.fcix_changed) AdjoinFFcs();
    return r;
}

void CamPlayer::SetTracking(FilterMode mode)
{
    trackingdata.filtermode=mode;
}

auto CamPlayer::GetTrackingFilterMode()->FilterMode
{
    return trackingdata.filtermode;
}

auto CamPlayer::GetTrackingColor(const QPoint& p, const QSize& s0) -> QColor // NOLINT(clazy-function-args-by-value)
{
    if(trackingdata.image.isNull()) return {};
    auto s = QSizeF(trackingdata.image.size());
    double rx = s.width()/s0.width();
    double ry = s.height()/s0.height();
    int x = p.x()*rx;
    int y = p.y()*ry;
    return trackingdata.image.pixel(x,y);
}

void CamPlayer::SetTrackingColor(const QColor& c)
{
    trackingdata.trackingcolor.color = c;
    trackingdata.trackingcolor.friendly_int = FriendlyRGB::ToFriendlyInt(
        c.red(),
        c.green(),
        c.blue());
    trackingdata.trackingcolor.fc = FriendlyRGB::FromFriendlyInt(
        trackingdata.trackingcolor.friendly_int);
}

auto CamPlayer::GetTrackingColor()->TrackingColor
{
    return trackingdata.trackingcolor;
}

auto CamPlayer::AddUnfcs() -> AddUnfcsR
{
    if(trackingdata.fcix==-1) return {{},false};
    auto& a = _unfcs[trackingdata.fcix]; //NOLINT

    int j0  = a.fcs.size();
    a.fcs.insert(trackingdata.trackingcolor.friendly_int);
    int j1  = a.fcs.size();

    AdjoinFFcs();
    return {trackingdata.trackingcolor.fc, j1>j0};
}

auto CamPlayer::AddFcs() -> AddFcsExtraR
{
    if(trackingdata.fcix==-1) return {{},false};
    auto& a = _fcs_extra[trackingdata.fcix]; //NOLINT

    int j0  = a.fcs.size();
    a.fcs.insert(trackingdata.trackingcolor.friendly_int);
    int j1  = a.fcs.size();

    AdjoinFFcs();
    return {trackingdata.trackingcolor.fc, j1>j0};
}


auto CamPlayer::DelUnfcs(const QString& txt) -> CamPlayer::DelUnfcsR
{
    if(trackingdata.fcix==-1) return {{},false};
    auto& a = _unfcs[trackingdata.fcix]; //NOLINT

    bool isok;
    auto fc = FriendlyRGB::FromCSV(txt, FriendlyRGB::hex, &isok);
    auto friendly_int = fc.toFriendlyInt();

    int j0  = a.fcs.size();
    a.fcs.remove(friendly_int);
    int j1  = a.fcs.size();

    AdjoinFFcs(); // ez a trackeléshez használt táblázatot önti össze
    return {fc, j1<j0};
}

auto CamPlayer::DelFcsExtra(const QString& txt) -> CamPlayer::DelFcsExtraR
{
    if(trackingdata.fcix==-1) return {{},false};
    auto& a = _fcs_extra[trackingdata.fcix]; //NOLINT

    bool isok;
    auto fc = FriendlyRGB::FromCSV(txt, FriendlyRGB::hex, &isok);
    auto friendly_int = fc.toFriendlyInt();

    int j0  = a.fcs.size();
    a.fcs.remove(friendly_int);
    int j1  = a.fcs.size();

    AdjoinFFcs(); // ez a trackeléshez használt táblázatot önti össze
    return {fc, j1<j0};
}

void CamPlayer::AdjoinFFcs(){
    if(trackingdata.fcix==-1) return;
    trackingdata.ffcs.clear();
    auto& unfcs = _unfcs[trackingdata.fcix].fcs; //NOLINT
    for(auto&i:_fcs[trackingdata.fcix].fcs){
        if(!unfcs.contains(i)) trackingdata.ffcs.insert(i);
    }

    //auto& unfcs2 = _unfcs2[trackingdata.fcix].fcs; //NOLINT
    for(auto&i:_fcs_extra[trackingdata.fcix].fcs){
        if(!unfcs.contains(i)) trackingdata.ffcs.insert(i);
        //if(!unfcs2.contains(i)) trackingdata.ffcs.insert(i);
    }
}

auto CamPlayer::GetTrackingUnfc() -> GetTrackingUnfcR
{
    if(trackingdata.fcix==-1) return {};
    auto& a = _unfcs[trackingdata.fcix]; //NOLINT
    GetTrackingUnfcR e;
    for(auto&i:a.fcs){
        auto fc = FriendlyRGB::FromFriendlyInt(i);
        auto j = fc.toHexString();
        e.unfcshex.append(j);
    }
    e.name = FriendlyRGB::GetName(trackingdata.fcix);
    return e;
}

auto CamPlayer::GetTrackingFcsExtraR() -> TrackingFcsExtraR
{
    if(trackingdata.fcix==-1) return {};
    auto& a = _fcs_extra[trackingdata.fcix]; //NOLINT
    TrackingFcsExtraR e;
    for(auto&i:a.fcs){
        auto fc = FriendlyRGB::FromFriendlyInt(i);
        auto j = fc.toHexString();
        e.unfcshex.append(j);
    }
    e.name = FriendlyRGB::GetName(trackingdata.fcix);
    return e;
}

auto CamPlayer::ShowTracking() -> CamPlayer::ShowTrackingR
{
    ShowTrackingR r;
    //r.x = trackingdata.x;
    //r.y = trackingdata.y;
    r.fcs_count = _fcs->fcs.size();
    if(!trackingdata.isValid()) return r;
    auto videodata = GetVideoData(trackingdata.vix);
    auto framedata = GetFrameData(videodata, frameix);
    if(!framedata || framedata->image.isNull()) return r;
    auto s = QSizeF(framedata->image.size());
    double x, y;
    if(trackingdata.bix==-1) {
        x = trackingdata.x;//*s.width();
        y = trackingdata.y;//*s.height();
    }
    else{
        if(!framedata->metadata.balldata.contains(trackingdata.bix)) return r;
        auto b = framedata->metadata.balldata[trackingdata.bix];
        x = b.x*s.width();
        y = b.y*s.height();
    }

    r.image = CamPlayer::GetImage(videodata,
                                  frameix,
                                  x,
                                  y,
                                  settings.tracking_radius);
    r.image_filtered = CamPlayer::Filter(r.image,
                                         trackingdata.fcix,
                                         trackingdata.filtermode,
                                         &r.fpixel_count);
    return r;
}

auto CamPlayer::ShowTrackingTxt() -> QString
{
    return trackingdata.toString();
}

auto CamPlayer::Settings() -> SettingsR
{
    return{videoFolder, fcsFolder};
}

auto CamPlayer::LoadExam(const QString& path) -> bool
{
    frameix = 0;
    bool ok1 = LoadVideoData(path, settings.videopath1, video1);
    bool ok2 = LoadVideoData(path, settings.videopath2, video2);
    bool ok3 = LoadVideoData(path, settings.videopath3, video3);
    bool ok4 = LoadVideoData(path, settings.videopath4, video4);

    maxframeix = std::max({
        video1.maxframeix,
        video2.maxframeix,
        video3.maxframeix,
        video4.maxframeix
    });

    return ok1&&ok2&&ok3&&ok4;
}

auto CamPlayer::LoadVideoData(const QString& path,
                              const QString& folder,
                              VideoData& videodata)->bool
{
    videodata.frames.clear();
    videodata.maxframeix = -1;

    QDir d(path);
    videodata.folder = d.filePath(folder);
    QDir d2(videodata.folder);

    auto filenames = d2.entryList({"*.jpg"}, QDir::Files, QDir::Name);
    QRegularExpression re(QStringLiteral("(\\d+)"));
    bool isok;

    for(auto&filename:filenames)
    {
        CamPlayer::FrameData data;
        data.videoix = videodata.ix;
        auto m = re.match(filename);
        if(!m.hasMatch()) continue;

        QString a = m.captured();
        int frameix = a.toInt(&isok);
        if(!isok) continue;

        for(bool & ball : data.balls) ball=false;
        data.fileName = d2.filePath(filename);
        data.image = QImage(data.fileName);

        QString metatxt = GetMetaData(data.image);
        data.metadata = ParseMeta(metatxt);
        data.metadata.videoix = videodata.ix;

        videodata.frames.insert(frameix, data);
        if(frameix>videodata.maxframeix) videodata.maxframeix = frameix;
    }
    return true;
}

void CamPlayer::SetVideoData(){
    video1.ix=1;
    video2.ix=2;
    video3.ix=3;
    video4.ix=4;
}

auto CamPlayer::GetVideoData(int videoix) -> CamPlayer::VideoData *
{    
    switch(videoix){
    case 1:return &video1;
    case 2:return &video2;
    case 3:return &video3;
    case 4:return &video4;
    default: return nullptr;
    }
}

auto CamPlayer::GetFrameData(VideoData* videodata,
                             int ix) -> CamPlayer::FrameData*
{
    if(videodata!=nullptr &&
        ix>=0 && ix<=videodata->maxframeix &&
        videodata->frames.contains(ix)){

        return &(videodata->frames[ix]);
    }
    return nullptr;
}


auto CamPlayer::NextFrame()-> CamPlayer::ShowFrameR
{
    if(frameix<maxframeix) frameix++;
    return ShowFrame(frameix);
}

auto CamPlayer::PrevFrame()-> CamPlayer::ShowFrameR
{
    if(frameix>0) frameix--;
    return ShowFrame(frameix);
}

auto CamPlayer::GotoFrame(int ix)-> CamPlayer::ShowFrameR
{
    if(0<=ix && ix<=maxframeix) frameix=ix;
    return ShowFrame(frameix);
}

auto CamPlayer::GetColor(int videoix, int fix, int x, int y) -> QColor
{
    auto videodata = GetVideoData(videoix);
    if(!videodata) return{};
    auto framedata = GetFrameData(videodata, fix==-1?frameix:fix);
    if(!framedata) return{};
    auto color = framedata->image.pixelColor(x,y);
    return color;
}

auto CamPlayer::GetImage(int videoix, int fix, int x, int y, int r) -> QImage
{
    auto videodata = GetVideoData(videoix);
    return GetImage(videodata, fix, x, y, r);
//    if(!videodata) return{};
//    auto framedata = GetFrameData(videodata, fix==-1?frameix:fix);
//    if(!framedata) return{};

//    QRect r2(x-r, y-r, r+r, r+r);

//    auto image = framedata->image.copy(r2);
//    return image;
}

auto CamPlayer::GetImage(VideoData* videodata,
                         int fix,
                         int center_x,
                         int center_y,
                         int r) -> QImage
{
    //auto videodata = GetVideoData(videoix);
    if(!videodata) return{};
    auto framedata = GetFrameData(videodata, fix==-1?frameix:fix);
    if(!framedata) return{};

    QRect r2(center_x-r, center_y-r, r+r, r+r);

    auto image = framedata->image.copy(r2);
    return image;
}

auto CamPlayer::Filter(const QImage &image,
                       int fcsix,
                       CamPlayer::FilterMode mode,
                       int *counter) -> QImage
{
    if(counter==nullptr) return {};
    *counter = 0;
    if(fcsix<0||fcsix>_fcs_length-1) return {};
    auto& fcs = trackingdata.ffcs;//_fcs[fcsix];
    if(fcs.empty()) return {};
    QImage img = image.copy();
    trackingdata.image = img;

    QRgb *st = reinterpret_cast<QRgb *>(img.bits()); // NOLINT
    quint64 pixelCount = img.width() * img.height();
    QRgb *pixel = st;
    int u;
    for (quint64 p = 0; p < pixelCount; p++) {
        //*pixel = QColor(qRed(*pixel)/2, qGreen(*pixel)/2, qBlue(*pixel)/2).rgb();
        int fi = FriendlyRGB::ToFriendlyInt(*pixel);
        if(mode == FilterMode::Copy){
            u = *pixel;
            *pixel = QColor(qRed(u), qGreen(u), qBlue(u)).rgb();
        }
        else if(mode == FilterMode::IsFriendly) {
            if(fcs.contains(fi)){
                (*counter)++;
                *pixel=0xffffff;
            } else *pixel=0;
        }
        else if(mode == FilterMode::AllFriendly){
            if(fcs.contains(fi)){
                (*counter)++;
            }else *pixel=0;
        }
        pixel++; // NOLINT
    }
    return img;
}

auto CamPlayer::FilterStat(const QImage& img,
                           FilterStatR* i) -> CamPlayer::FilterStatR
{
    FilterStatR r(img.size());
    //if(!i) return r;
    //r.pix_count=0;
    const QRgb *st = reinterpret_cast<const QRgb *>(img.bits()); // NOLINT
    const QRgb *pixel = st;
    //r.p.resize(r.fix.length());
    //for(auto& i:r.p) i=0;
    int pix;
    quint64 pixelCount = r.fix.pixelcount();
    for (quint64 p = 0; p < pixelCount; p++) {
        pix = r.fix.ix(p);
        if(((i && !i->p[pix])||!i) && *pixel) {
            r.pix_count++;
            r.p[pix]++;
        }
        pixel++; // NOLINT
    }
    return r;
}

/*
//y = p/w;
//x = p%w;

//xx = x/r.w;
//yy = y/r.w;
//ix = yy*w2+xx;
//r.map.insert({ x/r.w, y/r.w});

//ix = (((p*w2)/w)+p%w)/r.w;

*/
void CamPlayer::DrawFilterStat(QImage* img,
                               const FilterStatR& r)
{
    if(!img) return;
    QRgb *st = reinterpret_cast<QRgb *>(img->bits()); // NOLINT
    QRgb *pixel = st;

    quint64 pixelCount = r.fix.pixelcount();
    for (quint64 p = 0; p < pixelCount; p++) {
        if (!r.p[r.fix.ix(p)]) *pixel = 0x6e6e6d;
        pixel++; // NOLINT
    }
}

void CamPlayer::DrawFilterStat2(QImage* img,
                                FilterStatR* r,
                                const QColor& c,
                                bool isEnabled)
{
    if(!r) return;
    if(!img) return;
    QPainter painter(img);
    QBrush br(c);
    QPen pen(br, 1);
    painter.setPen(pen);

    int w = r->fix.rw;
    for(int y=0;y<r->fix.h2;y++){
        for(int x=0;x<r->fix.w2;x++){
            if (r->get(x,y)>0) {
                if (isEnabled) {
                    painter.drawRect(x*w,y*w,w-1,w-1);
                } else {
                    int x0 = x*w+1;
                    int y0 = y*w+1;
                    int x1 = x0+w-2;
                    int y1 = y0+w-2;

                    painter.drawLine(x0, y0, x1, y1);
                    painter.drawLine(x0, y1, x1, y0);
                }

            }
        }
    }
    painter.end();
}

auto CamPlayer::toHexString(const QColor & pix) -> QString
{
    int r = pix.red();
    int g = pix.green();
    int b = pix.blue();

    //auto hexstr = QString("#%1%2%3").arg(r, 2, 16, QChar('0')).arg(g, 2, 16, QChar('0')).arg(b, 2, 16, QChar('0'));
    return FriendlyRGB::toHexString(r,g,b);
}

auto CamPlayer::toString(const QColor & pix) -> QString
{
    int r = pix.red();
    int g = pix.green();
    int b = pix.blue();

    //auto str = QString("%1,%2,%3").arg(r).arg(g).arg(b);
    //return str;
    return FriendlyRGB::toString(r,g,b);
}

//auto CamPlayer::GetColorName(const QColor &color) -> QString
//{
//    int color_ix = GetColorIx(color);
//    auto name = FriendlyRGB::GetName(color_ix);
//    return name;
//}

auto CamPlayer::GetColorIx(const QColor &color) ->int
{
    FriendlyRGB fc(
        static_cast<byte>(color.red()),
        static_cast<byte>(color.green()),
        static_cast<byte>(color.blue()));

    //auto lab = toLab(frgb.r, frgb.g, frgb.b);
    double rng=5;
    static const int n = 6;
    return FriendlyRGB::GetRYBIxWheelN(fc.r, fc.g, fc.b, &rng, n);
}


auto CamPlayer::ShowFrame(int ix)->  CamPlayer::ShowFrameR
{
    int maxix = 0;
    if(video1.maxframeix>maxix) maxix = video1.maxframeix;
    if(video2.maxframeix>maxix) maxix = video2.maxframeix;
    if(video3.maxframeix>maxix) maxix = video3.maxframeix;
    if(video4.maxframeix>maxix) maxix = video4.maxframeix;

    bool hasNext = ix<maxix;
    bool hasPrev = ix>0;
    return{
        GetFrameData(&video1,ix),
        GetFrameData(&video2,ix),
        GetFrameData(&video3,ix),
        GetFrameData(&video4,ix),
        ix,
        hasNext,
        hasPrev
    };
}

auto CamPlayer::GetMetaData(const QImage& image) -> QString
{
    if(image.isNull()) return QLatin1String("");
    return image.text(QStringLiteral("ImageDescription"));
}

auto CamPlayer::ParseMeta(const QString& txt) -> CamPlayer::FrameMetaData
{    
    FrameMetaData r;
    r.timestamp = QDateTime();
    r.balls=0;
    r.txt = QString();
    if(txt.isEmpty()) return r;

    r.txt = txt;

    bool isok;

    int ix = txt.indexOf('|');
    if(ix==-1) return{};
    auto str1 = txt.left(ix);
    auto str2 = txt.mid(ix+1);
    auto str1s = str1.split(';');
    if(str1s.length()<3) return r;
    r.frameix = str1s[0].toInt(&isok);
    if(!isok) return r;
    r.timestamp =  QDateTime::fromString(str1s[1], Qt::ISODateWithMs);
    r.balls = str1s[2].toInt(&isok);
    if(!isok) return r;
    auto str2s = str2.split('|');

    for(auto&s2:str2s){
        BallData b;
        b.txt = s2;
        auto s2s = s2.split(';');

        if(s2s.length()<16) continue;
        b.id = s2s[0].toInt(&isok);
        //if(!isok) continue;
        b.is_valid_for_tracking = s2s[1]==QStringLiteral("true");
        b.shortname = s2s[2];
        b.x = s2s[3].toDouble(&isok);
        //if(!isok) continue;
        b.y = s2s[4].toDouble(&isok);
        //if(!isok) continue;
        b.diameter = s2s[5].toInt(&isok);
        //if(!isok) continue;
        b.fpxs = s2s[6].toInt(&isok);
        //if(!isok) continue;

        b.connected = s2s[7].toInt(&isok);
        //if(!isok) continue;
        b.partitions = s2s[8].toInt(&isok);
        //if(!isok) continue;
        b.partitions_too_small = s2s[9].toInt(&isok);
        //if(!isok) continue;
        b.partitions_too_large = s2s[10].toInt(&isok);
        //if(!isok) continue;
        b.partitions_ok = s2s[11].toInt(&isok);
        //if(!isok) continue;

        b.found_total = s2s[12].toInt(&isok);
        //if(!isok) continue;
        b.not_found_ftotal = s2s[13].toInt(&isok);
        //if(!isok) continue;
        b.not_found_last_not_ok = s2s[14].toInt(&isok);
        //if(!isok) continue;
        b.found_last_ok = s2s[15].toInt(&isok);
        //if(!isok) continue;

        if(s2s.length()>20){
            b.x = s2s[18].toDouble(&isok);
            //if(!isok) continue;
            b.y = s2s[19].toDouble(&isok);
            //if(!isok) continue;
            b.quality = s2s[20].toInt(&isok);
            //if(!isok) continue;
        }

        r.balldata.insert(b.id, b);
    }
    return r;
}

auto CamPlayer::GetColor(int quality)->QColor
{
    switch(quality){
        case 0: return Qt::darkRed;
        case 1: return Qt::cyan;
        case 2: return Qt::yellow;
        case 3: return {255,128,0};
        default: return Qt::red;
    }
}

void CamPlayer::DrawMetaData(QPainter& painter,
                             const FrameMetaData&m,
                             QSize size)
{
    static const int r=10;
    static const int tr=15;
    static const int r2 = r*2;
    static const int tr2 = tr*2;

    //auto s = painter.viewport().size();
    auto s = size;
    QFont font = painter.font();
    font.setPixelSize(tr2);
    font.setBold(true);
    painter.setFont(font);
//    painter.setPen(Qt::cyan);
//    painter.drawLine(0,0,size.width(), size.height());



    for(auto&b:m.balldata)
    {
        if(!b.isVisible()) continue;

        QColor c = GetColor(b.quality);
        QBrush br(c);
        QPen pen(br, 3);
        painter.setPen(pen);

        int x = b.x * static_cast<double>(s.width());//720.0;
        int y = b.y * static_cast<double>(s.height());//1280.0;

        QRectF rectangle(x-r, y-r, r2, r2);

        painter.drawLine(x+r, y, x+r2, y);
        QString txt = QString::number(b.id);
        //if(!b.shortname.isEmpty()) txt+=':'+b.shortname;

        QRect rect2(x+r2+4, y-tr, tr2, tr2);

        QFontMetrics fm(font, painter.device());
        auto fl = Qt::AlignVCenter|Qt::AlignJustify;
        auto bounding = fm.boundingRect(rect2, fl, txt);

        painter.fillRect(bounding, Qt::black);

        int x1 = bounding.left()-4;
        int yt = bounding.top()-1;
        int yb = bounding.bottom()+1;
        int x2 = bounding.right()+4;

        //painter.drawRect(bounding);
        painter.drawLine(x1, yt, x1, yb);
        painter.drawLine(x1, yt, x1+6, yt);
        painter.drawLine(x1, yb, x1+6, yb);

        painter.drawLine(x2, yt, x2, yb);
        painter.drawLine(x2, yt, x2-6, yt);
        painter.drawLine(x2, yb, x2-6, yb);

        painter.drawText(rect2, fl, txt);

        bool tracked =
            trackingdata.isValid() &&
            trackingdata.vix==m.videoix &&
            b.id==trackingdata.bix;

        if(tracked) {
            painter.drawRect(rectangle);
            rectangle.adjust(-1, -1, 1, 1);
            painter.drawRect(rectangle);
        } else{
            painter.drawArc(rectangle, 0, 5760);
        }
    }
    if(trackingdata.vix == m.videoix
        && trackingdata.bix==-1)
    {
        int x = trackingdata.x;
        int y = trackingdata.y;

        QBrush br(Qt::cyan);
        QPen pen(br, 3);
        painter.setPen(pen);

        painter.drawLine(x, y-tr, x, y+tr);
        painter.drawLine(x-tr, y, x+tr, y);
    }

    //painter.end();
}







