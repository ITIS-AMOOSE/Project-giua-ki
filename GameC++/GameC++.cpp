#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>

using namespace sf;

const int M = 23;
const int N = 10;
const int BlockSize = 30; // Kích thước của mỗi ô khối

int field[M][N] = { 0 };

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
bool isWaiting = true; // Biến trạng thái màn hình chờ

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
    srand(static_cast<unsigned>(time(0)));
    float timer = 0, delay = 0.3;

    // Sinh ra mảnh mới ngẫu nhiên
    int currentPiece = rand() % 7;
    int n = currentPiece;
    for (int i = 0; i < 4; i++) {
        a[i].x = figures[n][i] % 2;
        a[i].y = figures[n][i] / 2;
    }
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
    int currentPiece = rand() % 7;
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

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();
            if (e.type == Event::KeyPressed) {
                if (isWaiting && e.key.code == Keyboard::Enter) {
                    isWaiting = false;  // Thoát màn hình chờ, bắt đầu chơi
                    resetGame();  // Reset trò chơi khi bắt đầu
                }
                else if (isplaying == true) {
                    if (e.key.code == Keyboard::Up) rotate = true;
                    else if (e.key.code == Keyboard::Left) dx = -1;
                    else if (e.key.code == Keyboard::Right) dx = 1;
                }
            }
        }

        if (isplaying == false && Keyboard::isKeyPressed(Keyboard::Enter)) {
            resetGame();  // Reset lại trò chơi
        }

        if (isplaying) {
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
                    currentPiece = rand() % 7;
                    int n = currentPiece;
                    for (int i = 0; i < 4; i++) {
                        a[i].x = figures[n][i] % 2;
                        a[i].y = figures[n][i] / 2;
                    }

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
                if (count < N) k--;
            }
            dx = 0; rotate = 0; delay = 0.3;
        }

        window.clear(Color::White);
        window.draw(background);

        if (isWaiting) {
            // Hiển thị màn hình chờ
            Font font;
            if (font.loadFromFile("../data/MCR.ttf")) {  // Tải font từ tệp
                Text waitingText;
                waitingText.setFont(font);
                waitingText.setString("Press Enter to Start");
                waitingText.setCharacterSize(50);
                waitingText.setFillColor(Color::Blue);
                waitingText.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2 - 50);
                window.draw(waitingText);
            }
        }
        else {
            // Vẽ các khối trên bảng
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    if (field[i][j] == 0) continue;
                    s.setTextureRect(IntRect(field[i][j] * BlockSize, 0, BlockSize, BlockSize));
                    s.setPosition(j * BlockSize + offsetX, i * BlockSize + offsetY);
                    window.draw(s);
                }
            }

            // Vẽ mảnh hiện tại
            for (int i = 0; i < 4; i++) {
                s.setTextureRect(IntRect(pieceColors[currentPiece] * BlockSize, 0, BlockSize, BlockSize));
                s.setPosition(a[i].x * BlockSize + offsetX, a[i].y * BlockSize + offsetY);
                window.draw(s);
            }

            // Hiển thị thông báo "Game Over" nếu trò chơi kết thúc
            if (!isplaying) {
                Font font;
                if (font.loadFromFile("../data/MCR.ttf")) {  // Tải font từ tệp
                    Text gameOverText;
                    gameOverText.setFont(font);
                    gameOverText.setString("Game Over! Press Enter to Restart");
                    gameOverText.setCharacterSize(40);
                    gameOverText.setFillColor(Color::Red);
                    gameOverText.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2 - 50);
                    window.draw(gameOverText);
                }
            }
        }

        window.display();
    }

    return 0;
}
