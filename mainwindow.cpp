#include "mainwindow.h"
#include "mousebuttonsignaler.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <Qt>
extern MouseButtonSignaler signaler;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setUi_TrackingColor(CamPlayer::Settings());

    QSize s = ui->pushButton_next->size();


    QRect r(3,3,s.width()-6,s.height()-8);

    auto pixmap = ui->pushButton_next->grab(r);

    QIcon icon;
    //QPixmap pixmap = QPixmap(r'C:\Users\git\Desktop\test.png').scaled(QSize(160, 90))
    icon.addPixmap(pixmap.transformed(QTransform().scale(-1,1)));
    //ui->pushButton_prev->setText(" ");
    ui->pushButton_prev->setIcon(icon);
    ui->pushButton_prev->setIconSize(r.size());
    //ui->pushButton_prev->resize(ui->pushButton_next->frameSize());
    //pixmap.

    //    ui->pushButton_prev->setGraphicsEffect()

    signaler.installOn(ui->label_pic1);
    signaler.installOn(ui->label_pic2);
    signaler.installOn(ui->label_pic3);
    signaler.installOn(ui->label_pic4);

    signaler.installOn(ui->label_zoom);
    signaler.installOn(ui->label_zoom_2);

    QObject::connect(&signaler, &MouseButtonSignaler::mouseButtonEvent, this, &MainWindow::on_click);

    setUi_TrackingColor(CamPlayer::GetTrackingFilterMode());
    ui->pushButton_dir->setChecked(direction);
    RefreshDirLabel();

    ui->spinBox_timer->setValue(timer_step);
    timer.setInterval(timer_step);
    connect(&timer, &QTimer::timeout, this, &MainWindow::on_timeout);

    //listWidget_col_names
    for(int i=0;i<6;i++){
        QPixmap px(16, 16);
        FriendlyRGB a = FriendlyRGB::WheelColorsRYB[i];
        QColor c(a.r, a.g, a.b);
        px.fill(c);
        QIcon ic(px);
        auto item=new QListWidgetItem (ic, FriendlyRGB::WheelColorsRYBHumNames[i]);
        ui->listWidget_col_names->insertItem(i,item);
    }
}

void MainWindow::RefreshDirLabel(){
    ui->pushButton_dir->setText(direction?QStringLiteral("➡"):QStringLiteral("⬅"));
}

auto MainWindow::GetPicLabelIndex(QWidget* w) -> int
{
    if(w==ui->label_pic1) return 1;
    if(w==ui->label_pic2) return 2;
    if(w==ui->label_pic3) return 3;
    if(w==ui->label_pic4) return 4;
    return -1;
}

auto MainWindow::GetZoomLabelIndex(QWidget* w) -> int
{
    if(w==ui->label_zoom) return 1;
    if(w==ui->label_zoom_2) return 2;
    return -1;
}

void MainWindow::on_click(QWidget* w, QMouseEvent* e)
{
    if (e->type() == QEvent::MouseButtonPress) onMouseButtonPress(w, e);
}

void MainWindow::on_timeout(){
    if(direction) on_pushButton_next_clicked();  //NOLINT
    else on_pushButton_prev_clicked(); //NOLINT
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
    if(timer.isActive()) return;
    auto r = CamPlayer::Load(this);
    setUi_TrackingColor(r);
    auto r2 = CamPlayer::GotoFrame(0);
    setUi_TrackingColor(r2);
}

void MainWindow::on_pushButton_next_clicked()
{
    CamPlayer::ShowFrameR m = CamPlayer::NextFrame();
    setUi_TrackingColor(m);
}

void MainWindow::on_pushButton_prev_clicked()
{
    CamPlayer::ShowFrameR m = CamPlayer::PrevFrame();
    setUi_TrackingColor(m);
}


void MainWindow::on_pushButton_start_clicked()
{
    timer.start();
}

void MainWindow::on_pushButton_start_clicked(bool checked)
{
    if(checked){
        timer.start();
        ui->pushButton_start->setText(QStringLiteral("⏸"));
    }
    else{
        timer.stop();
        ui->pushButton_start->setText(QStringLiteral("▶"));
    }
}


void MainWindow::on_pushButton_stop_clicked()
{
    timer.stop();
    ui->pushButton_start->setText(QStringLiteral("▶"));
}

void MainWindow::on_pushButton_rew_clicked()
{
    CamPlayer::ShowFrameR m = CamPlayer::GotoFrame(0);
    setUi_TrackingColor(m);
}

