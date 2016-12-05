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
#include <QApplication>


int main(int argc, char *argv[])
{
	QCoreApplication::setApplicationName("Double Click Fixer");
	QApplication *a = new QApplication(argc, argv);
	MainWindow *w = new MainWindow();

	bool iShowMinimized = false;
	if(argc > 1)
	{
		for(int i = 1; i<argc;++i)
		{
			if(strcmp(argv[i], "-m")==0)
			{
				iShowMinimized = true;
				break;
			}
		}
	}
	if(iShowMinimized)
		w->showMinimized();
	else
		w->show();

	int iReturn = a->exec();

	delete w;
	delete a;

	return iReturn;
}
