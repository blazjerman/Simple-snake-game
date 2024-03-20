#include <iostream>
#include <random>
#include <ncurses.h>
#include <csignal>


struct Vec2 {

    uint32_t x, y;

    [[nodiscard]] bool equal(const Vec2 &to) const {
        return (x == to.x && y == to.y);
    }

    void print(char c) const {
        std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H" << c << std::endl;
    }

};



struct BodyPart {

    Vec2 position{};
    BodyPart *tail{};

    BodyPart *popLast() {

        if (!tail) return this;

        BodyPart *last = tail->popLast();
        if(last == tail) tail = nullptr;
        return last;

    }

    [[nodiscard]] bool collision(const Vec2 &collide) const {

        if (position.equal(collide)) return true;
        if (!tail) return false;
        return tail->collision(collide);

    }

};

class Game {

private:

    BodyPart* head;
    const uint32_t size;
    Vec2 apple{};

    char lastKey = 'd';

    void shuffleApple() {

        Vec2 oldApple = apple;

        while(head->collision(apple) || apple.equal(oldApple)) {
            apple = {std::rand() & size, std::rand() & size};
        }

        apple.print('o');
    }

public:

    explicit Game(const uint32_t size) : size(size) {
        head = new BodyPart;
        shuffleApple();
    }

    bool play(char key) {

        Vec2 newHeadPosition = head->position;

        switch (key) {
            case 'w': newHeadPosition.y = (newHeadPosition.y - 1) & size;break;
            case 's': newHeadPosition.y = (newHeadPosition.y + 1) & size;break;
            case 'a': newHeadPosition.x = (newHeadPosition.x - 1) & size;break;
            case 'd': newHeadPosition.x = (newHeadPosition.x + 1) & size;break;
            default: return play(lastKey);
        }

        lastKey = key;

        BodyPart *newHead;

        if (apple.equal(newHeadPosition)) {
            newHead = new BodyPart;
            shuffleApple();
        } else {
            newHead = head->popLast();          //Reuse last tail
            newHead->position.print(' ');    //Remove last tail from terminal
        }

        newHead->position = newHeadPosition;
        newHead->position.print('x');

        if (head == newHead) return false;  //Check if snake is length of 1 (so that we cannot create loop in "BodyPart linked list")
        newHead->tail = head;
        head = newHead;

        return head->tail->collision(newHeadPosition);  //We know that head->tail cannot be null.

    }
};



int main() {

    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    timeout(0);
    getch();

    Game b(0x3);

    while (!b.play(getch())) usleep(300000);

    return 0;
}
