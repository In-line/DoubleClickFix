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

#ifndef MOUSECATCHERTHREAD_H
#define MOUSECATCHERTHREAD_H
#include <QThread>
#include <QTime>
#include <windows.h>
#pragma comment(lib, "user32.lib")

class MouseCatcherThread : public QThread
{
Q_OBJECT
public:
    static MouseCatcherThread* getInstance();
    virtual ~MouseCatcherThread();
    bool isStarted() const;
    void setStarted(bool Value);

    int getDelay() const;
    void setDelay(int Delay);

    bool isLoggingStarted() const;
    void setLoggingStarted(bool isLoggingStarted);

private:
    MouseCatcherThread();
    void run();
    // Stuff for hooking
    HHOOK m_Hook;
    unsigned short iBlockMouseButtonUps[2];
    unsigned short iBlockMouseButtonDowns[2];
    static MouseCatcherThread *m_This;
    static LRESULT CALLBACK MouseCallBackProcProxy(int nCode,WPARAM wParam, LPARAM lParam);
    LRESULT MouseCallBackProc(int nCode,WPARAM wParam, LPARAM lParam);


private:
    QTime m_ClickTimer;
    bool m_isStarted;
    bool m_isLoggingStarted;
    int m_Delay;
signals:
    void delay(int msec, bool blocked, bool isRight);


};

#endif // MOUSECATCHERTHREAD_H