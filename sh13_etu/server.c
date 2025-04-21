#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>



struct _client
{
        char ipAddress[40];
        int port;
        char name[40];
} tcpClients[4];
int nbClients;
int fsmServer;
int deck[13]={0,1,2,3,4,5,6,7,8,9,10,11,12};
int tableCartes[4][8];
char *nomcartes[]=
{"Sebastian Moran", "irene Adler", "inspector Lestrade",
  "inspector Gregson", "inspector Baynes", "inspector Bradstreet",
  "inspector Hopkins", "Sherlock Holmes", "John Watson", "Mycroft Holmes",
  "Mrs. Hudson", "Mary Morstan", "James Moriarty"};
int joueurCourant;

int alive[4] = {1,1,1,1};      
int nbPlayersRemaining = 4;    


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void melangerDeck()
{
        int i;
        int index1,index2,tmp;

        for (i=0;i<1000;i++)
        {
                index1=rand()%13;
                index2=rand()%13;

                tmp=deck[index1];
                deck[index1]=deck[index2];
                deck[index2]=tmp;
        }
}

void createTable()
{
	// Le joueur 0 possede les cartes d'indice 0,1,2
	// Le joueur 1 possede les cartes d'indice 3,4,5 
	// Le joueur 2 possede les cartes d'indice 6,7,8 
	// Le joueur 3 possede les cartes d'indice 9,10,11 
	// Le coupable est la carte d'indice 12
	int i,j,c;

	for (i=0;i<4;i++)
		for (j=0;j<8;j++)
			tableCartes[i][j]=0;

	for (i=0;i<4;i++)
	{
		for (j=0;j<3;j++)
		{
			c=deck[i*3+j];
			switch (c)
			{
				case 0: // Sebastian Moran
					tableCartes[i][7]++;
					tableCartes[i][2]++;
					break;
				case 1: // Irene Adler
					tableCartes[i][7]++;
					tableCartes[i][1]++;
					tableCartes[i][5]++;
					break;
				case 2: // Inspector Lestrade
					tableCartes[i][3]++;
					tableCartes[i][6]++;
					tableCartes[i][4]++;
					break;
				case 3: // Inspector Gregson 
					tableCartes[i][3]++;
					tableCartes[i][2]++;
					tableCartes[i][4]++;
					break;
				case 4: // Inspector Baynes 
					tableCartes[i][3]++;
					tableCartes[i][1]++;
					break;
				case 5: // Inspector Bradstreet 
					tableCartes[i][3]++;
					tableCartes[i][2]++;
					break;
				case 6: // Inspector Hopkins 
					tableCartes[i][3]++;
					tableCartes[i][0]++;
					tableCartes[i][6]++;
					break;
				case 7: // Sherlock Holmes 
					tableCartes[i][0]++;
					tableCartes[i][1]++;
					tableCartes[i][2]++;
					break;
				case 8: // John Watson 
					tableCartes[i][0]++;
					tableCartes[i][6]++;
					tableCartes[i][2]++;
					break;
				case 9: // Mycroft Holmes 
					tableCartes[i][0]++;
					tableCartes[i][1]++;
					tableCartes[i][4]++;
					break;
				case 10: // Mrs. Hudson 
					tableCartes[i][0]++;
					tableCartes[i][5]++;
					break;
				case 11: // Mary Morstan 
					tableCartes[i][4]++;
					tableCartes[i][5]++;
					break;
				case 12: // James Moriarty 
					tableCartes[i][7]++;
					tableCartes[i][1]++;
					break;
			}
		}
	}
} 

void printDeck()
{
        int i,j;

        for (i=0;i<13;i++)
                printf("%d %s\n",deck[i],nomcartes[deck[i]]);

	for (i=0;i<4;i++)
	{
		for (j=0;j<8;j++)
			printf("%2.2d ",tableCartes[i][j]);
		puts("");
	}
}

