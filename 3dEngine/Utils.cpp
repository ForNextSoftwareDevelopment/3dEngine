#include "utils.h"

namespace Utils
{
    /*********************************************************************
    * Convert short to a string
    *********************************************************************/
    std::string ShortToStr(short num, unsigned int base)
    {
        std::string str = "";

        short i = 0;
        bool isNegative = false;

        // A zero is same "0" string in all base
        if (num == 0)
        {
            str += "0";
            return str;
        }

        // negative numbers are only handled if base is 10, otherwise considered unsigned number
        if (num < 0 && base == 10)
        {
            isNegative = true;
            num = -num;
        }

        unsigned short number = (unsigned short)num;

        while (number != 0)
        {
            unsigned short rem = number % base;
            char ch = ((rem > 9) ? (rem - 10) + 'A' : rem + '0');
            str += ch;
            number = number / base;
        }

        /* Append negative sign for negative numbers */
        if (isNegative)
        {
            str += '-';
        }

        unsigned int strStart = 0;
        unsigned int strEnd;
        unsigned int strLen = (unsigned int)str.size();

        char temp;
        for (strStart = 0, strEnd = strLen - 1; strStart < strEnd; strStart++, strEnd--)
        {
            temp = str[strStart];
            str[strStart] = str[strEnd];
            str[strEnd] = temp;
        }

        return str;
    }

    /*********************************************************************
    * Convert int to a string
    *********************************************************************/
    std::string IntToStr(int num, unsigned int base)
    {
        std::string str = "";

        int i = 0;
        bool isNegative = false;

        // A zero is same "0" string in all base
        if (num == 0)
        {
            str += "0";
            return str;
        }

        // negative numbers are only handled if base is 10, otherwise considered unsigned number
        if (num < 0 && base == 10)
        {
            isNegative = true;
            num = -num;
        }

        unsigned int number = (unsigned int)num;

        while (number != 0)
        {
            unsigned int rem = number % base;
            char ch = ((rem > 9) ? (rem - 10) + 'A' : rem + '0');
            str += ch;
            number = number / base;
        }

        /* Append negative sign for negative numbers */
        if (isNegative)
        {
            str += '-';
        }

        unsigned int strStart = 0;
        unsigned int strEnd;
        unsigned int strLen = (unsigned int)str.size();

        char temp;
        for (strStart = 0, strEnd = strLen - 1; strStart < strEnd; strStart++, strEnd--)
        {
            temp = str[strStart];
            str[strStart] = str[strEnd];
            str[strEnd] = temp;
        }

        return str;
    }
    
    /*********************************************************************
    * Convert floating point to a string
    *********************************************************************/
    std::string FloatToStr (GLfloat f)
    {
        std::string strFloat;
        int iTemp;
        bool negative = false;
        bool minusset = false;
        bool smallerthenone = false;
        bool billions = false;
        bool hundredmillions = false;
        bool tenmillions = false;
        bool millions = false;
        bool hundredthousands = false;
        bool tenthousands = false;
        bool thousands = false;
        bool hundreds = false;
        bool tens = false;

        // If float is negative the add a minus sign and make it positive
        if (f < 0.0f)
        {
            negative = true;
            f = -f;
        } else strFloat.append(" ");

        // If float is larger then 10 bilion then forget it
        if (f >= 10000000000.0f) return ("Overflow");
        if (f < 1.0f) smallerthenone = true; else strFloat.append(" ");

        if (f >= 1000000000.0f)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // billions
            iTemp = (int)f /1000000000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no billions)
            f = f - (iTemp * 1000000000);

