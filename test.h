#ifndef TEST_H
#define TEST_H

#include <QWidget>

class test : public QWidget
{
    Q_OBJECT
public:
    explicit test(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // TEST_H