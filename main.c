#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <unistd.h>
#include <windows.h>
#define t 25 // tamanio del mapa
#define t_in 4
#define PUNTAJES "./puntajes.txt"
#define RESET_COLOR ""
#define VERDE_T ""
#define ROJO_T ""
#define AMARILLO_T ""
#define BLANCO_T ""

// estructura del cuerpo y cabeza
struct cuerpo
{
	int x, y;
};

// estructura de la fruta
struct fruta
{
	int x, y;
};

struct node
{
	int puntaje;
	char nombre[20];
	struct node *siguiente;
};

struct tipo_fruta
{
	char tipo;
	struct tipo_fruta *siguiente;
};


void imprimir(char espacio[t][t], int *puntaje, struct fruta *manzana, struct tipo_fruta **front, struct tipo_fruta **back);
void generar(char espacio[t][t], struct fruta *manzana);
void spawn(char espacio[t][t], struct cuerpo *snake);
char direccion(char espacio[t][t], struct cuerpo *snake, char letra, int *muerto, char ult, int tam);
void guardar_puntaje(int puntaje_nuevo);
void ver_puntajes();
int menu(char *letra);
void comer(struct fruta *manzana, struct cuerpo *snake, int *puntaje, int *tam, char letra, char espacio[t][t], char ult, struct tipo_fruta **front, struct tipo_fruta **back);
char movimiento(char *letra, int *auxx, int *auxy, int *auxx2, int *auxy2, struct cuerpo *snake, char espacio[t][t], struct fruta *manzana, int *tam, int *muerto, char ult, int *puntaje, struct tipo_fruta **front, struct tipo_fruta **back);
void push(struct node **head, char nuevo_nombre[20], int nuevo_puntaje);
void pop(struct node **head);
void crear_nodo(struct tipo_fruta **front, struct tipo_fruta **back, char caracter);
void borrar_nodo(struct tipo_fruta **front, struct tipo_fruta **back);

int main(int argc, char *argv[])
{
	srand(time(NULL));
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);
	
	struct cuerpo snake[((t - 2) * (t - 2)) - 1];
	struct fruta manzana;
	char letra = 'w', ult = 'w', espacio[t][t];
	int muerto = 0, puntaje = 0, auxx2 = 0, auxy2 = 0, opcion = 0;
	int auxx = 0, auxy = 0; // para modificar el tamanio de la snake
	int tam = t_in;			// tamanio inicial de la matriz
	struct tipo_fruta *front = NULL;
	struct tipo_fruta *back = NULL;
	
	for(int i=0;i<3;i++){
		int random = rand() % 3;
		switch(random){
		case 0:
			crear_nodo(&front,&back,'#');
			break;
		case 1:
			crear_nodo(&front,&back,')');
			break;
		case 2:
			crear_nodo(&front,&back,'%');
			break;
		}
	}
	
	// genero la matriz del mapa con los bordes y la manzana
	generar(espacio, &manzana);
	// creo la vivorita
	spawn(espacio, snake);
	// imprimo menu de inicio
	opcion = menu(&letra);
	
	if (opcion == 1)
	{
		letra = 'w';
		ult = 'w';
		// mientras el jugador no haya perdido puede jugar
		while (muerto == 0)
		{
			ult=movimiento(&letra,&auxx,&auxy,&auxx2,&auxy2,snake,espacio,&manzana,&tam,&muerto,ult,&puntaje,&front,&back);
			
			// muestra el frame
			imprimir(espacio, &puntaje, &manzana, &front, &back);
		}
		
		printf(ROJO_T "\n\n\n			GAME OVER\n\n\n\n" RESET_COLOR);
		
		cursorInfo.bVisible = TRUE;
		SetConsoleCursorInfo(hConsole, &cursorInfo);
		
		if(puntaje != 0){
			guardar_puntaje(puntaje);
		}
	}
	else if (opcion == 2)
	{
		ver_puntajes();
	}
	else
	{
		exit(0);
	}
	
	struct tipo_fruta *temp = front;
	
	do{
		temp = front;
		front = front->siguiente;
		free(temp);
	} while(front->siguiente!=NULL);
	
	return 0;
}

