#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

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
	{'T', 'C', 'A', 'R', 'Q', 'A', 'C', 'T'},
	{'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
	{'t', 'c', 'a', 'r', 'q', 'a', 'c', 't'}};

bool turnoAnf = true;
int ganador = 0; // 0 si la partida no ha terminado, 1 Si gana el anfitrion, 2 si ganan los invitados

void crearJuego();
void turnoAnfitrion();
void turnoGuests();

bool validarMovimiento(int posAct, int pos);
bool valMoverTorre(int posAct, int pos);
bool valMoverCaballo(int posAct, int pos);
bool valMoverAlfil(int posAct, int pos);
bool valMoverRey(int posAct, int pos);
bool valMoverReina(int posAct, int pos);
bool valMoverPeon(char piezaAct, char piezaEnPos, int posAct, int pos);

void moverPieza(int posAct, int pos);
bool jaque();
bool jaquemate();

char getPieza(int pos);

// ==================================================

int main()
{

	crearJuego();

	while (ganador == 0)
	{
		if (turnoAnf)
		{
			if (jaquemate())
			{
				// +++ Avisar al anfitrion que perdio +++
				break;
			}

			if (jaque())
				// +++ Avisar al anfitrion que le hicieron jaque +++

				turnoAnfitrion();
		}
		else
		{
			if (jaquemate())
			{
				// +++ Avisar a los guests que perdieron +++
				break;
			}

			if (jaque())
				// +++ Avisar a los guests que les hicieron jaque +++

				turnoGuests();
		}
	}

	return 0;
}

// Inicializa el juego dandole un numero random a la partida
void crearJuego()
{

	// Numero random entre lower y upper para que sea el numero de partida
	int lower = 1, upper = 50;
	int num;
	while (true)
	{

		srand(time(0));
		num = (rand() % (upper - lower + 1)) + lower;

		// +++ If el numero no existe +++
		break;
	}

	numeroPartida = num;
}

// ==================================================

void turnoAnfitrion()
{

	int posAct;
	int pos;
	// +++ Recibir Posicion Actual +++
	// +++ Recibir Posicion a Jugar +++

	if (validarMovimiento(posAct, pos))
	{
		moverPieza(posAct, pos);

		turnoAnf = false;
	}
	else
	{
		// +++ Avisar al jugador que no es un movimiento valido ??? +++
		turnoAnfitrion(); // Recursividad hasta que se escoja un movimiento valido
	}
}

void turnoGuests()
{

	int posAct;
	int pos;
	// +++ Thread loop por votacion para cada jugador ??? +++

	// +++ Recibir Posicion Actual +++
	// +++ Recibir Posicion a Jugar +++

	// +++ Seleccionar posiciones mas votadas +++

	// +++ Si hay empate, posiciones random en un while de validacion para que sea una jugada valida +++

	if (validarMovimiento(posAct, pos))
	{
		moverPieza(posAct, pos);

		turnoAnf = true;
	}
	else
	{
		// +++ Avisar al jugador que no es un movimiento valido ??? +++
		turnoGuests(); // Recursividad hasta que se escoja un movimiento valido
	}
}

// ==================================================

bool validarMovimiento(int posAct, int pos)
{

	char piezaAct = getPieza(posAct);
	char piezaEnPos = getPieza(pos);

	if (isupper(piezaAct))
	{
		if (!turnoAnf) // Validar que esta moviendo las piezas que le pertenecen
			return false;
		if (isupper(piezaEnPos)) // No puede comerse una de sus propias piezas
			return false;
	}
	if (!isupper(piezaAct))
	{
		if (turnoAnf)
			return false;
		if (!isupper(piezaEnPos))
			return false;
	}

	if (pos == posAct) // No se puede mover a la misma posicion en la que se esta actualmente
		return false;

	// Validacion de movimiento por cada pieza
	if (piezaAct == 'T' || piezaAct == 't')
		return valMoverTorre(posAct, pos);
	if (piezaAct == 'C' || piezaAct == 'c')
		return valMoverCaballo(posAct, pos);
	if (piezaAct == 'A' || piezaAct == 'a')
		return valMoverAlfil(posAct, pos);
	if (piezaAct == 'R' || piezaAct == 'r')
		return valMoverRey(posAct, pos);
	if (piezaAct == 'Q' || piezaAct == 'q')
		return valMoverReina(posAct, pos);
	if (piezaAct == 'P' || piezaAct == 'p')
		return valMoverPeon(piezaAct, piezaEnPos, posAct, pos);

	return NULL;
}

