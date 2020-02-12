#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <linux/uinput.h>

#define BLACK   "\x1b[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[36m"
#define RESET   "\x1b[0m"

void emit(int fd, int type, int code, int val)
{
	struct input_event ie;

	ie.type = type;
	ie.code = code;
	ie.value = val;
	/* timestamp values below are ignored */
	ie.time.tv_sec = 0;
	ie.time.tv_usec = 0;

	int res = write(fd, &ie, sizeof(ie));
	printf("emit write bytes=%d fd=%d code=%d val=%d\n",res, fd, code, val);
}

int create_keyboard(int *key_fd)
{
	struct uinput_user_dev uud;
	int version, rc, fd;

	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	printf("fd=%d\n",fd);

	rc = ioctl(fd, UI_GET_VERSION, &version);
	printf("rd=%d\n",rc);

	if (rc == 0 && version >= 5)
	{
		printf("Error! version=%d\n",version);
		//return 0;
	}

	/*
	 * The ioctls below will enable the device that is about to be
	 * created, to pass key events.
	 */
	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_EVBIT, EV_SYN);

	for(int i = 0; i < 632; i++)
	{
		ioctl(fd, UI_SET_KEYBIT, i);
	}

	//printf("ioctl = %d, %d, %d ,%d , %d, %d\n", i1,i2,i3,i4,i5,i6);

	memset(&uud, 0, sizeof(uud));
	snprintf(uud.name, UINPUT_MAX_NAME_SIZE, "uinput-keyboard");
	uud.id.bustype = BUS_HOST;
	uud.id.vendor  = 0x1;
	uud.id.product = 0x2;
	uud.id.version = 1;

	write(fd, &uud, sizeof(uud));
	sleep(2);

	int i = ioctl(fd, UI_DEV_CREATE);
	printf("dev create =%d\n", i);
	sleep(2);

	*key_fd = fd;
	//printf("key_fd: %i\n", *key_fd);
	//printf("fd: %i\n", fd);
	return 0;
}

int version()
{
	printf("wl-virtual-keyboard v0.0.1 by JosephTheEngineer\n");
	return 0;
}

int help()
{
	printf(WHITE "Usage: " GREEN "wl-virtual-keyboard" RESET " <command> [<args>]\n");
        printf("\n");
        printf(WHITE "client  - the device being controlled"
        	"				default" RESET "\n");
        printf(BLUE "	-i" RESET " --ip		the ip that the client connects to"
        	GREEN "	127.0.0.1" RESET "\n");
        printf(BLUE "	-p" RESET " --port	the port that the client connects to"
        	GREEN "	8080" RESET "\n");
        printf(BLUE "	-v" RESET " --visual	stream the servers desktop"
        	GREEN "		false" RESET "\n");
        printf("\n");
        printf(WHITE "server  - the device controlling the clients"
        	"			default" RESET "\n");
        printf(BLUE "	-i" RESET " --ip		the ip that the sever listens for"
        	GREEN "	all" RESET "\n");
        printf(BLUE "	-p" RESET " --port	the port that the sever listens on"
        	GREEN "	8080 " RESET "\n");
        printf(BLUE "	-v" RESET " -visual	enable remote desktop mode"
        	GREEN "		false" RESET "\n");
        printf("\n");
        printf(WHITE "version - show the current version" RESET "\n");
        printf("\n");
        printf(WHITE "help    - show this help list" RESET "\n");
        printf("\n");
        printf("Report bugs to joseph@theengineer.life.\n");
	return 0;
}

int main(void)
{
	printf("Creating virtual keyboard...\n");
	int fd;
	create_keyboard(&fd);
	sleep(2);

	/* Key press, report the event, send key release, and report again */
	for(;;)
	{
		emit(fd, EV_KEY, KEY_D, 1);
		emit(fd, EV_SYN, SYN_REPORT, 1);
		sleep(1);
		emit(fd, EV_KEY, KEY_D, 0);
		emit(fd, EV_SYN, SYN_REPORT, 0);

		emit(fd, EV_KEY, KEY_U, 1);
		emit(fd, EV_SYN, SYN_REPORT, 0);
		emit(fd, EV_KEY, KEY_U, 0);
		emit(fd, EV_SYN, SYN_REPORT, 0);

		emit(fd, EV_KEY, KEY_P, 1);
		emit(fd, EV_SYN, SYN_REPORT, 0);
		emit(fd, EV_KEY, KEY_P, 0);
		emit(fd, EV_SYN, SYN_REPORT, 0);

		emit(fd, EV_KEY, KEY_A, 1);
		emit(fd, EV_SYN, SYN_REPORT, 0);
		emit(fd, EV_KEY, KEY_A, 0);
		emit(fd, EV_SYN, SYN_REPORT, 0);

		sleep(5);
	}
	ioctl(fd, UI_DEV_DESTROY);

	close(fd);
	return 0;
}

int main(int argc, char** argv)
{
	const int PRESS = 0;
	const int RELEASE = 1;
	int mode = PRESS;
	char code;
	char release;
	int c;

	opterr = 0;

	if (strcmp(argv[1], "press") == 0)
	{
		mode = PRESS;
	}
	else if (strcmp(argv[1], "release") == 0)
	{
		mode = RELEASE;
	}
	else if (strcmp(argv[1], "list-key-codes") == 0)
	{
		list-key-codes();
		return 0;
	}
	else if (strcmp(argv[1], "version") == 0)
	{
		version();
		return 0;
	}
	else if (strcmp(argv[1], "help") == 0)
	{
		help();
		return 0;
	}
	else
	{
		printf(RED "Invalid command %s." RESET "\n", argv[1]);
		abort();
	}

	// remove sub command to prepare for getopt
	for(int i = 0; i < argc-1; i++)
	{
		argv[i]=argv[i+1];
	}
	argc--;

	while ((c = getopt (argc, argv, "c:r:")) != -1)
		switch (c)
		{
			case 'c': // code
				code = optarg;
				break;
			case 'r': // release
				release = optarg;
				break;
			case '?':
				if (optopt == 'i' || optopt == 'p')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr,
							"Unknown option character `\\x%x'.\n",
							optopt);
				return 1;
			default:
				abort();
		}

	//printf ("aflag = %d, bflag = %d, cvalue = %s\n",
	//	aflag, bflag, cvalue);

	for (index = optind; index < argc; index++)
		printf ("Non-option argument %s\n", argv[index]);

	return 0;
}
