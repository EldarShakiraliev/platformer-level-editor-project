#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtWidgets>

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void loadLevelToGrid(const std::vector<std::string> &levelData);

    static std::vector<std::string> load_from_file(const QString &qpath);

    static void drawSplash();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void selectTile(char tile);

    void onTileClicked(int row, int col);

    void undoTilePlacement();

    void closeAllWindows();

    void clearLevel();

    void resizeLevel();

    void showDocumentation();

    void exportToFile();

    struct TileAction {
        int row;
        int col;
        char previousTile;
    };

    enum class TileType {
        Coin = '*',
        Enemy = '&',
        Exit = 'E',
        Player = '@',
        Spikes = '^',
        Wall = '#',
        DarkWall = '=',
        Cherry = '+',
        Diamond = 'D',
        Shoe = 'S',
        Air = ' '
    };

    TileType selectedTile;
    QStack<TileAction> undoStack;

    QPushButton *createButton(const QIcon &icon, TileType tileType, QVBoxLayout *layout);

    QTableWidget *level = nullptr;
    const float iconSize = 24.0f;
};

#endif // MAIN_WINDOW_H
