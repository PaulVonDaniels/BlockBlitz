/*
            Tetris is written by Paul Danilchenko
             ______   ___ ______ ____  ____ _____
            |      | /  _|      |    \l    / ___/
            |      |/  [_|      |  D  )|  (   \_ 
            l_j  l_|    _l_j  l_|    / |  |\__  T
              |  | |   [_  |  | |    \ |  |/  \ |
              |  | |     T |  | |  .  Yj  l\    |
              l__j l_____j l__j l__j\_|____j\___j

---
This game was written in C++.
The Qt framework was also used.

	-h, --help     Show this message
	-v, --version  Show game version and contact info

Email: www.FACEMASH@gmail.com
Telegram: @vonDaniels 

              Thank you for playing this game <3
---

*/


#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QTime>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsEffect>
#include <QFile>
#include <vector>
#include <utility>


class Database 
{
private:
    std::vector<std::pair<QString, QString>> users;
    
public:
    Database() 
    {
        users.push_back({"admin", "1234"});
        users.push_back({"player", "game"});
    }
    
    bool registerUser(const QString& username, const QString& password) 
    {
        for (const auto& user : users) 
        {
            if (user.first == username) 
            {
                return false;
            }
        }
        users.push_back({username, password});
        return true;
    }
    
    bool authenticate(const QString& username, const QString& password) 
    {
        for (const auto& user : users) 
        {
            if (user.first == username && user.second == password) 
            {
                return true;
            }
        }
        return false;
    }
};

class LoginDialog : public QDialog 
{
    Q_OBJECT
public:
    LoginDialog(QWidget* parent = nullptr) : QDialog(parent) 
    {
        setWindowTitle("Tetris - Login");
        setFixedSize(350, 250);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        QLabel* titleLabel = new QLabel("Welcome to Tetris", this);
        titleLabel->setAlignment(Qt::AlignCenter);
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        
        QFormLayout* formLayout = new QFormLayout();
        
        usernameEdit = new QLineEdit(this);
        passwordEdit = new QLineEdit(this);
        passwordEdit->setEchoMode(QLineEdit::Password);
        
        errorLabel = new QLabel(this);
        errorLabel->setStyleSheet("color: #ff453a; font-size: 12px;");
        errorLabel->setWordWrap(true);
        errorLabel->setVisible(false);
        
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        
        formLayout->addRow("Username:", usernameEdit);
        formLayout->addRow("Password:", passwordEdit);
        
        mainLayout->addWidget(titleLabel);
        mainLayout->addSpacing(10);
        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(errorLabel);
        mainLayout->addSpacing(10);
        mainLayout->addWidget(buttonBox);
        
        connect(buttonBox, &QDialogButtonBox::accepted, this, &LoginDialog::validateAndAccept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &LoginDialog::reject);
        connect(usernameEdit, &QLineEdit::returnPressed, this, &LoginDialog::focusPassword);
        connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::validateAndAccept);
        
        usernameEdit->setFocus();
    }
    
    QString getUsername() const 
    { 
        return usernameEdit->text(); 
    }
    
    QString getPassword() const 
    { 
        return passwordEdit->text(); 
    }
    
private slots:
    void focusPassword() 
    {
        passwordEdit->setFocus();
    }
    
    void validateAndAccept() 
    {
        QString username = usernameEdit->text().trimmed();
        QString password = passwordEdit->text();
        
        if (username.isEmpty() && password.isEmpty()) 
        {
            showError("Please enter both username and password");
            usernameEdit->setFocus();
            return;
        }
        
        if (username.isEmpty()) 
        {
            showError("Please enter your username");
            usernameEdit->setFocus();
            return;
        }
        
        if (password.isEmpty()) 
        {
            showError("Please enter your password");
            passwordEdit->setFocus();
            return;
        }
        
        hideError();
        accept();
    }
    
private:
    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QLabel* errorLabel;
    
    void showError(const QString& message) 
    {
        errorLabel->setText(message);
        errorLabel->setVisible(true);
        
        usernameEdit->setStyleSheet(usernameEdit->text().isEmpty() ? 
            "border: 1px solid #ff453a; border-radius: 6px;" : "border: 1px solid #c6c6c6; border-radius: 6px;");
        passwordEdit->setStyleSheet(passwordEdit->text().isEmpty() ? 
            "border: 1px solid #ff453a; border-radius: 6px;" : "border: 1px solid #c6c6c6; border-radius: 6px;");
    }
    
    void hideError() 
    {
        errorLabel->setVisible(false);
        usernameEdit->setStyleSheet("border: 1px solid #c6c6c6; border-radius: 6px;");
        passwordEdit->setStyleSheet("border: 1px solid #c6c6c6; border-radius: 6px;");
    }
};

