#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define NB_ENTRIES 15
#define NB_FILES 24
#define T_FOLDER 10
#define T_FOLDER_MOTION 10
#define T_PATH 200
#define NB_DATA 12
#define NB_MOTIONS 6
#define BUFFER_SIZE 200

#define PATH_DATA "E:\\Phase1\\archive\\A_DeviceMotion_data\\A_DeviceMotion_data\\"
#define PATH_INFO "E:\\Phase1\\archive\\data_subjects_info.csv"

#define TEST_SET "E:\\Phase1\\testSet.csv"
#define DATA_SET "E:\\Phase1\\dataSet.csv"

typedef enum motionType MotionType;
enum motionType {
	DOWNSTAIRS = 1,
	JOGGING = 2,
	UPSTAIRS = 3,
	SITDOWN = 4,
	STANDUP = 5,
	WALKING = 6
};

typedef struct row Row;
struct row {
	int dataNumber;
	double attitudeRoll;
	double attitudePitch;
	double attitudeYaw;
	double gravityX;
	double gravityY;
	double gravityZ;
	double rotationRateX;
	double rotationRateY;
	double rotationRateZ;
	double userAccelerationX;
	double userAccelerationY;
	double userAccelerationZ;
};

typedef struct dataSet DataSet;
struct dataSet {
	int entryNumber;
	double vector[1000];
	MotionType motionType;
	int genre;
};

int setMotionType(char name[]);
void setVectors(char folderPath[], DataSet* dataSet);
void setGenre(char folderPath[], DataSet* dataSet);
void writeSet(char folderPath[], DataSet dataSet);


char needPrep();
bool exists(char folderPath[]);
void prepareSet(char folderPath[]);
void displayFirstLine(char folder[], MotionType motion);



int main(void) {
	char paths[NB_ENTRIES][T_FOLDER] = { "dws_1\\","dws_2\\","dws_11\\","jog_9\\","jog_16\\","sit_5\\","sit_13\\","std_6\\","std_14\\","ups_3\\","ups_4\\","ups_12\\","wlk_7\\","wlk_8\\","wlk_15\\"};
	char currentFolder[T_FOLDER];
	char folderNameMotion[T_FOLDER_MOTION];
	int nbTestSet = 0;
	int nbDataSet = 0;
	int testDataNum = 0;
	MotionType motionType;
	char c;

	c = needPrep();
	if (c == 'n') return 0;
	//getchar();

	// Chaque dossier du tableau paths
	for (int iFolder = 0; iFolder < NB_ENTRIES; iFolder++) {
		// set motionType
		strcpy_s(currentFolder, sizeof(paths[iFolder]), paths[iFolder]);
		strncpy_s(folderNameMotion, sizeof(currentFolder), currentFolder, 3);
		motionType = setMotionType(folderNameMotion);

		system("cls");
		
		displayFirstLine(currentFolder, motionType);
		// Chaque fichier sub_ contenu dans le dossier paths[iFolder]
		for (int iFile = 1; iFile <= NB_FILES; iFile++) {
			char folderPath[T_PATH];
			char iFileChar[T_FOLDER_MOTION];
			DataSet dataSet;
			// Copie du chemin d'accès
			strcpy_s(folderPath, sizeof(folderPath), PATH_DATA);
			strcat_s(folderPath, sizeof(folderPath), currentFolder);
			strcat_s(folderPath, sizeof(folderPath), "sub_");
			sprintf_s(iFileChar,sizeof(int), "%d", iFile);
			strcat_s(folderPath, sizeof(folderPath), iFileChar);
			strcat_s(folderPath, sizeof(folderPath), ".csv");

			dataSet.entryNumber = iFile;
			dataSet.motionType = motionType;

			printf("\tPerson numero : %d\t", dataSet.entryNumber);
			setVectors(folderPath, &dataSet);
			setGenre(PATH_INFO, &dataSet);

			if (nbTestSet < (iFolder + 1)*3 && iFile - 1 == testDataNum) {
				writeSet(TEST_SET, dataSet);
				printf("testSet\n");
				nbTestSet++;
				if (testDataNum != NB_FILES-1)
					testDataNum++;
				else
					testDataNum = 0;
			}
			else {
				writeSet(DATA_SET, dataSet);
				printf("dataSet\n");
				nbDataSet++;
			}
		}
	}
	system("cls");
	do {
		printf("Nombre total de fichiers : %d\nNombre de fichier par nouveaux fichier :\n\tData set : %d\n\tTest set : %d\n\nPourcentage de data dans TestSet : %.2f%%\nAppuyez sur ENTER pour continuer...", nbDataSet+nbTestSet, nbDataSet,nbTestSet, (nbTestSet / ((double)nbDataSet + nbTestSet))*100);
		c = getchar();
	} while (c != '\n');
	
	return 0;
}



