#include "PreviewScene.h"
#include <QDebug>
#include <QLinearGradient>
PreviewScene::PreviewScene(QObject *parent)
    : QGraphicsScene(parent)
{
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &PreviewScene::animationTimerTick);
    m_animationTimer->start(16); // ~60fps
    
    initGraphics();
}
PreviewScene::~PreviewScene()
{
}
void PreviewScene::initGraphics()
{
    setSceneRect(0, 0, 400, 200);
    
    // Chunk flow lane
    for (int i = 0; i < 20; ++i) { // Example 20 chunks
        QGraphicsRectItem *chunk = addRect(0, 50 + i*5, 20, 4, QPen(Qt::darkGray), QBrush(Qt::lightGray));
        m_chunkItems.append(chunk);
    }
    
    // Speed graph area
    m_speedLine = addPath(QPainterPath(), QPen(Qt::blue, 2));
    
    m_etaText = addText("ETA: --");
    m_etaText->setPos(10, 10);
    m_etaText->setFont(QFont("Arial", 12, QFont::Bold));
    
    m_queueText = addText("Queue: 0/0");
    m_queueText->setPos(200, 10);
}
void PreviewScene::updateChunkFlow(int chunkIndex, int totalChunks, double progress)
{
    // Animate chunk flow
    double flowPos = (chunkIndex / double(totalChunks)) * sceneRect().width();
    for (auto chunk : m_chunkItems) {
        chunk->setBrush(progress > 0.5 ? QBrush(Qt::green) : QBrush(Qt::yellow));
        chunk->setX(fmod(flowPos + chunk->y() * 10, sceneRect().width()));
    }
    updateAnimation();
}
void PreviewScene::updateSpeedGraph(double currentSpeed)
{
    m_speedHistory.append(currentSpeed);
    if (m_speedHistory.size() > m_historySize) {
        m_speedHistory.removeFirst();
    }
    
    QPainterPath path;
    for (int i = 0; i < m_speedHistory.size(); ++i) {
        double x = (i / double(m_historySize)) * 380 + 10;
        double y = 180 - (m_speedHistory[i] / 100.0 * 80); // Normalize
        if (i == 0) path.moveTo(x, y);
        else path.lineTo(x, y);
    }
    m_speedLine->setPath(path);
}
void PreviewScene::updateETA(const QString &eta)
{
    m_etaText->setPlainText(QString("ETA: %1").arg(eta));
}
void PreviewScene::setQueueInfo(int queueSize, int active)
{
    m_queueText->setPlainText(QString("Queue: %1 active: %2").arg(queueSize).arg(active));
}
void PreviewScene::animationTimerTick()
{
    updateAnimation();
}
void PreviewScene::updateAnimation()
{
    // Continuous subtle animation
    // ...
}
PreviewView::PreviewView(PreviewScene *scene, QWidget *parent)
    : QGraphicsView(parent)
{
    setScene(scene);
    setFixedSize(420, 220);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