class TetrisWidget : public QWidget 
{
    Q_OBJECT
public:
    TetrisWidget(const QString& playerName, QWidget *parent = nullptr) : QWidget(parent), playerName(playerName) 
    {
        setFixedSize(400, 600);
        setFocusPolicy(Qt::StrongFocus);
        
        gameTimer = new QTimer(this);
        connect(gameTimer, &QTimer::timeout, this, &TetrisWidget::updateGame);
        
        challengeTimer = new QTimer(this);
        connect(challengeTimer, &QTimer::timeout, this, &TetrisWidget::updateChallengeTime);
        
        animationTimer = new QTimer(this);
        connect(animationTimer, &QTimer::timeout, this, &TetrisWidget::updateAnimation);
        animationTimer->start(50);
        
        comboResetTimer = new QTimer(this);
        comboResetTimer->setSingleShot(true);
        connect(comboResetTimer, &QTimer::timeout, this, [this]() 
        { 
            comboCount = 0; 
        });
        
        initGame();
    }

    void startGame(const QString &mode) 
    {
        gameMode = mode;
        initGame();
        
        if (mode == "Classic") 
        {
            speed = 1000;
            gameTimer->start(speed);
        } 
        else if (mode == "Marathon") 
        {
            speed = 800;
            gameTimer->start(speed);
        } 
        else if (mode == "Challenge") 
        {
            speed = 700;
            gameTimer->start(speed);
            challengeTimeLeft = 60;
            challengeTimer->start(1000);
            createChallengeLevel();
        }
        
        gameOver = false;
        setFocus();
        update();
    }

    void showGameOverMenu() 
    {
        gameTimer->stop();
        challengeTimer->stop();
        
        QDialog gameOverDialog(this);
        gameOverDialog.setWindowTitle("Game Over");
        gameOverDialog.setFixedSize(300, 200);
        
        QVBoxLayout layout(&gameOverDialog);
        
        QLabel titleLabel("Game Over!", &gameOverDialog);
        titleLabel.setAlignment(Qt::AlignCenter);
        QFont titleFont = titleLabel.font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        titleLabel.setFont(titleFont);
        
        QLabel scoreLabel(QString("Final Score: %1").arg(score), &gameOverDialog);
        scoreLabel.setAlignment(Qt::AlignCenter);
        
        QLabel levelLabel(QString("Level Reached: %1").arg(level), &gameOverDialog);
        levelLabel.setAlignment(Qt::AlignCenter);
        
        QHBoxLayout buttonLayout;
        QPushButton restartButton("Play Again", &gameOverDialog);
        QPushButton menuButton("Main Menu", &gameOverDialog);
        QPushButton quitButton("Quit", &gameOverDialog);
        
        buttonLayout.addWidget(&restartButton);
        buttonLayout.addWidget(&menuButton);
        buttonLayout.addWidget(&quitButton);
        
        layout.addWidget(&titleLabel);
        layout.addWidget(&scoreLabel);
        layout.addWidget(&levelLabel);
        layout.addSpacing(20);
        layout.addLayout(&buttonLayout);
        
        connect(&restartButton, &QPushButton::clicked, [&]() 
        {
            gameOverDialog.accept();
            startGame(gameMode);
        });
        
        connect(&menuButton, &QPushButton::clicked, [&]() 
        {
            gameOverDialog.accept();
            showMainMenu();
        });
        
        connect(&quitButton, &QPushButton::clicked, [&]() 
        {
            gameOverDialog.accept();
            QApplication::quit();
        });
        
        gameOverDialog.exec();
    }

