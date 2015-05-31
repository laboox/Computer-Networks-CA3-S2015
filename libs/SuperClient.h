/**
* File "SuperClient.h"
* Created by Sina on Sun May 31 10:50:26 2015.
*/

class SuperClass{
public:
    SuperClient();
    void parseCmd(string line);
    void connectRouter(int port);
private:
    address IP;
    address server_addr;
};
