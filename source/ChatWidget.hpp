#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QFocusEvent>
#include <random>

class ChatWidget : public QWidget
{
    Q_OBJECT

    QVBoxLayout _layout;
    QHBoxLayout _lineLayout;
    QTextBrowser _textBox;
    QLineEdit _lineEdit;
    QPushButton _sendButton;
    std::mt19937_64 _mt;
    std::uniform_int_distribution<int> _distribution;

    void append(QString text, QColor color);

public:
    explicit ChatWidget(QWidget* parent = nullptr);

    inline void focusChat() { _lineEdit.setFocus(); }

    virtual void focusInEvent(QFocusEvent* e);

signals:
    void emptyChatSent();

private slots:
    void sendChat();
    void onCardEvent(QString description);
};

#endif
