#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

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

    void setUi(const CamPlayer::ShowFrameR &m);
    void on_click(QWidget *, QMouseEvent *event);
    int GetPicLabelIndex(QWidget *w);
    int GetZoomLabelIndex(QWidget *w);
    void setUi(const CamPlayer::LoadFcsR &m);
    void onMouseButtonPress(QWidget *w, QMouseEvent *event);
    void setUi(const CamPlayer::ShowTrackingR &m);
    void setUi(CamPlayer::FilterMode mode);
    void setUi(const CamPlayer::TrackingColor& m);
    void setui(const CamPlayer);
    void setUi(const CamPlayer::SettingsR&);
    void setUi(const CamPlayer::LoadR&);
    void setUi(const CamPlayer::AddUnfcsR&);
    void setUi(const CamPlayer::DelUnfcsR &);

    void setUi(const CamPlayer::GetTrackingUnfcR&);
    void setUi(const CamPlayer::SaveFcsR&);

    void setDirLabel();
private slots:
    void on_pushButton_load_clicked();
    void on_pushButton_next_clicked();
    void on_pushButton_prev_clicked();
    void on_pushButton_start_clicked();
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

private:
    Ui::MainWindow *ui;
    QTimer timer;
    bool direction = true;
    void on_timeout();
};
#endif // MAINWINDOW_H
