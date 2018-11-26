#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tictac.h"
#define  PORT "9987";

using namespace std;

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, port_no, bindfd, listenfd, bytes_sent, bytes_recvd;
	char sbuffer[512], cli_ip[16], sname[64], cname[64]; 
	char *ptr_buff, *ptr_port;
	const char *ptr_cli_ip;
	struct sockaddr_in server_addr, client_addr;	
	socklen_t server_size, client_size;	

	int inp_true = 0, count = 0, inp, ni, x, y, toss;
	char serv_choice, client_choice, nc;
	char choice_buffer[2], buffer_coordinates[2], toss_buffer;
	
	system("clear");
	ptr_buff = &sbuffer[0];
	ptr_port = (char *)&PORT;

	//creating sever side socket	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{	
		perror("Server side listening Socket could not be created!");
		return 1;
	}

	memset(&server_addr, 0, sizeof(server_addr));	
	port_no = atoi(ptr_port);
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_no);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	
	//binding socket
	bindfd = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (bindfd == -1)
	{	
		perror("Failed to bind");
		return 1;
	}
	
	//listening for incoming connections
	cout<<"Name : ";
	cin>>sname;
	cout<<"Server created"<<endl<<"Waiting for a Player..."<<endl; 

	listenfd = listen(sockfd, 5);
	if (listenfd == -1)
	{	
		perror("Failed to listen");
		return 1;
	}

	server_size = sizeof(server_addr);
	client_size = sizeof(client_addr);
	newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_size);
		
	if (newsockfd == -1)
	{	
		perror("Failed to accept from client");
		return 1;
	}
		
	ptr_cli_ip = inet_ntop(AF_INET, &client_addr.sin_addr, cli_ip, client_size);
	cout<<"Server received connections from "<<cli_ip<<endl;	

	memset(&cname, 0, sizeof(cname));
	do
	{	
		static int flag = 0;
		bytes_recvd = recv(newsockfd, &cname, sizeof(cname), 0);
		if (bytes_recvd == -1 && flag == 0)
		{
			memset(&cname, 0, sizeof(cname));
			cout<<"Could not ACQUIRE Player Information!"<<endl<<"Trying again..."<<endl; 
			continue;
		}
		else
		{	
			flag = 1;
			bytes_sent = send(newsockfd, &sname, sizeof(sname), 0);
			if (bytes_sent == -1)
				cout<<"Could not SEND Player Data!"<<"Trying Again..."<<endl; 
			else
				cout<<cname<<" has joined the game."<<endl;
		}
	}while(bytes_recvd == -1 || bytes_sent == -1);

	cout<<"Creating Game. Please wait..."<<endl;
	sleep(2);
	cout<<endl<<"Game created!"<<endl<<endl<<"Doing a toss...";
	srand(time(NULL));
	toss = rand() % 2;
	sleep(1);
	sprintf(&toss_buffer, "%d", toss);
	bytes_sent = send(newsockfd, &toss_buffer, sizeof(toss_buffer), 0);
	if (bytes_sent == -1)
	{
		perror("TOSS BUFFER not sent!");
		return 1;
	}

	if (toss == 0)
	{	
		cout<<endl<<"You WON the toss!"<<endl;
		do
    	{
    		cout<<sname<<" Enter Your Choice (X or O): ";
			cin>>serv_choice;
			if (serv_choice == 'X' || serv_choice == 'x')
			{	
	    		serv_choice = 'X';
	    		client_choice = 'O';
	    		inp_true = 1;
				cout<<endl<<cname<<" gets O."<<endl<<endl<<"Lets Play!"<<endl<<endl;
			}
			else if (serv_choice == 'O' || serv_choice == 'o' || serv_choice == '0')
			{
			    serv_choice = 'O';
        		client_choice = 'X';
	    		inp_true = 1;
	    		cout<<endl<<cname<<" gets X."<<endl<<endl<<"Lets Play!"<<endl<<endl;
			}	
			else
			{
	    		cout<<"\nInvalid Choice! Please Choose Again..."<<endl;
	    		inp_true == 0;
			}
    	}while(inp_true == 0);

		memset(&choice_buffer, 0, sizeof(choice_buffer));
		choice_buffer[0] = serv_choice;
		choice_buffer[1] = client_choice;

		bytes_sent = send(newsockfd, &choice_buffer, sizeof(choice_buffer), 0);
		if (bytes_sent == -1)
		{
			perror("CHOICE BUFFER could not be sent!");
			return 1;
		}
	}
	else
	{
		cout<<endl<<cname<<" WON the toss."<<endl;
		cout<<cname<<" is choosing. Please wait..."<<endl<<endl;
	
		memset(&choice_buffer, 0, sizeof(choice_buffer));
		bytes_recvd = recv(newsockfd, &choice_buffer, sizeof(choice_buffer), 0);
		if (bytes_recvd == -1)
		{
			perror("CHOICE BUFFER not received!");
			return 1;
		}
		else
		{
			serv_choice = choice_buffer[0];
			client_choice = choice_buffer[1];
			cout<<sname<<" has chosen "<<serv_choice<<endl<<endl<<"You will play with "<<client_choice<<endl;
			cout<<endl<<"Lets Play!"<<endl<<endl;
		}
	}

	if (serv_choice == 'X')
	{
		inp = 1;
		cout<<"You  will play first."<<endl<<endl;
	}
	else
	{
		inp = 2;
		cout<<cname<<" will play first."<<endl<<endl;
	}
	
	init();
	cout<<endl<<"Starting Game..."<<endl;
	sleep(3);
	display();

	while (count < 9)
	{
		memset(&buffer_coordinates, 0, sizeof(buffer_coordinates));

		if (inp % 2 != 0 )
		{
			cout<<endl<<"Your turn. Enter co-ordinates separated by a space : ";
			cin>>x>>y;
			ni = input(serv_choice, x, y);
			if (ni == 0)
			{	
				inp ++;
				sprintf(&buffer_coordinates[0], "%d", x);
				sprintf(&buffer_coordinates[1], "%d", y);
				cout<<endl<<"Updating Matrix..."<<endl;
				
				bytes_sent = send(newsockfd, &buffer_coordinates, sizeof(buffer_coordinates), 0);
				if (bytes_sent == -1)
				{
					perror("CO-ORDINATES BUFFER not sent!");
					return 1;
				}
			}
		}
		else 
		{
			cout<<endl<<cname<<"'s turn. Please wait..."<<endl;
			bytes_recvd = recv(newsockfd, &buffer_coordinates, sizeof(buffer_coordinates), 0 );
			if (bytes_recvd == -1)
			{
				perror("CO-ORDINATES BUFFER not recieved!");
				return 1;
			}
			x = buffer_coordinates[0] - '0';
			y = buffer_coordinates[1] - '0';
			ni = input(client_choice, x, y);
			if (ni == 0)
			{
				inp ++;
				cout<<endl<<cname<<" has played. Updating Matrix..."<<endl;
			}
		}

		count ++;
		sleep(2);
		system("clear");
		display();

		if (count >=5)
		{	
			nc = check();
			if (nc == 'f')
				continue;
			else if (serv_choice == nc)
			{	
				cout<<endl<<"Congrats! You have won!!!"<<endl<<cname<<" lost."<<endl;
				break;
			}
			else if (client_choice == nc)
			{
				cout<<endl<<"You loose."<<endl<<cname<<" has won."<<endl;						
				break;
			}
		}
	}
	
	if (nc == 'f')
		cout<<endl<<"Game ends in a draw."<<endl;
	
	cout<<endl<<"Thank You for playing Tic-tac-Toe"<<endl;
	close(newsockfd);
	close(sockfd);
	return 0;
}

