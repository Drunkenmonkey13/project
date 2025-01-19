#include<stdio.h>
#include<string.h>
#include"decode.h"
#include"types.h"
#include"common.h"

Status do_decoding(DecodeInfo *decInfo){
    if(decode_open_files(decInfo)==e_success){
        printf("Files open successfully\n");
    }else{
        return e_failure;
    }
    if(decode_magic_string(MAGIC_STRING,decInfo)==e_success){
        printf("Magic string found successfully\n");
    }else{
        return e_failure;
    }
    if(decode_size_secret_extn(decInfo)==e_success){
        printf("Size of secret extension found successfully\n");
    }else{
        return e_failure;
    }
    if(decode_secret_file_extn(decInfo)==e_success){
        printf("Secret file extension found successfully\n");
    }else{
        return e_failure;
    }
    if(create_secret_file(decInfo)==e_success){
        printf("Secret file created successfully\n");
    }else{
        return e_failure;
    }
    if(decode_secret_file_size(decInfo)==e_success){
        //printf("val:%ld\n",decInfo->size_decode_file);        //print the size of secret file
        printf("Size of secret file found successfully\n");
    }else{
        return e_failure;
    }
    if(decode_secret_data(decInfo)==e_success){
        printf("Secret data decoded successfully\n");
    }else{
        return e_failure;
    }
    

    return e_success;
}

Status decode_secret_data(DecodeInfo* decInfo){
    char data[decInfo->size_decode_file];
    for(int i=0;i<decInfo->size_decode_file;i++){
        data[i]=decode_data_from_img(decInfo->fptr_src);
        // printf("%c\n",data[i]);                          //print the decoded char from image
    }
    for(int i=0;i<decInfo->size_decode_file;i++){
        // putc(data[i],decInfo->fptr_decode);
        fprintf(decInfo->fptr_decode,"%c",data[i]);
    }
    return e_success;
}

//decode sizeof secret file
Status decode_secret_file_size(DecodeInfo* decInfo){
    decInfo->size_decode_file=decode_size_from_lsb(decInfo->fptr_src);
    if(decInfo->size_decode_file==0){
        return e_failure;
    }
    return e_success;
}

//create secret file
Status create_secret_file(DecodeInfo* decInfo){
    // char *name_fptr=(decInfo->decode_fname,decInfo->extn_secret_file);
    char file[strlen(decInfo->decode_fname)+strlen(decInfo->extn_secret_file)];
    strcpy(file,decInfo->decode_fname);
    char *name_fptr=strcat(file,decInfo->extn_secret_file);
    decInfo->decode_fname=name_fptr;
    decInfo->fptr_decode=fopen(name_fptr,"w");
    if(decInfo->fptr_decode==NULL){
        return e_failure;
    }
    return e_success;
}

//decode secret file extn
Status decode_secret_file_extn(DecodeInfo* decInfo){
    int i;
    for(i=0;i<decInfo->extn_size;i++){
        decInfo->extn_secret_file[i]=decode_data_from_img(decInfo->fptr_src);
    }
    decInfo->extn_secret_file[i]='\0';
    return strlen(decInfo->extn_secret_file)==decInfo->extn_size? e_success:e_failure;
}

//decode_length_secret_extn
Status decode_size_secret_extn(DecodeInfo* decInfo){
    decInfo->extn_size=decode_size_from_lsb(decInfo->fptr_src);
    // printf("val:%ld\n",decInfo->extn_size);           // check int val(sizeof extn)
    if(decInfo->extn_size==0){
        return e_failure;
    }
    return e_success;
}

// decode and check the magic string 
Status decode_magic_string( char *magic_string, DecodeInfo *decInfo){
    char ch;
    // printf("magic string length:%ld",strlen(magic_string));          //print the length of magic string
    fseek(decInfo->fptr_src,54,SEEK_SET);
    for(int i=0;i<strlen(magic_string);i++){
        ch=decode_data_from_img(decInfo->fptr_src);
        if(ch!=magic_string[i]){
            printf("Magic string not match\n");
            return e_failure;
        }
    }
    return e_success;
}

//decode int val from img
int decode_size_from_lsb(FILE *fptr_src){
    char temp[32];
    int val=0;
    int r_check=fread(temp,sizeof(char),32,fptr_src);
    if(r_check!=32){
        return e_failure;
    }
    for(int i=0;i<32;i++){
        val|=(temp[i]&1)<<31-i;
    }
    //printf("val:%d\n",val);                  // check int val(sizeof extn)
    return val;
}

//fetch data from img and reconstruct the data from the img & return the data
char decode_data_from_img(FILE *fptr){
    char img[8]={0};
    int check_r=fread(img,sizeof(char),8,fptr);
    if(check_r!=8){
        return e_failure;
    }
    return decode_data_from_lsb(img);
}

// seperate the LSB and reconstruct the data from the img & return the data
char decode_data_from_lsb(char *data){
    char c=0x0;
    for(int i=0;i<8;i++){
        c=c|((data[i]&1)<<7-i);
    }
    //printf("d:");for(int i=0;i<8;i++){printf("%d ",(c>>7-i)&1);}printf("\n");   //print after decoded char bin val
    return c;
}

//open input file
Status decode_open_files(DecodeInfo *decInfo){
    decInfo->fptr_src=fopen(decInfo->src_fname,"r");
    if(decInfo->fptr_src==NULL){
        perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_fname);
        return e_failure;
    }
    return e_success;
}

// validate cmd arguments of decode choice
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo){
    if(strstr(argv[2],".bmp")!=NULL){
        if(strcmp(strstr(argv[2],".bmp"),".bmp")==0){
            decInfo->src_fname=argv[2];
        }
    }else{
        printf("Only \".bmp\" files are supported\n");
        return e_failure;
    }
    if(argv[3]==NULL){
        decInfo->decode_fname="output";
    }else{
        decInfo->decode_fname=strtok(argv[3],".");
        //printf("%s",decInfo->decode_fname);           //print the file name passed by cmd arug
    }
    return e_success;
}