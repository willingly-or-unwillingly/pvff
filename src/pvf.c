#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <libprocstat.h>
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

void print_remaining_time(double speed, long remaining_bytes) {
    if (speed > 0) {
        double remaining_time_sec = remaining_bytes / speed;
        int hours = (int)(remaining_time_sec / 3600);
        int minutes = (int)((remaining_time_sec - (hours * 3600)) / 60);
        int seconds = (int)(remaining_time_sec - (hours * 3600) - (minutes * 60));

        printf("Estimated time remaining: %02d:%02d:%02d\r", hours, minutes, seconds);
        fflush(stdout);
    }
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
    long previous_pos = 0;
    time_t previous_time = time(NULL);

    do {
        struct procstat *procstat;
        struct kinfo_proc *procs;
        unsigned int nprocs;
        int found = 0;
        long pos = 0;

        procstat = procstat_open_sysctl();
        if (procstat == NULL) {
            perror("procstat_open_sysctl");
            return 1;
        }

        procs = procstat_getprocs(procstat, KERN_PROC_PROC, 0, &nprocs);
        if (procs == NULL) {
            perror("procstat_getprocs");
            procstat_close(procstat);
            return 1;
        }

        for (unsigned int i = 0; i < nprocs; i++) {
            struct filestat_list *head;
            struct filestat *fst;

            head = procstat_getfiles(procstat, &procs[i], 0);
            if (head == NULL) {
                continue;
            }

            STAILQ_FOREACH(fst, head, next) {
                if (fst->fs_path != NULL && strcmp(fst->fs_path, filename) == 0) {
                    found = 1;
                    pos = fst->fs_offset;
                    break;
                }
            }

            procstat_freefiles(procstat, head);

            if (found) {
                break;
            }
        }

        procstat_freeprocs(procstat, procs);
        procstat_close(procstat);

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

            // Calculate speed and estimate remaining time
            time_t current_time = time(NULL);
            double time_diff = difftime(current_time, previous_time);

            if (time_diff > 0) {
                long bytes_read = pos - previous_pos;
                double speed = bytes_read / time_diff; // bytes per second
                long remaining_bytes = total_size - pos;
                print_remaining_time(speed, remaining_bytes);
            }

            previous_pos = pos;
            previous_time = current_time;

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
