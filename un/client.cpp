#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <set>
#include <algorithm>
#include <random>

using namespace std;

#define PORT_NUMBER_TCP "45981"
#define LOCAL_IP "127.0.0.1"

// from Beej
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET6)
    {
        // IPv6
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }
    // IPv4
    return &(((struct sockaddr_in *)sa)->sin_addr);
}

string originalEncrypt(const string& input) {
    string result = input;
    int length = input.length();
    
    for (int i = 0; i < length; ++i) {
        char& ch = result[i];

        if (std::isalpha(ch)) { // alpha
            char base = std::isupper(ch) ? 'A' : 'a'; 
            ch = ((ch - base + 3) % 26) + base; 
        } else if (std::isdigit(ch)) { // number
            ch = ((ch - '0' + 3) % 10) + '0'; 
        }
    }

    return result;
}

string confusionCipherEncrypt(const string& plaintext) {
    string ciphertext = plaintext;

    string allCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    unsigned int seed = 123; 
    std::random_device rd;
    std::mt19937 g(seed);
    std::shuffle(allCharacters.begin(), allCharacters.end(), g);


    std::unordered_map<char, char> charMap;
    for (size_t i = 0; i < allCharacters.length(); ++i) {
        charMap[allCharacters[i]] = allCharacters[(i + 1) % allCharacters.length()];
    }


    for (char& c : ciphertext) {
        if (charMap.find(c) != charMap.end()) {
            c = charMap[c];
        }
    }

    return ciphertext;
}

char offsetChar(char c) {
    if (c >= 'a' && c <= 'z') {
        return 'a' + (c - 'a' + 5) % 26;
    } else if (c >= 'A' && c <= 'Z') {
        return 'A' + (c - 'A' + 5) % 26;
    } else if (c >= '0' && c <= '9') {
        return '0' + (c - '0' + 5) % 10;
    } else {
        return c;
    }
}
string encrypt(const string& input) {
    string res;
    for (char c : input) {
        res += offsetChar(c);
    }
    return res;
}

