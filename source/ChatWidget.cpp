#include "ChatWidget.hpp"
#include <QDebug>

void ChatWidget::append(QString text, QColor color)
{
    _textBox.setTextColor(color);
    _textBox.append(text.toHtmlEscaped());
}

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , _sendButton("Send")
{
    _textBox.setTextColor(QColor(255, 0, 0));
    QPalette p = _textBox.palette();
    p.setColor(QPalette::Base, QColor(0, 0, 0));
    _textBox.setPalette(p);

    _lineLayout.addWidget(&_lineEdit);
    _lineLayout.addWidget(&_sendButton);

    _layout.addWidget(&_textBox);
    _layout.addLayout(&_lineLayout);
    setLayout(&_layout);

    connect(&_lineEdit, SIGNAL(returnPressed()), this, SLOT(sendChat()));
    connect(&_sendButton, SIGNAL(clicked(bool)), this, SLOT(sendChat()));
}

void ChatWidget::focusInEvent(QFocusEvent* e)
{
    Q_UNUSED(e);
}

void ChatWidget::sendChat()
{
    QString text = _lineEdit.text();

    if (text.length() > 0)
    {
        _textBox.append(text.toHtmlEscaped());
        _lineEdit.clear();
        _textBox.setTextColor(QColor(0, 255, 0));
    }
    else
    {
        emit emptyChatSent();
    }
}