void imprimir(char espacio[t][t], int *puntaje, struct fruta *manzana, struct tipo_fruta **front, struct tipo_fruta **back)
{
	// recorre la matriz e imprime el caracter
	for (int i = 0; i < t; i++)
	{
		for (int j = 0; j < t; j++)
		{
			if (i == 0 || i == t - 1 || j == 0 || j == t - 1)
			{
				printf("%c ", espacio[i][j]);
			}
			else if (espacio[i][j] == '#')
			{
				printf(ROJO_T "%c " RESET_COLOR, espacio[i][j]);
			}
			else
			{
				printf(VERDE_T "%c " RESET_COLOR, espacio[i][j]);
			}
		}
		printf("\n");
	}
	
	printf("\n PUNTAJE: %d \t\t\t\t", *puntaje); // imprime el puntaje actual
	
	struct tipo_fruta *temp = *front;
	
	do{
		printf(" %c ", temp->tipo);
		temp = temp->siguiente;
	} while((*back)->siguiente != temp);

	//printf("\n x:%d y:%d", manzana->y, manzana->x);
	
	// deja una espera antes de refrescar
	system("cls");
}

void generar(char espacio[t][t], struct fruta *manzana)
{
	// Pongo el caracter para los bordes horizontales
	for (int i = 0; i < t; i++)
	{
		espacio[0][i] = '-';
		espacio[t - 1][i] = '-';
	}
	
	for (int i = 1; i < t - 1; i++)
	{
		// Pongo el caracter para los bordes verticales
		espacio[i][0] = '|';
		espacio[i][t - 1] = '|';
		
		// pongo el caracter vacio para todo el resto
		for (int j = 1; j < t - 1; j++)
		{
			espacio[i][j] = ' ';
		}
	}
	
	manzana->x = 1 + rand() % (t - 2);
	manzana->y = 1 + rand() % (t - 2);
	
	int random = rand() % 3;
	switch(random){
	case 0:
		espacio[manzana->x][manzana->y] = '#';
		break;
	case 1:
		espacio[manzana->x][manzana->y] = ')';
		break;
	case 2:
		espacio[manzana->x][manzana->y] = '%';
		break;
	}

}

void spawn(char espacio[t][t], struct cuerpo *snake)
{
	// establece la posicion de toda la vivorita
	for (int i = 0; i < t_in; i++)
	{
		snake[i].x = (t / 2) + i;
		snake[i].y = t / 2;
	}
	
	// posiciona las partes en la matriz
	for (int i = 0; i < t_in; i++)
	{
		if (i == 0)
		{
			espacio[snake[i].x][snake[i].y] = '0';
		}
		else
		{
			espacio[snake[i].x][snake[i].y] = 'x';
		}
	}
}

char direccion(char espacio[t][t], struct cuerpo *snake, char letra, int *muerto, char ult, int tam)
{
	// ve que tecla se toco
	switch (letra)
	{
	case 'w':
		snake[0].x--;
		ult = 'w';
		break;
	case 'a':
		snake[0].y--;
		ult = 'a';
		break;
	case 's':
		snake[0].x++;
		ult = 's';
		break;
	case 'd':
		snake[0].y++;
		ult = 'd';
		break;
		// si toca cualquier tecla no definida mueve la vivorita en la direccion anterior
	default:
		if (ult == 'w')
		{
			snake[0].x--;
		}
		if (ult == 'a')
		{
			snake[0].y--;
		}
		if (ult == 's')
		{
			snake[0].x++;
		}
		if (ult == 'd')
		{
			snake[0].y++;
		}
		break;
	}
	
	// pone la posicion de las partes en el mapa
	espacio[snake[0].x][snake[0].y] = '0';
	// analiza las colisiones entre la cabeza y el cuerpo
	for (int i = 1; i < tam; i++)
	{
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
		{
			*muerto = 1;
			break;
		}
	}
	// analiza las colisiones entre la cabeza y los bordes
	if (snake[0].x == 0 || snake[0].x == t - 1 || snake[0].y == 0 || snake[0].y == t - 1)
	{
		*muerto = 1;
	}
	
	return ult;
}

