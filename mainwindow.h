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
#include <QCheckBox>

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

    struct VideoData{
        VideoData(){
            for(int i=0;i<CamPlayer::_fcs_length;i++) ignoretab[i]=nullptr;
        }
        ~VideoData(){
            for(int i=0;i<CamPlayer::_fcs_length;i++) delete ignoretab[i];
        };
    private:
        QLabel *_label=nullptr;
        QLabel *_label_pix=nullptr;
        QRadioButton *_t1 = nullptr;
    public:
        const CamPlayer::FrameData* framedata = nullptr;
        CamPlayer::FilterMode filterMode = CamPlayer::FilterMode::Copy;

        int total_pixel_counter;
        int filtered_pixel_counter;

        QLabel* label()const{return _label;}
        QLabel* label_pix()const{return _label_pix;}
        bool isT1() const {
            if(!_t1) return false;
            return _t1->isChecked();
        }
        void init(QLabel*l, QLabel* lpx, QRadioButton*r){
            _label=l;
            _t1=r;
            _label_pix=lpx;
        };

        void initIgnoreTab(QSize s)
        {
            for(int i=0;i<CamPlayer::_fcs_length;i++)
                ignoretab[i] = new CamPlayer::FilterStatR(s);
        }
        CamPlayer::FilterStatR* ignoretab[CamPlayer::_fcs_length];
    };

    //CamPlayer::FilterStatR _ignoretab_1[CamPlayer::_fcs_length];
    //CamPlayer::FilterStatR _ignoretab_2[CamPlayer::_fcs_length];
    //CamPlayer::FilterStatR _ignoretab_3[CamPlayer::_fcs_length];
    //CamPlayer::FilterStatR _ignoretab_4[CamPlayer::_fcs_length];
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
    void RefreshFrames();
    void RefreshFrame(int i);

    QPixmap DrawFrame(VideoData* v);
//        const CamPlayer::FrameData* framedata
//                      ,CamPlayer::FilterMode filterMode
//                      ,int* total_pixel_counter
//                      ,int* filtered_pixel_counter);
    void setUi_FilterMode(CamPlayer::FilterMode mode);
    void setUi_FilterMode1(CamPlayer::FilterMode mode);
    void setUi_FilterMode2(CamPlayer::FilterMode mode);
    void setUi_FilterMode3(CamPlayer::FilterMode mode);
    void setUi_FilterMode4(CamPlayer::FilterMode mode);
    void setUi_VideoBitmapData(VideoData* v);
    static void CopyIconMirrored(QAbstractButton *dst, QAbstractButton *src);
    void setUi_ListWidgetFcNames(int n);
    QLabel* picLabel(int i);
    static QString NumToString(int i);
    static void setItemData(QListWidgetItem *item, const CamPlayer::TrackingData::IgnoreData &data);
    static CamPlayer::TrackingData::IgnoreData itemData(QListWidgetItem *item);
    MainWindow::VideoData *videoData(int i);
private slots:
    void on_pushButton_load_clicked();
    void on_pushButton_next_clicked();
    void on_pushButton_prev_clicked();    
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
    void on_pushButton_play_clicked(bool checked);
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

    void on_pushButton_4_clicked();

    void on_pushButton_ffwd_clicked();

    void on_pushButton_ign_add_clicked();

    void on_pushButton_ign_del_clicked();

    void on_pushButton_save_ignore_clicked();

    void on_pushButton_load_ignore_clicked();

private:
    int _timer_step=16;
    Ui::MainWindow *ui;
    QTimer _timer;

    ZoomBitmapData _original_zoom;
    ZoomBitmapData _original_filtered;

    VideoData _videodata_1;
    VideoData _videodata_2;
    VideoData _videodata_3;
    VideoData _videodata_4;

    //bool _isRestartOnNextTimeout;
    bool _direction = true;

    bool _hasNext=false;
    bool _hasPrev=false;

    //void setUi_ShowFrame(const CamPlayer::FrameData* framedata, CamPlayer::FilterMode filterMode, QLabel *label);
    void setUi_FilterMode2(CamPlayer::FilterMode mode,
                           QRadioButton *c,
                           QRadioButton *f,
                           QRadioButton *a);

    void setFilterMode(VideoData *frame, CamPlayer::FilterMode mode);


    struct IgnoreListKeys{
        static const int ix = Qt::UserRole;
        static const int vix = Qt::UserRole+1;
        static const int fcix = Qt::UserRole+2;
        static const int x = Qt::UserRole+3;
        static const int y = Qt::UserRole+4;
    };
};
#endif // MAINWINDOW_H
