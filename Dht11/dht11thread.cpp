/******************************************************************
 Copyright (C) 2019 - All Rights Reserved by
 文 件 名 : Dht11thread.cpp --- Dht11Thread
 作 者    : Niyh(lynnhua)
 论 坛    : http://www.firebbs.cn
 编写日期 : 2019
 说 明    :
 历史纪录 :
 <作者>    <日期>        <版本>        <内容>
  Niyh	   2019    	1.0.0 1     文件创建
*******************************************************************/
#include <QDebug>

//包含头文件
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/time.h>
#include<errno.h>

#include<sys/ioctl.h>
#include<sys/select.h>

#include "dht11thread.h"


Dht11Thread::Dht11Thread(QObject *parent) : QThread(parent),
    m_bRun(false),m_fd(-1)
{

}

Dht11Thread::~Dht11Thread()
{
    m_bRun = false;
    if (this->isRunning()) {
        this->quit();
    }
}

void Dht11Thread::Stop()
{
    m_bRun = false;
    this->wait(2000);
}

//Dht11初始化
int Dht11Thread::Dht11_Init(void)
{
    int fd = -1;

    fd  = open("/dev/dht11", O_RDONLY);               // open file and enable read and  write

    if (fd < 0)
    {
        perror("Can't open /dev/Dht11 \n");       	// open i2c dev file fail
        return - 1;
    }

    printf("open /dev/Dht11 success !\n");          // open i2c dev file succes

    m_bRun = true;

    return fd;
}

void Dht11Thread::run()
{
    QString T,H;

    m_fd = Dht11_Init();

    qDebug() << "m_fd" << m_fd << m_bRun;

    while (m_bRun)
    {
        //printf("\033[2J");
        
        read(m_fd, buf, sizeof(buf));

        T.sprintf("%d.%d℃",buf[2],buf[3]);
        H.sprintf("%d.%d%%",buf[0],buf[1]);
        emit signalUpdate(T, H);

		msleep(2000);
    }
	
    close(m_fd);
}