void guardar_puntaje(int puntaje_nuevo)
{
	struct node *head = NULL;
	char nombre_nuevo[20], nombre_leido[20];
	int ranking=0, puntaje_leido=0, puntaje_agregado=0;
	
	// crea el tipo de dato archivo y lo abre en modo append
	FILE *fpuntaje;
	fpuntaje = fopen(PUNTAJES, "r");
	
	// si no lo encuentra intenta crearlo
	if (NULL == fpuntaje)
	{
		printf("  No se encontro el archivo de puntajes, creandolo...\n");
		fpuntaje = fopen(PUNTAJES, "w");
		
		// si no puede sale del programa
		if(NULL == fpuntaje)
		{
			printf("  No se pudo crear el archivo...");
			exit(1);
		}else
		{
			// pide que ingrese su nombre para registrarlo
			printf("  Nuevo puntaje alto!\n  Ingrese su nombre: ");
			scanf(" %s", nombre_nuevo);
			printf("  Su puntaje fue: %d", puntaje_nuevo);
			fprintf(fpuntaje, "%s %d\n", nombre_nuevo, puntaje_nuevo);
			fclose(fpuntaje);
		}
	}else
	{
		// lee los puntajes guardados
		while(!feof(fpuntaje)){
			fscanf(fpuntaje,"%s %d\n", nombre_leido, &puntaje_leido);
			
			if(puntaje_nuevo < puntaje_leido)
			{
				push(&head, nombre_leido, puntaje_leido);
				ranking++;
			}else
			{
				if(0 == puntaje_agregado)
				{
					printf("  Nuevo puntaje alto!\n  Ingrese su nombre: ");
					scanf(" %s", nombre_nuevo);
					printf("  Su puntaje fue: %d", puntaje_nuevo);
					push(&head, nombre_nuevo, puntaje_nuevo);
					puntaje_agregado=1;
					ranking++;
				}
				push(&head, nombre_leido, puntaje_leido);
				ranking++;
			}
		}
		fclose(fpuntaje);
		
		if(puntaje_agregado == 0 && ranking < 10) //Si no esta llena la lista de puntajes
		{
			printf("  Nuevo puntaje alto!\n  Ingrese su nombre: ");
			scanf(" %s", nombre_nuevo);
			printf("  Su puntaje fue: %d", puntaje_nuevo);
			push(&head, nombre_nuevo, puntaje_nuevo);
			puntaje_agregado=1;
			ranking++;
		}
			
		// en caso de que haya sido mayor a alguno de los puntajes ya almacenados
		if(1 == puntaje_agregado)
		{
			if(ranking > 10) //elimina el menor puntaje
			{
				pop(&head);
				ranking--;
			}
			// lo guarda
			fpuntaje = fopen(PUNTAJES, "w");
			if(NULL == fpuntaje){
				printf("No se pudo abrir el archivo de puntajes");
				exit(1);
			}
			
			
			struct node *temp = head;
			for(int i=0; i<ranking; i++){
				fprintf(fpuntaje, "%s %d\n", temp->nombre, temp->puntaje);
				temp = temp->siguiente;
			}
			
			fclose(fpuntaje);
		}
	}
	
	struct node *temp = head;
	
	do{
		temp = head;
		head = head->siguiente;
		free(temp);
	} while(head->siguiente!=NULL);
}

void ver_puntajes()
{
	FILE *fpuntaje;
	char nombre[20];
	int puntaje = 0;
	fpuntaje = fopen(PUNTAJES, "r");
	
	// si no lo encuentra vuelve al menu de inicio
	if (NULL == fpuntaje)
	{
		printf("No hay puntajes guardados");
	}
	else
	{
		// lee la informacion y cierra el archivo
		while (!feof(fpuntaje))
		{
			fscanf(fpuntaje, "%s %d\n", nombre, &puntaje);
			//printf("%s: %d puntos \n", nombre, puntaje);
		}
		
		fclose(fpuntaje);
	}
}

