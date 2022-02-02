#include "GazeTrackingQtApp.h"

GazeTrackingQtApp::GazeTrackingQtApp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	/*delete main window*/
	QWidget* p = takeCentralWidget();
	if (p)
		delete p;
	setDockNestingEnabled(true);
	setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);

	CreateWindows();
}
GazeTrackingQtApp::~GazeTrackingQtApp() {
	Release();
}

void GazeTrackingQtApp::CreateWindows() {
	Init_scene_img_dock();
	Init_eye_img_dock();
	Init_button_dock();

	addDockWidget(Qt::LeftDockWidgetArea, dock_eye1_img);
	splitDockWidget(dock_eye1_img, dock_eye2_img, Qt::Horizontal);
	splitDockWidget(dock_eye1_img, dock_eye3_img, Qt::Vertical);
	splitDockWidget(dock_eye2_img, dock_eye4_img, Qt::Vertical);
	addDockWidget(Qt::RightDockWidgetArea, dock_scene_img);
	splitDockWidget(dock_scene_img, dock_button, Qt::Vertical);

	List_Devices(camlist);
	combobox1->clear();
	combobox2->clear();
	combobox3->clear();
	combobox4->clear();
	combobox5->clear();
	QString empty_str = QString::fromStdString("");
	combobox1->addItem(empty_str);
	combobox2->addItem(empty_str);
	combobox3->addItem(empty_str);
	combobox4->addItem(empty_str);
	combobox5->addItem(empty_str);
	for (string cam_name : camlist) {
		QString cam_name_q = QString::fromStdString(cam_name);
		combobox1->addItem(cam_name_q);
		combobox2->addItem(cam_name_q);
		combobox3->addItem(cam_name_q);
		combobox4->addItem(cam_name_q);
		combobox5->addItem(cam_name_q);
	}
	for (int i = 0; i < camlist.size(); ++i) {
		if (camlist[i] == "5M Cam") combobox5->setCurrentIndex(i+1);
	}
}
int GazeTrackingQtApp::List_Devices(vector<string>& list)
{
	ICreateDevEnum* pDevEnum = NULL;
	IEnumMoniker* pEnum = NULL;
	int deviceCounter = 0;
	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
		reinterpret_cast<void**>(&pDevEnum));


	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator(
			CLSID_VideoInputDeviceCategory,
			&pEnum, 0);

		if (hr == S_OK) {

			IMoniker* pMoniker = NULL;

			while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {

				IPropertyBag* pPropBag;
				hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
					(void**)(&pPropBag));

				if (FAILED(hr)) {
					pMoniker->Release();
					continue;  // Skip this one, maybe the next one will work.
				}


				// Find the description or friendly name.
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"Description", &varName, 0);

				if (FAILED(hr)) hr = pPropBag->Read(L"FriendlyName", &varName, 0);

				if (SUCCEEDED(hr))
				{

					hr = pPropBag->Read(L"FriendlyName", &varName, 0);

					int count = 0;
					char tmp[255] = { 0 };
					//int maxLen = sizeof(deviceNames[0]) / sizeof(deviceNames[0][0]) - 2;
					while (varName.bstrVal[count] != 0x00 && count < 255)
					{
						tmp[count] = (char)varName.bstrVal[count];
						count++;
					}
					list.push_back(tmp);
					//deviceNames[deviceCounter][count] = 0;

					//if (!silent) DebugPrintOut("SETUP: %i) %s\n", deviceCounter, deviceNames[deviceCounter]);
				}

				pPropBag->Release();
				pPropBag = NULL;

				pMoniker->Release();
				pMoniker = NULL;

				deviceCounter++;
			}

			pDevEnum->Release();
			pDevEnum = NULL;

			pEnum->Release();
			pEnum = NULL;
		}

		//if (!silent) DebugPrintOut("SETUP: %i Device(s) found\n\n", deviceCounter);
	}

	//comUnInit();

	return deviceCounter;
}

