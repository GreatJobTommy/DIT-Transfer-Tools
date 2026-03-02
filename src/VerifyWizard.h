#ifndef VERIFYWIZARD_H
#define VERIFYWIZARD_H

#include <QWizard>
#include <QWizardPage>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QFileInfoList>
#include <QCryptographicHash>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>

class VerifyWizard : public QWizard {
    Q_OBJECT

public:
    VerifyWizard(QWidget* parent = nullptr);
    ~VerifyWizard();

    void setScanPath(const QString& path);
    QString getReportPath() const;

private slots:
    void scanDirectory();
    void generateReport();

private:
    QString m_scanPath;
    QString m_reportPath;
    QFileInfoList m_files;
    QHash<QString, QByteArray> m_hashes;
    QHash<QString, bool> m_comparisonResults;
};

class IntroPage : public QWizardPage {
    Q_OBJECT

public:
    IntroPage(QWidget* parent = nullptr);

private:
    QLabel* m_label;
};

class ScanPage : public QWizardPage {
    Q_OBJECT

public:
    ScanPage(QWidget* parent = nullptr);
    void initializePage() override;
    bool isComplete() const override;

private slots:
    void updateProgress();

private:
    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;
    QTextEdit* m_logView;
};

class ReportPage : public QWizardPage {
    Q_OBJECT

public:
    ReportPage(QWidget* parent = nullptr);
    void initializePage() override;

private:
    QLabel* m_summaryLabel;
    QTextEdit* m_reportView;
    QPushButton* m_saveButton;
};

#endif // VERIFYWIZARD_H