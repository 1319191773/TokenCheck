#ifndef TOAST_H
#define TOAST_H

#include <QWidget>

class QLabel;
class QPropertyAnimation;

class Toast : public QWidget
{
    Q_OBJECT
public:
    static void show(QWidget *parent, const QString &message, bool isError = false, int durationMs = 2000);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    explicit Toast(QWidget *parent, const QString &message, bool isError, int durationMs);
    QLabel *m_label;
    bool m_isError;
};

#endif // TOAST_H
