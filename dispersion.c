//Sergio Hernández Moya 70915609P PA1

#include "../include/dispersion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int leeHash(char *fichHash)
{ FILE *f;
  tipoCubo cubo;
  regConfig regC;
  int j,nCubo=0,densidadOcupacion;
  
   if ((f = fopen(fichHash,"rb"))==NULL) return -2;
   fread(&regC,sizeof(regConfig),1,f);
   fread(&cubo,sizeof(cubo),1,f); 
   while (!feof(f)){
	for (j=0;j<C;j++) {
        	if (j==0)    	printf("Cubo %2d (%2d reg. ASIGNADOS)",nCubo,cubo.numRegAsignados);
        	else 	if ((j==1) && cubo.desbordado) printf("DESBORDADO\t\t");
        		else printf("\t\t\t");
		if (j < cubo.numRegAsignados) 
			mostrarReg(&(cubo.reg[j]));
	    	else printf ("\n");
        	}
		nCubo++;
       fread(&cubo,sizeof(cubo),1,f);
   }
   fclose(f);
   printf("ORGANIZACIÓN MÉTODO DISPERSIÓN:\n\t %d CUBOS y %d CUBOS DESBORDE con capacidad %d\n",
   		regC.nCubos,regC.nCubosDes, C);   
   printf("Cubo de Desborde Actual: %d\n", regC.nCuboDesAct);  

   printf("\t Contiene %d registros de los cuales se han desbordado %d:\n",regC.numReg,regC.numRegDes);
   	
   densidadOcupacion = 100*regC.numReg/((regC.nCubos+regC.nCubosDes)*C);
   printf("Densidad ACTUAL de ocupación: %d \n\t(MÍNIMA permitida %f  MÁXIMA permitida %f)\n",
   				densidadOcupacion,regC.densidadMin,regC.densidadMax);
   
   
   if (densidadOcupacion>regC.densidadMax) {
   	printf("No se respeta la densidad máxima de ocupacion\n");
   }	

   if (densidadOcupacion<regC.densidadMin) {
   	printf("No se respeta la densidad mínima de ocupacion\n");
   }	
return 0;	
}

int creaHvacio(char *fichHash, regConfig *regC) {
    FILE *f = fopen(fichHash, "wb");
    if (!f) return -2;

   
    regC->numReg = 0;
    regC->numRegDes = 0;
    regC->nCuboDesAct = regC->nCubos;

    fwrite(regC, sizeof(regConfig), 1, f);

    tipoCubo cuboVacio;
    memset(&cuboVacio, 0, sizeof(tipoCubo));

    int totalCubos = regC->nCubos + regC->nCubosDes;
    for (int i = 0; i < totalCubos; i++) {
        fwrite(&cuboVacio, sizeof(tipoCubo), 1, f);
    }

    fclose(f);
    return 0;
}

int desborde(FILE *f, tipoReg *reg, regConfig *regC) {
    tipoCubo cubo;
    long offset;

    
    for (int cuboIndex = regC->nCubos; cuboIndex < regC->nCubos + regC->nCubosDes; cuboIndex++) {
        offset = sizeof(regConfig) + cuboIndex * sizeof(tipoCubo);
        fseek(f, offset, SEEK_SET);
        fread(&cubo, sizeof(tipoCubo), 1, f);

        if (cubo.numRegAsignados < C) {
            cubo.reg[cubo.numRegAsignados++] = *reg;
            fseek(f, offset, SEEK_SET);
            fwrite(&cubo, sizeof(tipoCubo), 1, f);

            regC->numReg++;
            regC->numRegDes++;

            
            regC->nCuboDesAct = cuboIndex;

            fseek(f, 0, SEEK_SET);
            fwrite(regC, sizeof(regConfig), 1, f);
            fflush(f);
            return 0;
        }
    }

    
    tipoCubo nuevoCubo;
    memset(&nuevoCubo, 0, sizeof(tipoCubo));
    nuevoCubo.reg[0] = *reg;
    nuevoCubo.numRegAsignados = 1;

    offset = sizeof(regConfig) + (regC->nCubos + regC->nCubosDes) * sizeof(tipoCubo);
    fseek(f, offset, SEEK_SET);
    fwrite(&nuevoCubo, sizeof(tipoCubo), 1, f);

    regC->numReg++;
    regC->numRegDes++;
    regC->nCubosDes++;
    regC->nCuboDesAct = regC->nCubos + regC->nCubosDes - 1;

    fseek(f, 0, SEEK_SET);
    fwrite(regC, sizeof(regConfig), 1, f);
    fflush(f);

    return 0;
}

