#include "Router.h"

int main(int argc, char *argv[])
{
	try
    {
    	if(argc<2)
	    {
	        cout << "Usage: ./Router #listen_port" << endl;
	        exit(0);
	    }

	    Router router(atoi(argv[1]));
	   	router.run();
    }
    catch(Exeption ex)
	{
            cout << ex.get_error() << endl;
    }        
}
