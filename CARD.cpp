//CELLULAR_AUTOMATON_REACTION_DIFFUSION_advanced computer science for physics
/*
	l'idea è quella di scrivere un automa cellulare che possa replicare gli stessi
	risultati di un sistema complesso regolato da equazioni di diffusione e di rea-
	zione. Le celle sono del tipo: inert_cells, free_particle_A, free_particle_B, source_B, source_A.
 */

#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

const int CARDwidth = 800;
const int CARDheight = 800;
//const int KERNELwidth = CARDwidth/10;
//const int KERNELheight = CARDheight/10;
const int SCREEN_width = CARDwidth;
const int CARDinfo = 10; //CARD data in the last 10 rows
const int SCREEN_HEIGHT = CARDheight + CARDinfo;
const int CARD_WIDTH  = CARDwidth + 2;  //  configuration width + frame of insensitive (to the transition function) cells
const int CARD_HEIGHT = CARDheight + 2; // configuration heigth + frame of insensitive (to the transition function) cells

//int ker0[KERNELwidth][KERNELheight];
//int ker1[KERNELwidth][KERNELheight];
struct CA
{
   int type;
   int health;
};

CA conf0[CARD_WIDTH][CARD_HEIGHT]; //conf at cronon t current
CA conf1[CARD_WIDTH][CARD_HEIGHT]; //conf at cronon t+1 next

BITMAP *buffer; //buffer declaration

int CARDcells = CARDwidth*CARDheight;
int sources;
int step = 0;
int numStep = 2512;
int perc_tipeA;
int perc_tipeB;
const int vita = 30;
const int foci = 20;
//colori di allegro
int black;      // = makecol(0,0,0);
int lightblue;  // = makecol(0,128,255);
int white;      // = makecol(255,255,255);
int darkgreen;  // = makecol(0, 128, 50);
int lightgreen; // = makecol( 0, 255, 128);
int red;        // = makecol(255,0,0);
int yellow;     // = makecol(255, 255, 0);
int grey;       // = makecol(150, 150, 150);

//funzione che copia la configurazione al tempo zero
void copy1_0 ()
{ for (int i=0; i<CARD_WIDTH; i++)
	for (int j=0; j<CARD_HEIGHT; j++){
		conf0[i][j].type = conf1[i][j].type ;
    conf0[i][j].health = conf1[i][j].health ;
  }
}

void initialize ()
{
   //insert input data: percentuale iniziale di particelle di tipo A e
   //numero di sorgenti di particelle di tipo B
   cout << "Type A particles percentage: ";
   cin >> perc_tipeA;
   cout << "Number of Type B aggregation points: ";
   cin >> sources;

   //pseudostochastic inizialization

   int c, height, width;
   srand(time(0));
   for (int i = 1; i<CARDheight; i++)
      for (int j = 1; j<CARDwidth; j++)
      {
         c = rand()%1000;
         if (c > perc_tipeA)
            conf1[j][i].type = -1; //empty (celle inerti)
         else
            conf1[j][i].type = 1;

      }

      for (int i = 1; i<CARDheight; i++)
         for (int j = 1; j<CARDwidth; j++)
          conf1[i][j].health = vita;
   

   for (int i=0; i<sources; i++)
      conf1[(rand()%CARDwidth)+1][(rand()%CARDheight)+1].type = 2;



      for (int i=0; i<foci; i++)
         conf1[(rand()%CARDwidth)+1][(rand()%CARDheight)+1].type = 5;

   
   width = CARDwidth -1;
   height = CARDheight - 1;
	for (int k=0; k<CARD_WIDTH; k++)
     {
      conf1[k][0].type = -1 ;
      conf1[k][height].type = -1;
     }
   for (int k=0; k<CARD_HEIGHT; k++)
     {
      conf1[0][k].type = -1;
      conf1[width][k].type = -1;
     }

	copy1_0();

}

// bool particle_B(int x, int y)
bool source_B(int x, int y)
{
   //if((conf0[x][y].type==2)||(conf0[x+1][y].type==2)||(conf0[x][y+1].type==2)||(conf0[x+1][y+1].type==2))
   if((conf0[x - 1][y].type == 2 || conf0[x + 1][y].type == 2 ||
                    conf0[x][y - 1].type == 2 || conf0[x][y + 1].type == 2 ||
                    conf0[x - 1][y-1].type == 2 || conf0[x + 1][y+1].type == 2 ||
                    conf0[x+1][y - 1].type == 2 || conf0[x-1][y + 1].type == 2))
      return true;
   else
      return false;
}

