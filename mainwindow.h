#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include "qcustomplot.h"
#include "Dht11/dht11thread.h"      // DHT11高精度温湿度传感器
#include "Ap3216/ap3216thread.h"    // AP3216三合一环境光（红外 + 亮度 + 接近距离）
#include "Adxl345/adxl345thread.h"  // ADXL345三轴加速度传感器

#define ADC_INPUT_PATH                  "cat /sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define SYSFS_GPIO_EXPORT               "/sys/class/gpio/export"
#define SYSFS_GPIO_UNEXPORT             "/sys/class/gpio/unexport"

#define SYSFS_GPIO_BEEP_PIN_VAL         "70"
#define SYSFS_GPIO_BEEP_DIR             "/sys/class/gpio/gpio70/direction"
#define SYSFS_GPIO_BEEP_DIR_VAL         "out"
#define SYSFS_GPIO_BEEP_VAL             "/sys/class/gpio/gpio70/value"

#define SK9822_PATH                     "/dev/sk9822"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // DHT11数据采集相关
    Dht11Thread *dht11_threadTest;

    // AP3216数据采集相关
    Ap3216Thread *ap3216_threadTest;

    // ADXL345数据采集相关
    Adxl345Thread *adxl345_threadTest;

    // ADC数据采集相关
    QProcess *adc_process;
    QTimer *adc_timer;

    // SK9822 多彩LED灯控制相关
    int led_fd;
    unsigned char led_close[3]  = {0  , 0  , 0  };  // 关
    unsigned char led_white[3]  = {255, 255, 255};  // 白
    unsigned char led_red[3]    = {255, 0  , 0  };  // 红
    unsigned char led_orange[3] = {255, 125, 0  };  // 橙
    unsigned char led_yellow[3] = {255, 255, 0  };  // 黄
    unsigned char led_green[3]  = {0  , 255, 0  };  // 绿
    unsigned char led_cyan[3]   = {0  , 255, 255};  // 青
    unsigned char led_bule[3]   = {0  , 0  , 255};  // 蓝
    unsigned char led_purple[3] = {255, 0  , 255};  // 紫

    // 蜂鸣器控制相关
    bool isBeepOn;
    void openBeep();
    void closeBeep();

    // 折线图绘制相关
    QCustomPlot *plot_tah;      // 绘图控件指针
    QStatusBar *bar;            // 状态栏指针
    QCPGraph *graph_temp;       // 温度曲线指针
    QCPGraph *graph_hum;        // 湿度曲线指针
    double temp_data;           // 温度数据
    double hum_data;            // 湿度数据
    QTimer *timer_plot;
    QString temp_str;
    QString hum_str;
    void QPlot_Init(QCustomPlot *customPlot);
    void Plot_Show(QCustomPlot *customPlot, double temp, double hum);

    // 数据转换相关
    double string2double(QString data);


private slots:
    // DHT11数据采集相关槽函数
    void onTahDataClicked();
    void onDht11UpdateValues(QString T, QString H);

    // AP3216数据采集相关槽函数
    void onTriadLightClicked();
    void AP3216SltUpdateValues(quint16 ir, quint16 als, quint16 ps);

    // ADXL345数据采集相关槽函数
    void onAccelerationClicked();
    void ADXL345SltUpdateValues(qint32 x, qint32 y, qint32 z);

    // ADC数据采集相关槽函数
    void onAdcClicked();
    void ADCslotTimeoutTimer(void);

    // Beep蜂鸣器控制相关槽函数
    void onBeepClicked();

    // SK9822控制相关槽函数
    void onLEDComboBoxChanged(QString n_str);

    // QCustomPlot绘图相关槽函数
    void onPlotTimerTimeOut(void);
    void onTempCheckBoxStateChanged(int arg_temp);
    void onHumCheckBoxStateChanged(int arg_hum);
};

#endif // MAINWINDOW_H
