#include "LogsDockWidget.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QScrollBar>
#include <QCheckBox>
#include <QGroupBox>
#include <QDebug>

LogsDockWidget::LogsDockWidget(QWidget* parent)
    : QDockWidget("Transfer Logs", parent),
      m_currentLevel(INFO),
      m_currentLevelFilter(DEBUG),
      m_autoScroll(true),
      m_lastLogCount(0) {
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    setupUI();

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &LogsDockWidget::updateDisplay);
    m_updateTimer->start(100); // Update every 100ms for tail -f effect
}

LogsDockWidget::~LogsDockWidget() {}

void LogsDockWidget::setupUI() {
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);

    // Filter controls
    QGroupBox* filterGroup = new QGroupBox("Filters");
    QHBoxLayout* filterLayout = new QHBoxLayout(filterGroup);

    QLabel* searchLabel = new QLabel("Search:");
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("Search logs...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &LogsDockWidget::onSearchTextChanged);

    QLabel* destLabel = new QLabel("Destination:");
    m_destFilter = new QComboBox;
    m_destFilter->addItem("All", "");
    connect(m_destFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LogsDockWidget::onDestinationFilterChanged);

    QLabel* levelLabel = new QLabel("Level:");
    m_levelFilter = new QComboBox;
    m_levelFilter->addItem("DEBUG", DEBUG);
    m_levelFilter->addItem("INFO", INFO);
    m_levelFilter->addItem("WARNING", WARNING);
    m_levelFilter->addItem("ERROR", ERROR);
    m_levelFilter->addItem("CRITICAL", CRITICAL);
    m_levelFilter->setCurrentIndex(DEBUG);
    connect(m_levelFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LogsDockWidget::onLevelFilterChanged);

    filterLayout->addWidget(searchLabel);
    filterLayout->addWidget(m_searchEdit);
    filterLayout->addWidget(destLabel);
    filterLayout->addWidget(m_destFilter);
    filterLayout->addWidget(levelLabel);
    filterLayout->addWidget(m_levelFilter);

    layout->addWidget(filterGroup);

    // Control buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;

    m_clearButton = new QPushButton("Clear");
    connect(m_clearButton, &QPushButton::clicked, this, &LogsDockWidget::onClearButtonClicked);

    m_autoScrollCheck = new QCheckBox("Auto Scroll");
    m_autoScrollCheck->setChecked(true);
    connect(m_autoScrollCheck, &QCheckBox::stateChanged, this, &LogsDockWidget::onAutoScrollChanged);

    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_autoScrollCheck);

    layout->addLayout(buttonLayout);

    // Log view
    m_logView = new QTextEdit;
    m_logView->setReadOnly(true);
    m_logView->setFontFamily("Monospace");
    m_logView->setFontPointSize(9);
    layout->addWidget(m_logView);

    setWidget(widget);
}

void LogsDockWidget::appendLog(const QString& message, LogLevel level, const QString& destination) {
    QMutexLocker locker(&m_logMutex);

    QString formatted = formatLogEntry(message, level, destination, QDateTime::currentDateTime());
    m_allLogs.append(formatted);

    // Add to file if path set
    if (!m_logPath.isEmpty()) {
        QFile file(m_logPath);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << formatted << "\n";
            file.close();
        }
    }

    // Update destination filter if new destination
    if (!destination.isEmpty()) {
        bool exists = false;
        for (int i = 0; i < m_destFilter->count(); ++i) {
            if (m_destFilter->itemData(i).toString() == destination) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            m_destFilter->addItem(destination, destination);
        }
    }
}

void LogsDockWidget::setLogLevel(LogLevel level) {
    m_currentLevel = level;
}

LogsDockWidget::LogLevel LogsDockWidget::getLogLevel() const {
    return m_currentLevel;
}

void LogsDockWidget::setLogPath(const QString& path) {
    m_logPath = path;
}

QString LogsDockWidget::getLogPath() const {
    return m_logPath;
}

void LogsDockWidget::clearLogs() {
    QMutexLocker locker(&m_logMutex);
    m_allLogs.clear();
    m_filteredLogs.clear();
    m_logView->clear();
}

void LogsDockWidget::filterByDestination(const QString& dest) {
    m_currentDestFilter = dest;
    applyFilters();
}

void LogsDockWidget::onSearchTextChanged(const QString& text) {
    m_currentSearchFilter = text;
    applyFilters();
}

void LogsDockWidget::onDestinationFilterChanged() {
    m_currentDestFilter = m_destFilter->currentData().toString();
    applyFilters();
}

void LogsDockWidget::onLevelFilterChanged() {
    m_currentLevelFilter = static_cast<LogLevel>(m_levelFilter->currentData().toInt());
    applyFilters();
}

void LogsDockWidget::onClearButtonClicked() {
    clearLogs();
}

void LogsDockWidget::onAutoScrollChanged() {
    m_autoScroll = m_autoScrollCheck->isChecked();
}

void LogsDockWidget::updateDisplay() {
    QMutexLocker locker(&m_logMutex);

    if (m_allLogs.size() != m_lastLogCount) {
        applyFilters();
        m_lastLogCount = m_allLogs.size();
    }
}

void LogsDockWidget::applyFilters() {
    QMutexLocker locker(&m_logMutex);

    m_filteredLogs.clear();

    for (const QString& log : m_allLogs) {
        // Parse log entry to check filters
        // Format: [timestamp] [LEVEL] [dest] message
        QStringList parts = log.split("] ");
        if (parts.size() >= 3) {
            QString levelStr = parts[1].mid(1); // Remove [
            LogLevel level = DEBUG;
            if (levelStr == "INFO") level = INFO;
            else if (levelStr == "WARNING") level = WARNING;
            else if (levelStr == "ERROR") level = ERROR;
            else if (levelStr == "CRITICAL") level = CRITICAL;

            if (level < m_currentLevelFilter) continue;

            QString dest = parts[2].mid(1); // Remove [
            if (!m_currentDestFilter.isEmpty() && dest != m_currentDestFilter) continue;

            QString message = parts.mid(3).join("] ");
            if (!m_currentSearchFilter.isEmpty() && !message.contains(m_currentSearchFilter, Qt::CaseInsensitive)) continue;

            m_filteredLogs.append(log);
        }
    }

    QString displayText = m_filteredLogs.join("\n");
    m_logView->setPlainText(displayText);

    if (m_autoScroll) {
        QScrollBar* scrollBar = m_logView->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

QString LogsDockWidget::formatLogEntry(const QString& message, LogLevel level, const QString& destination, const QDateTime& timestamp) {
    QString destPart = destination.isEmpty() ? "" : QString("[%1] ").arg(destination);
    return QString("[%1] [%2] %3%4")
        .arg(timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz"))
        .arg(levelToString(level))
        .arg(destPart)
        .arg(message);
}

QString LogsDockWidget::levelToString(LogLevel level) const {
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        case CRITICAL: return "CRITICAL";
    }
    return "UNKNOWN";
}

QColor LogsDockWidget::levelToColor(LogLevel level) const {
    switch (level) {
        case DEBUG: return QColor(Qt::gray);
        case INFO: return QColor(Qt::black);
        case WARNING: return QColor(255, 165, 0); // Orange
        case ERROR: return QColor(Qt::red);
        case CRITICAL: return QColor(Qt::darkRed);
    }
    return QColor(Qt::black);
}