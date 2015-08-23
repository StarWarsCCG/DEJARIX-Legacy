#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>

class ChatWidget : public QWidget
{
    Q_OBJECT

    QVBoxLayout _layout;
    QHBoxLayout _lineLayout;
    QTextBrowser _text;
    QLineEdit _line;
    QPushButton _sendButton;

    void append(const QString& text, const QColor& color);

public:
    explicit ChatWidget(QWidget* parent = nullptr);

    inline void focusChat() { _line.setFocus(); }

signals:

private slots:
    void pressReturn();
    void sendChat();
};

#endif
