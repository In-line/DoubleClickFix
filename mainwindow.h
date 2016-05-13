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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <windows.h>
#include <mousecatcherthread.h>
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

#include <QThread>
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool isMinimizing() const;
    void setMinimizing(bool value);

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
