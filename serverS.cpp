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
using namespace std;

#define PORT_NUMBER_UDP "44981"
#define PORT_NUMBER_D "41981"
#define LOCAL_IP "127.0.0.1"

//read in double info
unordered_map<string, int> readRoomInfo(string filename){
    ifstream file(filename);
    unordered_map<string, int> userInfo;
    string line;
    if (file.is_open()){
        while(getline(file, line)){
            stringstream ss(line);
            string roomcode, left_number;
            getline(ss, roomcode, ',');
            getline(ss, left_number);
            userInfo[roomcode] = stoi(left_number);
        }
        file.close();
    }else{
        cout<<"Unable to open "<< filename<<endl;
    }
    return userInfo;
}
int main(){
    unordered_map<string, int> Single_rooms;
    Single_rooms = readRoomInfo("single.txt");
    struct addrinfo hints, *servinfo, *p;
    int sockid, new_sockid;
    int rv;
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//ipv4
    hints.ai_socktype=SOCK_DGRAM;//udp
    // hints.ai_family = AF_UNSPEC;
    // hints.ai_socktype = SOCK_DGRAM;
    // hints.ai_flags = AI_PASSIVE;
    // from Beej
    //get socket address
    if((rv=getaddrinfo(LOCAL_IP, PORT_NUMBER_UDP, &hints, &servinfo))!=0){
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
    cout<<"Server S is up and running using UDP on port "<<PORT_NUMBER_D<<" ."<<endl;
    // send info to Main
    int state_index;
    for (const auto& pair : Single_rooms) {
        int state_index;
        string update_text;
        update_text += pair.first + ',' + to_string(pair.second);
        if((state_index=sendto(sockid,update_text.c_str(),update_text.length(),0,p->ai_addr, p->ai_addrlen))==-1){
            perror("reponse error");
            exit(1);
        }
    }
    string end_update_text;
    end_update_text += "Uexit";
    if((state_index=sendto(sockid,end_update_text.c_str(),end_update_text.length(),0,p->ai_addr, p->ai_addrlen))==-1){
                perror("reponse error");
                exit(1);
    }
    cout << "The Server S has sent the room status to the main server. "<<endl;

    struct addrinfo  *local_p;
    if((rv=getaddrinfo(LOCAL_IP, PORT_NUMBER_D, &hints, &servinfo))!=0){
        cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
        return 1;
    };
    //loop, make and bind socket
    for(local_p=servinfo; local_p!=nullptr; local_p=p->ai_next){
        if((new_sockid=socket(local_p->ai_family,local_p->ai_socktype,local_p->ai_protocol))==-1){
            perror("talker: socket");
            continue;
        }
        if (bind(new_sockid, local_p->ai_addr, local_p->ai_addrlen) == -1) {
            close(new_sockid);
            perror("listen: bind");
            continue;}
        break;
    }
    if(local_p==nullptr){
        cerr <<"talker: failed to create socket/n";
        return 2;
    }
    // handle query
    while(true){
        int recv_index;
        char query[200];
        int response,response_index;
        string roomcode,action;
        if((recv_index=recvfrom(new_sockid,query,sizeof(query),0,(struct sockaddr *)&their_addr, &addr_len))==-1){
            perror("rece from serverM");
            exit(1);
        }
        query[recv_index] = '\0';
        cout<<query<<endl;
        string user_query(query);
        stringstream ss(user_query);
        getline(ss,roomcode,',');
        getline(ss,action);
        if (action=="Availability"){
            cout<<"The Server S received an availability request from the main server."<<endl;
            if (Single_rooms.find(roomcode) != Single_rooms.end()){
                if (Single_rooms[roomcode] > 0){
                    response = 0;
                    cout<<"Room "<<roomcode<<" is available."<<endl;
                }else{
                    response = 1;
                    cout<<"Room "<<roomcode<<" is not available."<<endl;
                }
            }else{
                response = 2;
                cout<<"Not able to find the room layout."<<endl; 
            }
            if((response_index=sendto(sockid,&response,sizeof(response),0,p->ai_addr, p->ai_addrlen))==-1){
                perror("reponse error");
                exit(1);
            }
            cout<< "The Server S finished sending the response to the main server."<<endl;
        }else if(action=="Reservation"){
            cout<<"The Server S received an reservation request from the main server."<<endl;
            if (Single_rooms.find(roomcode) != Single_rooms.end()){
                if (Single_rooms[roomcode] > 0){
                    response = 0;
                    Single_rooms[roomcode] -= 1;
                    cout<<"Successful reservation. The count of Room "<<roomcode<<" is now "<<Single_rooms[roomcode]<<" ."<<endl;
                    cout << "The Server S finished sending the response and the updated room status to the main server. "<<endl; //还需要完善一下
                }else{
                    response = 1;
                    cout<<"Cannot make a reservation. Room "<<roomcode<<" is not available."<<endl;
                    cout<< "The Server S finished sending the response to the main server."<<endl;
                }
            }else{
                response = 2;
                cout<<"Cannot make a reservation. Not able to find the room layout."<<endl;
                cout<< "The Server S finished sending the response to the main server."<<endl; 
            }
            if((response_index=sendto(sockid,&response,sizeof(response),0,p->ai_addr, p->ai_addrlen))==-1){
                perror("reponse error");
                exit(1);
            }
            // cout<< "The Server S finished sending the response to the main server."<<endl;
        }else{
            cout<<"wtf"<<endl;
        }
    }
    freeaddrinfo(servinfo);
    shutdown(sockid, SHUT_RDWR);
    close(sockid);
    return 0;
}