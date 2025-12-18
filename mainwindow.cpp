#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/time.h>
#include<errno.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->showFullScreen();

    // DHT11数据采集
    // 1. 实例化dht11线程对象并连接信号与槽
    dht11_threadTest = new Dht11Thread(this);
    connect(dht11_threadTest, &Dht11Thread::signalUpdate, this, &MainWindow::onDht11UpdateValues);
    // 2. 初始化数据监测按钮并连接信号与槽
    ui->button_tah->setText("开始监测");
    connect(ui->button_tah, &QPushButton::clicked, this, &MainWindow::onTahDataClicked);

    // AP3216数据采集
    // 1. 实例化ap3216线程对象并连接信号与槽
    ap3216_threadTest = new Ap3216Thread(this);
    connect(ap3216_threadTest, &Ap3216Thread::Ap3216signalUpdate, this, &MainWindow::AP3216SltUpdateValues);
    // 2. 初始化数据监测按钮并连接信号与槽
    ui->button_triad_light->setText("开始监测");
    connect(ui->button_triad_light, &QPushButton::clicked, this, &MainWindow::onTriadLightClicked);

    // ADXL345数据采集
    // 1. 实例化ADXL345线程对象并连接信号与槽
    adxl345_threadTest = new Adxl345Thread(this);
    connect(adxl345_threadTest, &Adxl345Thread::Adxl345signalUpdate, this, &MainWindow::ADXL345SltUpdateValues);
    // 2. 初始化数据监测按钮并连接信号与槽
    ui->button_acceleration->setText("开始监测");
    connect(ui->button_acceleration, &QPushButton::clicked, this, &MainWindow::onAccelerationClicked);

    // ADC数据采集
    // 1. 实例化ADCProcess对象
    adc_process = new QProcess(this);
    // 2. 初始化数据监测按钮并连接信号与槽
    ui->button_adc->setText("开始监测");
    connect(ui->button_adc, &QPushButton::clicked, this, &MainWindow::onAdcClicked);
    // 3. 初始化定时器并连接信号与槽
    adc_timer = new QTimer(this);
    connect(adc_timer, &QTimer::timeout, this, &MainWindow::ADCslotTimeoutTimer);

    // SK9822控制
    // 1. 初始化fd，同时获取SK9822多彩LED灯的文件描述符fd
    led_fd = -1;
    led_fd = open(SK9822_PATH, O_RDWR);
    if (led_fd < 0)
    {
        perror("Can't open sk9822!!");
        return;
    }
    printf("open sk9822 success !!!");
    // 2. 初始化LED灯为关闭
    write(led_fd, &led_close, 3);
    // 3. 初始化颜色下拉框并绑定信号与槽
    ui->combo_box_led_color->setCurrentText("关闭");
    connect(ui->combo_box_led_color, &QComboBox::currentTextChanged, this, &MainWindow::onLEDComboBoxChanged);

    // Beep蜂鸣器控制
    // 1. 初始化蜂鸣器状态和控制按钮状态，连接信号与槽
    isBeepOn = false;
    this->closeBeep();
    ui->button_switch->setStyleSheet("border-image: url(:/icon/switch-off.png);");
    connect(ui->button_switch, &QToolButton::clicked, this, &MainWindow::onBeepClicked);

    // 折线图绘制
    // 1. 通过指针变量和UI控件绑定，方便书写
    plot_tah = ui->custom_plot_tah;
    // 2. 初始化状态栏指针
    bar = statusBar();
    // 3. 创建选择曲线显示复选框信号与槽
    connect(ui->checkBox_temp, &QCheckBox::stateChanged, this, &MainWindow::onTempCheckBoxStateChanged);
    connect(ui->checkBox_hum, &QCheckBox::stateChanged, this, &MainWindow::onHumCheckBoxStateChanged);
    // 4. 初始化定时器，用于定时生成曲线坐标点数据
    timer_plot = new QTimer(this);
    connect(timer_plot, &QTimer::timeout, this, &MainWindow::onPlotTimerTimeOut);
    // 5. 初始化图表内容
    QPlot_Init(plot_tah);

    // 关闭按钮功能绑定
    connect(ui->button_close, &QPushButton::clicked, this, &MainWindow::close);
}

MainWindow::~MainWindow()
{
    delete ui;
    ::close(led_fd);
}

void MainWindow::onTahDataClicked()
{
    // 1. 通过现在按钮的显示文字作为状态判断依据
    QString state = ui->button_tah->text();
    if (state == "开始监测")
    {
        // 2.1 显示：开始监测 执行监测程序
        ui->button_tah->setText("停止监测");
        dht11_threadTest->start();
        // 2.2 开启折线图内容显示
        timer_plot->start(200);
    }
    else if (state == "停止监测")
    {
        // 3.1 显示：停止监测 执行关闭监测程序
        ui->button_tah->setText("开始监测");
        dht11_threadTest->Stop();

        // 3.2 停止图表
        timer_plot->stop();
    }
}

