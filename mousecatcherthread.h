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

#ifndef MOUSECATCHERTHREAD_H
#define MOUSECATCHERTHREAD_H


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++" // TODO: Refactor
#include <QTime>
#include <QThread>
#pragma GCC diagnostic pop

#include <windows.h>

//#pragma comment(lib, "user32.lib")

class MouseCatcherThread : public QThread
{
	Q_OBJECT
public:
	MouseCatcherThread(const MouseCatcherThread&) = delete;
	static MouseCatcherThread* getInstance();
	virtual ~MouseCatcherThread();
	bool isStarted() const;
	void setStarted(bool Value);

	int getDelay() const;
	void setDelay(int Delay);

	bool isLoggingStarted() const;
	void setLoggingStarted(bool isLoggingStarted);

	bool isSelectionMode() const;
	void setSelectionMode(bool isSelectionMode);

	bool isMouseHoldAutoSelectionMode() const;
	void setMouseHoldAutoSelectionMode(bool MouseHoldAutoSelectionMode);

	double getMouseHoldingDelay() const;
	void setMouseHoldingDelay(double MouseHoldingDelay);

	MouseCatcherThread operator =(const MouseCatcherThread&) = delete;
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

	QTime m_ClickTimer;
	bool m_isStarted;
	bool m_isLoggingStarted;
	int m_Delay;
	bool m_isSelectionMode;
	bool m_isMouseHoldAutoSelectionMode;
	int m_MouseHoldingDelay;
	bool m_iToggled = false;
signals:
	void delay(int msec, bool blocked, bool isRight);
	void mouse_up();

};

#endif // MOUSECATCHERTHREAD_H
