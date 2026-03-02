#ifndef SPEEDHISTORY_H
#define SPEEDHISTORY_H

#include <QObject>
#include <QList>
#include <QPointF>

class SpeedHistory : public QObject {
    Q_OBJECT

public:
    explicit SpeedHistory(QObject* parent = nullptr);
    ~SpeedHistory();

    void addSpeed(qreal speed); // speed in MB/s
    qreal averageSpeed() const;

private:
    QList<qreal> m_speedData;
    static const int MAX_POINTS = 100;
};

#endif // SPEEDHISTORY_H