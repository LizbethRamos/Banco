#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX_CLIENTES 50

struct Cliente{//estrucura cliente
	char nombre[50];
	char direccion[50];
	int no_Cuenta;
	float saldo;
} clientes[MAX_CLIENTES], *ap_Clientes;

typedef struct resp{//estructura que se enviará como respuesta hacia el cliente
	char respuesta[50];
	char nombreCliente[50];
	float cantidad;
}Respuesta;

typedef struct _Cuenta{//estructura que recibe el servidor
	int opcion;
	int No_Cuenta;
	float cantidad;
}Cuenta;

void error(char *msg){
    perror(msg);
    exit(1);
}

void accionCuenta(Cuenta cuenta);
int buscar(int pos,int totalClientes);
void cargaClientes(int *numCliente);
void guarda(int numCliente);

float cantidad = 0.0;
char mensaje[50]=" ";
char nombreCliente[50]=" ";

void main(int argc, char *argv[]){

     int sockfd, newsockfd, portno, clilen;
     Cuenta buffer;
     Respuesta respuesta;
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = 3445;//puerto por el que se comunican seridor y cliente
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
   do{
        n = read(newsockfd,&buffer,sizeof(Cuenta));//RECIBIR LA ESTRUCTURA 
        if (n < 0) 
        	error("ERROR reading from socket");
        accionCuenta(buffer); 
	    //se llena la estructura de respuesta con los datos actualizados
        strcpy(respuesta.respuesta, mensaje);
	printf("%s\n", respuesta.respuesta);
        strcpy(respuesta.nombreCliente,nombreCliente);
	printf("%s\n", respuesta.nombreCliente);
        respuesta.cantidad = cantidad;
	printf("%.2f\n", respuesta.cantidad);       
        n = write(newsockfd,&respuesta,sizeof(Respuesta));//MANDAR MENSAJEDE CANTIDAD Y ERROR
        if (n < 0) 
        error("ERROR writing to socket");
        setbuf(stdin,NULL);

    }while (buffer.opcion!=0);
    
    
       
    
}
void accionCuenta(Cuenta cuenta){
    char menAux[] = ".";
	int numClientes, posicion;
	cargaClientes(&numClientes);
	posicion = buscar(cuenta.No_Cuenta, numClientes);
	if(posicion != -1){
          switch (cuenta.opcion){
            case 1://consulta el saldo
                cantidad = clientes[posicion].saldo;
		strcpy(mensaje,"CONSULTA REALIZADA");
		strcpy(nombreCliente,clientes[posicion].nombre);
                break;
            case 2:// deposito 
                clientes[posicion].saldo = clientes[posicion].saldo + cuenta.cantidad;
                cantidad = clientes[posicion].saldo;
	        strcpy(mensaje,"DEPOSITO REALIZADO");
				strcpy(nombreCliente,clientes[posicion].nombre);
                break;
            case 3://retiro
                if (clientes[posicion].saldo > 0){
                    if (cuenta.cantidad > clientes[posicion].saldo)
                        strcpy(mensaje,"SALDO INSUFICIENTE");
                    else{
                        clientes[posicion].saldo = clientes[posicion].saldo - cuenta.cantidad;    
						strcpy(mensaje,"RETIRO REALIZADO"); 
						strcpy(nombreCliente,clientes[posicion].nombre); 
                    }      
                }else
                    strcpy(mensaje,"SALDO INSUFICIENTE");
                cantidad = clientes[posicion].saldo;
                break;
            default:
                break;
            }
        	guarda(numClientes);
    }else{
          strcpy(mensaje,"NO SE ENCONTRO AL CLIENTE");
          cantidad=0;
    }

}

int buscar(int clienteBuscar,int totalClientes){
	int i = 0;
	while(clientes[i].no_Cuenta != clienteBuscar){
		i++;
		if(i > totalClientes){
			printf("El cliente con nUmero de cuenta %d no existe\n", clienteBuscar);
			return -1;
		}
	}
	return i;
}

void cargaClientes(int *numCliente){
	int df, i = 0;
	df = open("BD.dat", 0);
	if( df < 0){
		printf("No existe la base de datos\n");
		return;
	}
	else{
		read(df, numCliente, sizeof(int));
		if((read(df, clientes, sizeof(clientes)) != sizeof(clientes))){
			printf("Error al leer el archivo");
			exit(1);
		}
		close(df);
		return;
	}
}

void guarda(int numCliente){
	int df, i;
	if((df = creat("BD.dat", 0777)) < 0){
		perror("No se pudo crear el archivo\n");
		exit(1);
	}
	if((df = open("BD.dat", 2)) < 0){
		perror("No se pudo abrir el archivo\n");
		exit(1);
	}
	printf("Base de datos actualizada\n");
	write(df, &numCliente, sizeof(int));
	ap_Clientes = clientes;
	for(i = 0; i < MAX_CLIENTES; i++){
		if((write(df, ap_Clientes, sizeof(struct Cliente)) != sizeof(struct Cliente))){
			printf("Error al escribir el archivo");
			exit(1);
		}
		ap_Clientes++;
	}
	close(df);
}
