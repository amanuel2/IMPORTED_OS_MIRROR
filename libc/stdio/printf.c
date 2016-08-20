#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#if __STDC_HOSTED__ /* TODO: Hack until we have our own <stdint.h>. */
#include <stdint-gcc.h>
#else
#include <stdint.h>
#endif
#include <stdio.h>
#include <string.h>

static size_t convert_integer(char* destination, uintmax_t value,
                              uintmax_t base, const char* digits)
{
	size_t result = 1;
	uintmax_t copy = value;
	while ( base <= copy )
		copy /= base, result++;
	for ( size_t i = result; i != 0; i-- )
		destination[i-1] = digits[value % base],
		value /= base;
	return result;
}

static void print(const char* data, size_t data_length)
{
	for ( size_t i = 0; i < data_length; i++ )
		putchar((int) ((const unsigned char*) data)[i]);
}

int printf(const char* restrict format, ...)
{
	va_list parameters;
	va_start(parameters, format);

	int written = 0;
	size_t amount;
	bool rejected_bad_specifier = false;

	while ( *format != '\0' )
	{
		if ( *format != '%' )
		{
		print_c:
			amount = 1;
			while ( format[amount] && format[amount] != '%' )
				amount++;
			print(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format;

		if ( *(++format) == '%' )
			goto print_c;

		if ( rejected_bad_specifier )
		{
		incomprehensible_conversion:
			rejected_bad_specifier = true;
			format = format_begun_at;
			goto print_c;
		}

		enum length
		{
			LENGTH_SHORT_SHORT,
			LENGTH_SHORT,
			LENGTH_DEFAULT,
			LENGTH_LONG,
			LENGTH_LONG_LONG,
			LENGTH_LONG_DOUBLE,
			LENGTH_INTMAX_T,
			LENGTH_SIZE_T,
			LENGTH_PTRDIFF_T,
		};

		struct length_modifer
		{
			const char* name;
			enum length length;
		};

		struct length_modifer length_modifiers[] =
		{
			{ "hh", LENGTH_SHORT_SHORT },
			{ "h", LENGTH_SHORT },
			{ "", LENGTH_DEFAULT },
			{ "l", LENGTH_LONG },
			{ "ll", LENGTH_LONG_LONG },
			{ "L", LENGTH_LONG_DOUBLE },
			{ "j", LENGTH_INTMAX_T },
			{ "z", LENGTH_SIZE_T },
			{ "t", LENGTH_PTRDIFF_T },
		};

		enum length length = LENGTH_DEFAULT;
		size_t length_length = 0;
		for ( size_t i = 0;
		      i < sizeof(length_modifiers) / sizeof(length_modifiers[0]);
		      i++ )
		{
			size_t name_length = strlen(length_modifiers[i].name);
			if ( name_length < length_length )
				continue;
			bool different = false;
			for ( size_t n = 0; n < name_length; n++ )
				if ( format[n] != length_modifiers[i].name[n] )
					different = true;
			if ( different )
				continue;
			length = length_modifiers[i].length;
			length_length = name_length;
		}

		format += length_length;

		if ( *format == 'd' || *format == 'i' || *format == 'o' ||
		     *format == 'u' || *format == 'x' || *format == 'X' ||
		     *format == 'p' )
		{
			char conversion = *format++;

			bool negative_value = false;
			uintmax_t value;
			if ( conversion == 'i' || conversion == 'd' )
			{
				intmax_t signed_value;
				if ( length == LENGTH_SHORT_SHORT )
					signed_value = va_arg(parameters, int);
				else if ( length == LENGTH_SHORT )
					signed_value = va_arg(parameters, int);
				else if ( length == LENGTH_DEFAULT )
					signed_value = va_arg(parameters, int);
				else if ( length == LENGTH_LONG )
					signed_value = va_arg(parameters, long);
				else if ( length == LENGTH_LONG_LONG )
					signed_value = va_arg(parameters, long long);
				else if ( length == LENGTH_INTMAX_T )
					signed_value = va_arg(parameters, intmax_t);
#if 0
				else if ( length == LENGTH_SIZE_T )
					signed_value = va_arg(parameters, ssize_t);
#endif
				else if ( length == LENGTH_PTRDIFF_T )
					signed_value = va_arg(parameters, ptrdiff_t);
				else
					goto incomprehensible_conversion;
				value = (negative_value = signed_value < 0) ?
				         - (uintmax_t) signed_value : (uintmax_t) signed_value;
			}
			else
			{
				if ( length == LENGTH_SHORT_SHORT )
					value = va_arg(parameters, unsigned int);
				else if ( length == LENGTH_SHORT )
					value = va_arg(parameters, unsigned int);
				else if ( length == LENGTH_DEFAULT )
					value = va_arg(parameters, unsigned int);
				else if ( length == LENGTH_LONG )
					value = va_arg(parameters, unsigned long);
				else if ( length == LENGTH_LONG_LONG )
					value = va_arg(parameters, unsigned long long);
				else if ( length == LENGTH_INTMAX_T )
					value = va_arg(parameters, uintmax_t);
				else if ( length == LENGTH_SIZE_T )
					value = va_arg(parameters, size_t);
				else if ( length == LENGTH_PTRDIFF_T )
					value = (uintmax_t) va_arg(parameters, ptrdiff_t);
				else
					goto incomprehensible_conversion;
			}

			const char* digits = conversion == 'X' ? "0123456789ABCDEF" :
			                                         "0123456789abcdef";
			uintmax_t base = (conversion == 'x' || conversion == 'X') ? 16 :
			                 conversion == 'o' ? 8 : 10;
			if ( negative_value )
				putchar('-');

			char output[sizeof(uintmax_t) * 3];
			size_t output_length = convert_integer(output, value, base, digits);
			print(output, output_length);

		}
		else if ( *format == 'c' )
		{
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			print(&c, sizeof(c));
		}
		else if ( *format == 's' )
		{
			format++;
			const char* s = va_arg(parameters, const char*);
			print(s, strlen(s));
		}
		else
		{
			goto incomprehensible_conversion;
		}
	}

	va_end(parameters);

	return written;
}
