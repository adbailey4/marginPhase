/*
 * Copyright (C) 2017 by Benedict Paten (benedictpaten@gmail.com)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#include "stRPHmm.h"

/*
 * Functions for profile sequence
 */

stProfileSeq *stProfileSeq_constructEmptyProfile(char *referenceName, int64_t referenceStart,
        int64_t length) {
    /*
     * Creates an empty profile sequence, with all the profile probabilities set to 0.
     */
    stProfileSeq *seq = st_malloc(sizeof(stProfileSeq));
    seq->referenceName = stString_copy(referenceName);
    seq->refStart = referenceStart;
    seq->length = length;
    seq->profileProbs = st_calloc(length*ALPHABET_SIZE, sizeof(uint8_t));
    return seq;
}

void stProfileSeq_destruct(stProfileSeq *seq) {
    /*
     * Cleans up memory for profile sequence.
     */
    free(seq->profileProbs);
    free(seq->referenceName);
    free(seq);
}

float getProb(uint8_t *p, int64_t characterIndex) {
    /*
     * Gets probability of a given character as a float.
     */
    return ((float)p[characterIndex])/255;
}

void stProfileSeq_print(stProfileSeq *seq, FILE *fileHandle, bool includeProbs) {
    /*
     * Prints a debug representation of a profile sequence.
     */
    char profileString[seq->length+1];
    profileString[seq->length] = '\0';
    for(int64_t i=0; i<seq->length; i++) {
        uint8_t *p = &seq->profileProbs[i * ALPHABET_SIZE];
        float maxProb = getProb(p, 0);
        int64_t maxChar = 0;
        for(int64_t j=1; j<ALPHABET_SIZE; j++) {
            float prob = getProb(p, j);
            if(prob > maxProb) {
                maxProb = prob;
                maxChar = j;
            }
        }
        profileString[i] = maxChar + FIRST_ALPHABET_CHAR;
    }
    fprintf(fileHandle, "\tSEQUENCE REF_NAME: %s REF_START %"
            PRIi64 " REF_LENGTH: %" PRIi64 " ML_STRING: %s\n",
            seq->referenceName, seq->refStart, seq->length, profileString);
    if(includeProbs) {
        for(int64_t i=0; i<seq->length; i++) {
            uint8_t *p = &seq->profileProbs[i * ALPHABET_SIZE];
            // Print individual character probs
            fprintf(fileHandle, "\t\tPOS: %" PRIi64 "", i);
            for(int64_t j=0; j<ALPHABET_SIZE; j++) {
                fprintf(fileHandle, "\t%f\t", getProb(p, j));
            }
            fprintf(fileHandle, "\n");
        }
    }
}

void printSeqs(FILE *fileHandle, stSet *profileSeqs) {
    /*
     * Prints a set of profile seqs.
     */
    stSetIterator *seqIt = stSet_getIterator(profileSeqs);
    stProfileSeq *pSeq;
    while((pSeq = stSet_getNext(seqIt)) != NULL) {
        stProfileSeq_print(pSeq, fileHandle, 0);
    }
    stSet_destructIterator(seqIt);
}

void printPartition(FILE *fileHandle, stSet *profileSeqs1, stSet *profileSeqs2) {
    /*
     * Print a partition.
     */
    fprintf(fileHandle, "First partition\n");
    printSeqs(fileHandle, profileSeqs1);
    fprintf(fileHandle, "Second partition\n");
    printSeqs(fileHandle, profileSeqs2);
}
