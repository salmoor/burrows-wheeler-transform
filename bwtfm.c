/**
 * Author: Alemdar Salmoor
 * Date: October 20, 2019
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <string.h>


void computeMatch(char * L, int textSize, int * rank, int ** occ, int * sa, char * P, int patternSize, char * pName, char * tName){

    

    bool found = true;
    int start = 0;
    int end = textSize - 1;
    int curRank;
    int from;
    int to;
    int range;
    int index;

    char nuc;
    int cnt = patternSize - 1;
    while (cnt >= 0)
    {
        nuc = P[cnt];
        if(nuc == 'A'){ 
            curRank = rank[0];
            from = curRank + occ[start][0];
            to = curRank + (occ[end][0] - 1);
        }
        else if(nuc == 'C'){
            curRank = rank[1];
            from = curRank + occ[start][1];
            to = curRank + (occ[end][1] - 1);
        }
        else if(nuc == 'G'){
            curRank = rank[2];
            from = curRank + occ[start][2];
            to = curRank + (occ[end][2] - 1);
        }
        else if(nuc == 'T'){
            curRank = rank[3];
            from = curRank + occ[start][3];
            to = curRank + (occ[end][3] - 1);
        }
        
        start = from - 1; end = to;

        if((to - from) < 0){ found = false; break; }
        cnt--;
    }
    
    

    cnt = 1;
    if(found){
        range = to - from + 1;
        index = from;

        printf("Pattern %s found in %s %d time(s) at position(s):\n", pName, tName, range);
        while (index <= to)
        {
            printf("\tpos %d: %d\n", cnt, sa[index]);
            cnt++;
            index++;
        }
    }
    else{
        printf("Pattern %s is not found in %s\n", pName, tName);
    }


}

void generateRotations(char ** R, int textSize){
    int i = 1;
    int j = 0;

    while (i < textSize)
    {
        while (j < textSize - 1)
        {
            R[i][j] = R[i-1][j+1];
            j++;
        }
        R[i][j] = R[i-1][0];
        j = 0;
        i++; 
    }
    
}

int Partition(char *** A, int p, int r){
    char * x = (*A)[r];
    int i = p - 1;
    int res;

    int j = p;
    char * temp;
    char * Aj;
    while (j < r)
    {
        Aj = (*A)[j];
        res = strcmp(Aj, x);
        if(res <= 0){
            //printf("(*A)[j]: %s is less tham or equal to x: %s\n", Aj, x);
            i++;
            temp = (*A)[i];
            (*A)[i] = (*A)[j];
            (*A)[j] = temp;
        }
        j++;
    }

    temp = (*A)[i + 1];
    (*A)[i + 1] = (*A)[r];
    (*A)[r] = temp;

    return i + 1;
}

void QuickSort(char *** A, int p, int r){
    int q;
    if(p < r) {
        q = Partition(A, p, r);
        QuickSort(A, p, q -1);
        QuickSort(A, q + 1, r);
    }  
    
}

void generateIndex(char** argv){
    char * text = argv[2];
    char nucleotide = '0';

    FILE * ftext = fopen(text, "r");
    int textSize = 0;
    int textCapacity = 1;
    char * textS = malloc(sizeof(char));
    int rank[4];
    
    
    //count the number of each Character
    int numsA = 0, numsC = 0, numsG = 0, numsT = 0;


    while (fscanf(ftext, "%c", &nucleotide) > 0)
    {
        //Remove the line with <
        if (nucleotide == '>'){
            while (fscanf(ftext, "%c", &nucleotide) > 0){
                if (nucleotide == '\n'){
                    break;
                }
            }
        }

        if(nucleotide != '\n'){
            if(textSize >= textCapacity)
            {
                textS = realloc(textS, textCapacity * 2);
                textCapacity = textCapacity * 2;
            }
            textS[textSize] = nucleotide;
            textSize++;
        }
    }
    fclose(ftext);
    //Here we have the text
    if(textSize >= textCapacity){ textS = realloc(textS, textCapacity + 1); textCapacity = textCapacity + 1; }

    //Add $ sign
    textS[textSize] = '$';
    textSize++;

    int cnt = 0;

    // Use this to print the text
    // printf("The text is: ");
    // while (cnt < textSize)
    // {
    //     printf("%c",textS[cnt]);
    //     cnt++;
    // }
    // printf("\n");

    char ** R = malloc(textSize * sizeof(char *));
    cnt = 0;
    while (cnt < textSize)
    {
        R[cnt] = malloc(textSize * sizeof(char));
        cnt++;        
    }

    cnt = 0;
    while (cnt < textSize)
    {
        R[0][cnt] = textS[cnt];
        cnt++;        
    }

    generateRotations(R, textSize);
    QuickSort(&R, 0, textSize - 1);

    // Use this to print the rotations
    // cnt = 0;
    // int cnt2 = 0;
    // printf("The Rotations are:\n");
    // while (cnt < textSize)
    // {
    //     while (cnt2 < textSize)
    //     {
    //         printf(" %c ",R[cnt][cnt2]);
    //         cnt2++;
    //     }
    //     printf("\n");
    //     cnt2 = 0;
    //     cnt++;        
    // }
    // printf("\n");

    //Create output file for bwt
    size_t len = strlen(text);
    char outbwt[len + 5];
    strcpy(outbwt, text);
    strcat(outbwt, ".bwt");

    //create the output file for fm index
    char outfm[len + 4];
    strcpy(outfm, text);
    strcat(outfm, ".fm");


    //Output the last column to the file
    FILE *ouf = fopen(outbwt, "w+");
    FILE *ouf2 = fopen(outfm, "w+");
    cnt = 0;
    int incnt;
    char nuc;
    char lastRankNuc = '$';
    char fnuc;
    int * sa = malloc(textSize * sizeof(int));
    
    while (cnt < textSize)
    {

        //F part
        fnuc = R[cnt][0];
        if(fnuc != lastRankNuc){

            if(fnuc == 'A'){ rank[0] = cnt; lastRankNuc = fnuc; }
            else if(fnuc == 'C'){ rank[1] = cnt; lastRankNuc = fnuc; }
            else if(fnuc == 'G'){ rank[2] = cnt; lastRankNuc = fnuc; }
            else if(fnuc == 'T'){ rank[3] = cnt; lastRankNuc = fnuc; }

        }

        //L part
        nuc = R[cnt][textSize-1];

        //allocate space
        //Increment the occurence of the nucleotide
        if(nuc == 'A'){ numsA++; }
        else if(nuc == 'C'){ numsC++; }
        else if(nuc == 'G'){ numsG++; }
        else if(nuc == 'T'){ numsT++; }

        //Print the rows of Occ table
        fprintf(ouf2, "%d, %d, %d, %d\n", numsA, numsC, numsG, numsT);


        //print into the bwt
        fprintf(ouf, "%c", nuc);

        incnt = 0;
        //Work on suffix array entry
        while (incnt < textSize)
        {
            if(R[cnt][incnt] == '$'){ sa[cnt] = textSize - incnt;  break;}
            incnt++;
        }

        cnt++;      
    }

    cnt = 0;
    while (cnt < textSize)
    {
        fprintf(ouf2, "%d\n", sa[cnt]);
        cnt++;
    }
    fprintf(ouf2, "%d, %d, %d, %d\n", rank[0], rank[1], rank[2], rank[3]);


    fclose(ouf2);
    fclose(ouf);


    //Free memory
    free(textS);

    cnt = 0;
    while (cnt < textSize)
    {
        free(R[cnt]);
        cnt++;        
    }
    free(R);

    free(sa);


}

void search(char** argv){


    //Obtain the text name
    char * text = argv[2];
    char letter = '0';

    char * pattern = argv[4];
    char nucleotide = '0';

    FILE * ftext = fopen(text, "r");
    int textSize = 0;
    int textCapacity = 1;
    char * textS = malloc(sizeof(char));

    while (fscanf(ftext, "%c", &letter) > 0)
    {
        if (letter == '>'){
            while (fscanf(ftext, "%c", &letter) > 0){
                if (letter == '\n'){
                    break;
                }
                if(textSize >= textCapacity)
                {
                    textS = realloc(textS, textCapacity * 2);
                    textCapacity = textCapacity * 2;
                }
                textS[textSize] = letter;
                textSize++;
            }
        }
        break;
    }
    fclose(ftext);
    //printf("textName is: %s\n", textS);

    //Obtain the pattern
    FILE * fpattern = fopen(pattern, "r");
    int patternSize = 0, pNameSize = 0;
    int patternCapacity = 1, pNameCapacity = 1;
    char * patternS = malloc(sizeof(char));
    char * pName = malloc(sizeof(char));
    int cnt;

    while (fscanf(fpattern, "%c", &nucleotide) > 0)
    {
        
        if (nucleotide == '>'){
            while (fscanf(fpattern, "%c", &nucleotide) > 0){
                if (nucleotide == '\n'){
                    break;
                }
                if (pNameSize >= pNameCapacity)
                {
                    pName = realloc(pName, pNameCapacity * 2);
                    pNameCapacity = pNameCapacity * 2;
                }
                pName[pNameSize] = nucleotide;
                pNameSize++;
            }
        }

        if(nucleotide != '\n'){
            if (patternSize >= patternCapacity)
            {
                patternS = realloc(patternS, patternCapacity * 2);
                patternCapacity = patternCapacity * 2;
            }
            patternS[patternSize] = nucleotide;
            patternSize++;
        }

    }

    fclose(fpattern);

    //printf("pattern is: %s\n", patternS); 
    
    // cnt = 0;
    // printf("The pattern is: ");
    // while (cnt < patternSize)
    // {
    //     printf("%c", patternS[cnt]);
    //     cnt++;
    // }
    // printf("\n");

    //Create input file for bwt
    size_t len = strlen(text);
    char inbwt[len + 5];
    strcpy(inbwt, text);
    strcat(inbwt, ".bwt");

    
    FILE * finbwt = fopen(inbwt, "r");
    int bwtSize = 0;
    int bwtCapacity = 1;
    char * bwtS = malloc(sizeof(char));

    //Read in Bwt Column
    while (fscanf(finbwt, "%c", &nucleotide) > 0)
    {
        if (bwtSize >= bwtCapacity)
        {
            bwtS = realloc(bwtS, bwtCapacity * 2);
            bwtCapacity = bwtCapacity * 2;
        }
        bwtS[bwtSize] = nucleotide;
        bwtSize++;
    }

    // cnt = 0;
    // printf("The Bwt is: ");
    // while (cnt < bwtSize)
    // {
    //     printf("%c", bwtS[cnt]);
    //     cnt++;
    // }
    // printf("\n");

    // printf("bwt size is: %d\n", bwtSize);
    fclose(finbwt);


    //create the input file for fm index
    char infm[len + 4];
    strcpy(infm, text);
    strcat(infm, ".fm");
    FILE * finfm = fopen(infm, "r");


    //Read in occ table
    cnt = 0;
    int inputA, inputC, inputG, inputT;
    int ** occ = malloc(bwtSize * sizeof(int *));
    
    while (cnt < bwtSize)
    {
        fscanf(finfm, "%d, %d, %d, %d\n", &inputA, &inputC, &inputG, &inputT);

        //allocate space
        occ[cnt] = malloc(4 * sizeof(int));
        occ[cnt][0] = inputA; occ[cnt][1] = inputC; occ[cnt][2] = inputG;  occ[cnt][3] = inputT;

        cnt++;
    }

    // Use this to print the occurrence table
    // printf("The Occ is\n");
    // cnt = 0;
    // while (cnt < bwtSize)
    // {
    //     printf("A: %d, ", occ[cnt][0]);
    //     printf("C: %d, ", occ[cnt][1]);
    //     printf("G: %d, ", occ[cnt][2]);
    //     printf("T: %d", occ[cnt][3]);
    //     printf("\n");
    //     cnt++;
    // }
    // printf("\n");


    //Read in SA
    cnt = 0;
    int input;
    int * sa = malloc(bwtSize * sizeof(int));

    while (cnt < bwtSize)
    {
        fscanf(finfm, "%d\n", &input);

        //allocate space
        sa[cnt] = input;

        cnt++;
    }

    // Use this to print suffix array
    // printf("The SA\n");
    // cnt = 0;
    // while (cnt < bwtSize)
    // {
    //     printf("%d: %d\n", cnt, sa[cnt]);
    //     cnt++;
    // }
    // printf("\n");

    //Read in rank
    int rank[4];
    fscanf(finfm, "%d, %d, %d, %d\n", &rank[0], &rank[1], &rank[2], &rank[3]);

    // Use this to print rank table
    // printf("The rank is: ");
    // cnt = 0;
    // while (cnt < 4)
    // {
    //     printf("%d: %d, ", cnt, rank[cnt]);
    //     cnt++;
    // }
    // printf("\n");

    fclose(finfm);

    occ[0][0] = occ[0][1] = occ[0][2] = occ[0][3] = 0;
    

    computeMatch(bwtS, bwtSize, rank, occ, sa, patternS, patternSize, pName, textS);

    //Free Memory
    free(textS);
    free(patternS);
    free(pName);
    free(bwtS);

    cnt = 0;
    while (cnt < bwtSize)
    {
        free(occ[cnt]);
        cnt++;
    }

    free(occ);
    free(sa);

}

int main(int argc, char** argv){

    char * option = argv[1];

    if(strcmp(option, "--index") == 0) {
        struct timeval t1, t2;
        double elapsedTime;
        gettimeofday(&t1, NULL);
        
        generateIndex(argv);

        gettimeofday(&t2, NULL);
        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000; //sec to us
        elapsedTime += (t2.tv_usec - t1.tv_usec);   // add microseconds
        printf("Index generated in %.0lf microseconds\n", elapsedTime);

    }
    else
    {
        struct timeval t1, t2;
        double elapsedTime; 
        gettimeofday(&t1, NULL);

        search(argv);

        gettimeofday(&t2, NULL);
        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000000; //sec to us
        elapsedTime += (t2.tv_usec - t1.tv_usec);   // add microseconds
        printf("Search completed in %.0lf microseconds\n", elapsedTime);
    }

    return 0;
    
}