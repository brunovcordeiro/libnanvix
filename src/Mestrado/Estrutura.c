#include <nanvix/sys/dev.h>
#include <nanvix/sys/noc.h>
#include <nanvix/sys/mailbox.h>
#include <posix/errno.h>
#include "test.h"
#include "Pilha.h"


#define SLAVE_NODENUM  8
#define MASTER_NODENUM 0




/*============================================================================*
 * strlen()                                                                   *
 *============================================================================*/

/**
 * @brief Returns the length of a string.
 *
 * @param str String to be evaluated.
 *
 * @returns The length of the string.
 */
size_t strlen(const char *str)
{
	const char *p;

	/* Count the number of characters. */
	for (p = str; *p != '\0'; p++)
		/* noop */;

	return (p - str);
}

/*============================================================================*
 * nanvix_puts()                                                              *
 *============================================================================*/

/**
 * The nanvix_puts() function writes to the standard output device the string
 * pointed to by @p str.
 */
void nanvix_puts(const char *str)
{
	size_t len;

	len = strlen(str);

	nanvix_write(0, str, len);
}

/*============================================================================*
 * main()                                                                     *
 *============================================================================*/

/**
 * @brief Lunches user-land testing units.
 *
 * @param argc Argument counter.
 * @param argv Argument variables.
 */
int main(int argc, const char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

    int nodenum;

	((void) argc);
	((void) argv);

	nodenum = knode_get_num();
    kprintf("%d",nodenum);


}

void ___start(int argc, const char *argv[])
{
    ((void) argc);
	((void) argv);

		struct Pilha minhapilha;
		int capacidade = 0;
		int local  = knode_get_num();
		kprintf("%d",local);

		capacidade =  local == MASTER_NODENUM ? 30000 : 375;

		float valor = 10;
		criarpilha(&minhapilha, capacidade);

		for (int i = 0; i < capacidade; i++)
			if( estacheia( &minhapilha ) == 1 )
				kprintf("\nPILHA CHEIA! \n");
			else {
				if( capacidade % i == 0 ) kprintf("emilhando");
				empilhar(&minhapilha, valor);
			}
		
		kprintf("termine da pilha");

}	