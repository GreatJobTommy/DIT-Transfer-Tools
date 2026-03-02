/********************************************************************************
** Form generated from reading UI file 'AddTaskDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.15
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDTASKDIALOG_H
#define UI_ADDTASKDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AddTaskDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *pathGroup;
    QFormLayout *formLayout;
    QLabel *sourceLabel;
    QHBoxLayout *sourceLayout;
    QLineEdit *sourceEdit;
    QPushButton *sourceBrowseBtn;
    QLabel *destLabel;
    QHBoxLayout *destLayout;
    QLineEdit *destEdit;
    QPushButton *destBrowseBtn;
    QGroupBox *presetGroup;
    QHBoxLayout *horizontalLayout;
    QLabel *presetLabel;
    QComboBox *presetCombo;
    QSpacerItem *horizontalSpacer;
    QSplitter *splitter;
    QGroupBox *dragGroup;
    QVBoxLayout *verticalLayout_2;
    QListWidget *dragList;
    QGroupBox *previewGroup;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *previewEdit;
    QHBoxLayout *buttonLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *cancelBtn;
    QPushButton *addBtn;

    void setupUi(QDialog *AddTaskDialog)
    {
        if (AddTaskDialog->objectName().isEmpty())
            AddTaskDialog->setObjectName(QString::fromUtf8("AddTaskDialog"));
        AddTaskDialog->resize(600, 400);
        verticalLayout = new QVBoxLayout(AddTaskDialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pathGroup = new QGroupBox(AddTaskDialog);
        pathGroup->setObjectName(QString::fromUtf8("pathGroup"));
        formLayout = new QFormLayout(pathGroup);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        sourceLabel = new QLabel(pathGroup);
        sourceLabel->setObjectName(QString::fromUtf8("sourceLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, sourceLabel);

        sourceLayout = new QHBoxLayout();
        sourceLayout->setSpacing(6);
        sourceLayout->setObjectName(QString::fromUtf8("sourceLayout"));
        sourceEdit = new QLineEdit(pathGroup);
        sourceEdit->setObjectName(QString::fromUtf8("sourceEdit"));

        sourceLayout->addWidget(sourceEdit);

        sourceBrowseBtn = new QPushButton(pathGroup);
        sourceBrowseBtn->setObjectName(QString::fromUtf8("sourceBrowseBtn"));

        sourceLayout->addWidget(sourceBrowseBtn);


        formLayout->setLayout(0, QFormLayout::FieldRole, sourceLayout);

        destLabel = new QLabel(pathGroup);
        destLabel->setObjectName(QString::fromUtf8("destLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, destLabel);

        destLayout = new QHBoxLayout();
        destLayout->setSpacing(6);
        destLayout->setObjectName(QString::fromUtf8("destLayout"));
        destEdit = new QLineEdit(pathGroup);
        destEdit->setObjectName(QString::fromUtf8("destEdit"));

        destLayout->addWidget(destEdit);

        destBrowseBtn = new QPushButton(pathGroup);
        destBrowseBtn->setObjectName(QString::fromUtf8("destBrowseBtn"));

        destLayout->addWidget(destBrowseBtn);


        formLayout->setLayout(1, QFormLayout::FieldRole, destLayout);


        verticalLayout->addWidget(pathGroup);

        presetGroup = new QGroupBox(AddTaskDialog);
        presetGroup->setObjectName(QString::fromUtf8("presetGroup"));
        horizontalLayout = new QHBoxLayout(presetGroup);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        presetLabel = new QLabel(presetGroup);
        presetLabel->setObjectName(QString::fromUtf8("presetLabel"));

        horizontalLayout->addWidget(presetLabel);

        presetCombo = new QComboBox(presetGroup);
        presetCombo->addItem(QString());
        presetCombo->addItem(QString());
        presetCombo->addItem(QString());
        presetCombo->setObjectName(QString::fromUtf8("presetCombo"));

        horizontalLayout->addWidget(presetCombo);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(presetGroup);

        splitter = new QSplitter(AddTaskDialog);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        dragGroup = new QGroupBox(splitter);
        dragGroup->setObjectName(QString::fromUtf8("dragGroup"));
        verticalLayout_2 = new QVBoxLayout(dragGroup);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        dragList = new QListWidget(dragGroup);
        dragList->setObjectName(QString::fromUtf8("dragList"));
        dragList->setAcceptDrops(true);
        dragList->setDragEnabled(false);

        verticalLayout_2->addWidget(dragList);

        splitter->addWidget(dragGroup);
        previewGroup = new QGroupBox(splitter);
        previewGroup->setObjectName(QString::fromUtf8("previewGroup"));
        verticalLayout_3 = new QVBoxLayout(previewGroup);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        previewEdit = new QTextEdit(previewGroup);
        previewEdit->setObjectName(QString::fromUtf8("previewEdit"));
        previewEdit->setReadOnly(true);

        verticalLayout_3->addWidget(previewEdit);

        splitter->addWidget(previewGroup);

        verticalLayout->addWidget(splitter);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setSpacing(6);
        buttonLayout->setObjectName(QString::fromUtf8("buttonLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonLayout->addItem(horizontalSpacer_2);

        cancelBtn = new QPushButton(AddTaskDialog);
        cancelBtn->setObjectName(QString::fromUtf8("cancelBtn"));

        buttonLayout->addWidget(cancelBtn);

        addBtn = new QPushButton(AddTaskDialog);
        addBtn->setObjectName(QString::fromUtf8("addBtn"));

        buttonLayout->addWidget(addBtn);


        verticalLayout->addLayout(buttonLayout);


        retranslateUi(AddTaskDialog);

        QMetaObject::connectSlotsByName(AddTaskDialog);
    } // setupUi

    void retranslateUi(QDialog *AddTaskDialog)
    {
        AddTaskDialog->setWindowTitle(QCoreApplication::translate("AddTaskDialog", "Add Transfer Task", nullptr));
        pathGroup->setTitle(QCoreApplication::translate("AddTaskDialog", "Paths", nullptr));
        sourceLabel->setText(QCoreApplication::translate("AddTaskDialog", "Source:", nullptr));
        sourceBrowseBtn->setText(QCoreApplication::translate("AddTaskDialog", "Browse...", nullptr));
        destLabel->setText(QCoreApplication::translate("AddTaskDialog", "Destination:", nullptr));
        destBrowseBtn->setText(QCoreApplication::translate("AddTaskDialog", "Browse...", nullptr));
        presetGroup->setTitle(QCoreApplication::translate("AddTaskDialog", "Presets", nullptr));
        presetLabel->setText(QCoreApplication::translate("AddTaskDialog", "Preset:", nullptr));
        presetCombo->setItemText(0, QCoreApplication::translate("AddTaskDialog", "Default", nullptr));
        presetCombo->setItemText(1, QCoreApplication::translate("AddTaskDialog", "Fast Copy", nullptr));
        presetCombo->setItemText(2, QCoreApplication::translate("AddTaskDialog", "Safe Copy", nullptr));

        dragGroup->setTitle(QCoreApplication::translate("AddTaskDialog", "Drag Folders Here", nullptr));
        previewGroup->setTitle(QCoreApplication::translate("AddTaskDialog", "Preview", nullptr));
        cancelBtn->setText(QCoreApplication::translate("AddTaskDialog", "Cancel", nullptr));
        addBtn->setText(QCoreApplication::translate("AddTaskDialog", "Add to Queue", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddTaskDialog: public Ui_AddTaskDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDTASKDIALOG_H
