#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    E_SUNXI_PARSE_TAG,
    E_SUNXI_PARSE_COLON,
    E_SUNXI_PARSE_DATA,
    E_SUNXI_PARSE_END,
    E_SUNXI_PARSE_FINISH,
}eSUNXI_CPU_PARSE_T, *peSUNXI_CPU_PARSE_T;

static int _sunxi_parse_prepare(char* datap, size_t datan, int* tag_idx, size_t* tag_len, int *data_idx, size_t* data_len)
{
    eSUNXI_CPU_PARSE_T parse_state = E_SUNXI_PARSE_TAG;
    int ii = 0;
    if(' ' == datap[0] || '\n' == datap[0])
    {
        return -1;
    }

    *tag_idx = 0;

    for(ii=0; ii < datan; ii++)
    {
        switch(parse_state)
        {
            case E_SUNXI_PARSE_TAG:
            {
                if('\t' == datap[ii])
                {
                    parse_state = E_SUNXI_PARSE_COLON;
                    *tag_len = ii - 1;
                }
                break;
            }
            case E_SUNXI_PARSE_COLON:
            {
                if(':' == datap[ii] || '(' == datap[ii])
                {
                    parse_state = E_SUNXI_PARSE_DATA;
                }
                break;
            }
            case E_SUNXI_PARSE_DATA:
            {
                if(0x20 != datap[ii])
                {
                    parse_state = E_SUNXI_PARSE_END;
                    *data_idx = ii;
                    // the string only includes the raw string, except for \n
                    *data_len = strlen(&datap[ii]) - 1;
                }
                break;
            }
            default:
            {
                break;
            }
        }

        if(E_SUNXI_PARSE_END == parse_state)
        {
            break;
        }
    }

    return 0;
}

#define STR_TARGET  "Disk"

int main(void)
{
    FILE   *stream;
    char   buf[1024]={STR_TARGET};
//    memset( buf, '\0', sizeof(buf) );
    stream = popen( "fdisk -l", "r" );
//    fread( buf, sizeof(char), sizeof(buf),  stream);
//    printf("------------->\n%s\n<------------------\n",buf);
    while(1)
    {
        int ii = 0;
        char* linep = NULL;
        size_t linen = 0;
        int tag_idx = 0, data_idx = 0;
        size_t tag_len = 0, data_len = 0;

        if(1 == getline(&linep, &linen, stream))
        {
            if('\n' == linep[0] && EOF != linep[1])
            {
                free(linep);
                continue;
            }
            else
            {
                free(linep);
                break;
            }
        }
        else
        {
            if(0 == _sunxi_parse_prepare(linep, linen, &tag_idx, &tag_len, &data_idx, &data_len))
            {
//                for(ii=0; ii<sunxi_parse_units_num; ii++)
//                {
//                    if(0 == sunxi_parse_units[ii].flag)
//                    {
                        if(0 == strncmp(buf, &linep[tag_idx], strlen(buf)-1))
                        {
                                printf("target infor:%s\n",linep);
                                break;
                        }
//                    }
//                }
            }
        }
    }
    pclose( stream );
    return 0;
}

