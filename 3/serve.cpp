#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <future>

#define VASTINA_CPP
#include "../include/tools.h"
//#include "../include/ThreadPool.hpp"
//#include "../include/threadtest.hpp"

int main(){
    //ThreadPool pool(4);
    std::vector< std::future<int> > results{};
    std::vector< std::thread> workers{};
    std::vector< std::packaged_task<int(int)> > tasks{};

    for(int i = 0; i < 4; ++i) {
        tasks.emplace_back(std::packaged_task<int(int)>( [](int i)->int{
            int servesock = CreateSeverSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            int client = 0;
            while(!client) client = accept(servesock, NULL, NULL);
            char buffer[BUFSIZ];
            memset(buffer, 0 ,sizeof(buffer));
            while (true)
            {
                if (read(client, buffer, sizeof(buffer) ) > 0)
                {
                    if(quitjudge(buffer)){
                        //printf("client will be disconnected\nsend the last message:");
                        //scanf("%s", buffer);
                        strcpy(buffer, "goodbye\n\0");
                        write(client, buffer, sizeof(char)*strlen(buffer) );
                        break;
                    }
                    else if(sendfile(buffer)){
                        FILE* fp;
                        fp = fopen("a.txt", "rb");
                        if(fp == NULL) printf("no such a file\n");
                        else{
                            printf("sendfile..................\n");
                            int read_count = fread((void*)buffer, sizeof(char), sizeof(buffer), fp);
                            if(BUFSIZ > read_count){
                                write(client, buffer, read_count);
                            }
                            else write(client, buffer, BUFSIZ);
                            printf("send file done\n");
                            fclose(fp);
                        }
                    }
                    else printf("recive message from %d:%s \n",client, buffer);
                }
                memset(buffer, 0, sizeof(char)*strlen(buffer));
    
                //printf("send message or quit(enter quit or q to quit) : ");
                //scanf("%s", buffer);
                strcpy(buffer, "vastina\n\0");
                if (-1 == write(client, buffer, sizeof(char)*strlen(buffer)))
                {
                    errorhandling("fail to send with code:{} \n", errno );
                }
                //if(quitjudge(buffer)) break;
                memset(buffer, 0, sizeof(char)*strlen(buffer));
            }
            close(servesock);
            return i*i;
        }) );
    }

    for(int count=0;auto& task: tasks)
    {
        results.emplace_back(task.get_future() );
        workers.emplace_back(
            [&count, &task] 
            { 
                task(++count); 
            } 
        );
    }
    
    for(auto& worker: workers) worker.join();
    for(int count=0;auto& result:results)
        print("({}, {})\n", ++count,result.get());

    return 0;
}