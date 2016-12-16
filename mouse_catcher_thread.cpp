/*  	 Copyright (C) 2016 Alik Aslanyan <cplusplus256@gmail.com>
*  	  	This file is part of DoubleClickFix.
*
*  	  	DoubleClickFix is free software: you can redistribute it and/or modify
*  	  	it under the terms of the GNU General Public License as published by
*  	  	the Free Software Foundation, either version 3 of the License, or
*  	  	(at your option) any later version.
*
*  	  	DoubleClickFix is distributed in the hope that it will be useful,
*  	  	but WITHOUT ANY WARRANTY; without even the implied warranty of
*  	  	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  	See the
*  	  	GNU General Public License for more details.
*
*  	  	You should have received a copy of the GNU General Public License
*  	  	along with DoubleClickFix.  	If not, see <http://www.gnu.org/licenses/>.
*/

#include "mouse_catcher_thread.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++" // TODO: Refactor
#include <QMessageBox>
#include <QTimer>
#if defined QT_DEBUG
#include <QDebug>
#endif
#pragma GCC diagnostic pop

MouseCatcherThread *MouseCatcherThread::m_This;
#define BLOCK_CALL 1

MouseCatcherThread::MouseCatcherThread() :
	QThread(),
	m_Hook(),
	m_ClickTimer(),
	m_isStarted(false),
	m_isLoggingStarted(false),
	m_Delay(1),
	m_isSelectionMode(false),
	m_isMouseHoldAutoSelectionMode(false),
	m_MouseHoldingDelay(1),
	m_iToggled(false)
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
	m_This = nullptr;
	m_Hook = nullptr;
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

				if(!isRight)
				{
					if(m_iToggled)
					{
						m_iToggled=false;

						POINT p;
						GetCursorPos(&p);
						INPUT ip;
						memset(&ip, 0, sizeof(INPUT));
						ip.type = INPUT_MOUSE;
						ip.mi.dx = p.x;
						ip.mi.dy = p.y;
						ip.mi.dwFlags = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE;
						SendInput(1, &ip, sizeof(INPUT));
						setSelectionMode(false);
						return BLOCK_CALL;
					}
					else if(isSelectionMode())
					{
						m_iToggled=true;
					}
					else if(isMouseHoldAutoSelectionMode())
					{

						QTimer *t = new QTimer();
						t->setSingleShot(true);
						t->setInterval(getMouseHoldingDelay());
						t->moveToThread(this);
						connect(this, SIGNAL(mouse_up()), t, SLOT(stop()));
						connect(this, SIGNAL(mouse_up()), t, SLOT(deleteLater()));

						connect(this, SIGNAL(finished()), t, SLOT(stop()));
						connect(this, SIGNAL(finished()), t, SLOT(deleteLater()));


						connect(t, &QTimer::timeout, [this](){
							setSelectionMode(true);
							m_iToggled=true;
						});


						t->start();
					}
				}
			}
			break;
		}
		case WM_LBUTTONUP: case WM_RBUTTONUP:
		{
			if(isStarted())
			{
				short index = short(wParam==WM_RBUTTONUP);
				if((m_iToggled || isSelectionMode()) &&  	index==0)
				{
					if(iBlockMouseButtonUps[index]>0)
					{
						--iBlockMouseButtonUps[index];
					}
					return BLOCK_CALL;
				}
				else
				{
					if(iBlockMouseButtonUps[index]>0)
					{
						--iBlockMouseButtonUps[index];
						return BLOCK_CALL;
					}
					else if(isMouseHoldAutoSelectionMode())
					{
						emit mouse_up();
					}
				}
				break;
			}
		}
	}
	return CallNextHookEx(m_Hook, nCode, wParam, lParam);
}

double MouseCatcherThread::getMouseHoldingDelay() const
{
	return m_MouseHoldingDelay;
}

void MouseCatcherThread::setMouseHoldingDelay(double MouseHoldingDelay)
{
	m_MouseHoldingDelay = MouseHoldingDelay;
}

bool MouseCatcherThread::isMouseHoldAutoSelectionMode() const
{
	return m_isMouseHoldAutoSelectionMode;
}

void MouseCatcherThread::setMouseHoldAutoSelectionMode(bool MouseHoldAutoSelectionMode)
{
	m_isMouseHoldAutoSelectionMode = MouseHoldAutoSelectionMode;
}

bool inline MouseCatcherThread::isSelectionMode() const
{
	return m_isSelectionMode;
}

void MouseCatcherThread::setSelectionMode(bool isSelectionMode)
{
	m_isSelectionMode = isSelectionMode;
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
	m_Hook = SetWindowsHookEx(WH_MOUSE_LL,MouseCallBackProcProxy,nullptr,0);
	if(m_Hook==NULL)
	{
		QMessageBox Mb;
		Mb.critical(nullptr,"Critical Error!",(QString("Cannot set WH_MOUSE_LL hook! Error: ") + QString::number((double)GetLastError(),'f',1)));
	}
	else exec();
}
