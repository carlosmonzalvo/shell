#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define compara " "
#define MSJ_ERROR "Error de sintaxis\n"
#define MAX 51
#define PROMPT "#@> "

int CadIguales(char cadena1[MAX],char cadena2[MAX]);
void Comando(char cadena[MAX], int plano);
void redirecEntrada(char cadena[MAX]);
void redirecSalida(char cadena[MAX]);
void tuberias(char* argumento1[MAX], char* argumento2[MAX]);
void CrearProceso(char* argumento[MAX], int plano);


/* La funcion principal main contiene el bucle que se ejecutara de forma continua hasta que
el usuario escriba FIN en el terminal.Recibe una cadena, la analiza, y a partir de las
funciones que hemos creado, hara lo que debe de hacer */

int main(int argc, char *argv[])
{ /* INICIO DE LA FUNCION main */
char cadena[MAX];
char cadFin[]="FIN";
int fin=0,i,segplano=0, guardaStdout = dup(1), guardaStdin = dup(0);

while(fin==0)
	{
        close(1); /* Cierro la salida que tenga, caso de haber guardado algo en un fichero
                 sera el fichero*/
        dup(guardaStdout); // Asigno la salida estandar, es decir, la consola.
        close(0); /* Cierro la salida que tenga, caso de haber guardado algo en un fichero
                 sera el fichero*/
        dup(guardaStdin); // Asigno la salida estandar, es decir, la consola.
	printf(PROMPT); // Imprimimos el prompt
	scanf("\n%[^\n]",cadena); // Escaneamos la cadena entera hasta que pulsa intro
	segplano=0;
	for(i=0;cadena[i] != '\0'; i++){
		if(cadena[i] == '&') {
					cadena[i] = '\0';
					segplano = 1;
					}
						}
	fin=CadIguales(cadena,cadFin); // Comparamos si la cadena es fin, en tal caso sale
	Comando(cadena,segplano); // Y si no es fin, entra en la funcion Comando
	}

exit(0);//finaliza el programa (cadena 'FIN' introducida)

return(0);
} /* FIN DE LA FUNCION main */

/* Con esta funcion compararemos la cadena que introduce el usuario, con FIN,
en caso de ser iguales, saldra del programa, de lo contrario devolvera 0  y el programa
seguira en ejecucion */

int CadIguales(char cadena1[],char cadena2[])
{//inicio del CadIguales
int i=0;
int igual=0;

while ((cadena1[i]==cadena2[i])&&(cadena1[i]!='\0')&&(cadena2[i]!='\0')) i++;
if ((cadena1[i]==cadena2[i])&&(cadena1[i]=='\0')&&(cadena2[i]=='\0')) exit(0);
return(igual);
}//fin CadIguales

void Comando(char cadena[MAX], int plano)
{ /* INICIO DE LA FUNCION COMANDO */
int i,j,k,flag_tuberia;
char argumentoInd[MAX][MAX];
char argumentoInd2[MAX][MAX];
char redirec[MAX];
char entrada[MAX];
char *argumento[MAX];
char *argumento2[MAX];
int ejecutar=0;
argumento[0] = NULL;
argumento2[0] = NULL;
flag_tuberia = 0;

	 	k = 0;
	    i = 0;
		while(cadena[i] != '\0' && cadena[i] != '|' && cadena[i] != '>'){ /* INICIO DE WHILE */
					/* Con este for recorremos la cadena por completo, y la vamos
		almacenando
					en una variable que consiste en un array bidimensional de tipo char,
					es decir almacena cadenas de cadenas de caracteres */
					 for(j=0; cadena[i] != ' ' && cadena[i] != '\0' && cadena[i] != '|'  && cadena[i] != '>' && cadena[i] != '<';j++) {
								argumentoInd[k][j] = cadena[i];
								i++;}

					 /* Comprobamos si la cadena salir del bucle por encontrar un espacio...
					 en tal caso se incrementa la i, ya que sino entraria en un bucle
					 infinito*/
					 if (cadena[i] == ' ') i++;

					/* Asignamos el terminador de cadena a cada comando que es leido */
					argumentoInd[k][j] = '\0';

					/* Y finalmente una vez creada la cadena, se la pasamos al puntero
					argumento que sera el que se ejecute con la funcion execvp */
					argumento[k] = argumentoInd[k];
					k++;
					if (cadena[i] == '<') { /* INICIO DEL IF */
						i++;
						if (cadena[i] != ' ') ejecutar=1;
						else { i++;
							for(j=0; cadena[i] != '\0' && cadena[i] != ' ' && cadena [i] != '|' && cadena [i] != '>'; j++){
								entrada[j] = cadena[i];
								i++;
									}
							entrada[j] = '\0';
							if (cadena[i] != '\0') i++;
							redirecEntrada(entrada);
							}
						}/* FIN DEL IF */

					} /* FIN DEL WHILE */

		argumento[k] = NULL; // Asignamos NULL al ultimo comando a ejecutar para el EXECV

		/* Si encuentra una tuberia, entrara en el IF y separara el segundo comando de la
		misma forma que se hizo con el primero, y finalmente llamara a la funcion tuberias,
		pasandole los 2 argumentos como parametros */

		if (cadena[i] == '|') {/* INICIO DE IF */
			k=0;
			i++;
			if (cadena[i] != ' ') ejecutar=1;
			else { /* INICIO DEL ELSE */
			i++;
			flag_tuberia = 1;
			while(cadena[i] != '\0' && cadena[i] != '>'){/* INICIO DE WHILE */
					for(j=0; cadena[i] != ' ' && cadena[i] != '\0' && cadena[i] != '>';j++) {
								argumentoInd2[k][j] = cadena[i];
								i++;}
					if (cadena[i] == ' ' ) i++;

					argumentoInd2[k][j] = '\0';
					argumento2[k] = argumentoInd2[k];
					k++;
				}/* FIN DEL WHILE */
			argumento2[k] = NULL;
			}/*FIN EL ELSE */
		}/* FIN DE IF */


		/* Si encuentra un > cortara la cadena que sera el fichero que quiere utilizar
		para la salida */
		//
		if (cadena[i] == '>') {/* INICIO DE IF */
					i++;
					if (cadena[i] != ' ') ejecutar=1;
					else {
					i++;
					for(j=0; cadena[i] != '\0';j++) {
								redirec[j] = cadena[i];
								i++;}
					redirec[j] = '\0';
					redirecSalida(redirec);
					}
		}/* FIN DE IF */

		/* Comprobamos si la variable ejecutar tiene valor 0 o 1, si tiene valor 0
		el programa se ejecutara correctamente, si tiene valor 1 significa que
		mientras analizaba alguna de las cadenas ha encontrado un error de sintaxis
		y mostrara en pantalla un mensaje de error. */

		if(ejecutar == 0) {
					if (flag_tuberia==0) CrearProceso(argumento,plano);

					else tuberias(argumento,argumento2);
				  }
		else printf( MSJ_ERROR );

}/* FIN DE LA FUNCION COMANDO */

