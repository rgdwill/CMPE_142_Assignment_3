#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MEM_INDEX 100

typedef struct {
	int memList_JobID_Page[MEM_INDEX][2];
	char *memList_Action[MEM_INDEX];
} memList;

typedef struct {
	int processID[MEM_INDEX];
	int physicalPageAllocation[MEM_INDEX][MEM_INDEX];
} page_tables;

typedef struct {
	int processAllocation[20];
	int virtualPageAllocation[20];
	int allocationOrder[20];
} physical;

void formatLine(char *str);
void replaceTabs(char *str);
void parseLine(char *inputLine, char* outputValues[]);
void memorySortFIFO(memList memListFIFO, page_tables page_tables_FIFO, physical physical_FIFO, int index);
int checkProcessCreation(memList memListFIFO, page_tables page_tables_FIFO, int index, int processIDIndex);
int checkForAllocation(memList memListFIFO, page_tables page_tables_FIFO, physical physical_FIFO, int index);
//void sortJobList(char *selectSort, int jobList[][3], int jobListSize);
//void FIFOscheduling(int jobList[][3], int FIFOjobs[][5], int jobListSize);
//void SJFscheduling(int jobList[][3], int SJFjobs[][5], int jobListSize);
//void BJFscheduling(int jobList[][3], int BJFjobs[][5], int jobListSize);

//void SJFschedulingTest(int jobList[][3], int jobListSorted[100][3], int jobListSize);

int main(int argc, char **argv) {
	FILE *fp;
	char *line = NULL;
	size_t linesize = 0;
	ssize_t linelen;
	char* mem[3];
	//char* memList[100][2];
	
	//int memListSorted[100][3];
	int memIndex = 0;
	//int FIFOjobs[100][5];
	//int SJFjobs[100][5];
	//int BJFjobs[100][5];
	//int STCFjobs[100][5];
	//int RRjobs[100][5];

	memList memListInput;
	memList memListFIFO;
	page_tables page_tables_FIFO;
	physical physical_FIFO;

	for(int i = 0; i < MEM_INDEX; i++) {
		memListInput.memList_JobID_Page[i][1] = -1;
	}

	if(argc == 2) {
		fp = fopen(argv[1], "r");
		if(fp == NULL) exit(EXIT_FAILURE);
		while((linelen = getline(&line, &linesize, fp)) != -1) {
			if(strncmp("exit", line, 4) == 0) {
				exit(0);
			}
			formatLine(line);
			replaceTabs(line);
			parseLine(line, mem);
			memListInput.memList_JobID_Page[memIndex][0] = atoi(mem[0]);
			memListInput.memList_Action[memIndex] = mem[1];
			if(mem[2] != NULL) memListInput.memList_JobID_Page[memIndex][1] = atoi(mem[2]);
			if(memIndex < MEM_INDEX) memIndex++;
		}
		for(int i = 0; i < MEM_INDEX; i++) {
			memListFIFO.memList_JobID_Page[i][0] = memListInput.memList_JobID_Page[i][0];
			memListFIFO.memList_Action[i] = memListInput.memList_Action[i];
			memListFIFO.memList_JobID_Page[i][1] = memListInput.memList_JobID_Page[i][1];
		}
		memorySortFIFO(memListFIFO, page_tables_FIFO, physical_FIFO, memIndex);
		for(int i = 0; i < memIndex; i++) {
			if(memListFIFO.memList_JobID_Page[i][1] != -1) {
				printf("mem[0] = %i, mem[1] = %s, mem[2] = %i\n",
					memListFIFO.memList_JobID_Page[i][0], memListFIFO.memList_Action[i],
					memListFIFO.memList_JobID_Page[i][1]);
			} else {
				printf("mem[0] = %i, mem[1] = %s", memListFIFO.memList_JobID_Page[i][0], memListFIFO.memList_Action[i]);
			}
		}

		/*sortJobList("fifo", memList, jobIndex);
		FIFOscheduling(memList, FIFOjobs, jobIndex);

		sortJobList("sjf", memList, jobIndex);
		SJFscheduling(memList, SJFjobs, jobIndex);

		sortJobList("bjf", memList, jobIndex);
		BJFscheduling(memList, BJFjobs, jobIndex);

		printf("\nFIFO Scheduling:\nJob ID:\tStart:\tFinish:\tTotal:\tResponse:\n");
		for(int i = 0; i < jobIndex; i++) {
			printf("%i\t%i\t%i\t%i\t%i\n", FIFOjobs[i][0], FIFOjobs[i][1], FIFOjobs[i][2], FIFOjobs[i][3], FIFOjobs[i][4]);
		}

		printf("\nSJF Scheduling:\nJob ID:\tStart:\tFinish:\tTotal:\tResponse:\n");
		for(int i = 0; i < jobIndex; i++) {
			printf("%i\t%i\t%i\t%i\t%i\n", SJFjobs[i][0], SJFjobs[i][1], SJFjobs[i][2], SJFjobs[i][3], SJFjobs[i][4]);
		}

		printf("\nBJF Scheduling:\nJob ID:\tStart:\tFinish:\tTotal:\tResponse:\n");
		for(int i = 0; i < jobIndex; i++) {
			printf("%i\t%i\t%i\t%i\t%i\n", BJFjobs[i][0], BJFjobs[i][1], BJFjobs[i][2], BJFjobs[i][3], BJFjobs[i][4]);
		}*/
	}
}

