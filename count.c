/*
delete pointer refrences
change file pointers to binary mode
*/


#include <stdio.h>
#include <string.h>

typedef struct {
	int fileSize;
	int stringOccurenceNum;
} FileData;

//Knuth-morris-pratt implementaion
void fillLpsArray(const char* searchString, int* lps) {

	int len = strnlen(searchString, 1000);
	int preLen = 0;

	//since intial char will have no match
	lps[0] = 0;

	int i = 1;

	//preLen is incremented only when prefix match happens
	while (i < len) {
		if (searchString[i] == searchString[preLen]) {
			preLen++;
			lps[i] = preLen;
			i++;
		}
		else {
			if (preLen > 0) {
				preLen = lps[preLen - 1];
			}
			else {
				lps[i] = 0;
				i++;
			}
		}
	}
}

int countSearchStringMatches(const char* searchString, char *searchSpace) {
	int len = strnlen(searchString, 1000);
	int searchSpaceLen = strnlen(searchSpace, 1000);

	int *lps = (int*)malloc(sizeof(int)*len);
	fillLpsArray(searchString, lps);

	int count = 0;

	if (searchSpace == NULL) {
		printf("Issue in opening the input file\n");
		return 0;
	}
	int i = 0;
	int j = 0;

	/*Loop for reading the file till end*/
	while (i < searchSpaceLen) {

		if (searchSpace[i] == searchString[j]) {
			i++;
			j++;

			//full string match found
			if (j == len) {
				count++;
				j = lps[j - 1];
			}
		}
		else if (searchString[j] != searchSpace[i]) {

			//go to last matching prefix
			if (j > 0) {
				j = lps[j - 1];
			}
			else {
				//if already at 0th position in longest common prefix, then 
				//move the file pointer to next char
				i++;
			}
		}
	}

	return count;
}

//TBD: pass buffer chunks instead of reading single chars
int kmpNumMathes(const char* searchString) {

	int len = strnlen(searchString, 1000);
	int *lps = (int*)malloc(sizeof(int)*len);
	fillLpsArray(searchString, lps);

	int count = 0;
	long fileSize;

	printf("string size %d\n", len);

	FILE *searchSpace = fopen("example1", "r");

	if (searchSpace == NULL) {
		printf("Issue in opening the input file\n");
		return 0;
	}
	int sCounter = 0;
	int fCounter = 0;

	/*Loop for reading the file till end*/
	while (1) {
		char c[10];

		size_t read = fread(c, 1, 1, searchSpace);

		if (read != 1) {
			break;
		}

		//printf("%s   %c\n",c,searchString[sCounter]);

		if (c[0] == searchString[sCounter]) {
			sCounter++;
			fCounter++;

			//full string match found
			if (sCounter == len) {
				count++;
				sCounter = lps[sCounter - 1];
			}
		}
		else if (searchString[sCounter] != c[0]) {

			//go to last matching prefix
			if (sCounter > 0) {
				sCounter = lps[sCounter - 1];
			}
			else {
				//if already at 0th position in longest common prefix, then 
				//move the file pointer to next char
				fCounter++;
			}
		}

		fseek(searchSpace, fCounter, SEEK_SET);
	}

	printf("%d\n", count);

	fileSize = ftell(searchSpace);
	printf("File Size is %d bytes.\n", fileSize);

	/*Closing the input file after reading*/

	fclose(searchSpace);

	return count;
}

