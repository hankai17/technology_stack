#ifndef NEWSPAPER_H
#define NEWSPAPER_H

#include <QObject>

class Newspaper : public QObject
{
    Q_OBJECT
public:
signals:
    void hasNespaper();
    void hasNespaper1(QString name);
    void hasNespaper1(QString name, QString text);
public:
    explicit Newspaper(QObject *parent = 0)
        : QObject(parent) {}
    Newspaper(QString name, QObject *parent = 0)
        : QObject(parent) { m_name = name; }
    Newspaper(QString name, QString text, QObject *parent = 0)
        : QObject(parent) {m_name = name; m_text = text; }
    void send() {
        emit hasNespaper();
        emit hasNespaper1(m_name);
        emit hasNespaper1(m_name, m_text);
    }

public slots:
private:
    QString m_name;
    QString m_text;
};

#endif // NEWSPAPER_H
