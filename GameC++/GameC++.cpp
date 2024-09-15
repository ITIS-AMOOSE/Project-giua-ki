#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <string>
#include <sstream>

using namespace sf;

const int M = 23;
const int N = 10;
const int BlockSize = 30; // Kích thước của mỗi ô khối

int field[M][N] = { 0 };
int score = 0; // Biến lưu trữ điểm số

struct Point {
    int x, y;
} a[4], b[4];

int figures[7][4] = {
    1, 3, 5, 7, // I
    2, 4, 5, 7, // Z
    3, 5, 4, 6, // S
    3, 5, 4, 7, // T
    2, 3, 5, 7, // L
    3, 5, 7, 6, // J
    2, 3, 4, 5  // O
};

const int pieceColors[7] = {
    1, // Màu cho I
    2, // Màu cho Z
    3, // Màu cho S
    4, // Màu cho T
    5, // Màu cho L
    6, // Màu cho J
    7  // Màu cho O
};

bool check() {
    for (int i = 0; i < 4; i++)
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return false;
        else if (field[a[i].y][a[i].x]) return false;

    return true;
}

// Trạng thái trò chơi
bool isplaying = false;
bool isPaused = false;   // Biến trạng thái tạm dừng
bool isWaiting = true;   // Biến trạng thái màn hình chờ
bool countdownActive = true; // Biến trạng thái đếm ngược
float countdownTime = 3.0f; // Thời gian đếm ngược
Clock countdownClock; // Đồng hồ đếm ngược
bool isHolding = false; // Biến trạng thái hold
int holdPiece = -1; // Khối đã được hold (mặc định là -1 nghĩa là chưa hold)
bool canHold = true;
int currentPiece, nextPiece; // Mảnh hiện tại và tiếp theo