FileData* passFileBuffers(char* searchString, char* inputFile) {

	unsigned int bufferSize = 100;
	int count = 0;
	FileData *fileData = (FileData*)malloc(sizeof(FileData));

	FILE *filePtr = fopen(inputFile, "r");

	if (filePtr == NULL) {
		printf("Issue in opening the input file\n");
		return 0;
	}

	//get File size

	fseek(filePtr, 0, SEEK_END);
	unsigned long fileSize = (unsigned long)ftell(filePtr);
	long remFileSize = fileSize;

	fseek(filePtr, 0, SEEK_SET);
	int k = 0;
	char *prevBufferSuffix = (char*)malloc(sizeof(char)*bufferSize);

	/*Loop for reading the file till end*/
	while (1) {
		//printf("reading file\n");
		char *fileBuf = (char*)malloc(sizeof(char)*bufferSize);

		int bufSize = remFileSize >= bufferSize ? bufferSize : remFileSize;
		if (remFileSize <= 0) {
			break;
		}
		else {

			//printf("buf size: ", bufSize);
			size_t read = fread(fileBuf, bufSize, 1, filePtr);
			//printf("%s\n", fileBuf);
			//printf("reading file 3\n");
			//fileBuf[bufSize-1] = '\0';
			//printf("reading file2\n");

			printf("%s\n", fileBuf);
			printf("**************\n");
			count += countSearchStringMatches(searchString, fileBuf);
		}
		k++;
		remFileSize = remFileSize - bufSize;

		if (k > 1) {

			unsigned int searchLen = strnlen(searchString, 1000);
			int i, j;
			char preSufSearchSpace[100];
			int preBufLen = strnlen(prevBufferSuffix, 1000);
			for (i = 0; i < searchLen - 1; i++) {
				preSufSearchSpace[i] = prevBufferSuffix[preBufLen - searchLen + 1 + i];
			}
			//printf("partial string: %s  ",preSufSearchSpace);
			for (j = 0; j < searchLen - 1; j++) {
				preSufSearchSpace[i + j] = fileBuf[j];
				//printf(":%c", fileBuf[j]);
			}
			printf("\n");
			printf("full string:   %s\n", preSufSearchSpace);
			preSufSearchSpace[2 * searchLen - 2] = '\0';
			count = count + countSearchStringMatches(searchString, preSufSearchSpace);
		}
		prevBufferSuffix = fileBuf;
		free(fileBuf);
	}

	fileData->fileSize = ftell(filePtr);
	fileData->stringOccurenceNum = count;
	printf("File Size is %d bytes.\n", fileData->fileSize);

	fclose(filePtr);

	return fileData;
}

//string matching using brute-force (naive algo)
int fileIO(const char* searchString) {

	int count = 0;
	int k = 0;
	long fileSize;

	const int searchLen = strlen(searchString);
	FILE *searchSpace = fopen("example4", "r");

	if (searchSpace == NULL) {
		printf("Issue in opening the input file\n");
		return 0;
	}

	/*Loop for reading the file till end*/
	while (1) {
		char subStr[100];

		/*using fgets
			subStr = (char*)malloc((searchLen+2));
			subStr = fgets(subStr, searchLen+1, searchSpace);
		*/

		size_t read = fread(subStr, (searchLen), 1, searchSpace);
		subStr[searchLen] = '\0';
		if (read != 1 || subStr == NULL) {
			break;
		}
		else if (strlen(subStr) == searchLen) {

			if (strcmp(searchString, subStr) == 0) {
				///printf("matches\n");
				count++;
			}
			else {
				//printf("no match\n");

			}
		}
		k++;
		fseek(searchSpace, k, SEEK_SET);
	}

	fileSize = ftell(searchSpace);
	printf("File Size is %d bytes.\n", fileSize);

	/*Closing the input file after reading*/

	fclose(searchSpace);
	return count;
}

int writeOutputToFile(FileData* fileData, char* searchString, char* outputFile) {
	FILE * wFilePtr;

	int fileLength = snprintf(NULL, 0, "File size is: %d \nNumber of occurences of \"%s\": %d\n", fileData->fileSize, searchString, fileData->stringOccurenceNum);
	printf("%d\n", fileLength);
	char *writeBuffer = (char*)malloc(sizeof(char)*(fileLength + 1));
	snprintf(writeBuffer, fileLength + 1, "File size is: %d \nNumber of occurences of \"%s\": %d\n", fileData->fileSize, searchString, fileData->stringOccurenceNum);
	wFilePtr = fopen(outputFile, "a");

	if (wFilePtr == NULL) {
		perror("Error opening/creating file: ");
		return 0;
	}

	fwrite(writeBuffer, sizeof(char), fileLength, wFilePtr);

	fclose(wFilePtr);
	return 1;
}

int main() {

	int outputWritten = 0;
	char searchString[100] = "potato";
	char inputFile[100];
	char outputFile[100];

	int k = 0;
	int count = 0;

	printf("Enter seach string:  ");
	scanf("%s", searchString);

	printf("Enter input file name: ");
	scanf("%s", inputFile);

	printf("Enter output file name: ");
	scanf("%s", outputFile);


	FileData *fileData = passFileBuffers(searchString, inputFile);

	printf("There are %d %s in the file", fileData->stringOccurenceNum, searchString);

	//function to write data to new file
	outputWritten = writeOutputToFile(fileData, searchString, outputFile);

	if (outputWritten) {
		printf("Output succesfully written at %s", outputFile);
	}
	else {
		printf("Some error occured");
	}

	return 0;
}