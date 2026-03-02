#ifndef LOGSDOCKWIDGET_H
#define LOGSDOCKWIDGET_H

#include <QDockWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QStringList>
#include <QMutex>

class LogsDockWidget : public QDockWidget {
    Q_OBJECT

public:
    enum LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        CRITICAL = 4
    };

    explicit LogsDockWidget(QWidget* parent = nullptr);
    ~LogsDockWidget();

    void appendLog(const QString& message, LogLevel level = INFO, const QString& destination = QString());
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    void setLogPath(const QString& path);
    QString getLogPath() const;
    void clearLogs();
    void filterByDestination(const QString& dest);

private slots:
    void onSearchTextChanged(const QString& text);
    void onDestinationFilterChanged();
    void onLevelFilterChanged();
    void onClearButtonClicked();
    void onAutoScrollChanged();
    void updateDisplay();

private:
    void setupUI();
    void applyFilters();
    QString formatLogEntry(const QString& message, LogLevel level, const QString& destination, const QDateTime& timestamp);
    QString levelToString(LogLevel level) const;
    QColor levelToColor(LogLevel level) const;

    QTextEdit* m_logView;
    QLineEdit* m_searchEdit;
    QComboBox* m_destFilter;
    QComboBox* m_levelFilter;
    QPushButton* m_clearButton;
    QCheckBox* m_autoScrollCheck;

    QStringList m_allLogs;
    QStringList m_filteredLogs;
    LogLevel m_currentLevel;
    QString m_logPath;
    QString m_currentDestFilter;
    QString m_currentSearchFilter;
    LogLevel m_currentLevelFilter;
    QTimer* m_updateTimer;
    QMutex m_logMutex;

    // For tail -f style
    bool m_autoScroll;
    int m_lastLogCount;
};

#endif // LOGSDOCKWIDGET_H