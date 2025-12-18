/******************************************************************
 Copyright (C) 2019 - All Rights Reserved by
 文 件 名 : Adxl345thread.h --- Adxl345Thread
 作 者    : Niyh(lynnhua)
 论 坛    : http://www.firebbs.cn
 编写日期 : 2019
 说 明    :
 历史纪录 :
 <作者>    <日期>        <版本>        <内容>
  Niyh	   2019    	1.0.0 1     文件创建
*******************************************************************/
#ifndef ADXL345THREAD_H
#define ADXL345THREAD_H

#include <QThread>

class Adxl345Thread : public QThread
{
    Q_OBJECT
public:
    explicit Adxl345Thread(QObject *parent = 0);
    ~Adxl345Thread();

    void Stop();
signals:
    void Adxl345signalUpdate(qint32 x, qint32 y, qint32 z);

public slots:

private:
    bool m_bRun;
    int  m_fd;
	
    qint32 buf[3];

private:
    int Adxl345_Init(void);
	
protected:
    void run();
};

#endif // DHT11THREAD_H