            billions = true;
        } else strFloat.append(" ");

        if ((f >= 100000000.0f) || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // hundredmillions
            iTemp = (int)f /100000000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no hundredmillions)
            f = f - (iTemp * 100000000);

            hundredmillions = true;
        } else strFloat.append(" ");

        if ((f >= 10000000.0f) || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // tenmillions
            iTemp = (int)f /10000000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no tenmillions)
            f = f - (iTemp * 10000000);

            tenmillions = true;
        } else strFloat.append(" ");

        if ((f >= 1000000.0f) || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // millions
            iTemp = (int)f /1000000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no millions)
            f = f - (iTemp * 1000000);

            millions = true;
        } else strFloat.append(" ");

        if ((f >= 100000.0f) || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // hundredthousands
            iTemp = (int)f /100000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no hundredthousands)
            f = f - (iTemp * 100000);

            hundredthousands = true;
        } else strFloat.append(" ");

        if ((f >= 10000.0f) || hundredthousands || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // tenthousands
            iTemp = (int)f /10000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no tenthousands)
            f = f - (iTemp * 10000);

            tenthousands = true;
        } else strFloat.append(" ");

        if ((f >= 1000.0f) || tenthousands || hundredthousands || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // thousands
            iTemp = (int)f /1000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no thousands)
            f = f - (iTemp * 1000);

            thousands = true;
        } else strFloat.append(" ");

        if ((f >= 100.0f) || thousands || tenthousands || hundredthousands || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // hundreds
            iTemp = (int)f /100;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no hundreds)
            f = f - (iTemp * 100);

            hundreds = true;
        } else strFloat.append(" ");

        if ((f >= 10.0f) || hundreds || thousands || tenthousands || hundredthousands || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // tens
            iTemp = (int)f /10;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no tens)
            f = f - (iTemp * 10);

            tens = true;
        } else strFloat.append(" ");

        if ((f >= 1.0f) || tens || hundreds  || thousands || tenthousands || hundredthousands || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // ones
            iTemp = (int)f;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (everything after the decimal point)
            f = f - (iTemp);
        } else strFloat.append(" ");

        // Set minus sign if negative number and not allready set
        if (negative && !minusset)
        {
            strFloat.append("-");
            minusset = true;
        }

        // tend's
        if (smallerthenone) strFloat.append("0");
        iTemp = (int)(f * 10.0f);
        strFloat.append(".");
        strFloat.append(std::to_string(iTemp));

        // Calculate new f
        f = f - (((GLfloat)iTemp) * 0.1f);

        // hundred's
        iTemp = (int)(f * 100.0f);
        strFloat.append(std::to_string(iTemp));

        // Calculate new f
        f = f - (((GLfloat)iTemp) * 0.01f);

        // thousand's
        iTemp = (int)(f * 1000.0f);
        strFloat.append(std::to_string(iTemp));

        // Calculate new f
        f = f - (((GLfloat)iTemp) * 0.001f);

        // tenth thousand's
        iTemp = (int)(f * 10000.0f);
        strFloat.append(std::to_string(iTemp));

        // Calculate new f
        f = f - (((GLfloat)iTemp) * 0.0001f);

        // hundreds thousand's
        iTemp = (int)(f * 100000.0f);
        strFloat.append(std::to_string(iTemp));

        // Calculate new f
        f = f - (((GLfloat)iTemp) * 0.00001f);

        // millionth
        iTemp = (int)(f * 1000000.0f);
        strFloat.append(std::to_string(iTemp));

        return (strFloat);
    }

    /*********************************************************************
    * Convert a string to a floating point
    *********************************************************************/
    GLfloat StrToFloat (char *pStrFloat)
    {
        GLfloat f = 0.0;
        char *p = pStrFloat;
        bool decimalPointPresent = false;
        bool negativeNumber = false;
        int nBeforeDecimalPoint = 0;
        int nAfterDecimalPoint = 0;

        // Check for minus sign
        if (*p == '-')
        {
            negativeNumber = true;
            pStrFloat++;
            p++;
        }

        // First count the numbers before the decimal point
        while ((*p == '0') || (*p == '1') || (*p == '2') || (*p == '3') || (*p == '4') || (*p == '5') || (*p == '6') || (*p == '7') || (*p == '8') || (*p == '9'))
        {
            nBeforeDecimalPoint++;
            p++;
        }

        // Found dec point
        if (*p == '.')
        {
            decimalPointPresent = true;
            p++;
        }

        // Secondly count the numbers after the decimal point
        while ((*p == '0') || (*p == '1') || (*p == '2') || (*p == '3') || (*p == '4') || (*p == '5') || (*p == '6') || (*p == '7') || (*p == '8') || (*p == '9'))
        {
            nAfterDecimalPoint++;
            p++;
        }

        // Add number before decimal point
        for (int i=0; i<nBeforeDecimalPoint; i++)
        {
            float factor = 1.0f;
            for (int j=nBeforeDecimalPoint; j>(i+1); j--) factor = factor * 10.0f;

            f = f + factor * (*pStrFloat - '0');
            pStrFloat++;
        }

        // Add number after decimal point
        if (decimalPointPresent)
        {
            // Skip decimal point
            pStrFloat++;

            // Convert all numbers after the decimal point to float
            for (int i=0; i<nAfterDecimalPoint; i++)
            {
                float factor = 0.1f;
                for (int j=0; j<i; j++) factor = factor * 0.1f;

                f = f + factor * (*pStrFloat - '0');
                pStrFloat++;
            }
        }

        if (negativeNumber) f = -f;
        return (f);
    }

    /*********************************************************************
    * Create a random float between 0.0f and 1.0f
    *********************************************************************/
    float Random (void)
    {
        float res;
        unsigned int tmp;
        static unsigned int seed = 0xFFFF0C59;

        seed *= 16807;

        tmp = seed ^ (seed >> 4) ^ (seed << 15);

        *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

        return (float)(res - 1.0);
    }

    /*********************************************************************
    * Convert WCST to std::string
    *********************************************************************/
    std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen)
    {
        int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

        std::string dblstr(len, '\0');
        len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
            pstr, wslen /* not necessary NULL-terminated */,
            &dblstr[0], len,
            NULL, NULL /* no default char */);

        return dblstr;
    }
}

