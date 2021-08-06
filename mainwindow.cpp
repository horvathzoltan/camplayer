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

    setUi(CamPlayer::Settings());

    connect(&timer, &QTimer::timeout, this, &MainWindow::on_timeout);
    timer.setInterval(16);

    signaler.installOn(ui->label_pic1);
    signaler.installOn(ui->label_pic2);
    signaler.installOn(ui->label_pic3);
    signaler.installOn(ui->label_pic4);

    signaler.installOn(ui->label_zoom);
    signaler.installOn(ui->label_zoom_2);

    QObject::connect(&signaler, &MouseButtonSignaler::mouseButtonEvent, this, &MainWindow::on_click);

    setUi(CamPlayer::GetTrackingFilterMode());
    ui->pushButton_dir->setChecked(direction);
    setDirLabel();
}

void MainWindow::setDirLabel(){
    ui->pushButton_dir->setText(direction?QStringLiteral(">"):QStringLiteral("<"));
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
    if(direction) { on_pushButton_next_clicked();
    }
    else on_pushButton_prev_clicked();
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
    setUi(r);
    auto r2 = CamPlayer::GotoFrame(0);
    setUi(r2);
}

void MainWindow::on_pushButton_next_clicked()
{
    CamPlayer::ShowFrameR m = CamPlayer::NextFrame();
    setUi(m);    
}

void MainWindow::on_pushButton_prev_clicked()
{
    CamPlayer::ShowFrameR m = CamPlayer::PrevFrame();
    setUi(m);
}


void MainWindow::on_pushButton_start_clicked()
{
    timer.start();
}


void MainWindow::on_pushButton_stop_clicked()
{
    timer.stop();
}

void MainWindow::on_pushButton_rew_clicked()
{
    CamPlayer::ShowFrameR m = CamPlayer::GotoFrame(0);
    setUi(m);
}

void MainWindow::on_pushButton_loadfcs_clicked()
{
    auto m = CamPlayer::LoadFcs();
    setUi(m);

    auto m2 = CamPlayer::GetTrackingUnfc();
    setUi(m2);
}

void MainWindow::on_radioButton_copy_clicked()
{
    CamPlayer::SetTracking(CamPlayer::FilterMode::Copy);
    auto m2 = CamPlayer::ShowTracking();
    setUi(m2);
}

void MainWindow::on_radioButton_isfriendly_clicked()
{
    CamPlayer::SetTracking(CamPlayer::FilterMode::IsFriendly);
    auto m2 = CamPlayer::ShowTracking();
    setUi(m2);
}


void MainWindow::on_radioButton_allfriendly_clicked()
{
    CamPlayer::SetTracking(CamPlayer::FilterMode::AllFriendly);
    auto m2 = CamPlayer::ShowTracking();
    setUi(m2);
}

void MainWindow::on_pushButton_unfc_add_clicked()
{
    CamPlayer::AddUnfcsR m =CamPlayer::AddUnfcs();
    setUi(m);
    if(m.isok)
    {
        auto trackingdata = CamPlayer::ShowTracking();
        setUi(trackingdata);
    }
}

void MainWindow::on_pushButton_unfc_del_clicked()
{
    auto items = ui->listWidget_unfcs->selectedItems();
    auto item = items.first();
    auto txt = item->text();
    CamPlayer::DelUnfcsR m = CamPlayer::DelUnfcs(txt);
    setUi(m);
    if(m.isok)
    {
        auto trackingdata = CamPlayer::ShowTracking();
        setUi(trackingdata);
    }
}

void MainWindow::on_pushButton_dir_clicked(bool checked)
{
    direction = checked;
    setDirLabel();
}

void MainWindow::on_pushButton_savefcs_clicked()
{
    auto m = CamPlayer::SaveUnfcs();
    setUi(m);
}
/*
setUi
*/

void MainWindow::setUi(const CamPlayer::SettingsR& m)
{
    ui->label_examId->setText(m.folderName);
}

void MainWindow::setUi(const CamPlayer::LoadR& m)
{
    ui->label_examId->setText(m.folderName);
}

void MainWindow::setUi(const CamPlayer::SaveFcsR& m)
{
    ui->label_examId->setText(m.folderName);
}


