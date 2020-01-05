
#include <nanvix/sys/dev.h>
#include <nanvix/sys/noc.h>
#include <nanvix/sys/mailbox.h>
#include <posix/errno.h>
#include "test.h"

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

   	int local;
	int remote;
	int mbx_in;
	int mbx_out;

	char message[MAILBOX_MSG_SIZE];

	local  = knode_get_num();
	remote = (local == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;

	test_assert((mbx_in = kmailbox_create(local)) >= 0);
	test_assert((mbx_out = kmailbox_open(remote)) >= 0);

	if (local == MASTER_NODENUM)
	{
        int i;

            for  (i = 0; i < 15; i++) {
                kmemset(message, local, MAILBOX_MSG_SIZE);

                test_assert(kmailbox_awrite(mbx_out, message, MAILBOX_MSG_SIZE) == MAILBOX_MSG_SIZE);

                kmemset(message, 0, MAILBOX_MSG_SIZE);

                test_assert(kmailbox_aread(mbx_in, message, MAILBOX_MSG_SIZE) == MAILBOX_MSG_SIZE);
                test_assert(kmailbox_wait(mbx_in) == 0);

                kprintf("Sou o %d e recebi do %d" , local , message[1] );
                test_assert(kmailbox_close(mbx_out) == 0);
                remote++;
                test_assert((mbx_out = kmailbox_open(remote)) >= 0);
		}
	}
	else
	{
	        kmemset(message, local, MAILBOX_MSG_SIZE);

			test_assert(kmailbox_aread(mbx_in, message, MAILBOX_MSG_SIZE) == MAILBOX_MSG_SIZE);
			test_assert(kmailbox_wait(mbx_in) == 0);

			kprintf("Sou o %d e recebi do %d" , local , message[1] );

			kmemset(message, local, MAILBOX_MSG_SIZE);

			test_assert(kmailbox_awrite(mbx_out, message, MAILBOX_MSG_SIZE) == MAILBOX_MSG_SIZE);
            test_assert(kmailbox_close(mbx_out) == 0);

		
	}

    	test_assert(kmailbox_unlink(mbx_in) == 0);

}	