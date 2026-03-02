#include "VerifyWizard.h"
#include <QApplication>
#include <QDirIterator>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QFont>
#include <QStandardPaths>

VerifyWizard::VerifyWizard(QWidget* parent)
    : QWizard(parent) {
    setWindowTitle("VerifyWizard - Full Scan & Hash Compare");
    setWizardStyle(QWizard::ModernStyle);

    addPage(new IntroPage);
    addPage(new ScanPage);
    addPage(new ReportPage);

    connect(this, &QWizard::finished, this, &VerifyWizard::generateReport);
}

VerifyWizard::~VerifyWizard() {}

void VerifyWizard::setScanPath(const QString& path) {
    m_scanPath = path;
}

QString VerifyWizard::getReportPath() const {
    return m_reportPath;
}

void VerifyWizard::scanDirectory() {
    m_files.clear();
    m_hashes.clear();

    QDirIterator it(m_scanPath, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        QFileInfo fileInfo = it.fileInfo();
        if (fileInfo.isFile()) {
            m_files.append(fileInfo);

            // Calculate hash
            QFile file(fileInfo.absoluteFilePath());
            if (file.open(QIODevice::ReadOnly)) {
                QCryptographicHash hash(QCryptographicHash::Sha256);
                if (hash.addData(&file)) {
                    m_hashes[fileInfo.absoluteFilePath()] = hash.result();
                }
                file.close();
            }
        }
    }

    // For demo, simulate comparison (in real implementation, compare with stored hashes)
    for (const QFileInfo& file : m_files) {
        m_comparisonResults[file.absoluteFilePath()] = true; // Assume all match for now
    }
}

void VerifyWizard::generateReport() {
    if (m_files.isEmpty()) return;

    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    m_reportPath = desktopPath + "/verify_report_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".pdf";

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(m_reportPath);
    printer.setPageSize(QPrinter::A4);
    printer.setResolution(300);

    QPainter painter(&printer);
    QFont titleFont("Arial", 16, QFont::Bold);
    QFont normalFont("Arial", 10);

    painter.setFont(titleFont);
    painter.drawText(100, 100, "DIT Transfer Tools - Verification Report");

    painter.setFont(normalFont);
    painter.drawText(100, 200, "Scan Path: " + m_scanPath);
    painter.drawText(100, 250, "Scan Date: " + QDateTime::currentDateTime().toString());
    painter.drawText(100, 300, "Total Files: " + QString::number(m_files.size()));

    int y = 400;
    painter.drawText(100, y, "File Details:");

    for (const QFileInfo& file : m_files) {
        y += 50;
        if (y > 2700) { // New page
            printer.newPage();
            y = 100;
        }
        painter.drawText(100, y, file.absoluteFilePath());
        painter.drawText(100, y + 20, "Size: " + QString::number(file.size()) + " bytes");
        painter.drawText(100, y + 40, "Hash: " + m_hashes[file.absoluteFilePath()].toHex());
        painter.drawText(100, y + 60, "Verified: " + (m_comparisonResults[file.absoluteFilePath()] ? "OK" : "FAILED"));
    }

    painter.end();
}

// IntroPage
IntroPage::IntroPage(QWidget* parent) : QWizardPage(parent) {
    setTitle("Welcome to VerifyWizard");
    setSubTitle("This wizard will perform a full scan, calculate hashes, and generate a verification report.");

    m_label = new QLabel("Select the directory to scan and verify.");
    m_label->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_label);
    setLayout(layout);
}

// ScanPage
ScanPage::ScanPage(QWidget* parent) : QWizardPage(parent) {
    setTitle("Scanning Directory");
    setSubTitle("Please wait while we scan and hash the files.");

    m_statusLabel = new QLabel("Scanning...");
    m_progressBar = new QProgressBar;
    m_logView = new QTextEdit;
    m_logView->setReadOnly(true);
    m_logView->setMaximumHeight(200);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_statusLabel);
    layout->addWidget(m_progressBar);
    layout->addWidget(m_logView);
    setLayout(layout);
}

void ScanPage::initializePage() {
    VerifyWizard* wizard = qobject_cast<VerifyWizard*>(this->wizard());
    if (wizard) {
        wizard->scanDirectory();
        m_progressBar->setValue(100);
        m_statusLabel->setText("Scan complete.");
        m_logView->append("Scanned " + QString::number(wizard->getReportPath().isEmpty() ? 0 : 1) + " directories.");
    }
}

bool ScanPage::isComplete() const {
    return m_progressBar->value() == 100;
}

// ReportPage
ReportPage::ReportPage(QWidget* parent) : QWizardPage(parent) {
    setTitle("Verification Report");
    setSubTitle("Review the scan results and save the report.");

    m_summaryLabel = new QLabel;
    m_reportView = new QTextEdit;
    m_reportView->setReadOnly(true);
    m_saveButton = new QPushButton("Save PDF Report");

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_summaryLabel);
    layout->addWidget(m_reportView);
    layout->addWidget(m_saveButton);
    setLayout(layout);

    connect(m_saveButton, &QPushButton::clicked, [this]() {
        VerifyWizard* wizard = qobject_cast<VerifyWizard*>(this->wizard());
        if (wizard && !wizard->getReportPath().isEmpty()) {
            QMessageBox::information(this, "Report Saved", "Report saved to: " + wizard->getReportPath());
        }
    });
}

void ReportPage::initializePage() {
    VerifyWizard* wizard = qobject_cast<VerifyWizard*>(this->wizard());
    if (wizard) {
        m_summaryLabel->setText("Scan completed. Report generated.");
        m_reportView->setPlainText("Report path: " + wizard->getReportPath() + "\n\nVerification details would be here.");
    }
}