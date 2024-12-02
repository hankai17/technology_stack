#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <hs/hs.h>

int global_hit = 0;

int eventHandler(unsigned int id, unsigned long long from,
    unsigned long long to, unsigned int flags, void *ctx)
{
    //printf("Match found at from %llu to %llu id %u\n", from, to, id);
    global_hit++;
    return 0;
}

#define PATTERN_COUNT 1000
#define TEST_COUNT 1
const char *str = "User-Agent: <script>alert(12345)</script>";
//const char *str = "This is a test string";

int test1()
{
    // The file containing the patterns to search for
    const char *pattern_file = "rule";

    // Open the pattern file
    FILE *fp = fopen(pattern_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening pattern file\n");
        return 1;
    }

    // Read the patterns from the file into an array
    char **patterns = malloc(PATTERN_COUNT * sizeof(char *));
    if (patterns == NULL) {
        fclose(fp);
        return 1;
    }

    char buffer[1024 * 16];
    int i = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL && i < PATTERN_COUNT) {
        // Remove newline character from the end of the pattern
        size_t len = strlen(buffer);
        if (len > 1024) {
            continue;
        }
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        //printf("%s\n", buffer);
        // Copy the pattern into the array
        patterns[i] = strdup(buffer);
        if (patterns[i] == NULL) {
            fclose(fp);
            return 1;
        }
        i++;
        if (i == 60) {
            //break;
        }
    }
    fclose(fp);
    printf("i: %d\n", i);
    unsigned int ids[PATTERN_COUNT] = {0};
    int j = 0;
    for (j = 0; j < i; j++) {
        ids[j] = j;
    }

    // Compile the regular expressions
    hs_database_t *database;
    hs_compile_error_t *compile_err;
    hs_error_t err = hs_compile_multi(patterns, NULL, ids, i,
        HS_MODE_BLOCK, NULL, &database, &compile_err);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error compiling patterns: %s\n", compile_err->message);
        hs_free_compile_error(compile_err);
        for (int j = 0; j < i; j++) {
            free(patterns[j]);
        }
        free(patterns);
        return 1;
    }

    // Allocate scratch space
    hs_scratch_t *scratch;
    err = hs_alloc_scratch(database, &scratch);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error allocating scratch space\n");
        hs_free_database(database);
        for (int j = 0; j < i; j++) {
            free(patterns[j]);
        }
        free(patterns);
        return 1;
    }

    // The string to search for matches in

    struct  timeval   tv_begin,tv_end;
    gettimeofday(&tv_begin,NULL);

    // Search for matches in the string
    i = 0;
    while (i < 10000 * TEST_COUNT) {
        err = hs_scan(database, str, strlen(str), 0, scratch, eventHandler, NULL);
        if (err != HS_SUCCESS) {
            fprintf(stderr, "Error scanning string\n");
            hs_free_scratch(scratch);
            hs_free_database(database);
            for (int j = 0; j < i; j++) {
                free(patterns[j]);
            }
            free(patterns);
            return 1;
        }
        i++;
    }
    gettimeofday(&tv_end,NULL);
    float time_use = (tv_end.tv_sec-tv_begin.tv_sec)*1000000+(tv_end.tv_usec-tv_begin.tv_usec);
    printf("global_hit: %d\n", global_hit);
    printf("time use: %fn us\n", time_use);

    return 0;

    // Free resources
    hs_free_scratch(scratch);
    hs_free_database(database);
    for (int j = 0; j < i; j++) {
        free(patterns[j]);
    }
    free(patterns);
    return 0;
}

struct h_db {
    hs_database_t *database;
    hs_scratch_t *scratch;
};

struct h_db* gen_database(char *regex)
{
    hs_database_t *database;
    hs_compile_error_t *compile_err;
    hs_error_t err;

    err = hs_compile(regex, HS_FLAG_CASELESS, HS_MODE_BLOCK,
        NULL, &database, &compile_err);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error compiling pattern: %s\n",
            compile_err->message);
        hs_free_compile_error(compile_err);
        return NULL;
    }

    hs_scratch_t *scratch = NULL;
    err = hs_alloc_scratch(database, &scratch);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error allocating scratch space\n");
        hs_free_database(database);
        return NULL;
    }

    struct h_db * db = malloc(sizeof(struct h_db));
    db->database = database;
    db->scratch = scratch;
    return db;
}

int test2()
{
    // The file containing the patterns to search for
    const char *pattern_file = "rule";
    hs_error_t err;

    // Open the pattern file
    FILE *fp = fopen(pattern_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening pattern file\n");
        return 1;
    }

    struct h_db* dbs[1024] = {0};
    char buffer[1024 * 16];
    int i = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL && i < PATTERN_COUNT) {
        // Remove newline character from the end of the pattern
        size_t len = strlen(buffer);
        if (len > 1024) {
            continue;
        }
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        //printf("%s\n", buffer);
        // Copy the pattern into the array
        struct h_db *db = gen_database(buffer);
        if (db == NULL) {
            printf("compile failed\n");
            continue;
        } 
        dbs[i] = db;
        i++;
    }
    fclose(fp);
    printf("i: %d\n", i);

    // The string to search for matches in

    struct  timeval   tv_begin,tv_end;
    gettimeofday(&tv_begin,NULL);

    // Search for matches in the string
    i = 0;
    while (i < 10000 * TEST_COUNT) {
        int rule_count = 0;
        int j = 0;
        for (j = 0; j < 1024; j++) {
            struct h_db* db = dbs[j];
            if (db == NULL) {
                break;
            }
            hs_database_t *database = db->database;
            hs_scratch_t *scratch = db->scratch;
            err = hs_scan(database, str, strlen(str), 0, scratch,
                eventHandler, NULL);
            if (err != HS_SUCCESS) {
                fprintf(stderr, "Error scanning str\n");
                //hs_free_scratch(scratch);
                hs_free_database(database);
                return 1;
            }
        }
        i++;
    }
    gettimeofday(&tv_end,NULL);
    float time_use = (tv_end.tv_sec-tv_begin.tv_sec)*1000000+(tv_end.tv_usec-tv_begin.tv_usec);
    printf("global_hit: %d\n", global_hit);
    printf("time use: %fn us\n", time_use);

    return 0;

    // Free resources
    //hs_free_scratch(scratch);
    //hs_free_database(database);
    return 0;
}

int main()
{
    test1();
    global_hit = 0;
    test2();
    return 0;
}
