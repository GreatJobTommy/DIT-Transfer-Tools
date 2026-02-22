#ifndef PREVIEWSCENE_H
#define PREVIEWSCENE_H

#include &lt;QGraphicsScene&gt;
#include &lt;QGraphicsItem&gt;
#include &lt;QGraphicsView&gt;
#include &lt;QTimer&gt;
#include &lt;QVector&gt;
#include &lt;QPointF&gt;
#include &lt;QRectF&gt;
#include &lt;QPainter&gt;
#include &lt;QGraphicsTextItem&gt;

class PreviewScene : public QGraphicsScene
{
    Q_OBJECT

public:
    PreviewScene(QObject *parent = nullptr);
    ~PreviewScene();

    void updateChunkFlow(int chunkIndex, int totalChunks, double progress);
    void updateSpeedGraph(double currentSpeed);
    void updateETA(const QString &amp;eta);
    void setQueueInfo(int queueSize, int active);

private slots:
    void animationTimerTick();

private:
    QTimer *m_animationTimer;
    QVector&lt;QGraphicsRectItem*&gt; m_chunkItems;
    QGraphicsTextItem *m_etaText;
    QGraphicsTextItem *m_queueText;
    
    // Speed graph
    QVector&lt;double&gt; m_speedHistory;
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
