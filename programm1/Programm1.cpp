#include<thread>
#include<iostream>
#include<string>
#include<vector>
#include<mutex>
#include<condition_variable>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>

class TransBetThread
{
public:
    std::mutex mutex;
    std::condition_variable queueCheck;
    bool notif;
    TransBetThread()
    {
        Buffer.clear();
        this->notif = false;
    }
    
    void setBuf(std::string str)
    {   
        Buffer=str;
        this->notif=true;
        queueCheck.notify_all();
    }

    void getBuff(std::string *str)
    {
        *str=Buffer;
        Buffer.clear();
        this->notif = false;
        
    }

    
private:
    std::string Buffer;
};



class Edition_str
{
public:
    TransBetThread* message;
    Edition_str(TransBetThread* tbt)
    {
        message=tbt;
    }
    
    void fill_str()
    {
        std::cout<<"\nth1="<<std::this_thread::get_id()<<"\n";
        while(true)
        {
            bool check=false;
            std::string str;
            std::cin>>str;
            if(str.size()>64)
            {
                std::cout<<"very big input again";
                continue;
            }
            for(int i=0; i<str.size(); i++)
            {
                if(check=(str[i]>='A' && str[i]<='Z')|| (str[i] >='a' && str[i]<='z'))
                {
                    std::cout<<"\nthere is a letter in the string\n";
                    break;
                }
            }
            
            if(!check)
            {
                for(int i = 0; i<str.size()-1; i++)
                {
                    for(int j=0;j<str.size()-1; j++)
                    {
                        if(int(str[j])<int(str[j+1]))
                        {
                            char temporary_simb=str[j];
                            str[j]=str[j+1];
                            str[j+1]=temporary_simb;
                        }
                    }
                }
                
                std::string new_str;
                for(int i=0; i<str.size(); i++)
                {
                    if((int(str[i])%2)==0)
                    {
                        new_str.append("KB");
                        continue;
                    }
                    new_str.push_back(str[i]);
                }
                message->setBuf(new_str);
                std::cout<<"\n"<<new_str<<"\n";
            }

        }
    } 
    
};



class Sending_str
{
public:
    const char* socket_path="GeneralSocket";
    int sock;
    int data_len;
    sockaddr_un remote;
    TransBetThread* message;



    Sending_str(TransBetThread* tbt)
    {
        message=tbt;
        if((sock=socket(AF_UNIX, SOCK_STREAM, 0))==-1)
        {
            std::cout<<"\nError: socket";
        }
        remote.sun_family=AF_UNIX;
        strcpy(remote.sun_path, socket_path);
        data_len=strlen(remote.sun_path)+sizeof(remote.sun_family);
        
        
        if( connect(sock, (sockaddr*)&remote, data_len) == -1 )
        {
            std::cout<<"\nError: connect";
        }
    }


    void send_str()
    {
        std::cout<<"\nth1="<<std::this_thread::get_id()<<"\n";
        while (!message->notif)
        {
            std::unique_lock<std::mutex> ul(message->mutex);
            message->queueCheck.wait(ul);

            std::string str;
            message->getBuff(&str);
            int sum=0;
            std::cout<<"\nedited data: "<<str<<"\n";
    

            for(std::string::iterator it=str.begin(); it!=str.end();it++)
            {
        
                if(*it >= '0' && *it <= '9' )
                {
                    //ASCII
                    sum+=(*it)-48;
                }

            }

            despatch_message(sum);
        }
    }

    void despatch_message(int letter)
    {
        char send_msg[10];
        sprintf(send_msg, "%d", letter);
        for(int i=0;i>strlen(send_msg); i++)
        send_msg[i]-='0';
        if(send(sock, send_msg, strlen(send_msg)*sizeof(char), 0)==-1)
        {
            std::cout<<"\nError: send";
        }
        std::cout<<"\nsend\n";

    }


};


/*
void repl_str(TransBetThread *fill)
{
    
    
    while(true)
    {
        bool check=false;
        std::string str;
        std::cin>>str;
        if(str.size()>64)
        {
            std::cout<<"very big input again";
            continue;
        }
        for(int i=0; i<str.size(); i++)
        {
            if(check=(str[i]>='A' && str[i]<='Z')|| (str[i] >='a' && str[i]<='z'))
            {
                std::cout<<"\nthere is a Letter in the string\n";
                break;
            }
        }
        std::cout<<"\n"<<str<<"\n";
        if(!check)
        {
            for(int i = 0; i<str.size()-1; i++)
            {
                for(int j=0;j<str.size()-1; j++)
                {
                    if(int(str[j])<int(str[j+1]))
                    {
                        char temporary_simb=str[j];
                        str[j]=str[j+1];
                        str[j+1]=temporary_simb;
                    }
                }
            }
            std::cout<<"\n"<<str<<"\n";

            std::string *new_str;
            for(int i=0; i<str.size(); i++)
            {
                if((int(str[i])%2)==0)
                {
                    new_str->append("KB");
                    continue;
                }
                new_str->push_back(str[i]);
            }
            fill->setBuf(*new_str);
        }

        
        
    }
}
*/


/*
void out_sum(TransBetThread *fill)
{   
    std::cout<<"\nth1="<<std::this_thread::get_id()<<"\n";
    while (!fill->notif)
    {
        std::unique_lock<std::mutex> ul(fill->mutex);
        fill->queueCheck.wait(ul);

        std::string str;
        fill->getBuff(&str);
        int sum=0;
        std::cout<<"\nrecieved data: "<<str<<"\n";
    

        for(std::string::iterator it=str.begin(); it!=str.end();it++)
        {
        
            if(*it >= '0' && *it <= '9' )
            {
                //ASCII
                sum+=(*it)-48;
            }
        
        }
        
        
    }
}
*/



int main()
{    
    TransBetThread *t=new TransBetThread;

    Edition_str ed(t);

    Sending_str se(t);

    std::thread th1([&ed](){ed.fill_str();});

    std::thread th2([&se](){se.send_str();});


    th1.join();
    th2.join();
    return 0;
    
}