int menu(char *letra)
{
	/*
	int cambio=0;
	
	system("cls");
	while (kbhit() != 1)
	{
	// imprime el menu
	printf( VERDE_T "\n\n                         ....              ...::...                                                 \n                     ..::.......       ..:,::,,,,,::.                                               \n                ....::c;,::::,,;,::::,;;,,,:odxdlc:;::....                                          \n            ..::::::,:..::.....::..:;;,.,d00k0XKOkdd:.,;:.:::..                                     \n            :::;,,,,,;;;;:c;:......:;..:ONO;.c0XXK0Ol..,...::,:....                                 \n           .,cl:,;:.......::::,.......,dKXl..:OXXXX0l:..........:,,:.....                           \n           .,cc:;,.......,;;;,,.......,o0d...dKX0kl::..............:,,:::...                       \n              .::::,......,;,;..::.......:o:::xko;....::.::::........::..,;,:..                     \n                .,lc;;;,:.,;;;....... ............::::,;;;,:..:........:....,;:..                   \n                 .lc...:cll;;c;,,:............:;;:c::;::....::...............:,;:.                  \n                 .:l.   ;xl:,;.:,,,,,;;;;;;;:ll:,,,,:....,,:::..................;,.                 \n                  :l.   ,xc:;:.....;ccccllc::;........,;;::cll:::,..............:::.                \n                  .:,   ,o,:;...::lodool:;:.....,,,,,:oddxkkkddxkxol,............;;.                \n                  .:;   :;.;;.::odollc;......:,,,:....:ldxxxkkkkOOkOx;...........,;.                \n                 ..,.. .:.::;:oolll:,...:;,:::..  .    .,xOkkkkkkxxkkd;....:.....:,.                \n                 ...   ::.:llocllc,..,,,,...   .         :lxkkkxxxxxddd;.........;;.                \n                      .:.cdolccc;,,:::.                   .lOOOOOkdooodc:.......::,.                \n   ...              .:,,;clllc:,::..                       :dkxddddoodxo,.......;:.                 \n   ..:.         ...:;;,:. .....                            .:xxxxxxdoooc........:.                  \n    ..:.......:,;:;,:..                                    .:kxdddddddo,.......;;.                  \n      ..:,,;;,,::...                                       .lxdddooloo:.......;;.                   \n                                                          .;kxdoooooo:.......;,.                    \n                                                         .;okkxddool,......,;,..                    \n                                                       .,lxxxdoolc;.......,;..;. .                  \n                                                   ..::ldxxdoooc;........,,. .;;:..     .           \n                                              ..,;:coddoodocc;.........:,:. . :lc,:....  .          \n                                        ..:,;:lllllllllc;;;:.........:,:.     ..cc:,::.....         \n                                   ..:,;cloooolc::::;:............,,,:.         ..;ll;...:::..      \n                              ..:;;;:loollcc:;,:................;:,..              .cd;....:;,.     \n                  .........:;::cllc::cccc:,..........:.......:cc:,..::::::.:::::::::,,......;l,.    \n               ....:......:;,::,,,,;:;;,:............:::..::,;,:::::...........:...........::oc.    \n             ..,:............  .........  ...............  .............................,,;c:c;.    \n            .:,.................... .........   .. ................................:;;,:d.:;oc.     \n           .,cc;,;::,:,:..:::.........................................::........;:;;lo:,col:;.      \n           .cdo:lxxc:dxl,;llc;:::;:,c:;:.,::;;,,;::;,.:,,,::;,:,,,,:,:cc;:;cc:,,coc,cdl:cc,..       \n           .cdc:dxocokkl;ldo:cxdoc;oxoc,:doll:,cdool;::lcc;,cc;:llc;:lll:,:olc;::ol:loc,..          \n            .ccldxdcokxl:ddlcokdl::ddoc;lxdoo;;ddool,,lolc;:lc;cooc,;lllc,;oooc;ldl:,..             \n             ..:;:c:lxxolxkxodkxdcckkxocoxxxd:cxxdxo:cdddc:lol:looc:ldddo:cllc;::..                 \n                       :::,;:;:ccc:;ccc:;:lllc::llcc:;;cc:;,;:;,,,,::,,,::                          \n" RESET_COLOR );
	if(cambio==0){
	printf("\n\n			   	 PULSE" ROJO_T " CUALQUIER TECLA " RESET_COLOR "PARA COMENZAR");
	printf("\n				      PULSE" AMARILLO_T " P " RESET_COLOR "PARA VER PUNTAJES");
	
}
	
	sleep(1);
	// invierte el valor de cambio
	cambio=~cambio;
	// espera un cierto tiempo y refresca la pantalla
	
	system("cls");
}
	system("cls");
	*/
	int opcion = 1;
	while (*letra != 'e')
	{
		system("cls");
		// imprime el menu
		printf(VERDE_T "\n                         ....              ...::...                                                 \n                     ..::.......       ..:,::,,,,,::.                                               \n                ....::c;,::::,,;,::::,;;,,," BLANCO_T ":odxdlc:" VERDE_T ";::....                                          \n            ..::::::,:..::.....::..:;;,." BLANCO_T ",d00k0XKOkdd:" VERDE_T ".,;:.:::..                                     \n            :::;,,,,,;;;;:c;:......:;.." BLANCO_T ":ONO" AMARILLO_T ";." BLANCO_T "c0XXK0Ol" VERDE_T "..,...::,:....                                 \n           .,cl:,;:.......::::,.......," BLANCO_T "dKXl" AMARILLO_T "..:" BLANCO_T "OXXXX0l" VERDE_T ":..........:,,:.....                           \n           .,cc:;,.......,;;;,,.......," BLANCO_T "o0d" AMARILLO_T "..." BLANCO_T "dKX0kl" VERDE_T "::..............:,,:::...                       \n              .::::,......,;,;..::......" BLANCO_T ".ooxkoxko;" VERDE_T "....::.::::........::..,;,:..                     \n                .,lc;;;,:.,;;;....... ............::::,;;;,:..:........:....,;:..                   \n                 .lc...:cll;;c;,,:............:;;:c::;::....::...............:,;:.                  \n                 .:l.   ;xl:,;.:,,,,,;;;;;;;:ll:,,,,:....,,:::..................;,.                 \n                  :l.   ,xc:;:.....;ccccllc::;........,;;::cll:::,..............:::.                \n                  .:,   ,o,:;...::lodool:;:.....,,,,,:oddxkkkddxkxol,............;;.                \n                  .:;   :;.;;.::odollc;......:,,,:....:ldxxxkkkkOOkOx;...........,;.                \n                 ..,.. .:.::;:oolll:,...:;,:::..  .    .,xOkkkkkkxxkkd;....:.....:,.                \n                 ...   ::.:llocllc,..,,,,...   .         :lxkkkxxxxxddd;.........;;.                \n                      .:.cdolccc;,,:::.                   .lOOOOOkdooodc:.......::,.                \n   ...              .:,,;clllc:,::..                       :dkxddddoodxo,.......;:.                 \n   ..:.         ...:;;,:. .....                            .:xxxxxxdoooc........:.                  \n    ..:.......:,;:;,:..                                    .:kxdddddddo,.......;;.                  \n      ..:,,;;,,::...                                       .lxdddooloo:.......;;.                   \n                                                          .;kxdoooooo:.......;,.                    \n                                                         .;okkxddool,......,;,..                    \n                                                       .,lxxxdoolc;.......,;..;. .                  \n                                                   ..::ldxxdoooc;........,,. .;;:..     .           \n                                              ..,;:coddoodocc;.........:,:. . :lc,:....  .          \n                                        ..:,;:lllllllllc;;;:.........:,:.     ..cc:,::.....         \n                                   ..:,;cloooolc::::;:............,,,:.         ..;ll;...:::..      \n                              ..:;;;:loollcc:;,:................;:,..              .cd;....:;,.     \n                  .........:;::cllc::cccc:,..........:.......:cc:,..::::::.:::::::::,,......;l,.    \n               ....:......:;,::,,,,;:;;,:............:::..::,;,:::::...........:...........::oc.    \n             ..,:............  .........  ...............  .............................,,;c:c;.    \n            .:,.................... .........   .. ................................:;;,:d.:;oc.     \n           .,cc;,;::,:,:..:::.........................................::........;:;;lo:,col:;.      \n           .cdo:lxxc:dxl,;llc;:::;:,c:;:.,::;;,,;::;,.:,,,::;,:,,,,:,:cc;:;cc:,,coc,cdl:cc,..       \n           .cdc:dxocokkl;ldo:cxdoc;oxoc,:doll:,cdool;::lcc;,cc;:llc;:lll:,:olc;::ol:loc,..          \n            .ccldxdcokxl:ddlcokdl::ddoc;lxdoo;;ddool,,lolc;:lc;cooc,;lllc,;oooc;ldl:,..             \n             ..:;:c:lxxolxkxodkxdcckkxocoxxxd:cxxdxo:cdddc:lol:looc:ldddo:cllc;::..                 \n                       :::,;:;:ccc:;ccc:;:lllc::llcc:;;cc:;,;:;,,,,::,,,::                          \n" RESET_COLOR);
		// printf("\n\n			   	 PULSE" /*ROJO_T*/ " CUALQUIER TECLA " /*RESET_COLOR*/ "PARA COMENZAR");
		// printf("\n				      PULSE" /*AMARILLO_T*/ " P " /*RESET_COLOR*/ "PARA VER PUNTAJES");
		if (opcion == 1)
		{
			printf("\n				     --------- ");
			printf("\n				     | jugar | puntaje   salir");
			printf("\n				     --------- ");
		}
		else if (opcion == 2)
		{
			printf("\n				             ----------- ");
			printf("\n				       jugar | puntaje | salir");
			printf("\n				             ----------- ");
		}
		else
		{
			printf("\n				                       --------- ");
			printf("\n				       jugar   puntaje | salir |");
			printf("\n				                       --------- ");
		}
		while (kbhit() != 1)
		{
		}
		*letra = getch();
		if (*letra == 'd')
		{
			if (opcion < 3)
			{
				opcion++;
			}
			else
			{
				opcion -= 2;
			}
		}
		else if (*letra == 'a')
		{
			if (opcion > 1)
			{
				opcion--;
			}
			else
			{
				opcion += 2;
			}
		}
		system("cls");
	}
	return opcion;
}

