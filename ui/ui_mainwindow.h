/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *droppedFramesValueLabel;
    QLabel *fpsValueLabel;
    QLabel *droppedTextLabel;
    QPushButton *algorithmButton;
    QLabel *fpsTextLabel;
    QPushButton *openMovieButton;
    QPushButton *connectCameraButton;
    QPushButton *saveCameraButton;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_2;
    QLabel *imageDisplay;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label;
    QWidget *gridLayoutWidget_3;
    QGridLayout *gridLayout_3;
    QLabel *fpsTextLabel_2;
    QLabel *fpsValueLabel_2;
    QLabel *fpsTextLabel_3;
    QLabel *fpsValueLabel_3;
    QCheckBox *referenceSensorEnableCheckbox;
    QPushButton *connectCameraButton_2;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuCamera_heart_rate_monitor;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1021, 904);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayoutWidget = new QWidget(centralWidget);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(0, 0, 301, 441));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        droppedFramesValueLabel = new QLabel(gridLayoutWidget);
        droppedFramesValueLabel->setObjectName(QStringLiteral("droppedFramesValueLabel"));
        QFont font;
        font.setPointSize(16);
        droppedFramesValueLabel->setFont(font);
        droppedFramesValueLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(droppedFramesValueLabel, 1, 1, 1, 1);

        fpsValueLabel = new QLabel(gridLayoutWidget);
        fpsValueLabel->setObjectName(QStringLiteral("fpsValueLabel"));
        fpsValueLabel->setFont(font);
        fpsValueLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(fpsValueLabel, 0, 1, 1, 1);

        droppedTextLabel = new QLabel(gridLayoutWidget);
        droppedTextLabel->setObjectName(QStringLiteral("droppedTextLabel"));

        gridLayout->addWidget(droppedTextLabel, 1, 0, 1, 1);

        algorithmButton = new QPushButton(gridLayoutWidget);
        algorithmButton->setObjectName(QStringLiteral("algorithmButton"));
        algorithmButton->setMinimumSize(QSize(0, 80));

        gridLayout->addWidget(algorithmButton, 7, 0, 1, 2);

        fpsTextLabel = new QLabel(gridLayoutWidget);
        fpsTextLabel->setObjectName(QStringLiteral("fpsTextLabel"));

        gridLayout->addWidget(fpsTextLabel, 0, 0, 1, 1);

        openMovieButton = new QPushButton(gridLayoutWidget);
        openMovieButton->setObjectName(QStringLiteral("openMovieButton"));
        openMovieButton->setMinimumSize(QSize(0, 80));

        gridLayout->addWidget(openMovieButton, 6, 0, 1, 2);

        connectCameraButton = new QPushButton(gridLayoutWidget);
        connectCameraButton->setObjectName(QStringLiteral("connectCameraButton"));
        connectCameraButton->setMinimumSize(QSize(0, 80));

        gridLayout->addWidget(connectCameraButton, 4, 0, 1, 2);

        saveCameraButton = new QPushButton(gridLayoutWidget);
        saveCameraButton->setObjectName(QStringLiteral("saveCameraButton"));
        saveCameraButton->setMinimumSize(QSize(0, 80));

        gridLayout->addWidget(saveCameraButton, 5, 0, 1, 2);

        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(320, 0, 691, 691));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        imageDisplay = new QLabel(verticalLayoutWidget);
        imageDisplay->setObjectName(QStringLiteral("imageDisplay"));
        imageDisplay->setMinimumSize(QSize(0, 378));
        imageDisplay->setMaximumSize(QSize(16777215, 378));

        gridLayout_2->addWidget(imageDisplay, 0, 0, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        label_3 = new QLabel(verticalLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout->addWidget(label_3);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);

        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        gridLayoutWidget_3 = new QWidget(centralWidget);
        gridLayoutWidget_3->setObjectName(QStringLiteral("gridLayoutWidget_3"));
        gridLayoutWidget_3->setGeometry(QRect(0, 440, 301, 251));
        gridLayout_3 = new QGridLayout(gridLayoutWidget_3);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        fpsTextLabel_2 = new QLabel(gridLayoutWidget_3);
        fpsTextLabel_2->setObjectName(QStringLiteral("fpsTextLabel_2"));

        gridLayout_3->addWidget(fpsTextLabel_2, 1, 0, 1, 1);

        fpsValueLabel_2 = new QLabel(gridLayoutWidget_3);
        fpsValueLabel_2->setObjectName(QStringLiteral("fpsValueLabel_2"));

        gridLayout_3->addWidget(fpsValueLabel_2, 1, 1, 1, 1);

        fpsTextLabel_3 = new QLabel(gridLayoutWidget_3);
        fpsTextLabel_3->setObjectName(QStringLiteral("fpsTextLabel_3"));

        gridLayout_3->addWidget(fpsTextLabel_3, 2, 0, 1, 1);

        fpsValueLabel_3 = new QLabel(gridLayoutWidget_3);
        fpsValueLabel_3->setObjectName(QStringLiteral("fpsValueLabel_3"));

        gridLayout_3->addWidget(fpsValueLabel_3, 2, 1, 1, 1);

        referenceSensorEnableCheckbox = new QCheckBox(gridLayoutWidget_3);
        referenceSensorEnableCheckbox->setObjectName(QStringLiteral("referenceSensorEnableCheckbox"));
        referenceSensorEnableCheckbox->setMinimumSize(QSize(0, 40));
        referenceSensorEnableCheckbox->setLayoutDirection(Qt::RightToLeft);

        gridLayout_3->addWidget(referenceSensorEnableCheckbox, 0, 0, 1, 2);

        connectCameraButton_2 = new QPushButton(centralWidget);
        connectCameraButton_2->setObjectName(QStringLiteral("connectCameraButton_2"));
        connectCameraButton_2->setGeometry(QRect(530, 620, 299, 80));
        connectCameraButton_2->setMinimumSize(QSize(0, 80));
        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1021, 22));
        menuCamera_heart_rate_monitor = new QMenu(menuBar);
        menuCamera_heart_rate_monitor->setObjectName(QStringLiteral("menuCamera_heart_rate_monitor"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuCamera_heart_rate_monitor->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        droppedFramesValueLabel->setText(QApplication::translate("MainWindow", "<html><head/><body><p align=\"right\"><span style=\" font-size:16pt;\">0</span></p></body></html>", Q_NULLPTR));
        fpsValueLabel->setText(QApplication::translate("MainWindow", "<html><head/><body><p align=\"right\"><span style=\" font-size:16pt;\">FPS</span></p></body></html>", Q_NULLPTR));
        droppedTextLabel->setText(QApplication::translate("MainWindow", "Dropped frames:", Q_NULLPTR));
        algorithmButton->setText(QApplication::translate("MainWindow", "Start algorithm", Q_NULLPTR));
        fpsTextLabel->setText(QApplication::translate("MainWindow", "<html><head/><body><p>Capture FPS:</p></body></html>", Q_NULLPTR));
        openMovieButton->setText(QApplication::translate("MainWindow", "Open movie", Q_NULLPTR));
        connectCameraButton->setText(QApplication::translate("MainWindow", "Connect to camera", Q_NULLPTR));
        saveCameraButton->setText(QApplication::translate("MainWindow", "Save camera", Q_NULLPTR));
        imageDisplay->setText(QApplication::translate("MainWindow", "<html><head/><body><p align=\"center\"><span style=\" font-size:24pt;\">Connect camera</span></p></body></html>", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "Pulse signal from qCustomPlot", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "Original signal from qCustomPlot", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "Reference sensor PSD from qCustomPlot", Q_NULLPTR));
        fpsTextLabel_2->setText(QApplication::translate("MainWindow", "<html><head/><body><p>Pulse from algorithm</p></body></html>", Q_NULLPTR));
        fpsValueLabel_2->setText(QApplication::translate("MainWindow", "<html><head/><body><p align=\"right\"><span style=\" font-size:16pt;\">0</span></p></body></html>", Q_NULLPTR));
        fpsTextLabel_3->setText(QApplication::translate("MainWindow", "<html><head/><body><p>Pulse from sensor</p></body></html>", Q_NULLPTR));
        fpsValueLabel_3->setText(QApplication::translate("MainWindow", "<html><head/><body><p align=\"right\"><span style=\" font-size:16pt;\">0</span></p></body></html>", Q_NULLPTR));
        referenceSensorEnableCheckbox->setText(QApplication::translate("MainWindow", "Enable reference sensor read:                    ", Q_NULLPTR));
        connectCameraButton_2->setText(QApplication::translate("MainWindow", "Connect to camera", Q_NULLPTR));
        menuCamera_heart_rate_monitor->setTitle(QApplication::translate("MainWindow", "Camera heart rate monitor", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