int main(){
    // cout<<argv[1] <<endl;
    int encrypted_index = 0;
    // if (strcmp(argv[1],"-en") == 0){
    //     encrypted_index = 1;
    // }else if(strcmp(argv[1],"-un") == 0){
    //     encrypted_index = 0;
    // }else{
    //     cout<<"Please enter right instruction"<<endl;
    //     exit(1);
    // }
    string username;
    string passcode;
    string user_info;
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    char s[INET6_ADDRSTRLEN];
    char rev[2];

    string roomcode;
    string action;

    int index;

    // char sbuffer[1000];
    // char rbuffer[5000];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // from Beej get the socket address
    // checking error code
    if ((rv = getaddrinfo(LOCAL_IP, PORT_NUMBER_TCP, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // from Beej make TCP socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;}
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;}
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    // PORT
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr*)&addr, &addr_len) == -1) {
        std::cerr << "Error getting socket name" << std::endl;
        return 1;
    }
    unsigned short port = ntohs(addr.sin_port);

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    
    freeaddrinfo(servinfo);
    // printf("client: connecting to %s\n", s);
    // here we go
    cout<<"The client is up and running"<<endl;

    // Phase B
    while (1){
        printf("Please enter the username: ");
        getline(cin, username);
        printf("Please enter the password: ");
        getline(cin, passcode);
        string en_username;
        string en_passcode;
        if (encrypted_index == 1){
            en_username = confusionCipherEncrypt(username);
            en_passcode = confusionCipherEncrypt(passcode);
        }else{
            en_username = originalEncrypt(username);
            en_passcode = originalEncrypt(passcode);
        }
        user_info = en_username + "," + en_passcode;
        // cout<<user_info<<endl;
        // cout<<encrypted_index<<endl;
        // sprintf(sendbuf, "%s %s", username, passcode);
        if ((numbytes = send(sockfd, user_info.c_str(), user_info.length(), 0)) == -1){
            perror("send function error");
            exit(1);
        }
        cout<<username<<" sent an authentication request to the main server."<<endl;
        if(recv(sockfd, &rev, sizeof(rev), 0) == -1){
            perror("recv");
        }
        rev[1] = '\0';
        if (passcode.length() > 0){
            if (rev[0] == 'A'){
                cout<<"Welcome member "<<username<<"!"<<endl;
                break;
            }
            else if (rev[0] == 'B'){
                cout<<"Failed login: Username does not exist."<<endl;
            }
            else if(rev[0] == 'C'){
                cout<<"Failed login: Password does not match."<<endl;
            }
        }
        else{
            if (rev[0] == 'A'){
                cout<<"Welcome guest "<<username<<"!"<<endl;
                break;
            }
        }
    }

    string query;
    // cout<<username<<endl;
    // cout<<passcode<<endl;
    if (passcode.length() > 0){
        while(true){
            cout<<"Please enter the room code: ";
            getline(cin, roomcode);
            cout<<"Would you like to search for the availability or make a reservation? (Enter “Availability” to search for the availability or Enter “Reservation” to make a reservation  ): ";
            getline(cin, action);
            query = roomcode + "," + action;

            // send query
            if (action == "Availability"){//need to tag A and R
                cout<<query.c_str()<<endl;
                if ((numbytes = send(sockfd, query.c_str(), query.length(), 0)) == -1){
                    perror("send function error");
                    exit(1);
                }
                cout<<username<< " sent an availability request to the main server." <<endl;
            }else if (action == "Reservation") {
                if ((numbytes = send(sockfd, query.c_str(), query.length(), 0)) == -1){
                    perror(" send function error");
                    exit(1);
                }
                cout<<username<< " sent a reservation request to the main server." <<endl;
            }else{
                cout<<"BRUH FOLLOW THE RULES!" <<endl;
            }

            // receive message
            if (roomcode[0] =='D' || roomcode[0] =='S' || roomcode[0] =='U'){//第一种处理方法不发送无效roomcode
                if (action == "Availability"){
                    if((numbytes=recv(sockfd,&index,sizeof(index),0))==-1){
                        perror("recv book amount");
                        exit(1);
                    }
                    // index = ntohl(index);
                    if (index == 0){
                        cout<< "The client received the response from the main server using TCP over port "<< port <<" . The requested room is available. "<< endl;
                        cout<<" "<<endl;
                        cout<< "-----Start a new request-----"<<endl;
                    }else if (index == 1){
                        cout<< "The client received the response from the main server using TCP over port "<< port <<" . The requested room is not available. "<< endl;
                        cout<<" "<<endl;
                        cout<< "-----Start a new request-----"<<endl;
                    }else if (index == 2){
                        cout<< "The client received the response from the main server using TCP over port "<< port <<" . Not able to find the room layout. "<< endl;
                        cout<<" "<<endl;
                        cout<< "-----Start a new request-----"<<endl;
                    }else{
                        printf("receive A error");
                    }
                }else if (action == "Reservation") {
                    if((numbytes=recv(sockfd,&index,sizeof(index),0))==-1){
                        perror("recv book amount");
                        exit(1);
                    }
                    // index = ntohl(index);
                    if (index == 0){
                        cout<< "The client received the response from the main server using TCP over port "<< port <<" . Congratulation! The reservation for Room "<< roomcode <<" has been made."<< endl;
                        cout<<" "<<endl;
                        cout<< "-----Start a new request-----"<<endl;
                    }else if (index == 1){
                        cout<< "The client received the response from the main server using TCP over port "<< port <<" . Sorry! The requested room is not available. "<< endl;
                        cout<<" "<<endl;
                        cout<< "-----Start a new request-----"<<endl;
                    }else if (index == 2){
                        cout<< "The client received the response from the main server using TCP over port "<< port <<" . Oops! Not able to find the room. "<< endl;
                        cout<<" "<<endl;
                        cout<< "-----Start a new request-----"<<endl;
                    }else{
                        cout<< "Enter the right roomcode, bro"<<endl;
                    }
                }
            }else{
                cout<<"Enter the right roomcode, amigo!"<<endl;
            }
        }
    }
    else{
        while(true){
            cout<<"Please enter the room code: ";
            getline(cin, roomcode);
            cout<<"Would you like to search for the availability or make a reservation? (Enter “Availability” to search for the availability or Enter “Reservation” to make a reservation  ): ";
            getline(cin, action);
            // send query

            if (roomcode[0] =='D' || roomcode[0] =='S' || roomcode[0] =='U'){
                if (action == "Availability"){
                    query = roomcode+","+action;
                    if ((numbytes = send(sockfd, query.c_str(), query.length(), 0)) == -1){
                        perror("send function error");
                        exit(1);
                    }
                    cout<<username<< " sent an availability request to the main server." <<endl;
                }else if (action == "Reservation") {
                    query = roomcode+","+"R"+"Reservation";
                    if ((numbytes = send(sockfd, query.c_str(), query.length(), 0)) == -1){
                        perror("send function error");
                        exit(1);
                    }
                    cout<<username<< " sent a reservation request to the main server." <<endl; // 没有实际送
                }else{
                    cout<<"BRUH FOLLOW THE RULES!" <<endl;
                }

                // receive message
                if (action == "Availability"){
                    if((numbytes=recv(sockfd,&index,sizeof(index),0))==-1){
                        perror("recv book amount");
                        exit(1);
                    }
                    if (index == 0){
                        cout<< "The client received the response from the main server using TCP over port "<< port <<" . The requested room is available. "<< endl;
                        cout<<" "<<endl;
                        cout<< "-----Start a new request-----"<<endl;
                    }else if (index == 1){
                        cout<< "The client received the response from the main server using TCP over port "<< port <<" . The requested room is not available. "<< endl;
                        cout<<" "<<endl;
                        cout<< "-----Start a new request-----"<<endl;
                    }else if (index == 2){
                        cout<< "The client received the response from the main server using TCP over port "<< port <<" . Not able to find the room layout. "<< endl;
                        cout<<" "<<endl;
                        cout<< "-----Start a new request-----"<<endl;
                    }else{
                        printf("receive A error");
                    }
                }else if (action == "Reservation"){
                    if((numbytes=recv(sockfd,&index,sizeof(index),0))==-1){
                        perror("recv book amount");
                        exit(1);
                    }
                    cout<< "Permission denied: Guest cannot make a reservation. "<<endl;
                }
            }else{
                cout<<"Enter the right roomcode, amigo!"<<endl;
            }
        }
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
    }
    // shutdown(sockfd, SHUT_RDWR);
    // close(sockfd);
    return 0;
}