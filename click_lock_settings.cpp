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

#include "click_lock_settings.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++" // TODO: Refactor
#include <QEventLoop>
#include <QMessageBox>
#include <QTimer>
#include "ui_click_lock_settings.h"
#pragma GCC diagnostic pop

click_lock_settings::click_lock_settings(QWidget *parent, click_lock_settings_data &data) :
	QWidget(parent, Qt::Window | Qt:: Dialog),
	ui(new Ui::click_lock_settings),
	m_Data(),
	m_UpdatedData(),
	m_bChanged(false),
	m_KeyEditingState(0)
{
	ui->setupUi(this);

	m_Data = m_UpdatedData = data;

	ui->HoldingDelay->setValue(data.dMouseHoldingDelay);

	ui->MouseHoldActivate->setChecked(data.bMouseHoldingEnabled);
	UpdateHotKey();
	ui->HotKeyEnabled->setChecked(data.bHotKeyEnabled);


	connect(this, &click_lock_settings::settingsChanged, [this](){
		this->ui->OKButton->setEnabled(true);
		this->ui->ApplyButton->setEnabled(true);
	});

	connect(this, &click_lock_settings::settingsRestored, [this](){
		this->ui->OKButton->setEnabled(false);
		this->ui->ApplyButton->setEnabled(false);
	});


	connect(this->ui->OKButton, &QPushButton::clicked, [this](){
		if(m_KeyEditingState == true) return;
		ui->ApplyButton->click();
		this->close();
	});

	connect(this->ui->ApplyButton, &QPushButton::clicked, [this](){
		if(m_KeyEditingState == true) return;
		m_Data = m_UpdatedData;
		CheckState();
		emit settingsSaved( m_UpdatedData );
	});

	connect(this->ui->CancelButton, &QPushButton::clicked, [this](){
		this->close();
	});

	QWidgetInputEventCatcher *icCatcher = new QWidgetInputEventCatcher(ui->HotKey);

	connect(icCatcher, &QWidgetInputEventCatcher::mouseClicked, [this](){
		if(m_KeyEditingState == true) return;
		QMessageBox::information(this, "Hot Key Changing",
										 "You have 3 seconds to change hot key");
		m_KeyEditingState = 1;

		m_UpdatedData.hkKey.clear();

		UpdateHotKey();

		QTimer* timer = new QTimer(this);
		timer->setInterval(3000);
		timer->setTimerType(Qt::CoarseTimer);
		timer->setSingleShot(true);

		connect(this, SIGNAL(closed()), timer, SLOT(stop()));
		connect(this, SIGNAL(closed()), timer, SLOT(deleteLater()));



		timer->start();
		connect(timer, &QTimer::timeout, [this](){
			m_KeyEditingState = 0;
			if(!m_UpdatedData.hkKey.empty())
			{
				CheckState();
				QMessageBox::information(this, "Hot Key Changing", "Hot Key Changed");
			}
			else
			{
				m_UpdatedData.hkKey = m_Data.hkKey;
				CheckState();
				UpdateHotKey();
				QMessageBox::information(this, "Hot Key Changing", "Hot Key Empty!");
			}


		});
	});

	connect(icCatcher, &QWidgetInputEventCatcher::virtualKeyPress, [this](UINT virtualKey){
		if(m_KeyEditingState == true)
		{
			m_UpdatedData.hkKey.insert(virtualKey);
			UpdateHotKey();
		}
	});

	connect(icCatcher, &QWidgetInputEventCatcher::virtualKeyRelease, [this](UINT virtualKey){
		if(m_KeyEditingState == true)
		{
			m_UpdatedData.hkKey.remove(virtualKey);
			UpdateHotKey();
		}
	});
	m_bChanged = false;
	CheckState();

}

click_lock_settings::~click_lock_settings()
{
	delete ui;
}

void click_lock_settings::showModal()
{
	auto old = this->windowModality();
	this->setWindowModality(Qt::WindowModal);
	this->show();

	QEventLoop loop;
	connect(this, &click_lock_settings::closed, [&loop](){
		loop.exit();
	});
	loop.exec();
	this->setWindowModality(old);
}

void click_lock_settings::closeEvent(QCloseEvent *event)
{
	emit closed();
	event->accept();
}




QString click_lock_settings::KeyCodesToQString(const QSet<UINT> &qCodes)
{

	auto VirtualKeyCodeToStdString = [] (UCHAR virtualKey) -> std::basic_string<TCHAR>
	{
		UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

		TCHAR szName[128];
		int result = 0;
		switch (virtualKey)
		{
			case VK_PAUSE:
				wsprintf(szName, L"Pause Break");
				result = 1;
				break;
			case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
			case VK_PRIOR: case VK_NEXT: // page up and page down
			case VK_END: case VK_HOME:
			case VK_INSERT: case VK_DELETE:
			case VK_DIVIDE: // numpad slash
			case VK_NUMLOCK:
				scanCode |= 0x100;
			default:
				result = GetKeyNameText(scanCode << 16, szName, 128);
		}
		if(result == 0)
			return (L"[ERROR]");
		/*throw std::system_error(std::error_code(GetLastError(), std::system_category()),
									"WinAPI Error occured.");*/
		return szName;
	};

	QString szReturn;
	for(UINT virtualKey : qCodes)
	{
		szReturn+=QString::fromWCharArray(VirtualKeyCodeToStdString(virtualKey).c_str())+" + ";
	}
	return szReturn.left(szReturn.size()-2);
}

void click_lock_settings::CheckState()
{
	if(m_UpdatedData != m_Data)
	{
		if(!m_bChanged)
		{
			m_bChanged = true;
			emit settingsChanged();
		}
	}
	else if(m_bChanged)
	{
		m_bChanged = false;
		emit settingsRestored();
	}
}

void click_lock_settings::UpdateHotKey()
{
	ui->HotKey->setText(KeyCodesToQString(m_UpdatedData.hkKey.toSet()));
}

void click_lock_settings::on_HotKeyEnabled_clicked()
{
	m_UpdatedData.bHotKeyEnabled = ui->HotKeyEnabled->isChecked();
	CheckState();
}

void click_lock_settings::on_HoldingDelay_valueChanged(double arg1)
{
	m_UpdatedData.dMouseHoldingDelay = arg1;
	CheckState();
}

void click_lock_settings::on_MouseHoldActivate_clicked()
{
	m_UpdatedData.bMouseHoldingEnabled = ui->MouseHoldActivate->isChecked();
	CheckState();
}
