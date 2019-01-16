#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct {
	uint64_t fileSize;
	uint64_t stringOccurenceNum;
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

uint64_t countSearchStringMatches(const char* searchString, char *searchSpace) {
	int len = strnlen(searchString, 1000);
	int searchSpaceLen = strnlen(searchSpace, 1000);

	int *lps = (int*)malloc(sizeof(int)*len);
	fillLpsArray(searchString, lps);

	uint64_t count = 0;

	if (searchSpace == NULL) {
		printf("Issue in opening the input file\n");
		return 0;
	}
	int i = 0;
	int j = 0;

	/*Loop for reading the file till end*/
	while (i < searchSpaceLen) {

		if (searchSpace[i] == searchString[j] & 0xff) {
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


/*
Function to parse the input file in 1KB chunks

@input string{searchString}, string{}input file name}
@output File data struct containing file size and number of
string matches
*/

FileData* passFileBuffers(char* searchString, char* inputFile) {

	unsigned int bufferSize = 1024;

	uint64_t count = 0;
	FileData *fileData = (FileData*)malloc(sizeof(FileData));
	size_t sLen = strnlen(searchString, 1000);
	size_t read = sLen;

	FILE *filePtr = fopen(inputFile, "rb");
	uint64_t fz = 0;

	if (filePtr == NULL) {
		printf("Issue in opening the input file\n");
		return NULL;
	}
	while (1) {

		char *fileBuf = (char*)malloc(sizeof(char)*bufferSize);

		read = fread(fileBuf, 1, bufferSize, filePtr);

		if (read >= bufferSize) {
			count = count + countSearchStringMatches(searchString, fileBuf);

			//fseek to search string length -1 in case string is broken 
			//between chunks
			fseek(filePtr, -(sLen - 1), SEEK_CUR);
			fz = fz + read - sLen + 1;
		}
		else {
			// reached the last available buffer in file
			char *lastBuf = (char*)malloc(sizeof(char)*read);
			//take only the last available chunk
			memcpy(lastBuf, fileBuf, read);
			count = count + countSearchStringMatches(searchString, lastBuf);
			fz = fz + read;
			break;
		}
		free(fileBuf);
	}

	if (ftell(filePtr) != -1) {
		fileData->fileSize = ftell(filePtr);
	}
	else {
		fileData->fileSize = fz;
	}

	fileData->stringOccurenceNum = count;

	fclose(filePtr);

	return fileData;
}

/*
Function to write output to file
@input - File data struct, string{searchString}, string{Output File}
*/

int writeOutputToFile(FileData* fileData, char* searchString, char* outputFile) {
	FILE * wFilePtr;

	int fileLength = snprintf(NULL, 0, "File size is: %lld \nNumber of occurences of \"%s\": %lld\n", fileData->fileSize, searchString, fileData->stringOccurenceNum);
	//printf("%d\n", fileLength);
	char *writeBuffer = (char*)malloc(sizeof(char)*(fileLength + 1));
	snprintf(writeBuffer, fileLength + 1, "File size is: %lld \nNumber of occurences of \"%s\": %lld\n", fileData->fileSize, searchString, fileData->stringOccurenceNum);
	wFilePtr = fopen(outputFile, "a");

	if (wFilePtr == NULL) {
		perror("Error opening/creating file: ");
		return 0;
	}

	fwrite(writeBuffer, sizeof(char), fileLength, wFilePtr);

	fclose(wFilePtr);
	return 1;
}

int main(int argc, char** argv) {

	int outputWritten = 0;

	if (argc != 4) {
		perror("Please enter correct number of arguments");
		exit(0);
	}

	char *searchString = (char*)malloc(20);
	char *inputFile = (char*)malloc(100);
	char *outputFile = (char*)malloc(100);

	searchString = argv[1];

	inputFile = argv[2];

	outputFile = argv[3];

	FileData *fileData = passFileBuffers(searchString, inputFile);

	printf("There are %lld %s in the file \n", fileData->stringOccurenceNum, searchString);

	//function to write data to new file
	outputWritten = writeOutputToFile(fileData, searchString, outputFile);

	if (outputWritten) {
		printf("Output succesfully written at %s", outputFile);
	}
	else {
		printf("Some error occured");
	}

	free(fileData);

	return 0;
}