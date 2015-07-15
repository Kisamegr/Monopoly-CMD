#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include "screenUtils.h"

#define STARTING_CASH 10000


/*	  
	  ****************
      * DECLARATIONS *
	  ****************
*/

//Screen Input Functions
int RedArrow(int number,int x,int y,int sbl, int pages); //The basic function to get answers with visual graphics
void GetName(int player); //Reads player's name
int GetPawn(int av[]); //Reads player's pawn
int GetColor(); // Reads player's color
int RedArrowHighlight(int number,int x,int y,int sbl,int plot[]); //It is like RedArrow but for plot choosing. Uses Highlight function

//Screen Output Functions
void PrintGameBoard(int players,int player); //Combines ShowBoard, ShowPawns, ShowHHC and ShowPlayerStats
void ShowPlot(int orientation,int x,int y); //Prints a plot
void ShowBoard(); //Uses ShowPlot to print all board
void ShowPawns(int players); //Uses CalcPawnCoords(at Other Functions) to print all the pawns
void ShowHHC(); //Prints the Houses, the Hotels and the Color of the plots
void ShowDice(int dice,int num); //Prints a dice
void ShowPlayerStats(int player); //Show player's name, cash, pawn and owned plots at the right of the board
void ShowCash(int x,int y,int cash); //Shows the cash at the xx.xxx.xxx form
void ShowPlotCard(int plot); //Show the plot's card with it's properties

//Game Functions
void Game(int players,int player); //The main funcion of the game
int PlayerTurn(int player); //Shows/Returns the choice of the player (roll dices,buy hotel,buy house,quit)
void RollDices(int d[2]); //Calculates the 2 dices randomly and uses ShowDice to print them
void PlayerOnDecision(int player); //Called if a player goes on a decision block
void PlayerOnPlot(int player); //Called if a player goes on a plot
void BuyHouseHotel(int player,int what); //Function to buy house/hotel. if what==0 built house, if what==1 built hotel
void PlayerLost(int player); //Called if a player has lost (bankrupt)
int Pause(int players,int player); //The Pause menu, returns 1 if user selected End Game
void ResetGame(); //Resets everything for a new game

//File Associated Funcions
int Load(); //Calls all load functions and checks for errors. Returns 1 if all loaded successfully and 0 if there are errors and game cant start
int LoadPlots(); //Loads the plots properties from a file
int LoadDecisions(); //Loads the decisions from a file
int LoadOptions(); //Loads options from a file
int LoadGame(); //Loads the saved game from a file
void SaveGame(); //Saves the game to a file
void SaveOptions(); //Saves the options values

//Other Functions
void Window(); //Tell the user to lengthen the cmd window
void MsgBox(int number,int sbl); //The menu box
void endLine(); //Moves the cursor at the bottom right corner
void CalcPawnCoords(int i,int Coords[]); //Calculates the pawns coordinates on the screen based on the player's position
void clriob(); //Clears the Inside Of Board
void clrstats(); //Clears the players stats area
void clrscreen(); //Used 1 time at the start (before main menu) because background colors are staying with clrscr
void pak(int position); //Press Any Key to continue
void Highlight(int plot,int highlight); //Highlights a plot with magenta color, or the opposite
void NotAllColors(); //Prints a msg if the player does not have all the same colored cards to built a house


//Structs
struct Plots  //Struct of plots
{
	char Name[10];
	char Domain[10];
	unsigned short int Value;
	unsigned short int Rent;
	unsigned short int Houses:3; 
	unsigned short int Hotels:3;
	short int BelongsTo; //noone=-1, player1=0, player2=1 ...

} Plot[20];
  
struct Human  //Struct of players
{
	char Name[10];
	int Cash;
	unsigned short int Color:6;
	char Pawn;
	unsigned short int Position; //The position of the player, ex. 0 for START, 1 for first plot
	unsigned short int Lost:1; //0 if player is still playing, 1 if he Lost
} Player[16];

struct Game  //Struct of options
{
	int Starting_cash;
	int Cash_from_start;
} Options;

struct SaveLoad //Struct for save/load the game
{
	int Player_Turn; //Whose player turn is
	int Players; //How many players are
} Game_Save;

struct Decisions
{
	char Text1[26]; //The first line of the text of the decision
	char Text2[26]; //The second line
	int Action; //0 if player gives money, 1 if player takes money
	int Money; //The money the player gives/takes
} Decision[16];


/*	  
	  *****************
      * Main Function *
	  *****************
*/

void main()
{
	int i,menu,quit=0,players,p,pawn,loaded[4],option;
	int available[16];  //1 if pawn is available, 0 if pawn taken by another player
	
	srand(time(NULL));
	
	Window();
	
	do
	{
		//Print the menu
		clrscreen();
		MsgBox(3,1);
		setColor(lightgreen,black);
		gotoxy(33,11);
		printf("New Game");
		gotoxy(33,13);
		printf("Continue Game");
		gotoxy(33,15);
		printf("Options");

		menu=RedArrow(3,30,11,1,0);

		//New Game
		if (menu==0)  
		{
			clrscr();

			for (i=0;i<16;i++) //Initialisation of the available pawns array
				available[i]=1;

			if (Load())
			{
                //Reset the values
				ResetGame();

				//Get Player number
				MsgBox(2,1);
				setColor(lightgreen,black);
				gotoxy(25,12);
				printf("How many players (2-16): ");
				setColor(white,black);
				do
				{
					gotoxy(50,12);
					scanf("%d",&players);
				}
				while(players>16 || players<2);
			
				//Get players properties
				for(p=0;p<players;p++)
				{
					GetName(p);
					Player[p].Pawn=GetPawn(available);
					Player[p].Color=GetColor();
				}
                
				//Start the game
				Game(players,0);
			}
		}

		if (menu==1) //Continue Game
		{
			if (Load())
			{
				if(LoadGame())
					Game(Game_Save.Players,Game_Save.Player_Turn);
				else
				{
					clrscr();
					MsgBox(2,1);
					setColor(lightred,black);
					gotoxy(28,12);
					printf("There is no saved game");

					pak(0);
				}
			}
		}

		if (menu==2)
		{
			if (LoadOptions())
			{
				do
				{
					clrscr();
					MsgBox(2,2);
					setColor(lightwhite,blue);
					gotoxy(34,11);
					printf(" OPTIONS ");

					setColor(lightwhite,black);
					gotoxy(30,13);
					printf("Starting Cash: %d",Options.Starting_cash);
					gotoxy(30,14);
					printf("Cash from start: %d",Options.Cash_from_start);
					gotoxy(30,15);
					printf("Default Values");

					option=RedArrow(3,28,13,0,0);

					if (option==0)
					{
						clrscr();
						MsgBox(2,1);
						setColor(lightgreen,black);
						gotoxy(29,10);
						printf("Enter a new value for");
						gotoxy(32,11);
						printf("Starting Cash:");
						gotoxy(34,13);
						setColor(lightwhite,black);
						scanf("%d",&Options.Starting_cash);

						SaveOptions();
					}
					if (option==1)
					{
						clrscr();
						MsgBox(2,1);
						setColor(lightgreen,black);
						gotoxy(29,10);
						printf("Enter a new value for");
						gotoxy(31,11);
						printf("Cash from Start:");
						gotoxy(34,13);
						setColor(lightwhite,black);
						scanf("%d",&Options.Cash_from_start);

						SaveOptions();
					}
					if (option==2)
					{
						Options.Starting_cash=15000;
						Options.Cash_from_start=1500;

						SaveOptions();
					}
				
				}
				while (option!=-1);

			}
			else
			{
				clrscr();
				MsgBox(2,1);
				setColor(lightred,black);
				gotoxy(25,11);
				printf("Missing the options.ini file");
				gotoxy(27,12);
				printf("Options cant be changed");

				pak(0);
			}

		}

	}while(quit==0);
				
			
}

