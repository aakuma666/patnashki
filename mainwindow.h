#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QVector>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void tileClicked();
    void newGame();
    void checkWin();
    void updateTimer();

private:
    void initGame();
    void shuffleTiles();
    void moveTile(int row, int col);
    bool isSolvable(const QVector<int>& tiles);
    int findTilePosition(QPushButton* tile, int& row, int& col);

    QVector<QPushButton*> tiles;
    QGridLayout *gridLayout;
    QWidget *centralWidget;
    QLabel *movesLabel;
    QLabel *timerLabel;
    QTimer *gameTimer;

    int emptyRow;
    int emptyCol;
    int moveCount;
    int gameTime;
    bool gameStarted;
};

#endif // MAINWINDOW_H