bool valMoverTorre(int posAct, int pos)
{
	// Si la coordenada y de la posicion destino es igual a la coordenada y de la posicion actual, es valido
	if (pos % 10 == posAct % 10)
		return true;
	// Si la coordenada x de la posicion destino es igual a la coordenada x de la posicion actual, es valido
	if (pos / 10 == posAct / 10)
		return true;

	return false;
}

bool valMoverCaballo(int posAct, int pos)
{
	// Son los 8 movimientos posibles de un caballo
	if (posAct + 8 == pos || pos + 8 == posAct)
		return true;
	if (posAct + 12 == pos || pos + 12 == posAct)
		return true;
	if (posAct + 19 == pos || pos + 19 == posAct)
		return true;
	if (posAct + 21 == pos || pos + 21 == posAct)
		return true;

	return false;
}

bool valMoverAlfil(int posAct, int pos)
{
	// Diagonal en esta direccion: /
	if (pos % 9 == posAct % 9)
		return true;
	// Diagonal en esta direccion: \ 
	if (pos % 11 == posAct % 11)
	return true;

	return false;
}

bool valMoverRey(int posAct, int pos)
{
	// Horizontal
	if (posAct + 1 == pos || pos + 1 == posAct)
		return true;
	// Vertical
	if (posAct + 10 == pos || pos + 10 == posAct)
		return true;
	// Diagonal /
	if (posAct + 9 == pos || pos + 9 == posAct)
		return true;
	// Diagonal \ 
	if (posAct + 11 == pos || pos + 11 == posAct)
	return true;

	return false;
}

bool valMoverReina(int posAct, int pos)
{
	// Es una combinacion de los movimientos de la torre y el alfil

	// Movimientos de la torre
	if (pos % 10 == posAct % 10)
		return true;
	if (pos / 10 == posAct / 10)
		return true;

	// Movimientos del alfil
	if (pos % 9 == posAct % 9)
		return true;
	if (pos % 11 == posAct % 11)
		return true;

	return false;
}

bool valMoverPeon(char piezaAct, char piezaEnPos, int posAct, int pos)
{
	if (isupper(piezaAct))
	{ // Se moveria para abajo por ser blanca
		if (posAct + 10 == pos)
		{ // Movimiento vertical si no hay pieza en frente
			if (piezaEnPos == ' ')
				return true;
			return false;
		}
		if (posAct + 9 == pos || posAct + 11 == pos)
		{						   // Movimiento en diagonal hacia el frente solo si puede comer pieza enemiga
			if (piezaEnPos != ' ') // Ya se valido antes que no sea una pieza del mismo jugador
				return true;
			return false;
		}
	}
	else
	{ // Se moveria hacia arriba por ser negra
		if (pos + 10 == posAct)
		{
			if (piezaEnPos == ' ')
				return true;
			return false;
		}
		if (pos + 9 == posAct || pos + 11 == posAct)
		{
			if (piezaEnPos != ' ')
				return true;
			return false;
		}
	}
	return false;
}

// ==================================================

// Se modifica el tablero para que una pieza se mueva de una posicion a otra
void moverPieza(int posAct, int pos)
{

	char pieza = getPieza(posAct);

	int count = 0;
	for (int i = 0; i < 8; i++)
	{ // Por cada fila
		for (int j = 0; j < 8; j++)
		{					  // Para cada columna
			if (count == pos) // Se encontro la posicion destino
				tablero[i][j] = pieza;
			if (count == posAct) // Se encontro la posicion actual
				tablero[i][j] = ' ';
			count++;
		}
		count += 3;
	}
}

bool jaque()
{

	return false;
}
bool jaquemate()
{

	return false;
}

// ==================================================

// Recibe una posicion y retorna el nombre de la pieza correspondiente
char getPieza(int pos)
{

	int posAct = 0;
	for (int i = 0; i < 8; i++)
	{ // Por cada fila
		for (int j = 0; j < 8; j++)
		{ // Para cada columna
			if (posAct == pos)
				return tablero[i][j];
			posAct++;
		}
		posAct += 3;
	}
	return ' ';
}
