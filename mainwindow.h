#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QList>
#include <QListWidgetItem>
#include <QPushButton>
#include "camplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setUi_TrackingColor(const CamPlayer::ShowFrameR &m);
    void on_click(QWidget *, QMouseEvent *event);
    void on_move(QWidget *, QMouseEvent *event);
    int GetPicLabelIndex(QWidget *w);
    int GetZoomLabelIndex(QWidget *w);
    void setUi_TrackingColor(const CamPlayer::LoadFcsR &m);
    void onMouseButtonPress(QWidget *w, QMouseEvent *event);
    void setUi_ShowTrackingR(const CamPlayer::ShowTrackingR &m);
    void setUi_TrackingColor(CamPlayer::FilterMode mode);
    void setUi_TrackingColor(const CamPlayer::TrackingColor& m);
    void setui(const CamPlayer);
    void setUi_TrackingColor(const CamPlayer::SettingsR&);
    void setUi_TrackingColor(const CamPlayer::LoadR&);
    void setUi_TrackingColor(CamPlayer::AddUnfcsR );
    void setUi_TrackingColor(const CamPlayer::DelUnfcsR &);

    void setUi_GetTrackingUnfcR(const CamPlayer::GetTrackingUnfcR&);
    void setUi_TrackingColor(const CamPlayer::SaveFcsR&);

    void RefreshDirLabel();
    void RefreshUnfcs();
    void RefreshZoom();
    void listWidget_unfcs_delete_items(const QList<QListWidgetItem*>& items);
    void listWidget_fcs_extra_delete_items(const QList<QListWidgetItem*>& items);
    void setUi_TrackingColor(const CamPlayer::AddFcsExtraR &m);
    void setUi_TrackingColor(const CamPlayer::DelFcsExtraR &m);
    void ButtonEnable(QPushButton *b, bool s);
    void setUi_GetTrackingFcsExtraR(const CamPlayer::TrackingFcsExtraR &m);
    void onMouseMove(QWidget *w, QMouseEvent *event);
    void onMouseLeave(QWidget *w, QMouseEvent *event);
    void onMouseEnter(QWidget *w, QMouseEvent *event);
    void DrawMarker(QPixmap* pxm, const QPoint &p, const QSize& tsize);
private slots:
    void on_pushButton_load_clicked();
    void on_pushButton_next_clicked();
    void on_pushButton_prev_clicked();
    //void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void on_pushButton_rew_clicked();
    void on_pushButton_loadfcs_clicked();
    void on_radioButton_isfriendly_clicked();
    void on_radioButton_allfriendly_clicked();
    void on_radioButton_copy_clicked();
    void on_pushButton_unfc_add_clicked();
    void on_pushButton_dir_clicked(bool checked);
    void on_pushButton_savefcs_clicked();

    void on_pushButton_unfc_del_clicked();

    void on_spinBox_timer_valueChanged(int arg1);

    void on_pushButton_start_clicked(bool checked);

    void on_pushButton_fcs_extra_add_clicked();

    void on_pushButton_fcs_extra_del_clicked();

    void on_listWidget_fcs_extra_itemClicked(QListWidgetItem *item);

    void on_listWidget_unfcs_itemClicked(QListWidgetItem *item);


    void on_listWidget_col_names_currentRowChanged(int currentRow);

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

private:
    int timer_step=16;
    Ui::MainWindow *ui;
    QTimer timer;

    struct ZoomBitmapData{
        QPixmap pixmap;
        QSize size;
        bool isValid=false;

//        void DrawMarker(){
//            z.isValid
//        }
    };

    ZoomBitmapData _original_zoom;
    ZoomBitmapData _original_filtered;
    bool isRestartOnNext_on_timeout;
    bool direction = true;
    void on_timeout();
};
#endif // MAINWINDOW_H
