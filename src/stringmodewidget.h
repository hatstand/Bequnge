//
// C++ Interface: stringmodewidget
//
// Description: 
//
//
// Author: David Sansome <david@hoover>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef STRINGMODEWIDGET_H
#define STRINGMODEWIDGET_H

#include <QLabel>

class QPaintEvent;
class QMouseEvent;

class StringModeWidget : public QLabel
{
	Q_OBJECT
	
public:
	StringModeWidget(QWidget* parent);	
	~StringModeWidget();
	
	bool getStringMode();

public slots:
	void setStringMode(bool enabled);
	
signals:
	void stringModeChanged(bool enabled);
	
private:
	void mousePressEvent(QMouseEvent* event);
	
private:
	bool m_enabled;
};

#endif
