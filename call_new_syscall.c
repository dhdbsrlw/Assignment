#include <linux/unistd.h>
#include <linux/kernel.h>

int main (void)
{
	char name[] = "Yoonjin Oh";
	char id[] = "2020120036";
	char school[] = "Korea University";
	char major[] = "Business";

	syscall(333, name);
	syscall(334, id);
	syscall(335, school, major);

	return 0;
}
