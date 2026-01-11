#include<stdio.h>
#ifndef DECODE_H
#define DECODE_H

#include "types.h"

/* 
 * Structure to store information required for decoding data
 * from the stego image.
 */


typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    //output
    char *optr;
    FILE *out_ptr;
    int size_of_extn;
    int size_of_file;
}DecodeInfo;

/* Function prototypes for decoding */

/* Validate decoding arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform entire decoding process */
Status do_decoding(DecodeInfo *decInfo);

/* Open stego image file */
Status open_img_file(DecodeInfo *decInfo);

/* Skip first 54 bytes of BMP header */
Status skip_bmp_header(DecodeInfo *decInfo);

/* Decode magic string and verify */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extension size (32 bits) */
Status decode_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extension (.txt) */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size (32 bits) */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret data byte-by-byte */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Extract 1 byte from 8 LSBs */
char decode_byte_from_lsb(char *image_buffer);

/* Extract 32-bit integer from LSBs */
int decode_int_from_lsb(char *image_buffer);


#endif