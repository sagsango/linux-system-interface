/* nftw_dir_tree.c

	 Demonstrate the use of nftw(3). Walk though the directory tree specified
	 on the command line (or the current working directory if no directory
	 is specified on the command line), displaying an indented hierarchy
	 of files in the tree. For each file, display:

 * a letter indicating the file type (using the same letters
 as "ls -l"), as obtained using stat(2);
 * a string indicating the file type, as supplied by nftw(); and
 * the file's i-node number.
 */
#if defined(__sun)
#define _XOPEN_SOURCE 500       /* Solaris 8 needs it this way */
#else
#if ! defined(_XOPEN_SOURCE) || _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600       /* Get nftw() and S_IFSOCK declarations */
#endif
#endif
#include <ftw.h>
#include "tlpi_hdr.h"
	static void
usageError(const char *progName, const char *msg)
{
	if (msg != NULL)
		fprintf(stderr, "%s\n", msg);
	fprintf(stderr, "Usage: %s [-d] [-m] [-p] [directory-path]\n", progName);
	fprintf(stderr, "\t-d Use FTW_DEPTH flag\n");
	fprintf(stderr, "\t-m Use FTW_MOUNT flag\n");
	fprintf(stderr, "\t-p Use FTW_PHYS flag\n");
	exit(EXIT_FAILURE);
}
	static int                      /* Function called by nftw() */
dirTree(const char *pathname, const struct stat *sbuf, int type,
		struct FTW *ftwb)
{
	if (type == FTW_NS) {                  /* Could not stat() file */
		printf("?");
	} else {
		switch (sbuf->st_mode & S_IFMT) {  /* Print file type */
			case S_IFREG:  printf("-"); break;
			case S_IFDIR:  printf("d"); break;
			case S_IFCHR:  printf("c"); break;
			case S_IFBLK:  printf("b"); break;
			case S_IFLNK:  printf("l"); break;
			case S_IFIFO:  printf("p"); break;
			case S_IFSOCK: printf("s"); break;
			default:       printf("?"); break; /* Should never happen (on Linux) */
		}
	}

	printf(" %s  ", (type == FTW_D)  ? "D  " : (type == FTW_DNR) ? "DNR" :
			(type == FTW_DP) ? "DP " : (type == FTW_F)   ? "F  " :
			(type == FTW_SL) ? "SL " : (type == FTW_SLN) ? "SLN" :
			(type == FTW_NS) ? "NS " : "  ");

	if (type != FTW_NS)
		printf("%7ld ", (long) sbuf->st_ino);
	else
		printf("        ");

	printf(" %*s", 4 * ftwb->level, "");        /* Indent suitably */
	//printf("%s\n",  &pathname[ftwb->base]);     /* Print basename */
	printf("%s\n",  &pathname[0]);     /* Print basename */

	return 0;                                   /* Tell nftw() to continue */
}
	int
main(int argc, char *argv[])
{
	int flags, opt;

	flags = 0;
	/*
	//For more details see man page of nftw.
	Possible values for the flag passed to fn are:

	FTW_F    A regular file.

	FTW_D    A directory being visited in pre-order.

	FTW_DNR  A directory which cannot be read.  The directory will not be descended into.

	FTW_DP   A directory being visited in post-order (nftw() only).

	FTW_NS   A file for which no stat(2) information was available.  The contents of the stat structure are undefined.

	FTW_SL   A symbolic link.

	FTW_SLN  A symbolic link with a non-existent target (nftw() only).

	The ftw() function traverses the tree in pre-order.  That is, it processes the directory before the directory's contents.

	The depth argument specifies the maximum number of file descriptors to keep open while traversing the tree.  It has no effect in this
	implementation.

	The nftw() function has an additional flags argument with the following possible values:

	FTW_PHYS   Physical walk, don't follow symbolic links.

	FTW_MOUNT  The walk will not cross a mount point.

	FTW_DEPTH  Process directories in post-order.  Contents of a directory are visited before the directory itself.  By default, nftw()
	traverses the tree in pre-order.

	FTW_CHDIR  Change to a directory before reading it.  By default, nftw() will change its starting directory.  The current working
	directory will be restored to its original value before nftw() returns.
	 */
	while ((opt = getopt(argc, argv, "dmp")) != -1) {
		switch (opt) {
			case 'd': flags |= FTW_DEPTH;   break;
			case 'm': flags |= FTW_MOUNT;   break;
			case 'p': flags |= FTW_PHYS;    break;
			default:  usageError(argv[0], NULL);
		}
	}

	if (argc > optind + 1)
		usageError(argv[0], NULL);

	if (nftw((argc > optind) ? argv[optind] : ".", dirTree, 10, flags) == -1) {
		perror("nftw");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