    void showMainMenu() 
    {
        QDialog menuDialog(this);
        menuDialog.setWindowTitle("Tetris");
        menuDialog.setFixedSize(300, 300);
        
        QVBoxLayout layout(&menuDialog);
        
        QLabel titleLabel("TETRIS", &menuDialog);
        titleLabel.setAlignment(Qt::AlignCenter);
        QFont titleFont = titleLabel.font();
        titleFont.setPointSize(24);
        titleFont.setBold(true);
        titleLabel.setFont(titleFont);
        
        QLabel playerLabel(QString("Player: %1").arg(playerName), &menuDialog);
        playerLabel.setAlignment(Qt::AlignCenter);
        
        QVBoxLayout buttonLayout;
        QPushButton classicButton("Classic Mode", &menuDialog);
        QPushButton marathonButton("Marathon Mode", &menuDialog);
        QPushButton challengeButton("Challenge Mode", &menuDialog);
        QPushButton quitButton("Quit Game", &menuDialog);
        
        classicButton.setFixedHeight(40);
        marathonButton.setFixedHeight(40);
        challengeButton.setFixedHeight(40);
        quitButton.setFixedHeight(40);
        
        buttonLayout.addWidget(&classicButton);
        buttonLayout.addWidget(&marathonButton);
        buttonLayout.addWidget(&challengeButton);
        buttonLayout.addWidget(&quitButton);
        
        layout.addWidget(&titleLabel);
        layout.addWidget(&playerLabel);
        layout.addSpacing(20);
        layout.addLayout(&buttonLayout);
        
        connect(&classicButton, &QPushButton::clicked, [&]() 
        {
            menuDialog.accept();
            startGame("Classic");
        });
        
        connect(&marathonButton, &QPushButton::clicked, [&]() 
        {
            menuDialog.accept();
            startGame("Marathon");
        });
        
        connect(&challengeButton, &QPushButton::clicked, [&]() 
        {
            menuDialog.accept();
            startGame("Challenge");
        });
        
        connect(&quitButton, &QPushButton::clicked, [&]() 
        {
            menuDialog.accept();
            QApplication::quit();
        });
        
        menuDialog.exec();
    }

protected:
    void paintEvent(QPaintEvent *) override 
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.fillRect(0, 0, width(), height(), Qt::black);
        
        painter.setPen(QColor(50, 50, 50));
        for (int x = 0; x <= boardWidth; ++x) 
        {
            painter.drawLine(x * blockSize, 0, x * blockSize, height());
        }
        for (int y = 0; y <= boardHeight; ++y) 
        {
            painter.drawLine(0, y * blockSize, boardWidth * blockSize, y * blockSize);
        }
        
        for (int y = 0; y < boardHeight; ++y) 
        {
            for (int x = 0; x < boardWidth; ++x) 
            {
                if (board[y][x]) 
                {
                    if (board[y][x] == 9) 
                    {
                        painter.setBrush(QColor(255, 165, 0));
                    } 
                    else 
                    {
                        painter.setBrush(getColor(board[y][x]));
                    }
                    painter.setPen(Qt::black);
                    painter.drawRect(x * blockSize, y * blockSize, blockSize - 1, blockSize - 1);
                    
                    painter.setPen(Qt::white);
                    painter.drawLine(x * blockSize, y * blockSize, (x + 1) * blockSize - 1, y * blockSize);
                    painter.drawLine(x * blockSize, y * blockSize, x * blockSize, (y + 1) * blockSize - 1);
                }
            }
        }

        if (!gameOver) 
        {
            drawShadow(painter);
            
            for (const auto &p : currentPiece) 
            {
                int x = currentX + p.x();
                int y = currentY + p.y();
                if (y >= 0) 
                {
                    painter.setBrush(getColor(currentPieceType + 1));
                    painter.setPen(Qt::black);
                    painter.drawRect(x * blockSize, y * blockSize, blockSize - 1, blockSize - 1);
                    
                    painter.setPen(Qt::white);
                    painter.drawLine(x * blockSize, y * blockSize, (x + 1) * blockSize - 1, y * blockSize);
                    painter.drawLine(x * blockSize, y * blockSize, x * blockSize, (y + 1) * blockSize - 1);
                }
            }
        }

        painter.setPen(Qt::white);
        painter.drawText(320, 30, "Next:");
        for (const auto &p : nextPiece) 
        {
            painter.setBrush(getColor(nextPieceType + 1));
            painter.setPen(Qt::black);
            painter.drawRect(320 + p.x() * blockSize, 50 + p.y() * blockSize, blockSize - 1, blockSize - 1);
        }

        painter.setPen(Qt::white);
        painter.drawText(10, 20, QString("Player: %1").arg(playerName));
        painter.drawText(320, 200, QString("Score: %1").arg(score));
        painter.drawText(320, 220, QString("Level: %1").arg(level));
        painter.drawText(320, 240, QString("Lines: %1").arg(linesClearedTotal));
        painter.drawText(320, 260, QString("Mode: %1").arg(gameMode));
        
        if (comboCount > 1) 
        {
            painter.setPen(Qt::yellow);
            painter.drawText(320, 280, QString("COMBO x%1!").arg(comboCount));
        }
        