void displayFirstLine(char folder[], MotionType motion) {
	char type[15] = { "" };
	switch (motion) {
	case DOWNSTAIRS :
		strcpy_s(type, 15*sizeof(char), "DOWNSTAIRS");
		break;
	case JOGGING :
		strcpy_s(type, 15 * sizeof(char), "JOGGING");
		break;
	case UPSTAIRS:
		strcpy_s(type, 15 * sizeof(char), "UPSTAIRS");
		break;
	case SITDOWN:
		strcpy_s(type, 15 * sizeof(char), "SITDOWN");
		break;
	case STANDUP:
		strcpy_s(type, 15 * sizeof(char), "STANDUP");
		break;
	case WALKING:
		strcpy_s(type, 15 * sizeof(char), "WALKING");
		break;
	}
	printf("Dossier courant : %s\nMotion Type : %s\t%sDossier\n", folder, type, (motion == DOWNSTAIRS ? "" : "\t"));
}

MotionType setMotionType(char name[]) {
	if (strcmp(name, "dws") == 0)
		return DOWNSTAIRS;
	else if (strcmp(name, "jog") == 0)
		return JOGGING;
	else if (strcmp(name, "ups") == 0)
		return UPSTAIRS;
	else if (strcmp(name, "sit") == 0)
		return SITDOWN;
	else if (strcmp(name, "std") == 0)
		return STANDUP;
	else if (strcmp(name, "wlk") == 0)
		return WALKING;
	else
		return 0;
}

void setVectors(char folderPath[] , DataSet* dataSet) {
	char caracLu;
	Row currentRow;
	FILE* fiData;

	fopen_s(&fiData, folderPath, "r");

	if (fiData == NULL) {
		printf("Fichier inexistant.\n");
	}
	else {
		int iVector = 0;
		char tempData[30] = {""};
		char* tempStr = NULL;
		int nbData = 0;

		//Premiere ligne à négliger
		do {
			fread_s(&caracLu, sizeof(caracLu), sizeof(caracLu), 1, fiData); //lecture crac par carac pour trouver les , qui séparent les diff element
																			// sauter la ligne avec les titres
		} while (caracLu != '\n');

		while (!feof(fiData) && iVector < 1000) {
			// Les autres lignes avec les données
			fread_s(&caracLu, sizeof(caracLu), sizeof(caracLu), 1, fiData);
			if (caracLu == '\n' || caracLu == ',') {
				// Ajout de la valeur dans la variable Row
				switch (nbData) {
				case 0:
					currentRow.dataNumber = atoi(tempData);
					break;
				case 1:
					currentRow.attitudeRoll = strtod(tempData, &tempStr);
					break;
				case 2:
					currentRow.attitudePitch = strtod(tempData, &tempStr);
					break;
				case 3:
					currentRow.attitudeYaw = strtod(tempData, &tempStr);
					break;
				case 4:
					currentRow.gravityX = strtod(tempData, &tempStr);
					break;
				case 5:
					currentRow.gravityY = strtod(tempData, &tempStr);
					break;
				case 6:
					currentRow.gravityZ = strtod(tempData, &tempStr);
					break;
				case 7:
					currentRow.rotationRateX = strtod(tempData, &tempStr);
					break;
				case 8:
					currentRow.rotationRateY = strtod(tempData, &tempStr);
					break;
				case 9:
					currentRow.rotationRateZ = strtod(tempData, &tempStr);
					break;
				case 10:
					currentRow.userAccelerationX = strtod(tempData, &tempStr);
					break;
				case 11:
					currentRow.userAccelerationY = strtod(tempData, &tempStr);
					break;
				case 12:
					// Traitement fin de ligne et numéro de ligne
					currentRow.userAccelerationZ = strtod(tempData, &tempStr);
					(*dataSet).vector[iVector] = sqrt(pow(currentRow.userAccelerationX, 2) + pow(currentRow.userAccelerationY, 2) + pow(currentRow.userAccelerationZ, 2));

					fread_s(&caracLu, sizeof(caracLu), sizeof(caracLu), 1, fiData);
					currentRow.dataNumber = atoi(&caracLu);

					iVector++;
				}
				strcpy_s(tempData, sizeof(char), "");

				if (nbData == NB_DATA) nbData = 0;
				else nbData++;
			}
			else {
				strncat_s(tempData,sizeof(tempData), &caracLu,1);
			}
					
		}
		fclose(fiData);
	}
}

