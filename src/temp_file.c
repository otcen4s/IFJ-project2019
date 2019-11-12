#include "temp_file.h"

#define GET_TOKEN() \
                do{ \
                    parser->curr_token = read_token(parser->scanner, &err); \
                    if(err) return err; \
                }while(0)

int skip_empty_lines(Parser *parser)
{
    int err;

    GET_TOKEN();
    
    if(parser->curr_token.type == TOKEN_EOL)
    {
        skip_empty_lines(parser);
    }
    return NO_ERROR;   
}