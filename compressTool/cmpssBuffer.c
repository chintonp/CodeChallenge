#include <stdint.h>
#include "cmpssBuffer.h"
#include <stdio.h>

#ifdef _DEBUG
#include <inttypes.h>
#endif

uint8_t bitBuffer = 0;
int pointer;
FILE *fout = NULL;
extern char *outputFile;
extern uint8_t freqTableSize;
extern bool readPrefixTable;
extern uint64_t fileLen;
extern htree_node *ht_root;

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

uint32_t invertByteOrder (const uint32_t ui32)
{
    uint32_t invertOrder = 0;

    for (int i = 0; i < 4; i++) {
        uint32_t tmp = ui32;

        tmp >>= (8 * i);
        tmp &=255;
        invertOrder <<= 8;
        invertOrder |= tmp;
    }
    return invertOrder;
}

int saveBuffer(void)
{
    /*if (pointer < 31) {
        for (int i = 31; i >= pointer; i--) {
            bitBuffer <<= 1;
        }
    }

    uint32_t invertOrder = invertByteOrder(bitBuffer);
    */
    /*for (int i = 0; i < 4; i++) {
        uint32_t tmp = bitBuffer;

        tmp >>= (8 * i);
        tmp &=255;
        invertOrder <<= 8;
        invertOrder |= tmp;
        //printUI32AsBinary(invertOrder);
    }*/
    int ret = fwrite(&bitBuffer, sizeof(bitBuffer), 1, fout);

    if (ret != 1) return CMPSSBUFFER_WRITE_ERROR;
    return CMPSSBUFFER_SUCCESS;
}

int saveBit(unsigned bit) {
    bitBuffer = bitBuffer << 1;
    pointer++;
    if (bit) bitBuffer |= 1;
    else bitBuffer &= ~(1);
    if (pointer == 8) {
        int ret = saveBuffer();

        if (ret != CMPSSBUFFER_SUCCESS) return ret;
        pointer = 0;
        bitBuffer = 0;
    }
    return CMPSSBUFFER_SUCCESS;
}

int save8(uint8_t b8)
{
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

    if (ret != CMPSSBUFFER_SUCCESS) return ret;

    int len = 64 / 8;

    //printf("len: %d\n", len);

    //printf("Len %" PRIu64 "\n", fileLen);
    for (int i = 0; i < len; i++) {
        uint8_t byte = (fileLen >> ((len - i - 1) * 8)) & 255;

        //printf("%d - ", i);
        //printByteAsBinary(byte);
        ret = save8(byte);
        if (ret != CMPSSBUFFER_SUCCESS) return ret;
    }

    ret = save8(freqTableSize);
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
    //return save8('!');
    return CMPSSBUFFER_SUCCESS;
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

    while(freq[idx] != 0) {
        //printf("%d ", idx);
        if (freq[idx] == '0') {
            ret = saveBit(0);
            //printf("0");
            if (ret != CMPSSBUFFER_SUCCESS) return ret;
        } else if (freq[idx] == '1') {
            ret = saveBit(1);
            //printf("1");
            if (ret != CMPSSBUFFER_SUCCESS) return ret;
        } else return CMPSSBUFFER_FREQ_TABLE_INTERNAL_ERROR;
        idx++;
    }
    //printf("!\n");
    return CMPSSBUFFER_SUCCESS;
}

void prepareExtractPrefixTable(void)
{
    bitBuffer = 0;
    pointer = 8;
}

int readBit(FILE *fin, uint8_t *bit)
{
    if (pointer == 8) {
        int ret = fread(&bitBuffer, sizeof(bitBuffer), 1, fin);

        //bitBuffer = invertByteOrder(bitBuffer);
        //printf(">> ");
        //printByteAsBinary(bitBuffer);

        if (ret != 1) {
            if (feof(fin)) return CMPSSBUFFER_READ_EOF;
        }
        pointer = 0;
    }
    uint8_t temp8 = bitBuffer;

    temp8 >>= (7 - pointer);
    temp8 &= 1;
    if (temp8 == 1) *bit = 1;
    else *bit = 0;
    pointer ++;
    return CMPSSBUFFER_SUCCESS;
}

