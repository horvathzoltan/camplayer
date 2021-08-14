#include "mainwindow.h"
#include "mousebuttonsignaler.h"
#include "mousemovesignaler.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <Qt>
extern MouseButtonSignaler signaler;
extern MouseMoveSignaler mouse_move_signaler;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    setUi_SettingsR(CamPlayer::Settings());

    CopyIconMirrored(ui->pushButton_next, ui->pushButton_prev);

    signaler.installOn(ui->label_pic1);
    signaler.installOn(ui->label_pic2);
    signaler.installOn(ui->label_pic3);
    signaler.installOn(ui->label_pic4);

    signaler.installOn(ui->label_zoom);
    signaler.installOn(ui->label_zoom_2);

    QObject::connect(&signaler, &MouseButtonSignaler::mouseButtonEvent,
                     this, &MainWindow::on_click_signaler);

    mouse_move_signaler.installOn(ui->label_zoom);
    mouse_move_signaler.installOn(ui->label_zoom_2);
    QObject::connect(&mouse_move_signaler, &MouseMoveSignaler::mouseMoveEvent,
                     this, &MainWindow::on_move_signaler);

    _frame_1.init(ui->label_pic1, ui->label_pix_1, ui->radioButton_t1_1);
    _frame_2.init(ui->label_pic2, ui->label_pix_2, ui->radioButton_t1_2);
    _frame_3.init(ui->label_pic3, ui->label_pix_3, ui->radioButton_t1_3);
    _frame_4.init(ui->label_pic4, ui->label_pix_4, ui->radioButton_t1_4);

    setUi_FilterMode(CamPlayer::GetTrackingFilterMode());
    setUi_FilterMode1(_frame_1.filterMode);
    setUi_FilterMode2(_frame_2.filterMode);
    setUi_FilterMode3(_frame_3.filterMode);
    setUi_FilterMode4(_frame_4.filterMode);

    ui->pushButton_dir->setChecked(_direction);
    RefreshDirLabel();

    ui->spinBox_timer->setValue(_timer_step);
    _timer.setInterval(_timer_step);
    connect(&_timer, &QTimer::timeout, this, &MainWindow::on_timeout);

    setUi_ListWidgetFcNames(CamPlayer::_fcs_length);
}

void MainWindow::setUi_ListWidgetFcNames(int n)
{
    if(n<0) return;
    if(n>=FriendlyRGB::WheelColorsRYBLen) return;

    for(int i=0;i<n;i++){
        const FriendlyRGB& a = FriendlyRGB::WheelColorsRYB[i];
        QColor c(a.r, a.g, a.b);
        QPixmap px(16, 16);
        px.fill(c);
        QIcon ic(px);
        auto item = new QListWidgetItem(ic,
                                        FriendlyRGB::WheelColorsRYBHumNames[i]);
        ui->listWidget_col_names->insertItem(i,item);
    }
}

void MainWindow::CopyIconMirrored(QAbstractButton* dst, QAbstractButton* src){
    QSize s = dst->size();
    QRect r(3,1,s.width()-6,s.height()-8);
    auto pixmap = dst->grab(r);
    QIcon icon;
    icon.addPixmap(pixmap.transformed(QTransform().scale(-1,1)));
    src->setIcon(icon);
    src->setIconSize(r.size());
}

void MainWindow::RefreshDirLabel(){
    ui->pushButton_dir->setText(
        _direction?QStringLiteral("▶"):QStringLiteral("◀"));
}

void MainWindow::RefreshPlayLabel(){
    ui->pushButton_play->setText(
        _timer.isActive()?QStringLiteral("⏸"):QStringLiteral("▶"));
}

auto MainWindow::picLabelIndex(QWidget* w) -> int
{
    if(w==ui->label_pic1) return 1;
    if(w==ui->label_pic2) return 2;
    if(w==ui->label_pic3) return 3;
    if(w==ui->label_pic4) return 4;
    return -1;
}

auto MainWindow::picLabel(int i) -> QLabel*
{
    if(i==1) return ui->label_pic1;
    if(i==2) return ui->label_pic2;
    if(i==3) return ui->label_pic3;
    if(i==4) return ui->label_pic4;
    return nullptr;
}

auto MainWindow::zoomLabelIndex(QWidget* w) -> int
{
    if(w==ui->label_zoom) return 1;
    if(w==ui->label_zoom_2) return 2;
    return -1;
}

void MainWindow::on_click_signaler(QWidget* w, QMouseEvent* e)
{
    if (e->type() == QEvent::MouseButtonPress) on_MouseButtonPress(w, e);
}

