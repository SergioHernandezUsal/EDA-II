#include "../include/dispersion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Lee el contenido del fichero hash organizado mediante el método de DISPERSIÓN según los criterios
// especificados en la práctica. Se leen todos los cubos completos tengan registros asignados o no. La
// salida que produce esta función permite visualizar el método de DISPERSIÓN
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
    FILE *f;
    tipoCubo cuboVacio;
    int i;

    // Inicializar campos de configuración
    regC->numReg = 0;
    regC->numRegDes = 0;
    regC->nCuboDesAct = regC->nCubos;

    // Inicializar cubo vacío
    memset(&cuboVacio, 0, sizeof(tipoCubo));
    cuboVacio.numRegAsignados = 0;
    cuboVacio.desbordado = 0;

    // Crear el fichero en modo binario escritura
    f = fopen(fichHash, "wb");
    if (!f) return -2;

    // Escribir estructura de configuración al principio
    if (fwrite(regC, sizeof(regConfig), 1, f) != 1) {
        fclose(f);
        return -2;
    }

    // Escribir cubos primarios vacíos
    for (i = 0; i < regC->nCubos; i++) {
        if (fwrite(&cuboVacio, sizeof(tipoCubo), 1, f) != 1) {
            fclose(f);
            return -2;
        }
    }

    // Escribir cubos de desborde vacíos
    for (i = 0; i < regC->nCubosDes; i++) {
        if (fwrite(&cuboVacio, sizeof(tipoCubo), 1, f) != 1) {
            fclose(f);
            return -2;
        }
    }

    fclose(f);
    return 0;
}