#ifndef GLOBALS_H
#define GLOBALS_H

#include <QObject>
#include <QSharedDataPointer>

class GlobalsData;

class Globals : public QObject
{
    Q_OBJECT
public:
    explicit Globals(QObject *parent = nullptr);
    Globals(const Globals &);
    Globals &operator=(const Globals &);
    ~Globals();

    QString getVersion();
    bool isRV();

signals:

public slots:

private:
    QSharedDataPointer<GlobalsData> data;
};

#endif // GLOBALS_H
