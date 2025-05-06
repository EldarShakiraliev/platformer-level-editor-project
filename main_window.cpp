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

    auto *editToolBar = new QToolBar();
    editToolBar->setMovable(false);
    auto *editToolWidget = new QWidget();
    auto *editLayout = new QHBoxLayout(editToolWidget);

    QPushButton *clearButton = new QPushButton("Clear");
    QPushButton *resizeButton = new QPushButton("Resize");
    QPushButton *undoButton = new QPushButton("Undo");

    editLayout->addWidget(clearButton);
    editLayout->addWidget(resizeButton);
    editLayout->addWidget(undoButton);

    editToolBar->addWidget(editToolWidget);
    addToolBar(editToolBar);

    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearLevel);
    connect(resizeButton, &QPushButton::clicked, this, &MainWindow::resizeLevel);
    connect(undoButton, &QPushButton::clicked, this, &MainWindow::undoTilePlacement);
    connect(level, &QTableWidget::cellClicked, this, &MainWindow::onTileClicked);

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

    QFile file(filePath);
    QTextStream out(&file);

    // TODO

    /* Snippets:
     * std::vector<char> data(rows * cols, '-');
     * char tile = item->data(Qt::UserRole).toChar().toLatin1();
     * encrypt(rows, cols, data.data(), output);
     * out << output;
     */

    file.close();
}
