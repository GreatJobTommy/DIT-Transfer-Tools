#include &quot;ParallelManager.h&quot;
#include &lt;QDebug&gt;
#include &lt;QThread&gt;

ParallelManager::ParallelManager(QObject *parent)
    : QObject(parent), m_maxThreads(4), m_paused(false)
{
    m_threadPool.setMaxThreadCount(m_maxThreads);
    // Connect if needed for task finished signal
}

ParallelManager::~ParallelManager() {
    pause();
    m_threadPool.waitForDone();
}

void ParallelManager::enqueueTask(TransferTask* task, int priority) {
    QMutexLocker lock(&amp;m_mutex);
    m_taskQueue.push({task, priority});
    if (!m_paused) startNextTask();
}

void ParallelManager::setMaxThreads(int threads) {
    QMutexLocker lock(&amp;m_mutex);
    m_maxThreads = qMax(1, qMin(32, threads));
    m_threadPool.setMaxThreadCount(m_maxThreads);
    loadBalance();
}

int ParallelManager::maxThreads() const {
    return m_maxThreads;
}

void ParallelManager::pause() {
    QMutexLocker lock(&amp;m_mutex);
    m_paused = true;
}

void ParallelManager::resume() {
    QMutexLocker lock(&amp;m_mutex);
    m_paused = false;
    startNextTask();
}

int ParallelManager::activeThreads() const {
    return m_threadPool.activeThreadCount();
}

void ParallelManager::startNextTask() {
    QMutexLocker lock(&amp;m_mutex);
    while (!m_taskQueue.empty() &amp;&amp; m_activeTasks.size() &lt; static_cast&lt;size_t&gt;(m_maxThreads) &amp;&amp; !m_paused) {
        auto entry = m_taskQueue.top(); m_taskQueue.pop();
        m_activeTasks.append(entry.task);
        emit taskStarted(entry.task);
        m_threadPool.start(entry.task); // Assume TransferTask is QRunnable
    }
}

void ParallelManager::onTaskFinished() {
    QMutexLocker lock(&amp;m_mutex);
    // Note: This slot assumes connection to thread pool or task signal
    for (int i = m_activeTasks.size() - 1; i &gt;= 0; --i) {
        TransferTask* task = m_activeTasks[i];
        if (task-&gt;isFinished()) { // Assume method
            m_activeTasks.removeAt(i);
            emit taskFinished(task, task-&gt;success());
            startNextTask();
            break;
        }
    }
}

void ParallelManager::loadBalance() {
    // Simple load balance: adjust based on CPU? Placeholder
    qDebug() &lt;&lt; &quot;Load balanced to&quot; &lt;&lt; m_maxThreads &lt;&lt; &quot;threads&quot;;
}
