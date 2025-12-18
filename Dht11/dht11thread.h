/******************************************************************
 Copyright (C) 2019 - All Rights Reserved by
 文 件 名 : Dht11thread.h --- Dht11Thread
 作 者    : Niyh(lynnhua)
 论 坛    : http://www.firebbs.cn
 编写日期 : 2019
 说 明    :
 历史纪录 :
 <作者>    <日期>        <版本>        <内容>
  Niyh	   2019    	1.0.0 1     文件创建
*******************************************************************/
#ifndef DHT11THREAD_H
#define DHT11THREAD_H

#include <QThread>

class Dht11Thread : public QThread
{
    Q_OBJECT
public:
    explicit Dht11Thread(QObject *parent = 0);
    ~Dht11Thread();

    void Stop();
signals:
    void signalUpdate(QString T, QString H);

public slots:

private:
    bool m_bRun;
    int  m_fd;
	
	quint8 buf[4];

private:
    int Dht11_Init(void);
	
protected:
    void run();
};

#endif // DHT11THREAD_H
