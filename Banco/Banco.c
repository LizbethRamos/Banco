#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTES 50

struct Cliente{
	char nombre[50];
	char direccion[50];
	int no_Cuenta;
	float saldo;
} clientes[MAX_CLIENTES], *ap_Clientes;

void cargaClientes(int *numCliente);
int buscar(int pos,int totalClientes);
void alta(int numCliente);
void baja(int aux, int numCliente);
void consulta(int aux, int numCliente);
void guarda(int numCliente);


int main(){
	char opcion = ' ';
	int numCliente = 0, aux;
	cargaClientes(&numCliente);
	printf("Presione enter para continuar\n");
	getchar();
	while(opcion!= '4'){
		system("clear");
		printf("\t\t MENU\n1. Dar de alta.\n2. Dar de baja.\n3.Consultar.\n4. Salir\nElija una opcion: ");
		scanf("%s", &opcion);
		switch(opcion){
			case '1':
				alta(numCliente);
				numCliente = numCliente+1;
				break;
			case '2':
				printf("ingrese el Numero de cuenta del cliente a eliminar\n");
				scanf("%d", &aux);
				baja(aux, numCliente);
				numCliente = numCliente -1;
				break;
			case '3':
				printf("ingrese el Numero de cuenta del cliente a consultar\n");
				scanf("%d", &aux);
				consulta(aux, numCliente);
				break;
			case '4':
				guarda(numCliente);
				printf("Au revoir!\n");
				exit(1);
			default:
				printf("Elija una opcion del menu\n");
				break;
		}
	}
	return 0;
}

void cargaClientes(int *numCliente){
	printf("\t Cargando Base de Datos Clientes\n");
	sleep(1);
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

void alta(int numCliente){

	if(numCliente >= MAX_CLIENTES)
		printf("No se pueden agregar mas clientes\n");
	else{
		char nombre[50], direccion[50];

		printf("Nombre: ");
		setbuf(stdin,NULL);
		scanf("%[^\n]",&nombre);
		strcpy(clientes[numCliente].nombre, nombre);
		printf("Direccion: ");
		setbuf(stdin,NULL);
		scanf("%[^\n]",&direccion);
		strcpy(clientes[numCliente].direccion, direccion);
		getchar();
		clientes[numCliente].no_Cuenta = (numCliente+1);
		printf("Saldo: ");
		scanf("%f",&clientes[numCliente].saldo);
		strcpy(nombre, "");
		strcpy(direccion, "");
		printf("El cliente fué guardado exitosamente\n");
	}
	printf("Presione enter para continuar\n");
	getchar();
	getchar();
}

void baja(int aux, int numCliente){
	if(numCliente == 0)
		printf("No hay clientes registrados\n");
	else{
		int posicion = buscar(aux, numCliente);
		if(posicion != -1){
			for(posicion; posicion < numCliente; posicion++)
				clientes[posicion] = clientes[posicion+1];
			printf("Cliente eliminado exitosamente\n");

		}
	}
	printf("Presione enter para continuar\n");
	getchar();
	getchar();
}

void consulta(int aux, int numCliente){
	if(numCliente == 0)
		printf("No hay clientes registrados\n");
	else{
		int posicion = buscar(aux, numCliente);
		if(posicion != -1){
			printf("Datos del cliente %d\n",aux );
			printf("\nNombre: %s\n", clientes[posicion].nombre);
			printf("DirecciOn: %s\n", clientes[posicion].direccion);
			printf("No_Cuenta: %d\n", clientes[posicion].no_Cuenta);
			printf("Saldo: %.2f\n", clientes[posicion].saldo);
		}
	}
	printf("Presione enter para continuar\n");
	getchar();
	getchar();
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
