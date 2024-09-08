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

//stat 
bool isplaying = false;

int main() {
    srand(static_cast<unsigned>(time(0)));

    RenderWindow window(VideoMode(600, 720), "Tetris");

    Texture t1, t2;
    t1.loadFromFile("../data/block.png");
    t2.loadFromFile("../data/BG.png");

    Sprite s(t1), background(t2);

    int dx = 0;
    bool rotate = false;
    int currentPiece = 0;
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
            if (e.type == Event::KeyPressed && isplaying == true)
                if (e.key.code == Keyboard::Up) rotate = true;
                else if (e.key.code == Keyboard::Left) dx = -1;
                else if (e.key.code == Keyboard::Right) dx = 1;
        }

        if (isplaying == false && Keyboard::isKeyPressed(Keyboard::Enter))
        {
            isplaying = true;
        }

        if (isplaying) {
            if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;

            // Move
            for (int i = 0; i < 4; i++) {
                b[i] = a[i];
                a[i].x += dx;
            }
            if (!check()) {
                for (int i = 0; i < 4; i++) a[i] = b[i];
            }

            // Rotate
            if (rotate) {
                Point p = a[1]; // Center of rotation
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

            // Tick
            if (timer > delay) {
                for (int i = 0; i < 4; i++) {
                    b[i] = a[i];
                    a[i].y += 1;
                }

                if (!check()) {
                    for (int i = 0; i < 4; i++) {
                        field[b[i].y][b[i].x] = pieceColors[currentPiece];
                    }

                    currentPiece = rand() % 7;
                    int n = currentPiece;
                    for (int i = 0; i < 4; i++) {
                        a[i].x = figures[n][i] % 2;
                        a[i].y = figures[n][i] / 2;
                    }
                }

                timer = 0;
            }

            // Check lines
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
        if (isplaying)
        {
            // Draw
            window.clear(Color::White);
            window.draw(background);

            // Draw field
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    if (field[i][j] == 0) continue;
                    s.setTextureRect(IntRect(field[i][j] * BlockSize, 0, BlockSize, BlockSize));
                    s.setPosition(j * BlockSize + offsetX, i * BlockSize + offsetY);
                    window.draw(s);
                }
            }

            // Draw current piece
            for (int i = 0; i < 4; i++) {
                s.setTextureRect(IntRect(pieceColors[currentPiece] * BlockSize, 0, BlockSize, BlockSize));
                s.setPosition(a[i].x * BlockSize + offsetX, a[i].y * BlockSize + offsetY);
                window.draw(s);
            }
        }
        

        window.display();
    }

    return 0;
}