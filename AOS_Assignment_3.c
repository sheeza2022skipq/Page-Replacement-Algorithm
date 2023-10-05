
#include<stdio.h>
#include <stdlib.h>
#include <string.h>

struct PageTableEntry
{
    char* pid;
    int frameNo;
    int pageNo;
    int access;
    int offset;
    struct PageTableEntry* next;
};

int searchEntry(struct PageTableEntry* pt, char* pid, int pageNo,int offset)
{

    struct PageTableEntry* current = pt;
    while (current != NULL) {
        if (strcmp(current->pid , pid) == 0 && current->pageNo == pageNo && current->offset == offset)
        {
            return current->access;
        }
        current = current->next;
    }
}

int existingsearchEntry(struct PageTableEntry* pt, char* pid, int pageNo,int offset)
{
    struct PageTableEntry* current = pt;
    while (current != NULL) {
        if (strcmp(current->pid , pid) == 0 && current->pageNo == pageNo && current->offset == offset)
            return current->frameNo;
        current = current->next;
    }
}



void insertEntry(struct PageTableEntry** pt, char* pid, int pageNo,int offset, int frameNo, int access)
{
    // create new entry
    struct PageTableEntry* newEntry;
    newEntry = (struct PageTableEntry*)malloc(sizeof(struct PageTableEntry));
    if (newEntry)
    {
        newEntry->frameNo = frameNo;
        newEntry->pageNo = pageNo;
        newEntry->pid = pid;
        newEntry->offset= offset;
        newEntry->access = access + 1;
        //strcpy(newEntry->entry, entry);
        newEntry->next = NULL;
    }

    if (*pt == NULL)
        *pt = newEntry;
    else {
        // insert the new entry in the end of the page table
        struct PageTableEntry* temp = *pt;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newEntry;
    }
}

void displayPageTable(struct PageTableEntry* pt)
{
    struct PageTableEntry* temp = pt;
    printf("------------------------------------------------------------------------------------------------\n");
    printf(" pid   |      page No      |         offset          |       frameNo      |   access    |   Page\n");
    printf("------------------------------------------------------------------------------------------------\n");
    while (temp != NULL) {
        if(temp->access == 1)
        {
            printf(" %s    |      %5d        |      %9d          |      %5d         |     %2d      |  Fault  \n", temp->pid, temp->pageNo, temp->offset, temp->frameNo, temp->access);
        }
        else
        {
            printf(" %s    |      %5d        |      %9d          |      %5d         |     %2d      |  Hit  \n", temp->pid, temp->pageNo, temp->offset, temp->frameNo, temp->access);
        }
        temp = temp->next;
    }
}

int WriteDisplayOnFile(struct PageTableEntry* pt,int frameSize,int allocatedFrames,int freeFrames)
{
    struct PageTableEntry* temp = pt;
    int hit = 0, miss =0;

    char *filename = "Output.txt";
    FILE *wp = fopen(filename, "w");
    if (wp == NULL)
    {
        printf("Error opening the file %s", filename);
        return -1;
    }
    fprintf(wp,"Final Content Of PageTable:\n");
    fprintf(wp,"------------------------------------------------------------------------------------------------\n");
    fprintf(wp," pid   |      page No      |         offset          |       frameNo      |   access    |   Page\n");
    fprintf(wp,"------------------------------------------------------------------------------------------------\n");
    while (temp != NULL) {
        if(temp->access == 1)
        {
            fprintf(wp," %s    |      %5d        |      %9d          |      %5d         |     %2d      |  Fault  \n", temp->pid, temp->pageNo, temp->offset, temp->frameNo, temp->access);
            miss++;
        }
        else
        {
            fprintf(wp," %s    |      %5d        |      %9d          |      %5d         |     %2d      |  Hit  \n", temp->pid, temp->pageNo, temp->offset, temp->frameNo, temp->access);
            hit++;
        }
        temp = temp->next;
    }
    char pid[2];
    int pgno,offs;
    long physicalAdd = 0;
    printf("If you want to calculate physical address enter PID , PAGENO ,and offset \n");
    printf("Enter Process No : ");
    gets(pid);
    printf("Enter  PageNo : ");
    scanf("%d",&pgno);
    printf("Enter  Page Offset : ");
    scanf("%d",&offs);
    struct PageTableEntry* temp_t = pt;
    while (temp_t != NULL)
    {
        if(strcmp(temp_t->pid , pid) == 0 && temp_t->pageNo == pgno && temp_t->offset >= offs)
        {
            int frame = temp_t->frameNo;
            int pg =  temp_t->pageNo;
            physicalAdd = ( frame * frameSize ) + offs ;
            printf("Physical Address : %d \n",physicalAdd);
            break;
        }
        temp_t = temp_t->next;
    }
    fprintf(wp,"entered PID , PAGENO ,and offset : %s : %d : %d \n",pid, pgno, offs);
    fprintf(wp,"Physical Address : %d \n",physicalAdd);
    fprintf(wp,"Total page hits: %d \n",hit);
    fprintf(wp,"Total page miss: %d \n",miss);
    int Extraframes = freeFrames - allocatedFrames;
    int MemorySize = Extraframes * frameSize;
    fprintf(wp,"Main Memory left in bytes: %dB \n",MemorySize);
    fprintf(wp,"Main Memory left in KB: %dK \n",MemorySize/1024);
    fclose(wp);
    return 0;
}