void MainWindow::on_pushButton_loadfcs_clicked()
{
    CamPlayer::LoadFcsR m = CamPlayer::LoadFcs();
    setUi_TrackingColor(m);

    RefreshUnfcs();
    //RefreshFcsExtra();
    RefreshZoom();
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
    timer_step = arg1;
    timer.setInterval(timer_step);
}

void MainWindow::on_pushButton_unfc_add_clicked()
{
    CamPlayer::AddUnfcsR m =CamPlayer::AddUnfcs();
    setUi_TrackingColor(m);
    if(m.isok) RefreshZoom();

}

void MainWindow::on_pushButton_fcs_extra_add_clicked()
{
    CamPlayer::AddFcsExtraR m =CamPlayer::AddFcs();
    setUi_TrackingColor(m);
    if(m.isok) RefreshZoom();

}


void MainWindow::on_pushButton_unfc_del_clicked()
{
    auto items = ui->listWidget_unfcs->selectedItems();
    //ui->pushButton_unfc_del->setDisabled(items.isEmpty());
    //if(items.empty()) return;

    for(auto&item:items){
        auto txt = item->text();
        CamPlayer::DelUnfcsR m = CamPlayer::DelUnfcs(txt);
        setUi_TrackingColor(m);
        if(m.isok) RefreshZoom();
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
        setUi_TrackingColor(m);
        if(m.isok) RefreshZoom();
        //delete ui->listWidget_unfcs->takeItem(ui->listWidget_unfcs->row(item));
    }
}

void MainWindow::on_pushButton_dir_clicked(bool checked)
{
    direction = checked;
    RefreshDirLabel();
}

void MainWindow::on_pushButton_savefcs_clicked()
{
    auto m = CamPlayer::SaveUnfcs();
    setUi_TrackingColor(m);
}
/*
setUi
*/

void MainWindow::setUi_TrackingColor(const CamPlayer::SettingsR& m)
{
    ui->label_examId->setText(m.video_folderName);
    ui->label_fcs->setText(m.fcs_folderName);
}

void MainWindow::setUi_TrackingColor(const CamPlayer::LoadR& m)
{
    ui->label_examId->setText(m.folderName);
}

void MainWindow::setUi_TrackingColor(const CamPlayer::SaveFcsR& m)
{
    ui->label_fcs->setText(m.folderName);
}


void MainWindow::setUi_TrackingColor(CamPlayer::AddUnfcsR m)
{
    if(m.isok){
        QString hexstr = m.fc.toHexString();
        ui->listWidget_unfcs->addItem(hexstr);
    }
}

void MainWindow::setUi_TrackingColor(const CamPlayer::AddFcsExtraR &m)
{
    if(m.isok){
        QString hexstr = m.fc.toHexString();
        ui->listWidget_fcs_extra->addItem(hexstr);
    }
}


void MainWindow::listWidget_unfcs_delete_items(const QList<QListWidgetItem*>& items){
    foreach(QListWidgetItem * item, items)
    {
        delete ui->listWidget_unfcs->takeItem(ui->listWidget_unfcs->row(item));
    }
}

void MainWindow::listWidget_fcs_extra_delete_items(const QList<QListWidgetItem*>& items){
    foreach(QListWidgetItem * item, items)
    {
        delete ui->listWidget_fcs_extra->takeItem(ui->listWidget_fcs_extra->row(item));
    }
}

void MainWindow::setUi_TrackingColor(const CamPlayer::DelUnfcsR &m) // NOLINT(clazy-function-args-by-value)
{
    if(m.isok){
        QString hexstr = m.fc.toHexString();
        auto items = ui->listWidget_unfcs->findItems(hexstr, Qt::MatchExactly);
        listWidget_unfcs_delete_items(items);

    }
}

void MainWindow::setUi_TrackingColor(const CamPlayer::DelFcsExtraR &m) // NOLINT(clazy-function-args-by-value)
{
    if(m.isok){
        QString hexstr = m.fc.toHexString();
        auto items = ui->listWidget_fcs_extra->findItems(hexstr, Qt::MatchExactly);
        listWidget_fcs_extra_delete_items(items);
    }
}

void MainWindow::setUi_GetTrackingUnfcR(const CamPlayer::GetTrackingUnfcR &m)
{
    ui->listWidget_unfcs->insertItems(0, m.unfcshex);
    //ui->label_unfcs_name->setText(m.name);
}

