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
*    along with DoubleClickFix.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <windows.h>
#include "mousecatcherthread.h"
#include <qsystemtrayicon.h>

namespace Ui {
class MainWindow;
}
#include <QSettings>

#define SETTINGS_ENTRY_NAME "DoubleClickFix.ini"

#define NO 0
#define YES 1

#define AUTO_START_WHEN_LAUNCH "AutoStartWhenLaunch"
#define LAUNCH_WITH_OS "LaunchWithOS"
#define LAUNCH_MINIMIZED "LaunchMinimized"

#define BLOCK_DELAY "BlockDelayInMsecs"
#define BLOCK_DELAY_DEFAULT 0.125

#define LOGGING_ENABLE "LoggingEnabled_I_love_you_mazdan"
#define LOGGING_ENABLED_NO "Off"

#define CLICK_LOCK_HOTKEY_ENABLED "ClickLockHotKeyEnabled"
#define CLICK_LOCK_HOTKEY_ENABLED_DEFAULT false


#define CLICK_LOCK_HOTKEY "ClickLockHotKey"
#define CLICK_LOCK_HOTKEY_DEFAULT "0"

#define CLICK_CLOCK_MOUSE_ENABLED "ClickLockMouseEnabled"
#define CLICK_CLOCK_MOUSE_ENABLED_DEFAULT CLICK_LOCK_HOTKEY_ENABLED_DEFAULT

#define CLICK_LOCK_MOUSE_DELAY "ClickLockMouseDelay"
#define CLICK_LOCK_MOUSE_DELAY_DEFAULT 0.01
#include <QThread>
#ifdef QT_DEBUG
#include <QDebug>
#endif
#include "hotkey.h"
#define HOTKEY_ID 1

#include "click_lock_settings.h"

class MainWindow;

class WindowsHotKeyFilter : public QObject
{
	Q_OBJECT
private:
	static WindowsHotKeyFilter* m_This;
	static HHOOK m_Hook;
	MainWindow *m_Overlord;
	LRESULT True_LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	WindowsHotKeyFilter(MainWindow *Overlord)
	{
		m_Overlord = Overlord;
	}
	bool m_iDisabled = false;
public:
	virtual ~WindowsHotKeyFilter()
	{
		UnhookWindowsHookEx(m_Hook);
		m_Hook = nullptr;
		m_This = nullptr;
	}

	static WindowsHotKeyFilter *getInstance(MainWindow *Overlord);
	static LRESULT CALLBACK  WindowsHotKey_LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	bool isDisabled() const;
	void setDisabled(bool iDisabled);

signals:
	void HotKey_Up();
	void HotKey_Down();
};

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	bool isMinimizing() const;
	void setMinimizing(bool value);
	hotKey getHotKey() const;
private:
	void inline setHotKey(hotKey &value);
	click_lock_settings_data m_ClickLockSettings;
	bool m_iHotKeyToggled = false;
	WindowsHotKeyFilter *m_HkFilter;
	void UpdateClickLockSettings();
protected:
	void changeEvent(QEvent *e);
private slots:
	void on_StartPutton_clicked();

	void on_StopButton_clicked();

	void on_doubleSpinBox_valueChanged(double arg1);

	void on_comboBox_currentIndexChanged(const QString &arg1);
	void slotProcessLogInformation(int msec, bool blocked, bool isRight);
	void on_StartMinimized_clicked();

	void slotIconActivated(QSystemTrayIcon::ActivationReason reason);

	void on_ClickLockSettingsButton_clicked();

private:
	MouseCatcherThread *m_Catcher;
	Ui::MainWindow *ui;
	QSettings *m_Settings = 0;
	quint64 m_TotalClicks = 0;
	quint64 m_BlockedClicks = 0;
	QSystemTrayIcon *trayIcon;

	bool m_isMinimizing = false;

};


#endif // MAINWINDOW_H
