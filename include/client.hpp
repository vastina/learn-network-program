#ifndef _CLIENT_SOCK_H_
#define _CLIENT_SOCK_H_

#include <iostream>
#include <format>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <future>
#include <queue>
#include <functional>
#include <chrono>
#include <fstream>
#include <deque>

#include <SFML/Graphics.hpp>

#define VASTINA_CPP
#include "tools.h"
#include "loger.hpp"
//#include "chatbox.hpp"
#include "Taskpool.hpp"


const char* localaddr = "127.0.0.1";

class client{
private:
    int clientsock;
    char readbuffer[4096], sendbuffer[4096];
    Taskpool tpool;
    std::condition_variable condition;
    struct _stop{
        bool stop;
        std::mutex smutex;
    } stop_;


    //ChatBox *chatbox;
    struct _chatbox{
        std::deque<std::string> messages;
        std::size_t maxMessages = 20; 
        const unsigned int fontSize = 15;
    } chatbox;
public:

    

    void init();
    void setclientsock(int af,int type,int protocol, short port);
    void connect_(int af,int type,int protocol, unsigned short port);
    template<typename F, typename...Args>
        void submit(F&& f, Args&&... args, int level);

    //void manage(const char* request);
    void send_message();
    void getfile(const char* filename);
    bool recive_message();
    void deal_message(sf::RenderWindow& window, const sf::Font& font);

    void test1();
    void test2();
    void fucksfml(std::string message,sf::RenderWindow& window, const sf::Font& font);
    
    void end(){
        tpool.setstoped();
        tpool.lastwork();
        close(clientsock);
    }

};

void client::test1(){
    while(true){
        printf("\nsend message> ");
        scanf("%s", sendbuffer);
        {
            std::lock_guard<std::mutex> l(stop_.smutex);
            if(quitjudge(sendbuffer))
            {
                stop_.stop = true;
            }
            condition.notify_one();
        }
        if(stop_.stop) return;
        send_message();
        showtime();
    }  
}

void client::test2(){
    sf::RenderWindow window{ sf::RenderWindow(sf::VideoMode(800,600), "fuck") };  
    sf::Font font;
    window.display();
    if (!font.loadFromFile("/usr/share/fonts/truetype/arial.ttf"))
        throw std::runtime_error("can not load arial.ttf"); 
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }
        {
            std::unique_lock<std::mutex> l(stop_.smutex);
            condition.wait(l);
            if(stop_.stop) break;
        }
        //window.clear();
        if(recive_message()){
            deal_message(window, font);
            //showtime();
        }
        //window.display(); 
    }
}

void client::init(){
    memset(readbuffer, 0, sizeof(readbuffer));
    memset(sendbuffer, 0, sizeof(readbuffer));
    //chatbox = new ChatBox();
    tpool.start();
}

void client::setclientsock(int af,int type,int protocol, short port = -1){
    clientsock = socket(af, type, protocol);
    struct sockaddr_in clientaddr;
    memset(&clientaddr, 0, sizeof(clientaddr));
    clientaddr.sin_family = af;
    clientaddr.sin_addr.s_addr = inet_addr(localaddr);
    if(port == -1)
	clientaddr.sin_port = htons(INADDR_ANY) ;
    else clientaddr.sin_port = htons(port);
    if(bind(clientsock, (struct sockaddr*)&clientaddr, sizeof(clientaddr))== -1){
        errorhandling("bind fail error code: {}\n", errno);
        submit([]{
            vastina_log::logtest("bind fail");
        }, importance::lowest);
    }   
}

void client::connect_(int af,int type,int protocol, unsigned short port = 8888){
    struct sockaddr_in serveaddr;
    memset(&serveaddr, 0, sizeof(serveaddr));
    serveaddr.sin_family = AF_INET;
    serveaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveaddr.sin_port = htons(port) ;
    if(connect(clientsock, (struct sockaddr*)&serveaddr, sizeof(serveaddr)) == -1){
        submit([]{
            vastina_log::logtest(std::format("connect fail {}\n", errno).c_str());
        }, importance::lowest);
    }
}

template<typename F, typename...Args>
void client::submit(F&& f, Args&&... args, int level){
    if(level == importance::maintk){
        //todo
    }
    else tpool.submittask(f, args..., level);
};

void client::send_message(){
    if (-1 == write(clientsock, sendbuffer, strlen(sendbuffer)))
    {
        submit([]{
            vastina_log::logtest(std::format("fail to send with code: {}\n", errno).c_str());
        }, importance::lowest);
    } 
}

void client::getfile(const char* filename){
    std::ofstream ofs;

    system(std::format("touch {}", filename).c_str());

    ofs.open(filename, std::ofstream::out);
    if(!ofs.is_open()){
        submit([]{
            vastina_log::logtest(std::format("can't open file, error code: {}", errno).c_str());
        }, importance::lowest);
        return ;
    } 
    {
        int cnt;    char temp[1024];
        memset(temp, 0, sizeof(temp));
        if( (cnt = read(clientsock, (void*)temp, sizeof(temp)) ) > 0)
        {
            if(cnt==1024) --cnt;
            temp[cnt] = 0x00;
            ofs.write(temp, cnt);
        }
        ofs.close();
        strcpy(readbuffer, "file recived\n");
        //print("file recived\n");
        //fucksfml("file recived\n");
    }
}

bool client::recive_message(){
    int bufferlen = -1;
    {
        if( (bufferlen = read(clientsock, (void*)readbuffer, sizeof(readbuffer))) > 0)
        {
            readbuffer[bufferlen] = '\0';
        }
    }
    if(bufferlen < 0) return false;
    return true;
}

void client::deal_message(sf::RenderWindow& window, const sf::Font& font){
    if(sendfile(readbuffer)){
        getfile("clientfile.txt");
    }
    else{
        
        //print("\nrecive message:{} \n", readbuffer);
    }
    fucksfml(std::format("recive message: {} ", readbuffer), window, font);
    memset(readbuffer, 0, sizeof(readbuffer));
}

void client::fucksfml(std::string message,sf::RenderWindow& window, const sf::Font& font){
    gettimebystring(message);
    chatbox.messages.push_back(message);
    if (chatbox.messages.size() > chatbox.maxMessages) {
        chatbox.messages.pop_front();
    }
    window.clear();
    for (std::size_t i = 0; i < chatbox.messages.size(); ++i) {
        sf::Text text(chatbox.messages[i], font, chatbox.fontSize);
        text.setPosition(10.0f, float(i * (2*chatbox.fontSize + 5) + 10));
        window.draw(text) ;
    }
    window.display(); 
}

#endif