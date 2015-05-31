/**
* File "main.cpp"
* Created by Sina on Sun May 31 13:38:32 2015.
*/

#include "Server.h"
#include "primary_header.h"
#include "address.h"

int main(int argc, char *argv[]){
    string cip;
    if(argc<2){
        cout<<"Usage is ./Server.out #server_ip";
    }
    address IP = stringToAddr(argv[1]);
    int routerPort;

    while(getline(cin, cip)){
        try{
            string cmd0, cmd1;
            istringstream iss(cip);
            if(iss>>cmd0>>cmd1>>routerPort){
                if(cmd0=="Connect" && cmd1=="Router"){
                    break;
                }
            } else {
                cout <<"You should connect router before anything!\n";
            }
        } catch (Exeption ex) {
            cout << ex.get_error() << endl;
        }
    }
    try{
        Server server(IP, IP, routerPort);
        server.run();
    } catch (Exeption ex){
        cout<<ex.get_error()<<endl;
    }
}
