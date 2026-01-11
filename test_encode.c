#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char* argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    if((argc < 3 || argc > 5))               //argument count should be between 3 to 5
    {
        printf("........................................................................\n\n");
        printf("Usage:\n");
        printf("Encode: ./a.out -e <src_image.bmp> <secret_file> <stego_image.bmp>\n");
        printf("Decode: ./a.out -d <stego_image.bmp> [output_file_name]\n");
        printf("\n........................................................................\n\n");
        return 0;
    }
    /* Determine if operation is encode or decode */
    if ((check_operation_type(argv)) == e_encode)
    {
        /* Encode needs 3 or 4 arguments */
        if(argc<4 || argc>5)
        {
            printf("Encoding:./lsb_steg -e <.bmp file> <.txt file> [output file] \n");
            printf("Decoding:./lsb_steg -d <.bmp file> [output file] \n");
            return e_failure;
        }
        printf("\n==================== SELECTED ENCODING OPERATION =============================\n");
        /* Validate encode arguments */
        if(read_and_validate_encode_args(argv,&encInfo) == e_success)
        {
            printf("read and validate is succes\n");
            /* Perform encoding */
            if(do_encoding(&encInfo) == e_success)
            {
                printf("\n===================== Data encoded successfully =======================\n");
                
            }
            else
            {
                printf("Encoding failed\n");
            }
        }        
    
    }
    else if((check_operation_type(argv)) == e_decode)
    {
        /* Decode needs 2 or 3 arguments */
        if(argc < 3  || argc > 4)
        {
            printf("Encoding:./lsb_steg -e <.bmp file> <.txt file> [output file] \n");
            printf("Decoding:./lsb_steg -d <.bmp file> [output file] \n");
            return e_failure;
        }
        printf("\n================ SELECTED DECODING OPERATION ===================\n");
        /* Validate decode arguments */
        if(read_and_validate_decode_args(argv,&decInfo) == e_success)
        {
            /* Perform decoding */
            if(do_decoding(&decInfo) == e_success)
            {
                printf("\n================  Data decoded successfully ===============\n");

            }
            else
            {
                printf("ERROR : Decoding failed.\n");
            }
        }
    }
    
    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
        return e_encode;
    else if(strcmp(argv[1],"-d")==0)
        return e_decode;
    else
        return e_unsupported;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //checking bmp file
    char *ret = strstr(argv[2],".bmp");
    if(ret == NULL)
    {
        printf("ERROR: Wrong extension! Expected a .bmp file for source image.\n");
        return e_failure;
    }
    else if(strcmp(ret,".bmp")!=0)
    {
        printf("ERROR: Wrong extension! Expected a .bmp file for source image.\n");
        return e_failure;
    }
    else
    {
        printf("INFO: Source image has valid .bmp extension.\n");
        encInfo->src_image_fname = argv[2];
    }
    // checking .txt file
    char *res = strstr(argv[3],".txt");
    if(res == NULL)
    {
        printf("ERROR: Wrong extension! Expected a .txt file for secret data.\n");
        return e_failure;
    }
    else if(strcmp(res,".txt")!=0)
    {
        printf("ERROR: Wrong extension! Expected a .txt file for secret data.\n");
        return e_failure;
    }
    else
    {
        printf("Secret file has valid .txt extension.\n");
        encInfo->secret_fname = argv[3]; //putting full txt file into secret_fname
        strcpy(encInfo->extn_secret_file, res); //putting only .txt
    }

    if(argv[4] == NULL)
    {
        printf("No output file provided. Defaulting to stego.bmp\n");
        encInfo->stego_image_fname = "stego.bmp";
    }
    else
    {
        if(strstr(argv[4],".bmp")==NULL)
        {
            printf("ERROR: Output file must have .bmp extension.\n");
            return e_failure;
        }
        else
        {
            printf("INFO: Output file has valid .bmp extension.\n");
            encInfo->stego_image_fname = argv[4];
        }
    }
    printf("Arguments validated successfully\n\n");
    return e_success;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *ret = strstr(argv[2],".bmp");
    if(ret == NULL)
    {
        printf(" ERROR: Invalid stego image extension! Expected a .bmp file.\n");
        return e_failure;
    }
    else if(strcmp(ret,".bmp")!=0)
    {
        printf(" ERROR: Invalid stego image extension! Expected a .bmp file.\n");
        return e_failure;
    }
    else
    {
        
        printf(" INFO: Stego image has valid .bmp extension.\n");
        decInfo->stego_image_fname = argv[2];
    }

    if(argv[3] == NULL)
    {
        printf(" INFO: No output filename provided. Using default: output\n");
        decInfo->optr = "output";
    }
    else
    {
        printf(" INFO: Output file set to: %s\n", argv[3]);
        decInfo->optr = argv[3];
    }
    if(argv[3] == NULL)
    {
        printf(" INFO: No output filename provided. Using default: output\n");
        decInfo->optr = malloc(20);
        strcpy(decInfo->optr, "output");
    }
    else
    {
        printf(" INFO: Output file set to: %s\n", argv[3]);
        decInfo->optr = malloc(strlen(argv[3]) + 10);
        strcpy(decInfo->optr, argv[3]);
    }
    printf(" Arguments validated successfully\n\n");
    return e_success;
}