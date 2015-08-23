#include "ChatWidget.hpp"
#include <QDebug>

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , _sendButton("Send")
{
    _lineLayout.addWidget(&_line);
    _lineLayout.addWidget(&_sendButton);

    _layout.addWidget(&_text);
    _layout.addLayout(&_lineLayout);
    setLayout(&_layout);

    connect(&_line, SIGNAL(returnPressed()), this, SLOT(sendChat()));
    connect(&_sendButton, SIGNAL(clicked(bool)), this, SLOT(sendChat()));
}

void ChatWidget::sendChat()
{
    _text.append(_line.text().toHtmlEscaped());
    _line.clear();
}

