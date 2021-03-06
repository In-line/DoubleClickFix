﻿/*  	 Copyright (C) 2016 Alik Aslanyan <cplusplus256@gmail.com>
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


#ifndef CLICK_LOCK_SETTINGS_H
#define CLICK_LOCK_SETTINGS_H


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++" // TODO: Refactor
#include <QWidget>
#include <QCloseEvent>
#pragma GCC diagnostic pop

namespace Ui {
class click_lock_settings;
}
struct click_lock_settings_data;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++" // TODO: Refactor
#include <QLineEdit>
#pragma GCC diagnostic pop

#include "widget_input_event_catcher.h"
#include "hotkey.h"
struct click_lock_settings_data
{
	double dMouseHoldingDelay;
	hotKey hkKey;
	bool bHotKeyEnabled;
	bool bMouseHoldingEnabled;

	click_lock_settings_data() : dMouseHoldingDelay(0.010), hkKey(),
		bHotKeyEnabled(false), bMouseHoldingEnabled(false) {}
	bool operator!=(click_lock_settings_data &o)
	{
		auto a = std::tie(dMouseHoldingDelay, hkKey, bHotKeyEnabled, bMouseHoldingEnabled);
		auto b = std::tie(o.dMouseHoldingDelay, o.hkKey, o.bHotKeyEnabled, o.bMouseHoldingEnabled);
		return a != b;

	}
};

#include <tchar.h>

class click_lock_settings : public QWidget
{
	Q_OBJECT

public:
	explicit click_lock_settings(QWidget *parent, click_lock_settings_data &data);
	click_lock_settings(const click_lock_settings&) = delete;
	click_lock_settings* operator =(const click_lock_settings&) = delete;
	~click_lock_settings();
	void showModal();

private:
	void closeEvent (QCloseEvent *event);
	QString KeyCodesToQString(const QSet<UINT> &qCodes);
	Ui::click_lock_settings *ui;
	click_lock_settings_data m_Data;
	click_lock_settings_data m_UpdatedData;
	bool m_bChanged;
	bool m_KeyEditingState;
	void CheckState();
	void UpdateHotKey();
signals:
	void closed();
	void settingsChanged();
	void settingsRestored();
	void settingsSaved(click_lock_settings_data &newData);
private slots:
	void on_HotKeyEnabled_clicked();
	void on_HoldingDelay_valueChanged(double arg1);
	void on_MouseHoldActivate_clicked();
};

#endif // CLICK_LOCK_SETTINGS_H