void printClients()
{
        int i;

        for (i=0;i<nbClients;i++)
                printf("%d: %s %5.5d %s\n",i,tcpClients[i].ipAddress,
                        tcpClients[i].port,
                        tcpClients[i].name);
}

int findClientByName(char *name)
{
        int i;

        for (i=0;i<nbClients;i++)
                if (strcmp(tcpClients[i].name,name)==0)
                        return i;
        return -1;
}

void sendMessageToClient(char *clientip,int clientport,char *mess)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname(clientip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(clientport);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        {
                printf("ERROR connecting\n");
                exit(1);
        }

        sprintf(buffer,"%s\n",mess);
        n = write(sockfd,buffer,strlen(buffer));

    close(sockfd);
}

void broadcastMessage(char *mess)
{
        int i;

        for (i=0;i<nbClients;i++)
                sendMessageToClient(tcpClients[i].ipAddress,
                        tcpClients[i].port,
                        mess);
}

int main(int argc, char *argv[])
{
	srand( time( NULL ) );
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
	int i;

        char com;
        char clientIpAddress[256], clientName[256];
        int clientPort;
        int id;
        char reply[256];


     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

	printDeck();
	melangerDeck();
	createTable();
	printDeck();
	joueurCourant=0;

	for (i=0;i<4;i++)
	{
        	strcpy(tcpClients[i].ipAddress,"localhost");
        	tcpClients[i].port=-1;
        	strcpy(tcpClients[i].name,"-");
	}

     while (1)
     {    
     	newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     	if (newsockfd < 0) 
          	error("ERROR on accept");

     	bzero(buffer,256);
     	n = read(newsockfd,buffer,255);
     	if (n < 0) 
		error("ERROR reading from socket");

        printf("Received packet from %s:%d\nData: [%s]\n\n",
                inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);

        if (fsmServer==0)
        {
        	switch (buffer[0])
        	{
                	case 'C':
                        	sscanf(buffer,"%c %s %d %s", &com, clientIpAddress, &clientPort, clientName);
                        	printf("COM=%c ipAddress=%s port=%d name=%s\n",com, clientIpAddress, clientPort, clientName);

                        	
                                strcpy(tcpClients[nbClients].ipAddress,clientIpAddress);
                                tcpClients[nbClients].port=clientPort;
                                strcpy(tcpClients[nbClients].name,clientName);
                                nbClients++;

                                printClients();

				

                                id=findClientByName(clientName);
                                printf("id=%d\n",id);

				

                                sprintf(reply,"I %d",id);
                                sendMessageToClient(tcpClients[id].ipAddress,
                                       tcpClients[id].port,
                                       reply);


                                sprintf(reply,"L %s %s %s %s", tcpClients[0].name, tcpClients[1].name, tcpClients[2].name, tcpClients[3].name);
                                broadcastMessage(reply);

				// Si le nombre de joueurs atteint 4, alors on peut lancer le jeu

				if (nbClients == 4)
				{
					// Pour chaque joueur p = 0..3, on construit et envoie :
					//   D <carte0> <carte1> <carte2> <sym0> ... <sym7>
					for (int p = 0; p < 4; p++)
					{
						char msg[256];
						// Récupère les 3 cartes du joueur p dans le deck
						int c0 = deck[p*3];
						int c1 = deck[p*3 + 1];
						int c2 = deck[p*3 + 2];
				
						// Commence le message par D suivi des 3 cartes
						int pos = snprintf(msg, sizeof(msg), "D %d %d %d", c0, c1, c2);
				
						// Ajoute les 8 compteurs de tableCartes[p][s]
						for (int s = 0; s < 8; s++)
						{
							pos += snprintf(msg + pos, sizeof(msg) - pos, " %d", tableCartes[p][s]);
						}
				
						// Envoie au joueur p
						sendMessageToClient(
							tcpClients[p].ipAddress,
							tcpClients[p].port,
							msg
						);
					}
				
					// Enfin, on indique à tous quel joueur commence (ici joueurCourant = 0)
					snprintf(reply, sizeof(reply), "T %d", joueurCourant);
					broadcastMessage(reply);
				
					fsmServer = 1;
				}
				
				break;
                }	
	}
	else if (fsmServer==1)
	{
		switch (buffer[0])
		{
			case 'G':  // Guilt (proposition de coupable)
			{
				int yourId, suspectId;
				sscanf(buffer, "G %d %d", &yourId, &suspectId);
			
				if (suspectId == deck[12])
				{
					// victoire immédiate
					char winMsg[32];
					snprintf(winMsg, sizeof(winMsg), "WIN %d", yourId);
					broadcastMessage(winMsg);
					// ici tu peux terminer ou reset fsmServer…
				}
				else
				{
					// mauvais choix → élimination
					alive[yourId] = 0;
					nbPlayersRemaining--;
			
					// annonce de l'élimination
					char elimMsg[32];
					snprintf(elimMsg, sizeof(elimMsg), "ELIM %d", yourId);
					broadcastMessage(elimMsg);
			
					if (nbPlayersRemaining == 1)
					{
						// plus qu'un joueur → il gagne
						int winner = -1;
						for (int p = 0; p < 4; p++)
							if (alive[p]) { winner = p; break; }
			
						char finalWin[32];
						snprintf(finalWin, sizeof(finalWin), "WIN %d", winner);
						broadcastMessage(finalWin);
					}
					else
					{
						// on passe au joueur suivant vivant
						int next = (yourId + 1) % 4;
						while (!alive[next])
							next = (next + 1) % 4;
						joueurCourant = next;
			
						// on notifie tous
						char tourMsg[32];
						snprintf(tourMsg, sizeof(tourMsg), "T %d", joueurCourant);
						broadcastMessage(tourMsg);
					}
				}
			}
			break;
			
			
			case 'O':  // Others
			{
				int yourId, sym;
				sscanf(buffer, "O %d %d", &yourId, &sym);
	
				// Calcul de qui a le symbole
				char respO[256];
				int pos = snprintf(respO, sizeof(respO), "R O %d %d", yourId, sym);
				for (int p = 0; p < 4; p++)
				{
					int h = (alive[p] && tableCartes[p][sym] > 0) ? 1 : 0;
					pos += snprintf(respO + pos, sizeof(respO) - pos, " %d", h);
				}
				broadcastMessage(respO);
	
				// *** on passe au joueur suivant ***
				do {
					joueurCourant = (joueurCourant + 1) % 4;
				} while (!alive[joueurCourant]);
				// et on notifie tous
				char tourMsg[32];
				snprintf(tourMsg, sizeof(tourMsg), "T %d", joueurCourant);
				broadcastMessage(tourMsg);
			}
			break;
	
			case 'S':  // Solo (interroger un joueur précis)
			{
				int yourId, targetId, sym;
				// On s’attend à : "S <yourId> <targetId> <sym>"
				sscanf(buffer, "S %d %d %d", &yourId, &targetId, &sym);
			
				// Récupère le nombre exact du symbole chez targetId
				int count = tableCartes[targetId][sym];
			
				// Envoie la réponse SOLO en incluant <sym> et <count>
				// Format : R S <yourId> <targetId> <sym> <count>
				char respS[64];
				snprintf(respS, sizeof(respS),
						 "R S %d %d %d %d",
						 yourId, targetId, sym, count);
				sendMessageToClient(
					tcpClients[yourId].ipAddress,
					tcpClients[yourId].port,
					respS
				);
			
				// Puis on passe au joueur suivant (en sautant les éliminés)
				do {
					joueurCourant = (joueurCourant + 1) % 4;
				} while (!alive[joueurCourant]);
			
				// Et on notifie tous du nouveau tour
				char tourMsg[32];
				snprintf(tourMsg, sizeof(tourMsg), "T %d", joueurCourant);
				broadcastMessage(tourMsg);
			}
			break;
						
            default:
            break;
		}
        }
     	close(newsockfd);
     }
     close(sockfd);
     return 0; 
}