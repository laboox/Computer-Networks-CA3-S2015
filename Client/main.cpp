/**
* File "main.cpp"
* Created by Sina on Sun May 31 13:38:32 2015.
*/

#include "Client.h"
#include "primary_header.h"
#include "address.h"

int main(int argc, char *argv[]){
    string cip;
    if(argc<5){
        cout<<"Usage is ./Client.out #name #server_ip #router_ip #router_port";
    }
    string name = argv[1];
    address serverIp = stringToAddr(argv[2]);
    address routerIp = stringToAddr(argv[3]);
    address IP;
    int routerPort = atoi(argv[4]);

    while(getline(cin, cip)){
        try{
            string cmd0, cmd1, ip;
            istringstream iss(cip);
            if(iss>>cmd0>>cmd1>>ip){
                if(cmd0=="Set" && cmd1=="IP"){
                    IP = stringToAddr(ip);
                    cout<<"ip "<<addrToString(IP)<<" set.\n";
                    break;
                }
            } else {
                cout <<"You Should Set IP Berfor anything!\n";
            }
        } catch (Exeption ex) {
            cout << ex.get_error() << endl;
        }
    }
    try{
        Client client(name, IP, serverIp, routerPort);
        client.run();
    } catch (Exeption ex){
        cout<<ex.get_error()<<endl;
    }
}