bool source_A(int x, int y)
{
   //if((conf0[x][y].type==5)||(conf0[x+1][y].type==5)||(conf0[x][y+1].type==5)||(conf0[x+1][y+1].type==5))
   if((conf0[x - 1][y].type == 5 || conf0[x + 1][y].type == 5 ||
                    conf0[x][y - 1].type == 5 || conf0[x][y + 1].type == 5 ||
                    conf0[x - 1][y-1].type == 5 || conf0[x + 1][y+1].type == 5 ||
                    conf0[x+1][y - 1].type == 5 || conf0[x-1][y + 1].type == 5))
      return true;
   else
      return false;
}

bool is_there_B(int x, int y)
{
   if((conf0[x - 1][y].type == 3 || conf0[x + 1][y].type == 3 ||
                    conf0[x][y - 1].type == 3 || conf0[x][y + 1].type == 3 ||
                    conf0[x - 1][y-1].type == 3 || conf0[x + 1][y+1].type == 3 ||
                    conf0[x+1][y - 1].type == 3 || conf0[x-1][y + 1].type == 3))
      return true;
   else
      return false;
}

bool is_there_A(int x, int y)
{
   if((conf0[x - 1][y].type == 1 || conf0[x + 1][y].type == 1 ||
                    conf0[x][y - 1].type == 1 || conf0[x][y + 1].type == 1 ||
                    conf0[x - 1][y-1].type == 1 || conf0[x + 1][y+1].type == 1 ||
                    conf0[x+1][y - 1].type == 1 || conf0[x-1][y + 1].type == 1))
      return true;
   else
      return false;
}

bool is_valid_position(int x, int y, int max_x, int max_y)
{
   //check if position (x,y) is within bounds of the array
   return (x >= 0 && x < max_x && y >= 0 && y < max_y);
}

bool sfondo(int x, int y)
{
   //if((conf0[x][y].type==-1)||(conf0[x+1][y].type==-1)||(conf0[x][y+1].type==-1)||(conf0[x+1][y+1].type==-1))
   if((conf0[x - 1][y].type == -1 || conf0[x + 1][y].type == -1 ||
                    conf0[x][y - 1].type == -1 || conf0[x][y + 1].type == -1 ||
                    conf0[x - 1][y-1].type == -1 || conf0[x + 1][y+1].type == -1 ||
                    conf0[x+1][y - 1].type == -1 || conf0[x-1][y + 1].type == -1))
      return true;
   else
      return false;
}

void genera_B(int x, int y)
{
   //aggiunto ora
//   if (step % 3 == 0 && particle_B(x,y)) {
   if (step % 2 == 0 && source_B(x,y)) {
        // Genera le particelle di tipo B dalla sorgente

            conf1[x+2][y+2].type = 3; // Tipo B dalla sorgente
						conf1[x-2][y-2].type = 3;
						conf1[x-2][y+2].type = 3;
						conf1[x+2][y-2].type = 3;

            conf1[x+2][y+2].health = vita; // Tipo B dalla sorgente
						conf1[x-2][y-2].health = vita;
						conf1[x-2][y+2].health = vita;
						conf1[x+2][y-2].health = vita;
        }
    }

    void genera_A(int x, int y)
    {
       if (step % 2 == 0 && source_A(x,y)) {
            // Genera le particelle di tipo A dalla sorgente

                     conf1[x+2][y+2].type = 1; // Tipo A dalla sorgente
    						conf1[x-2][y-2].type = 1;
    						conf1[x-2][y+2].type = 1;
    						conf1[x+2][y-2].type = 1;
            }
        }


   void reproduce_A(int x, int y) {
		if (step % 30 == 0){
		 int directions[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 1}}; // Possible directions: right, left, down, up

     int availableSpots[8][2] = {0} ; // Array to store available spots
     int availableCount = 0; // Counter for available spots

     // Check adjacent spots for availability
     for (int i = 0; i < 8; i++) {
         int newX = x + directions[i][0];
         int newY = y + directions[i][1];

         // Check bounds and spot availability
         if (newX > 0 && newX < CARDwidth - 1 && newY > 0 && newY < CARDheight - 1 &&
             conf0[newX][newY].type == -1) {
				 //if (newX > 0  && newY > 0  && conf0[newX][newY] == -1) {
             availableSpots[availableCount][0] = newX;
             availableSpots[availableCount][1] = newY;
             availableCount++;
         }
     }

     // If there are available spots, generate a new particle A in a random direction
     if (availableCount > 0) {
         int randomIndex = rand() % availableCount;
         int X_2 = availableSpots[randomIndex][0];
         int Y_2 = availableSpots[randomIndex][1];

         conf1[X_2][Y_2].type = 1; // Generate a new particle A in a random direction
     }
}
}
//}

