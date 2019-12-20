#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <selinux/selinux.h>

static void print_usage(char *progname)
{
	fprintf(stderr,
		"usage:  %s [-v] -f\n"
		"Where:\n\t"
		"-f  File to create\n\t"
		"-v  Print information.\n", progname);
	exit(-1);
}

int main(int argc, char **argv)
{
	int opt, result, fd, save_err;
	char *context, *file = NULL;
	bool verbose = false;

	while ((opt = getopt(argc, argv, "f:v")) != -1) {
		switch (opt) {
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

	if (!file)
		print_usage(argv[0]);

	if (verbose) {
		result = getcon(&context);
		if (result < 0) {
			fprintf(stderr, "Failed to obtain process context\n");
			exit(-1);
		}
		printf("Process context:\n\t%s\n", context);
		free(context);
	}

	fd = creat(file, O_RDWR);
	save_err = errno;
	if (fd < 0) {
		fprintf(stderr, "creat(2) Failed: %s\n", strerror(errno));
		return save_err;
	}

	context = NULL;
	result = fgetfilecon(fd, &context);
	if (result < 0) {
		fprintf(stderr, "fgetfilecon(3) Failed: %s\n",
			strerror(errno));
		result = -1;
		goto err;
	}
	result = 0;

	if (verbose)
		printf("File context is: %s\n", context);

err:
	free(context);
	close(fd);

	return result;
}