void setGenre(char folderPath[], DataSet* dataSet) {
	char caracLu;
	char tempData[30] = { "" };
	char* tempStr = NULL;
	int indexEnCours;
	int nbData = 0;

	FILE* fiInfo;

	fopen_s(&fiInfo, folderPath, "r");

	if (fiInfo == NULL) {
		printf("Fichier inexistant.\n");
	}
	else {
		//Premiere ligne à négliger
		do {
			fread_s(&caracLu, sizeof(caracLu), sizeof(caracLu), 1, fiInfo); //lecture crac par carac pour trouver les , qui séparent les diff element
																			// sauter la ligne avec les titres
		} while (caracLu != '\n');

		while (!feof(fiInfo)) {
			//trouver la personne
			if (caracLu == '\n') {
				fread_s(&caracLu, sizeof(caracLu), sizeof(caracLu), 1, fiInfo);
				indexEnCours = atoi(&caracLu);
			}
			fread_s(&caracLu, sizeof(caracLu), sizeof(caracLu), 1, fiInfo);
			if ((*dataSet).entryNumber == indexEnCours) {
				if (caracLu == '\n' || caracLu == ',') {
					if (nbData == 4) {
						(*dataSet).genre = atoi(tempData);
						fread_s(&caracLu, sizeof(caracLu), sizeof(caracLu), 1, fiInfo);
						indexEnCours = atoi(&caracLu);
						nbData = 0;
					} else nbData++;
					strcpy_s(tempData, sizeof(char), "");
				}
				else {
					strncat_s(tempData, sizeof(tempData), &caracLu, 1);
				}
			}
		}

		fclose(fiInfo);
	}
}

char needPrep() {
	char c;
	char e = 'y';
	do {
		if (!exists(DATA_SET) && !exists(TEST_SET)) {
			printf("Les fichiers n'existent pas \nEn cours de creation...");
			prepareSet(DATA_SET);
			prepareSet(TEST_SET);
		}
		else {
			do {
				printf("Les fichiers sont deja present !\nContinuer ? (y/n)");
				e = getchar();
			} while (e != 'y' && e != 'n');
		}
		printf("Appuyez sur ENTER pour continuer...");
		c = getchar();
	} while (c != '\n');

	return e;
}

void prepareSet(char folderPath[]) {
	FILE* fiData;
	//char writtenCarac;
	char toWrite[BUFFER_SIZE] = { "movement,genre,index,Vacc\n" };

	fopen_s(&fiData, folderPath, "a");

	if (fiData == NULL) {
		printf("Fichier inexistant.\n");
	}
	else {
		fwrite(&toWrite, 27, 1, fiData);
		fclose(fiData);
	}
}

bool exists(char folderPath[]) {
	FILE* fiPrep;

	fopen_s(&fiPrep, folderPath, "r");
	if (fiPrep != NULL) {
		fclose(fiPrep);
	}
	return fiPrep != NULL;
}

void writeSet(char folderPath[], DataSet dataSet) {
	FILE* fiData;
	char toWrite[BUFFER_SIZE] = { "" };
	int charCount;

	fopen_s(&fiData, folderPath, "a");

	if (fiData == NULL) {
		printf("Fichier inexistant.\n");
	}
	else {
		int iVector = 0;
		char tmp;
		// Rassembler mov,genre, index en une variable
		// Conersion movement en string et ajout a toWrite
		for (int iMotion = 0; iMotion < NB_MOTIONS; iMotion++) {
			if (dataSet.motionType == iMotion+1) {
				charCount = sprintf_s(toWrite, BUFFER_SIZE, "%d,", iMotion+1);
				iMotion = NB_MOTIONS - 1;
			}
		}
		// Conersion index en string et ajout a toWrite
		tmp = dataSet.genre == 0 ? 'm' : 'f';
		charCount += sprintf_s(toWrite + charCount, BUFFER_SIZE - charCount, "%c,", tmp);
		// Conersion genre en string et ajout a toWrite
		charCount += sprintf_s(toWrite + charCount, BUFFER_SIZE - charCount, "%d,", dataSet.entryNumber);
		// Ecriture des 3 premieres data dans la fichier => charCount reçois le nombre total de caractère dans le string
		fwrite(&toWrite, sizeof(char), charCount, fiData);

		// Ajout de tous les vecteur Vacc
		while (dataSet.vector[iVector] != 0 && iVector < 1000) {
			charCount = sprintf_s(toWrite, BUFFER_SIZE, "%f%s", dataSet.vector[iVector],(dataSet.vector[iVector + 1] != 0 && iVector < 999 ? "," : ""));
			fwrite(&toWrite, sizeof(char), charCount, fiData);
			iVector++;
		}
		fwrite("\n", sizeof(char), 1, fiData);
		
		fclose(fiData);
	}
}