void MainWindow::setUi_GetTrackingFcsExtraR(const CamPlayer::TrackingFcsExtraR &m)
{
    ui->listWidget_fcs_extra->insertItems(0, m.unfcshex);
    //ui->label_fcs_extra_name->setText(m.name);
}

void MainWindow::setUi_TrackingColor(const CamPlayer::ShowFrameR& m)
{
    ui->label_frameIx->setText(QString::number(m.frameix));

    if(m.framedata1==nullptr || m.framedata1->image.isNull()) {
        ui->label_pic1->clear();
    } else {
        QPixmap pixmap(m.framedata1->image.size());
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.drawPixmap(0,0,QPixmap::fromImage(m.framedata1->image));
        CamPlayer::DrawMetaData(painter, m.framedata1->metadata, m.framedata1->image.size());
        painter.end();
        ui->label_pic1->setPixmap(pixmap);
    }

    if(m.framedata2==nullptr || m.framedata2->image.isNull()) {
        ui->label_pic2->clear();
    } else {
        QPixmap pixmap(m.framedata2->image.size());
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.drawPixmap(0,0,QPixmap::fromImage(m.framedata2->image));
        CamPlayer::DrawMetaData(painter, m.framedata2->metadata, m.framedata2->image.size());
        painter.end();
        ui->label_pic2->setPixmap(pixmap);
    }

    if(m.framedata3==nullptr || m.framedata3->image.isNull()) {
        ui->label_pic3->clear();
    } else {
        QPixmap pixmap(m.framedata3->image.size());
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.drawPixmap(0,0,QPixmap::fromImage(m.framedata3->image));
        CamPlayer::DrawMetaData(painter, m.framedata3->metadata, m.framedata3->image.size());
        painter.end();
        ui->label_pic3->setPixmap(pixmap);
    }

    if(m.framedata4==nullptr || m.framedata4->image.isNull()) {
        ui->label_pic4->clear();
    } else {
        QPixmap pixmap(m.framedata4->image.size());
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.drawPixmap(0,0,QPixmap::fromImage(m.framedata4->image));
        CamPlayer::DrawMetaData(painter, m.framedata4->metadata, m.framedata4->image.size());
        painter.end();

        ui->label_pic4->setPixmap(pixmap);
    }

    RefreshZoom();
}

void MainWindow::setUi_TrackingColor(const CamPlayer::LoadFcsR& m)
{
    ui->label_fcs->setText(m.folderName+':'+QString::number(m.fcs_count)+','+QString::number(m.unfcs_count));
    RefreshUnfcs();
}

void MainWindow::RefreshUnfcs(){
    ui->listWidget_unfcs->clear();
    auto fcs = CamPlayer::GetTrackingUnfc();
    setUi_GetTrackingUnfcR(fcs);

    ui->listWidget_fcs_extra->clear();
    auto fcs_extra = CamPlayer::GetTrackingFcsExtraR();
    setUi_GetTrackingFcsExtraR(fcs_extra);
}

//void MainWindow::RefreshFcsExtra(){
//    ui->listWidget_fcs_extra->clear();
//    auto fcs = CamPlayer::GetTrackingFcsExtra();
//    setUi_GetTrackingUnfcR(fcs);
//}

void MainWindow::RefreshZoom()
{
    auto m2 = CamPlayer::ShowTracking();
    setUi_ShowTrackingR(m2);
}