/*	  
	  **************************
      * Screen Input Functions *
	  **************************
*/
int RedArrow(int number,int x,int y,int sbl, int pages) //sbl: space between lines, number: number of choices, pages: if it has pages
{
	//27,11
	int k1,k2,position=0; //k1,k2 are the keys that the user presses
	
	setColor(lightred,black);

	gotoxy(x,y);  //puts the red arrow at the start
	printf("%c",26);

	endLine();

	do
	{
		k1=getch();
	}
	while (k1!=13 && k1!=224 && k1!=27);
		
	while (k1!=13 && k1!=27)
	{
		k2=getch();
		if (k2==80) //down arrow
		{
			gotoxy(x,y+(sbl+1)*position);  //"erases" the old red arrow
			printf(" ");
			if (position==number-1) //if it reached the last position
			{
				gotoxy(x,y);  //puts a new red arrow at the start
				printf("%c",26);
				position=0;
			}
			else
			{
				gotoxy(x,y+(sbl+1)*(position+1));  //puts a new red arrow at the next position
				printf("%c",26);
				position++;
			}
		}
		
		if (k2==72) //up arrow
		{
			gotoxy(x,y+(sbl+1)*position);  //"erases" the old red arrow
			printf(" ");
			if (position==0) //if it is at the first position
			{
				gotoxy(x,y+(sbl+1)*(number-1));  //puts a new red arrow at the end
				printf("%c",26);
				position=number-1;
			}
			else
			{
				gotoxy(x,y+(sbl+1)*(position-1));
				printf("%c",26);
				position--;
			}
		}
		if (pages)
		{
			if (k2==75) //left arrow
			{
				gotoxy(x,y+(sbl+1)*position);  //"erases" the old red arrow
				printf(" ");
				return -10; //returns -10 to go to the previous page
			}
		
			if (k2==77) //right arrow
			{
				gotoxy(x,y+(sbl+1)*position);  //"erases" the old red arrow
				printf(" ");
				return -11; //returns -11 to go to the next page
			}
		}
		endLine();
		do
		{
			k1=getch();
		}
		while (k1!=13 && k1!=224 && k1!=27); 
	}

	gotoxy(x,y+(sbl+1)*position);  //"erases" the red arrow
	printf(" ");

	if (k1==27)
		return -1;
	else
		return position;
	
}
void GetName(int player)
{
    int i,done=1;
    char name[100];
     
	clrscr();
	
	do
	{
        MsgBox(2,2);                   
    	gotoxy(27,11);
    	setColor(lightgreen,black);
    	printf("Player %d name (max 8 char)",player+1);
    	
    	if (done==0)
    	{
        	gotoxy(29,12);
            setColor(lightred,black);
            printf("Name already taken");
         }
    	do
    	{
    	    gotoxy(33,14);
            setColor(white,black);
            
    		scanf("%s",name);  //Read the name
    		if (strlen(name)>8)  //Check if name length is less than 8 char
    		{
    			MsgBox(2,2);
    			gotoxy(30,11);
    			setColor(lightred,black);
    			printf("Max 8 characters!");
    			gotoxy(33,14);
    			setColor(white,black);
    			fflush(stdin);
    		}
    	}
    	while (strlen(name)>8);
    	
        if (player!=0)  //Check if the name belongs to another player
        	for (i=0;i<player;i++)
        	    if (strcmp(name,Player[i].Name)==0)
        	    {
                    done=0;
                    break;
                }
                else
                    done=1;
    }
    while (done==0);
	
	strcpy(Player[player].Name,name);
}
int GetPawn(int av[])
{
	int i,pawn=-10,page,Pawns[16]={1,2,3,4,5,6,11,12,14,15,35,36,37,38,233,20}; //The ascii code of "pawns"

	clrscr();
	MsgBox(4,1);  
	gotoxy(27,13);
	setColor(lightgreen,black);
	printf("Choose your pawn");
	gotoxy(49,12);
	printf("Page:");

	do
	{
		if(pawn==-10)   //Page 1
		{
			page=1;
			
			setColor(lightwhite,black);
			gotoxy(50,14);
			printf("1");

			setColor(white,black);
			gotoxy(52,14);
			printf("2");
            			
			for(i=0;i<8;i++)
			{
				gotoxy(46,10+i);
				if (av[i])
				{
					setColor(white,black);
					printf("%c",Pawns[i]);
				}
				else  //If it is not available write it with red color
				{
					setColor(lightred,black);
					printf("%c",Pawns[i]);
				}
			
			}
			
			pawn=RedArrow(8,44,10,0,1);
			
			if (pawn!=-10 && pawn!=-11)
			{
				if (!av[pawn])
				{
					gotoxy(27,15);
					setColor(lightred,black);
					printf("Can't take red ");
					gotoxy(27,16);
					printf("colored pawns");
					pawn=-10;
				}
			}

		}
		if(pawn==-11)   //Page 2
		{
			page=2;
			
			setColor(white,black);
			gotoxy(50,14);
			printf("1");

			setColor(lightwhite,black);
			gotoxy(52,14);
			printf("2");
			
			for(i=8;i<16;i++)
			{
				gotoxy(46,10+i-8);
				if (av[i])
				{
					setColor(white,black);
					printf("%c",Pawns[i]);
				}
				else  //If it is not available write it with red color
				{
					setColor(lightred,black);
					printf("%c",Pawns[i]);
				}
			}

			pawn=RedArrow(8,44,10,0,1);
			
			if (pawn!=-10 && pawn!=-11)
			{
				if (!av[pawn+8])
				{
					gotoxy(27,15);
					setColor(lightred,black);
					printf("Can't take red ");
					gotoxy(27,16);
					printf("colored pawns");
					pawn=-11;
				}
			}
		}
	}
	while(pawn==-10 || pawn==-11);
	
	if(page==1)
	{
		av[pawn]=0;
		return Pawns[pawn];
	}
	else
	{
		av[pawn+8]=0;
		return Pawns[pawn+8];
	}

	
}
int GetColor()
{
	int i=8,color;

	clrscr();
	MsgBox(11,0);
	setColor(lightgreen,black);
	gotoxy(31,11);
	printf("Choose your color");

	setColor(i++,black);
	gotoxy(35,4+i);
	printf("Grey");

	setColor(i++,black);
	gotoxy(35,4+i);
	printf("Blue");

	setColor(i++,black);
	gotoxy(35,4+i);
	printf("Green");

	setColor(i++,black);
	gotoxy(35,4+i);
	printf("Cyan");

	setColor(i++,black);
	gotoxy(35,4+i);
	printf("Red");

	setColor(i++,black);
	gotoxy(35,4+i);
	printf("Magenta");

	setColor(i++,black);
	gotoxy(35,4+i);
	printf("Yellow");

	setColor(i++,black);
	gotoxy(35,4+i);
	printf("White");

	color=RedArrow(8,33,13,0,0);

	return color+8;

}
int RedArrowHighlight(int number,int x,int y,int sbl,int plot[])
{
	int k1,k2,position=0,p=0; //k1,k2 are the keys that the user presses
	
	setColor(lightred,black);

	gotoxy(x,y);  //puts the red arrow at the start
	printf("%c",26);

	Highlight(plot[0],1);
	
	endLine();

	do
	{
		k1=getch();
	}
	while (k1!=13 && k1!=224 && k1!=27);
		
	while (k1!=13 && k1!=27)
	{
		k2=getch();
		if (k2==80) //down arrow
		{
			gotoxy(x,y+(sbl+1)*position);  //"erases" the old red arrow
			printf(" ");

			Highlight(plot[position],0);  //De-highlight the previous plot

			if (position==number-1)
			{
				setColor(lightred,black);
				gotoxy(x,y);  //puts a new red arrow at the start
				printf("%c",26);
				position=0;
				Highlight(plot[position],1); //Highlight the new plot
			}
			else
			{
				setColor(lightred,black);
				gotoxy(x,y+(sbl+1)*(position+1));  //puts a new red arrow at the next position
				printf("%c",26);
				position++;
				Highlight(plot[position],1); //Highlight the new plot
			}
		}
		
		if (k2==72) //up arrow
		{
			gotoxy(x,y+(sbl+1)*position);  //"erases" the old red arrow
			printf(" ");

			Highlight(plot[position],0);  //De-highlight the previous plot

			if (position==0)
			{
				setColor(lightred,black);
				gotoxy(x,y+(sbl+1)*(number-1));  //puts a new red arrow at the end
				printf("%c",26);
				position=number-1;
				Highlight(plot[position],1); //Highlight the new plot
			}
			else
			{
				setColor(lightred,black);
				gotoxy(x,y+(sbl+1)*(position-1));
				printf("%c",26);
				position--;
				Highlight(plot[position],1); //Highlight the new plot
			}
		}
		
		endLine();
		do
		{
			k1=getch();
		}
		while (k1!=13 && k1!=224 && k1!=27); 
	}

	gotoxy(x,y+(sbl+1)*position);  //"erases" the red arrow
	printf(" ");

	Highlight(plot[position],0); //De-Highlight the plot

	if (k1==27)
		return -1;
	else
		return position;
	
}

