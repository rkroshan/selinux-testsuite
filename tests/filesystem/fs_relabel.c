#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <linux/unistd.h>
#include <selinux/selinux.h>

static void print_usage(char *progname)
{
	fprintf(stderr,
		"usage:  %s [-v] -n\n"
		"Where:\n\t"
		"-n  New fs context\n\t"
		"-v  Print information.\n", progname);
	exit(-1);
}

int main(int argc, char **argv)
{
	int opt, result, save_err;
	char *context, *fscreate_con, *newcon = NULL;
	bool verbose = false;

	while ((opt = getopt(argc, argv, "n:v")) != -1) {
		switch (opt) {
		case 'n':
			newcon = optarg;
			break;
		case 'v':
			verbose = true;
			break;
		default:
			print_usage(argv[0]);
		}
	}

	if (!newcon)
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

	result = setfscreatecon(newcon);
	save_err = errno;
	if (result < 0) {
		fprintf(stderr, "Failed setfscreatecon(3): %s\n",
			strerror(errno));
		return save_err;
	}

	result = getfscreatecon(&fscreate_con);
	if (result < 0) {
		fprintf(stderr, "Failed getfscreatecon(3): %s\n",
			strerror(errno));
		exit(-1);
	}
	if (verbose)
		printf("New fscreate context: %s\n", fscreate_con);

	free(fscreate_con);
	return 0;
}