void MainWindow::on_move_signaler(QWidget* w, QMouseEvent* e)
{
    if (e->type() == QEvent::MouseMove) on_MouseMove(w,e);
    if (e->type() == QEvent::Leave) on_MouseLeave(w,e);
    if (e->type() == QEvent::Enter) on_MouseEnter(w,e);
}

void MainWindow::on_pushButton_next_clicked()
{
    if(_hasNext)
    {
        CamPlayer::ShowFrameR m = CamPlayer::NextFrame();
        setUi_ShowFrameR(m);
    } else {
        if(ui->pushButton_cycle->isChecked()){
            on_pushButton_rew_clicked();
            return;
        }
    }

}


void MainWindow::on_timeout(){
    if (_direction) {
        if (_hasNext){
            on_pushButton_next_clicked();
        } else {
            if(ui->pushButton_cycle->isChecked()){
                on_pushButton_rew_clicked();
            }
        }
    } else {
        if(_hasPrev){
            on_pushButton_prev_clicked();
        }else {
            if(ui->pushButton_cycle->isChecked()){
                on_pushButton_ffwd_clicked();
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
buttons
*/

void MainWindow::on_pushButton_load_clicked()
{
    if(_timer.isActive()) return;
    auto r = CamPlayer::Load(this);
    setUi_LoadR(r);
    auto r2 = CamPlayer::GotoFrame(0);
    setUi_ShowFrameR(r2);
}


void MainWindow::on_pushButton_prev_clicked()
{
    if(_hasPrev) {
        CamPlayer::ShowFrameR m = CamPlayer::PrevFrame();
        setUi_ShowFrameR(m);
    } else {
        if(ui->pushButton_cycle->isChecked()){
            on_pushButton_ffwd_clicked();
            return;
        }
    }
}

void MainWindow::on_pushButton_play_clicked(bool checked)
{
    Q_UNUSED(checked)
    if (_timer.isActive()){
        on_pushButton_stop_clicked();
    } else {
        _timer.start();        
        RefreshPlayLabel();
        ui->pushButton_play->setChecked(true);
    }
}


void MainWindow::on_pushButton_stop_clicked()
{
    _timer.stop();
    RefreshPlayLabel();
    ui->pushButton_play->setChecked(false);
}

void MainWindow::on_pushButton_rew_clicked()
{
    CamPlayer::ShowFrameR m = CamPlayer::GotoFrame(0);
    setUi_ShowFrameR(m);
}

void MainWindow::on_pushButton_ffwd_clicked()
{
    CamPlayer::ShowFrameR m = CamPlayer::GotoFrame(CamPlayer::maxframeix);
    setUi_ShowFrameR(m);
}

void MainWindow::on_pushButton_loadfcs_clicked()
{
    CamPlayer::LoadFcsR m = CamPlayer::LoadFcs(this);
    setUi_LoadFcsR(m);

    //RefreshUnfcs();
}

void MainWindow::on_radioButton_copy_clicked()
{
    CamPlayer::SetTracking(CamPlayer::FilterMode::Copy);
    RefreshZoom();
}

void MainWindow::on_radioButton_isfriendly_clicked()
{
    CamPlayer::SetTracking(CamPlayer::FilterMode::IsFriendly);
    RefreshZoom();
}

void MainWindow::on_radioButton_allfriendly_clicked()
{
    CamPlayer::SetTracking(CamPlayer::FilterMode::AllFriendly);
    RefreshZoom();
}


void MainWindow::on_spinBox_timer_valueChanged(int arg1)
{
    _timer_step = arg1;
    _timer.setInterval(_timer_step);
}

void MainWindow::on_pushButton_unfc_add_clicked()
{
    CamPlayer::AddUnfcsR m =CamPlayer::AddUnfcs();
    setUi_AddUnfcsR(m);
    if(m.isok) {RefreshZoom();RefreshFrames();}

}

void MainWindow::on_pushButton_fcs_extra_add_clicked()
{
    CamPlayer::AddFcsExtraR m =CamPlayer::AddFcs();
    setUi_AddFcsExtraR(m);
    if(m.isok) {RefreshZoom();RefreshFrames();}

}


void MainWindow::on_pushButton_unfc_del_clicked()
{
    auto items = ui->listWidget_unfcs->selectedItems();
    //ui->pushButton_unfc_del->setDisabled(items.isEmpty());
    //if(items.empty()) return;

    for(auto&item:items){
        auto txt = item->text();
        CamPlayer::DelUnfcsR m = CamPlayer::DelUnfcs(txt);
        setUi_DelUnfcsR(m);
        if(m.isok){ RefreshZoom(); RefreshFrames();}
        //delete ui->listWidget_unfcs->takeItem(ui->listWidget_unfcs->row(item));
    }
}

void MainWindow::on_pushButton_fcs_extra_del_clicked()
{
    auto items = ui->listWidget_fcs_extra->selectedItems();
    //ui->pushButton_fc_extra_del->setDisabled(items.isEmpty());
    //if(items.empty()) return;

    for(auto&item:items){
        auto txt = item->text();
        CamPlayer::DelFcsExtraR m = CamPlayer::DelFcsExtra(txt);
        setUi_DelFcsExtraR(m);
        if(m.isok){
            RefreshFrames();
            RefreshZoom();
        }
        //delete ui->listWidget_unfcs->takeItem(ui->listWidget_unfcs->row(item));
    }
}

void MainWindow::on_pushButton_dir_clicked(bool checked)
{
    _direction = checked;
    RefreshDirLabel();
}

void MainWindow::on_pushButton_savefcs_clicked()
{
    auto m = CamPlayer::SaveUnfcs();
    setUi_SaveFcsR(m);
}
/*
setUi
*/

void MainWindow::setUi_SettingsR(const CamPlayer::SettingsR& m)
{
    ui->label_examId->setText(m.video_folderName);
    ui->label_fcs->setText(m.fcs_folderName);
}

void MainWindow::setUi_LoadR(const CamPlayer::LoadR& m)
{
    if(m.isCanceled) return;
    ui->label_examId->setText(m.folderName);
}

void MainWindow::setUi_SaveFcsR(const CamPlayer::SaveFcsR& m)
{
    ui->label_fcs->setText(m.folderName);
}


void MainWindow::setUi_AddUnfcsR(CamPlayer::AddUnfcsR m)
{
    if(m.isok){
        QString hexstr = m.fc.toHexString();
        ui->listWidget_unfcs->addItem(hexstr);
    }
}

void MainWindow::setUi_AddFcsExtraR(const CamPlayer::AddFcsExtraR &m)
{
    if(m.isok){
        QString hexstr = m.fc.toHexString();
        ui->listWidget_fcs_extra->addItem(hexstr);
    }
}


void MainWindow::setUi_UnfcsDeleteItems(const QList<QListWidgetItem*>& items){
    foreach(QListWidgetItem * item, items)
    {
        delete ui->listWidget_unfcs->takeItem(ui->listWidget_unfcs->row(item));
    }
}

void MainWindow::setUi_FcsExtraDeleteItems(const QList<QListWidgetItem*>& items){
    foreach(QListWidgetItem * item, items)
    {
        delete ui->listWidget_fcs_extra->takeItem(ui->listWidget_fcs_extra->row(item));
    }
}

void MainWindow::setUi_picLabel(const QPixmap &pixmap, QLabel *label)
{
    if (!pixmap.isNull()) {
        label->setPixmap(pixmap);
    } else {
        QString msg = QStringLiteral("No pixmap");
//        if (CamPlayer::_unfcs->fcs.isEmpty())
//            msg+=QStringLiteral("\nNo color table");
        if (CamPlayer::trackingdata.fcix==-1){
            msg+=QStringLiteral("\nNo tracking color selected");
        }
        label->setText(msg);
    }
}

void MainWindow::setUi_DelUnfcsR(const CamPlayer::DelUnfcsR &m) // NOLINT(clazy-function-args-by-value)
{
    if(m.isok){
        QString hexstr = m.fc.toHexString();
        auto items = ui->listWidget_unfcs->findItems(hexstr, Qt::MatchExactly);
        setUi_UnfcsDeleteItems(items);

    }
}

void MainWindow::setUi_DelFcsExtraR(const CamPlayer::DelFcsExtraR &m) // NOLINT(clazy-function-args-by-value)
{
    if(m.isok){
        QString hexstr = m.fc.toHexString();
        auto items = ui->listWidget_fcs_extra->findItems(hexstr,
                                                         Qt::MatchExactly);
        setUi_FcsExtraDeleteItems(items);
    }
}

void MainWindow::setUi_GetTrackingUnfcR(const CamPlayer::GetTrackingUnfcR &m)
{
    ui->listWidget_unfcs->insertItems(0, m.unfcshex);
}

void MainWindow::setUi_GetTrackingFcsExtraR(
    const CamPlayer::TrackingFcsExtraR &m)
{
    ui->listWidget_fcs_extra->insertItems(0, m.unfcshex);
}

auto MainWindow::DrawFrame(const CamPlayer::FrameData* framedata
                           ,CamPlayer::FilterMode filterMode
                           ,int* total_pixel_counter
                           ,int* filtered_pixel_counter) -> QPixmap
{
    if(!framedata) return {};
    if(framedata->image.isNull()) return {};
    //int pixel_counter=0;
    //framedata->pixel_counter=0;
    *total_pixel_counter=0;
    *filtered_pixel_counter=0;
    QImage filtered_image =
        (filterMode == CamPlayer::Copy)
        ?framedata->image.copy()
        :CamPlayer::Filter(framedata->image
                          ,CamPlayer::trackingdata.fcix
                          ,filterMode
                          ,total_pixel_counter);

    if(filtered_image.isNull()) return{};

    if (filterMode!=CamPlayer::Copy) {
        CamPlayer::FilterStatR ignoretab(filtered_image.size());
        for(int i = 0; i < ui->listWidget->count(); ++i)
        {
            //auto item = ui->listWidget->item(i);
            auto d = itemData(ui->listWidget->item(i));

            if(d.vix==framedata->videoix &&
                d.fcix==CamPlayer::trackingdata.fcix)
                ignoretab.p[d.ix]=1;
        }
        CamPlayer::DrawFilterStat2(&filtered_image,
                                   ignoretab,
                                   Qt::red,
                                   false);

        CamPlayer::FilterStatR filterstat = CamPlayer::FilterStat(
            filtered_image,
            ignoretab);

        if(filterstat.pix_count>0)
        {
            CamPlayer::DrawFilterStat2(&filtered_image,
                                       filterstat,
                                       Qt::cyan,
                                       true);
        }
        *filtered_pixel_counter=filterstat.pix_count;
    }

    auto p = QPixmap::fromImage(filtered_image);
    QPainter painter(&p);

    CamPlayer::DrawMetaData(painter,
                            framedata->metadata,
                            framedata->image.size());
    painter.end();
    return p;
}

void MainWindow::setUi_ShowFrameR(const CamPlayer::ShowFrameR& m)
{
    ui->label_frameIx->setText(QString::number(m.frameix));

    _hasNext = m.hasNext;
    _hasPrev = m.hasPrev;

    _frame_1.framedata = m.framedata1;
    _frame_2.framedata = m.framedata2;
    _frame_3.framedata = m.framedata3;
    _frame_4.framedata = m.framedata4;

    RefreshFrames();
    RefreshZoom();
}

auto MainWindow::NumToString(int i) -> QString
{
    if(i==0) return QStringLiteral("");
    if(i>999) return QStringLiteral(">999");
    return QString::number(i);
};

void MainWindow::setUi_VideoBitmapData(VideoBitmapData& v)
{
    if (!v.framedata) return;

    if (v.framedata->image.isNull()) {
        v.label()->setText(QStringLiteral("No image in frame"));
        v.label_pix()->clear();
        return;
    }
    v.total_pixel_counter=0;
    v.filtered_pixel_counter=0;
    auto pixmap = DrawFrame(v.framedata
                            , v.filterMode
                            , &v.total_pixel_counter
                            , &v.filtered_pixel_counter);
    setUi_picLabel(pixmap, v.label());

    v.label_pix()->setText( NumToString(v.total_pixel_counter)
                           +'/'
                           +NumToString(v.filtered_pixel_counter));

    // ha átment valami a szűőn
    if (_timer.isActive()) {
        if(v.isT1() && v.filtered_pixel_counter>0){
            on_pushButton_stop_clicked();
        }
    }
}

void MainWindow::RefreshFrames(){
    setUi_VideoBitmapData(_frame_1);
    setUi_VideoBitmapData(_frame_2);
    setUi_VideoBitmapData(_frame_3);
    setUi_VideoBitmapData(_frame_4);
}

void MainWindow::RefreshFrame(int i)
{
    if (i==1) { setUi_VideoBitmapData(_frame_1); return;}
    if (i==2) { setUi_VideoBitmapData(_frame_2); return;}
    if (i==3) { setUi_VideoBitmapData(_frame_3); return;}
    if (i==4) { setUi_VideoBitmapData(_frame_4); return;}
}

void MainWindow::setUi_LoadFcsR(const CamPlayer::LoadFcsR& m)
{
    if(m.isCanceled) return;
    ui->label_fcs->setText(m.folderName+':'
                           +QString::number(m.fcs_count)+','
                           +QString::number(m.unfcs_count));
    RefreshUnfcs();
    RefreshZoom();
    RefreshFrames();
}

void MainWindow::RefreshUnfcs(){
    ui->listWidget_unfcs->clear();
    auto fcs = CamPlayer::GetTrackingUnfc();
    setUi_GetTrackingUnfcR(fcs);

    ui->listWidget_fcs_extra->clear();
    auto fcs_extra = CamPlayer::GetTrackingFcsExtraR();
    setUi_GetTrackingFcsExtraR(fcs_extra);
}

void MainWindow::RefreshZoom()
{
    auto m2 = CamPlayer::ShowTracking();
    setUi_ShowTrackingR(m2);
}

void MainWindow::on_MouseButtonPress(QWidget* w, QMouseEvent * event)
{
    QString txt;
    auto p = event->pos();

    int vix = picLabelIndex(w);

    //if(vix==-1) return;
    txt = QStringLiteral("%3: %1,%2").arg(p.x()).arg(p.y()).arg(vix);

    //ahogy aránylik a widget a picturehoz

    auto l = reinterpret_cast<QLabel*>(w); //NOLINT

    auto pixmap = l->pixmap(Qt::ReturnByValue);
    if(!pixmap.isNull()){
        if(vix!=-1){// a videón
            double rx = pixmap.width()/l->width();
            double ry = pixmap.height()/l->height();

            double x = p.x()*rx;
            double y = p.y()*ry;

            auto txt2 = QStringLiteral("%1,%2").arg(x).arg(y);
            txt+=' '+txt2;

            //megszerezzük a colort és leírjuk az rgb-ből csinálunk stringet
            QColor color = CamPlayer::GetColor(vix, -1, x, y);            
            auto color_str = CamPlayer::toString(color);
            auto color_hexstr = CamPlayer::toHexString(color);
            txt+= " ("+color_str+") "+color_hexstr;

            int ballIx = CamPlayer::GetBallIx(vix, -1, x, y, -1);
            int color_ix = -1;
            if(ballIx!=-1 && !ui->pushButton_fc_lock->isChecked()){
                color_ix = CamPlayer::GetColorIx(color);
                // majd a colorwheel alapján szerzünk hozzá egy szabványos nevet
                //QString colorname = FriendlyRGB::GetName(color_ix);//CamPlayer::GetColorName(color);
                //if(!colorname.isEmpty()) txt+=' '+colorname;
                ui->listWidget_col_names->setCurrentRow(color_ix);
            }

            //int ballIx = CamPlayer::GetBallIx(vix, -1, x, y, -1);
            //if(ballIx==-1)color_ix=-1; //nincs ball, előző szín marad
            // TODO 99 itt a kattintás színe a releváns ?
            auto trackingR = CamPlayer::SetTracking(vix,
                                                    -1,
                                                    ballIx ,
                                                    color_ix,
                                                    x,
                                                    y);
            QString tracking_txt = CamPlayer::ShowTrackingTxt();
            RefreshZoom();
            ui->label_tracking->setText(tracking_txt);
            if(trackingR.fcix_changed) RefreshUnfcs();
            auto r2 = CamPlayer::GotoFrame(-1);
            setUi_ShowFrameR(r2);

        }
        else{ // a zoomon
            int zix = zoomLabelIndex(w);
            if(zix!=-1){
                QColor c;
                ZoomBitmapData z;
                if(zix==1) {
                    c = _original_zoom.pixmap.copy(p.x(),
                                                   p.y(),
                                                   1,
                                                   1).toImage().pixel(0,0);
                    //pxm = _original_zoom.pixmap.copy();
                    z = _original_zoom;

                }
                if(zix==2){
                    c = CamPlayer::GetTrackingColor(p, l->size());
//                    pxm = _original_filtered.pixmap.copy();
                    z = _original_filtered;
                }
                if(c.isValid())
                {
                    int color_ix = CamPlayer::GetColorIx(c);
                    if(color_ix>-1){
                        ui->label_unfcs_name->setText(
                            FriendlyRGB::WheelColorsRYBHumNames[color_ix]);
                    }
                    auto lab = FriendlyRGB::toLab(c.red(), c.green(),c.blue());
                    QString labtxt =
                        QString::number(lab.l, 'f', 0)+' '+
                        QString::number(lab.a, 'f', 0)+' '+
                        QString::number(lab.b, 'f', 0);
                    ui->label_tracking->setText(labtxt);


                    QString txt4 = CamPlayer::toHexString(c); //sima hex
                    ui->label_filterclick->setText(txt4);

                    int f_int = FriendlyRGB::ToFriendlyInt(
                        static_cast<byte>(c.red()),
                        static_cast<byte>(c.green()),
                        static_cast<byte>(c.blue()));
                    QString f_txt4 =
                        FriendlyRGB::FromFriendlyInt(f_int).toHexString();
                    ui->label_unfcs->setText(f_txt4); //friendly hex


                    CamPlayer::SetTrackingColor(c);
                    CamPlayer::TrackingColor tcolor =
                        CamPlayer::GetTrackingColor();
                    setUi_TrackingColor(tcolor);

                    {
                    auto items =
                            ui->listWidget_unfcs->findItems(f_txt4,
                                                            Qt::MatchExactly);
                    ButtonEnable(ui->pushButton_unfc_del, !items.isEmpty());
                    ui->listWidget_unfcs->clearSelection();
                    for(auto&item:items) item->setSelected(true);
                    }
                    {
                    auto items2 =
                            ui->listWidget_fcs_extra->findItems(
                                f_txt4,
                                Qt::MatchExactly);
                    ButtonEnable(ui->pushButton_fcs_extra_del,
                                 !items2.isEmpty());
                    ui->listWidget_fcs_extra->clearSelection();
                    for(auto&item:items2) item->setSelected(true);
                    }               

//                    auto tsize = CamPlayer::trackingdata_image_size();
//                    int rx = pxm.width()/tsize.width();
//                    int ry = pxm.height()/tsize.height();
//                    QPainter painter(&pxm);
//                    painter.setPen(Qt::darkCyan);
//                    painter.drawRect((p.x()/rx)*rx-1, (p.y()/ry)*ry-1, rx+1, ry+1);
//                    l->setPixmap(pxm);
                    if(z.isValid){
                        //QPixmap pxm = z.pixmap.copy();
                        QPixmap pxm = DrawMarker_ZoomBitmapData(z, p);
                        if(!pxm.isNull()) l->setPixmap(pxm);
                    }
                }
            }
        }
    }

    ui->label_msg->setText(txt);
}

void MainWindow::on_MouseMove(QWidget* w, QMouseEvent * event){
    auto p = event->pos();
    auto l = reinterpret_cast<QLabel*>(w); //NOLINT

    int vix = picLabelIndex(w);
    if(vix==-1){
        int zix = zoomLabelIndex(w);
        if(zix!=-1){
            ZoomBitmapData z;

            if (zix==1) {z = _original_zoom;
            } else if (zix==2) {z = _original_filtered;
            }
            if(z.isValid){
                l->setCursor(Qt::BlankCursor);                
                QPixmap pxm = DrawMarker_ZoomBitmapData(z, p);
                if(!pxm.isNull()) l->setPixmap(pxm);
                //l->setPixmap(pxm);
            } else {
                l->setCursor(Qt::ArrowCursor);
            }
        }
    }
}

void MainWindow::on_MouseLeave(QWidget *w, QMouseEvent *event){
    Q_UNUSED(event)
    auto l = reinterpret_cast<QLabel*>(w); //NOLINT
    //l->setCursor(Qt::ArrowCursor);
    int vix = picLabelIndex(w);
    if(vix==-1){
        int zix = zoomLabelIndex(w);
        if(zix!=-1){
            ZoomBitmapData z;

            if (zix==1) {z = _original_zoom;
            } else if (zix==2) {z = _original_filtered;
            }
            l->setPixmap(z.pixmap.copy());
        }
    }
}

void MainWindow::on_MouseEnter(QWidget *w, QMouseEvent *event){
    Q_UNUSED(event)
    auto l = reinterpret_cast<QLabel*>(w); //NOLINT
    l->setCursor(Qt::ArrowCursor);
}

auto MainWindow::DrawMarker_ZoomBitmapData(const ZoomBitmapData& z,
                                           const QPoint& p) -> QPixmap{
    QPixmap pxm = z.pixmap.copy();
    //QSize& tsize
    //if(!pxm) return;
    //if(pxm->isNull()) return;
    if(z.size.width()==0 || z.size.height()==0) return{};
    double rx = (double)pxm.width()/z.size.width();//NOLINT
    double ry = (double)pxm.height()/z.size.height();//NOLINT
    QPainter painter(&pxm);
    painter.setPen(Qt::cyan);
    int x0 = p.x()/rx;
    int y0 = p.y()/ry;
    double x = x0*rx;
    double y = y0*ry;
    painter.drawRect(x-1, y-1, rx+1, ry+1);
    painter.drawRect(x-2, y-2, rx+3, ry+3);
    ui->label_tracking_xy->setText(QStringLiteral("%1,%2").arg(x0).arg(y0));
    painter.end();
    return pxm;
}

void MainWindow::ButtonEnable(QPushButton *b, bool s){
    b->setEnabled(s);
    auto p = b->palette();
    static const QColor original_color = p.color(QPalette::Button);
    if(s){
        p.setColor(QPalette::Button, Qt::darkGreen);
        b->setPalette(p);
    }
    else{
        p.setColor(QPalette::Button, original_color);
        b->setPalette(p);
    }

}

void MainWindow::setUi_ShowTrackingR(const CamPlayer::ShowTrackingR& m){
    auto size2 = ui->label_zoom->size();    
    if(m.image.isNull()){
        ui->label_zoom->clear();
        _original_zoom.isValid = false;
    }
    else{
        auto pixmap_zoom =
            QPixmap::fromImage(m.image).scaled(size2,
                                               Qt::KeepAspectRatio,
                                               Qt::FastTransformation);
        ui->label_zoom->setPixmap(pixmap_zoom);
        _original_zoom.pixmap = pixmap_zoom;
        _original_zoom.size = m.image.size();
        _original_zoom.isValid = true;
    }

    auto size3 = ui->label_zoom_2->size();
    if(m.image_filtered.isNull()){        
        ui->label_zoom_2->clear();
        _original_filtered.isValid = false;
        //set label zoom
        QString msg;
        if(m.fcs_count==0) { msg +=  QStringLiteral("No colors loaded");
        }
        else{ msg+= QStringLiteral("No tracking data");
        }
        ui->label_zoom_2->setText(msg);
    }else{
        auto pixmap_filtered =
            QPixmap::fromImage(m.image_filtered).scaled(size3,
                                                        Qt::KeepAspectRatio,
                                                        Qt::FastTransformation);
        ui->label_zoom_2->setPixmap(pixmap_filtered);
        _original_filtered.pixmap=pixmap_filtered;
        _original_filtered.size = m.image_filtered.size();
        _original_filtered.isValid = true;
    }

    if( _timer.isActive())
    {
        if(m.fpixel_count>0 && ui->radioButton_t1->isChecked()){
            on_pushButton_stop_clicked();
        } else if (m.fpixel_count==0 && ui->radioButton_t2->isChecked()) {
            on_pushButton_stop_clicked();
        }
    }
}

void MainWindow::setUi_TrackingColor(const CamPlayer::TrackingColor& m){
    //int a = m.color.
    QPixmap p(1,1);
    p.fill(m.color);
    ui->label_color->setPixmap(p);
    p.fill(QColor(m.fc.r, m.fc.g, m.fc.b));
    ui->label_unfc->setPixmap(p);
}


void MainWindow::setUi_FilterMode2(CamPlayer::FilterMode mode,
                                   QRadioButton* c,
                                   QRadioButton* f,
                                   QRadioButton* a) {
    switch(mode){
    case CamPlayer::FilterMode::Copy:c->setChecked(true);break;
    case CamPlayer::FilterMode::IsFriendly:f->setChecked(true);break;
    case CamPlayer::FilterMode::AllFriendly:a->setChecked(true);break;
    }
}

void MainWindow::setUi_FilterMode(CamPlayer::FilterMode mode){
    setUi_FilterMode2(mode,
                     ui->radioButton_copy,
                     ui->radioButton_isfriendly,
                     ui->radioButton_allfriendly);
}

void MainWindow::setUi_FilterMode1(CamPlayer::FilterMode mode){
    setUi_FilterMode2(mode,
                      ui->radioButton_copy_1,
                      ui->radioButton_isfriendly_1,
                      ui->radioButton_allfriendly_1);
}

void MainWindow::setUi_FilterMode2(CamPlayer::FilterMode mode){
    setUi_FilterMode2(mode,
                      ui->radioButton_copy_2,
                      ui->radioButton_isfriendly_2,
                      ui->radioButton_allfriendly_2);
}

void MainWindow::setUi_FilterMode3(CamPlayer::FilterMode mode){
    setUi_FilterMode2(mode,
                      ui->radioButton_copy_3,
                      ui->radioButton_isfriendly_3,
                      ui->radioButton_allfriendly_3);
}

void MainWindow::setUi_FilterMode4(CamPlayer::FilterMode mode){
    setUi_FilterMode2(mode,
                      ui->radioButton_copy_3,
                      ui->radioButton_isfriendly_3,
                      ui->radioButton_allfriendly_3);
}


void MainWindow::on_listWidget_fcs_extra_itemClicked(QListWidgetItem *item)
{
    ButtonEnable(ui->pushButton_fcs_extra_del, true);

    auto items = ui->listWidget_unfcs->findItems(item->text(),
                                                 Qt::MatchExactly);
    ButtonEnable(ui->pushButton_unfc_del, !items.isEmpty());

    ui->listWidget_unfcs->clearSelection();
    for(auto&item:items) item->setSelected(true);
}


void MainWindow::on_listWidget_unfcs_itemClicked(QListWidgetItem *item)
{
    ButtonEnable(ui->pushButton_unfc_del, true);

    auto items = ui->listWidget_fcs_extra->findItems(item->text(),
                                                     Qt::MatchExactly);
    ButtonEnable(ui->pushButton_fcs_extra_del, !items.isEmpty());

    ui->listWidget_fcs_extra->clearSelection();
    for(auto&item:items) item->setSelected(true);
}

void MainWindow::on_listWidget_col_names_currentRowChanged(int color_ix)
{
    auto trackingR = CamPlayer::SetTrackingFcix(color_ix);
    QString tracking_txt = CamPlayer::ShowTrackingTxt();
    ui->label_tracking->setText(tracking_txt);
    if(trackingR.fcix_changed) RefreshUnfcs();
    RefreshZoom();
    RefreshFrames();
}


void MainWindow::on_pushButton_clicked()
{
    ui->spinBox_timer->setValue(ui->spinBox_timer->minimum());
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->spinBox_timer->setValue(ui->spinBox_timer->maximum());
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->spinBox_timer->setValue(16);
}

/*filter radiobuttons*/

void MainWindow::setFilterMode(VideoBitmapData *videodata,
                               CamPlayer::FilterMode mode){
    videodata->filterMode = mode;
    setUi_VideoBitmapData(*videodata);
    //setUi_ShowFrame(frame->framedata, frame->filterMode, frame->label());
}

void MainWindow::on_radioButton_copy_1_clicked()
{
    setFilterMode(&_frame_1, CamPlayer::FilterMode::Copy);
}

void MainWindow::on_radioButton_allfriendly_1_clicked()
{
    setFilterMode(&_frame_1, CamPlayer::FilterMode::AllFriendly);
}

void MainWindow::on_radioButton_isfriendly_1_clicked()
{
    setFilterMode(&_frame_1, CamPlayer::FilterMode::IsFriendly);
}


void MainWindow::on_radioButton_copy_2_clicked()
{
    setFilterMode(&_frame_2, CamPlayer::FilterMode::Copy);
}


void MainWindow::on_radioButton_allfriendly_2_clicked()
{
    setFilterMode(&_frame_2, CamPlayer::FilterMode::AllFriendly);
}


void MainWindow::on_radioButton_isfriendly_2_clicked()
{
    setFilterMode(&_frame_2, CamPlayer::FilterMode::IsFriendly);
}


void MainWindow::on_radioButton_copy_3_clicked()
{
    setFilterMode(&_frame_3, CamPlayer::FilterMode::Copy);
}


void MainWindow::on_radioButton_allfriendly_3_clicked()
{
    setFilterMode(&_frame_3, CamPlayer::FilterMode::AllFriendly);
}


void MainWindow::on_radioButton_isfriendly_3_clicked()
{
    setFilterMode(&_frame_3, CamPlayer::FilterMode::IsFriendly);
}


void MainWindow::on_radioButton_copy_4_clicked()
{
    setFilterMode(&_frame_4, CamPlayer::FilterMode::Copy);
}


void MainWindow::on_radioButton_allfriendly_4_clicked()
{
    setFilterMode(&_frame_4, CamPlayer::FilterMode::AllFriendly);
}


void MainWindow::on_radioButton_isfriendly_4_clicked()
{
    setFilterMode(&_frame_4, CamPlayer::FilterMode::IsFriendly);
}


void MainWindow::on_pushButton_4_clicked()
{
    CamPlayer::DeleteTracking();
    RefreshZoom();
    RefreshFrames();
}

void MainWindow::on_pushButton_ign_add_clicked()
{
    if(!CamPlayer::trackingdata.isValid()) return;
    if(CamPlayer::trackingdata.fcix==-1) return;

    QLabel* l = picLabel(CamPlayer::trackingdata.vix);
    auto pixmap = l->pixmap(Qt::ReturnByValue);

    CamPlayer::FilterIx fix(pixmap.size(), CamPlayer::FILTER_W);

    auto itemData = CamPlayer::trackingdata.ignoreData();

    auto mp = fix.mpoint(CamPlayer::trackingdata.mpoint());
    itemData.setIx(fix.ix(mp));

    auto itemtxt = itemData.toString();
    auto item = new QListWidgetItem({}, itemtxt, ui->listWidget);
    setItemData(item, itemData);

    RefreshFrame(CamPlayer::trackingdata.vix);
}

void MainWindow::setItemData(QListWidgetItem* item,
                             const CamPlayer::TrackingData::IgnoreData& data){
    item->setData(IgnoreListKeys::ix, data.ix);
    item->setData(IgnoreListKeys::fcix, data.fcix);
    item->setData(IgnoreListKeys::vix, data.vix);
    item->setData(IgnoreListKeys::x, data.x);
    item->setData(IgnoreListKeys::y, data.y);
}

auto MainWindow::itemData(QListWidgetItem* item) ->
    CamPlayer::TrackingData::IgnoreData
{
    return{
        item->data(IgnoreListKeys::vix).toInt(),
        item->data(IgnoreListKeys::fcix).toInt(),
        item->data(IgnoreListKeys::x).toInt(),
        item->data(IgnoreListKeys::y).toInt(),
        item->data(IgnoreListKeys::ix).toInt(),
    };
}