void MainWindow::onTriadLightClicked()
{
    // 1. 通过现在按钮的显示文字作为状态判断依据
    QString state = ui->button_triad_light->text();
    if (state == "开始监测")
    {
        // 2. 显示：开始监测 执行监测程序
        ui->button_triad_light->setText("停止监测");
        ap3216_threadTest->start();
    }
    else if (state == "停止监测")
    {
        // 3. 显示：停止监测 执行关闭监测程序
        ui->button_triad_light->setText("开始监测");
        ap3216_threadTest->Stop();
    }
}

void MainWindow::onAccelerationClicked()
{
    // 1. 通过现在按钮的显示文字作为状态判断依据
    QString state = ui->button_acceleration->text();
    if (state == "开始监测")
    {
        // 2. 显示：开始监测 执行监测程序
        ui->button_acceleration->setText("停止监测");
        adxl345_threadTest->start();
    }
    else if (state == "停止监测")
    {
        // 3. 显示：停止监测 执行关闭监测程序
        ui->button_acceleration->setText("开始监测");
        adxl345_threadTest->Stop();
    }
}

void MainWindow::onAdcClicked()
{
    // 1. 通过现在按钮的显示文字作为状态判断依据
    QString state = ui->button_adc->text();
    if (state == "开始监测")
    {
        // 2. 显示：开始监测 执行监测程序
        ui->button_adc->setText("停止监测");
        adc_timer->start(200);
    }
    else if (state == "停止监测")
    {
        // 3. 显示：停止监测 执行关闭监测程序
        ui->button_adc->setText("开始监测");
        adc_timer->stop();
    }
}

void MainWindow::onBeepClicked()
{
    if (isBeepOn)
    {
        // isBeepOn为真 -> 蜂鸣器是打开状态 -> 执行关闭操作 -> 替换按钮显示图片为关闭样式
        this->closeBeep();
        isBeepOn = !isBeepOn;
        ui->button_switch->setStyleSheet("border-image: url(:/icon/switch-off.png);");
    }
    else
    {
        // isBeepOn为假 -> 蜂鸣器是关闭状态 -> 执行打开操作 -> 替换按钮显示图片为打开样式
        this->openBeep();
        isBeepOn = !isBeepOn;
        ui->button_switch->setStyleSheet("border-image: url(:/icon/switch-on.png);");
    }
}

void MainWindow::onLEDComboBoxChanged(QString n_str)
{
    if (n_str == "关闭")
    {
        write(led_fd, &led_close, 3);
    }
    else if (n_str == "白色")
    {
        write(led_fd, &led_white, 3);
    }
    else if (n_str == "红色")
    {
        write(led_fd, &led_red, 3);
    }
    else if (n_str == "橙色")
    {
        write(led_fd, &led_orange, 3);
    }
    else if (n_str == "黄色")
    {
        write(led_fd, &led_yellow, 3);
    }
    else if (n_str == "绿色")
    {
        write(led_fd, &led_green, 3);
    }
    else if (n_str == "青色")
    {
        write(led_fd, &led_cyan, 3);
    }
    else if (n_str == "蓝色")
    {
        write(led_fd, &led_bule, 3);
    }
    else if (n_str == "紫色")
    {
        write(led_fd, &led_purple, 3);
    }
}

void MainWindow::onDht11UpdateValues(QString T, QString H)
{
    temp_str = T;
    hum_str = H;
    ui->label_text_sensor_data_temp->setText(T);
    ui->label_text_sensor_data_humility->setText(H);
}

void MainWindow::AP3216SltUpdateValues(quint16 ir, quint16 als, quint16 ps)
{
    ui->label_text_sensor_data_ir->setText(QString::number(ir));
    ui->label_text_sensor_data_als->setText(QString::number(als));
    ui->label_text_sensor_data_ps->setText(QString::number(ps));
}

void MainWindow::ADXL345SltUpdateValues(qint32 x, qint32 y, qint32 z)
{
    ui->lcd_number_x->display(x);
    ui->lcd_number_y->display(y);
    ui->lcd_number_z->display(z);
}

void MainWindow::ADCslotTimeoutTimer(void)
{
    // 1. 通过QProcess获取ADC数据
    adc_process->start(ADC_INPUT_PATH);
    adc_process->waitForFinished(-1);

    // 2. 数据封装显示
    QString outPutADC = QString(adc_process->readAllStandardOutput());
    // 2.1 adc数据处理
    int adc_raw = outPutADC.toDouble();
    // 2.2 电压值处理 单位: mv
    float adc_volt = adc_raw * 1800 / 1023;

    // 3. 数据显示
    ui->label_text_sensor_data_adc->setText(QString::number(adc_raw));
    ui->label_text_sensor_data_voltage->setText(QString::number(adc_volt));
}

