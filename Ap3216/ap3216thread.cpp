/******************************************************************
 Copyright (C) 2019 - All Rights Reserved by
 文 件 名 : Ap3216thread.cpp --- Ap3216Thread
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

#include "ap3216thread.h"


Ap3216Thread::Ap3216Thread(QObject *parent) : QThread(parent),
    m_bRun(false),m_fd(-1)
{

}

Ap3216Thread::~Ap3216Thread()
{
    m_bRun = false;
    if (this->isRunning()) {
        this->quit();
    }
}

void Ap3216Thread::Stop()
{
    m_bRun = false;
    this->wait(2000);
}

//Ap3216初始化
int Ap3216Thread::Ap3216_Init(void)
{
    int fd = -1;

    fd  = open("/dev/ap3216c", O_RDONLY);               // open file and enable read and  write

    if (fd < 0)
    {
        perror("Can't open /dev/ap3216c \n");       	// open i2c dev file fail
        return - 1;
    }

    printf("open /dev/ap3216c success !\n");          // open i2c dev file succes

    m_bRun = true;

    return fd;
}

void Ap3216Thread::run()
{
    m_fd = Ap3216_Init();

    qDebug() << "m_fd" << m_fd << m_bRun;

    while (m_bRun)
    {
        //printf("\033[2J");
        
        read(m_fd, buf, sizeof(buf));

        emit Ap3216signalUpdate(buf[0] , buf[1] , buf[2]);
        msleep(200);
    }
	
    close(m_fd);
}
