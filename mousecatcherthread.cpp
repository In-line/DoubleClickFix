/*   Copyright (C) 2016 Alik Aslanyan <cplusplus256@gmail.com>
*    This file is part of DoubleClickFix.
*
*    DoubleClickFix is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    DoubleClickFix is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mousecatcherthread.h"
#include <QMessageBox>
#include <QDebug>

MouseCatcherThread *MouseCatcherThread::m_This;
#define BLOCK_CALL 1

MouseCatcherThread::MouseCatcherThread() : QThread(), m_isStarted(false), m_Delay(0),m_isLoggingStarted(false),m_Hook(NULL)
{
    MouseCatcherThread::m_This = this;
    m_ClickTimer.start();
    setObjectName("MouseCatcherThread");
    iBlockMouseButtonUps[0] = iBlockMouseButtonUps[1] = 0;
    iBlockMouseButtonDowns[0] = iBlockMouseButtonDowns[1] = 0;
}

MouseCatcherThread *MouseCatcherThread::getInstance()
{
    if(!m_This) m_This = new MouseCatcherThread();

    return m_This;
}

MouseCatcherThread::~MouseCatcherThread()
{
    UnhookWindowsHookEx(m_Hook);
    m_This = NULL;
    m_Hook = NULL;
}
#if defined QT_DEBUG
#include <QTimer>
#endif
LRESULT inline MouseCatcherThread::MouseCallBackProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode != HC_ACTION) return CallNextHookEx(m_Hook, nCode, wParam, lParam);

    switch(wParam)
    {
    case WM_MOUSEMOVE:
    {
#if defined QT_DEBUG
        static int iCount = 0;
        if(iCount == 0)
        {
            QTimer::singleShot(1000,Qt::VeryCoarseTimer,this,[=](){
                qDebug()<<"WM_MOUSEMOVE per sec: " << iCount;
                iCount=0;
            });
        }
        iCount++;
#endif
        return CallNextHookEx(m_Hook, nCode, wParam, lParam);
        break;
    }
    case WM_LBUTTONDOWN: case WM_RBUTTONDOWN:
    {
        if(isStarted())
        {
            bool isRight = wParam==WM_RBUTTONDOWN;
            int iMSecs = m_ClickTimer.elapsed();
            m_ClickTimer.restart();
            bool needLog = isLoggingStarted();

            if(getDelay()>=iMSecs)
            {
                iBlockMouseButtonUps[(int)isRight]++;
                if(needLog) emit delay(iMSecs, true, isRight);
                return BLOCK_CALL;
            }
            else if(needLog)
            {
                emit delay(iMSecs, false, isRight);
            }
        }
        break;
    }
    case WM_LBUTTONUP: case WM_RBUTTONUP:
    {
        if(isStarted())
        {
            int index = (int)wParam==WM_RBUTTONUP;
            if(iBlockMouseButtonUps[index]>0)
            {
                iBlockMouseButtonUps[index]--;
                return BLOCK_CALL;
            }

        }
        break;
    }
    }

    return CallNextHookEx(m_Hook, nCode, wParam, lParam);
}



LRESULT inline MouseCatcherThread::MouseCallBackProcProxy(int nCode, WPARAM wParam, LPARAM lParam)
{
    return MouseCatcherThread::m_This->MouseCallBackProc( nCode,wParam, lParam);
}

int inline MouseCatcherThread::getDelay() const
{
    return m_Delay;
}

void MouseCatcherThread::setDelay(int Delay)
{
    m_Delay = Delay;
}


bool inline MouseCatcherThread::isStarted() const
{
    return m_isStarted;
}

void MouseCatcherThread::setStarted(bool Value)
{
    m_isStarted = Value;
}

bool inline MouseCatcherThread::isLoggingStarted() const
{
    return m_isLoggingStarted;
}

void MouseCatcherThread::setLoggingStarted(bool isLoggingStarted)
{
    m_isLoggingStarted = isLoggingStarted;
}


void MouseCatcherThread::run()
{
    m_Hook = SetWindowsHookEx(WH_MOUSE_LL,MouseCallBackProcProxy,NULL,0);
    if(m_Hook==NULL)
    {
        QMessageBox Mb;
        Mb.critical(0,"Critical Error!",(QString("Cannot set WH_MOUSE_LL hook! Error: ") + QString::number((double)GetLastError(),'f',1)));
    }
    else exec();
}
