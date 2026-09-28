#include "file_manager.h"
#include "common.h"
#include <errno.h>
#include <sys/stat.h>

#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(p) _mkdir(p)
#else
  #define MKDIR(p) mkdir(p, 0755)
#endif

int fm_init(void)
{
    if (MKDIR(DATA_DIR) != 0 && errno != EEXIST) {
        perror("Cannot create data folder");
        return -1;
    }
    return 0;
}

int fm_save(const char *file, const void *data, size_t size, int count)
{
    FILE *fp = fopen(file, "wb");
    if (!fp) { perror("Cannot open file for writing"); return -1; }

    if (count > 0 && fwrite(data, size, (size_t)count, fp) != (size_t)count) {
        perror("Write error");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

void *fm_load(const char *file, size_t size, int *count)
{
    long bytes;
    void *buf;
    FILE *fp;

    *count = 0;
    fp = fopen(file, "rb");
    if (!fp) return NULL;               /* first run: file does not exist, not an error */

    fseek(fp, 0, SEEK_END);
    bytes = ftell(fp);
    rewind(fp);

    if (bytes < (long)size) { fclose(fp); return NULL; }

    buf = malloc((size_t)bytes);
    if (!buf) { printf("Memory allocation failed!\n"); fclose(fp); return NULL; }

    *count = (int)fread(buf, size, (size_t)(bytes / (long)size), fp);
    fclose(fp);
    return buf;
}