/* Con esta funcion si el usuario introduce quiere cambiar la entrada estandar
( el teclado ) por lo que hay en un fichero, podra hacerlo mediante el simbolo
"<", por ejemplo: wc < fichero */

void redirecEntrada(char cadena[MAX])

{ /* INICIO DE LA FUNCION redirecEntrada */
  char *cadenaPtr;
  int fd;
  cadenaPtr = cadena; // Puntero a la cadena
  fd = open (cadenaPtr,O_RDONLY); // Asigno a la salida el fichero
  close (0); // Cierro la salida estandar
  dup(fd);


} /* FIN DE LA FUNCION redirecEntrada */

/* Con esta funcion si el usuario introduce una redireccion de salida a un
fichero, en lugar de mostrar el comando en pantalla, lo guardara en fichero
por ejemplo: man -k file > lista.file */

void redirecSalida(char cadena[MAX])

{ /* INICIO DE LA FUNCION redirecSalida */
  char *cadenaPtr;
  cadenaPtr = cadena; // Puntero a la cadena
  close (1); // Cierro la salida estandar
  open (cadenaPtr,O_CREAT | O_WRONLY,0777); // Asigno a la salida el fichero

} /* FIN DE LA FUNCION redirecSalida */


/* Con esta funcion en el caso de que haya tuberias ( se ejecuten dos comandos
concatenados) por ejemplo: ls -la | sort , recibira como parametro argumento1 y
argumento2 de tipo puntero a cadena de cadena de caracteres y creara un hijo que
sera el encargado de ejecutar los 2 comandos pasados por tuberia.*/

void tuberias(char* argumento1[MAX], char* argumento2[MAX])

{ /* INICIO DE LA FUNCION tuberias */
  int fd[2],estado;
	pid_t hijo;
	hijo=fork();


	if (hijo==-1) printf("ERROR Creacion de proceso");
	else if (hijo==0) {
		   pipe(&fd[0]); /* Funcion pipe encargada de crear la tuberia */
   			if (fork()!=0) {
      				close(fd[0]); /* cerramos el lado de lectura del pipe */
      				close(1);
      				dup(fd[1]); /* STDOUT = extremo de salida del pipe */
      				close(fd[1]);/* cerramos el descriptor de fichero que sobra
								tras el dup(fd[1])*/
      				execvp(argumento1[0],argumento1);
   				}
   			else {
      				close(fd[1]);/* cerramos el lado de lectura del pipe */
      				close(0);
      				dup(fd[0]);/* STDOUT = extremo de salida del pipe */
      				close(fd[0]);/* cerramos el descriptor de fichero que sobra
								tras el dup(fd[0])*/
     				execvp(argumento2[0],argumento2);

  				 }
		   }
	else  hijo=wait(&estado);
} /* FIN DE LA FUNCION tuberias */


/* Con esta funcion en el caso de que no haya tuberias y sea un salo comando a ejecutar,
por ejemplo: ls -la , recibira como parametro el argumento de tipo puntero a cadena
de cadena de caracteres, creara un hijo para que el comando execvp no finalice el
programa y en definitiva ejecutara el comando que le pedimos.*/

void CrearProceso(char* argumento[MAX],int plano)

{	/* INICIO DE LA FUNCION CrearProceso */
	int estado=0;
	pid_t hijo;
	hijo=fork();
/* Comprobamos si el hijo se crea bien */
if (hijo==-1) printf("ERROR Creacion de proceso");
else if (hijo==0) {
			/* Y en caso de que el hijo esta bien creado, ejecutamos el proceso,
			si el programa comando a ejecutar no existe, nos da un error.*/
			execvp(argumento[0],argumento);
			perror("JASHELL");
			exit(estado);
		   }
else  {
		if (plano == 0) hijo=wait(&estado);
		}

} /* FIN DE LA FUNCION CrearProceso */