void MainWindow::onMouseButtonPress(QWidget* w, QMouseEvent * event)
{
    QString txt;
    auto p = event->pos();

    int vix = GetPicLabelIndex(w);

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
            int color_ix;
            if(ballIx!=-1){
                color_ix = CamPlayer::GetColorIx(color);
                // majd a colorwheel alapján szerzünk hozzá egy szabványos nevet
                //QString colorname = FriendlyRGB::GetName(color_ix);//CamPlayer::GetColorName(color);
                //if(!colorname.isEmpty()) txt+=' '+colorname;
                ui->listWidget_col_names->setCurrentRow(color_ix);
            }else color_ix=-1;

            //int ballIx = CamPlayer::GetBallIx(vix, -1, x, y, -1);
            //if(ballIx==-1)color_ix=-1; //nincs ball, előző szín marad
            // TODO 99 itt a kattintás színe a releváns ?
            auto trackingR = CamPlayer::SetTracking(vix, -1, ballIx , color_ix, x, y);
            QString tracking_txt = CamPlayer::ShowTrackingTxt();
            RefreshZoom();
            ui->label_tracking->setText(tracking_txt);
            if(trackingR.fcix_changed) RefreshUnfcs();
            auto r2 = CamPlayer::GotoFrame(-1);
            setUi_TrackingColor(r2);

        }
        else{ // a zoomon
            int zix = GetZoomLabelIndex(w);
            if(zix!=-1){
                QColor c;
                if(zix==1) {
                    c = pixmap.copy(p.x(),p.y(), 1,1).toImage().pixel(0,0);
                }
                if(zix==2) c = CamPlayer::GetTrackingColor(p, l->size());
                if(c.isValid())
                {
                    int color_ix = CamPlayer::GetColorIx(c);
                    if(color_ix>-1){
                        ui->label_unfcs_name->setText(FriendlyRGB::WheelColorsRYBHumNames[color_ix]);
                    }
                    auto lab = FriendlyRGB::toLab(c.red(), c.green(),c.blue());
                    QString labtxt =
                        QString::number(lab.l, 'f', 0)+' '+
                        QString::number(lab.a, 'f', 0)+' '+
                        QString::number(lab.b, 'f', 0);
                    ui->label_tracking->setText(labtxt);


                    QString txt4 = CamPlayer::toHexString(c); //sima hex
                    ui->label_filterclick->setText(txt4);

                    int f_int = FriendlyRGB::ToFriendlyInt((byte)c.red(), (byte)c.green(), (byte)c.blue());
                    QString f_txt4 = FriendlyRGB::FromFriendlyInt(f_int).toHexString();
                    ui->label_unfcs->setText(f_txt4); //friendly hex


                    CamPlayer::SetTrackingColor(c);
                    CamPlayer::TrackingColor tcolor = CamPlayer::GetTrackingColor();
                    setUi_TrackingColor(tcolor);

                    {
                    auto items = ui->listWidget_unfcs->findItems(f_txt4, Qt::MatchExactly);
                    ButtonEnable(ui->pushButton_unfc_del, !items.isEmpty());
                    ui->listWidget_unfcs->clearSelection();
                    for(auto&item:items) item->setSelected(true);
                    }
                    {
                    auto items2 = ui->listWidget_fcs_extra->findItems(f_txt4, Qt::MatchExactly);
                    ButtonEnable(ui->pushButton_fcs_extra_del, !items2.isEmpty());
                    ui->listWidget_fcs_extra->clearSelection();
                    for(auto&item:items2) item->setSelected(true);
                    }
                }
            }
        }
    }

    ui->label_msg->setText(txt);
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
    }
    else{
        auto pixmap_zoom = QPixmap::fromImage(m.image).scaled(size2,Qt::KeepAspectRatio, Qt::TransformationMode::FastTransformation);
        ui->label_zoom->setPixmap(pixmap_zoom);
    }

    auto size3 = ui->label_zoom_2->size();
    if(m.image_filtered.isNull()){
        ui->label_zoom_2->clear();
    }else{
        auto pixmap_filtered = QPixmap::fromImage(m.image_filtered).scaled(size3,Qt::KeepAspectRatio, Qt::TransformationMode::FastTransformation);
        ui->label_zoom_2->setPixmap(pixmap_filtered);
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

void MainWindow::setUi_TrackingColor(CamPlayer::FilterMode mode){
    switch(mode){
    case CamPlayer::FilterMode::Copy:ui->radioButton_copy->setChecked(true);break;
    case CamPlayer::FilterMode::IsFriendly:ui->radioButton_isfriendly->setChecked(true);break;
    case CamPlayer::FilterMode::AllFriendly:ui->radioButton_allfriendly->setChecked(true);break;
    }
}


void MainWindow::on_listWidget_fcs_extra_itemClicked(QListWidgetItem *item)
{
    ButtonEnable(ui->pushButton_fcs_extra_del, true);

    auto items = ui->listWidget_unfcs->findItems(item->text(), Qt::MatchExactly);
    ButtonEnable(ui->pushButton_unfc_del, !items.isEmpty());

    ui->listWidget_unfcs->clearSelection();
    for(auto&item:items) item->setSelected(true);
}


void MainWindow::on_listWidget_unfcs_itemClicked(QListWidgetItem *item)
{
    ButtonEnable(ui->pushButton_unfc_del, true);

    auto items = ui->listWidget_fcs_extra->findItems(item->text(), Qt::MatchExactly);
    ButtonEnable(ui->pushButton_fcs_extra_del, !items.isEmpty());

    ui->listWidget_fcs_extra->clearSelection();
    for(auto&item:items) item->setSelected(true);
}

