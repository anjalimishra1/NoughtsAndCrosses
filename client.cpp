#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include "tictac.h"
#define  PORT "9987"

using namespace std;

int main(int argc, char *argv[])
{
	int socketfd, newsocketfd, port_no, n, connfd, sent_bytes, rcvd_bytes;
	char cbuffer[512], sname[64], cname[64];
	char *ptr = &cbuffer[0];
	char *ptr_port = (char *)&PORT;
	struct sockaddr_in serv_addr;
	struct hostent *he;
	
	int count = 0, inp, x, y, ni, inp_true = 0, toss;
	char serv_choice, cli_choice, nc;
	char choice_buffer[2], buffer_coordinates[2], toss_buffer;

	system("clear");

	if (argc != 2)
	{
		perror("Incomplete arguments!");
		return 1;
	}
	port_no = atoi(ptr_port);
	he = gethostbyname(argv[1]);
	if (he == NULL)
	{
		perror("No Such Host!");
		return 1;
	}

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd == -1)
	{	
		perror("Could not create socket");
		return 1;
	}

    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr = *((struct in_addr *)he->h_addr);  

	connfd = connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (connfd == -1)
	{	
		perror("No connection to server");
		return 1;
	}
		
	cout<<"Name : ";
	cin>>cname;
	do
	{
		static int flag = 0;
		sent_bytes = send(socketfd, &cname, sizeof(cname), 0);
		if (sent_bytes == -1 && flag == 0)
		{
			cout<<"No data sent"<<endl<<"Trying...";  
			continue;
		}
		else
		{		cli_choice = 'X';
	    	
			flag = 1;
			memset(&sname, 0, sizeof(sname));
			rcvd_bytes = recv(socketfd, &sname, sizeof(sname), 0);
			if (rcvd_bytes == -1)
				cout<<"No player information acquired"endl<<"Trying Again..."<<endl; 
			else
				cout<<"You have joined "<<sname<<" for a game ."<<endl;
		}
	}while(sent_bytes == -1 || rcvd_bytes == -1);
	
	cout<<"Creating game. Please wait..."<<endl;
	sleep(2);
	cout<<endl<<"Game created!"<<endl<<endl<<"Doing a toss...";
	
	rcvd_bytes = recv(socketfd, &toss_buffer, sizeof(toss_buffer), 0);
	if (rcvd_bytes == -1)
	{
		perror("no Toss buffer");
		return 1;
	}

	toss = toss_buffer - '0';
	if (toss == 0)		
	{
		cout<<endl<<sname<<" WON the toss."<<endl;
		cout<<sname<<" is choosing. Please wait..."<<endl<<endl;
		memset(&choice_buffer, 0, sizeof(choice_buffer));
		rcvd_bytes = recv(socketfd, &choice_buffer, sizeof(choice_buffer), 0);
		if (rcvd_bytes == -1)
		{
			perror("CHOICE BUFFER not received!");
			return 1;
		}
		else
		{
			serv_choice = choice_buffer[0];
			cli_choice = choice_buffer[1];
			cout<<sname<<" has chosen "<<serv_choice<<endl<<endl<<"You will play with "<<cli_choice<<endl;
			cout<<endl<<"Lets Play!"<<endl<<endl;
		}
	}
	else
	{
		cout<<endl<<"You won the TOSS!"<<endl;
		do
    	{
    		cout<<cname<<" Enter Your Choice (X or O): ";
			cin>>cli_choice;
			if (cli_choice == 'X' || cli_choice == 'x')
			{	
	    		serv_choice = 'O';
	    		cli_choice = 'X';
	    		inp_true = 1;
				cout<<endl<<sname<<" gets O."<<endl<<endl<<"Lets Play!"<<endl<<endl;
			}
			else if (cli_choice == 'O' || cli_choice == 'o' || cli_choice == '0')
			{
			    serv_choice = 'X';
        		cli_choice = 'O';
	    		inp_true = 1;
	    		cout<<endl<<sname<<" gets X."<<endl<<endl<<"Lets Play!"<<endl<<endl;
			}	
			else
			{
	    		cout<<"\nInvalid Choice! Please Choose Again..."<<endl;
	    		inp_true == 0;
			}
    	}while(inp_true == 0);

		memset(&choice_buffer, 0, sizeof(choice_buffer));
		choice_buffer[0] = serv_choice;
		choice_buffer[1] = cli_choice;

		sent_bytes = send(socketfd, &choice_buffer, sizeof(choice_buffer), 0);
		if (sent_bytes == -1)
		{
			perror("CHOICE BUFFER could not be sent.");
			return 1;
		}

	}

	if (serv_choice == 'X')
	{
		inp = 1;
		cout<<sname<<" will play first."<<endl<<endl;
	
	}
	else
	{
		inp = 2;
		cout<<"You will play first."<<endl<<endl;
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
			cout<<endl<<sname<<"'s turn. Please wait..."<<endl;
			rcvd_bytes = recv(socketfd, &buffer_coordinates, sizeof(buffer_coordinates), 0);
			if (rcvd_bytes == -1)
			{
				perror("Could not receive coordinates buffer!");
				return 1;
			}
			x = buffer_coordinates[0] - '0';
			y = buffer_coordinates[1] - '0';
			ni = input(serv_choice, x, y);
			if (ni == 0)
			{	
				inp ++;
				cout<<endl<<sname<<" has played. Updating Matrix..."<<endl;
			}
		}
		else 
		{
			cout<<endl<<"Your turn. Enter co-ordinates separated by a space : ";
			cin>>x>>y;
			ni = input(cli_choice, x, y);
			if (ni == 0)
			{
				inp ++;
				sprintf(&buffer_coordinates[0], "%d", x);
				sprintf(&buffer_coordinates[1], "%d", y);
				cout<<endl<<"Updating Matrix..."<<endl;
				
				sent_bytes = send(socketfd, &buffer_coordinates, sizeof(buffer_coordinates), 0);
				if (sent_bytes == -1)
				{
					perror("Could not send coordinates buffer!");
					return 1;
				}
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
				cout<<endl<<"You loose."<<endl<<sname<<" has won."<<endl;	
				break;
			}
			else if (cli_choice == nc)
			{
				cout<<endl<<"Congrats! You have won!!!"<<endl<<sname<<" lost."<<endl;
				break;
			}
		}

	}
	
	if (nc == 'f')
		cout<<endl<<"Game ends in a draw."<<endl;

	cout<<endl<<"Thank You for playing Tic-tac-Toe"<<endl;
	close(socketfd);
	return 0;
}
	

