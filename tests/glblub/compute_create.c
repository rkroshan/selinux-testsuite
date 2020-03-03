#include <selinux/selinux.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	const char *scon = argv[1];
	const char *tcon = argv[2];
	const char *tclass = argv[3];
	char *res = NULL;

	security_class_t cls = string_to_security_class(tclass);

	int rc = security_compute_create(scon, tcon, cls, &res);

	if (rc)
		return 1;

	printf("%s\n", res);
	free(res);
	return 0;
}
