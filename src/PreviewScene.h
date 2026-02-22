#ifndef PREVIEWSCENE_H
#define PREVIEWSCENE_H
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QTimer>
#include <QVector>
#include <QPointF>
#include <QRectF>
#include <QPainter>
#include <QGraphicsTextItem>
class PreviewScene : public QGraphicsScene
{
    Q_OBJECT
public:
    PreviewScene(QObject *parent = nullptr);
    ~PreviewScene();
    void updateChunkFlow(int chunkIndex, int totalChunks, double progress);
    void updateSpeedGraph(double currentSpeed);
    void updateETA(const QString &eta);
    void setQueueInfo(int queueSize, int active);
private slots:
    void animationTimerTick();
private:
    QTimer *m_animationTimer;
    QVector<QGraphicsRectItem*> m_chunkItems;
    QGraphicsTextItem *m_etaText;
    QGraphicsTextItem *m_queueText;
    
    // Speed graph
    QVector<double> m_speedHistory;
    int m_historySize = 60; // 60fps, 1 sec
    QGraphicsPathItem *m_speedLine;
    
    void initGraphics();
    void updateAnimation();
};
class PreviewView : public QGraphicsView
{
public:
    PreviewView(PreviewScene *scene, QWidget *parent = nullptr);
};
#endif // PREVIEWSCENE_H
