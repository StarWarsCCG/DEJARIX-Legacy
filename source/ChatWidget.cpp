#include "ChatWidget.hpp"
#include <QDebug>

void ChatWidget::append(const QString& text, const QColor& color)
{
    _text.setTextColor(color);
    _text.append(text.toHtmlEscaped());
}

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , _sendButton("Send")
{
    _text.setTextColor(QColor(255, 0, 0));
    QPalette p = _text.palette();
    p.setColor(QPalette::Base, QColor(0, 0, 0));
    _text.setPalette(p);

    _lineLayout.addWidget(&_line);
    _lineLayout.addWidget(&_sendButton);

    _layout.addWidget(&_text);
    _layout.addLayout(&_lineLayout);
    setLayout(&_layout);

    connect(&_line, SIGNAL(returnPressed()), this, SLOT(pressReturn()));
    connect(&_sendButton, SIGNAL(clicked(bool)), this, SLOT(sendChat()));
}

void ChatWidget::pressReturn()
{
    if (_line.text().size() > 0)
        sendChat();
    else
        clearFocus(), qDebug("hi");
}

void ChatWidget::sendChat()
{
    _text.append(_line.text().toHtmlEscaped());
    _line.clear();
    _text.setTextColor(QColor(0, 255, 0));
}