/*	  
	 ***************************
     * Screen Output Functions *
	 ***************************
*/
void PrintGameBoard(int players,int player)
{
	clrscr();
	//Print the game board
	ShowBoard();
	ShowPawns(players);
	ShowHHC();

	//Print players stats
	ShowPlayerStats(player);
}
void ShowPlot(int orientation,int x,int y)
{
     int i;
	
     switch(orientation)
     {
         case 0:
              gotoxy(x,y);
              printf("%c",218);
              for (i=1;i<=10;i++)
                  printf("%c",196);
              printf("%c",191);
              for (i=1;i<=5;i++)
              {
                  gotoxy(x,y+i);
                  printf("%c          %c",179,179);
              }
              gotoxy(x,y+6);
              printf("%c",192);
              for (i=1;i<=10;i++)
                  printf("%c",196);
              printf("%c",217);  
              break;               
     
         case 1:
              gotoxy(x,y);
              printf("%c",218);
              for (i=0;i<5;i++)
                  printf("%c",196);
              printf("%c",191);
              gotoxy(x,y+1);
              printf("%c",179);
			  gotoxy(x+6,y+1);
			  printf("%c",179);
              gotoxy(x,y+2);
              printf("%c",195);
              for (i=0;i<5;i++)
                  printf("%c",196);
              printf("%c",180);

              for (i=0;i<3;i++)
              {
				  gotoxy(x,y+3+i);
                  printf("%c",179);
				  gotoxy(x+6,y+3+i);
                  printf("%c",179);
              }
			  gotoxy(x,y+6);
              printf("%c",192);
              for(i=0;i<5;i++)
                  printf("%c",196);
              printf("%c",217);
              break;
         
         case 2:
              gotoxy(x,y);
              printf("%c",218);
              for (i=0;i<6;i++)
                  printf("%c",196);
              printf("%c%c%c%c%c",194,196,196,196,191);
              for (i=1;i<=3;i++)
              {
                  gotoxy(x,y+i);
                  printf("%c",179);
				  gotoxy(x+7,y+i);
				  printf("%c",179);
				  gotoxy(x+11,y+i);
				  printf("%c",179);
              }
              gotoxy(x,y+4);
              printf("%c",192);
              for (i=0;i<6;i++)
                  printf("%c",196);
              printf("%c%c%c%c%c",193,196,196,196,217);
              break;
              
         case 3:
              gotoxy(x,y);
              printf("%c",218);
              for (i=0;i<5;i++)
                  printf("%c",196);
              printf("%c",191);

               for (i=0;i<3;i++)
              {
				  gotoxy(x,y+1+i);
                  printf("%c",179);
				  gotoxy(x+6,y+1+i);
                  printf("%c",179);
              }

			  gotoxy(x,y+4);
              printf("%c",195);
              for (i=0;i<5;i++)
                  printf("%c",196);
              printf("%c",180);
              gotoxy(x,y+5);
              printf("%c",179);
			  gotoxy(x+6,y+5);
			  printf("%c",179);

              gotoxy(x,y+6);
              printf("%c",192);
              for(i=0;i<5;i++)
                  printf("%c",196);
              printf("%c",217);
              break;
              
         case 4:
              gotoxy(x,y);
              printf("%c%c%c%c%c",218,196,196,196,194);
              for (i=0;i<6;i++)
                  printf("%c",196);
              printf("%c",191);
              for (i=1;i<=3;i++)
              {
				  gotoxy(x,y+i);
                  printf("%c",179);
				  gotoxy(x+4,y+i);
				  printf("%c",179);
				  gotoxy(x+11,y+i);
				  printf("%c",179);
              }
              gotoxy(x,y+4);
              printf("%c%c%c%c%c",192,196,196,196,193);
              for (i=0;i<6;i++)
                  printf("%c",196);
              printf("%c",217);
              break;
     }
}             
void ShowBoard()
{
     int i;
	 setColor(lightwhite,black);
     ShowPlot(0,0,0);
     for (i=0;i<4;i++)
         ShowPlot(3,12+i*7,0);
     ShowPlot(0,40,0);
     for (i=0;i<4;i++)
         ShowPlot(4,40,7+i*5);
     ShowPlot(0,40,27);
     for (i=0;i<4;i++)
         ShowPlot(1,12+i*7,27);
     ShowPlot(0,0,27);
     for (i=0;i<4;i++)
         ShowPlot(2,0,7+i*5);

	 //Print start, decision and free parking names
	 setColor(white,black);
	 gotoxy(1,1);
	 printf("%s",Plot[0].Name);
	 gotoxy(41,1);
	 printf("%s",Plot[5].Name);
	 gotoxy(41,28);
	 printf("%s",Plot[10].Name);
	 gotoxy(1,28);
	 printf("%s",Plot[15].Name);
}
void ShowPawns(int players)
{
	int i,Coordinates[2];
	for (i=0;i<players;i++)
	{
       	if (!Player[i].Lost)
        {		
    		CalcPawnCoords(i,Coordinates);
    		gotoxy(Coordinates[0],Coordinates[1]);
    		setColor(Player[i].Color,black);
    		printf("%c",Player[i].Pawn);
        }
	}
}
void ShowHHC()
{
	int i;
	//Gray plots
	setColor(lightwhite,gray);
	for (i=1;i<=2;i++)
	{
		gotoxy(13+7*(i-1),5);
		printf(" %d %d ",Plot[i].Houses,Plot[i].Hotels);
	}
	//Cyan plots
	setColor(lightwhite,lightcyan);
	for (i=3;i<=4;i++)
	{
		gotoxy(13+7*(i-1),5);
		printf(" %d %d ",Plot[i].Houses,Plot[i].Hotels);
	}
	gotoxy(41,8);
	printf(" %d ",Plot[6].Houses);
	gotoxy(41,9);
	printf("   ");
	gotoxy(41,10);
	printf(" %d ",Plot[6].Hotels);
	//Red plots
	setColor(lightwhite,lightred);
	for (i=7;i<=9;i++)
	{
		gotoxy(41,13+5*(i-7));
		printf(" %d ",Plot[i].Houses);
		gotoxy(41,14+5*(i-7));
		printf("   ");
		gotoxy(41,15+5*(i-7));
		printf(" %d ",Plot[i].Hotels);
	}
	//Yellow plots
	setColor(lightwhite,lightyellow);
	for (i=11;i<=13;i++)
	{
		gotoxy(34-7*(i-11),28);
		printf(" %d %d ",Plot[i].Houses,Plot[i].Hotels);
	}
	//Green plots
	setColor(lightwhite,lightgreen);
	gotoxy(13,28);
	printf(" %d %d ",Plot[i].Houses,Plot[i].Hotels);
	for (i=16;i<=17;i++)
	{
		gotoxy(8,23-5*(i-16));
		printf(" %d ",Plot[i].Houses);
		gotoxy(8,24-5*(i-16));
		printf("   ");
		gotoxy(8,25-5*(i-16));
		printf(" %d ",Plot[i].Hotels);
	}
	//Blue plots
	setColor(lightwhite,lightblue);
	for (i=18;i<=19;i++)
	{
		gotoxy(8,13-5*(i-18));
		printf(" %d ",Plot[i].Houses);
		gotoxy(8,14-5*(i-18));
		printf("   ");
		gotoxy(8,15-5*(i-18));
		printf(" %d ",Plot[i].Hotels);
	}
}
void ShowDice(int dice,int num)
{
	int i=16;
    
	if (num==1)
	{
        gotoxy(16+(dice-1)*12,i++);
        printf(" _______  ");
        gotoxy(16+(dice-1)*12,i++);
        printf("/       %c ",92);
        gotoxy(16+(dice-1)*12,i++);
        printf("|       | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("|   O   | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("|       | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("%c_______/ ",92);
    }
    
    if (num==2)
    {
        gotoxy(16+(dice-1)*12,i++);
        printf(" _______  ");
        gotoxy(16+(dice-1)*12,i++);
        printf("/       %c ",92);
        gotoxy(16+(dice-1)*12,i++);
        printf("|     O | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("|       | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("| O     | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("%c_______/ ",92);
    }
    
    if (num==3)
	{
        gotoxy(16+(dice-1)*12,i++);
        printf(" _______  ");
        gotoxy(16+(dice-1)*12,i++);
        printf("/       %c ",92);
        gotoxy(16+(dice-1)*12,i++);
        printf("|     O | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("|   O   | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("| O     | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("%c_______/ ",92);
    }
    
    if (num==4)
	{
        gotoxy(16+(dice-1)*12,i++);
        printf(" _______  ");
        gotoxy(16+(dice-1)*12,i++);
        printf("/       %c ",92);
        gotoxy(16+(dice-1)*12,i++);
        printf("| O   O | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("|       | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("| O   O | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("%c_______/ ",92);
    }
    
    if (num==5)
	{
        gotoxy(16+(dice-1)*12,i++);
        printf(" _______  ");
        gotoxy(16+(dice-1)*12,i++);
        printf("/       %c ",92);
        gotoxy(16+(dice-1)*12,i++);
        printf("| O   O | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("|   O   | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("| O   O | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("%c_______/ ",92);
    }
    
    if (num==6)
	{
        gotoxy(16+(dice-1)*12,i++);
        printf(" _______  ");
        gotoxy(16+(dice-1)*12,i++);
        printf("/       %c ",92);
        gotoxy(16+(dice-1)*12,i++);
        printf("| O   O | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("| O   O | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("| O   O | ");
        gotoxy(16+(dice-1)*12,i++);
        printf("%c_______/ ",92);
    }
}
void ShowPlayerStats(int player)
{
	int cash=Player[player].Cash,i=0,n;

	setColor(yellow,black);   //************
	gotoxy(60,2);             //* Player X *
	printf("*************");  //************
	gotoxy(60,4);
	printf("*************");
	gotoxy(60,3);
	printf("*           *");
	gotoxy(61,3);
	setColor(lightwhite,yellow);
	if (player<10)
		printf(" Player  %d ",player+1);
	else
		printf(" Player %d ",player+1);

	gotoxy(57,8);  //Player name
	setColor(lightwhite,black);
	printf("Name: ");
	setColor(Player[player].Color,black);
	printf("%s",Player[player].Name);

	gotoxy(57,10);  //Player pawn
	setColor(lightwhite,black);
	printf("Pawn: ");
	setColor(Player[player].Color,black);
	printf("%c",Player[player].Pawn);

	
	gotoxy(57,12);  //Player cash
	setColor(lightwhite,black);
	printf("Cash: ");

	ShowCash(64,12,cash);

	setColor(lightwhite,black); //Print the plots the player ownes
	gotoxy(55,15);
	printf("Cards Owned:");
	i=17;
	setColor(gray,black);
	for (n=1;n<=2;n++)
	{
		if (Plot[n].BelongsTo==player)
		{
			gotoxy(56,i++);
			printf("%s",Plot[n].Name);
		}
	}

	setColor(lightcyan,black);
	for (n=3;n<=6;n++)
	{
		if (n!=5)
		{
			if (Plot[n].BelongsTo==player)
			{
				gotoxy(56,i++);
				printf("%s",Plot[n].Name);
			}
		}
	}

	setColor(lightred,black);
	for (n=7;n<=9;n++)
	{
		if (Plot[n].BelongsTo==player)
		{
			gotoxy(56,i++);
			printf("%s",Plot[n].Name);
		}
	}

	setColor(lightyellow,black);
	for (n=11;n<=13;n++)
	{
		if (Plot[n].BelongsTo==player)
		{
			gotoxy(56,i++);
			printf("%s",Plot[n].Name);
		}
	}

	setColor(lightgreen,black);
	for (n=14;n<=17;n++)
	{
		if (n!=15)
		{
			if (Plot[n].BelongsTo==player)
			{
				gotoxy(56,i++);
				printf("%s",Plot[n].Name);
			}
		}
	}

	setColor(lightblue,black);
	for (n=18;n<=19;n++)
	{
		if (Plot[n].BelongsTo==player)
		{
			gotoxy(56,i++);
			printf("%s",Plot[n].Name);
		}
	}

}
void ShowCash(int x,int y,int cash)
{
	int digit[20],i=0,n=0,dot=0,lastwasdot=1;

	while (cash!=0)  //Find the cash digits and put them at the digit[] array
	{
		digit[i]=cash%10;
		cash=cash/10;
		i++;
	}
	i--;  //Cause i is 1 higher from the digits number

	gotoxy(x+i+i/3+1,y);
	printf("$");

	for (n=0;n<=i+dot;n++)  //Print cash at the xx.xxx.xxx form
	{
		gotoxy(x+i+i/3-n,y);
     	if (  ( ( ((n-dot)%3)!=0 || lastwasdot==1) ) && !(  ( ((n-dot)%3)!=0 && lastwasdot==1) )   )
		{
			printf("%d",digit[n-dot]);
			lastwasdot=0;
		}
		else
		{
			printf(".");
			lastwasdot=1;
			dot++;
		}
	}
}
void ShowPlotCard(int plot)
{
	int i,nspace,dspace;

	nspace=14-strlen(Plot[plot].Name);
	dspace=14-strlen(Plot[plot].Domain);

	clriob();
	
	//Show card
	setColor(lightwhite,black);
	gotoxy(13,8); 
    printf("%c",218);
    for (i=0;i<14;i++)
        printf("%c",196);
    printf("%c",191);
   
	gotoxy(13,9);
    printf("%c              %c",179,179);
	gotoxy(13,10);
    printf("%c              %c",179,179);

	if (plot<=2)
		setColor(lightwhite,gray);
	else
	if (plot<=6)
		setColor(lightwhite,lightcyan);
	else
	if (plot<=9)
		setColor(lightwhite,lightred);
	else
	if (plot<=13)
		setColor(lightwhite,lightyellow);
	else
	if (plot<=17)
		setColor(lightwhite,lightgreen);
	else
	if (plot<=19)
		setColor(lightwhite,lightblue);

	gotoxy(14,9);
	if (nspace%2==0)
	{
		for (i=0;i<(nspace/2);i++)
			printf(" ");
		printf("%s",Plot[plot].Name);
		for (i=0;i<(nspace/2);i++)
			printf(" ");
	}
	else
	{
		for (i=0;i<(nspace/2);i++)
			printf(" ");
		printf("%s",Plot[plot].Name);
		for (i=0;i<(nspace/2+1);i++)
			printf(" ");
	}

	gotoxy(14,10);
	if (dspace%2==0)
	{
		for (i=0;i<(dspace/2);i++)
			printf(" ");
		printf("%s",Plot[plot].Domain);
		for (i=0;i<(dspace/2);i++)
			printf(" ");
	}
	else
	{
		for (i=0;i<(dspace/2);i++)
			printf(" ");
		printf("%s",Plot[plot].Domain);
		for (i=0;i<(dspace/2+1);i++)
			printf(" ");
	}

	setColor(lightwhite,black);
	gotoxy(13,11);
    printf("%c",195);
    for (i=0;i<14;i++)
        printf("%c",196);
    printf("%c",180);
    for (i=12;i<=23;i++)
	{
		gotoxy(13,i);
		printf("%c              %c",179,179);
	}
    gotoxy(13,24);
    printf("%c",192);
    for(i=0;i<14;i++)
        printf("%c",196);
    printf("%c",217);

	//Show plots properties on card
	setColor(lightwhite,yellow); //Rent
	gotoxy(14,12);
	printf("Rent: %d$",Plot[plot].Rent);
	
	setColor(lightwhite,green); //Houses
	gotoxy(14,13);
	printf("    Houses    ");
	setColor(lightwhite,black);
	for (i=1;i<=4;i++)
	{
		gotoxy(14,13+i);
		printf("%d.",i);
		ShowCash(17,13+i,Plot[plot].Rent + i * 0.5 * Plot[plot].Rent);
	}

	setColor(lightwhite,red); //Hotels
	gotoxy(14,18);
	printf("    Hotels    ");
	setColor(lightwhite,black);
	for (i=1;i<=4;i++)
	{
		gotoxy(14,18+i);
		printf("%d.",i);
		ShowCash(17,18+i,Plot[plot].Rent + i * 2.5 * Plot[plot].Rent);
	}
}

/*	  
	 ******************
     * Game Functions *
	 ******************
*/
void Game(int players,int player)
{
	int i,choice,active_players,dice[2],had_doubles,dsum,passed_start,quit,has_rolled,winner=-1,pause;

	/*
	active_players:   the number that shows how many players are playing (have not lost)
	dice[2]:          it holds the 2 dice numbers
	had_doubles:      1 if dice[0]==dice[1] (player had doubles) and the player will play again
	dsum:             dice[0]+dice[1]
	passed_start:     1 if player passed start, so that he takes the Options.Cash_from_start
	quit:             just a variable to insert the choice the player makes when he chooses Quit (at the warning msg)
	has_rolled:       if this variable is 0, it means the player has not rolled, so the same player has to play again
	winner:           the winner's number is inserted here after the game ends
	pause:            just a variable to insert the choice the player makes when he chooses an option at pause menu
	*/

	active_players=players;

	i=player;

	while (active_players>1)
	{
		if (Player[i].Lost==0)
		{
			passed_start=0;
			has_rolled=0;
			had_doubles=0;
			
			PrintGameBoard(players,i);

			//Get players choice
			choice = PlayerTurn(i);

			if (choice==-1) //Pause
			{
				pause=Pause(players,i);

				if (pause==1)
					active_players=0; //Everybody loses cause game ended
			}

			if (choice==0) //Roll Dices
			{
				clriob();
				RollDices(dice);
				if (dice[0]==dice[1]) //If he had doubles
					had_doubles=1;

				dsum=dice[0]+dice[1];

				if (Player[i].Position+dsum<=19) //If the potision and dice sum exceeds 19 then the player has passed start
					Player[i].Position=Player[i].Position+dsum;
				else
				{
					Player[i].Position=dsum-(20-Player[i].Position);
					if (Player[i].Position!=0)
						passed_start=1;
				}

				PrintGameBoard(players,i);

				if (passed_start && Player[i].Position!=0) //Player passed start
				{
					clriob;
					setColor(lightcyan,black);
					gotoxy(18,14);
					printf("You passed Start");
					gotoxy(20,16);
					printf("You gain %d$",Options.Cash_from_start);
					Player[i].Cash=Player[i].Cash+Options.Cash_from_start;

					pak(1);

					clrstats();
					ShowPlayerStats(i);
				}

				if (Player[i].Position==0) //Player went to start
				{
					clriob();
					setColor(lightcyan,black);
					gotoxy(18,14);
					printf("You hit on Start");
					gotoxy(20,16);
					printf("You gain %d$",Options.Cash_from_start);
					Player[i].Cash = Player[i].Cash + Options.Cash_from_start;
					
					pak(1);

					clrstats();
					ShowPlayerStats(i);
				}
				else
				if (Player[i].Position==5 || Player[i].Position==15) //Player went to decision
					PlayerOnDecision(i);
				else
				if (Player[i].Position==10) //Player went to free parking
				{
					clriob();
					setColor(lightcyan,black);
					gotoxy(16,14);
					printf("You hit on Free Parking");
					
					pak(1);
				}
				else    //Player went on a plot
					PlayerOnPlot(i);

				has_rolled=1;
			}

			if (choice==1) //Buy house
				BuyHouseHotel(i,0);
			
			if (choice==2) //Buy hotel
				BuyHouseHotel(i,1);

			if (choice==3) //Quit
			{
				clriob();
				setColor(lightyellow,black); //Ask if he is sure
				gotoxy(18,14);
				printf("Are you sure you");
				gotoxy(20,15);
				printf("want to quit?");
				
				setColor(lightwhite,black);
				gotoxy(24,17);
				printf("No");
				gotoxy(24,18);
				printf("Yes");
				quit=RedArrow(2,22,17,0,0);

				if (quit==1)
				{
					PlayerLost(i);
					active_players--;
				}
			}

			if (Player[i].Cash<=0) //If the player's cash reach 0 or bellow, the player automatically loses
			{
				PlayerLost(i);
				active_players--;
			}
			else
			if (has_rolled==0 || had_doubles==1) //Decrease i by 1, because it will be increased by 1 afterwards and the same player will play again
				i--;
		}

		if (i<players-1) //Increase i to proceed to the next player turn
			i++;
		else  //Make i=0 when the last player played to continue with the first
			i=0;

	}

	if (active_players==1) //If there is only 1 left (the winner)
	{
		i=0;
		//Find the winner (the only person who has not lost)
		while (winner==-1)
		{
			if (Player[i].Lost==0)
				winner=i;
			else
				i++;
		}

		//Print winning msg
		clriob();
		setColor(lightwhite,green);
		gotoxy(17,9);
		printf("                  ");
		gotoxy(17,10);
		printf(" CONGRATULATIONS! ");
		gotoxy(17,11);
		printf("                  ");
		setColor(Player[winner].Color,black);
		gotoxy(14,15);
		printf("%s",Player[winner].Name);
		setColor(lightwhite,black);
		printf(" has won the game");

		pak(1);
	}
}
int PlayerTurn(int player)
{
	//XXXXXX's Turn   msg
	clriob();
	setColor(Player[player].Color,black);
	gotoxy(20,15);
	printf("%s",Player[player].Name);
	setColor(lightcyan,black);
	printf("'s Turn");

	pak(1);

	//Show the menu
	clriob();
	setColor(lightgreen,black);
	gotoxy(20,13);
	printf("Choose an option");

	setColor(lightwhite,black);
	gotoxy(21,17);
	printf("Roll Dices");
	gotoxy(21,19);
	printf("Buy House");
	gotoxy(21,21);
	printf("Buy Hotel");
	gotoxy(21,23);
	printf("Quit");

	setColor(gray,black);
	gotoxy(14,25);
	printf("Press ESC for pause menu");

	return RedArrow(4,18,17,1,0);
}
void RollDices(int d[2])
{
	d[0]=rand() % 6 + 1;
	d[1]=rand() % 6 + 1;

	setColor(lightgreen,black);
	gotoxy(21,13);
	printf("You rolled: ");

	setColor(lightwhite,black);
	ShowDice(1,d[0]);
	ShowDice(2,d[1]);

	pak(1);

}		
void PlayerOnDecision(int player)
{
	int decision;

	decision=rand()%16; //Generate a random decision

	clriob();
	setColor(lightcyan,lightblue);
	gotoxy(22,14);
	printf("DECISION");
	setColor(lightwhite,black);
	gotoxy(13,16);
	printf(" %s",Decision[decision].Text1); //Write the text of the decision
	gotoxy(13,17);
	printf(" %s",Decision[decision].Text2);
	
	ShowCash(24,19,Decision[decision].Money);

	pak(1);

	//Take/Give the money
	if (Decision[decision].Action==0) 
		Player[player].Cash=Player[player].Cash-Decision[decision].Money;
	if (Decision[decision].Action==1)
		Player[player].Cash=Player[player].Cash+Decision[decision].Money;
}
void PlayerOnPlot(int player)
{
	int plot,choice,payment;

	plot=Player[player].Position;

	ShowPlotCard(plot);

	if (Plot[plot].BelongsTo==-1) //Plot belongs to noone
	{
		setColor(lightcyan,black);
		gotoxy(31,9);
		printf("Value:");
		gotoxy(31,14);
		printf("Choose:");

		setColor(lightwhite,black);
		gotoxy(32,11);
		printf("%d$",Plot[plot].Value);
		gotoxy(33,16);
		printf("Buy");
		gotoxy(33,17);
		printf("Pass");

		choice=RedArrow(2,31,16,0,0);

		if (choice==0) //Buy
		{
			if (Player[player].Cash-Plot[plot].Value>0) //If he has the money
			{
				Player[player].Cash=Player[player].Cash-Plot[plot].Value; //Make the transaction

				Plot[plot].BelongsTo=player;

				clriob();
				setColor(lightcyan,black);
				gotoxy(18,15);
				printf("You bought %s",Plot[plot].Name);
				gotoxy(22,16);
				printf("of %s",Plot[plot].Domain);

				clrstats();
				ShowPlayerStats(player);

				pak(1);
			}
			else //No money to buy, so the player must pass
			{
                clriob();
				setColor(lightred,black);
				gotoxy(16,14);
				printf("You dont enough money");
				gotoxy(19,16);
				printf("to buy the card");

				pak(1);
			}
		}
	}
	else  //Plot belongs to a player
	{
		if (Plot[plot].BelongsTo==player) //Plot belongs to the active player
		{
			setColor(lightcyan,black);
			gotoxy(31,15);
			printf("Already");
			gotoxy(32,16);
			printf("Owned");

			pak(1);
		}
		else //Plot belongs to another player
		{
			setColor(lightcyan,black); //Print the info
			gotoxy(30,9);
			printf("Owner");
			gotoxy(30,14);
			printf("Houses");
			gotoxy(30,15);
			printf("Hotels");
			gotoxy(30,18);
			printf("Payment");

			setColor(Player[Plot[plot].BelongsTo].Color,black);
			gotoxy(30,11);
			printf("%s",Player[Plot[plot].BelongsTo].Name);

			setColor(lightwhite,black);
			gotoxy(37,14);
			printf("%d",Plot[plot].Houses);
			gotoxy(37,15);
			printf("%d",Plot[plot].Hotels);
		
			payment= Plot[plot].Rent + Plot[plot].Houses * 0.5 * Plot[plot].Rent + Plot[plot].Hotels * 2.5 * Plot[plot].Rent;
			ShowCash(31,20,payment);

			if (payment<Player[player].Cash) //Player has money to pay
			{
				Player[player].Cash=Player[player].Cash-payment;
				Player[Plot[plot].BelongsTo].Cash=Player[Plot[plot].BelongsTo].Cash+payment;

				setColor(lightgreen,black);
				gotoxy(30,22);
				printf("Payment");
				gotoxy(31,23);
				printf("Given");

				pak(1);
			}
			else //Player does not have the money
			{
				Player[Plot[plot].BelongsTo].Cash=Player[Plot[plot].BelongsTo].Cash+Player[player].Cash;
				Player[player].Cash=0;

				setColor(lightred,black);
				gotoxy(29,22);
				printf("Not enough");
				gotoxy(31,23);
				printf("money!");

				pak(1);
			}
		}
	}
}
void BuyHouseHotel(int player,int what)
{
	int i=0,counter=0,plot[16],choice;

	while (i<20) //Find the plots that belong to the player and put them at the plot[16] array
	{
		if (Plot[i].BelongsTo==player)
		{
			plot[counter]=i;
			counter++;
			i++;
		}
		else
			i++;
	}

	if (counter!=0) //If player has cards
	{
		clriob();
		setColor(lightcyan,black);
		gotoxy(19,9);
		printf("Choose a card:");
		setColor(lightwhite,black);

		for (i=0;i<counter;i++)
		{
			gotoxy(20,11+i);
			printf("%s ",Plot[plot[i]].Name);
			if (what==0)
				printf("%d$",Plot[plot[i]].Rent);
			else
				printf("%d$",5*Plot[plot[i]].Rent);
		}

		setColor(gray,black);
		gotoxy(15,25);
		printf("Press ESC to cancel");

		choice=RedArrowHighlight(counter,18,11,0,plot);

		if (choice!=-1)
		{
			//Built house
			if (what==0) 
			{
				if (Plot[plot[choice]].Houses<4 && Plot[plot[choice]].Hotels==0) //If the houses are less than 4 and he has no hotels
				{
					if (Plot[plot[choice]].Rent<Player[player].Cash) //If the player has the money
					{
						if (plot[choice]==1 || plot[choice]==18) //First gray/blue plot
						{
							if (Plot[plot[choice]+1].BelongsTo==player)
							{
								Plot[plot[choice]].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[plot[choice]].Rent;
							}
							else
								NotAllColors();
						}

						if (plot[choice]==2 || plot[choice]==19) //Second gray/blue plot
						{
							if (Plot[plot[choice]-1].BelongsTo==player)
							{
								Plot[plot[choice]].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[plot[choice]].Rent;
							}
							else
								NotAllColors();
						}

						if (plot[choice]==7 || plot[choice]==11) //First red/yellow plot
						{
							if (Plot[plot[choice]+1].BelongsTo==player && Plot[plot[choice]+2].BelongsTo==player)
							{
								Plot[plot[choice]].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[plot[choice]].Rent;
							}
							else
								NotAllColors();
						}

						if (plot[choice]==8 || plot[choice]==12) //Second red/yellow plot
						{
							if (Plot[plot[choice]+1].BelongsTo==player && Plot[plot[choice]-1].BelongsTo==player)
							{
								Plot[plot[choice]].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[plot[choice]].Rent;
							}
							else
								NotAllColors();
						}

						if (plot[choice]==9 || plot[choice]==13) //Third red/yellow plot
						{
							if (Plot[plot[choice]-1].BelongsTo==player && Plot[plot[choice]-2].BelongsTo==player)
							{
								Plot[plot[choice]].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[plot[choice]].Rent;
							}
							else
								NotAllColors();
						}

			
						if (plot[choice]==3) //First cyan plot
						{
							if (Plot[4].BelongsTo==player && Plot[6].BelongsTo==player)
							{
								Plot[3].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[3].Rent;
							}
							else
								NotAllColors();
						}

						if (plot[choice]==4) //Second cyan plot
						{
							if (Plot[3].BelongsTo==player && Plot[6].BelongsTo==player)
							{
								Plot[4].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[4].Rent;
							}
							else
								NotAllColors();
						}

						if (plot[choice]==6) //Third cyan plot
						{
							if (Plot[3].BelongsTo==player && Plot[4].BelongsTo==player)
							{
								Plot[6].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[6].Rent;
							}
							else
								NotAllColors();
						}

						if (plot[choice]==14) //First green plot
						{
							if (Plot[16].BelongsTo==player && Plot[17].BelongsTo==player)
							{
								Plot[14].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[14].Rent;
							}
							else
								NotAllColors();
						}

						if (plot[choice]==16) //Second green plot
						{
							if (Plot[13].BelongsTo==player && Plot[17].BelongsTo==player)
							{
								Plot[16].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[16].Rent;
							}
							else
								NotAllColors();
						}

						if (plot[choice]==17) //Third green plot
						{
							if (Plot[16].BelongsTo==player && Plot[13].BelongsTo==player)
							{
								Plot[17].Houses++;
								Player[player].Cash=Player[player].Cash-Plot[17].Rent;
							}
							else
								NotAllColors();
						}
					}
					else
					{
						clriob();
						setColor(lightred,black);
						gotoxy(19,14);
						printf("You dont have");
						gotoxy(19,15);
						printf("enough money");

						pak(1);
					}
				}
				else
				{
					clriob();
					setColor(lightred,black);
					gotoxy(18,14);
					printf("You already have");
					gotoxy(21,15);
					printf("4 houses or");
					gotoxy(23,16);
					printf("hotels");

					pak(1);
				}
			}
		
			//Built hotel
			else
			if (what==1 && (Plot[plot[choice]].Houses==4 || Plot[plot[choice]].Hotels>0)) 
			{
				if (Plot[plot[choice]].Hotels<4) //If the hotels are less than 4
				{
					if ((2.5*Plot[plot[choice]].Rent)<Player[player].Cash) //If the player has the money
					{
						Plot[plot[choice]].Hotels++;
						Plot[plot[choice]].Houses=0;
						Player[player].Cash=Player[player].Cash-5*Plot[plot[choice]].Rent;
					}
					else
					{
						clriob();
						setColor(lightred,black);
						gotoxy(19,14);
						printf("You dont have");
						gotoxy(19,15);
						printf("enough money");

						pak(1);
					}
				}
				else
				{
					clriob();
					setColor(lightred,black);
					gotoxy(18,14);
					printf("You already have");
					gotoxy(22,15);
					printf("4 hotels");

					pak(1);
				}
			}
			else
			{
				clriob();
				setColor(lightred,black);
				gotoxy(19,14);
				printf("You cant build a");
				gotoxy(21,15);
				printf("hotel here");

				pak(1);
			}
		}
	}
	else
	{
		clriob();
		setColor(lightred,black);
		gotoxy(19,14);
		printf("You dont have");
		gotoxy(21,15);
		printf("any cards");

		pak(1);
	}
}
void PlayerLost(int player)
{
	int i;

	clriob();
	setColor(Player[player].Color,black);
	gotoxy(18,15);
	printf("%s",Player[player].Name);
	setColor(lightcyan,black);
	printf(" has lost");
	gotoxy(14,17);
	printf("Better luck next time!");

	for (i=0;i<20;i++)
		if (Plot[i].BelongsTo==player)
		{
			Plot[i].BelongsTo=-1;
			Plot[i].Hotels=0;
			Plot[i].Houses=0;
		}

	Player[player].Lost=1;

	pak(1);
}
int Pause(int players,int player)
{
	int choice,c;

	do
	{
		clriob();
		setColor(lightwhite,magenta); //Print the menu
		gotoxy(22,10);
		printf("-PAUSE-");
		setColor(lightgreen,black);
		gotoxy(21,13);
		printf("Resume Game");
		gotoxy(21,15);
		printf("Save Game");
		gotoxy(21,17);
		printf("End Game");

		choice=RedArrow(3,19,13,1,0);

		if (choice==-1) //Back
			return 0;

		if (choice==0) //Resume Game
			return 0;

		if (choice==1) //Save Game
		{
			clriob();
			setColor(lightyellow,black); //Ask a question if he is sure
			gotoxy(14,10);
			printf("This will overwrite any");
			gotoxy(16,11);
			printf("previous saved game");
			gotoxy(14,13);
			printf("Do you wish to continue?");

			setColor(lightwhite,black);
			gotoxy(25,15);
			printf("No");
			gotoxy(25,16);
			printf("Yes");

			c=RedArrow(2,23,15,0,0);

			if (c==1) //Yes
			{
				Game_Save.Players=players;
				Game_Save.Player_Turn=player;

				SaveGame();
				clriob();
				setColor(lightcyan,black);
				gotoxy(20,15);
				printf("Game Saved");
				gotoxy(19,16);
				printf("Successfully");
				choice=-2;

				pak(1);
				
			}
			else
				choice=-2;
		}

		if (choice==2) //End Game
		{
			clriob();
			setColor(lightyellow,black); //Ask a question if he is sure
			gotoxy(17,10);
			printf("Are you sure you");
			gotoxy(17,11);
			printf("want to end game?");
			gotoxy(16,13);
			printf("Any unsaved progress");
			gotoxy(19,14);
			printf("will be lost");

			setColor(lightwhite,black);
			gotoxy(25,16);
			printf("No");
			gotoxy(25,17);
			printf("Yes");

			c=RedArrow(2,23,16,0,0);

			if (c==1) //Yes
				return 1;
			else
				choice=-2;
		}
	}
	while (choice==-2);
}
void ResetGame()
{
    int i;
	
	//Reset the board
	strcpy(Plot[0].Name,"  Start   ");  //Start Block

	strcpy(Plot[5].Name," Decision "); //Decision Blocks
	strcpy(Plot[15].Name," Decision ");

	strcpy(Plot[10].Name,"Free Park."); //Free Parking Block

	for (i=0;i<20;i++)
	{
		Plot[i].Houses=0;
		Plot[i].Hotels=0;
		Plot[i].BelongsTo=-1;
	}

	//Reset the Players
	for (i=0;i<16;i++)
	{
		Player[i].Cash=Options.Starting_cash;
		Player[i].Position=0;
		Player[i].Lost=0;
		Player[i].Pawn=-1;
	}
}

/*	  
	 ****************************
     * File Associated Funcions *
	 ****************************
*/
int Load()
{
	int loaded[4];

	//Load everything
	loaded[0]=LoadPlots();
	loaded[1]=LoadDecisionTexts();
	loaded[2]=LoadDecisionValues();
	loaded[3]=LoadOptions();

	if (loaded[0]==0) //Missing the plots file
	{
		clrscr();
		MsgBox(2,1);
		setColor(lightred,black);
		gotoxy(26,11);
		printf("Missing the plots.mnpl file");
		gotoxy(30,12);
		printf("Game cant start!");

		pak(0);
	}
	if (loaded[1]==0) //Missing the decision_texts file
	{
		clrscr();
		MsgBox(2,1);
		setColor(lightred,black);
		gotoxy(22,11);
		printf("Missing decisions_texts.mnpl file");
		gotoxy(30,12);
		printf("Game cant start!");

		pak(0);
	}
	if (loaded[2]==0) //Missing the decision_values file
	{
		clrscr();
		MsgBox(2,1);
		setColor(lightred,black);
		gotoxy(22,11);
		printf("Missing decisions_values.mnpl file");
		gotoxy(30,12);
		printf("Game cant start!");

		pak(0);
	}
	if (loaded[3]==0) //Missing the options file
	{
		clrscr();
		MsgBox(2,1);
		setColor(lightred,black);
		gotoxy(25,11);
		printf("Missing the options.ini file");
		gotoxy(30,12);
		printf("Game cant start!");

		pak(0);
	}

	if (loaded[0]==1 && loaded[1]==1 && loaded[2]==1 && loaded[3]==1)
		return 1;
	else
		return 0;

}
int LoadPlots()
{
    int i;

	FILE *pl;
	pl=fopen("data\\plots.mnpl","r");
	
	if (pl!=NULL)
	{
		i=1;
		while (i<=19)
		{
			if(i%5!=0) //Cause plots with number 5,10,15 are decisions and free parking
			{
				fscanf(pl,"%s%s%d%d",Plot[i].Name,Plot[i].Domain,&Plot[i].Value,&Plot[i].Rent);
				i++;
			}
			else
				i++;
		}
		fclose(pl);
	}
	else
		return 0;
	
	return 1;
}
int LoadDecisionTexts()
{
	int i=0;

	FILE *dt;
	dt=fopen("data\\decision_texts.mnpl","r");

	if (dt!=NULL)
	{
		while (i<=15)
		{
			fgets(Decision[i].Text1,26,dt); //Read Text1
			fgets(Decision[i].Text2,26,dt); //Read Text2
			i++;	
		}
		fclose(dt);
	}
	else
		return 0;

	return 1;
}
int LoadDecisionValues()
{
	int i=0;

	FILE *dv;
	dv=fopen("data\\decision_values.mnpl","r");

	if (dv!=NULL)
	{
		while (i<=15)
		{
			fscanf(dv,"%d %d",&Decision[i].Money,&Decision[i].Action); //Read Money and Action
			i++;
		}
		fclose(dv);
	}
	else
		return 0;

	return 1;
}
int LoadOptions()
{
	FILE *opt;
	opt=fopen("data\\options.ini","r");

	if (opt!=NULL)
	{
		fscanf(opt,"%d%d",&Options.Starting_cash,&Options.Cash_from_start);
		fclose(opt);
	}
	else
		return 0;

	return 1;
}
int LoadGame()
{
	FILE *lg;
	lg=fopen("data\\save.mnpl","rb");

	if (lg!=NULL)
	{
		fread(Plot,sizeof(struct Plots),20,lg); //Read the plots properties
		fread(Player,sizeof(struct Human),16,lg); //Read the player stats
		fread(&Game_Save,sizeof(struct SaveLoad),1,lg); //Read the whose player turn is
		fclose(lg);
	}
	else
		return 0;

	return 1;
}
void SaveGame()
{
	FILE *sg;
	sg=fopen("data\\save.mnpl","wb");

	fwrite(Plot,sizeof(struct Plots),20,sg); //Write the plots properties
	fwrite(Player,sizeof(struct Human),16,sg); //Write the player stats
	fwrite(&Game_Save,sizeof(struct SaveLoad),1,sg); //Write the whose player turn is
	fclose(sg);
}
void SaveOptions()
{
	FILE *sopt;
	sopt=fopen("data\\options.ini","w");

	fprintf(sopt,"%d %d",Options.Starting_cash,Options.Cash_from_start);
	fclose(sopt);
}

/*	  
	 ******************
     * Other Funcions *
	 ******************
*/
void Window()
{
    int i,c;
     
    setColor(lightred,black);
    for (i=0;i<=79;i++)
    {
        gotoxy(i,33);
        printf("%c",196);
    }
    setColor(gray,black);
    gotoxy(28,32);
    printf("Press SPACE to continue");
    
    setColor(lightcyan,black);
    gotoxy(16,0);
    printf("Lengthen the window until you see the red line");
    setColor(lightgreen,black);
    
    do
    {
        c=getch();
    }
    while (c!=32);
}    
void MsgBox(int number,int sbl)
{
	int i,maxy=11+number*(sbl+1); //maxy: the hight of the box, generated by the number of choices and the space between them
	
	clrscr();
	setColor(lightcyan,black);
	gotoxy(21,8);
	for (i=0;i<13;i++)
		printf("%c",178);
	printf(" MONOPOLY ");
	for (i=0;i<13;i++)
		printf("%c",178);
	for (i=9;i<maxy;i++)
	{
		gotoxy(21,i);
		printf("%c                                  %c",178,178,178,178,178,178);
	}
	gotoxy(21,maxy);
	for (i=0;i<36;i++)
		printf("%c",178);

	setColor(white,black);
	gotoxy(0,33);
	printf(" Use the %c%c%c%c to move, ENTER to select, ESC to go back",24,26,25,27);
}
void endLine()
{
	gotoxy(79,33);
}
void CalcPawnCoords(int i,int Coords[])
{
	int row,col,pos;

	pos=Player[i].Position;

	if (pos!=0 && pos!=5 && pos!=10 && pos!=15) //If pawn is on a plot
	{			
		//Calculate the row and the column of the pawn position for plots
		if (i<5)  
		{
			col=i;
			row=0;
		}
		else
		if (i<10)
		{
			col=i-5;
			row=1;
		}
		else
		if (i<15)
		{
			col=i-10;
			row=2;
		}
		if (i!=15)
		{
			//Calculate pawns coordinates based on the column, row and position for plots
			if (pos<=4)
			{
				Coords[0]=7*(pos-1)+13+col;
				Coords[1]=1+row;
			}			
			else
			if (pos<=9)
			{
				Coords[0]=46+col;
				Coords[1]=5*(pos-6)+8+row;
			}
			else
			if (pos<=14)
			{
				Coords[0]=34+col-7*(pos-11);
				Coords[1]=30+row;
			}
			else
			if (pos<=19)
			{
				Coords[0]=1+col;
				Coords[1]=23+row-5*(pos-16);
			}
		}
		else  //Because the 16th pawn does not fit in the plots, i put it outside
		{
			if (pos<=4)
			{
				Coords[0]=7*(pos-1)+15;
				Coords[1]=7;
			}			
			else
			if (pos<=9)
			{
				Coords[0]=39;
				Coords[1]=5*(pos-6)+9;
			}
			else
			if (pos<=14)
			{
				Coords[0]=36-7*(pos-11);
				Coords[1]=26;
			}
			else
			if (pos<=19)
			{
				Coords[0]=12;
				Coords[1]=24-5*(pos-16);
			}
		}		
	}
	else  //If pawn is on start, desicion or free parking
	{
		//Calculate the row and the column of the pawn position for non-plot blocks
		if (i<4)  
		{
			col=i;
			row=0;
		}
		else
		if (i<8)
		{
			col=i-4;
			row=1;
		}
		else
		if (i<12)
		{
			col=i-8;
			row=2;
		}
		else
		if (i<16)
		{
			col=i-12;
			row=3;
		}

		//Calculate pawns coordinates based on the column, row and position for non-plot blocks
		if (pos==0)
		{
			Coords[0]=2+2*col;
			Coords[1]=2+row;
		}
		if (pos==5)
		{
			Coords[0]=42+2*col;
			Coords[1]=2+row;
		}
		if (pos==10)
		{
			Coords[0]=42+2*col;
			Coords[1]=29+row;
		}
		if (pos==15)
		{
			Coords[0]=2+2*col;
			Coords[1]=29+row;
		}
	}
}
void clriob()
{
	int y;

	for (y=8;y<=25;y++)
	{
		gotoxy(13,y);
		printf("                          ");
	}
}
void clrstats()
{
	int x,y;
	for (x=53;x<=78;x++)
		for (y=0;y<=33;y++)
		{
			gotoxy(x,y);
			printf(" ");
		}
}
void clrscreen()
{
	int i;
	
	setColor(lightwhite,black);
	for (i=0;i<=33;i++)
	{
		gotoxy(0,i);
		printf("                                                                              ");
	}
}
void pak(int position)
{
	setColor(gray,black);

	if (position==0)
	{
		gotoxy(26,14);
		printf("Press any key to continue");
		endLine();
		getch();
	}
	else
	{
		gotoxy(13,25);
		printf("Press any key to continue");
		endLine();
		getch();
	}
}
void Highlight(int plot,int highlight)
{ 
	int i=plot;

	if (highlight)
		setColor(lightmagenta,black);
	else
		setColor(lightwhite,black);

     if (i<=4)
	 {
		 i=i-1;
         ShowPlot(3,12+i*7,0);
	 }
	 else
     if (i<=9)
	 {
		 i=i-6;
         ShowPlot(4,40,7+i*5);
	 }
	 else
     if (i<=14)
	 {
		 i=i-11;
         ShowPlot(1,33-i*7,27);
	 }
	 else
	 {
		 i=i-16;
         ShowPlot(2,0,22-i*5);
	 }
}
void NotAllColors()
{
	clriob();
	setColor(lightred,black);
	gotoxy(19,14);
	printf("You dont have");
	gotoxy(18,15);
	printf("all the colored");
	gotoxy(18,16);
	printf("cards to built a");
	gotoxy(23,17);
	printf("house");

	pak(1);
}
