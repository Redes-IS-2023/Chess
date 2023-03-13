#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <winsock2.h>
#include <json.h>
//#include <pthread.h>
#include "cJSON.h"
#include "cJSON.c"

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024

char buffer[1024];
int client_socket;
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

void recibirDatos();
int recvPosAct();
int recvPosDes();
void enviarDatos(int movedFrom, int movedTo);

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
//Tabla para codificar el msj
static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};
 
//Funcion q de-codifica el msj
void build_decoding_table() {
 
    decoding_table = malloc(256);
 
    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}
 
 
void base64_cleanup() {
    free(decoding_table);
} 
 
//Funcion q codifica el msj
char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length) {
 
    *output_length = 4 * ((input_length + 2) / 3);
 
    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;
 
    for (int i = 0, j = 0; i < input_length;) {
 
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;
 
        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
 
        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }
 
    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';
 
    return encoded_data;
}
 
 
unsigned char *base64_decode(const char *data,
                             size_t input_length,
                             size_t *output_length) {
 
    if (decoding_table == NULL) build_decoding_table();
 
    if (input_length % 4 != 0) return NULL;
 
    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;
 
    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;
 
    for (int i = 0, j = 0; i < input_length;) {
 
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
 
        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);
 
        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }
 
    return decoded_data;
}

void encode(const unsigned char *buffer){
    
    //cJSON *jSON_string = cJSON_Parse(buffer);
    //char *json_string = "{\"name\": \"John\", \"age\": 30, \"city\": \"New York\"}";
    char * data = buffer;
    long input_size = strlen(data);
    char * encoded_data = base64_encode(data, input_size, &input_size);
    printf("Encoded Data is: %s \n",encoded_data);
    
    long decode_size = strlen(encoded_data);
    char * decoded_data = base64_decode(encoded_data, decode_size, &decode_size);
    printf("Decoded Data is: %s \n",decoded_data);
    
}
int main()
{

	// ---------- Configuracion de Sockets -----------
	
	// Crear el socket
	int ret;
	
	struct sockaddr_in server_address;
	
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0){
		printf("Error connecting to socket.\n\n");
		exit(1);
	}
	
	printf("Client Socket created successfully!\n\n");
	
	// Especificar el address
	memset(&server_address, '\0', sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(6666);
	server_address.sin_addr.s_addr = INADDR_ANY;
	
	int yes = 1;
	if (setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1){
		perror("setsockopt");
		exit(1);
	}	
	
	// Conectar al server
	ret = connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
	if (ret < 0){
		printf("Error in the connection.\n\n");
		exit(1);
	}
	
	printf("Client connected to server.\n\n");
	
	// -------------------------------------------

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

	// Cerrar socket
	printf("Nos vemos!\n");
	close(client_socket);

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

// Recibe informacion en forma de arreglo y la almacena en la variable global "buffer"
void recibirDatos(){
	bzero(buffer, sizeof(buffer));
	while (1) {
        // Recibir mensaje del cliente
        n = recv(client_socket, buffer, 1024, 0);
        if (n < 0) {
            perror("Error al recibir mensaje");
            continue;
        }

		break;
    }
}

int recvPosAct(){
	struct json_object *parsedJSON;
	struct json_object *movedFrom;
	parsedJSON = json_tocken_parser(buffer);
	json_object_object_get_ex(parsedJSON, "movedFrom", &movedFrom);
	return movedFrom;
}
int recvPosDes(){
	struct json_object *parsedJSON;
	struct json_object *movedTo;
	parsedJSON = json_tocken_parser(buffer);
	json_object_object_get_ex(parsedJSON, "movedTo", &movedTo);
	return movedTo;
}

void enviarDatos(int movedFrom, int movedTo){
	int id = numeroPartida;
	bool isTurn = false;
	if (turnoAnf)
		isTurn = true;
	char type = getPieza(movedFrom);

	// +++ Armar JSON y guardarlo en buffer +++

	send(client_socket, buffer, sizeof(buffer), 0);
	bzero(buffer, sizeof(buffer));
}

// ==================================================

void turnoAnfitrion()
{

	recibirDatos();
	int posAct = recvPosAct();
	int pos = recvPosDes();
	bzero(buffer, sizeof(buffer));

	if (validarMovimiento(posAct, pos))
	{
		moverPieza(posAct, pos);

		turnoAnf = false;
		enviarDatos(posAct, pos);
	}
	else
	{
		enviarDatos(-1, -1);
		turnoAnfitrion(); // Recursividad hasta que se escoja un movimiento valido
	}
}

void turnoGuests()
{

	int posAct;
	int pos;
	// +++ Thread loop por votacion para cada jugador ??? +++

	recibirDatos();
	int posAct = recvPosAct();
	int pos = recvPosDes();
	bzero(buffer, sizeof(buffer));

	// +++ Seleccionar posiciones mas votadas +++

	// +++ Si hay empate, posiciones random en un while de validacion para que sea una jugada valida +++

	if (validarMovimiento(posAct, pos))
	{
		moverPieza(posAct, pos);

		turnoAnf = true;
		enviarDatos(isTurn, posAct, pos);
	}
	else
	{
		enviarDatos(isTurn, -1, -1);
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
