#include <stdio.h>

int passed;
int failed;

int starts_with(const char *prefix,const char *string)
{
    char c_string,c_prefix;
    while((c_prefix=*prefix++) && (c_string=*string++))
    {
        if(c_prefix !=c_string )
            return 0;
    }

		// If the string runs out before the prefix.
		if (!c_string && c_prefix)
			return 0;
		else  
			return 1;
}

int perform_test(const char *string, const char *prefix,const int expected)
{
	int test_result= starts_with(string,prefix);
	printf("starts_with(%s,%s) = %d ",string,prefix,expected);
	if (test_result==expected)
	{
		printf("passed\n ");
		passed ++;
	}
	else 
	{
		printf("failed \n ");
		failed;
	}
}

int main()
{
	perform_test("hol","hola",1);
	perform_test("ch","hola",0);
	perform_test("hol","ahola",0);
	perform_test("hol","ho",0);
	printf("\nTotal results:\n");
	printf("%d test passed. \n",passed);
	printf("%d test failed.\n",failed);
	
	return 0;
}
