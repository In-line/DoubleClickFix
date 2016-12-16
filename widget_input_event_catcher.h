#ifndef WIDGET_INPUT_EVENT_CATCHER_H
#define WIDGET_INPUT_EVENT_CATCHER_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++" // TODO: Refactor
#include <QWidget>
#include <QObject>
#include <QtGlobal>
#include <QMouseEvent>
#pragma GCC diagnostic pop

class QWidgetInputEventCatcher : public QWidget
{
	Q_OBJECT
public:
	QWidgetInputEventCatcher(QWidget *parent = nullptr) : QWidget(parent)
	{
		connect(this, &QWidgetInputEventCatcher::mousePress, [this](QMouseEvent *event){
			if(event->button() == Qt::LeftButton)
				emit mouseClicked();
		});
		connect(this, &QWidgetInputEventCatcher::keyPress, [this](QKeyEvent *event){
			emit virtualKeyPress(event->nativeVirtualKey());
		});

		connect(this, &QWidgetInputEventCatcher::keyRelease, [this](QKeyEvent *event){
			emit virtualKeyRelease(event->nativeVirtualKey());
		});

		setFocusPolicy(Qt::StrongFocus);
	}
	virtual ~QWidgetInputEventCatcher() { }

protected:
	virtual void resizeEvent(QResizeEvent*)
	{
		QWidget *parentPointer = dynamic_cast<QWidget*>(this->parent());
		if(parentPointer!=nullptr)
			this->setFixedSize(parentPointer->size());
	}

	virtual void mousePressEvent(QMouseEvent *event)
	{
		emit mousePress(event);
	}
	virtual void mouseReleaseEvent(QMouseEvent *event)
	{
		emit mouseRelease(event);
	}
	virtual void keyPressEvent(QKeyEvent *event)
	{
		emit keyPress(event);
	}
	virtual void keyReleaseEvent(QKeyEvent *event)
	{
		emit keyRelease(event);
	}

signals:
	void mousePress(QMouseEvent *event);
	void mouseRelease(QMouseEvent *event);
	void mouseClicked();

	void keyPress(QKeyEvent *event);
	void keyRelease(QKeyEvent *event);

	void virtualKeyPress(unsigned int virtualKey);
	void virtualKeyRelease(unsigned int virtualKey);
};

#endif // WIDGET_INPUT_EVENT_CATCHER_H
