#include "main_window.h"
#include "utilities.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), selectedTile(TileType::Air)
{
    QPixmap pixmap("data/sprites/wall.png");
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    setWindowTitle("Platformer Level Editor");
    setFocusPolicy(Qt::StrongFocus);
    centralWidget->setFocusPolicy(Qt::NoFocus);

    auto *mainLayout = new QHBoxLayout(centralWidget);

    auto *buttonLayout = new QVBoxLayout();
    QPushButton *airButton      = createButton(QIcon("data/sprites/air.png"),      TileType::Air,      buttonLayout);
    QPushButton *wallButton     = createButton(QIcon("data/sprites/wall.png"),     TileType::Wall,     buttonLayout);
    QPushButton *darkWallButton = createButton(QIcon("data/sprites/wall_dark.png"),TileType::DarkWall, buttonLayout);
    QPushButton *playerButton   = createButton(QIcon("data/sprites/player.png"),   TileType::Player,   buttonLayout);
    QPushButton *coinButton     = createButton(QIcon("data/sprites/coin.png"),     TileType::Coin,     buttonLayout);
    QPushButton *exitButton     = createButton(QIcon("data/sprites/exit.png"),     TileType::Exit,     buttonLayout);
    QPushButton *spikesButton   = createButton(QIcon("data/sprites/spikes.png"),   TileType::Spikes,   buttonLayout);
    QPushButton *enemyButton    = createButton(QIcon("data/sprites/enemy.png"),    TileType::Enemy,    buttonLayout);

    QWidget *buttonWidget = new QWidget();
    buttonWidget->setLayout(buttonLayout);
    mainLayout->addWidget(buttonWidget);

    level = new QTableWidget(10, 10);
    level->verticalHeader()->setDefaultSectionSize(24);
    level->horizontalHeader()->setDefaultSectionSize(24);
    level->verticalHeader()->setMinimumSectionSize(24);
    level->horizontalHeader()->setMinimumSectionSize(24);
    level->verticalHeader()->setMaximumSectionSize(24);
    level->horizontalHeader()->setMaximumSectionSize(24);
    level->verticalHeader()->hide();
    level->horizontalHeader()->hide();
    level->setEditTriggers(QAbstractItemView::NoEditTriggers);
    level->setSelectionMode(QAbstractItemView::NoSelection);
    level->setSelectionBehavior(QAbstractItemView::SelectItems);

    connect(level, &QTableWidget::cellClicked, this, &MainWindow::onTileClicked);

    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

//File
    QMenu* file = menuBar->addMenu("&File");

    QAction* exportLevelAction = file->addAction("Export Level...");
    connect(exportLevelAction, &QAction::triggered, this, &MainWindow::exportToFile);

    QAction* closeAction = file->addAction("Close All Windows");
    connect(closeAction, &QAction::triggered, this, &MainWindow::closeAllWindows);

//Edit
    QMenu* edit = menuBar->addMenu("&Edit");

    QAction* clear = edit->addAction("Clear Level");
    connect(clear, &QAction::triggered, this, &MainWindow::clearLevel);

    QAction* resize = edit->addAction("Resize Level");
    connect(resize, &QAction::triggered, this, &MainWindow::resizeLevel);

    QAction* undo = edit->addAction("Undo Tile Placement");
    connect(undo, &QAction::triggered, this, &MainWindow::undoTilePlacement);

//Help
    QMenu* help = menuBar->addMenu("&Help");

    QAction* document = help->addAction("Documentation");
    connect(document, &QAction::triggered, this, &MainWindow::showDocumentation);

    mainLayout->addWidget(level);
    centralWidget->show();
}

MainWindow::~MainWindow() { }

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // TODO

    QMainWindow::keyPressEvent(event);
}

void MainWindow::selectTile(char tile)
{
    switch (tile) {
        case '*': selectedTile = TileType::Coin; break;
        case '&': selectedTile = TileType::Enemy; break;
        case 'E': selectedTile = TileType::Exit; break;
        case '@': selectedTile = TileType::Player; break;
        case '^': selectedTile = TileType::Spikes; break;
        case '#': selectedTile = TileType::Wall; break;
        case '=': selectedTile = TileType::DarkWall; break;
        case ' ': selectedTile = TileType::Air; break;
    }
}

