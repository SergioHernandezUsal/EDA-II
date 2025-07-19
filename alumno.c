//Sergio Hernández Moya 70915609P PA1

#include <stdio.h>
#include <string.h>
#include "../include/alumno.h"
#include "../include/dispersion.h"


int funcionHash(tAlumno *reg, int nCubos) {
    long dniNum = atol(reg->dni);  
    return dniNum % nCubos;
}



void mostrarReg(tAlumno *reg) {
    printf("%s %s %s %s %s\n", reg->dni, reg->nombre, reg->ape1, reg->ape2, reg->provincia);
}

int cmpClave(tAlumno *reg1, tAlumno *reg2) {
    return (strcmp(reg1->dni, reg2->dni)==0)? 1: 0;
}


int buscar(char *fichero, char *dni) {
    FILE *f = fopen(fichero, "rb");
    if (!f) return -2;

    tAlumno reg;
    memset(&reg, 0, sizeof(tAlumno));

    strncpy(reg.dni, dni, sizeof(reg.dni) - 1);
    reg.dni[sizeof(reg.dni) - 1] = '\0';  
    tPosicion pos;
    int res = busquedaHash(f, &reg, &pos);
    if (res == 0) {
        mostrarReg(&reg);
    }

    fclose(f);
    return res;
}



int modificar(char *fichero, char *dni, char *nuevaProvincia) {
    FILE *f = fopen(fichero, "rb+");
    if (!f) return -2;

    tAlumno reg;
    memset(&reg, 0, sizeof(tAlumno));  

    strncpy(reg.dni, dni, sizeof(reg.dni) - 1);
    reg.dni[sizeof(reg.dni) - 1] = '\0';  

    tPosicion pos;
    int res = busquedaHash(f, &reg, &pos);
    if (res != 0) {
        fclose(f);
        return res;
    }

    strncpy(reg.provincia, nuevaProvincia, sizeof(reg.provincia) - 1);
    reg.provincia[sizeof(reg.provincia) - 1] = '\0';

    res = modificarReg(f, &reg, &pos);
    fclose(f);
    return res;
}


int insertarAlumno(char *fichero, tAlumno *nuevo) {
    FILE *f = fopen(fichero, "rb+");
    if (!f) return -2;

    regConfig regC;
    fread(&regC, sizeof(regConfig), 1, f);

    int res = insertar(f, nuevo, &regC);

    fseek(f, 0, SEEK_SET);
    fwrite(&regC, sizeof(regConfig), 1, f);

    fclose(f);
    return res;
}
