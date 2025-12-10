#include "mainwindow.h"
#include <QRandomGenerator>
#include <QTime>
#include <QFont>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), emptyRow(3), emptyCol(3), moveCount(0), gameTime(0), gameStarted(false)
{
    setWindowTitle("Пятнашки");
    resize(400, 500);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *infoLayout = new QHBoxLayout();

    movesLabel = new QLabel("Ходы: 0", this);
    movesLabel->setAlignment(Qt::AlignCenter);
    movesLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; }");

    timerLabel = new QLabel("Время: 00:00", this);
    timerLabel->setAlignment(Qt::AlignCenter);
    timerLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; }");

    infoLayout->addWidget(movesLabel);
    infoLayout->addWidget(timerLabel);
    mainLayout->addLayout(infoLayout);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(2);
    gridLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addLayout(gridLayout);

    QPushButton *newGameButton = new QPushButton("Новая игра", this);
    newGameButton->setStyleSheet(
        "QPushButton {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    padding: 8px;"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        );
    connect(newGameButton, &QPushButton::clicked, this, &MainWindow::newGame);
    mainLayout->addWidget(newGameButton);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateTimer);

    initGame();
}

MainWindow::~MainWindow()
{
    qDeleteAll(tiles);
}

void MainWindow::initGame()
{
    qDeleteAll(tiles);
    tiles.clear();

    QLayoutItem* item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (int i = 1; i <= 15; ++i) {
        QPushButton *tile = new QPushButton(QString::number(i));
        tile->setFixedSize(80, 80);
        tile->setStyleSheet(
            "QPushButton {"
            "    font-size: 20px;"
            "    font-weight: bold;"
            "    background-color: #3498db;"
            "    color: white;"
            "    border: 2px solid #2980b9;"
            "    border-radius: 5px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #2980b9;"
            "    border: 2px solid #1c6ea4;"
            "}"
            );

        connect(tile, &QPushButton::clicked, this, &MainWindow::tileClicked);
        tiles.append(tile);
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int index = i * 4 + j;
            if (index < 15) {
                gridLayout->addWidget(tiles[index], i, j);
            }
        }
    }

    emptyRow = 3;
    emptyCol = 3;
    moveCount = 0;
    gameTime = 0;
    gameStarted = false;

    movesLabel->setText("Ходы: 0");
    timerLabel->setText("Время: 00:00");

    shuffleTiles();
}

int MainWindow::findTilePosition(QPushButton* tile, int& row, int& col)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            QLayoutItem* item = gridLayout->itemAtPosition(i, j);
            if (item && item->widget() == tile) {
                row = i;
                col = j;
                return true;
            }
        }
    }
    return false;
}

void MainWindow::tileClicked()
{
    if (!gameStarted) {
        gameStarted = true;
        gameTimer->start(1000);
    }

    QPushButton *clickedTile = qobject_cast<QPushButton*>(sender());
    if (!clickedTile) return;

    int row = -1, col = -1;
    if (!findTilePosition(clickedTile, row, col)) {
        return;
    }

    qDebug() << "Tile clicked at:" << row << col << "Empty at:" << emptyRow << emptyCol;

    if ((abs(row - emptyRow) == 1 && col == emptyCol) ||
        (abs(col - emptyCol) == 1 && row == emptyRow)) {

        moveTile(row, col);
        moveCount++;
        movesLabel->setText("Ходы: " + QString::number(moveCount));

        checkWin();
    }
}

void MainWindow::moveTile(int row, int col)
{
    QLayoutItem *item = gridLayout->itemAtPosition(row, col);
    if (!item) return;

    QWidget *tile = item->widget();

    gridLayout->removeWidget(tile);
    gridLayout->addWidget(tile, emptyRow, emptyCol);
    emptyRow = row;
    emptyCol = col;

    centralWidget->update();
}

void MainWindow::shuffleTiles()
{
    QVector<int> numbers;
    for (int i = 1; i <= 15; ++i) {
        numbers.append(i);
    }

    QVector<int> shuffled;
    while (!numbers.isEmpty()) {
        int index = QRandomGenerator::global()->bounded(numbers.size());
        shuffled.append(numbers[index]);
        numbers.removeAt(index);
    }

    shuffled.append(0);

    while (!isSolvable(shuffled)) {
        numbers.clear();
        for (int i = 1; i <= 15; ++i) numbers.append(i);
        shuffled.clear();
        while (!numbers.isEmpty()) {
            int index = QRandomGenerator::global()->bounded(numbers.size());
            shuffled.append(numbers[index]);
            numbers.removeAt(index);
        }
        shuffled.append(0);
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int index = i * 4 + j;
            QLayoutItem* item = gridLayout->itemAtPosition(i, j);
            if (item && item->widget()) {
                gridLayout->removeWidget(item->widget());
            }
        }
    }


    for (int i = 0; i < 16; ++i) {
        int row = i / 4;
        int col = i % 4;

        if (shuffled[i] == 0) {
            emptyRow = row;
            emptyCol = col;
            qDebug() << "Empty cell at:" << row << col;
        } else {
            for (int j = 0; j < tiles.size(); ++j) {
                if (tiles[j]->text().toInt() == shuffled[i]) {
                    gridLayout->addWidget(tiles[j], row, col);
                    break;
                }
            }
        }
    }

    gameStarted = true;
    gameTimer->start(1000);
}

bool MainWindow::isSolvable(const QVector<int>& tiles)
{
    int inversions = 0;

    for (int i = 0; i < 15; ++i) {
        for (int j = i + 1; j < 16; ++j) {
            if (tiles[i] != 0 && tiles[j] != 0 && tiles[i] > tiles[j]) {
                inversions++;
            }
        }
    }

    int emptyRowFromBottom = 4 - (tiles.indexOf(0) / 4);
    return (inversions % 2 == 0) == (emptyRowFromBottom % 2 == 1);
}

void MainWindow::checkWin()
{
    bool win = true;
    int expected = 1;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == 3 && j == 3) {
                QLayoutItem *item = gridLayout->itemAtPosition(i, j);
                if (item && item->widget()) {
                    win = false;
                }
            } else {
                QLayoutItem *item = gridLayout->itemAtPosition(i, j);
                if (!item || !item->widget()) {
                    win = false;
                } else {
                    int value = item->widget()->property("text").toString().toInt();
                    if (value != expected) {
                        win = false;
                    }
                }
                expected++;
            }
        }
    }

    if (win) {
        gameTimer->stop();
        gameStarted = false;

        QMessageBox::information(this, "Поздравляем!",
                                 QString("Вы выиграли!\nХодов: %1\nВремя: %2:%3")
                                     .arg(moveCount)
                                     .arg(gameTime / 60, 2, 10, QChar('0'))
                                     .arg(gameTime % 60, 2, 10, QChar('0')));
    }
}

void MainWindow::newGame()
{
    gameTimer->stop();
    initGame();
}

void MainWindow::updateTimer()
{
    gameTime++;
    int minutes = gameTime / 60;
    int seconds = gameTime % 60;
    timerLabel->setText(
        QString("Время: %1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'))
        );
}
