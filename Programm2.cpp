#include<iostream>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<string>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/tcp.h>



int main()
{

    const char* socket_path="GeneralSocket";

    int sock, acc_socket;
    int len;

    sockaddr_un remout, locale;

    if((sock=socket(AF_UNIX, SOCK_STREAM, 0))==-1)
    {
        std::cout<<"\nError: socket\n";
        return 1;
    }

    locale.sun_family=AF_UNIX;
    strcpy(locale.sun_path, socket_path);

    len=strlen(locale.sun_path)+sizeof(locale.sun_family);
    
    unlink(locale.sun_path);

    if(bind(sock, (sockaddr*)&locale, len)==-1)
    {
        std::cout<<"\nError: bind\n";
        return 1;
    }

    if(listen(sock, 2)==-1)
    {
        std::cout<<"\nError: listen\n";
    }

    

    while (true)
    {
        int unsigned sock_len=0;

        if((acc_socket=accept(sock, (sockaddr*)&remout, &sock_len))==-1)
        {
            std::cout<<"\nError: accept\n";
            return 0;
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
                

                   int recv_int = std::atoi(recv_buf);

                   if((strlen(recv_buf)>2)&&(recv_int%32==0))
                    {
                    std::cout<<"\n"<<recv_buf<<"\n";
                    }
                

                }
            }
            while (data_recv>0);
        }



    }
    
}