// Hàm reset game
void resetGame() {
    // Xóa bảng chơi (đặt tất cả về 0)
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            field[i][j] = 0;
        }
    }

    // Đặt lại biến điều khiển
    isplaying = true;  // Bắt đầu lại trò chơi
    score = 0;         // Đặt lại điểm số
    srand(static_cast<unsigned>(time(0)));
    float timer = 0, delay = 0.3;

    // Sinh ra mảnh mới ngẫu nhiên
    currentPiece = nextPiece; // Mảnh hiện tại
    nextPiece = rand() % 7;   // Mảnh tiếp theo
    int n = currentPiece;
    for (int i = 0; i < 4; i++) {
        a[i].x = figures[n][i] % 2;
        a[i].y = figures[n][i] / 2;
    }
    canHold = true;
    isHolding = false; // Đánh dấu chưa hold
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    RenderWindow window(VideoMode(600, 720), "Tetris");

    Texture t1, t2;
    t1.loadFromFile("../data/block.png");
    t2.loadFromFile("../data/BG.png");

    Sprite s(t1), background(t2);

    int dx = 0;
    bool rotate = false;
    currentPiece = rand() % 7;
    nextPiece = rand() % 7;
    float timer = 0, delay = 0.3;

    Clock clock;

    // Tính toán vị trí căn giữa cho background
    FloatRect backgroundBounds = background.getLocalBounds();
    float backgroundX = (window.getSize().x - backgroundBounds.width) / 2;
    float backgroundY = (window.getSize().y - backgroundBounds.height) / 2;
    background.setPosition(backgroundX, backgroundY);

    // Tính toán vị trí căn giữa cho các khối
    float offsetX = (window.getSize().x - (N * BlockSize)) / 2;
    float offsetY = (window.getSize().y - (M * BlockSize)) / 2;

    Font font;
    if (!font.loadFromFile("../data/MCR.ttf")) {
        return -1; // Thoát nếu không tải được font
    }

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        timer += deltaTime;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape && isplaying) {
                    isPaused = !isPaused;  // Đảo ngược trạng thái tạm dừng
                    if (isPaused) {
                        countdownActive = false; // Hủy đếm ngược khi tạm dừng
                    }
                }

                if (!isPaused && isWaiting && e.key.code == Keyboard::Enter) {
                    isWaiting = false;  // Thoát màn hình chờ, bắt đầu chơi
                    resetGame();        // Reset trò chơi khi bắt đầu
                }
                else if (isPaused && e.key.code == Keyboard::Enter) {
                    isPaused = false;  // Tiếp tục trò chơi khi đang ở trạng thái pause
                    countdownActive = true; // Bắt đầu đếm ngược
                    countdownTime = 3.0f;   // Đặt lại thời gian đếm ngược
                    countdownClock.restart(); // Khởi động đồng hồ đếm ngược
                }
                else if (!isplaying && e.key.code == Keyboard::Enter) {
                    resetGame();        // Reset trò chơi khi game over
                    isWaiting = false;  // Bắt đầu trò chơi ngay
                }

                if (!isPaused && isplaying) {
                    if (e.key.code == Keyboard::Up) rotate = true;
                    else if (e.key.code == Keyboard::Left) dx = -1;
                    else if (e.key.code == Keyboard::Right) dx = 1;
                }
            }

            if (e.key.code == Keyboard::Space && !isPaused && isplaying && canHold) {
                if (!isHolding) {
                    holdPiece = currentPiece; // Lưu khối hiện tại vào biến hold
                    currentPiece = nextPiece; // Đổi khối hiện tại
                    isHolding = true; // Đánh dấu đã hold
                    canHold = false; // Không cho phép hold lại cho đến lượt sau
                    nextPiece = rand() % 7; // Sinh khối mới
                }
                else {
                    int temp = holdPiece; // Lưu khối hold hiện tại
                    holdPiece = currentPiece; // Đổi khối hold
                    currentPiece = temp; // Đổi khối hiện tại
                }

                // Cập nhật mảnh mới
                int n = currentPiece;
                for (int i = 0; i < 4; i++) {
                    a[i].x = figures[n][i] % 2;
                    a[i].y = figures[n][i] / 2;
                }
                canHold = false; // Không cho phép hold lại cho đến lượt sau
            }
        }

        if (countdownActive) {
            // Cập nhật thời gian đếm ngược
            float elapsed = countdownClock.getElapsedTime().asSeconds();
            countdownTime -= elapsed;
            countdownClock.restart();

            if (countdownTime <= 0) {
                countdownTime = 0;
                countdownActive = false; // Kết thúc đếm ngược
            }
        }

        if (!isPaused && isplaying && !countdownActive) {
            if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;

            // Di chuyển theo chiều ngang
            for (int i = 0; i < 4; i++) {
                b[i] = a[i];
                a[i].x += dx;
            }
            if (!check()) {
                for (int i = 0; i < 4; i++) a[i] = b[i];
            }

            // Xoay mảnh
            if (rotate) {
                Point p = a[1]; // Tâm xoay
                for (int i = 0; i < 4; i++) {
                    int x = a[i].y - p.y;
                    int y = a[i].x - p.x;
                    a[i].x = p.x - x;
                    a[i].y = p.y + y;
                }
                if (!check()) {
                    for (int i = 0; i < 4; i++) a[i] = b[i];
                }
            }

            // Tăng mảnh xuống dần theo thời gian
            if (timer > delay) {
                for (int i = 0; i < 4; i++) {
                    b[i] = a[i];
                    a[i].y += 1;
                }

                if (!check()) {
                    for (int i = 0; i < 4; i++) {
                        field[b[i].y][b[i].x] = pieceColors[currentPiece];
                    }

                    // Sinh mảnh mới ngẫu nhiên
                    currentPiece = nextPiece;
                    nextPiece = rand() % 7; // Cập nhật mảnh tiếp theo

                    int n = currentPiece;
                    for (int i = 0; i < 4; i++) {
                        a[i].x = figures[n][i] % 2;
                        a[i].y = figures[n][i] / 2;
                    }

                    // Reset lại trạng thái hold
                    canHold = true; // Cho phép hold khi khối mới được sinh ra

                    // Kiểm tra va chạm ngay khi tạo mảnh mới
                    if (!check()) {
                        isplaying = false;  // Kết thúc trò chơi nếu va chạm ngay lập tức
                    }
                }

                timer = 0;
            }

            // Kiểm tra và loại bỏ dòng đã đầy
            int k = M - 1;
            for (int i = M - 1; i >= 0; i--) {
                int count = 0;
                for (int j = 0; j < N; j++) {
                    if (field[i][j]) count++;
                    field[k][j] = field[i][j];
                }
                if (count == N) {
                    score += 100; // Tăng điểm mỗi khi xóa 1 dòng
                }
                if (count < N) k--;
            }
            dx = 0; rotate = 0; delay = 0.3;
        }

        window.clear(Color::White);
        window.draw(background);

        if (isWaiting) {
            // Hiển thị màn hình chờ
            Text waitingText;
            waitingText.setFont(font);
            waitingText.setString("Press Enter to Start");
            waitingText.setCharacterSize(50);
            waitingText.setFillColor(Color::Blue);
            waitingText.setPosition(window.getSize().x / 2 - 250, window.getSize().y / 2 - 50);
            window.draw(waitingText);
        }
        else if (isPaused) {
            if (countdownActive) {
                // Hiển thị đếm ngược
                Text countdownText;
                std::stringstream ss;
                ss << static_cast<int>(countdownTime);
                countdownText.setString(ss.str());
                countdownText.setFont(font);
                countdownText.setCharacterSize(50);
                countdownText.setFillColor(Color::Red);
                countdownText.setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2 - 50);
                window.draw(countdownText);
            }
            else {
                // Hiển thị thông báo tạm dừng
                Text pauseText;
                pauseText.setFont(font);
                pauseText.setString("Game Paused\nPress Enter to Resume");
                pauseText.setCharacterSize(40);
                pauseText.setFillColor(Color::Green);
                pauseText.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2 - 50);
                window.draw(pauseText);
            }
        }
        else if (!isplaying) {
            // Hiển thị thông báo game over
            Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("                 Game Over\nPress Enter to Restart");
            gameOverText.setCharacterSize(50);
            gameOverText.setFillColor(Color::Red);
            gameOverText.setPosition(window.getSize().x / 2 - 295, window.getSize().y / 2 - 50);
            window.draw(gameOverText);
        }
        else {
            // Vẽ các khối
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    if (field[i][j] == 0) continue;
                    s.setTextureRect(IntRect(field[i][j] * 30, 0, 30, 30));
                    s.setPosition(j * BlockSize + offsetX, i * BlockSize + offsetY);
                    window.draw(s);
                }
            }

            for (int i = 0; i < 4; i++) {
                s.setTextureRect(IntRect(pieceColors[currentPiece] * 30, 0, 30, 30));
                s.setPosition(a[i].x * BlockSize + offsetX, a[i].y * BlockSize + offsetY);
                window.draw(s);
            }

            // Hiển thị điểm ở góc dưới bên trái
            Text scoreText;
            scoreText.setFont(font);
            scoreText.setString("Score: " + std::to_string(score));
            scoreText.setCharacterSize(24);
            scoreText.setFillColor(Color::Black);
            scoreText.setPosition(10, window.getSize().y - 50);
            window.draw(scoreText);

            if (holdPiece != -1) {
                // Hiển thị chữ "HOLD" ở góc trên bên trái
                Text holdText;
                holdText.setFont(font);
                holdText.setString("HOLD");
                holdText.setCharacterSize(40);
                holdText.setFillColor(Color::Black);
                holdText.setPosition(10, 10); // Vị trí hiển thị chữ "HOLD"
                window.draw(holdText);

                // Hiển thị khối đã hold dưới chữ "HOLD"
                for (int i = 0; i < 4; i++) {
                    int x = figures[holdPiece][i] % 2;
                    int y = figures[holdPiece][i] / 2;
                    s.setTextureRect(IntRect(pieceColors[holdPiece] * BlockSize, 0, BlockSize, BlockSize));
                    s.setPosition(x * BlockSize + 20, y * BlockSize + 70); // Đặt vị trí của khối hold bên dưới chữ "HOLD"
                    window.draw(s);
                }
            }

            // Hiển thị mảnh tiếp theo ở bên phải
            Text nextText;
            nextText.setFont(font);
            nextText.setString("NEXT");
            nextText.setCharacterSize(40);
            nextText.setFillColor(Color::Black);
            nextText.setPosition(window.getSize().x - 130, 10); // Vị trí hiển thị chữ "NEXT"
            window.draw(nextText);

            for (int i = 0; i < 4; i++) {
                int x = figures[nextPiece][i] % 2;
                int y = figures[nextPiece][i] / 2;
                s.setTextureRect(IntRect(pieceColors[nextPiece] * BlockSize, 0, BlockSize, BlockSize));
                s.setPosition(window.getSize().x - 130 + x * BlockSize, 60 + y * BlockSize); // Đặt vị trí của mảnh tiếp theo
                window.draw(s);
            }
        }

        window.display();
    }

    return 0;
}
