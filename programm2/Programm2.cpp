#include<iostream>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<string>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/tcp.h>
#include<functional>



class Connection
{
    public:
    int acc_socket;             // accepted socket
    sockaddr_un remoute;         // accepted address



    Connection(const char* socket_path)
    {
        if((sock=socket(AF_UNIX, SOCK_STREAM, 0))==-1)
        {
            std::cout<<"\nError: socket\n";
            exit(1);
        }
        locale.sun_family=AF_UNIX;
        strcpy(locale.sun_path, socket_path);

        len = strlen(locale.sun_path) + sizeof(locale.sun_family);

        unlink(locale.sun_path);

        if(bind(sock, (sockaddr*)&locale, len)==-1)
        {
            std::cout<<"\nError: bind\n";
            exit(1);
        }
        if(listen(sock, 1)==-1)
        {
            std::cout<<"\nError: listen\n";
            exit(1);
        }
    }




    // Accept connection and accomplish function with received data //

    void accept_connection(std::function<void(char[])>func)
    {
        while (true)
        {
            int unsigned sock_len=0;

            if((acc_socket=accept(sock, (sockaddr*)&remoute, &sock_len))==-1)
            {
                std::cout<<"\nError: accept\n";
                exit(1);
            }

            std::cout<<"connected\n";

            int data_recv;
            char recv_buf[128];


            if(!fork())
            {
                do
                {
                    memset(recv_buf, 0, 128*sizeof(char));

                    if((data_recv=recv(acc_socket, recv_buf, 128, 0))>0)
                    {

                        func(recv_buf);

                    }
                }
            while (data_recv>0);
            }


        }
        
    }

    private:
    int sock;                   // person socket
    
    char recv_buf[128];

    sockaddr_un locale;         // used address     
    int len;                    // size of sockaddr_un locale



};





// Output data if number is longer than 2 simbol and multiple 32
void print(char str[128])
{
    int recv_int = std::atoi(str);

    if((strlen(str)>2)&&(recv_int%32==0))
    {
        std::cout<<"\n"<<str<<"\n";
    }
}






int main()
{
    
    const char* socket_path="GeneralSocket";

    Connection con(socket_path);

    con.accept_connection(print);

    return 0;
    
}