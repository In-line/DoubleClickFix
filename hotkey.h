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


#ifndef HOTKEY_H
#define HOTKEY_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++" // TODO: Refactor
#include <QSet>
#pragma GCC diagnostic pop

#include <windows.h>
class hotKey : public QSet<UINT>
{

public:
	hotKey() : QSet<UINT>() {}
	template<typename INITIALIZER_TYPE>

	inline hotKey(const std::initializer_list<INITIALIZER_TYPE> &l) {
		for(const INITIALIZER_TYPE &value : l)
		{
			this->insert(UINT(value));
		}
	}
	inline hotKey(UINT singleKey) : QSet<UINT>() {
		this->insert(singleKey);
	}
	inline const QSet<UINT> toSet() {
		return static_cast<QSet<UINT>>(*this);
	}
	inline const QString toQString() {
		QString szReturn;
		for(UINT value : this->toSet())
		{
			szReturn+=QString::number(value)+" ";
		}
		return szReturn.left(szReturn.size()-1);
	}
	inline void fromQString(const QString &str)
	{
		this->clear();
		auto list = str.split(" ");
		for(auto value : list)
		{
			this->insert(value.toInt());
		}
	}
};
#endif // HOTKEY_H