void comer(struct fruta *manzana, struct cuerpo *snake, int *puntaje, int *tam, char letra, char espacio[t][t], char ult, struct tipo_fruta **front, struct tipo_fruta **back)
{
	
	if (manzana->x == snake[0].x && manzana->y == snake[0].y)
	{
		*tam += 1;
		*puntaje += 100;
		// muevo la vivorita si toca una tecla adecuada
		switch (letra)
		{
		case 'w':
			snake[*tam - 1].x = snake[*tam - 2].x + 1;
			snake[*tam - 1].y = snake[*tam - 2].y;
			break;
		case 'a':
			snake[*tam - 1].x = snake[*tam - 2].x;
			snake[*tam - 1].y = snake[*tam - 2].y + 1;
			break;
		case 's':
			snake[*tam - 1].x = snake[*tam - 2].x - 1;
			snake[*tam - 1].y = snake[*tam - 2].y;
			break;
		case 'd':
			snake[*tam - 1].x = snake[*tam - 2].x;
			snake[*tam - 1].y = snake[*tam - 2].y - 1;
			break;
		default:
			switch (ult)
			{
			case 'w':
				snake[*tam - 1].x = snake[*tam - 2].x + 1;
				snake[*tam - 1].y = snake[*tam - 2].y;
				break;
			case 'a':
				snake[*tam - 1].x = snake[*tam - 2].x;
				snake[*tam - 1].y = snake[*tam - 2].y + 1;
				break;
			case 's':
				snake[*tam - 1].x = snake[*tam - 2].x - 1;
				snake[*tam - 1].y = snake[*tam - 2].y;
				break;
			case 'd':
				snake[*tam - 1].x = snake[*tam - 2].x;
				snake[*tam - 1].y = snake[*tam - 2].y - 1;
				break;
			}
			break;
		}
		
		int dif = 0;
		// agrega una manzana en una posicion libre dentro de los margenes
		while (dif == 0)
		{
			dif = 1;
			manzana->x = 1 + rand() % (t - 2);
			manzana->y = 1 + rand() % (t - 2);
			for (int j = 0; j < (*tam - 1); j++)
			{
				if (manzana->x == snake[j].x && manzana->y == snake[j].y)
				{
					dif = 0;
					break;
				}
			}
		}
		
		// asigna la manzana en la posicion adecuada
		espacio[manzana->x][manzana->y] = (*front)->tipo;
		int random = rand() % 3;
		switch(random){
		case 0:
			crear_nodo(front,back,'#');
			break;
		case 1:
			crear_nodo(front,back,')');
			break;
		case 2:
			crear_nodo(front,back,'%');
			break;
		}
		borrar_nodo(front,back);
	}
}