void GazeTrackingQtApp::Init_scene_img_dock()
{
	dock_scene_img = new QDockWidget(tr("场景图像"), this);
	dock_scene_img->setFeatures(QDockWidget::NoDockWidgetFeatures);
	dock_scene_img->setStyleSheet("QDockWidget:title{font-family:Microsoft YaHei;text-align: center}");
	dock_scene_img->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea);
	dock_scene_img->setFixedSize(640, 360);

	scene_imgLabel = new QLabel(dock_scene_img);
	scene_imgLabel->setScaledContents(true);
	QImage img = QImage(640, 360, QImage::Format_RGB32);
	img.fill(qRgb(255, 255, 255));
	scene_imgLabel->setPixmap(QPixmap::fromImage(img));
	scene_imgLabel->resize(img.width(), img.height());

	scene_scrollArea = new QScrollArea(this);
	scene_scrollArea->setBackgroundRole(QPalette::Dark);
	scene_scrollArea->setAlignment(Qt::AlignCenter);
	scene_scrollArea->setWidget(scene_imgLabel);
	dock_scene_img->setWidget(scene_scrollArea);
}
void GazeTrackingQtApp::Init_eye_img_dock()
{
	// init eye image dock widget
	dock_eye1_img = new QDockWidget(tr("眼部图像 eye1"), this);
	dock_eye1_img->setFeatures(QDockWidget::NoDockWidgetFeatures);
	dock_eye1_img->setStyleSheet("QDockWidget:title{font-family:Microsoft YaHei;text-align: center}");
	dock_eye1_img->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea);
	dock_eye1_img->setFixedSize(360, 288);
	// init image display area
	eye1_imgLabel = new QLabel(dock_eye1_img);
	eye1_imgLabel->setScaledContents(true);
	QImage img = QImage(360, 288, QImage::Format_RGB32);
	img.fill(qRgb(255, 255, 255));
	eye1_imgLabel->setPixmap(QPixmap::fromImage(img));
	eye1_imgLabel->resize(img.width(), img.height());
	// init scroll area
	eye1_scrollArea = new QScrollArea(this);
	eye1_scrollArea->setBackgroundRole(QPalette::Dark);
	eye1_scrollArea->setAlignment(Qt::AlignCenter);
	eye1_scrollArea->setWidget(eye1_imgLabel);
	dock_eye1_img->setWidget(eye1_scrollArea);

	dock_eye2_img = new QDockWidget(tr("眼部图像 eye2"), this);
	dock_eye2_img->setFeatures(QDockWidget::NoDockWidgetFeatures);
	dock_eye2_img->setStyleSheet("QDockWidget:title{font-family:Microsoft YaHei;text-align: center}");
	dock_eye2_img->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea);
	dock_eye2_img->setFixedSize(360, 288);
	eye2_imgLabel = new QLabel(dock_eye2_img);
	eye2_imgLabel->setScaledContents(true);
	eye2_imgLabel->setPixmap(QPixmap::fromImage(img));
	eye2_imgLabel->resize(img.width(), img.height());
	eye2_scrollArea = new QScrollArea(this);
	eye2_scrollArea->setBackgroundRole(QPalette::Dark);
	eye2_scrollArea->setAlignment(Qt::AlignCenter);
	eye2_scrollArea->setWidget(eye2_imgLabel);
	dock_eye2_img->setWidget(eye2_scrollArea);

	dock_eye3_img = new QDockWidget(tr("眼部图像 eye2"), this);
	dock_eye3_img->setFeatures(QDockWidget::NoDockWidgetFeatures);
	dock_eye3_img->setStyleSheet("QDockWidget:title{font-family:Microsoft YaHei;text-align: center}");
	dock_eye3_img->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea);
	dock_eye3_img->setFixedSize(360, 288);
	eye3_imgLabel = new QLabel(dock_eye3_img);
	eye3_imgLabel->setScaledContents(true);
	eye3_imgLabel->setPixmap(QPixmap::fromImage(img));
	eye3_imgLabel->resize(img.width(), img.height());
	eye3_scrollArea = new QScrollArea(this);
	eye3_scrollArea->setBackgroundRole(QPalette::Dark);
	eye3_scrollArea->setAlignment(Qt::AlignCenter);
	eye3_scrollArea->setWidget(eye3_imgLabel);
	dock_eye3_img->setWidget(eye3_scrollArea);

	dock_eye4_img = new QDockWidget(tr("眼部图像 eye4"), this);
	dock_eye4_img->setFeatures(QDockWidget::NoDockWidgetFeatures);
	dock_eye4_img->setStyleSheet("QDockWidget:title{font-family:Microsoft YaHei;text-align: center}");
	dock_eye4_img->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea);
	dock_eye4_img->setFixedSize(360, 288);
	eye4_imgLabel = new QLabel(dock_eye4_img);
	eye4_imgLabel->setScaledContents(true);
	eye4_imgLabel->setPixmap(QPixmap::fromImage(img));
	eye4_imgLabel->resize(img.width(), img.height());
	eye4_scrollArea = new QScrollArea(this);
	eye4_scrollArea->setBackgroundRole(QPalette::Dark);
	eye4_scrollArea->setAlignment(Qt::AlignCenter);
	eye4_scrollArea->setWidget(eye4_imgLabel);
	dock_eye4_img->setWidget(eye4_scrollArea);
}
void GazeTrackingQtApp::Init_button_dock()
{
	dock_button = new QDockWidget(tr("功能按键"), this);
	dock_button->setFeatures(QDockWidget::NoDockWidgetFeatures);
	dock_button->setStyleSheet("QDockWidget:title{font-family:Microsoft YaHei;text-align: center}");
	dock_button->setFixedWidth(660);
	dock_button->setFixedHeight(250);

	
	cam_choice1 = new QLabel(tr("EyeCam 1"), dock_button);
	cam_choice2 = new QLabel(tr("EyeCam 2"), dock_button);
	cam_choice3 = new QLabel(tr("EyeCam 3"), dock_button);
	cam_choice4 = new QLabel(tr("EyeCam 4"), dock_button);
	cam_choice5 = new QLabel(tr("SceneCam"), dock_button);
	cam_choice1->setFixedSize(70, 20);
	cam_choice2->setFixedSize(70, 20);
	cam_choice3->setFixedSize(70, 20);
	cam_choice4->setFixedSize(70, 20);
	cam_choice5->setFixedSize(70, 20);

	QVBoxLayout* vlayout1 = new QVBoxLayout();
	vlayout1->addWidget(cam_choice1);
	vlayout1->addWidget(cam_choice2);
	vlayout1->addWidget(cam_choice3);
	vlayout1->addWidget(cam_choice4);
	vlayout1->addWidget(cam_choice5);

	combobox1 = new QComboBox();
	combobox2 = new QComboBox();
	combobox3 = new QComboBox();
	combobox4 = new QComboBox();
	combobox5 = new QComboBox();
	combobox1->setFixedSize(300, 20);
	combobox2->setFixedSize(300, 20);
	combobox3->setFixedSize(300, 20);
	combobox4->setFixedSize(300, 20);
	combobox5->setFixedSize(300, 20);
	QVBoxLayout* vlayout2 = new QVBoxLayout();
	vlayout2->addWidget(combobox1);
	vlayout2->addWidget(combobox2);
	vlayout2->addWidget(combobox3);
	vlayout2->addWidget(combobox4);
	vlayout2->addWidget(combobox5);

	open_cam = new QPushButton(tr("打开相机"), dock_button);
	close_cam = new QPushButton(tr("关闭相机"), dock_button);
	vid_capture = new QPushButton(tr("采集视频"), dock_button);
	gaze_track = new QPushButton(tr("实时视线跟踪"), dock_button);
	save_gaze = new QPushButton(tr("保存视线结果"), dock_button);
	open_cam->adjustSize();
	close_cam->adjustSize();
	vid_capture->adjustSize();
	gaze_track->adjustSize();
	save_gaze->adjustSize();
	QVBoxLayout* vlayout3 = new QVBoxLayout();
	vlayout3->addWidget(open_cam);
	vlayout3->addWidget(close_cam);
	vlayout3->addWidget(vid_capture);
	vlayout3->addWidget(gaze_track);
	vlayout3->addWidget(save_gaze);
	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->addItem(vlayout1);
	hlayout->addItem(vlayout2);
	hlayout->addItem(vlayout3);


	folder_path_label = new QLabel(tr("视频保存路径"), dock_button);
	eye_width_label = new QLabel(tr("眼部图像宽度"), dock_button);
	eye_height_label = new QLabel(tr("眼部图像高度"), dock_button);
	QVBoxLayout* vlayout4 = new QVBoxLayout();
	vlayout4->addWidget(folder_path_label);
	vlayout4->addWidget(eye_width_label);
	vlayout4->addWidget(eye_height_label);
	folder_path = new QLineEdit();
	eye_width_input = new QLineEdit();
	eye_height_input = new QLineEdit();
	folder_path->setText(tr("D:\\test_folder\\"));
	eye_width_input->setText(tr("640"));
	eye_height_input->setText(tr("480"));
	QVBoxLayout* vlayout5 = new QVBoxLayout();
	vlayout5->addWidget(folder_path);
	vlayout5->addWidget(eye_width_input);
	vlayout5->addWidget(eye_height_input);
	QHBoxLayout* hlayout2 = new QHBoxLayout();
	hlayout2->addItem(vlayout4);
	hlayout2->addItem(vlayout5);

	QVBoxLayout* vlayout6 = new QVBoxLayout();
	vlayout6->addItem(hlayout);
	vlayout6->addItem(hlayout2);

	QWidget* docklayout = new QWidget(dock_button);
	docklayout->setLayout(vlayout6);
	dock_button->setWidget(docklayout);

	/* connect signal and slot function */
	connect(open_cam, SIGNAL(clicked()), this, SLOT(open_system()));
	connect(close_cam, SIGNAL(clicked()), this, SLOT(close_system()));
	connect(vid_capture, SIGNAL(clicked()), this, SLOT(save_video()));
}

