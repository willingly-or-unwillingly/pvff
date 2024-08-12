#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define BAR_WIDTH 50

void usage(const char *progname) {
    printf("Usage: %s [--watch=NNN | -w NNN] <filename>\n", progname);
    exit(1);
}

long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("stat");
        return -1;
    }
    return st.st_size;
}

void print_progress_bar(double percentage, int spin_index) {
    int filled = (int)(percentage / 100 * BAR_WIDTH);
    char spinner[] = {'|', '/', '-', '\\'};

    printf("\r[");
    for (int i = 0; i < BAR_WIDTH; i++) {
        if (i < filled) {
            printf("#");
        } else {
            printf(" ");
        }
    }
    printf("] %.2f%% %c ", percentage, spinner[spin_index % 4]);
    fflush(stdout);
}

void print_completion_time() {
    time_t current_time;
    char time_string[100];

    time(&current_time);
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
    printf("\nProcess completed at: %s\n", time_string);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
    }

    const char *filename = NULL;
    int watch_interval = 0;

    // Parse arguments to find the filename and watch interval
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--watch=", 8) == 0) {
            watch_interval = atoi(argv[i] + 8);
        } else if (strcmp(argv[i], "-w") == 0) {
            if (i + 1 < argc) {
                watch_interval = atoi(argv[++i]);
            } else {
                usage(argv[0]); // Missing value for -w
            }
        } else if (filename == NULL) {
            filename = argv[i];
        } else {
            usage(argv[0]); // Invalid argument sequence
        }
    }

    if (!filename) {
        usage(argv[0]);
    }

    int spin_index = 0;
//    double last_percentage = -1.0;

    do {
        DIR *proc_dir;
        struct dirent *proc_entry;
        char fd_path[256];
        char link_path[256];
        char resolved_path[256];
        int found = 0;
        long pos = 0;

        proc_dir = opendir("/proc");
        if (!proc_dir) {
            perror("opendir");
            return 1;
        }

        // Iterate through the /proc directory to find process directories
        while ((proc_entry = readdir(proc_dir)) != NULL) {
            // Check if the entry is a PID directory
            if (proc_entry->d_type == DT_DIR && atoi(proc_entry->d_name) > 0) {
                snprintf(fd_path, sizeof(fd_path), "/proc/%s/fd", proc_entry->d_name);
                DIR *fd_dir = opendir(fd_path);

                if (!fd_dir) {
                    continue;
                }

                struct dirent *fd_entry;
                // Iterate through the file descriptors of each process
                while ((fd_entry = readdir(fd_dir)) != NULL) {
                    snprintf(link_path, sizeof(link_path), "%s/%s", fd_path, fd_entry->d_name);
                    ssize_t len = readlink(link_path, resolved_path, sizeof(resolved_path) - 1);
                    if (len != -1) {
                        resolved_path[len] = '\0';
                        if (strcmp(resolved_path, filename) == 0) {
                            found = 1;

                            // Read position from /proc/[pid]/fdinfo/[fd]
                            char fdinfo_path[256];
                            snprintf(fdinfo_path, sizeof(fdinfo_path), "/proc/%s/fdinfo/%s", proc_entry->d_name, fd_entry->d_name);
                            FILE *fdinfo_file = fopen(fdinfo_path, "r");
                            if (fdinfo_file) {
                                char line[256];
                                while (fgets(line, sizeof(line), fdinfo_file)) {
                                    if (sscanf(line, "pos: %ld", &pos) == 1) {
                                        break;
                                    }
                                }
                                fclose(fdinfo_file);
                            }
                            break;
                        }
                    }
                }
                closedir(fd_dir);

                if (found) {
                    break;
                }
            }
        }
        closedir(proc_dir);

        if (!found) {
            printf("File is not open by any process or cannot be accessed.\n");
            fflush(stdout);
            return 1;
        }

        // Get the total size of the file
        long total_size = get_file_size(filename);
        if (total_size < 0) {
            return 1;
        }

        // Calculate the position as a percentage
        if (total_size > 0) {
            double percentage = (double)pos / total_size * 100;
            print_progress_bar(percentage, spin_index);
//            if (last_percentage != percentage) 
//                    spin_index++;
//            last_percentage = percentage;

            // Check if the file is completely read
            if (percentage >= 100.0) {
                print_completion_time();
                return 0;
            }
        } else {
            printf("File size is zero or cannot be accessed.\n");
            fflush(stdout);
        }

        spin_index++;
        if (watch_interval > 0) {
            sleep(watch_interval);
        }
    } while (watch_interval > 0);

    if (watch_interval == 0) {
        printf("\n"); // Ensure the progress bar completes the line in non-watch mode
    }

    return 0;
}
