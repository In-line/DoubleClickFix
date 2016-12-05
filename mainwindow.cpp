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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QTimer>
#include <QMenu>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <ctime>
#include <QAbstractEventDispatcher>
MainWindow::MainWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainWindow)
{

	m_Catcher = MouseCatcherThread::getInstance();
	connect(m_Catcher,SIGNAL(delay(int,bool,bool)),this,SLOT(slotProcessLogInformation(int,bool,bool)),Qt::QueuedConnection);

	m_HkFilter = WindowsHotKeyFilter::getInstance(this);
	this->m_Catcher->setSelectionMode(false);
	connect(this->m_HkFilter, &WindowsHotKeyFilter::HotKey_Down,[this](){
		if(!m_iHotKeyToggled)
		{
			this->m_iHotKeyToggled = true;
			this->m_Catcher->setSelectionMode(true);
		}
	});

	connect(this->m_HkFilter, &WindowsHotKeyFilter::HotKey_Up,[this](){
		if(m_iHotKeyToggled)
		{
			this->m_iHotKeyToggled = false;
			this->m_Catcher->setSelectionMode(false);
		}
	});

	m_Settings = new QSettings(QDir::toNativeSeparators(QCoreApplication::applicationDirPath())+QDir::separator()+SETTINGS_ENTRY_NAME,QSettings::IniFormat);
	m_Settings->beginGroup("MAIN");
	QApplication::processEvents();

	ui->setupUi(this);
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

	setHotKey(hotKey{ VK_NUMLOCK, VK_SCROLL });

	ui->StopButton->setEnabled(false);
	ui->plainTextEdit->hide();

	bool AutoStart = m_Settings->value(AUTO_START_WHEN_LAUNCH,NO).toBool();
	if(AutoStart == YES)
	{
		ui->StartPutton->click();
	}
	ui->AutoStartWhenLaunch->setChecked(AutoStart);

	ui->doubleSpinBox->setValue(m_Settings->value(BLOCK_DELAY,BLOCK_DELAY_DEFAULT).toDouble());
	m_Catcher->setDelay(ui->doubleSpinBox->value()*1000);

	ui->comboBox->setCurrentText(m_Settings->value(LOGGING_ENABLE,LOGGING_ENABLED_NO).toString());
	on_comboBox_currentIndexChanged(ui->comboBox->currentText());
	ui->LaunchWithOS->setChecked(m_Settings->value(LAUNCH_WITH_OS,NO).toBool());
	ui->StartMinimized->setChecked(m_Settings->value(LAUNCH_MINIMIZED,NO).toBool());

	m_ClickLockSettings.bHotKeyEnabled = m_Settings->value(CLICK_LOCK_HOTKEY_ENABLED, CLICK_LOCK_HOTKEY_ENABLED_DEFAULT).toBool();
	m_HkFilter->setDisabled(!this->m_ClickLockSettings.bHotKeyEnabled);

	m_ClickLockSettings.hkKey.fromQString(m_Settings->value(CLICK_LOCK_HOTKEY, CLICK_LOCK_HOTKEY_DEFAULT).toString());

	m_ClickLockSettings.bMouseHoldingEnabled = m_Settings->value(CLICK_CLOCK_MOUSE_ENABLED, CLICK_CLOCK_MOUSE_ENABLED_DEFAULT).toBool();
	m_ClickLockSettings.dMouseHoldingDelay = m_Settings->value(CLICK_LOCK_MOUSE_DELAY, CLICK_LOCK_MOUSE_DELAY_DEFAULT).toDouble();

	m_Settings->endGroup();

	m_Catcher->start(QThread::TimeCriticalPriority);
	UpdateClickLockSettings();

	QIcon theIcon = QIcon(":/icon.ico");

	setWindowIcon(theIcon);

	trayIcon = new QSystemTrayIcon(theIcon,this);
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			  this, SLOT(slotIconActivated(QSystemTrayIcon::ActivationReason)));


	QMenu *menu = new QMenu();

	QAction *action = new QAction(QIcon(":/restore.ico"),"Restore",this);
	connect(action,&QAction::triggered,[this](){
		this->slotIconActivated(QSystemTrayIcon::Trigger);
	});
	menu->addAction(action);

	QAction *action2 =  	new QAction(QIcon(":/quit.ico"),"Quit", this);
	connect(action2,SIGNAL(triggered(bool)),qApp,SLOT(quit()));

	menu->addAction(action2);

	trayIcon->setContextMenu(menu);

	trayIcon->show();

	//Some magic for __TIMESTAMP__
	ui->BuildDateLabel->setText((QString)"Build on: " + __TIMESTAMP__);
	srand(time(NULL));
	int TIME_IN_MSECS = rand()%10000+10000;

	QTimer::singleShot(TIME_IN_MSECS-1000, this, [this](){
		this->ui->BuildDateLabel->hide();
		this->adjustSize();
		this->ui->BuildDateLabel->deleteLater();
	});
	QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(ui->BuildDateLabel);
	ui->BuildDateLabel->setGraphicsEffect(effect);
	QPropertyAnimation *anim = new QPropertyAnimation(effect,"opacity");
	anim->setStartValue(1.0);
	anim->setEndValue(0.01);
	anim->setDuration(TIME_IN_MSECS);
	anim->start();
	this->repaint();
}

