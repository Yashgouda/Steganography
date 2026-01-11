#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Perform complete decoding process */
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_img_file(decInfo) == e_success) // Open stego image
    {
        printf("INFO : files opened.\n");
        printf("\n============ Decoding Procedure Started ===========\n\n");
        if(skip_bmp_header(decInfo) == e_success)
        {
            printf(" INFO : Skipped bmp header\n");
           if(decode_magic_string(decInfo) == e_success)
            {
               printf(" INFO: Magic string decoded.\n");
               if(decode_file_extn_size(decInfo) == e_success)
                {
                    printf(" INFO: File extension size decoded. \n");   
                    if(decode_secret_file_extn(decInfo) == e_success)
                    {
                        printf(" INFO: File secret file extension size decoded.\n");
                        if(decode_secret_file_size(decInfo) == e_success)
                        {
                            printf(" INFO: secret file size decoded.\n");
                            if(decode_secret_file_data(decInfo) == e_success)
                            {
                                printf(" INFO: secret file data decoded.\n");
                                return e_success;
                            }
                            else
                            {
                                printf("ERROR: Decode secret file data failed.\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("ERROR: Decode secret file size failed.\n");
                            return e_failure;
                        }
                        
                    }
                    else
                    {
                        printf("ERROR: Decode secret file extension size failed.\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR: Decode file extension size failed.\n");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR: Decode Magic string failed.\n");
                return e_failure;
            }
        }
        else
        {
            printf("ERROR: failed to skip bmp header.\n");
            return e_failure;
        }

    }
    else
    {
        printf("ERROR: failed to open image file.\n");
        return e_failure;
    }
}
/* Open stego image in read-binary mode */
Status open_img_file(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname ,"rb");
    if(decInfo->fptr_stego_image == NULL)
    {
        perror("error\n");
        fprintf(stderr,"ERROR : Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    printf("INFO : Opened stego.bmp\n");
    return e_success;
}

/* Skip 54-byte BMP header */
Status skip_bmp_header(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    return e_success;
}

/* Decode and verify magic string "#*" */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[3];
    char image_buffer[9];
    int i;
    for( i = 0;i<strlen(MAGIC_STRING);i++)
    {
        fread(image_buffer,8,1,decInfo->fptr_stego_image);
        magic_string[i] = decode_byte_from_lsb(image_buffer);
    }
    magic_string[i] = '\0';
    if(strcmp(magic_string,MAGIC_STRING) == 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
    
}

/* Decode a single byte from 8 LSBs */
char decode_byte_from_lsb(char *image_buffer)
{
    char ch = 0;
    for(int i=0; i<8; i++)
    {
        unsigned int bit = image_buffer[i] & 1;  // Extract LSB
        ch = ch | (bit << i);          // Rebuild character 
    }
    return ch;
}

/* Decode extension size (stored in 32 bits) */
Status decode_file_extn_size(DecodeInfo * decInfo)
{
    char image_buffer[32];
    fread(image_buffer,32,1,decInfo->fptr_stego_image); //read 32 bytes
    decInfo->size_of_extn = decode_int_from_lsb(image_buffer);
    return e_success;
}

/* Convert 32 LSBs into an integer */
int decode_int_from_lsb(char *image_buffer)
{
    int ch = 0;

    for (int i = 0; i < 32; i++)
    {
        unsigned int bit = image_buffer[i] & 1;  // Extract bit
        ch = ch | (bit << i);                    // Accumulate into integer
    }
    return ch;
}

/* Decode file extension (.txt) */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char Extn[decInfo->size_of_extn+1]; // Store extension
    char image_buffer[8];
    int i;
    for(i = 0; i < decInfo->size_of_extn; i++)
    {
        fread(image_buffer,8,1,decInfo->fptr_stego_image);
        Extn[i] = decode_byte_from_lsb(image_buffer);
    }
    Extn[i] = '\0';
    //printf("\n%s\n",Extn);
    // Append extension ONLY if optr does not already have it
    if (!strstr(decInfo->optr, Extn))
    {
        strcat(decInfo->optr, Extn);
    }
   // strcat(decInfo->optr,Extn);
    //printf("\n%s\n",decInfo->optr);
     /* Open output file with decoded extension */
    decInfo->out_ptr = fopen(decInfo->optr,"wb");
    if(decInfo->out_ptr == NULL)
    {
        printf("fopen failed\n");
        return e_failure;
    }

    return e_success;
}

/* Decode secret file size (32 bits) */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    fread(image_buffer,32,1,decInfo->fptr_stego_image);
    decInfo->size_of_file = decode_int_from_lsb(image_buffer);
    //printf("%d\n",decInfo->size_of_file);
    return e_success;
}

/* Decode secret file actual data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    int i;
    char image_buffer[8];
    for(i=0; i<decInfo->size_of_file; i++)
    {
        fread(image_buffer,8,1,decInfo->fptr_stego_image);  // Read byte data
        ch = decode_byte_from_lsb(image_buffer);          // Convert to character
        fwrite(&ch,1,1,decInfo->out_ptr);              // Write to output file
    }
    return e_success;
}