int insertar(FILE *f, tipoReg *reg, regConfig *regC) {
    
     int hash = funcionHash(reg, regC->nCubos); 
    long offset = sizeof(regConfig) + hash * sizeof(tipoCubo);

    tipoCubo cubo;
    fseek(f, offset, SEEK_SET);
    fread(&cubo, sizeof(tipoCubo), 1, f);

    if (cubo.numRegAsignados < C) {
        cubo.reg[cubo.numRegAsignados++] = *reg;
        fseek(f, offset, SEEK_SET);
        fwrite(&cubo, sizeof(tipoCubo), 1, f);
        regC->numReg++;  
    } else {
        cubo.desbordado = 1;
        fseek(f, offset, SEEK_SET);
        fwrite(&cubo, sizeof(tipoCubo), 1, f);
        return desborde(f, reg, regC);  
    }

    return 0;
}



int creaHash(char *fichEntrada, char *fichHash, regConfig *regC) {
    FILE *fIn = fopen(fichEntrada, "rb");
    if (!fIn) return -1;

    if (creaHvacio(fichHash, regC) != 0) {
        fclose(fIn);
        return -2;
    }

    FILE *fOut = fopen(fichHash, "rb+");
    if (!fOut) {
        fclose(fIn);
        return -2;
    }

    if (fseek(fOut, 0, SEEK_SET) != 0 || fread(regC, sizeof(regConfig), 1, fOut) != 1) {
        fprintf(stderr, "\ncreaHash: Error al posicionar el cursor al primer cubo o leer la configuración\n");
        fclose(fIn);
        fclose(fOut);
        return -2;
    }

    tipoReg reg;
    while (fread(&reg, sizeof(tipoReg), 1, fIn) == 1) {
        int res = insertar(fOut, &reg, regC);
        if (res < 0) {
            fclose(fIn);
            fclose(fOut);
            return res; 
        }

       
        int densidad = 100 * regC->numReg / ((regC->nCubos + regC->nCubosDes) * C);
        if (densidad >= 80) {
            break;
        }
    }

    
    fseek(fOut, 0, SEEK_SET);
    fwrite(regC, sizeof(regConfig), 1, fOut);

    fclose(fIn);
    fclose(fOut);
    return 0;
}



int busquedaHash(FILE *fHash, tipoReg *reg, tPosicion *posicion) {
    regConfig config;

    
    fseek(fHash, 0, SEEK_SET);
    if (fread(&config, sizeof(regConfig), 1, fHash) != 1) {
        return -2; 
    }

    
    int hash = funcionHash(reg, config.nCubos);
    long offset = sizeof(regConfig) + hash * sizeof(tipoCubo);

    tipoCubo cubo;
    fseek(fHash, offset, SEEK_SET);
    if (fread(&cubo, sizeof(tipoCubo), 1, fHash) != 1) {
        return -3;  
    }

   
    for (int i = 0; i < cubo.numRegAsignados; i++) {
        if (cmpClave(&cubo.reg[i], reg)) {
            *reg = cubo.reg[i];
            posicion->cubo = hash;
            posicion->cuboDes = -1;
            posicion->posReg = i;
            return 0;
        }
    }

 
    if (cubo.desbordado) {
        for (int i = config.nCubos; i < config.nCubos + config.nCubosDes; i++) {
            offset = sizeof(regConfig) + i * sizeof(tipoCubo);
            fseek(fHash, offset, SEEK_SET);
            if (fread(&cubo, sizeof(tipoCubo), 1, fHash) != 1) {
                return -4; 
            }

            for (int j = 0; j < cubo.numRegAsignados; j++) {
                if (cmpClave(&cubo.reg[j], reg)) {
                    *reg = cubo.reg[j];
                    posicion->cubo = hash;
                    posicion->cuboDes = i;
                    posicion->posReg = j;
                    return 0;
                }
            }
        }
    }

    return -1;  
}

int modificarReg(FILE *fHash, tipoReg *reg, tPosicion *posicion) {
    long offset;

   
    if (posicion->cuboDes == -1) {
        offset = sizeof(regConfig) + posicion->cubo * sizeof(tipoCubo);
    } else {
        offset = sizeof(regConfig) + posicion->cuboDes * sizeof(tipoCubo);
    }

    tipoCubo cubo;

  
    fseek(fHash, offset, SEEK_SET);
    if (fread(&cubo, sizeof(tipoCubo), 1, fHash) != 1) {
        perror("Error leyendo cubo");
        return -1;
    }

    
   

   
    cubo.reg[posicion->posReg] = *reg;

    
    fseek(fHash, offset, SEEK_SET);
    if (fwrite(&cubo, sizeof(tipoCubo), 1, fHash) != 1) {
        perror("Error escribiendo cubo");
        return -2;
    }

    
    fflush(fHash);
  
  #ifdef ALUM
    leeHash("../datos/alumnosC.hash");
#elif defined(ASIG)
    leeHash("../datos/asignaturasC.hash");
#else
    
    printf("Tipo de datos no definido (ni ALUM ni ASIG), no se puede mostrar el contenido.\n");
#endif
    
    return 0;
}




