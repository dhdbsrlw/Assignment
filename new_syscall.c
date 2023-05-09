#include <linux/unistd.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/sched.h>

asmlinkage void sys_print_student_name(char *name)
{
	printk(KERN_INFO "My Name is %s\n", name);
}
EXPORT_SYMBOL_GPL (sys_print_student_name);

asmlinkage void sys_print_student_id(char *id)
{
	printk(KERN_INFO "My Student ID is %s\n", id);
}
EXPORT_SYMBOL_GPL (sys_print_student_id);

asmlinkage void sys_print_student_info(char *school, char *major)
{
	printk(KERN_INFO "I go to %s\n", school);
	printk(KERN_INFO "I major in %s\n", major);
}
EXPORT_SYMBOL_GPL (sys_print_student_info);