void MainWindow::onTileClicked(int row, int col)
{
    QTableWidgetItem* item = level->item(row, col);
    char previousTile = ' ';
    if (item != nullptr)
        previousTile = item->data(Qt::UserRole).toChar().toLatin1();

    undoStack.push({row, col, previousTile});

    if (item == nullptr) {
        item = new QTableWidgetItem();
        level->setItem(row, col, item);
    }

    QIcon icon;
    switch (selectedTile) {
        case TileType::Coin: icon = QIcon("data/sprites/coin.png"); break;
        case TileType::Enemy: icon = QIcon("data/sprites/enemy.png"); break;
        case TileType::Exit: icon = QIcon("data/sprites/exit.png"); break;
        case TileType::Player: icon = QIcon("data/sprites/player.png"); break;
        case TileType::Spikes: icon = QIcon("data/sprites/spikes.png"); break;
        case TileType::Wall: icon = QIcon("data/sprites/wall.png"); break;
        case TileType::DarkWall: icon = QIcon("data/sprites/wall_dark.png"); break;
    }
    item->setIcon(icon);

    char data = '-';
    switch (selectedTile) {
        case TileType::Coin: data = '*'; break;
        case TileType::Enemy: data = '&'; break;
        case TileType::Exit: data = 'E'; break;
        case TileType::Player: data = '@'; break;
        case TileType::Spikes: data = '^'; break;
        case TileType::Wall: data = '#'; break;
        case TileType::DarkWall: data = '='; break;
    }

    item->setData(Qt::UserRole, data);
}

QPushButton* MainWindow::createButton(const QIcon &icon, TileType tileType, QVBoxLayout* layout)
{
    QPushButton *button = new QPushButton();
    button->setFixedSize(32, 32);
    button->setIcon(icon);
    connect(button, &QPushButton::clicked, this, [this, tileType]() {
        selectTile(static_cast<char>(tileType));
    });
    layout->addWidget(button);
    return button;
}

void MainWindow::clearLevel()
{
    undoStack.clear();
    for (int row = 0; row < level->rowCount(); ++row) {
        for (int col = 0; col < level->columnCount(); ++col) {
            delete level->takeItem(row, col);
        }
    }
}

void MainWindow::resizeLevel()
{
    QDialog resizeDialog(this);
    resizeDialog.setWindowTitle("Resizing");
    QFormLayout form(&resizeDialog);

    QLineEdit *rowsEdit = new QLineEdit(&resizeDialog);
    QLineEdit *colsEdit = new QLineEdit(&resizeDialog);

    rowsEdit->setText(QString::number(level->rowCount()));
    colsEdit->setText(QString::number(level->columnCount()));

    form.addRow("Rows:", rowsEdit);
    form.addRow("Columns:", colsEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &resizeDialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &resizeDialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &resizeDialog, &QDialog::reject);

    resizeDialog.setLayout(&form);

    if (resizeDialog.exec() == QDialog::Accepted) {
        int newRows = rowsEdit->text().toInt();
        int newCols = colsEdit->text().toInt();
        level->setRowCount(newRows);
        level->setColumnCount(newCols);
    }
}

void MainWindow::undoTilePlacement()
{
    if (undoStack.isEmpty()) return;
    TileAction action = undoStack.pop();

    if (action.previousTile == ' ') {
        delete level->takeItem(action.row, action.col);
        return;
    }

    QTableWidgetItem* item = new QTableWidgetItem();
    QIcon icon;

    switch (action.previousTile) {
        case '*': icon = QIcon("data/sprites/coin.png"); break;
        case '&': icon = QIcon("data/sprites/enemy.png"); break;
        case 'E': icon = QIcon("data/sprites/exit.png"); break;
        case '@': icon = QIcon("data/sprites/player.png"); break;
        case '^': icon = QIcon("data/sprites/spikes.png"); break;
        case '#': icon = QIcon("data/sprites/wall.png"); break;
        case '=': icon = QIcon("data/sprites/wall_dark.png"); break;
    }

    item->setIcon(icon);
    item->setData(Qt::UserRole, action.previousTile);
    level->setItem(action.row, action.col, item);
}

void MainWindow::exportToFile()
{
    QString filePath = QFileDialog::getSaveFileName(
            this,
            "Export Level",
            "",
            "RLL Files (*.rll);;All Files (*)"
    );

    if (filePath.isEmpty()) return;
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    int rows = level->rowCount();
    int cols = level->columnCount();
    std::vector<char> data(rows * cols, '-');

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            QTableWidgetItem* item = level->item(row, col);
            if (item) {
                char tile = item->data(Qt::UserRole).toChar().toLatin1();
                data[row * cols + col] = tile;
            }
        }
    }

    QString output;
    encrypt(rows, cols, data.data(), output);

    QTextStream out(&file);
    out << output;
    file.close();
}

void MainWindow::closeAllWindows()
{
    QApplication::closeAllWindows();
}

void MainWindow::showDocumentation()
{
    QMessageBox::information(this, "Documentation",
                             "Use tile buttons on the left to place icons to change the level as you like.\n"
                             "Use Edit menu to undo, to resize, or clear.\n"
                             "Use undo to get rid of the last tile placement.\n"
                             "Use resize to change the amount of rows and columns.\n"
                             "Use clear to get rid of all the icons on the level.\n"
                             "Export your project from File or use File to close all windows.");
}
