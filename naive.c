#include <stdio.h>

/*
 * input: *len >= 1
 * return utf-8 char length, 0 or error
 */
static const unsigned char *validate(const unsigned char *data, int *len)
{
    unsigned char b1, b2;

    b1 = *data++;
    --(*len);

    /* 00-7F */
    if (b1 <= 0x7F)
        return data;

    /* Invalid code: 80-C1, F5-FF */
    if (b1 < 0xC2 || b1 > 0xF4)
        return NULL;

    /* 2~4 bytes, 2nd byte = 80-BF */
    if (*len == 0)
        return NULL;
    b2 = *data++;
    --(*len);
    if (b2 < 0x80 || b2 > 0xBF)
        return NULL;

    /* C2-DF, 2 bytes */
    if (b1 < 0xE0)
        return data;

    /* 3~4 bytes, 3rd byte = 80-BF */
    if (*len == 0)
        return NULL;
    if (*data < 0x80 || *data > 0xBF)
        return NULL;
    ++data;
    --(*len);

    /* Special cases: E0, A0-BF; ED, 80-9F; F0, 90-BF; F4, 80-8F */
    if ((b1 == 0xE0 && b2 < 0xA0) || (b1 == 0xED && b2 > 0x9F) ||
        (b1 == 0xF0 && b2 < 0x90) || (b1 == 0xF4 && b2 > 0x8F))
        return NULL;

    /* E0-EF, 3 bytes*/
    if (b1 < 0xF0)
        return data;

    /* F0-F4, 4 bytes, 4th byte = 80-BF */
    if (*len == 0)
        return NULL;
    if (*data < 0x80 || *data > 0xBF)
        return NULL;
    --(*len);
    return data+1;
}

/* return 0-invalid, 1-valid */
int utf8_naive(const unsigned char *data, int len)
{
    while (len && data)
        data = validate(data, &len);

    return !!data;
}
