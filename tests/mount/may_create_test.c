#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <linux/unistd.h>
#include <selinux/selinux.h>
#include <selinux/context.h>

static void print_usage(char *progname)
{
	fprintf(stderr,
		"usage:  %s [-v] -t -f\n"
		"Where:\n\t"
		"-t  Type for context of created file\n\t"
		"-f  File to create\n\t"
		"-v  Print information.\n", progname);
	exit(-1);
}

int main(int argc, char **argv)
{
	int opt, result, fd, save_err;
	char *context, *newcon, *orgfcon, *type = NULL, *file = NULL;
	bool verbose = false;
	context_t con_t;

	while ((opt = getopt(argc, argv, "t:f:v")) != -1) {
		switch (opt) {
		case 't':
			type = optarg;
			break;
		case 'f':
			file = optarg;
			break;
		case 'v':
			verbose = true;
			break;
		default:
			print_usage(argv[0]);
		}
	}

	if (!type || !file)
		print_usage(argv[0]);

	result = getcon(&context);
	if (result < 0) {
		fprintf(stderr, "Failed to obtain process context\n");
		exit(-1);
	}

	/* Build new file context */
	con_t = context_new(context);
	if (!con_t) {
		fprintf(stderr, "Unable to create context structure\n");
		exit(-1);
	}

	if (context_type_set(con_t, type)) {
		fprintf(stderr, "Unable to set new type\n");
		exit(-1);
	}

	newcon = context_str(con_t);
	if (!newcon) {
		fprintf(stderr, "Unable to obtain new context string\n");
		exit(-1);
	}

	if (verbose) {
		printf("Process context:\n\t%s\n", context);
		printf("is creating test file:\n\t%s\n", file);
		printf("and changing its context to:\n\t%s\n", newcon);
	}

	/* hooks.c may_create() FILESYSTEM__ASSOCIATE */
	fd = creat(file, O_RDWR);
	save_err = errno;
	if (fd < 0) {
		fprintf(stderr, "creat(2) Failed: %s\n", strerror(errno));
		result = save_err;
		goto err;
	}

	result = fgetfilecon(fd, &orgfcon);
	if (result < 0) {
		fprintf(stderr, "fgetfilecon(3) Failed: %s\n", strerror(errno));
		result = -1;
		goto err;
	}

	if (verbose)
		printf("Current test file context is: %s\n", orgfcon);

	free(orgfcon);

	/* hooks.c selinux_inode_setxattr() FILESYSTEM__ASSOCIATE */
	result = fsetfilecon(fd, newcon);
	save_err = errno;
	if (result < 0) {
		fprintf(stderr, "fsetfilecon(3) Failed: %s\n", strerror(errno));
		result = save_err;
		goto err;
	}

	fd = open(file, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "open(2) Failed: %s\n", strerror(errno));
		result = -1;
	}

err:
	free(context);
	free(newcon);
	close(fd);
	return result;

}