int width = CARDwidth - 1;
int height = CARDheight - 1;


void starving_B(int x, int y) {
    // Type 3 particles (B particles) lose energy each step
    int riga = 0;
    int colonna = 0;
    int somma = 0;

    if (conf0[x][y].type == 3 && conf0[x][y].health > 0) {
      if(is_there_A(x,y))
      {
        for(int i = -1; i < 2; i++)
          for(int j=-1; j<2; j++)
          {
            if(conf1[x+i][y+j].type == 1)
          {
            riga = x + i;
            colonna = y + j;
            break;
        }
      }
      for(int i=riga - 1; i < riga + 2; i++)
      for(int j=colonna - 1; j < colonna + 2 ; j++)
      {
        if(conf1[i][j].type == 3)
        somma += conf1[i][j].type;
      }
      }
      if(somma <= 3)
        conf1[x][y].health --;
    }

    if (conf0[x][y].type == 3 && conf0[x][y].health <= 0) {  // Check conf1 for health status
        //conf1[x][y].health = 5;
        conf1[x][y].type = -1; // Remove type 3 particle
    }
}

// void reazione_cubica(int x, int y)
// {
   //2B+1A -> 3B
void reazione_cubica() {
    for (int y = 1; y < CARDheight - 1; y++) {
        for (int x = 1; x < CARDwidth - 1; x++) {
            //if (conf0[x][y] == 1) {  // If the current cell is a type A particle
            if (conf1[x][y].type == 1) {  // Skip inert cells and check for type A
                // Check neighboring cell product looking for at least 2 type B particles
                //---------------------------------------------------------------------
                if (conf0[x - 1][y].type == 3 || conf0[x + 1][y].type == 3 ||
                    conf0[x][y - 1].type == 3 || conf0[x][y + 1].type == 3 ||
                    conf0[x - 1][y-1].type == 3 || conf0[x + 1][y+1].type == 3 ||
                    conf0[x+1][y - 1].type == 3 || conf0[x-1][y + 1].type == 3) {
                    // Convert type A particle to type B

               int somma = 0;
               for(int i=-1; i < 2;i++)
               {
                  for (int j = -1; j <2; j++)
                  {
                     somma += conf1[x+i][y+j].type;
                  }
               }
               if (somma >= 0)
                  conf1[x][y].type = 3;
                  conf1[x][y].health = vita;
            }
         }
        }
    }
}
//}

void cell_diff (int x, int y){ 
      bool clockwise=rand()%2 ;
      if (!(((conf0[x][y].type==0)||(conf0[x+1][y].type==0)||(conf0[x][y+1].type==0)||(conf0[x+1][y+1].type==0)) 
         ||(conf0[x][y].type == 2 || conf0[x][y].type == 5 || source_B(x, y) && (sfondo(x, y)) || source_A(x,y))))
      {
         if (clockwise)
         {
            conf1[x][y].type = conf0[x][y+1].type;
            conf1[x][y+1].type = conf0[x+1][y+1].type;
            conf1[x+1][y+1].type = conf0[x+1][y].type;
            conf1[x+1][y].type = conf0[x][y].type;

            conf1[x][y].health = conf0[x][y+1].health;
            conf1[x][y+1].health = conf0[x+1][y+1].health;
            conf1[x+1][y+1].health = conf0[x+1][y].health;
            conf1[x+1][y].health = conf0[x][y].health;
         }
         else
         {
         conf1[x][y].type = conf0[x+1][y].type;
         conf1[x+1][y].type = conf0[x+1][y+1].type;
         conf1[x+1][y+1].type = conf0[x][y+1].type;
         conf1[x][y+1].type = conf0[x][y].type;

         conf1[x][y].health = conf0[x+1][y].health;
         conf1[x+1][y].health = conf0[x+1][y+1].health;
         conf1[x+1][y+1].health = conf0[x][y+1].health;
         conf1[x][y+1].health = conf0[x][y].health;
         }
      }
   }
//}
void diffusion1 ()  // even blocks
{ 	int height = CARDheight-1;
	int width = CARDwidth-1;
	for (int i=1; i<width; i+=2)
		for (int j=1; j<height; j+=2)
	  		cell_diff (i,j);
}