void GazeTrackingQtApp::open_system() {
	if (!Init())
	{
		return;
	}
	else
	{
		/* for image show */
		show_img_timer = new QTimer(this);
		connect(show_img_timer, SIGNAL(timeout()), this, SLOT(Image_Show()));
		show_img_timer->start(50);
		open_cam->setEnabled(false);
		close_cam->setEnabled(true);
	}
}
bool GazeTrackingQtApp::Init()
{
	eye1_img = cv::Mat();
	eye2_img = cv::Mat();
	eye3_img = cv::Mat();
	eye4_img = cv::Mat();
	scene_img = cv::Mat();

	close_all_thread_ = false;
	image_ready = false;
	start_video_capture = false;

	if (!Camera_Init())
	{
		return false;
	}
	else
		return true;

}
bool GazeTrackingQtApp::Camera_Init()
{
	/*Use Direct Show mode, not use MSMF mode, because default MSMF mode can't use correct camera setting.*/
	/*Notific: please use two usb interface, not use a usb hub*/
	/*cameras id need 3rd-part program check it*/
	if (camlist.size() == 0) {
		QMessageBox::information(this, tr("错误"), tr("未连接摄像机"));
		return false;
	}
	vector<int> camid_cnt(camlist.size() + 1, 0);
	camid_cnt[combobox1->currentIndex()]++;
	camid_cnt[combobox2->currentIndex()]++;
	camid_cnt[combobox3->currentIndex()]++;
	camid_cnt[combobox4->currentIndex()]++;
	camid_cnt[combobox5->currentIndex()]++;
	for (int i = 1; i < camid_cnt.size(); ++i) {
		if (camid_cnt[i] > 1) {
			QMessageBox::information(this, tr("错误"), tr("摄像机选择重复"));
			return false;
		}
	}

	if (combobox1->currentIndex() == 0) {
		if (eye1.isOpened()) eye1.release();
	}
	else eye1.open(combobox1->currentIndex() - 1 + cv::CAP_DSHOW);
	if (combobox2->currentIndex() == 0) {
		if (eye2.isOpened()) eye2.release();
	}
	else eye2.open(combobox2->currentIndex() - 1 + cv::CAP_DSHOW);
	if (combobox3->currentIndex() == 0) {
		if (eye3.isOpened()) eye3.release();
	}
	else eye3.open(combobox3->currentIndex() - 1 + cv::CAP_DSHOW);
	if (combobox4->currentIndex() == 0) {
		if (eye4.isOpened()) eye4.release();
	}
	else eye4.open(combobox4->currentIndex() - 1 + cv::CAP_DSHOW);
	if (combobox5->currentIndex() == 0) {
		if (scene.isOpened()) scene.release();
	}
	else scene.open(combobox5->currentIndex() - 1 + cv::CAP_DSHOW);
	cv::waitKey(100);

	/*set camera properities*/
	if (scene.isOpened()) {
		scene.set(cv::CAP_PROP_AUTOFOCUS, 0);							/*Disable auto focus*/
		scene.set(cv::CAP_PROP_FOCUS, 3);								/*Set infinity focus*/
		scene.set(cv::CAP_PROP_FRAME_WIDTH, 1920);						/*Set frame size*/
		scene.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
		scene.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));	/*Default YUV2 codecs fps is 10, MJPEG codecs fps is 50+*/
		scene_img_width = scene.get(cv::CAP_PROP_FRAME_WIDTH);
		scene_img_height = scene.get(cv::CAP_PROP_FRAME_HEIGHT);
	}
	if (eye1.isOpened()) {
		eye1.set(cv::CAP_PROP_FRAME_WIDTH, 640);	
		eye1.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
		eye1.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));
		eye_img_width = eye1.get(cv::CAP_PROP_FRAME_WIDTH);
		eye_img_height = eye1.get(cv::CAP_PROP_FRAME_HEIGHT);
	}
	if (eye2.isOpened()) {
		eye2.set(cv::CAP_PROP_FRAME_WIDTH, 640);	
		eye2.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
		eye2.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));	
		eye_img_width = eye2.get(cv::CAP_PROP_FRAME_WIDTH);
		eye_img_height = eye2.get(cv::CAP_PROP_FRAME_HEIGHT);
	}
	if (eye3.isOpened()) {
		eye3.set(cv::CAP_PROP_FRAME_WIDTH, 640);	
		eye3.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
		eye3.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));	
		eye_img_width = eye3.get(cv::CAP_PROP_FRAME_WIDTH);
		eye_img_height = eye3.get(cv::CAP_PROP_FRAME_HEIGHT);
	}
	if (eye4.isOpened()) {
		eye4.set(cv::CAP_PROP_FRAME_WIDTH, 640);
		eye4.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
		eye4.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));
		eye_img_width = eye4.get(cv::CAP_PROP_FRAME_WIDTH);
		eye_img_height = eye4.get(cv::CAP_PROP_FRAME_HEIGHT);
	}

	return true;
}
void GazeTrackingQtApp::Image_Show()
{
	/* read image safety */
	std::unique_lock<std::mutex> lock_(image_read_mutex);
	if (eye1.isOpened()) eye1 >> eye1_img;
	if (eye2.isOpened()) eye2 >> eye2_img;
	if (eye3.isOpened()) eye3 >> eye3_img;
	if (eye4.isOpened()) eye4 >> eye4_img;
	if (scene.isOpened()) scene >> scene_img;

	Mat eye1_show, eye2_show, eye3_show, eye4_show, scene_show;
	if (!eye1_img.empty()) eye1_show = eye1_img.clone();
	if (!eye2_img.empty()) eye2_show = eye2_img.clone();
	if (!eye3_img.empty()) eye3_show = eye3_img.clone();
	if (!eye4_img.empty()) eye4_show = eye4_img.clone();
	if (!scene_img.empty()) scene_show = scene_img.clone();

	image_ready = true;
	lock_.unlock();
	image_read_condition.notify_all();

	Show_img(eye1_show, eye2_show, eye3_show, eye4_show, scene_show);
	
}
void GazeTrackingQtApp::Show_img(Mat img_eye1, Mat img_eye2, Mat img_eye3, Mat img_eye4, Mat img_scene)
{
	/*image size fit dock size*/
	if (!img_eye1.empty()) {
		QImage img_show;
		img_show = Mat2QImage(img_eye1);
		double imgratio = 1. * img_show.width() / img_show.height();
		double winratio = 1. * (eye1_scrollArea->width() - 2) / (eye1_scrollArea->height() - 2);
		if (imgratio > winratio)
			img_show = img_show.scaled((eye1_scrollArea->width() - 2),
				(eye1_scrollArea->width() - 2) / imgratio,
				Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		else
			img_show = img_show.scaled((eye1_scrollArea->height() - 2) * imgratio,
				(eye1_scrollArea->height() - 2),
				Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		eye1_imgLabel->setPixmap(QPixmap::fromImage(img_show));
		eye1_imgLabel->resize(img_show.width(), img_show.height());
	}
	if (!img_eye2.empty()) {
		QImage img_show;
		img_show = Mat2QImage(img_eye2);
		double imgratio = 1. * img_show.width() / img_show.height();
		double winratio = 1. * (eye2_scrollArea->width() - 2) / (eye2_scrollArea->height() - 2);
		if (imgratio > winratio)
			img_show = img_show.scaled((eye2_scrollArea->width() - 2),
				(eye2_scrollArea->width() - 2) / imgratio,
				Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		else
			img_show = img_show.scaled((eye2_scrollArea->height() - 2) * imgratio,
				(eye2_scrollArea->height() - 2),
				Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		eye2_imgLabel->setPixmap(QPixmap::fromImage(img_show));
		eye2_imgLabel->resize(img_show.width(), img_show.height());
	}
	if (!img_eye3.empty()) {
		QImage img_show;
		img_show = Mat2QImage(img_eye3);
		double imgratio = 1. * img_show.width() / img_show.height();
		double winratio = 1. * (eye3_scrollArea->width() - 2) / (eye3_scrollArea->height() - 2);
		if (imgratio > winratio)
			img_show = img_show.scaled((eye3_scrollArea->width() - 2),
				(eye3_scrollArea->width() - 2) / imgratio,
				Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		else
			img_show = img_show.scaled((eye3_scrollArea->height() - 2) * imgratio,
				(eye3_scrollArea->height() - 2),
				Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		eye3_imgLabel->setPixmap(QPixmap::fromImage(img_show));
		eye3_imgLabel->resize(img_show.width(), img_show.height());
	}
	if (!img_eye4.empty()) {
		QImage img_show;
		img_show = Mat2QImage(img_eye4);
		double imgratio = 1. * img_show.width() / img_show.height();
		double winratio = 1. * (eye4_scrollArea->width() - 2) / (eye4_scrollArea->height() - 2);
		if (imgratio > winratio)
			img_show = img_show.scaled((eye4_scrollArea->width() - 2),
				(eye4_scrollArea->width() - 2) / imgratio,
				Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		else
			img_show = img_show.scaled((eye4_scrollArea->height() - 2) * imgratio,
				(eye4_scrollArea->height() - 2),
				Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		eye4_imgLabel->setPixmap(QPixmap::fromImage(img_show));
		eye4_imgLabel->resize(img_show.width(), img_show.height());
	}
	if (!img_scene.empty()) {
		QImage img_show;
		img_show = Mat2QImage(img_scene);
		double imgratio = 1. * img_show.width() / img_show.height();
		double winratio = 1. * (scene_scrollArea->width() - 2) / (scene_scrollArea->height() - 2);
		if (imgratio > winratio)
			img_show = img_show.scaled((scene_scrollArea->width() - 2),
				(scene_scrollArea->width() - 2) / imgratio,
				Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		else
			img_show = img_show.scaled((scene_scrollArea->height() - 2) * imgratio,
				(scene_scrollArea->height() - 2),
				Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		scene_imgLabel->setPixmap(QPixmap::fromImage(img_show));
		scene_imgLabel->resize(img_show.width(), img_show.height());
	}
}
QImage GazeTrackingQtApp::Mat2QImage(const Mat& mat)
{
	if (mat.type() == CV_8UC1)                          // 单通道
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		image.setColorCount(256);                       // 灰度级数256
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		uchar* pSrc = mat.data;                         // 复制mat数据
		for (int row = 0; row < mat.rows; row++)
		{
			uchar* pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	else if (mat.type() == CV_8UC3)                     // 3通道
	{
		const uchar* pSrc = (const uchar*)mat.data;     // 复制像素
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);    // R, G, B 对应 0,1,2
		return image.rgbSwapped();                      // rgbSwapped是为了显示效果色彩好一些。
	}
	else if (mat.type() == CV_8UC4)                     // 4通道
	{
		const uchar* pSrc = (const uchar*)mat.data;     // 复制像素
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);        // B,G,R,A 对应 0,1,2,3
		return image.copy();
	}
	else
	{
		return QImage();
	}
}

void GazeTrackingQtApp::close_system()
{
	Release();
	open_cam->setEnabled(true);
	close_cam->setEnabled(false);

	Mat empty_eye(480, 640, CV_8UC3, cv::Scalar::all(255));
	Mat empty_scene(1080, 1920, CV_8UC3, cv::Scalar::all(255));
	Show_img(empty_eye, empty_eye, empty_eye, empty_eye, empty_scene);
}
void GazeTrackingQtApp::Release()
{
	if (!close_all_thread_)
		close_all_thread_ = true;
	start_video_capture = false;
	if (eye1.isOpened()) eye1.release();
	if (eye2.isOpened()) eye2.release();
	if (eye3.isOpened()) eye3.release();
	if (eye4.isOpened()) eye4.release();
	if (scene.isOpened()) scene.release();
	show_img_timer->stop();//stop timer to clear image display after closing system
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void GazeTrackingQtApp::save_video() {
	string folder_str = folder_path->text().toStdString();
	int i = 0;
	while (i < folder_str.length()) {
		if (folder_str[i] == '/') {
			folder_str = folder_str.substr(0, i) + "\\" + folder_str.substr(i + 1, folder_str.length() - i - 1);
		}
		++i;
	}
	if (_access(folder_str.c_str(), 0) != 0) {
		string command;
		command = "mkdir " + folder_str;
		int ret = system(command.c_str());
		if (ret != 0) {
			QMessageBox::critical(this, tr("错误"), tr("文件路径错误"));
			return;
		}
	}

	if (eye1.isOpened()) eye1_writer.open(folder_str + "\\eye1.avi", CV_FOURCC('X', 'V', 'I', 'D'), 20, Size(eye_img_width, eye_img_height));
	if (eye2.isOpened()) eye2_writer.open(folder_str + "\\eye2.avi", CV_FOURCC('X', 'V', 'I', 'D'), 20, Size(eye_img_width, eye_img_height));
	if (eye3.isOpened()) eye3_writer.open(folder_str + "\\eye3.avi", CV_FOURCC('X', 'V', 'I', 'D'), 20, Size(eye_img_width, eye_img_height));
	if (eye4.isOpened()) eye4_writer.open(folder_str + "\\eye4.avi", CV_FOURCC('X', 'V', 'I', 'D'), 20, Size(eye_img_width, eye_img_height));
	if (scene.isOpened()) scene_writer.open(folder_str + "\\scene.avi", CV_FOURCC('X', 'V', 'I', 'D'), 20, Size(scene_img_width, scene_img_height));
	start_video_capture = true;
	std::thread save_vid(std::bind(&GazeTrackingQtApp::SaveVideotoFile, this));
	save_vid.detach();
	QMessageBox::information(this, tr("提示"), tr("采集视频"));

	vid_capture->setText(tr("停止采集"));
	vid_capture->disconnect();
	connect(vid_capture, SIGNAL(clicked()), this, SLOT(stop_video_cap()));
}
void GazeTrackingQtApp::SaveVideotoFile() {
	while (!close_all_thread_ && start_video_capture) {
		std::unique_lock<std::mutex> write_lock(image_read_mutex);
		image_read_condition.wait(write_lock, [=]() {return image_ready; });
		image_ready = false;
		if (!eye1_img.empty()) eye1_writer.write(eye1_img);
		if (!eye2_img.empty()) eye2_writer.write(eye2_img);
		if (!eye3_img.empty()) eye3_writer.write(eye3_img);
		if (!eye4_img.empty()) eye4_writer.write(eye4_img);
		if (!scene_img.empty()) scene_writer.write(scene_img);
		write_lock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	if (eye1_writer.isOpened()) eye1_writer.release();
	if (eye2_writer.isOpened()) eye2_writer.release();
	if (eye3_writer.isOpened()) eye3_writer.release();
	if (eye4_writer.isOpened()) eye4_writer.release();
	if (scene_writer.isOpened()) scene_writer.release();

}

void GazeTrackingQtApp::stop_video_cap() {
	start_video_capture = false;
	vid_capture->setText(tr("采集视频"));
	vid_capture->disconnect();
	connect(vid_capture, SIGNAL(clicked()), this, SLOT(save_video()));

}