int read8(FILE *fin, uint8_t *p_ui8)
{
    uint8_t ui8 = 0;

    for (int i = 0; i < 8; i++) {
        //printByteAsBinary(ui8);
        uint8_t bit;
        int ret = readBit(fin, &bit);

        ui8 <<= 1;
        if (ret != CMPSSBUFFER_SUCCESS) return ret;
        if (bit == 1) ui8 |= 1;
        //printf("%d - ", bit);
    }
    *p_ui8 = ui8;
    //printByteAsBinary(ui8);
    return CMPSSBUFFER_SUCCESS;
}

int readPrefixCode(FILE *fin, uint8_t letter)
{
    bool ft_end = false;
    char prefix[256] = "\0";
    int pointer = 0;

    while (!ft_end) {
        uint8_t bit1, bit2;
        int ret = readBit(fin, &bit1);

        if (ret != CMPSSBUFFER_SUCCESS) return ret;
        //printf("%d - ", bit1);
        ret = readBit(fin, &bit2);
        //printf("%d - ", bit2);
        if (ret != CMPSSBUFFER_SUCCESS) return ret;
        if ((bit1 == 1 && bit2 == 1) || (bit1 == 0 && bit2 == 0)) prefix[pointer++] = '0' + bit1;
        else if (bit1 == 1 && bit2 == 0) {
            prefix[pointer] = 0;
            ft_end = true;
            if (readPrefixTable) printf("%s\n", prefix);
            else {
                ret = createPrefixNode(letter, prefix);
                if (ret != HT_SUCCESS) {
                    printf("Not enoygh memory!\n");
                    return CMPSSBUFFER_NOT_ENOYGH_MEMORY;
                }
            }
        }
        else {
            printf("\nCorruped file\n");
            return CMPSSBUFFER_CORRUPTED_FILE;
        }
    }
    return CMPSSBUFFER_SUCCESS;
}

int readFreqTable(FILE *fin)
{
    int freqLen = (freqTableSize) ? freqTableSize : 256;

    //printf("Len: %d\n",freqLen);
    for (int i = 0; i < freqLen; i++) {
        uint8_t ui8;
        int ret = read8(fin, &ui8);

        if (ret != CMPSSBUFFER_SUCCESS) return ret;
        if (readPrefixTable) {
            if (ui8 >= 32 && ui8 <= 126) {
                printf("'%c' : ", ui8);
            }
            else {
                // For non-printable characters, print their ASCII value
                printf("ASCII %d : ", ui8);
            }
        }
        ret = readPrefixCode(fin, ui8);
        if (ret != CMPSSBUFFER_SUCCESS) return ret;
    }
    return CMPSSBUFFER_SUCCESS;
}

int readLetter(FILE *fin, uint8_t *p_ui8)
{
    uint8_t bit;
    int ret;
    htree_node *node = ht_root;
    bool foundLetter = false;

    if (!node) return CMPSSBUFFER_FREQ_TABLE_INTERNAL_ERROR;
    if (fileLen == 0) return CMPSSBUFFER_READ_EOF;
    while (!foundLetter) {
        ret = readBit(fin, &bit);
        if (ret != CMPSSBUFFER_SUCCESS) return ret;
        //printf("Bit: %d\n", bit);
        if (bit == 0) node = node -> leftNode;
        else node = node -> rightNode;

        if (!node -> leftNode && !node -> rightNode) { // It's a leafNode
            *p_ui8 = node -> letter;
            foundLetter = true;
        }
    }
    //printf(" - ASCII %d [%c]\n", *p_ui8, *p_ui8);
    fileLen--;
    return CMPSSBUFFER_SUCCESS;
}