void diffusion2 () // odd blocks
{int height = CARDheight-2;
 int width = CARDwidth - 2;
 for (int i=2; i<width; i+=2)
	for (int j=2; j<height; j+=2)
	  cell_diff (i,j);
}

void transition ()    //not for frame cells
{ if (step%2)
   diffusion1();
  else
   diffusion2();
  copy1_0();

int height = CARDheight-2;
int width = CARDwidth - 2;

 for (int i=2; i<width; i+=2){
 	for (int j=2; j<height; j+=2){
		genera_B (i,j);
    genera_A (i,j);
  }
}
	reazione_cubica();

	//Generate a new particle A from existing particle A (if possible)

	    for (int y = 1; y < CARDheight - 1; y++) {
	        for (int x = 1; x < CARDwidth - 1; x++) {
            int z = rand()%3;
	            if (conf0[x][y].type == 1 && z == 1) {
	                reproduce_A(x, y);
	            }
            starving_B(x, y);
	        }
	    }

  copy1_0();
}

int main()
 {// inizialization;
  char percent = '%';
  int diffB_sum, diffA_sum;
  initialize ();
  allegro_init();
  install_keyboard();
  install_mouse();
  set_gfx_mode( GFX_SAFE, SCREEN_width, SCREEN_HEIGHT, 0, 0);
     //choice of graphic representation
  show_mouse(screen);
     //show mouse on the screen
  buffer = create_bitmap(SCREEN_width, SCREEN_HEIGHT);
     // buffer
  lightblue    = makecol(0, 128, 255);
  black   = makecol(0, 0, 0);
  white = makecol(255,255,255);
  darkgreen  = makecol( 0, 128, 50);
  lightgreen = makecol (0, 255, 128);
  red = makecol (255, 0, 0);
  yellow = makecol(255, 255, 0);
  grey = makecol(150, 150, 150);
  ofstream myfile;
  myfile.open ("data.txt", ios::out | ios::app | ios::binary);
  myfile << "# Perc. typeA (t=0) = " << perc_tipeA <<" \t" << "Number of B sources = " << sources << endl ;
  myfile << "# Number of A foci = " << foci << " \t" << "Matrix dimension = 800x800" << endl;
  myfile << "# MaxHealth of B part. = " << vita << " \t" << "Reproduction step of A = 30" << endl;
  myfile << "# Step:" << " \t" << "Number of part. A: " << " \t" << "Number of part. B: " << endl;

// for (int step = 0; step < numStep; step++)
//{

  while (!key[KEY_ESC])
    {
     //for (int step = 0; step < numStep; step++){ 
     transition();
     myfile << step << " \t" << diffA_sum << " \t" << diffB_sum  << endl;
     step++;
     
     diffB_sum = 0;
     diffA_sum = 0;
     for (int y=0; y<CARDheight; y++)
	     for (int x=0; x<CARDwidth; x++)
	         switch(conf0[x][y].type)
               { 	
				      case -1: 
                     putpixel(buffer, x, y, grey); break;
                  case 2: 
                     putpixel(buffer, x, y, yellow); diffB_sum++; break;
	               case 1: 
                     putpixel(buffer, x, y, lightgreen); diffA_sum++; break; 
                  case 3: 
                     putpixel(buffer, x, y, red); diffB_sum++; break;
                  case 5: 
                     putpixel(buffer, x, y, darkgreen); diffB_sum++; break;
               }
   textprintf_ex(buffer,font,0,CARDheight,white,black, "Partic=%i",perc_tipeA);//print the initial percentage of free particles
   textprintf_ex(buffer,font,70,CARDheight,white,black, "%c", percent); // print the character '%'
	textprintf_ex(buffer,font,80,CARDheight,white,black, "-aggrPoints=%i", sources);//print the initial number of aggregation points
   textprintf_ex(buffer,font,190,CARDheight,white,black, ">step:%i",step); //print the number of CARD
   textprintf_ex(buffer,font,290,CARDheight,white,black, "                             "); // cancel the previous text
   textprintf_ex(buffer,font,290,CARDheight,white,black, ">freePA:%i",diffA_sum);// number of  free particles
   textprintf_ex(buffer,font,390,CARDheight,white,black, ">freePB:%i",diffB_sum);// number of aggregation points
   blit(buffer,screen,0,0,0,0,SCREEN_width,SCREEN_HEIGHT);
     //stampa (disegna)sullo schermo il buffer d'uscita
   vsync();
   rest(16);


   if (key[KEY_DOWN]) while (!(key[KEY_UP])); //keyDOWN:stop, keyUP:recover
   }
	return 0;
	 }
 //}
END_OF_MAIN()
