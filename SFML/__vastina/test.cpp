#define VASTINA_CPP
#include "../../include/tools.h"
#include "../../include/chatbox.hpp"
#include <chrono>
#include <thread>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Chat Box");

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/arial.ttf")) {
        // Handle font loading error
        return 1;
    }

    ChatBox chatBox;

    while (window.isOpen()) {
        // sf::Event event;
        // while (window.pollEvent(event)) {
        //     if (event.type == sf::Event::Closed) {
        //         window.close();
        //     }
        // }

        // Simulate receiving a message
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::string receivedMessage = "";
        gettimebystring(receivedMessage);
        std::cout << receivedMessage.c_str() << '\n';
        chatBox.addnewMessage(receivedMessage, window, font);

        window.clear();
        chatBox.draw(window, font);
        window.display();
    }

    return 0;
}