MainWindow::~MainWindow()
{
	m_Catcher->exit();
	m_Catcher->deleteLater();
	QApplication::processEvents();
	m_Settings->beginGroup("MAIN");
	m_Settings->setValue(AUTO_START_WHEN_LAUNCH,ui->AutoStartWhenLaunch->isChecked());
	m_Settings->setValue(BLOCK_DELAY,ui->doubleSpinBox->value());
	m_Settings->setValue(LOGGING_ENABLE, ui->comboBox->currentText());
	m_Settings->setValue(LAUNCH_WITH_OS,ui->LaunchWithOS->isChecked());
	m_Settings->setValue(LAUNCH_MINIMIZED,ui->StartMinimized->isChecked());
	m_Settings->setValue(CLICK_LOCK_HOTKEY_ENABLED, this->m_ClickLockSettings.bHotKeyEnabled);
	m_Settings->setValue(CLICK_LOCK_HOTKEY, this->m_ClickLockSettings.hkKey.toQString());
	m_Settings->setValue(CLICK_CLOCK_MOUSE_ENABLED, this->m_ClickLockSettings.bMouseHoldingEnabled);
	m_Settings->setValue(CLICK_LOCK_MOUSE_DELAY, this->m_ClickLockSettings.dMouseHoldingDelay);
	m_Settings->endGroup();
	QSettings bootUpSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

	if (ui->LaunchWithOS->isChecked())
	{
		bootUpSettings.remove("DoubleClickFixer");
		bootUpSettings.setValue("DoubleClickFixer","\""+QDir::toNativeSeparators(QCoreApplication::applicationFilePath())+ (ui->StartMinimized->isChecked() ? "\" -m" : "\""));
	}
	else
		bootUpSettings.remove("DoubleClickFixer");
	delete m_Settings;


	delete ui;
}
// TODO: This code is not working. Fix it.
void MainWindow::changeEvent(QEvent *e)
{
	if(e->type() == QEvent::WindowStateChange)
	{
		if(isMinimizing())
		{
			if( !( this->windowState() & Qt::WindowMinimized ) )
			{
				this->showMinimized();
				this->hide();
			}
		}
		else
		{
			Qt::WindowStates thisState = this->windowState();
			if(thisState & Qt::WindowMinimized)
			{
				setMinimizing(true);
				trayIcon->showMessage("DoubleClickFix", "DoubleClickFix is minimized to system tray", QSystemTrayIcon::MessageIcon(), 10000);
				QTimer::singleShot(2500,this, [=](){
					hide();
					setMinimizing(false);
				});
			}
		}
	}
	e->accept();
}


void MainWindow::on_StartPutton_clicked()
{
	m_Catcher->setStarted(true);
	ui->StopButton->setEnabled(true);
	ui->StartPutton->setEnabled(false);
}

void MainWindow::on_StopButton_clicked()
{
	m_Catcher->setStarted(false);
	ui->StopButton->setEnabled(false);
	ui->StartPutton->setEnabled(true);
}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
	m_Catcher->setDelay(int(arg1 * 1000.0));
}


void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
	m_TotalClicks=0;
	m_BlockedClicks=0;
	ui->plainTextEdit->clear();
	ui->label_2->clear();
	if(arg1=="On")
	{
		ui->label_2->show();
		ui->plainTextEdit->show();
		m_Catcher->setLoggingStarted(true);
	}
	else if(arg1=="Off")
	{

		ui->plainTextEdit->hide();
		m_Catcher->setLoggingStarted(false);
		ui->label_2->hide();
	}
	this->adjustSize();
}

void MainWindow::slotProcessLogInformation(int msec, bool blocked, bool isRight)
{
	double seconds = double(msec)/1000.0;

	ui->plainTextEdit->moveCursor (QTextCursor::End);
	ui->plainTextEdit->insertPlainText (QDateTime::currentDateTime().toString() + " : "+(isRight?"Right":"Left")+" click" + (blocked?" blocked ":" ") + QString::number(seconds) + " delay\n");
	ui->plainTextEdit->moveCursor (QTextCursor::End);

	if(blocked) m_BlockedClicks++;
	m_TotalClicks++;

	ui->label_2->setText("Total: "+QString::number(m_TotalClicks)+" Blocked: "+QString::number(m_BlockedClicks));
}

void MainWindow::on_StartMinimized_clicked()
{

}

void MainWindow::slotIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch(reason)
	{
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
		case QSystemTrayIcon::MiddleClick:
		{
			this->showNormal();
			this->activateWindow();
			this->raise();
			this->setFocus();
			QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon();
			trayIcon->showMessage("DoubleClickFix", "Enjoy the interface!", icon, 10000);
			break;
		}
	}
}
bool inline MainWindow::isMinimizing() const
{
	return m_isMinimizing;
}