void MainWindow::openBeep()
{
    int fd;

    // 1. 导出控制蜂鸣器的GPIO引脚编号
    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    if(fd == -1)
    {
        printf("ERR: Beep pin open error!");
        return;
    }
    write(fd, SYSFS_GPIO_BEEP_PIN_VAL, sizeof(SYSFS_GPIO_BEEP_PIN_VAL));
    ::close(fd);    // 关闭fd，为下一个操作准备

    // 2. 定义GPIO的输入输出方向
    fd = open(SYSFS_GPIO_BEEP_DIR, O_WRONLY);
    if(fd == -1)
    {
        printf("ERR: Beep pin open error!");
        return;
    }
    write(fd, SYSFS_GPIO_BEEP_DIR_VAL, sizeof(SYSFS_GPIO_BEEP_DIR_VAL));
    ::close(fd);    // 关闭fd，为下一个操作准备

    // 3. GPIO输出打开蜂鸣器
    fd = open(SYSFS_GPIO_BEEP_VAL, O_WRONLY);
    write(fd, "0", 1);
    :: close(fd);
}

void MainWindow::onTempCheckBoxStateChanged(int arg_temp)
{
    if(arg_temp){
        graph_temp->setVisible(true);
    }else{
        graph_temp->setVisible(false);
    }
    plot_tah->replot();
}

void MainWindow::onHumCheckBoxStateChanged(int arg_hum)
{
    if(arg_hum){
        graph_hum->setVisible(true);
    }else{
        graph_hum->setVisible(false);
    }
    plot_tah->replot();
}

void MainWindow::closeBeep()
{
    int fd;

    // 1. GPIO输出关闭蜂鸣器
    fd = open(SYSFS_GPIO_BEEP_VAL, O_WRONLY);
    write(fd, "1", 1);
    :: close(fd);

    // 2. 取消导出控制的GPIO引脚编号
    fd = open(SYSFS_GPIO_UNEXPORT, O_WRONLY);
    if(fd == -1)
    {
        printf("ERR: Beep pin open error!");
        return;
    }
    write(fd, SYSFS_GPIO_BEEP_PIN_VAL, sizeof(SYSFS_GPIO_BEEP_PIN_VAL));
    ::close(fd);
}

void MainWindow::QPlot_Init(QCustomPlot *customPlot)
{
    // 图表添加两条曲线
    graph_temp = customPlot->addGraph();
    graph_hum = customPlot->addGraph();

    // 设置曲线参数
    QPen pen_temp;
    pen_temp.setWidth(3);       // 线宽
    pen_temp.setColor(Qt::red); // 颜色
    QPen pen_hum;
    pen_hum.setWidth(3);        // 线宽
    pen_hum.setColor(Qt::blue); // 颜色

    // 设置曲线颜色
    graph_temp->setPen(pen_temp);
    graph_hum->setPen(pen_hum);

    // 设置y坐标轴显示范围
    customPlot->yAxis->setRange(10,90);

    // 显示图表的图例
    customPlot->legend->setVisible(true);
    // 添加曲线名称
    graph_temp->setName("温度数据");
    graph_hum->setName("湿度数据");

    // 允许用户用鼠标拖动轴范围，用鼠标滚轮缩放，点击选择图形:
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void MainWindow::onPlotTimerTimeOut(void)
{
    // 获取温度和湿度数据
    temp_data = this->string2double(temp_str);
    hum_data = this->string2double(hum_str);
    this->Plot_Show(plot_tah, temp_data, hum_data);
}

void MainWindow::Plot_Show(QCustomPlot *customPlot, double temp, double hum)
{
    static double cnt;
    cnt++;
    if (temp > 100)
        temp = 50;
    if (hum > 100)
        hum = 100;
    graph_temp->addData(cnt, temp);
    graph_hum->addData(cnt, hum);
    customPlot->xAxis->setRange((graph_temp->dataCount() > 1000 ? (graph_temp->dataCount() - 1000) : 0), graph_temp->dataCount());
    customPlot->replot(QCustomPlot::rpQueuedReplot);
}

double MainWindow::string2double(const QString data)
{
    // 正则表达式，匹配负数或正数的浮点数
    QRegExp regExp("(-?\\d+\\.\\d+)");
    if (regExp.indexIn(data) != -1)
    {
        // 获取第一个匹配的子表达式
        QString match = regExp.cap(1);
        bool ok;
        // 转换为double
        double data_double = match.toDouble(&ok);
        if (ok)
        {
            // 如果转换成功，返回double数值
            printf("now double data: %lf", data_double);
            return data_double;
        }
    }
    // 如果没有匹配或转换失败，返回0
    return 0;
}
