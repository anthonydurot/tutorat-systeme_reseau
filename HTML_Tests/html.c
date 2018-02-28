#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FICHIER_HTML_SOURCE "index_source.html"
#define FICHIER_HTML_GENERE "index.html"
#define MAX_BUFFER 1024


int traiter_HTML(FILE* fs, FILE* fd) {
	
	char ligne[MAX_BUFFER], buffer[MAX_BUFFER], value[6];
	char *ret, *str;
	
	// Pour tester
	
	float x = 1.2;
	float y = 3.4;
	float z = 0;
	float temp = 21.8;
	
	memset(buffer, '\0', sizeof(buffer));
	
	while ( fgets(ligne, MAX_BUFFER, fs) != NULL ) {
	
		if((ret = strstr(ligne, "{{")) == NULL) {
		
			fprintf(fd,"%s",ligne);	
			
		}
		else {
			
			if((ret = strstr(ligne, "{{ACCEL_X}}")) != NULL) {
			
				str = strtok(ligne,"{");
				strcat(buffer,str);
				sprintf(value, "%.2f", x);
				
			}
			
			else if((ret = strstr(ligne, "{{ACCEL_Y}}")) != NULL) {
			
				str = strtok(ligne,"{");
				strcat(buffer,str);
				sprintf(value, "%.2f", y);

			}
			
			else if((ret = strstr(ligne, "{{ACCEL_Z}}")) != NULL) {
			
				str = strtok(ligne,"{");
				strcat(buffer,str);
				sprintf(value, "%.2f", z);
				
			}
			
			else if((ret = strstr(ligne, "{{TEMP}}")) != NULL) {
			
				str = strtok(ligne,"{");
				strcat(buffer,str);
				sprintf(value, "%.2f", temp);
				
			}	
										
			str = strtok(NULL,"}");
			str = strtok(NULL,"}");
			strcat(buffer,value);
			strcat(buffer,str);
			fprintf(fd,"%s",buffer);			
			memset(buffer, '\0', sizeof(buffer));

		}
	}	

	return 0;

}


int main(void) {

	FILE* fs = fopen(FICHIER_HTML_SOURCE,"r");
	
	// Prendre le semaphore
	
	FILE* fd = fopen(FICHIER_HTML_GENERE,"w+");
	
	traiter_HTML(fs,fd);
	
	// Envoyer les fichiers (HTML/CSS/JS)
	fclose(fs);
	fclose(fd);
	
	// Rendre le sémaphore
	

}
