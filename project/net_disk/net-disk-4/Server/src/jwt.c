#include "../Include/jwt.h"

int getJwt(const char* user, char* token)
{
    char* jwt;
    size_t jwt_length;

    struct l8w8jwt_encoding_params params;
    l8w8jwt_encoding_params_init(&params);

    params.alg = L8W8JWT_ALG_HS512;//算法

    params.sub = "Netdisk";//主题
    params.iss = "overflow";//签发人
    params.aud = "User";//受众

    params.iat = 0;//签发时间
    params.exp = 0x7fffffff;//过期时间

    params.secret_key = (unsigned char*)user;//每个用户不同的内容，需要自己订
    params.secret_key_length = strlen((char*)params.secret_key);

    params.out = &jwt;
    params.out_length = &jwt_length;

    int r = l8w8jwt_encode(&params);

    printf("jwt = %s\n", jwt);
    strncpy(token, jwt, BUFSIZE);

    l8w8jwt_free(jwt);
    return 0;
}

