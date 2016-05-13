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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <mousecatcherthread.h>
#include <QTimer>
#include <QMenu>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <ctime>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    m_Settings = new QSettings(SETTINGS_ENTRY_NAME,QSettings::IniFormat);
    m_Settings->beginGroup("MAIN");
    QApplication::processEvents();
    m_Catcher = MouseCatcherThread::getInstance();
    connect(m_Catcher,SIGNAL(delay(int,bool,bool)),this,SLOT(slotProcessLogInformation(int,bool,bool)),Qt::QueuedConnection);

    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);



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
    m_Settings->endGroup();

    m_Catcher->start(QThread::TimeCriticalPriority);


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

    QAction *action2 =  new QAction(QIcon(":/quit.ico"),"Quit", this);
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
    m_Settings->endGroup();
    QSettings bootUpSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    if (ui->LaunchWithOS->isChecked())
    {
        bootUpSettings.remove("DoubleClickFixer");
        bootUpSettings.setValue("DoubleClickFixer","\""+QDir::toNativeSeparators(QCoreApplication::applicationFilePath())+ (ui->StartMinimized->isChecked() ? "\" /MINIMIZED" : "\""));
    }
    else
        bootUpSettings.remove("DoubleClickFixer");
    delete m_Settings;


    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
        if(e->type()==QEvent::WindowStateChange)
        {
            if(isMinimizing())
            {
                this->showMinimized();
                this->hide();
                e->ignore();
            }
            else
            {
                Qt::WindowStates thisState = this->windowState();
                if(thisState & Qt::WindowMinimized)
                {
                    setMinimizing(true);
                    trayIcon->showMessage("DoubleClickFix", "DoubleClickFix is minimized to system tray", QSystemTrayIcon::MessageIcon(), 10000);
                    QTimer::singleShot(250,this, [=](){
                        hide();
                        setMinimizing(false);
                    });
                }
            }
        }
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