char movimiento(char *letra, int *auxx, int *auxy, int *auxx2, int *auxy2, struct cuerpo *snake, char espacio[t][t], struct fruta *manzana, int *tam, int *muerto, char ult, int *puntaje, struct tipo_fruta **front, struct tipo_fruta **back){
	if (kbhit())
	{
		// lo almaceno
		*letra = getch();
		// asigno el movimiento que voy a hacer dependiendo de la direccion que ya tenia
		if (*letra == 'w' && ult == 's')
		{
			*letra = 's';
		}
		if (*letra == 'a' && ult == 'd')
		{
			*letra = 'd';
		}
		if (*letra == 's' && ult == 'w')
		{
			*letra = 'w';
		}
		if (*letra == 'd' && ult == 'a')
		{
			*letra = 'a';
		}
	}
	
	for (int i = 0; i < (*tam - 1); i++)
	{
		*auxx = snake[i + 1].x;
		*auxy = snake[i + 1].y;
		
		if (i != 0)
		{
			snake[i + 1].x = *auxx2;
			snake[i + 1].y = *auxy2;
		}
		else
		{
			snake[i + 1].x = snake[i].x;
			snake[i + 1].y = snake[i].y;
			
			ult = direccion(espacio, snake, *letra, muerto, ult, *tam);
			
			// si la posicion de la cabeza es la misma que la de la manzana
			comer(manzana, snake, puntaje, tam, *letra, espacio, ult, front, back);
		}
		espacio[snake[i + 1].x][snake[i + 1].y] = 'x';
		espacio[*auxx][*auxy] = ' ';
		*auxx2 = *auxx;
		*auxy2 = *auxy;
	}
	
	return ult;
}
	
	
void push(struct node **head, char nuevo_nombre[20], int nuevo_puntaje)
{
	struct node *nodo_nuevo = NULL;
	nodo_nuevo = (struct node*) malloc(sizeof(struct node));
	if(NULL == nodo_nuevo)
	{
		printf("No hay memoria suficiente");
		exit(1);
	}
	
	struct node *temp = *head;
	for(int i=0; i<20; i++){
		nodo_nuevo->nombre[i] = nuevo_nombre[i];
	}
	nodo_nuevo->puntaje = nuevo_puntaje;
	nodo_nuevo->siguiente = NULL;
	
	if(NULL == *head)
	{
		*head = nodo_nuevo;
	}else
	{
		while (NULL != temp->siguiente)
		{
			temp = temp->siguiente;
		}
		temp->siguiente = nodo_nuevo;
	}
}
	
void pop(struct node **head)
{
	struct node *temp = *head;
	while(temp->siguiente != NULL){
		temp = temp->siguiente;
	}
	free(temp);
}

void crear_nodo(struct tipo_fruta **front, struct tipo_fruta **back, char caracter){
	
	struct tipo_fruta *nodo_fruta = NULL;
	nodo_fruta = (struct tipo_fruta*) malloc(sizeof(struct tipo_fruta));
	
	nodo_fruta->tipo = caracter;
	nodo_fruta->siguiente = NULL;
	
	if(*back == NULL){
		*back = nodo_fruta;
		*front = *back;
	}
	else{
		(*back)->siguiente = nodo_fruta;
		*back = nodo_fruta;
	}
}
	
void borrar_nodo(struct tipo_fruta **front, struct tipo_fruta **back){
	
	struct tipo_fruta *temp = *front;
	
	*front = (*front)->siguiente;
	free(temp);
}
