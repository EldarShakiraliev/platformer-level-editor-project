#include <QApplication>

#include "main_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow::drawSplash();

    QDialog welcomeDialog;
    welcomeDialog.setWindowTitle("Level Editor");
    welcomeDialog.setFixedSize(300, 200);

    QVBoxLayout *layout = new QVBoxLayout(&welcomeDialog);
    QLabel *label = new QLabel("Welcome to Level Editor!");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: white; font-size: 20px;");

    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *newButton = new QPushButton("New level");
    newButton->setStyleSheet("color: white; font-size: 13px; background-color: blue;");
    QPushButton *uploadButton = new QPushButton("Upload level");
    uploadButton->setStyleSheet("color: white; font-size: 13px; background-color: darkgreen;");
    QPushButton *cancelButton = new QPushButton("Cancel");
    cancelButton->setStyleSheet("color: white; font-size: 13px; background-color: darkred;");

    buttons->addWidget(newButton);
    buttons->addWidget(uploadButton);
    buttons->addWidget(cancelButton);

    layout->addWidget(label);
    layout->addLayout(buttons);

    bool uploadSelected = false;
    QObject::connect(newButton, &QPushButton::clicked, [&]() {
        uploadSelected = false;
        welcomeDialog.accept();
    });
    QObject::connect(uploadButton, &QPushButton::clicked, [&]() {
        uploadSelected = true;
        welcomeDialog.accept();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&]() {
        uploadSelected = false;
        welcomeDialog.reject();
    });

    if (welcomeDialog.exec() == QDialog::Accepted) {
        MainWindow window;

        if (uploadSelected) {
            QString fileName = QFileDialog::getOpenFileName(
                    &window,
                    "Open RLL File",
                    "",
                    "RLL Files (*.rll)"
            );

            if (!fileName.isEmpty()) {
                try {
                    std::vector<std::string> rows = MainWindow::load_from_file(fileName);

                    if (!rows.empty()) {
                        window.loadLevelToGrid(rows);
                    } else {
                        QMessageBox::warning(&window, "Invalid File", "No valid level data found.");
                    }
                }
                catch (const std::exception &e) {
                    QMessageBox::critical(&window, "Error", e.what());
                }
            }
        }
        window.show();
        return app.exec();
    }
    return 0;
}
