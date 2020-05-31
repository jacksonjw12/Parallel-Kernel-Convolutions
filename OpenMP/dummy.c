#include <stdio.h>

#include <stdlib.h>
#include <sys/syscall.h>
#include "cJSON.h"
#define MAXBUFLEN 1000000

void copyFile(char* s, char* d){
    FILE *source, *dest;
    int i;
    source = fopen(s, "rb");

    if( source == NULL ) {
        return;
    } //exit(EXIT_FAILURE);

    fseek(source, 0, SEEK_END);
    int length = ftell(source);

    fseek(source, 0, SEEK_SET);

    dest = fopen(d, "wb");

    if( dest == NULL ) {
        fclose(source);
        return;
    }

    for(i = 0; i < length; i++){
        fputc(fgetc(source), dest);
    }

    printf("File copied successfully.\n");
    fclose(source);
    fclose(dest);

}
cJSON* getConfig(){
    char source[MAXBUFLEN + 1];
    FILE *fp = fopen("../transferData/config.json", "r");
    if (fp == NULL) {
        return NULL;
    }
    size_t newLen = fread(source, sizeof(char), MAXBUFLEN, fp);
    if ( ferror( fp ) != 0 ) {
        fputs("Error reading file", stderr);
    } else {
        source[newLen++] = '\0';
    }
    fclose(fp);
    return cJSON_Parse(source);



}
void writeToTimingJSON(double timing, char* fileOutputLocation){
    cJSON *obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "timing", timing);
    cJSON_AddStringToObject(obj, "fileOutputLocation", fileOutputLocation);

    FILE *fp = fopen("../transferData/OMPTiming.json", "w");
    if (fp != NULL){
        fputs(cJSON_Print(obj), fp);
        fclose(fp);
    }
}

int main() {


    cJSON *configjson = getConfig();

    cJSON *fileInputJSON = cJSON_GetObjectItem(configjson, "fileInputLocation");
    cJSON *fileOutputJSON = cJSON_GetObjectItem(configjson, "openMPOutputLocation");

    char* fileInputLocation = fileInputJSON->valuestring;
    char* fileOutputLocation = fileOutputJSON->valuestring;

    printf("Copying %s to %s \n",fileInputLocation,fileOutputLocation);


    //This copy file function is in place of the filter that has to be written
    //In the implementation, read image from the input file, and write image to output file
    //then replace the .01 with the seconds it took to perform the kernel convolution
    copyFile(fileInputLocation,fileOutputLocation);



    //replace the .02 with the actual timing(its a double (seconds))
    writeToTimingJSON(0.02, fileOutputLocation);
    return 0;
}