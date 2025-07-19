//Sergio Hernández Moya 70915609P PA1

#include <stdio.h>
#include <string.h>
#include "../include/asignatura.h"
#include "../include/dispersion.h"  

int funcionHash(tAsignatura *reg, int nCubos) {
    return reg->codigo % nCubos;
}

void mostrarReg(tAsignatura *reg) {
    printf("%-6d %-50s %c %.1f %.1f %c %c %d / %d\n",
           reg->codigo,        
           reg->nombre,        
           reg->curso,         
           reg->creditosT,     
           reg->creditosP,     
           reg->tipo,          
           reg->cuatrimestre,  
           reg->numGrT,        
           reg->numGrP);      
}



int cmpClave(tAsignatura *reg1, tAsignatura *reg2) {
    return (reg1->codigo == reg2->codigo)? 1: 0;
}

int buscar(char *fichero, int codigo) {
    FILE *f = fopen(fichero, "rb");
    if (!f) return -2;

    tAsignatura reg;
    tPosicion pos;
    reg.codigo = codigo;

    int res = busquedaHash(f, &reg, &pos);
   if (res == 0) {
        mostrarReg(&reg);
    }

    fclose(f);
    return res;
}

int modificar(char *fichero, int codigo, float creditosT, float creditosP) {
    FILE *f = fopen(fichero, "rb+");
    if (!f) return -2;

    tAsignatura reg;
    tPosicion pos;
    reg.codigo = codigo;

    int res = busquedaHash(f, &reg, &pos);
    if (res != 0) {
        fclose(f);
        return res;
    }

    reg.creditosT = creditosT;
    reg.creditosP = creditosP;

    res = modificarReg(f, &reg, &pos);

    fclose(f);
    return res;
}

int insertarAsignatura(char *fichero, tAsignatura *nuevo) {
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
