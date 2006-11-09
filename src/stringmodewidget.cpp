#include "stringmodewidget.h"

#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>

StringModeWidget::StringModeWidget(QWidget* parent)
	: QLabel(parent)
{
	setToolTip("When the editor is in string mode, special characters such as > and < can be inserted without changing the direction of the cursor.  String mode can be toggled by typing \".");
	setStringMode(false);
	setAutoFillBackground(true);
}


StringModeWidget::~StringModeWidget()
{
}


bool StringModeWidget::getStringMode()
{
	return m_enabled;
}

void StringModeWidget::setStringMode(bool enabled)
{
	m_enabled = enabled;
	if (enabled)
	{
		setText("String mode is enabled");
		setBackgroundRole(QPalette::Highlight);
		setForegroundRole(QPalette::HighlightedText);
	}
	else
	{
		setText("String mode is disabled");
		setBackgroundRole(QPalette::NoRole);
		setForegroundRole(QPalette::WindowText);
	}
	update();
}

void StringModeWidget::mousePressEvent(QMouseEvent* event)
{
	m_enabled = !m_enabled;
	emit stringModeChanged(m_enabled);
}