void MainWindow::setUi(CamPlayer::AddUnfcsR m)
{
    if(m.isok){
        QString hexstr = m.fc.toHexString();
        ui->listWidget_unfcs->addItem(hexstr);
    }
}

void MainWindow::setUi(const CamPlayer::DelUnfcsR &m) // NOLINT(clazy-function-args-by-value)
{
    if(m.isok){
        QString hexstr = m.fc.toHexString();
        auto items = ui->listWidget_unfcs->findItems(hexstr, Qt::MatchExactly);
        foreach(QListWidgetItem * item, items)
        {
            delete ui->listWidget_unfcs->takeItem(ui->listWidget_unfcs->row(item));
        }
    }
}

void MainWindow::setUi(const CamPlayer::GetTrackingUnfcR &m)
{
    ui->listWidget_unfcs->insertItems(0, m.unfcshex);
    ui->label_unfcs_name->setText(m.name);
}

void MainWindow::setUi(const CamPlayer::ShowFrameR& m)
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

    auto m2 = CamPlayer::ShowTracking();
    setUi(m2);
}

void MainWindow::setUi(const CamPlayer::LoadFcsR& m)
{
    ui->label_fcs->setText(m.folderName+':'+QString::number(m.fcs_count)+','+QString::number(m.unfcs_count));
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
    if(!pixmap.isNull())
    {
        if(vix!=-1)
        {
            double rx = pixmap.width()/l->width();
            double ry = pixmap.height()/l->height();

            double x = p.x()*rx;
            double y = p.y()*ry;

            auto txt2 = QStringLiteral("%1,%2").arg(x).arg(y);
            txt+=' '+txt2;

            QColor color = CamPlayer::GetColor(vix, -1, x, y);
            auto color_str = CamPlayer::toString(color);
            auto color_hexstr = CamPlayer::toHexString(color);
            txt+= " ("+color_str+") "+color_hexstr;

            QString colorname = CamPlayer::GetColorName(color);
            if(!colorname.isEmpty()) txt+=' '+colorname;

            int ballIx = CamPlayer::GetBallIx(vix, -1, x, y, -1);
            int color_ix = CamPlayer::GetColorIx(color);

            auto trackingR = CamPlayer::SetTracking(vix, -1, ballIx , color_ix, x, y);
            QString tracking_txt = CamPlayer::ShowTrackingTxt();
            auto trackingdata = CamPlayer::ShowTracking();
            setUi(trackingdata);
            ui->label_tracking->setText(tracking_txt);
            if(trackingR.fcix_changed)
            {
                ui->listWidget_unfcs->clear();
                auto fcs = CamPlayer::GetTrackingUnfc();
                setUi(fcs);
            }
            auto r2 = CamPlayer::GotoFrame(-1);
            setUi(r2);

        }
        else{
            int zix = GetZoomLabelIndex(w);
            if(zix!=-1){
                QColor c = CamPlayer::GetTrackingColor(p, l->size());
                QString txt4 = CamPlayer::toHexString(c);
                ui->label_filterclick->setText(txt4);
                CamPlayer::SetTrackingColor(c);
                auto tcolor = CamPlayer::GetTrackingColor();
                setUi(tcolor);
            }
        }
    }

    ui->label_msg->setText(txt);
}

void MainWindow::setUi(const CamPlayer::ShowTrackingR& m){
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

void MainWindow::setUi(const CamPlayer::TrackingColor& m){
    //int a = m.color.
    QPixmap p(1,1);
    p.fill(m.color);
    ui->label_color->setPixmap(p);
    p.fill(QColor(m.fc.r, m.fc.g, m.fc.b));
    ui->label_unfc->setPixmap(p);
}

void MainWindow::setUi(CamPlayer::FilterMode mode){
    switch(mode){
    case CamPlayer::FilterMode::Copy:ui->radioButton_copy->setChecked(true);break;
    case CamPlayer::FilterMode::IsFriendly:ui->radioButton_isfriendly->setChecked(true);break;
    case CamPlayer::FilterMode::AllFriendly:ui->radioButton_allfriendly->setChecked(true);break;
    }
}