        if (gameMode == "Challenge") 
        {
            painter.setPen(challengeTimeLeft <= 10 ? Qt::red : Qt::white);
            painter.drawText(320, 300, QString("Time: %1").arg(challengeTimeLeft));
            painter.drawText(320, 320, QString("Target: %1/10").arg(challengeLevel));
        }
        
        if (gameOver) 
        {
            QFont font = painter.font();
            font.setPointSize(24);
            font.setBold(true);
            painter.setFont(font);
            
            QLinearGradient gradient(0, 0, width(), 0);
            gradient.setColorAt(0, Qt::red);
            gradient.setColorAt(0.5, Qt::yellow);
            gradient.setColorAt(1, Qt::red);
            painter.setPen(QPen(gradient, 3));
            
            painter.drawText(100, 280, "GAME OVER");
            
            font.setPointSize(14);
            painter.setFont(font);
            painter.setPen(Qt::white);
            painter.drawText(120, 310, "Opening menu...");
            
            if (specialEffect) 
            {
                painter.setPen(Qt::cyan);
                painter.drawText(80, 340, "Great job! Try again!");
            }
        }
    }

    void keyPressEvent(QKeyEvent *event) override 
    {
        if (gameOver) return;
        
        switch (event->key()) 
        {
        case Qt::Key_A:
        case Qt::Key_Left: 
            movePiece(-1, 0); 
            break;
            
        case Qt::Key_D:
        case Qt::Key_Right: 
            movePiece(1, 0); 
            break;
            
        case Qt::Key_S:
            movePieceOneStepDown();
            break;
            
        case Qt::Key_W:
        case Qt::Key_Up:
            rotatePieceClockwise(); 
            break;
            
        case Qt::Key_Q:
        case Qt::Key_Z:
            rotatePieceCounterClockwise();
            break;
            
        case Qt::Key_Space:
            dropPiece(); 
            break;
            
        case Qt::Key_Escape:
            showMainMenu();
            break;
        }
        update();
    }

private slots:
    void updateGame() 
    {
        if (!gameOver) 
        {
            if (!movePiece(0, 1)) 
            {
                placePiece();
                if (!spawnPiece()) 
                {
                    gameOver = true;
                    specialEffect = (score > 5000);
                    QTimer::singleShot(1000, this, &TetrisWidget::showGameOverMenu);
                }
            }
            update();
        }
    }
    
    void updateChallengeTime() 
    {
        if (gameMode == "Challenge" && !gameOver) 
        {
            challengeTimeLeft--;
            if (challengeTimeLeft <= 0) 
            {
                gameOver = true;
                challengeTimer->stop();
                QMessageBox::information(this, "Challenge Failed", "Time's up! You didn't complete the level.");
                QTimer::singleShot(1000, this, &TetrisWidget::showGameOverMenu);
            }
            update();
        }
    }

    void updateAnimation() 
    {
        update();
    }