void inline MainWindow::setMinimizing(bool value)
{
	m_isMinimizing = value;
}

hotKey inline MainWindow::getHotKey() const
{
	return m_ClickLockSettings.hkKey;
}

void inline MainWindow::setHotKey(hotKey &value)
{
	m_ClickLockSettings.hkKey = value;
}

void MainWindow::UpdateClickLockSettings()
{
	this->m_HkFilter->setDisabled(!m_ClickLockSettings.bHotKeyEnabled);
	this->setHotKey(m_ClickLockSettings.hkKey);
	this->m_Catcher->setMouseHoldAutoSelectionMode(m_ClickLockSettings.bMouseHoldingEnabled);
	this->m_Catcher->setMouseHoldingDelay(m_ClickLockSettings.dMouseHoldingDelay * 1000.0);
}
/*

bool WindowsHotKeyFilter::hotKeyFilter(const QByteArray &eventType, void *message, long *)
{
	MSG *msg = static_cast< MSG * >( message );
	switch(msg->message)
	{
		case WM_HOTKEY:
		{
			hotKey hk = m_Overlord->getHotKey();
			INPUT input;
			input.type = INPUT_KEYBOARD;
			memset(&input.ki, 0, sizeof(KEYBDINPUT));
			input.ki.wVk = hk.first;

			UnregisterHotKey(NULL, HOTKEY_ID);

			// input.ki.dwFlags = 0; // No flags = keyDown

			SendInput(1, &input, sizeof(INPUT));
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &input, sizeof(INPUT));
			RegisterHotKey(NULL, 1, 0, hk.first);

			qDebug () << "Press CAPS";
			break;
		}
	}

	return false;
}
*/

inline LRESULT WindowsHotKeyFilter::True_LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode ==HC_ACTION && !isDisabled())
	{
		UINT iCode = ( (KBDLLHOOKSTRUCT*)(lParam) )->vkCode;
		hotKey hkKey = m_Overlord->getHotKey();
		int iSize = hkKey.size();

		const enum EVENT_TYPE
		{
			KEY_DOWN,
			KEY_UP,
			NON_HANDLED,
		};

		const auto lambda = [](WPARAM &wParam) -> EVENT_TYPE{
			switch(wParam)
			{
				case WM_KEYDOWN: case WM_SYSKEYDOWN:
				{
					return KEY_DOWN;
				}
				case WM_SYSKEYUP: case WM_KEYUP:
				{
					return KEY_UP;
				}

			}
			return NON_HANDLED;
		};

		if(iSize > 0)
		{
			if ( (iSize > 1) && hkKey.find(iCode)!=hkKey.end() )
			{
				static QSet<UINT> downKeys;

				switch(lambda(wParam))
				{
					case KEY_DOWN:
					{
						downKeys.insert(iCode);
						if(downKeys.size() == iSize)
						{
							emit HotKey_Down();
						}
						break;
					}
					case KEY_UP:
					{
						if(downKeys.size() == iSize)
						{
							emit HotKey_Up();
						}
						downKeys.remove(iCode);
						break;
					}
				}
			}
			/* iSize == 1 */
			else if( ( iCode == *hkKey.begin() ) )
			{
				switch(lambda(wParam))
				{
					case KEY_DOWN:
					{
						emit HotKey_Down();
						break;
					}
					case KEY_UP:
					{
						emit HotKey_Up();
						break;
					}
				}
			}

		}
	}
	return CallNextHookEx(m_Hook, nCode, wParam, lParam);
}

inline bool WindowsHotKeyFilter::isDisabled() const
{
	return m_iDisabled;
}

void WindowsHotKeyFilter::setDisabled(bool iDisabled)
{
	m_iDisabled = iDisabled;
}
WindowsHotKeyFilter *WindowsHotKeyFilter::m_This;
HHOOK WindowsHotKeyFilter::m_Hook;
WindowsHotKeyFilter *WindowsHotKeyFilter::getInstance(MainWindow *Overlord)
{
	if(!m_This)
		m_This = new WindowsHotKeyFilter(Overlord);

	if(!m_Hook)
		m_Hook = SetWindowsHookEx(WH_KEYBOARD_LL,WindowsHotKey_LowLevelKeyboardProc, 0, 0);

	return m_This;
}

LRESULT CALLBACK WindowsHotKeyFilter::WindowsHotKey_LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return m_This->True_LowLevelKeyboardProc(nCode, wParam, lParam);
}

void MainWindow::on_ClickLockSettingsButton_clicked()
{
	click_lock_settings* ClickLockSettingsWindow =
			new click_lock_settings(this, m_ClickLockSettings);

	connect(ClickLockSettingsWindow, &click_lock_settings::settingsSaved,[this](click_lock_settings_data &data){
		m_ClickLockSettings = data;
		UpdateClickLockSettings();
	});

	ClickLockSettingsWindow->showModal();
	ClickLockSettingsWindow->deleteLater();
}
