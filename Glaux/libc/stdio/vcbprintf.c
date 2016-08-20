#include <sys/types.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const char* digits_x = "0123456789abcdef";
static const char* digits_X = "0123456789ABCDEF";

static size_t convert_int (char* dest, uintmax_t value,
                           uintmax_t base, const char* digits)
{
	size_t len = 1;
	uintmax_t copy = value;
	while (base - 1 < copy)
	{
		copy /= base;
		len += 1;
	}
	for (size_t i = len; i != 0; i--)
	{
		dest[i-1] = digits[value%base];
		value /= base;
	}
	return len;
}

static size_t nop_callback (void* info, const char* str, size_t len)
{
	(void) info;
	(void) str;
	return len;
}

int vcbprintf (void* info, size_t (*callback) (void*, const char*, size_t),
               const char* format, va_list args)
{
	if (!callback)
	{
		callback = nop_callback;
	}
	size_t written = 0;
	while (*format != '\0')
	{
		if (*format != '%')
		{
			size_t i = 1;
			while (format[i] != '\0' && format[i] != '%')
			{
				i += 1;
			}
			if (callback (info, format, i) != i)
			{
				return -1;
			}
			format += i;
			written += i;
			continue;
		}
		format += 1;
		const char* format_start_addr = format;
		bool alternate = false;
		bool negative_width = false;
		bool prepend_plus = false;
		bool prepend_blank = false;
		bool zero_pad = false;
		while (true)
		{
			if (*format == '#')
			{
				alternate = true;
			}
			else if (*format == '-')
			{
				negative_width = true;
			}
			else if (*format == '+')
			{
				prepend_plus = true;
			}
			else if (*format == ' ')
			{
				prepend_blank = true;
			}
			else if (*format == '0')
			{
				zero_pad = true;
			}
			else
			{
				break;
			}
			format += 1;
		}
		int signed_width = 0;
		size_t width = 0;
		if (*format == '*')
		{
			if ((signed_width = va_arg (args, int)) < 0)
			{
				negative_width = !negative_width;
				width = -signed_width;
			}
			format += 1;
		}
		else
		{
			if (*format == '-')
			{
				negative_width = !negative_width;
				format += 1;
			}
			while ('0' <= *format && *format <= '9')
			{
				width = 10 * width + *format - '0';
				format += 1;
			}
		}
		size_t precision = SIZE_MAX;
		if (*format == '.')
		{
			format += 1;
			precision = 0;
			if (*format == '*')
			{
				format += 1;
				int signed_precision = va_arg (args, int);
				precision = signed_precision < 0 ? 0 : signed_precision;
			}
			else if (*format == '-')
			{
				format += 1;
				while ('0' <= *format && *format <= '9')
				{
					format += 1;
				}
			}
			else
			{
				while ('0' <= *format && *format <= '9')
				{
					precision = 10 * precision + *format - '0';
					format += 1;
				}
			}
		}
		enum length
		{
			LEN_SHORT_SHORT,
			LEN_SHORT,
			LEN_DEFAULT,
			LEN_LONG,
			LEN_LONG_LONG,
			LEN_LONG_DOUBLE,
			LEN_INTMAX_T,
			LEN_SIZE_T,
			LEN_PTRDIFF_T,
		};
		int length = LEN_DEFAULT;
		if (*format == 'h')
		{
			if (format[1] == 'h')
			{
				length = LEN_SHORT_SHORT;
				format += 2;
			}
			else
			{
				length = LEN_SHORT;
				format += 1;
			}
		}
		else if (*format == 'l')
		{
			if (format[1] == 'l')
			{
				length = LEN_LONG_LONG;
				format += 2;
			}
			else
			{
				length = LEN_LONG;
				format += 1;
			}
		}
		else if (*format == 'L')
		{
			length = LEN_LONG_DOUBLE;
			format += 1;
		}
		else if (*format == 'j')
		{
			length = LEN_INTMAX_T;
			format += 1;
		}
		else if (*format == 'z')
		{
			length = LEN_SIZE_T;
			format += 1;
		}
		else if (*format == 't')
		{
			length = LEN_PTRDIFF_T;
			format += 1;
		}
		if (*format == 'd' || *format == 'i' ||
		    *format == 'x' || *format == 'X' ||
		    *format == 'u' || *format == 'o')
		{
			uintmax_t value;
			bool negative_value = false;
			if (*format == 'd' || *format == 'i')
			{
				intmax_t signed_value;
				if (length == LEN_SHORT_SHORT)
				{
					signed_value = (char) va_arg (args, int);
				}
				else if (length == LEN_SHORT)
				{
					signed_value = (short) va_arg (args, int);
				}
				else if (length == LEN_LONG)
				{
					signed_value = va_arg (args, long);
				}
				else if (length == LEN_LONG_LONG)
				{
					signed_value = va_arg (args, long long);
				}
				else if (length == LEN_INTMAX_T)
				{
					signed_value = va_arg (args, intmax_t);
				}
				else if (length == LEN_SIZE_T)
				{
					signed_value = va_arg (args, ssize_t);
				}
				else if (length == LEN_PTRDIFF_T)
				{
					signed_value = va_arg (args, ptrdiff_t);
				}
				else
				{
					signed_value = va_arg (args, int);
				}
				negative_value = signed_value < 0;
				value = negative_value ? - (uintmax_t) signed_value :
				                           (uintmax_t) signed_value;
			}
			else
			{
				if (length == LEN_SHORT_SHORT)
				{
					value = (unsigned char) va_arg (args, unsigned int);
				}
				else if (length == LEN_SHORT)
				{
					value = (unsigned short) va_arg (args, unsigned int);
				}
				else if (length == LEN_LONG)
				{
					value = va_arg (args, unsigned long);
				}
				else if (length == LEN_LONG_LONG)
				{
					value = va_arg (args, unsigned long long);
				}
				else if (length == LEN_INTMAX_T)
				{
					value = va_arg (args, uintmax_t);
				}
				else if (length == LEN_SIZE_T)
				{
					value = va_arg (args, size_t);
				}
				else if (length == LEN_PTRDIFF_T)
				{
					value = va_arg (args, ptrdiff_t);
				}
				else
				{
					value = va_arg (args, unsigned int);
				}
				prepend_plus = false;
				prepend_blank = false;
			}
			char prefix[4];
			size_t prefix_len = 0;
			if (negative_value)
			{
				prefix[prefix_len++] = '-';
			}
			else if (prepend_plus)
			{
				prefix[prefix_len++] = '+';
			}
			else if (prepend_blank)
			{
				prefix[prefix_len++] = ' ';
			}
			if (alternate)
			{
				if (*format == 'x')
				{
					prefix[prefix_len++] = '0';
					prefix[prefix_len++] = 'x';
				}
				else if (*format == 'X')
				{
					prefix[prefix_len++] = '0';
					prefix[prefix_len++] = 'X';
				}
				else if (*format == 'o')
				{
					prefix[prefix_len++] = '0';
				}
			}
			const char* digits = *format == 'X' ? digits_X : digits_x;
			size_t base = 10;
			if (*format == 'x' || *format == 'X')
			{
				base = 16;
			}
			else if (*format == 'o')
			{
				base = 8;
			}
			char output [8 * sizeof (uintmax_t) + 1];
			size_t output_len = convert_int (output, value, base, digits);
			if (callback (info, prefix, prefix_len) != prefix_len)
			{
				return -1;
			}
			written += prefix_len;
			if (negative_width)
			{
				if (callback (info, output, output_len) != output_len)
				{
					return -1;
				}
				written += output_len;
				for (size_t i = output_len; i < width; i++)
				{
					if (callback (info, " ", 1) != 1)
					{
						return -1;
					}
					written += 1;
				}
				format += 1;
			}
			else if (zero_pad)
			{
				for (size_t i = output_len; i < width; i++)
				{
					if (callback (info, "0", 1) != 1)
					{
						return -1;
					}
					written += 1;
				}
				if (callback (info, output, output_len) != output_len)
				{
					return -1;
				}
				written += output_len;
				format += 1;
			}
			else
			{
				for (size_t i = output_len; i < width; i++)
				{
					if (callback (info, " ", 1) != 1)
					{
						return -1;
					}
					written += 1;
				}
				if (callback (info, output, output_len) != output_len)
				{
					return -1;
				}
				written += output_len;
				format += 1;
			}
		}
		else if (*format == 's')
		{
			const char* string = va_arg (args, char*);
			size_t output_len = strnlen (string, precision);
			if (negative_width)
			{
				if (callback (info, string, output_len) != output_len)
				{
					return -1;
				}
				written += output_len;
				for (size_t i = output_len; i < width; i++)
				{
					if (callback (info, " ", 1) != 1)
					{
						return -1;
					}
					written += 1;
				}
			}
			else
			{
				for (size_t i = output_len; i < width; i++)
				{
					if (callback (info, " ", 1) != 1)
					{
						return -1;
					}
					written += 1;
				}
				if (callback (info, string, output_len) != output_len)
				{
					return -1;
				}
				written += output_len;
			}
			format += 1;
		}
		else
		{
			format = format_start_addr;
			size_t i = 1;
			while (format[i] != '\0' && format[i] != '%')
			{
				i += 1;
			}
			if (callback (info, format, i) != i)
			{
				return -1;
			}
			format += i;
			written += i;
		}
	}
	va_end (args);
	return written;
}
