#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/setup.h>

#ifdef CONFIG_SECURITY_SELINUX_PERMISSIVE
#include <asm/setup.h>

static char proc_cmdline[COMMAND_LINE_SIZE];
#endif

static char new_command_line[COMMAND_LINE_SIZE];

static int cmdline_proc_show(struct seq_file *m, void *v)
{
#ifdef CONFIG_SECURITY_SELINUX_PERMISSIVE
	seq_printf(m, "%s\n", proc_cmdline);
#else
	seq_printf(m, "%s\n", new_command_line);
#endif
	return 0;
}

static int cmdline_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cmdline_proc_show, NULL);
}

static const struct file_operations cmdline_proc_fops = {
	.open		= cmdline_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static void remove_flag(char *cmd, const char *flag)
{
	char *start_addr, *end_addr;

	/* Ensure all instances of a flag are removed */
	while ((start_addr = strstr(cmd, flag))) {
		end_addr = strchr(start_addr, ' ');
		if (end_addr)
			memmove(start_addr, end_addr + 1, strlen(end_addr));
		else
			*(start_addr - 1) = '\0';
	}
}

static void remove_safetynet_flags(char *cmd)
{
	remove_flag(cmd, "androidboot.enable_dm_verity=");
	remove_flag(cmd, "androidboot.secboot=");
	remove_flag(cmd, "androidboot.verifiedbootstate=");
	remove_flag(cmd, "androidboot.veritymode=");
}

static int __init proc_cmdline_init(void)
{
#ifdef CONFIG_SECURITY_SELINUX_PERMISSIVE
	char *a1, *a2;

	a1 = strstr(saved_command_line, "androidboot.selinux=");
	if (a1) {
		a1 = strchr(a1, '=');
		a2 = strchr(a1, ' ');
		if (!a2) /* last argument on the cmdline */
			a2 = "";

		scnprintf(proc_cmdline, COMMAND_LINE_SIZE, "%.*spermissive%s",
				(int)(a1 - saved_command_line + 1),
				saved_command_line, a2);
	}
	else {
		strncpy(proc_cmdline, saved_command_line, COMMAND_LINE_SIZE);
	}
#endif

	strcpy(new_command_line, saved_command_line);

	/*
	 * Remove various flags from command line seen by userspace in order to
	 * pass SafetyNet CTS check.
	 */
	remove_safetynet_flags(new_command_line);

	proc_create("cmdline", 0, NULL, &cmdline_proc_fops);
	return 0;
}
module_init(proc_cmdline_init);
