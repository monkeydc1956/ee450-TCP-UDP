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
#include <unordered_map>
#include <sstream>
#include <cstring>

using namespace std;

#define PORT_NUMBER_UDP "44981"
#define PORT_NUMBER_TCP "45981"

#define PORT_NUMBER_S "41981"
#define PORT_NUMBER_D "42981"
#define PORT_NUMBER_U "43981"

#define LOCAL_IP "127.0.0.1"
#define BACKLOG 20


//read in user info
unordered_map<string, string> readUserInfo(string filename){
    ifstream file(filename);
    unordered_map<string, string> userInfo;
    string line;
    if (file.is_open()){
        while(getline(file, line)){
            stringstream ss(line);
            string username, passcode;
            getline(ss, username, ',');
            getline(ss, passcode);
            // userInfo[username] = passcode;
            userInfo[username] = passcode.erase(0, 1);
        }
        file.close();
    }else{
        cout<<"Unable to open "<<filename<<endl;
    }
    return userInfo;
}

// send request to SERVER
int sendQueryServer(string servername, string query){
    struct addrinfo hints, *servinfo, *p;
    int state_index;
    int sockid;
    int rv;
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//ipv4
    hints.ai_socktype=SOCK_DGRAM;//udp
    // from Beej
    if (servername[0] == 'D'){
        if((rv=getaddrinfo(LOCAL_IP, PORT_NUMBER_D, &hints, &servinfo))!=0){
            cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
            return 1;
        };
        // from Beej
        for(p=servinfo; p!=nullptr; p=p->ai_next){
            if((sockid=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
                perror("talker: socket");
                continue;
            }
            break;
        }
        // from Beej
        if(p==nullptr){
            cerr <<"talker: failed to create socket/n";
            return 2;
        }
    }else if(servername[0] == 'S'){
        // from Beej
        if((rv=getaddrinfo(LOCAL_IP, PORT_NUMBER_S, &hints, &servinfo))!=0){
            cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
            return 1;
        };
        // from Beej
        for(p=servinfo; p!=nullptr; p=p->ai_next){
            if((sockid=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
                perror("talker: socket");
                continue;
            }
            break;
        }
        if(p==nullptr){
            cerr <<"talker: failed to create socket/n";
            return 2;
        }
    }else if (servername[0] == 'U'){
        // from Beej
        if((rv=getaddrinfo(LOCAL_IP, PORT_NUMBER_U, &hints, &servinfo))!=0){
            cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
            return 1;
        };
        // from Beej
        for(p=servinfo; p!=nullptr; p=p->ai_next){
            if((sockid=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
                perror("talker: socket");
                continue;
            }
            break;
        }
        // from Beej
        if(p==nullptr){
            cerr <<"talker: failed to create socket/n";
            return 2;
        }
    }else{
        return -1; //后续操作需要进行
    }
    // from Beej
    if((state_index=sendto(sockid,query.c_str(),query.length(),0,p->ai_addr, p->ai_addrlen))==-1){
            perror("reponse error");
            exit(1);
    }
    // cout<<state_index<<endl;
    freeaddrinfo(servinfo);
    shutdown(sockid, SHUT_RDWR);
    close(sockid);
    return 0;
}

// edit user info

int main(){
    // read files bruh
    unordered_map<string, string> userInfo=readUserInfo("member.txt");
    // for (const auto& pair : userInfo) {
    //     std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    // }
    // cout<<"Main Server loaded the member list."<<endl;
    //create UDP socket
    struct addrinfo hints, *servinfo, *p;
    int udp_sockid;
    int rv;
    // unordered_map<char, addrinfo*> server_address;
    // struct addrinfo *sinfo,*dinfo,*uinfo;
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    // from Beej get the socket address
    // checking error code
    if ((rv = getaddrinfo(LOCAL_IP, PORT_NUMBER_UDP, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // from Beej make UDP socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((udp_sockid = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;}
        if (bind(udp_sockid, p->ai_addr, p->ai_addrlen) == -1) {
            close(udp_sockid);
            perror("listen: bind");
            continue;}
        break;
    }
    // from Beej
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    freeaddrinfo(servinfo);
    cout<<"Main Server is up and running."<<endl;

    unordered_map<string, int> Rooms;
    int index_update = 0;
    char response[200];
    while(true){
        int recv_len;
        // recv_len = recvfrom(udp_sockid,response,sizeof(response),0,(struct sockaddr *)&their_addr, &addr_len);
        if(((recv_len = recvfrom(udp_sockid,response,sizeof(response),0,(struct sockaddr *)&their_addr, &addr_len))==-1)){
            perror("talker: recv from server error");
            exit(1);
        }
        response[recv_len]='\0';
        // response[recvfrom(udp_sockid,response,sizeof(response),0,(struct sockaddr *)&their_addr, &addr_len)]='\0';
        string res(response);
        stringstream ss(res);
        string roomcode, roomnumber;
        getline(ss,roomcode,',');
        getline(ss,roomnumber);
        if (roomcode == "Sexit"){
            index_update += 1;
            cout<< "The main server has received the room status from Server S using UDP over port " << PORT_NUMBER_UDP << " ."<<endl;
        }else if(roomcode == "Uexit"){
            index_update += 1;
            cout<< "The main server has received the room status from Server U using UDP over port " << PORT_NUMBER_UDP << " ."<<endl;
        }else if(roomcode == "Dexit"){
            index_update +=  1;
            cout<< "The main server has received the room status from Server D using UDP over port " << PORT_NUMBER_UDP << " ."<<endl;
            // break;//测试使用
        }else{
            Rooms[roomcode] = stoi(roomnumber);
        }

        if (index_update == 3){
            break;
        }else if(index_update < 3){
            continue;
        }else{
            cout<<"WTF BRO"<<endl;
            exit(1);
        }
    }


    //create TCP socket 
    struct addrinfo thints, *tservinfo, *tp;
    struct sockaddr_storage client_addr;
    int tcp_sockid, tcp_socket;
    socklen_t client_addr_len;
    int yes = 1;
    // from Beej get the socket address
    memset(&thints,0,sizeof thints);
    thints.ai_family=AF_INET;
    thints.ai_socktype=SOCK_STREAM;
    // from Beej get the socket address
    if((rv=getaddrinfo(LOCAL_IP, PORT_NUMBER_TCP, &thints, &tservinfo))!=0){
    cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
    return 1;
    }
    // from Beej make TCP socket
    for(tp = tservinfo; tp != NULL; tp = tp->ai_next) {
        if ((tcp_sockid = socket(tp->ai_family, tp->ai_socktype, tp->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(tcp_sockid, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(tcp_sockid, tp->ai_addr, tp->ai_addrlen) == -1) {
            close(tcp_sockid);
            perror("server: bind");
            continue;
        }
        break;
     }
    // from Beej
    freeaddrinfo(tservinfo);
    // from Beej
    if(tp==nullptr){
        cerr <<"talker: failed to create TCP socket\n";
        return 2;
    }
    // from Beej
    if (listen(tcp_sockid, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    while (true){
        client_addr_len=sizeof(client_addr);
        //create childsocket
        tcp_socket=accept(tcp_sockid,(struct sockaddr *)&client_addr,&client_addr_len);
        if (tcp_socket ==-1){
            perror("accept");
            continue;
        }
        string username,passcode;
        // phaseB
        // from Beej
        if(!fork()){
            while(true){
                char response[100];
                int recv_len;
                if((recv_len=recv(tcp_socket,response,sizeof(response),0))==-1){
                    perror("tcprecv error");
                    exit(1);
                }
                response[recv_len]='\0';
                string user_info(response);
                stringstream ss(user_info);
                // string username,passcode;
                string auth;
                getline(ss,username,',');
                getline(ss,passcode);

                if (passcode.length()>0){
                    cout<<"The main server received the authentication for "<< username <<" using TCP over port "<<PORT_NUMBER_TCP<<" ."<<endl;
                    auto it = userInfo.find(username);
                    if (it != userInfo.end()) {
                        // cout<<passcode<<endl;
                        // cout<<passcode.length()<<endl;
                        // cout<<it->second<<endl;
                        // cout<<it->second.length()<<endl;
                        if (it->second.compare(passcode) == 0) {
                            auth = "A";
                        } else {
                            auth = "C";
                        }
                    } else {
                        auth = "B";
                    }
                }else{
                    auth = "A";
                    cout<<"The main server received the guest request for "<<username<<" using TCP over port "<<PORT_NUMBER_TCP<<" . The main server accepts "<<username<< " as a guest."<<endl;
                }
                cout<<"The main server sent the authentication result to the client."<<endl;
                if(send(tcp_socket,auth.c_str(),auth.length(),0)==-1){
                    perror("send auth res");
                }
                if(auth=="A"){
                    break;
                }
            }
            int rev_index;
            int res_user;
            // phase C
            while (true){
                char response[200];
                int roomnumtypes=recv(tcp_socket,response,sizeof(response),0);
                if(roomnumtypes==-1){
                    perror("recv query");
                    exit(1);
                }
                response[roomnumtypes] = '\0';
                string query(response);
                stringstream ss(query);
                string roomcode, action;
                getline(ss,roomcode,',');
                getline(ss,action);
                int query_index;
                if (action == "Availability"){
                    cout<<"The main server has received the availability request on Room "<<roomcode<<" from "<<username<<" using TCP over port "<<PORT_NUMBER_TCP<<"."<<endl;
                    query_index = sendQueryServer(roomcode,query);
                    if (query_index == -1){ // wrong formate roomcode
                        res_user = 4;
                        cout<<"Enter the right roomcode, bro!"<<endl;
                        if((roomnumtypes=send(tcp_socket,&res_user,sizeof(res_user),0))==-1){
                            perror("talker:send to client error");
                            exit(1);
                        }
                    }else{
                        cout<<"The main server sent a request to Server "<< roomcode[0] <<"."<<endl;
                        if((rev_index=recvfrom(udp_sockid,&res_user,sizeof(res_user),0,(struct sockaddr *)&their_addr, &addr_len)==-1)){
                            perror("talker: recv from server error");
                            exit(1);
                        }
                        // cout<<res_user<<endl;
                        cout<<"The main server received the response from Server "<<roomcode[0]<<" using UDP over port "<< PORT_NUMBER_UDP<<" ."<<endl;
                        if((roomnumtypes=send(tcp_socket,&res_user,sizeof(res_user),0))==-1){
                            perror("talker:send to client error");
                            exit(1);
                        }
                        cout<<"The main server sent the availability information to the client."<<endl;
                    }
                }else if (action == "Reservation"){
                    cout<< "The main server has received the reservation request on Room "<<roomcode<< " from "<<username<< " using TCP over port "<<PORT_NUMBER_TCP<<" ."<<endl;
                    if (passcode.length()>0){
                        query_index = sendQueryServer(roomcode,query);
                        if (query_index == -1){ // wrong formate roomcode
                            res_user = 4;
                            cout<<"Enter the right roomcode, bro!"<<endl;
                            if((roomnumtypes=send(tcp_socket,&res_user,sizeof(res_user),0))==-1){
                                perror("talker:send to client error");
                                exit(1);
                            }
                        }else{ 
                            cout<<"The main server sent a request to Server "<< roomcode[0] <<"."<<endl;
                            if((rev_index=recvfrom(udp_sockid,&res_user,sizeof(res_user),0,(struct sockaddr *)&their_addr, &addr_len)==-1)){
                                perror("talker: recv from server error");
                                exit(1);
                            }
                            // cout<<"The main server received the response from Server "<<roomcode[0]<<" using UDP over port "<< PORT_NUMBER_UDP<<" ."<<endl;
                            if((roomnumtypes=send(tcp_socket,&res_user,sizeof(res_user),0))==-1){
                                perror("talker:send to client error");
                                exit(1);
                            }
                            if (res_user == 0){
                                cout<<"The main server received the response and the updated room status from Server "<<roomcode[0]<<" using UDP over port "<< PORT_NUMBER_UDP<<" ."<<endl;
                                Rooms[roomcode] -=1;
                                cout<<"The room status of Room "<<roomcode<<" has been updated."<<endl;
                            }else{
                                cout<<"The main server received the response from Server "<<roomcode[0]<<" using UDP over port "<< PORT_NUMBER_UDP<<" ."<<endl;
                            }
                            // cout<<"The room status of Room "<<roomcode<<" has been updated."<<endl; //实际上还没update
                            cout<<"The main server sent the reservation result to the client."<<endl;
                        }
                    }
                }else if (action == "RReservation"){
                        res_user = 0;
                        cout<<username<<" cannot make a reservation."<<endl;
                        if((roomnumtypes=send(tcp_socket,&res_user,sizeof(res_user),0))==-1){
                            perror("talker:send to client error");
                            exit(1);
                        }
                        cout<<"The main server sent the error message to the client."<<endl;
                }
            }
        shutdown(tcp_socket, SHUT_RDWR);
        close(tcp_socket);
        }
    }
    close(udp_sockid);
    close(tcp_sockid);
    return 0;  
}