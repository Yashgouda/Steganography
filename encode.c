#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
/* Main encoding workflow – calls all steps */
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        printf(" INFO: Files opened successfully.\n");
        printf("\n================== Encoding Procedure Started =====================\n\n");
        if(check_capacity(encInfo) == e_success)  // Ensure image can store data
        {
            printf(" INFO: Capacity check passed successfully.\n");
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
            {
                printf(" INFO: Header copied successfully.\n");   
                if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
                {
                    printf(" INFO: Magic string encoded successfully.\n");
                    if(encode_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
                    {
                        printf(" INFO: Encode file extension size successfully.\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
                        {
                            printf(" INFO: Encoded file extension successfully.\n");
                            if(encode_secret_file_size(encInfo->secret_file_size ,encInfo) == e_success)
                            {
                                printf(" INFO: Encoded secret file size successfully.\n");
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf(" INFO: Encoded secret file data successfully.\n");
                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf(" INFO: Remaining image data copied successfully.\n");
                                        fclose(encInfo->fptr_src_image);
                                        fclose(encInfo->fptr_stego_image);
                                        return e_success;
                                    }
                                    else
                                    {
                                        printf("ERROR: Copy remaining data failed.\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("ERROR: Encode secret file data failed.\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("ERROR: Encode secret file size failed.\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("ERROR: Encode secret file extension failed.\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("ERROR: Encode file extension size failed.\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR: Magic string encoding failed.\n");
                    return e_failure;
                }

            }
            else
            {
                printf("ERROR: Copy BMP header failed.\n");
                return e_failure;
            }
        } 
        else
        {
            printf("ERROR: Capacity check failed.\n");
            return e_failure;
        }
        
    }
    else
    {
        printf("opening files failed.\n");
        return e_failure;
    }

}
/* Check if image has enough capacity to store magic string + ext + file size + data */
Status check_capacity(EncodeInfo *encInfo)
{
    
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->secret_file_size =get_file_size(encInfo->fptr_secret);
    int magic_string = strlen(MAGIC_STRING);
    int extension = strlen(encInfo->extn_secret_file);
    /* Total bits required */
    int Encoding_thing = (magic_string + sizeof(int) + extension + sizeof(int) + encInfo->secret_file_size) *8;
    if(encInfo->image_capacity > Encoding_thing)
        return e_success;
    else
        return e_failure;
}
/* Returns size of secret file using fseek + ftell */
uint get_file_size(FILE *fptr)
{
    //finding the size of secret.txt
    if(fptr == NULL)
    {
        perror("error\n");
        return 0;
    }
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);   
}
/* Copy first 54 bytes (BMP header) to output image */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(arr,54,1,fptr_src_image);
    fwrite(arr,54,1,fptr_dest_image);
    return e_success;
}
/* Encode magic string "#*" */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if(encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
/* Encode any byte-wise data into image (8 bytes per character) */
Status encode_data_to_image(const char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char str[8];
    for(int i=0; i<size; i++)
    {
        fread(str,8,1,fptr_src_image);  // Read 8 bytes from image
        encode_byte_to_lsb(data[i],str); // Modify LSBs
        fwrite(str,8,1,fptr_stego_image);// Write back
    }
    return e_success;
}
/* Encode 1 byte into LSB of 8 bytes */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0; i < 8; i++)
    {
        unsigned int bit = (data >> i) & 1 ; // Extract i-th bit
        image_buffer[i] = (image_buffer[i] & ~1 ) | bit ; // Replace LSB
    }
    return e_success;
}
/* Encode size of extension (32 bits) */
Status encode_file_extn_size(int size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str,32,1,encInfo->fptr_src_image);
    encode_int_to_lsb(size,str);
    fwrite(str,32,1,encInfo->fptr_stego_image);
    return e_success;

}
/* Encode integer into 32 bits of LSB */
Status encode_int_to_lsb(int size,char *image_buffer)
{
    for(int i=0; i<32; i++)
    {
        unsigned int bit = (size >> i) & 1;
        image_buffer[i] = (image_buffer[i] & ~1) | bit;
    }
    return e_success;
}
/* Encode the actual extension (.txt) */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if(encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Encode actual secret file size (in bytes) */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str,32,1,encInfo->fptr_src_image);
    encode_int_to_lsb(file_size,str);
    fwrite(str,32,1,encInfo->fptr_stego_image);
    return e_success;
}
/* Read secret file fully into buffer, then encode */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    int i = 0;
    char ch;
    char secret_file_data[encInfo->secret_file_size];
    rewind(encInfo->fptr_secret);  // Move to start of secret.txt
    while((ch = fgetc(encInfo->fptr_secret)) != EOF && i < encInfo->secret_file_size)
    {
        secret_file_data[i] = ch;
        i++;
    } 
    if(encode_data_to_image(secret_file_data, encInfo->secret_file_size ,encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
/* After encoding, copy remaining bytes of image */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;
    while((ch = fgetc(fptr_src)) != EOF)
    {
        fputc(ch, fptr_dest);
    }
    return e_success;
}