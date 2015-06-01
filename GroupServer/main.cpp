/**
* File "main.cpp"
* Created by Sina on Sun May 31 13:38:32 2015.
*/

#include "GroupServer.h"
#include "primary_header.h"
#include "address.h"

int main(int argc, char *argv[]){
    string cip;
    if(argc<3){
        cout<<"Usage is ./GroupServer.out #name #server_ip\n";
    }
    address serverIP = stringToAddr(argv[2]);
    address IP;
    address multiIP;
    string name = argv[1];
    int routerPort;
    bool ipset=false, mulipset=false;

    while(getline(cin, cip)){
        try{
            string cmd0, cmd1;
            istringstream iss(cip);
            if(iss>>cmd0>>cmd1){
                if(cmd0=="Set" && cmd1=="IP"){
                    string ip;
                    iss>>ip;
                    if(ip == "multicast"){
                        iss>>ip;
                        multiIP = stringToAddr(ip);
                        mulipset = true;
                    } else {
                        IP = stringToAddr(ip);
                        ipset = true;
                    }
                } 
                else if(cmd0=="Connect" && cmd1=="Router"){
                    if(ipset && mulipset){
                        iss>>routerPort;
                        break;
                    } else {
                        cout<<"set ip and muticast ip before connecting\n";
                    }
                }
            } else {
                cout <<"You should connect router before anything!\n";
            }
        } catch (Exeption ex) {
            cout << ex.get_error() << endl;
        }
    }
    try{
        GroupServer server(name, multiIP, IP, serverIP, routerPort);
        server.run();
    } catch (Exeption ex){
        cout<<ex.get_error()<<endl;
    }
}
