#include <stdint.h>
#include "cmpssBuffer.h"
#include <stdio.h>

uint32_t bitBuffer = 0;
int pointer;
FILE *fout = NULL;
extern char *outputFile;

#ifdef _DEBUG

// This function prints the 8 bits of a single byte
void printByteAsBinary(uint8_t byte) {
    // Iterate from the 7th bit (MSB) down to the 0th bit (LSB)
    for (int i = 7; i >= 0; i--) {
        // Create a mask with a 1 at the current bit position
        unsigned char mask = 1 << i;

        // Use bitwise AND to check if the bit is set in the byte
        if (byte & mask) {
            putchar('1');
        } else {
            putchar('0');
        }
    }
    putchar('\n');
}

void printUI32AsBinary(uint32_t ui32) {
    // Iterate from the 7th bit (MSB) down to the 0th bit (LSB)
    for (int i = 31; i >= 0; i--) {
        // Create a mask with a 1 at the current bit position
        uint32_t mask = 1 << i;

        // Use bitwise AND to check if the bit is set in the byte
        if (ui32 & mask) {
            putchar('1');
        } else {
            putchar('0');
        }
        if (i == 24 || i == 16 || i == 8) putchar(':');
    }
    putchar('\n');
}

#endif // _DEBUG

int saveBuffer(void)
{
    if (pointer < 31) {
        for (int i = 31; i >= pointer; i--) {
            //printUI32AsBinary(bitBuffer);
            bitBuffer <<= 1;
        }
    }
#ifdef _DEBUG
    //printUI32AsBinary(bitBuffer);
#endif

    uint32_t invertOrder = 0;
    for (int i = 0; i < 4; i++) {
        uint32_t tmp = bitBuffer;

        tmp >>= (8 * i);
        tmp &=255;
        invertOrder <<= 8;
        invertOrder |= tmp;
        //printUI32AsBinary(invertOrder);
    }
    int ret = fwrite(&invertOrder, sizeof(invertOrder), 1, fout);

    //printf("fwrite: %d\n", ret);

    if (ret != 1) return CMPSSBUFFER_WRITE_ERROR;
    return CMPSSBUFFER_SUCCESS;
}

int saveBit(unsigned bit) {
    //printf("saveBit\n");
    bitBuffer = bitBuffer << 1;
    pointer ++;
    if (bit) bitBuffer |= 1;
    else bitBuffer &= ~(1);
    if (pointer == 32) {
        int ret = saveBuffer();

        if (ret != CMPSSBUFFER_SUCCESS) return ret;
        pointer = 0;
        bitBuffer = 0;
    }
    return CMPSSBUFFER_SUCCESS;
}

int save8(uint8_t b8)
{
    //printf("save8 - %c\n", b8);
    //printByteAsBinary(b8);

    for (int i = 0; i < 8; i++) {
        int ret;

        if (b8 & 128) ret = saveBit(1);
        else ret = saveBit(0);
        if (ret != CMPSSBUFFER_SUCCESS) return ret;
        b8 = b8 << 1;
    }
    return CMPSSBUFFER_SUCCESS;
}

int prepareBitBuffer(void)
{
    bitBuffer = 0;
    pointer = 0;
    fout = fopen(outputFile, "wb");
    if (fout == NULL) return CMPSSBUFFER_OUTPUT_ERROR;

    int ret = save8('C');
    //printUI32AsBinary(bitBuffer);

    return ret;
}

int saveNode(htree_node *ht_node)
{
    if (ht_node == NULL) return CMPSSBUFFER_SUCCESS;

    int ret;

    if (ht_node -> leftNode) {
        ret = saveNode(ht_node -> leftNode);
        if (ret != CMPSSBUFFER_SUCCESS) return ret;
    }
    if (ht_node -> rightNode) {
        ret = saveNode(ht_node -> rightNode);
        if (ret != CMPSSBUFFER_SUCCESS) return ret;
    }
    if (!ht_node -> leftNode && !ht_node -> rightNode) { // Node leaf
        //printf("Leaf: %c - %s\n", ht_node -> letter, ht_node -> prefixCode);
        ret = save8(ht_node -> letter);
        //printUI32AsBinary(bitBuffer);
        //printf("ret=%d ", ret);

        if (ret != CMPSSBUFFER_SUCCESS) return ret;

        char *p_prefixCode = ht_node -> prefixCode;

        while (*p_prefixCode != 0) {
            if (*p_prefixCode == '1') {
                ret = saveBit(1);
                if (ret != CMPSSBUFFER_SUCCESS) return ret;
                ret = saveBit(1);
                if (ret != CMPSSBUFFER_SUCCESS) return ret;
            } else {
                ret = saveBit(0);
                if (ret != CMPSSBUFFER_SUCCESS) return ret;
                ret = saveBit(0);
                if (ret != CMPSSBUFFER_SUCCESS) return ret;
            }
            //printUI32AsBinary(bitBuffer);
            p_prefixCode++;
        }
        ret = saveBit(1);
        if (ret != CMPSSBUFFER_SUCCESS) return ret;
        ret = saveBit(0);
        if (ret != CMPSSBUFFER_SUCCESS) return ret;
    }
    return CMPSSBUFFER_SUCCESS;
}

int finishSavingPrefixTable(void)
{
    int ret = save8('!');

    if (ret != CMPSSBUFFER_SUCCESS) return ret;
    ret = saveBit(0);
    if (ret != CMPSSBUFFER_SUCCESS) return ret;
    return saveBit(1);
}

int closeBitBuffer(void)
{
    int ret = saveBuffer();

    fclose(fout);
    return ret;
}

int saveFrequency(char *freq)
{
    int idx = 0,
        ret;

    //printf("%s: ", freq);

    while(freq[idx] != 0) {
        //printf("%d ", idx);
        if (freq[idx] == '0') {
            ret = saveBit(0);
            if (ret != CMPSSBUFFER_SUCCESS) return ret;
        } else if (freq[idx] == '1') {
            ret = saveBit(1);
            if (ret != CMPSSBUFFER_SUCCESS) return ret;
        } else return CMPSSBUFFER_FREQ_TABLE_INTERNAL_ERROR;
        idx++;
    }
    //printf("!\n");
    return CMPSSBUFFER_SUCCESS;
}
