/*
 * _4x4_calculator.c
 *
 * Created: 11/8/2023 2:32:18 AM
 *  Author: VTE
 */ 


#define F_CPU 8000000UL
#include <util/delay.h>
#include "keypad.h"
#include "LCD.h"
#include "convertDoubleToString.h"

int main(void)
{
	LCD_vInit();
	keypad_vInit();
	long long result = 0, result2 = 0;
	char x, flag = 0, flag2 = 0, flag3 = 0, op = 0, arr[16], i = -1, str[16];
	float f_res = 0;

	while (1)
	{
		x = keypad_u8check_press(); // Check for keypad input

		if (x == 'A') // Restart calculator
		{
			LCD_clearscreen();
			LCD_vSend_cmd(0x0e);
			result = 0;
			result2 = 0;
			flag = 0;
			flag3=0;
			op = 0;
			char ind = 0;
			f_res=0;
			for (ind; ind < 16; ind++)
			{
				arr[ind] = 0;
			}
			i = -1;
			flag2 = 1;
		}
		else if (flag2 == 1)
		{
			if (x != NOTPRESSED) // Display the entered character on the LCD and store it in an array to trace later
			{
				i++;
				arr[i] = x;
				if (x != '=')
				LCD_vSend_char(x);
			}

			if (x == '=')
			{
				int j, k;
				for (j = 0; j <= i - 1; j++)
				{
					if ((j == 0 && arr[j] < '0') || (arr[j] < '0' && arr[j + 1] < '0' ) || (arr[j] < '0' && arr[j+1]=='=')) // Syntax error
					{
						flag = 1;
						break;
					}
					
				}
				for (j = 0; j <= i - 1; j++) // Calculate the first number before the operation
				{
					if (arr[j] >= 48 && op == 0) 
					result = result * 10 + (arr[j] - 48);
					else
					{
						op = arr[j];
						k = j + 1;
						break;
					}
				}
				for (k; k <= i - 1; k++) // Calculate the second number after the operation
				{
					result2 = result2 * 10 + (arr[k] - 48);
				}
				switch (op) // Perform the calculation based on the operator
				{
					case '+':
					result += result2;
					break;
					case '*':
					result *= result2;
					break;
					case '/':
					if (result % result2 != 0) //handling floating results
					{
						f_res = result;
						f_res /= result2;
						flag3 = 1;
					}
					else
					result /= result2;
					break;
					case '-':
					result -= result2;
					break;
					default:
					break;
				}
				LCD_vSend_cmd(0x0c);
				 if (flag == 1) // Handle syntax error
				{
					LCD_movecursor(2, 3);
					LCD_vSend_string("Syntax error");
				}
				else if (op == '/' && result2 == 0) // Handle division by zero error
				{
					LCD_movecursor(2, 5);
					LCD_vSend_string("Infinity");
				}
				
				else if (flag3 == 1) //Display floating results
				{
					LCD_movecursor(2, 13);
					ftoa(f_res, str, 4);
					LCD_vSend_string(str);
				}
				else // Display the result on the LCD
				{
					LCD_movecursor(2, 15);
					LCD_vSend_cmd(0x04);
					if (result == 0) // Display zero result
					LCD_vSend_char('0');
					else if (result > 0) // Display the positive result
					{
						while (result != 0)
						{
							LCD_vSend_char((result % 10 + 48));
							result /= 10;
						}
					}
					else // Display the negative result
					{
						result *= -1;
						while (result != 0)
						{
							LCD_vSend_char((result % 10 + 48));
							result /= 10;
						}
						LCD_vSend_char('-');
					}
				}
				flag2 = 0; 
			}
		}

		_delay_ms(200);
	}
}
