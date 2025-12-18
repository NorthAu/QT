/******************************************************************
 Copyright (C) 2019 - All Rights Reserved by
 文 件 名 : Adxl345thread.cpp --- Adxl345Thread
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

#include "adxl345thread.h"


Adxl345Thread::Adxl345Thread(QObject *parent) : QThread(parent),
    m_bRun(false),m_fd(-1)
{

}

Adxl345Thread::~Adxl345Thread()
{
    m_bRun = false;
    if (this->isRunning()) {
        this->quit();
    }
}

void Adxl345Thread::Stop()
{
    m_bRun = false;
    this->wait(2000);
}

//Adxl345初始化
int Adxl345Thread::Adxl345_Init(void)
{
    int fd = -1;

    fd  = open("/dev/adxl345", O_RDWR);               // open file and enable read and  write

    if (fd < 0)
    {
        perror("Can't open /dev/adxl345 \n");       	// open i2c dev file fail
        return - 1;
    }

    printf("open /dev/adxl345 success !\n");          // open i2c dev file succes

    m_bRun = true;

    return fd;
}

void Adxl345Thread::run()
{
    m_fd = Adxl345_Init();

    qDebug() << "m_fd" << m_fd << m_bRun;

    while (m_bRun)
    {
        //printf("\033[2J");
        
        read(m_fd, buf, sizeof(buf));

        emit Adxl345signalUpdate(buf[0] , buf[1] , buf[2]);

        msleep(200);
    }
	
    close(m_fd);
}
