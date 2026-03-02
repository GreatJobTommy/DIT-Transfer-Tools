#ifndef SPEEDHISTORY_H
#define SPEEDHISTORY_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

class SpeedHistory : public QObject {
    Q_OBJECT

public:
    explicit SpeedHistory(QObject* parent = nullptr);
    ~SpeedHistory();

    void addSpeed(qreal speed); // speed in MB/s
    QChart* createChart() const;
    qreal averageSpeed() const;

private:
    QList<QPointF> m_speedData;
    static const int MAX_POINTS = 100;
};

#endif // SPEEDHISTORY_H