int Findmax(struct PageTableEntry* pt)
{
    int max = 0;
    struct PageTableEntry* temp = pt;
    while (temp != NULL) {

        if (max < temp->access)
        {
            max = temp->access;
        }
        temp = temp->next;
    }
    return max;
}



int checkpageTable(struct PageTableEntry* pt,int max_v)
{
    struct PageTableEntry* temp = pt;
    while (temp != NULL) {

        if (max_v == temp->access)
        {
            struct PageTableEntry* pre;
            struct PageTableEntry* local;
            local = pt;
            int frame = temp->frameNo;
            if(local->frameNo == frame)
            {
                *pt = *local->next;
                return frame;
            }
            pre = local;
            while(local != NULL)
            {
                if(local->frameNo == frame)
                {
                    pre->next = local->next;
                    return frame;
                }
                pre = local;
                local = local->next;
            }
        }
        temp = temp->next;
    }
}

void deleteframe(struct PageTableEntry* pt,int delframe)
{
            struct PageTableEntry* pre;
            struct PageTableEntry* local;
            local = pt;
            if(local->frameNo == delframe)
            {
                *pt = *local->next;
                return;
            }
            pre = local;
            while(local != NULL)
            {
                if(local->frameNo == delframe)
                {
                    pre->next = local->next;
                    return;
                }
                pre = local;
                local = local->next;
            }

}


char* getField(char*, int);
double getSize(char* );

double physicalMemorySize = 0;
double frameSize = 0;
int noOfFrames = 0;
int freeFrames = 0;
int allocatedFrames = 0;

int PID_COL = 1;
int pageNo_COL = 2;
int offset_COL = 3;

int* physicalMemory = 0;


int main() {


    FILE* filePtr;

    filePtr = fopen("sample data.txt", "r");

    if (NULL == filePtr) {
        printf("file can't be opened \n");
    }
    else {
        int lineNo = 0;

        struct PageTableEntry* pageTable = NULL;

        char line[1024]; // read only 1024 characters in a line
        while (fgets(line, 1024, filePtr))
        {
            if (lineNo == 0) {
                physicalMemorySize = getSize(line);
                printf("physical memory size: %lf\n", physicalMemorySize);

            }

            else if (lineNo == 1) {
                frameSize = getSize(line);
                printf("frame size is: %lf\n", frameSize);
                freeFrames = noOfFrames = physicalMemorySize / frameSize;
                printf("Total frames are: %d\n", noOfFrames);
            }
            else if (lineNo > 1)
            {
                char* pid; int pgNo = 0; int d = 0;
                if (line[0] != '\n') {
                    char* tmp = _strdup(line);
                    pid = getField(tmp, PID_COL);
                    pgNo = atoi(getField(tmp, pageNo_COL));
                    d = atoi(getField(tmp, offset_COL));
                    if ( freeFrames > allocatedFrames)
                    {
                            int access = searchEntry(pageTable,pid,pgNo,d);
                            if(access > 0)
                            {
                                int existingframe = existingsearchEntry(pageTable, pid, pgNo,d);
                                deleteframe(pageTable,existingframe);
                                if (d <= frameSize)
                                {
                                    insertEntry(&pageTable, pid, pgNo,d,existingframe,access);
                                }
                            }
                            else
                            {
                                if (d <= frameSize)
                                {
                                    insertEntry(&pageTable, pid, pgNo,d,allocatedFrames,0);
                                    allocatedFrames++;
                                }

                            }
                    }
                    else
                    {
                            int access = searchEntry(pageTable,pid,pgNo,d);
                            if(access > 0)
                            {
                                int existingframe = existingsearchEntry(pageTable, pid, pgNo,d);
                                deleteframe(pageTable,existingframe);
                                if (d <= frameSize)
                                {
                                    insertEntry(&pageTable, pid, pgNo,d,existingframe,access);
                                }
                            }
                            else
                            {
                                if (d <= frameSize)
                                {
                                int max = Findmax(pageTable);
                                int replacementframe = checkpageTable(pageTable,max);
                                deleteframe(pageTable,replacementframe);
                                insertEntry(&pageTable, pid, pgNo,d,replacementframe ,0);
                                }
                            }
                    }
                }
                printf("After every entry Display PageTable \n");
                displayPageTable(pageTable);
                printf("\n\n");
            }
           // printf("%s", line);
            lineNo++;

            //NOTE strtok clobbers tmp
            //free(tmp);
        }
        WriteDisplayOnFile(pageTable,frameSize,allocatedFrames,freeFrames);
        fclose(filePtr);
    }
    // deallocating the memory
    free(physicalMemory);

}


char* getField(char* line, int num)
{
    // https://www.codingame.com/playgrounds/14213/how-to-play-with-strings-in-c/string-split
    char* temp = (char*)malloc(4 * sizeof(char*));
    if (temp)
        strcpy(temp, line);
    char* tok;
    for (tok = strtok(temp, ", "); // delimeter in the file
        tok && *tok;
        tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

double getSize(char* line)
{
    char* temp = (char*)malloc(4 * sizeof(char*));
    double val = 0;
    char* p;
    if (temp)
    {
        strcpy(temp, line);
        p = temp;
        while (*p) {
            if (isdigit(*p)) {
                val = strtol(p, &p, 10);
            }
            else {
                if (*p == 'k' || *p == 'K') {
                    val = val * 1024;// pow(2, 10);
                    //printf("\nval in if is %lf", val);
                }
                else if (*p == 'm' || *p == 'M')
                    val = val * 4096; //pow(2, 20);

                p++;
            }
        }
        free(temp);
    }

    return val;
}

