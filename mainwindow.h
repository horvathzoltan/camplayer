#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QList>
#include <QListWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include "camplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    struct ZoomBitmapData{
        QPixmap pixmap;
        QSize size;
        bool isValid=false;
    };

    struct VideoBitmapData{
    private:
        QLabel *_label;
    public:
        QImage image;
        const CamPlayer::FrameData* framedata;
        CamPlayer::FilterMode filterMode = CamPlayer::FilterMode::Copy;

        void setLabel(QLabel*l){_label=l;}
        QLabel* label(){return _label;}
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void on_click_signaler(QWidget *, QMouseEvent *event);
    void on_move_signaler(QWidget *, QMouseEvent *event);
    void on_MouseMove(QWidget *w, QMouseEvent *event);
    void on_MouseLeave(QWidget *w, QMouseEvent *event);
    void on_MouseEnter(QWidget *w, QMouseEvent *event);
    void on_MouseButtonPress(QWidget *w, QMouseEvent *event);
    void on_timeout();

    int picLabelIndex(QWidget *w);
    int zoomLabelIndex(QWidget *w);

    void setUi_ShowFrameR(const CamPlayer::ShowFrameR &m);
    void setUi_LoadFcsR(const CamPlayer::LoadFcsR &m);
    void setUi_ShowTrackingR(const CamPlayer::ShowTrackingR &m);
    void setUi_TrackingColor(const CamPlayer::TrackingColor& m);
    //void setui(const CamPlayer);
    void setUi_SettingsR(const CamPlayer::SettingsR&);
    void setUi_LoadR(const CamPlayer::LoadR&);
    void setUi_AddUnfcsR(CamPlayer::AddUnfcsR );
    void setUi_DelUnfcsR(const CamPlayer::DelUnfcsR &);
    void setUi_GetTrackingUnfcR(const CamPlayer::GetTrackingUnfcR&);
    void setUi_SaveFcsR(const CamPlayer::SaveFcsR&);
    void setUi_AddFcsExtraR(const CamPlayer::AddFcsExtraR &m);
    void setUi_DelFcsExtraR(const CamPlayer::DelFcsExtraR &m);
    void setUi_GetTrackingFcsExtraR(const CamPlayer::TrackingFcsExtraR &m);    
    void setUi_UnfcsDeleteItems(const QList<QListWidgetItem*>& items);
    void setUi_FcsExtraDeleteItems(const QList<QListWidgetItem*>& items);
    void setUi_picLabel(const QPixmap& pixmap, QLabel *label);
    QPixmap DrawMarker_ZoomBitmapData(const ZoomBitmapData& z, const QPoint& p);
    void ButtonEnable(QPushButton *b, bool s);
    void RefreshDirLabel();
    void RefreshPlayLabel();
    void RefreshUnfcs();
    void RefreshZoom();

    QPixmap DrawFrame(const CamPlayer::FrameData* framedata, CamPlayer::FilterMode filterMode);
    void setUi_FilterMode(CamPlayer::FilterMode mode);
    void setUi_FilterMode1(CamPlayer::FilterMode mode);
    void setUi_FilterMode2(CamPlayer::FilterMode mode);
    void setUi_FilterMode3(CamPlayer::FilterMode mode);
    void setUi_FilterMode4(CamPlayer::FilterMode mode);
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

    void on_radioButton_copy_1_clicked();

    void on_radioButton_allfriendly_1_clicked();

    void on_radioButton_isfriendly_1_clicked();

    void on_radioButton_copy_2_clicked();

    void on_radioButton_allfriendly_2_clicked();

    void on_radioButton_isfriendly_2_clicked();

    void on_radioButton_copy_3_clicked();

    void on_radioButton_allfriendly_3_clicked();

    void on_radioButton_isfriendly_3_clicked();

    void on_radioButton_copy_4_clicked();

    void on_radioButton_allfriendly_4_clicked();

    void on_radioButton_isfriendly_4_clicked();

private:
    int _timer_step=16;
    Ui::MainWindow *ui;
    QTimer _timer;

    ZoomBitmapData _original_zoom;
    ZoomBitmapData _original_filtered;

    VideoBitmapData _frame_1;
    VideoBitmapData _frame_2;
    VideoBitmapData _frame_3;
    VideoBitmapData _frame_4;

    bool _isRestartOnNextTimeout;
    bool _direction = true;

    void setUi_ShowFrame(const CamPlayer::FrameData* framedata, CamPlayer::FilterMode filterMode, QLabel *label);
    void setUi_FilterMode2(CamPlayer::FilterMode mode, QRadioButton *c, QRadioButton *f, QRadioButton *a);

    void setFilterMode(VideoBitmapData *frame, CamPlayer::FilterMode mode);
};
#endif // MAINWINDOW_H
