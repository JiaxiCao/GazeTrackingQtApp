#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GazeTrackingQtApp.h"

#include <QtWidgets/QMainWindow>
#include <QString>
#include <QTimer>
#include <QtWidgets>

#include <opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <conio.h>
#include "windows.h"
#include "dshow.h"
#include <io.h>
#include <ctime>
#include <sys/timeb.h>

using namespace std;
using namespace cv;

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")

class GazeTrackingQtApp : public QMainWindow
{
    Q_OBJECT

public:
    GazeTrackingQtApp(QWidget *parent = Q_NULLPTR);
    ~GazeTrackingQtApp();

private:
    Ui::GazeTrackingQtAppClass ui;

    void CreateWindows();

    QDockWidget* dock_scene_img, * dock_eye1_img, * dock_eye2_img, * dock_eye3_img, * dock_eye4_img;
    QLabel* scene_imgLabel, * eye1_imgLabel, * eye2_imgLabel, * eye3_imgLabel, * eye4_imgLabel;
    QScrollArea* scene_scrollArea, * eye1_scrollArea, * eye2_scrollArea, * eye3_scrollArea, * eye4_scrollArea;
    void Init_scene_img_dock();
    void Init_eye_img_dock();
    QDockWidget* dock_button;
    QLabel* cam_choice1, * cam_choice2, * cam_choice3, * cam_choice4, * cam_choice5;
    QComboBox* combobox1, * combobox2, * combobox3, * combobox4, * combobox5;
    QPushButton* open_cam, * close_cam, * vid_capture, * gaze_track, * save_gaze;
    QLabel* folder_path_label, * eye_width_label, * eye_height_label;
    QLineEdit* folder_path, * eye_width_input, * eye_height_input;
    void Init_button_dock();

    volatile bool close_all_thread_;

    std::vector<std::string> camlist;
    int List_Devices(vector<string>& list);

    bool Init();
    bool Camera_Init();
    void Show_img(Mat img_eye1, Mat img_eye2, Mat img_eye3, Mat img_eye4, Mat img_scene);
    QImage Mat2QImage(const Mat& mat);
    Mat eye1_img, eye2_img, eye3_img, eye4_img, scene_img;
    VideoCapture eye1, eye2, eye3, eye4, scene;
    int eye_img_width = 0, eye_img_height = 0, scene_img_width = 0, scene_img_height = 0;
    std::mutex image_read_mutex;
    std::condition_variable image_read_condition;
    volatile bool image_ready;
    QTimer* show_img_timer;

    void Release();

    VideoWriter eye1_writer, eye2_writer, eye3_writer, eye4_writer, scene_writer;
    void SaveVideotoFile();
    std::mutex image_write_mutex;
    std::condition_variable image_write_condition;
    volatile bool start_video_capture;
    ofstream timestamp_file;
    struct timeb timestamp;

private slots:
    void open_system();
    void close_system();
    void save_video();
    void stop_video_cap();

    void Image_Show();
};
