#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	FILE *file;
	char line[256]="0";
	char elements[255]="0";
	
	printf("Caractéristiques de la machine à afficher (cpuinfo, meminfo, partitions, version : ");
	
	scanf("%s",&elements);
	
	if(strcmp(elements,"cpuinfo") == 0 ){
	file = fopen("/proc/cpuinfo","r");
	}
	else if(strcmp(elements,"meminfo") == 0 ){
	file = fopen("/proc/meminfo","r");
	}
	else if(strcmp(elements,"partitions") == 0 ){
	file = fopen("/proc/partitions","r");
	}
	else if(strcmp(elements,"version") == 0 ){
	file = fopen("/proc/version","r");
	}
	

	if(file == NULL){
		perror("Impossible d'ouvrir ");
		return 1;
		}
		
	while(fgets(line, sizeof(line), file) != NULL) {
		printf("%s",line);
	}
	
	fclose(file);
	
	return 0;
}



   
