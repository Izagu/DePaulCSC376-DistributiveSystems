#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>


wchar_t 	STARTH 	= L'\x3042';
wchar_t         ENDH    = L'\x3093';
wchar_t         STARTK  = L'\x30A2';
wchar_t		ENDK    = L'\x30f3';
wchar_t		DIFF	= L'\x60';
wchar_t		SPACE	= L'\x0020';

int		main	()
{
	wchar_t		usr;
	wchar_t		i;
	wchar_t		ret;
	setlocale(LC_ALL, ""); //set the locale as defined by the user
	//wprintf(L"Please enter a Hiragana char,a Katakana char, or a blank line to quit:");
        //wscanf(L"%lc", &usr);
	wprintf(L"Please enter a Hiragana char,a Katakana char, or a blank line to quit:");
        wscanf(L"%lc", &usr);
        //fgetws(&usr,264, stdin);

	while(1)
	{
			
		if(STARTH <= usr && usr <= ENDH){ //this means its Hiragana 
			for(i = STARTH; i <= ENDH; i++)
			{
				if (usr == i)
				{
					ret = usr+DIFF;
					wprintf(L"Hiragana char %lc has Katakana equivalent %lc \n",usr, ret);
					//return(EXIT_SUCCESS);  
				}
			}
		}
		else if(STARTK <= usr && usr <= ENDK){ //THIS MEANS ITS KATAKANA
                        for(i = STARTK; i <= ENDK; i++)
                        {
                                if (usr == i)
                                {
                                        ret = usr-DIFF;
                                        wprintf(L"Katakan char %lc has Hiragana equivalent %lc \n",usr, ret);
                                        //return(EXIT_SUCCESS);
                                }
                        }
                }
		else if(usr == SPACE){
			return(EXIT_SUCCESS);
		}
		else
		{
			 wprintf(L"%lc is neither Hiragana nor Katakana.\n", usr);
		}

		wprintf(L"Please enter a Hiragana char,a Katakana char, or a blank line to quit:");
        	wscanf(L"%lc", &usr);
        	//fgetws(&usr,264, stdin);

	}
}
