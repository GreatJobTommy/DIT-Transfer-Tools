#include "SpeedHistory.h"
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QDateTime>

SpeedHistory::SpeedHistory(QObject* parent)
    : QObject(parent) {
}

SpeedHistory::~SpeedHistory() {
}

void SpeedHistory::addSpeed(qreal speed) {
    QDateTime now = QDateTime::currentDateTime();
    m_speedData.append(QPointF(now.toMSecsSinceEpoch(), speed));
    if (m_speedData.size() > MAX_POINTS) {
        m_speedData.removeFirst();
    }
}

QChart* SpeedHistory::createChart() const {
    QChart* chart = new QChart();
    QLineSeries* series = new QLineSeries();
    series->setName("Transfer Speed (MB/s)");

    for (const QPointF& point : m_speedData) {
        series->append(point);
    }

    chart->addSeries(series);
    chart->setTitle("Speed History");

    QDateTimeAxis* axisX = new QDateTimeAxis();
    axisX->setFormat("hh:mm:ss");
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("Speed (MB/s)");
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    return chart;
}

qreal SpeedHistory::averageSpeed() const {
    if (m_speedData.isEmpty()) return 0.0;
    qreal sum = 0.0;
    for (const QPointF& point : m_speedData) {
        sum += point.y();
    }
    return sum / m_speedData.size();
}