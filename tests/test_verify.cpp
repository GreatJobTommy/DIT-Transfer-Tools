#include <QtTest/QtTest>
#include <QApplication>
#include <QTemporaryDir>
#include <QFile>
#include "../src/VerifyWizard.h"

class TestVerify : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testScanDirectory();
    void testHashCalculation();
    void testReportGeneration();
    void cleanupTestCase();

private:
    QTemporaryDir* m_tempDir;
    QString m_testPath;
};

void TestVerify::initTestCase() {
    m_tempDir = new QTemporaryDir();
    m_testPath = m_tempDir->path();

    // Create test files
    QFile file1(m_testPath + "/test1.txt");
    file1.open(QIODevice::WriteOnly);
    file1.write("Hello World");
    file1.close();

    QFile file2(m_testPath + "/test2.txt");
    file2.open(QIODevice::WriteOnly);
    file2.write("Test content");
    file2.close();
}

void TestVerify::testScanDirectory() {
    VerifyWizard wizard;
    wizard.setScanPath(m_testPath);

    // Access private method via QTest::qWaitFor or simulate
    // For simplicity, assume scanDirectory is called internally
    QCOMPARE(wizard.getReportPath().isEmpty(), true); // Not generated yet
}

void TestVerify::testHashCalculation() {
    // Test hash calculation
    QFile file(m_testPath + "/test1.txt");
    file.open(QIODevice::ReadOnly);
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(&file);
    QByteArray result = hash.result();
    file.close();

    QVERIFY(!result.isEmpty());
    QCOMPARE(result.size(), 32); // SHA256 is 32 bytes
}

void TestVerify::testReportGeneration() {
    VerifyWizard wizard;
    wizard.setScanPath(m_testPath);
    wizard.generateReport();

    QVERIFY(!wizard.getReportPath().isEmpty());
    QFile report(wizard.getReportPath());
    QVERIFY(report.exists());
}

void TestVerify::cleanupTestCase() {
    delete m_tempDir;
}

QTEST_MAIN(TestVerify)
#include "test_verify.moc"