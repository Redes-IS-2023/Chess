#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include <stdbool.h>
#include <time.h>

int numeroPartida;

// Las mayusculas son las piezas del blanco (Anfitrion), las minusculas del negro (Guests)
	// Tt = Torre, Cc = Caballo, Aa = Alfil, Rr = Rey, Qq = Reina, Pp = Peon
	/* Para las coordenadas de las posiciones se usara el siguiente formato

		[00,01,02,03,04,05,06,07]
		[10,11,12,13,14,15,16,17]
		[20,21,22,23,24,25,26,27]
		[30,31,32,33,34,35,36,37]
		[40,41,42,43,44,45,46,47]
		[50,51,52,53,54,55,56,57]
		[60,61,62,63,64,65,66,67]
		[70,71,72,73,74,75,76,77]

	*/

char tablero[8][8] = {
                    { 'T' , 'C' , 'A' , 'R' , 'Q' , 'A' , 'C' , 'T' },
                    { 'P' , 'P' , 'P' , 'P' , 'P' , 'P' , 'P' , 'P' },
                    { ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' },
                    { ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' },
                    { ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' },
                    { ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' },
                    { 'p' , 'p' , 'p' , 'p' , 'p' , 'p' , 'p' , 'p' },
                    { 't' , 'c' , 'a' , 'r' , 'q' , 'a' , 'c' , 't' }
                    };

bool turnoAnf = true;
int ganador = 0;	// 0 si la partida no ha terminado, 1 Si gana el anfitrion, 2 si ganan los invitados


void crearJuego();
void turnoAnfitrion();
void turnoGuests();

bool validarMovimiento(int posAct, int pos);
bool valMoverTorre(int posAct, int pos);
bool valMoverCaballo(int posAct, int pos);
bool valMoverAlfil(int posAct, int pos);
bool valMoverRey(int posAct, int pos);
bool valMoverReina(int posAct, int pos);
bool valMoverPeon(int posAct, int pos);

void moverPieza(int posAct, int pos);

// Jaque
// Jaque Mate

char getPieza(int pos);


int main (){

	crearJuego();

	while (ganador == 0){
		if (turnoAnf)
			turnoAnfitrion();
		else
			turnoGuests();
	}

	return 0;
}


void crearJuego(){

	// Numero random entre lower y upper para que sea el numero de partida
	int lower = 1, upper = 50;
	int num;
	while (true){
		
		srand(time(0));
		num = (rand() % (upper - lower + 1)) + lower;

		// If el numero no existe
		break;
	}

	numeroPartida = num;
}


void turnoAnfitrion(){

	int posAct;
	int pos;
	// Recibir Posicion Actual
	// Recibir Posicion a Jugar

	if (validarMovimiento(posAct, pos)){
		moverPieza(posAct, pos);

		if (turnoAnf)
			turnoAnf = false;
		else
			turnoAnf = true;
	}
}

bool validarMovimiento(int posAct, int pos){

	char pieza = getPieza(posAct);
	char piezaEnPos = getPieza(pos);

	// Validar que esta moviendo las piezas que le pertenecen
	// If pieza es mayuscula
		if (!turnoAnf)
			return false;
		// If piezaEnPos es mayuscula	// No puede comerse una de sus propias piezas
			//return false;
	// If pieza es minuscula
		if (turnoAnf)
			return false;
		// If piezaEnPos es minuscula
			//return false;

	if (pieza == 'T' || pieza == 't')	// -- STRCMP? --
		return valMoverTorre(posAct, pos);
	if (pieza == 'C' || pieza == 'c')
		return valMoverCaballo(posAct, pos);
	if (pieza == 'A' || pieza == 'a')
		return valMoverAlfil(posAct, pos);
	if (pieza == 'R' || pieza == 'r')
		return valMoverRey(posAct, pos);
	if (pieza == 'Q' || pieza == 'q')
		return valMoverReina(posAct, pos);
	if (pieza == 'P' || pieza == 'p')
		return valMoverPeon(posAct, pos);
	return NULL;
}

char getPieza(int pos){

	int posAct = 0;
	for (int i = 0; i < 8; i++){	// Por cada fila
		for (int j = 0; j < 8; j++){	// Para cada columna
			if (posAct == pos)
				return tablero[i][j];
			posAct ++;
		}
		posAct += 3;
	}
	return ' ';
}

bool valMoverTorre(int posAct, int pos){}
bool valMoverCaballo(int posAct, int pos){}
bool valMoverAlfil(int posAct, int pos){}
bool valMoverRey(int posAct, int pos){}
bool valMoverReina(int posAct, int pos){}
bool valMoverPeon(int posAct, int pos){}

void moverPieza(int posAct, int pos){

	char pieza = getPieza(posAct);
	
	int count = 0;
	for (int i = 0; i < 8; i++){	// Por cada fila
		for (int j = 0; j < 8; j++){	// Para cada columna
			if (count == pos)
				// tablero = Funcion para cambiar un caracter por otro en un arreglo (cambiar tablero[i][j] por pieza)
			if (count == posAct)
				// tablero = Funcion para cambiar un caracter por otro en un arreglo (cambiar tablero[i][j] por ' ')
			count ++;
		}
		count += 3;
	}
}




















