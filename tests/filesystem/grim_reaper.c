#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/mount.h>
#include <selinux/selinux.h>

#define WAIT_COUNT 60
#define USLEEP_TIME 10000

/* Remove any mounts associated with the loop device in argv[1] */
int main(int argc, char *argv[])
{
	FILE *fp;
	size_t len;
	ssize_t num;
	int index = 0, i, result = 0;
	char *mount_info[2];
	char *buf = NULL, *item;

	if (!argv[1])
		return -1;

	fp = fopen("/proc/mounts", "re");
	if (!fp) {
		fprintf(stderr, "Failed to open /proc/mounts: %s\n",
			strerror(errno));
		return -1;
	}

	while ((num = getline(&buf, &len, fp)) != -1) {
		index = 0;
		item = strtok(buf, " ");
		while (item != NULL) {
			mount_info[index] = item;
			index++;
			if (index == 2)
				break;
			item = strtok(NULL, " ");
		}

		if (strcmp(mount_info[0], argv[1]) == 0) {
			for (i = 0; i < WAIT_COUNT; i++) {
				result = umount(mount_info[1]);
				if (!result)
					break;

				if (errno != EBUSY) {
					fprintf(stderr, "Failed umount(2): %s\n",
						strerror(errno));
					break;
				}
				usleep(USLEEP_TIME);
			}
		}
	}

	free(buf);
	fclose(fp);
	return result;
}