void formatLine(char *str) {
	char *src, *dst;
	char *space = " ";
	char *tab = "\t";
	int skip = 1;

	for(src = dst = str; *src != '\0'; src++) {
		*dst = *src;
		if((*dst != *space && *dst != *tab) || skip == 1) {
			if(*dst == *space || *dst == *tab) {
				skip = 0;
			} else {
				skip = 1;
			}
			dst++;
		}
	}
	*dst = '\0';
}

void replaceTabs(char *str) {
	char *src, *dst;
	char *space = " ";
	char *tab = "\t";

	for(src = dst = str; *src != '\0'; src++) {
		*dst = *src;
		dst++;
		if(*dst == *tab) *dst = *space;
	}
	*dst = '\0';
}

void parseLine(char *inputLine, char* outputValues[]) {
	int parseIndex = 0;
	char *subLine = (char *)malloc(10);
	char *lineToParse = (char *)malloc(10);
	strcpy(lineToParse, inputLine);

	subLine = strsep(&lineToParse, " ");
	outputValues[0] = subLine;
	
	subLine = strsep(&lineToParse, " ");
	outputValues[1] = subLine;

	subLine = strsep(&lineToParse, " ");
	outputValues[2] = subLine;
}

void memorySortFIFO(memList memListFIFO, page_tables page_tables_FIFO, physical physical_FIFO, int index) {
	int processIDIndex = 0;
	for(int i = 0; i < index; i++) {
		if(strncmp("C", memListFIFO.memList_Action[i], 1) == 0) {
			if(checkProcessCreation(memListFIFO, page_tables_FIFO, index, i) == 1) {
				page_tables_FIFO.processID[processIDIndex++] = memListFIFO.memList_JobID_Page[i][0];
			}
			printf("process: %i\n", page_tables_FIFO.processID[processIDIndex - 1]);
		} /*else if(strncmp("T", memListFIFO.memList_Action[i], 1) == 0) {

		}*/ else if(strncmp("A", memListFIFO.memList_Action[i], 1) == 0) {
			
		} else if(strncmp("R", memListFIFO.memList_Action[i], 1) == 0) {

		} else if(strncmp("W", memListFIFO.memList_Action[i], 1) == 0) {

		} else if (strncmp("F", memListFIFO.memList_Action[i], 1) == 0) {

		}
	}
}

int checkProcessCreation(memList memListFIFO, page_tables page_tables_FIFO, int index, int ProcessIDIndex) {
	for(int i = 0; i < index; i++) {
		if(page_tables_FIFO.processID[i] == memListFIFO.memList_JobID_Page[ProcessIDIndex][0]) {
			return 0;
		}
	}
	return 1;
}

