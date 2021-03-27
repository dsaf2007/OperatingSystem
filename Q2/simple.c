#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h> //kmalloc



struct birthday
{
	int day;
	int month;
	int year;
	struct list_head list;
};
static LIST_HEAD(birthday_list);
//initailize birthdays
void init_birthday(struct birthday *name,int year,int month, int day)
{	
	name -> day=day;
	name -> month=month;
	name -> year = year;
	INIT_LIST_HEAD(&name->list);
	list_add_tail(&name->list,&birthday_list);
}


//when module is called
int simple_init(void)
{
	struct birthday *chung, *donggu, *kim, *kyung, *min;
	struct birthday *ptr;
	printk(KERN_INFO "Loading Module\n");

	chung = kmalloc(sizeof(chung),GFP_KERNEL);
	donggu = kmalloc(sizeof(donggu),GFP_KERNEL);
	kim = kmalloc(sizeof(kim),GFP_KERNEL);
	kyung = kmalloc(sizeof(kyung) ,GFP_KERNEL);
	min = kmalloc(sizeof(min),GFP_KERNEL);

	init_birthday(chung, 1996, 7, 30);
	init_birthday(donggu, 1997,8,31);
	init_birthday(kim, 1996,4,16);
	init_birthday(kyung,2001,1,10);
	init_birthday(min, 2018,10,29);

	list_for_each_entry(ptr, &birthday_list,list)
	{
		printk(KERN_INFO "person year: %d,month: %d,day: %d \n",ptr->year,ptr->month,ptr->day);
	}
	return 0;
}

//This function is called when the module is removed
void simple_exit(void)
{
	struct birthday *ptr, *next;
	printk(KERN_INFO "Removing Module\n");

	list_for_each_entry_safe(ptr,next,&birthday_list,list)
	{
		printk(KERN_INFO "removing year: %d, month: %d, day: %d \n",ptr->year,ptr->month,ptr->day);
		list_del(&ptr->list);
		kfree(ptr);
	}
}

//Macros for registering module entry and exit points
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SSG");
