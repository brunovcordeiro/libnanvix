/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis     <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <nanvix/sys/portal.h>
#include <nanvix/sys/noc.h>
#include <posix/errno.h>

#include "test.h"

#if __TARGET_HAS_PORTAL

/**
 * @brief Test's parameters
 */
#define NR_NODES       2
#define NR_NODES_MAX   PROCESSOR_NOC_NODES_NUM
#define MESSAGE_SIZE   1024
#define MASTER_NODENUM 0
#ifdef __mppa256__
	#define SLAVE_NODENUM  8
#else
	#define SLAVE_NODENUM  1
#endif

/*============================================================================*
 * API Test: Create Unlink                                                    *
 *============================================================================*/

/**
 * @brief API Test: Mailbox Create Unlink
 */
static void test_api_portal_create_unlink(void)
{
	int local;
	int remote;
	int portalid;

	local  = knode_get_num();
	remote = (local == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;

	test_assert((portalid = kportal_create(local)) >= 0);
	test_assert(kportal_unlink(portalid) == 0);

	test_assert((portalid = kportal_create(local)) >= 0);
	test_assert(kportal_allow(portalid, remote) >= 0);
	test_assert(kportal_unlink(portalid) == 0);
}

/*============================================================================*
 * API Test: Create Unlink                                                    *
 *============================================================================*/

/**
 * @brief API Test: Mailbox Create Unlink
 */
static void test_api_portal_open_close(void)
{
	int local;
	int remote;
	int portalid;

	local  = knode_get_num();
	remote = (local == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;

	test_assert((portalid = kportal_open(local, remote)) >= 0);
	test_assert(kportal_close(portalid) == 0);
}

/*============================================================================*
 * API Test: Get volume                                                       *
 *============================================================================*/

/**
 * @brief API Test: Portal Get volume
 */
static void test_api_portal_get_volume(void)
{
	int local;
	int remote;
	int portal_in;
	int portal_out;
	size_t volume;

	local = knode_get_num();
	remote = (local == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;

	test_assert((portal_in = kportal_create(local)) >= 0);
	test_assert((portal_out = kportal_open(local, remote)) >= 0);

		test_assert(kportal_ioctl(portal_in, PORTAL_IOCTL_GET_VOLUME, &volume) == 0);
		test_assert(volume == 0);

		test_assert(kportal_ioctl(portal_out, PORTAL_IOCTL_GET_VOLUME, &volume) == 0);
		test_assert(volume == 0);

	test_assert(kportal_close(portal_out) == 0);
	test_assert(kportal_unlink(portal_in) == 0);
}

/*============================================================================*
 * API Test: Get latency                                                      *
 *============================================================================*/

/**
 * @brief API Test: Portal Get latency
 */
static void test_api_portal_get_latency(void)
{
	int local;
	int remote;
	int portal_in;
	int portal_out;
	uint64_t latency;

	local = knode_get_num();
	remote = (local == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;

	test_assert((portal_in = kportal_create(local)) >= 0);
	test_assert((portal_out = kportal_open(local, remote)) >= 0);

		test_assert(kportal_ioctl(portal_in, PORTAL_IOCTL_GET_LATENCY, &latency) == 0);
		test_assert(latency == 0);

		test_assert(kportal_ioctl(portal_out, PORTAL_IOCTL_GET_LATENCY, &latency) == 0);
		test_assert(latency == 0);

	test_assert(kportal_close(portal_out) == 0);
	test_assert(kportal_unlink(portal_in) == 0);
}

/*============================================================================*
 * API Test: Read Write 2 CC                                                  *
 *============================================================================*/

/**
 * @brief API Test: Read Write 2 CC
 */
static void test_api_portal_read_write(void)
{
	int local;
	int remote;
	int portal_in;
	int portal_out;
	size_t volume;
	uint64_t latency;
	char message[MESSAGE_SIZE];

	local  = knode_get_num();
	remote = (local == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;

	test_assert((portal_in = kportal_create(local)) >= 0);
	test_assert((portal_out = kportal_open(local, remote)) >= 0);

	test_assert(kportal_ioctl(portal_in, PORTAL_IOCTL_GET_VOLUME, &volume) == 0);
	test_assert(volume == 0);
	test_assert(kportal_ioctl(portal_in, PORTAL_IOCTL_GET_LATENCY, &latency) == 0);
	test_assert(latency == 0);

	test_assert(kportal_ioctl(portal_out, PORTAL_IOCTL_GET_VOLUME, &volume) == 0);
	test_assert(volume == 0);
	test_assert(kportal_ioctl(portal_out, PORTAL_IOCTL_GET_LATENCY, &latency) == 0);
	test_assert(latency == 0);

	if (local == MASTER_NODENUM)
	{
		for (unsigned i = 0; i < NITERATIONS; i++)
		{
			kmemset(message, 0, MESSAGE_SIZE);

			test_assert(kportal_allow(portal_in, remote) == 0);
			test_assert(kportal_aread(portal_in, message, MESSAGE_SIZE) == MESSAGE_SIZE);
			test_assert(kportal_wait(portal_in) == 0);

			for (unsigned j = 0; j < MESSAGE_SIZE; ++j)
				test_assert(message[j] == 1);

			kmemset(message, 2, MESSAGE_SIZE);

			test_assert(kportal_awrite(portal_out, message, MESSAGE_SIZE) == MESSAGE_SIZE);
			test_assert(kportal_wait(portal_out) == 0);
		}
	}
	else
	{
		for (unsigned i = 0; i < NITERATIONS; i++)
		{
			kmemset(message, 1, MESSAGE_SIZE);

			test_assert(kportal_awrite(portal_out, message, MESSAGE_SIZE) == MESSAGE_SIZE);
			test_assert(kportal_wait(portal_out) == 0);

			kmemset(message, 0, MESSAGE_SIZE);

			test_assert(kportal_allow(portal_in, remote) == 0);
			test_assert(kportal_aread(portal_in, message, MESSAGE_SIZE) == MESSAGE_SIZE);
			test_assert(kportal_wait(portal_in) == 0);

			for (unsigned j = 0; j < MESSAGE_SIZE; ++j)
				test_assert(message[j] == 2);
		}
	}

	test_assert(kportal_ioctl(portal_in, PORTAL_IOCTL_GET_VOLUME, &volume) == 0);
	test_assert(volume == (NITERATIONS * MESSAGE_SIZE));
	test_assert(kportal_ioctl(portal_in, PORTAL_IOCTL_GET_LATENCY, &latency) == 0);
	test_assert(latency > 0);

	test_assert(kportal_ioctl(portal_out, PORTAL_IOCTL_GET_VOLUME, &volume) == 0);
	test_assert(volume == (NITERATIONS * MESSAGE_SIZE));
	test_assert(kportal_ioctl(portal_out, PORTAL_IOCTL_GET_LATENCY, &latency) == 0);
	test_assert(latency > 0);

	test_assert(kportal_close(portal_out) == 0);
	test_assert(kportal_unlink(portal_in) == 0);
}

/*============================================================================*
 * Fault Test: Invalid Create                                                 *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Create
 */
static void test_fault_portal_invalid_create(void)
{
	int nodenum;

	nodenum = (knode_get_num() == MASTER_NODENUM) ? SLAVE_NODENUM : MASTER_NODENUM;

	test_assert(kportal_create(-1) == -EINVAL);
	test_assert(kportal_create(nodenum) == -EINVAL);
	test_assert(kportal_create(PROCESSOR_NOC_NODES_NUM) == -EINVAL);
}

/*============================================================================*
 * Fault Test: Invalid Unlink                                                 *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Unlink
 */
static void test_fault_portal_invalid_unlink(void)
{
	test_assert(kportal_unlink(-1) == -EINVAL);
	test_assert(kportal_unlink(PORTAL_CREATE_MAX) == -EBADF);
	test_assert(kportal_unlink(1000000) == -EINVAL);
}

/*============================================================================*
 * Fault Test: Double Unlink                                                  *
 *============================================================================*/

/**
 * @brief Fault Test: Double Unlink
 */
static void test_fault_portal_double_unlink(void)
{
	int local;
	int portalid;

	local = knode_get_num();

	test_assert((portalid = kportal_create(local)) >= 0);
	test_assert(kportal_unlink(portalid) == 0);
	test_assert(kportal_unlink(portalid) == -EBADF);
}

/*============================================================================*
 * Fault Test: Invalid Open                                                   *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Open
 */
static void test_fault_portal_invalid_open(void)
{
	int local;

	local = knode_get_num();

	test_assert(kportal_open(local, -1) == -EINVAL);
	test_assert(kportal_open(-1, local + 1) == -EINVAL);
	test_assert(kportal_open(-1, -1) == -EINVAL);
	test_assert(kportal_open(local, PROCESSOR_NOC_NODES_NUM) == -EINVAL);
	test_assert(kportal_open(PROCESSOR_NOC_NODES_NUM, local + 1) == -EINVAL);
	test_assert(kportal_open(local, local) == -EINVAL);
}

/*============================================================================*
 * Fault Test: Invalid Close                                                  *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Close
 */
static void test_fault_portal_invalid_close(void)
{
	test_assert(kportal_close(-1) == -EINVAL);
	test_assert(kportal_close(PORTAL_OPEN_MAX) == -EBADF);
	test_assert(kportal_close(1000000) == -EINVAL);
}

/*============================================================================*
 * Fault Test: Bad Close                                                      *
 *============================================================================*/

/**
 * @brief Fault Test: Bad Close
 */
static void test_fault_portal_bad_close(void)
{
	int local;
	int portalid;

	local = knode_get_num();

	test_assert((portalid = kportal_create(local)) >= 0);
	test_assert(kportal_close(portalid) == -EBADF);
	test_assert(kportal_unlink(portalid) == 0);
}

/*============================================================================*
 * Fault Test: Invalid Read                                                   *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Read
 */
static void test_fault_portal_invalid_read(void)
{
	char buffer[MESSAGE_SIZE];

	test_assert(kportal_aread(-1, buffer, MESSAGE_SIZE) == -EINVAL);
	test_assert(kportal_aread(0, buffer, MESSAGE_SIZE) == -EBADF);
	test_assert(kportal_aread(PORTAL_CREATE_MAX, buffer, MESSAGE_SIZE) == -EBADF);
	test_assert(kportal_aread(1000000, buffer, MESSAGE_SIZE) == -EINVAL);
}

/*============================================================================*
 * Fault Test: Invalid Read Size                                              *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Read Size
 */
static void test_fault_portal_invalid_read_size(void)
{
	int portalid;
	int local;
	char buffer[MESSAGE_SIZE];

	local = knode_get_num();

	test_assert((portalid = kportal_create(local)) >= 0);

		test_assert(kportal_aread(portalid, buffer, -1) == -EINVAL);
		test_assert(kportal_aread(portalid, buffer, 0) == -EINVAL);
		test_assert(kportal_aread(portalid, buffer, PORTAL_MAX_SIZE + 1) == -EINVAL);

	test_assert(kportal_unlink(portalid) == 0);
}

/*============================================================================*
 * Fault Test: Null Read                                                      *
 *============================================================================*/

/**
 * @brief Fault Test: Null Read
 */
static void test_fault_portal_null_read(void)
{
	int portalid;
	int local;

	local = knode_get_num();

	test_assert((portalid = kportal_create(local)) >= 0);

		test_assert(kportal_aread(portalid, NULL, MESSAGE_SIZE) == -EINVAL);

	test_assert(kportal_unlink(portalid) == 0);
}

/*============================================================================*
 * Fault Test: Invalid Write                                                  *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Write
 */
static void test_fault_portal_invalid_write(void)
{
	char buffer[MESSAGE_SIZE];

	test_assert(kportal_awrite(-1, buffer, MESSAGE_SIZE) == -EINVAL);
	test_assert(kportal_awrite(0, buffer, MESSAGE_SIZE) == -EBADF);
	test_assert(kportal_awrite(PORTAL_OPEN_MAX, buffer, MESSAGE_SIZE) == -EBADF);
	test_assert(kportal_awrite(1000000, buffer, MESSAGE_SIZE) == -EINVAL);
}

/*============================================================================*
 * Fault Test: Bad Write                                                      *
 *============================================================================*/

/**
 * @brief Fault Test: Bad Write
 */
static void test_fault_portal_bad_write(void)
{
	int local;
	int portalid;
	char buffer[MESSAGE_SIZE];

	local = knode_get_num();

	test_assert((portalid = kportal_create(local)) >= 0);

		test_assert(kportal_awrite(portalid, buffer, MESSAGE_SIZE) == -EBADF);

	test_assert(kportal_unlink(portalid) == 0);
}

/*============================================================================*
 * Fault Test: Bad Wait                                                       *
 *============================================================================*/

/**
 * @brief Fault Test: Bad Write
 */
static void test_fault_portal_bad_wait(void)
{
	test_assert(kportal_wait(-1) == -EINVAL);
#ifndef __unix64__
	test_assert(kportal_wait(PORTAL_CREATE_MAX) == -EBADF);
	test_assert(kportal_wait(PORTAL_OPEN_MAX) == -EBADF);
#endif
	test_assert(kportal_wait(1000000) == -EINVAL);
}

/*============================================================================*
 * Fault Test: Invalid ioctl                                                  *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid ioctl
 */
static void test_fault_portal_invalid_ioctl(void)
{
	int local;
	int portalid;
	size_t volume;
	uint64_t latency;

	test_assert(kportal_ioctl(-1, PORTAL_IOCTL_GET_VOLUME, &volume) == -EINVAL);
	test_assert(kportal_ioctl(-1, PORTAL_IOCTL_GET_LATENCY, &latency) == -EINVAL);
	test_assert(kportal_ioctl(1000000, PORTAL_IOCTL_GET_VOLUME, &volume) == -EINVAL);
	test_assert(kportal_ioctl(1000000, PORTAL_IOCTL_GET_LATENCY, &latency) == -EINVAL);

	local = knode_get_num();

	test_assert((portalid = kportal_create(local)) >=  0);

		test_assert(kportal_ioctl(portalid, -1, &volume) == -ENOTSUP);

	test_assert(kportal_unlink(portalid) == 0);
}

/*============================================================================*
 * Fault Test: Bad ioctl                                                      *
 *============================================================================*/

/**
 * @brief Fault Test: Bad ioctl
 */
static void test_fault_portal_bad_ioctl(void)
{
	size_t volume;

	test_assert(kportal_ioctl(0, PORTAL_IOCTL_GET_VOLUME, &volume) == -EBADF);
}

/*============================================================================*
 * Test Driver                                                                *
 *============================================================================*/

/**
 * @brief Unit tests.
 */
static struct test portal_tests_api[] = {
	{ test_api_portal_create_unlink, "[test][portal][api] portal create unlink [passed]\n" },
	{ test_api_portal_open_close,    "[test][portal][api] portal open close    [passed]\n" },
	{ test_api_portal_read_write,    "[test][portal][api] portal read write    [passed]\n" },
	{ test_api_portal_get_volume,    "[test][portal][api] portal get volume    [passed]\n" },
	{ test_api_portal_read_write,    "[test][portal][api] portal read write    [passed]\n" },
	{ NULL,                           NULL                                                 },
};

/**
 * @brief Unit tests.
 */
static struct test portal_tests_fault[] = {
	{ test_fault_portal_invalid_create,    "[test][portal][fault] portal invalid create    [passed]\n" },
	{ test_fault_portal_invalid_unlink,    "[test][portal][fault] portal invalid unlink    [passed]\n" },
	{ test_fault_portal_double_unlink,     "[test][portal][fault] portal double unlink     [passed]\n" },
	{ test_fault_portal_invalid_open,      "[test][portal][fault] portal invalid open      [passed]\n" },
	{ test_fault_portal_invalid_close,     "[test][portal][fault] portal invalid close     [passed]\n" },
	{ test_fault_portal_bad_close,         "[test][portal][fault] portal bad close         [passed]\n" },
	{ test_fault_portal_invalid_read,      "[test][portal][fault] portal invalid read      [passed]\n" },
	{ test_fault_portal_invalid_read_size, "[test][portal][fault] portal invalid read size [passed]\n" },
	{ test_fault_portal_null_read,         "[test][portal][fault] portal null read         [passed]\n" },
	{ test_fault_portal_invalid_write,     "[test][portal][fault] portal invalid write     [passed]\n" },
	{ test_fault_portal_bad_write,         "[test][portal][fault] portal bad write         [passed]\n" },
	{ test_fault_portal_bad_wait,          "[test][portal][fault] portal bad wait          [passed]\n" },
	{ test_fault_portal_invalid_ioctl,     "[test][portal][fault] portal invalid ioctl     [passed]\n" },
	{ test_fault_portal_bad_ioctl,         "[test][portal][fault] portal bad ioctl         [passed]\n" },
	{ NULL,                                 NULL                                                       },
};

/**
 * The test_thread_mgmt() function launches testing units on thread manager.
 *
 * @author Pedro Henrique Penna
 */
void test_portal(void)
{
	int nodenum;

	nodenum = knode_get_num();

	if (nodenum == MASTER_NODENUM || nodenum == SLAVE_NODENUM)
	{
		/* API Tests */
		if (nodenum == MASTER_NODENUM)
			nanvix_puts("--------------------------------------------------------------------------------\n");
		for (unsigned i = 0; portal_tests_api[i].test_fn != NULL; i++)
		{
			portal_tests_api[i].test_fn();

			if (nodenum == MASTER_NODENUM)
				nanvix_puts(portal_tests_api[i].name);
		}

		/* Fault Tests */
		if (nodenum == MASTER_NODENUM)
			nanvix_puts("--------------------------------------------------------------------------------\n");
		for (unsigned i = 0; portal_tests_fault[i].test_fn != NULL; i++)
		{
			portal_tests_fault[i].test_fn();

			if (nodenum == MASTER_NODENUM)
				nanvix_puts(portal_tests_fault[i].name);
		}
	}
}

#endif /* __TARGET_HAS_PORTAL */