private:
    static const int boardWidth = 10, boardHeight = 20, blockSize = 30;
    std::vector<std::vector<int>> board;
    std::vector<QPoint> currentPiece, nextPiece;
    int currentPieceType, nextPieceType;
    int currentX, currentY;
    QTimer *gameTimer, *challengeTimer, *animationTimer, *comboResetTimer;
    int score, level, speed, linesClearedTotal;
    bool gameOver;
    QString gameMode;
    QString playerName;
    int challengeTimeLeft;
    int challengeLevel;
    int comboCount;
    bool specialEffect;

    void initGame() 
    {
        board.assign(boardHeight, std::vector<int>(boardWidth, 0));
        score = 0;
        level = 1;
        linesClearedTotal = 0;
        gameOver = false;
        challengeLevel = 1;
        comboCount = 0;
        specialEffect = false;
        nextPieceType = QRandomGenerator::global()->bounded(7);
        spawnPiece();
        generateNextPiece();
    }
    
    void createChallengeLevel() 
    {
        for (int y = boardHeight - 1; y >= boardHeight - challengeLevel; --y) 
        {
            for (int x = 0; x < boardWidth; ++x) 
            {
                if (QRandomGenerator::global()->bounded(100) < 60) 
                {
                    board[y][x] = 9;
                }
            }
        }
    }
    
    void drawShadow(QPainter &painter) 
    {
        int shadowY = currentY;
        while (isValidPosition(currentPiece, currentX, shadowY + 1)) 
        {
            shadowY++;
        }
        
        if (shadowY > currentY) 
        {
            painter.setBrush(QColor(100, 100, 100, 150));
            painter.setPen(Qt::NoPen);
            for (const auto &p : currentPiece) 
            {
                int x = currentX + p.x();
                int y = shadowY + p.y();
                if (y >= 0) 
                {
                    painter.drawRect(x * blockSize, y * blockSize, blockSize - 1, blockSize - 1);
                }
            }
        }
    }

    bool spawnPiece() 
    {
        currentPieceType = nextPieceType;
        currentPiece = getPiece(currentPieceType);
        currentX = boardWidth / 2 - 1;
        currentY = 0;
        
        if (!isValidPosition(currentPiece, currentX, currentY)) 
        {
            return false;
        }
        
        nextPieceType = QRandomGenerator::global()->bounded(7);
        generateNextPiece();
        return true;
    }

    void generateNextPiece() 
    {
        nextPiece = getPiece(nextPieceType);
    }

    std::vector<QPoint> getPiece(int type) 
    {
        static const std::vector<std::vector<QPoint>> pieces = 
        {
            {{0,1}, {1,1}, {2,1}, {3,1}},
            {{0,0}, {1,0}, {0,1}, {1,1}},
            {{0,1}, {1,1}, {2,1}, {1,0}},
            {{1,0}, {2,0}, {0,1}, {1,1}},
            {{0,0}, {1,0}, {1,1}, {2,1}},
            {{0,1}, {1,1}, {2,1}, {2,0}},
            {{0,1}, {1,1}, {2,1}, {0,0}}
        };
        return pieces[type];
    }

    bool isValidPosition(const std::vector<QPoint> &piece, int x, int y) 
    {
        for (const auto &p : piece) 
        {
            int newX = x + p.x();
            int newY = y + p.y();
            if (newX < 0 || newX >= boardWidth || newY >= boardHeight) 
                return false;
            if (newY >= 0 && board[newY][newX] && board[newY][newX] != 9)
                return false;
        }
        return true;
    }

    bool movePiece(int dx, int dy) 
    {
        if (isValidPosition(currentPiece, currentX + dx, currentY + dy)) 
        {
            currentX += dx;
            currentY += dy;
            return true;
        }
        return false;
    }

    void movePieceOneStepDown() 
    {
        if (isValidPosition(currentPiece, currentX, currentY + 1)) 
        {
            currentY += 1;
        } 
        else 
        {
            placePiece();
            if (!spawnPiece()) 
            {
                gameOver = true;
                specialEffect = (score > 5000);
                QTimer::singleShot(1000, this, &TetrisWidget::showGameOverMenu);
            }
        }
    }
    
    void rotatePieceClockwise() 
    {
        if (currentPieceType == 1) return;
        
        std::vector<QPoint> rotated;
        for (const auto &p : currentPiece) 
        {
            rotated.push_back(QPoint(p.y(), -p.x()));
        }
        
        if (isValidPosition(rotated, currentX, currentY)) 
        {
            currentPiece = rotated;
        } 
        else 
        {
            tryWallKick(rotated);
        }
    }

    void rotatePieceCounterClockwise() 
    {
        if (currentPieceType == 1) return;
        
        std::vector<QPoint> rotated;
        for (const auto &p : currentPiece) 
        {
            rotated.push_back(QPoint(-p.y(), p.x()));
        }
        
        if (isValidPosition(rotated, currentX, currentY)) 
        {
            currentPiece = rotated;
        } 
        else 
        {
            tryWallKick(rotated);
        }
    }

    bool tryWallKick(const std::vector<QPoint> &rotatedPiece) 
    {
        const std::vector<QPoint> kicks = 
        {
            QPoint(0, 0), QPoint(-1, 0), QPoint(1, 0), QPoint(0, -1), QPoint(0, 1)
        };
        
        for (const auto &kick : kicks) 
        {
            if (isValidPosition(rotatedPiece, currentX + kick.x(), currentY + kick.y())) 
            {
                currentX += kick.x();
                currentY += kick.y();
                currentPiece = rotatedPiece;
                return true;
            }
        }
        return false;
    }

    void dropPiece() 
    {
        int dropHeight = 0;
        while (movePiece(0, 1)) 
        {
            dropHeight++;
        }
        if (dropHeight > 0) 
        {
            score += dropHeight * 2;
        }
        placePiece();
        if (!spawnPiece()) 
        {
            gameOver = true;
            specialEffect = (score > 5000);
            QTimer::singleShot(1000, this, &TetrisWidget::showGameOverMenu);
        }
        update();
    }

    void placePiece() 
    {
        for (const auto &p : currentPiece) 
        {
            int x = currentX + p.x();
            int y = currentY + p.y();
            if (y >= 0) 
            {
                board[y][x] = currentPieceType + 1;
            }
        }
        clearLines();
    }

    void clearLines() 
    {
        int linesCleared = 0;
        
        for (int y = boardHeight - 1; y >= 0; --y) 
        {
            bool fullLine = true;
            
            for (int x = 0; x < boardWidth; ++x) 
            {
                if (board[y][x] == 0) 
                {
                    fullLine = false;
                    break;
                }
            }
            
            if (fullLine) 
            {
                linesCleared++;
                linesClearedTotal++;
                
                for (int yy = y; yy > 0; --yy) 
                {
                    for (int x = 0; x < boardWidth; ++x) 
                    {
                        board[yy][x] = board[yy - 1][x];
                    }
                }
                
                for (int x = 0; x < boardWidth; ++x) 
                {
                    board[0][x] = 0;
                }
                
                y++;
            }
        }
        
        if (linesCleared > 0) 
        {
            comboCount++;
            comboResetTimer->start(3000);
            
            int lineBonus = 0;
            switch (linesCleared) 
            {
                case 1: lineBonus = 100; break;
                case 2: lineBonus = 300; break;
                case 3: lineBonus = 500; break;
                case 4: lineBonus = 800; break;
            }
            
            int comboBonus = comboCount > 1 ? comboCount * 50 : 0;
            score += (lineBonus + comboBonus) * level;
            level = score / 1000 + 1;
            
            if (gameMode == "Marathon") 
            {
                speed = std::max(100, 800 - (level - 1) * 50);
                gameTimer->setInterval(speed);
            }
        } 
        else 
        {
            comboCount = 0;
        }
        
        if (gameMode == "Challenge") 
        {
            checkChallengeCompletion();
        }
    }
    
    void checkChallengeCompletion() 
    {
        bool challengeComplete = true;
        for (int y = 0; y < boardHeight; ++y) 
        {
            for (int x = 0; x < boardWidth; ++x) 
            {
                if (board[y][x] == 9) 
                {
                    challengeComplete = false;
                    break;
                }
            }
            if (!challengeComplete) break;
        }
        
        if (challengeComplete) 
        {
            if (challengeLevel < 10) 
            {
                challengeLevel++;
                int timeBonus = challengeTimeLeft * 10;
                score += timeBonus;
                QMessageBox::information(this, "Level Complete", 
                    QString("Level %1 completed! Time bonus: +%2 points").arg(challengeLevel-1).arg(timeBonus));
                initGame();
                createChallengeLevel();
                challengeTimeLeft = 60;
            } 
            else 
            {
                gameOver = true;
                challengeTimer->stop();
                QMessageBox::information(this, "Challenge Complete", 
                    "Congratulations! You completed all 10 challenge levels!");
                QTimer::singleShot(1000, this, &TetrisWidget::showGameOverMenu);
            }
        }
    }

    QColor getColor(int type) 
    {
        static const std::vector<QColor> colors = 
        {
            Qt::black,
            Qt::cyan,
            Qt::yellow,
            Qt::magenta,
            Qt::green,
            Qt::red,
            Qt::blue,
            Qt::gray
        };
        return colors[type % colors.size()];
    }
};

int main(int argc, char *argv[]) 
{
    QApplication app(argc, argv);
    
    Database db;
    QString username;
    bool authenticated = false;
    
    while (!authenticated) 
    {
        LoginDialog loginDialog;
        if (loginDialog.exec() == QDialog::Accepted) 
        {
            username = loginDialog.getUsername();
            QString password = loginDialog.getPassword();
            
            if (db.authenticate(username, password)) 
            {
                authenticated = true;
            } 
            else 
            {
                if (db.registerUser(username, password)) 
                {
                    authenticated = true;
                } 
                else 
                {
                    QMessageBox::warning(nullptr, "Error", "Authentication failed!");
                }
            }
        } 
        else 
        {
            return 0;
        }
    }
    
    QFile styleFile("STYLE_WIDGETS.qss");
    if (styleFile.open(QFile::ReadOnly)) 
    {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
    }
    
    TetrisWidget widget(username);
    widget.setWindowTitle("Tetris");
    widget.show();
    widget.showMainMenu();

    return app.exec();
}

#include "main.moc"
