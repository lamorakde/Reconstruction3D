#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
FILE *fio, *fir;

int main()
{
	// System parameter
	unsigned char chi;
	int *ior, *iov, *iob, *irr, *irv, *irb;
	int *tmp, *tmp2;
	long int i, j, ic, jc, m, l, dimx, ff, dimy, n, taille, maxr, maxv, maxb;
	long int nbg;
	char entree[35], sortie[35], chaine[10];

	// Algorithm parameter
	double meanRed = 134.8768;
	double meanGreen = 42.2682;
	double distance, distanceThreshold;

	printf("This program is going to extract the red color.\n");
	printf("Please give the input file name without .ppm\n");
	scanf("%s", entree);
	strcat(entree, ".ppm");

	printf("Please give the output file name without .ppm\n");
	scanf("%s", sortie);
	strcat(sortie, ".ppm");


	fio = fopen(entree, "rb");
	fir = fopen(sortie, "wb+");

	if (fio != NULL) printf("ouverture du fichier in\n");
	if (fir != NULL) printf("ouverture du fichier out\n");

	//if(im!=0) printf("allocation de im\n");

	fscanf(fio, "%s\n", chaine);
	fprintf(fir, "%s\n", chaine);


	i = 0;
	while (i < 1)
	{
		chi = fgetc(fio);
		// if((int)chi==6)  fputc((unsigned char)5,fr);
		fputc(chi, fir);
		if (chi == '\n') i++;
	}

	fscanf(fio, "%ld %ld\n", &dimx, &dimy);
	fprintf(fir, "%ld %ld\n", dimx, dimy);


	fscanf(fio, "%d\n", &nbg);
	fprintf(fir, "%d\n", nbg);

	printf("dimx=%d dimy=%d nbg=%d\n", dimx, dimy, nbg);

	ior = (int *)malloc(dimx*dimy*sizeof(int));
	iov = (int *)malloc(dimx*dimy*sizeof(int));
	iob = (int *)malloc(dimx*dimy*sizeof(int));

	irr = (int *)malloc(dimx*dimy*sizeof(int));
	irv = (int *)malloc(dimx*dimy*sizeof(int));
	irb = (int *)malloc(dimx*dimy*sizeof(int));

	tmp = (int *)malloc(dimx*dimy*sizeof(int));
	tmp2 = (int *)malloc(dimx*dimy*sizeof(int));

	if (irb == NULL) printf("allocation iob impossible\n");

	taille = dimx*dimy;
	printf("Taille=%d\n", taille);
	for (i = 0; i < dimy; i++)
		for (j = 0; j < dimx; j++)
		{
			chi = (unsigned char)fgetc(fio);
			ior[i*dimx + j] = (int)chi;
			chi = (unsigned char)fgetc(fio);
			iov[i*dimx + j] = (int)chi;
			chi = (unsigned char)fgetc(fio);
			iob[i*dimx + j] = (int)chi;
		}

	////////////////////////////////////////////////////////////////////////////////////////
	///////////////////          Image Processing Algorithm Begins      ////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

    // This is the algorithm which M.Chapron suggested, but I didn't find a algorithm to pick the best threshold

	distanceThreshold = 50;
	for (i = 0; i < dimy; i++)
	{
		for (j = 0; j < dimx; j++)
		{
			// Calculate the distance of color red and green
			distance = fabs(ior[i*dimx + j] - meanRed) + fabs(iov[i*dimx + j] - meanGreen);
			if (distance < distanceThreshold)
			{
				irr[i*dimx + j] = 255;
				irb[i*dimx + j] = 255;
				irv[i*dimx + j] = 255;
			}
			else
			{
				irr[i*dimx + j] = 0;
				irb[i*dimx + j] = 0;
				irv[i*dimx + j] = 0;
			}
		}
	}

	////////////////////   Output File /////////////////////////////

	for (i = 0; i < dimy; i++)
		for (j = 0; j < dimx; j++)
		{
			fputc((unsigned char)irr[i*dimx + j], fir);
			fputc((unsigned char)irv[i*dimx + j], fir);
			fputc((unsigned char)irb[i*dimx + j], fir);
		}

	free(ior); //...

	fclose(fio); fclose(fir);
	return 0;
}
