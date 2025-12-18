/******************************************************************
 Copyright (C) 2019 - All Rights Reserved by
 文 件 名 : Ap3216thread.h --- Ap3216Thread
 作 者    : Niyh(lynnhua)
 论 坛    : http://www.firebbs.cn
 编写日期 : 2019
 说 明    :
 历史纪录 :
 <作者>    <日期>        <版本>        <内容>
  Niyh	   2019    	1.0.0 1     文件创建
*******************************************************************/
#ifndef AP3216THREAD_H
#define AP3216THREAD_H

#include <QThread>

class Ap3216Thread : public QThread
{
    Q_OBJECT
public:
    explicit Ap3216Thread(QObject *parent = 0);
    ~Ap3216Thread();

    void Stop();
signals:
    void Ap3216signalUpdate(quint16 ir, quint16 als, quint16 ps);

public slots:

private:
    bool m_bRun;
    int  m_fd;
	
	quint16 buf[3];

private:
    int Ap3216_Init(void);
	
protected:
    void run();
};

#endif // DHT11THREAD_H
