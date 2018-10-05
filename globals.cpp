#include "globals.h"
#include "config.h"

class GlobalsData : public QSharedData
{
public:

};

Globals::Globals(QObject *parent) : QObject(parent), data(new GlobalsData)
{

}

Globals::Globals(const Globals &rhs) : data(rhs.data)
{

}

QString Globals::getVersion() {
    QString suffix = "";
    if (VERSION_IS_ALPHA)
        suffix = " ALPHA";
    if (VERSION_IS_BETA)
        suffix = " BETA";
    if (VERSION_IS_RV)
        suffix += " RV";

    return QString::number(VERSION_MAJOR) + "." + QString::number(VERSION_MINOR) + suffix;
}

bool Globals::isRV() {
    if (VERSION_IS_RV)
        return true;
    else
        return false;
}

Globals &Globals::operator=(const Globals &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

Globals::~Globals()
{

}