/*
void sortJobList(char *selectSort, int memList[][3], int memListSize) {
	int i, j, k;
	int finalTime = 0;
	int key[3];
	for(i = 1; i < memListSize; i++) {
		key[0] = memList[i][0];
		key[1] = memList[i][1];
		key[2] = memList[i][2];
		j = i - 1;
		k = j;
		
		if(strncmp("job_id", selectSort, 6) == 0) {
			while(j >= 0 && memList[j][0] > key[0]) {
				memList[j + 1][0] = memList[j][0];
				memList[j + 1][1] = memList[j][1];
				memList[j + 1][2] = memList[j][2];
				j = j - 1;
			}
			memList[j + 1][0] = key[0];
			memList[j + 1][1] = key[1];
			memList[j + 1][2] = key[2];
		} else if(strncmp("fifo", selectSort, 4) == 0) {
			while(j >= 0 && memList[j][1] > key[1]) {
				memList[j + 1][0] = memList[j][0];
				memList[j + 1][1] = memList[j][1];
				memList[j + 1][2] = memList[j][2];
				j = j - 1;
			}
			memList[j + 1][0] = key[0];
			memList[j + 1][1] = key[1];
			memList[j + 1][2] = key[2];
		} else if(strncmp("sjf", selectSort, 3) == 0) {
			while(j >= 0 && memList[j][1] > key[1]) {
				memList[j + 1][0] = memList[j][0];
				memList[j + 1][1] = memList[j][1];
				memList[j + 1][2] = memList[j][2];
				j = j - 1;
			}
			memList[j + 1][0] = key[0];
			memList[j + 1][1] = key[1];
			memList[j + 1][2] = key[2];
			while(k >= 0 && memList[k][1] == memList[k + 1][1] && memList[k][2] > key[2]) {
				memList[k + 1][0] = memList[k][0];
				memList[k + 1][1] = memList[k][1];
				memList[k + 1][2] = memList[k][2];
				k = k - 1;
			}
			memList[k + 1][0] = key[0];
			memList[k + 1][1] = key[1];
			memList[k + 1][2] = key[2];
		} else if(strncmp("bjf", selectSort, 3) == 0) {
			while(j >= 0 && memList[j][1] > key[1]) {
				memList[j + 1][0] = memList[j][0];
				memList[j + 1][1] = memList[j][1];
				memList[j + 1][2] = memList[j][2];
				j = j - 1;
			}
			memList[j + 1][0] = key[0];
			memList[j + 1][1] = key[1];
			memList[j + 1][2] = key[2];
			while(k >= 0 && memList[k][1] == memList[k + 1][1] && memList[k][2] < key[2]) {
				memList[k + 1][0] = memList[k][0];
				memList[k + 1][1] = memList[k][1];
				memList[k + 1][2] = memList[k][2];
				k = k - 1;
			}
			memList[k + 1][0] = key[0];
			memList[k + 1][1] = key[1];
			memList[k + 1][2] = key[2];
		}
	}
}

void FIFOscheduling(int jobList[][3], int FIFOjobs[][5], int jobListSize) {
	int startTime = jobList[0][1];
	int finishTime = 0;
	for(int i = 0; i < jobListSize; i++) {
		FIFOjobs[i][0] = jobList[i][0];
		//Start Time For Each Job
		if(i == 0) {
			FIFOjobs[i][1] = jobList[i][1];
		} else {
			if(startTime < jobList[i][1]) {
				FIFOjobs[i][1] = jobList[i][1];				
			} else {
				FIFOjobs[i][1] = startTime;
			}
		}
		//Finish Time For Each Job
		finishTime = startTime + jobList[i][2];
		FIFOjobs[i][2] = finishTime;
		if(startTime < jobList[i][1]) {
			startTime = jobList[i][1] + jobList[i][2];
		} else {
			startTime = startTime + jobList[i][2];
		}
		//Total Time Elapsed For Each Job
		FIFOjobs[i][3] = finishTime - jobList[i][1];
		//Response Time For Each Job
		if(i == 0) {
			FIFOjobs[i][4] = jobList[i][1];
		} else {
			FIFOjobs[i][4] = startTime - jobList[i][1];
		}
	}
}

void SJFscheduling(int jobList[][3], int SJFjobs[][5], int jobListSize) {	
	int startTime = jobList[0][1];
	int finishTime = 0;
	for(int i = 0; i < jobListSize; i++) {
		SJFjobs[i][0] = jobList[i][0];
		//Start Time For Each Job
		if(i == 0) {
			SJFjobs[i][1] = jobList[i][1];
		} else {
			if(startTime < jobList[i][1]) {
				SJFjobs[i][1] = jobList[i][1];				
			} else {
				SJFjobs[i][1] = startTime;
			}
		}
		//Finish Time For Each Job
		finishTime = startTime + jobList[i][2];
		SJFjobs[i][2] = finishTime;
		if(startTime < jobList[i][1]) {
			startTime = jobList[i][1] + jobList[i][2];
		} else {
			startTime = startTime + jobList[i][2];
		}
		//Total Time Elapsed For Each Job
		SJFjobs[i][3] = finishTime - jobList[i][1];
		//Response Time For Each Job
		if(i == 0) {
			SJFjobs[i][4] = jobList[i][1];
		} else {
			SJFjobs[i][4] = startTime - jobList[i][1];
		}
	}
}

void BJFscheduling(int jobList[][3], int BJFjobs[][5], int jobListSize) {
	int startTime = jobList[0][1];
	int finishTime = 0;
	for(int i = 0; i < jobListSize; i++) {
		BJFjobs[i][0] = jobList[i][0];
		//Start Time For Each Job
		if(i == 0) {
			BJFjobs[i][1] = jobList[i][1];
		} else {
			if(startTime < jobList[i][1]) {
				BJFjobs[i][1] = jobList[i][1];				
			} else {
				BJFjobs[i][1] = startTime;
			}
		}
		//Finish Time For Each Job
		finishTime = startTime + jobList[i][2];
		BJFjobs[i][2] = finishTime;
		if(startTime < jobList[i][1]) {
			startTime = jobList[i][1] + jobList[i][2];
		} else {
			startTime = startTime + jobList[i][2];
		}
		//Total Time Elapsed For Each Job
		BJFjobs[i][3] = finishTime - jobList[i][1];
		//Response Time For Each Job
		if(i == 0) {
			BJFjobs[i][4] = jobList[i][1];
		} else {
			BJFjobs[i][4] = startTime - jobList[i][1];
		}
	}
}*/
