#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <hs/hs.h>

int eventHandler(unsigned int id, unsigned long long from,
    unsigned long long to, unsigned int flags, void *ctx)
{
    //printf("Match found at from %llu to %llu id %u\n", from, to, id);
    return 0;
}

int test1()
{
    hs_database_t *database;
    hs_compile_error_t *compile_err;
    hs_error_t err;

    // Compile the regular expression
    const char *regex = "hello|world";;
    err = hs_compile(regex, HS_FLAG_CASELESS, HS_MODE_BLOCK,
        NULL, &database, &compile_err);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error compiling pattern: %s\n",
            compile_err->message);
        hs_free_compile_error(compile_err);
        return 1;
    }

    hs_scratch_t *scratch = NULL;
    err = hs_alloc_scratch(database, &scratch);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error allocating scratch space\n");
        hs_free_database(database);
        return 1;
    }

    struct  timeval   tv_begin,tv_end;
    gettimeofday(&tv_begin,NULL);

    int i = 0;
    while (i < 1000000 * 10) {

        // Scan the input
        const char *input = "world";
        //const char *input = "hello";
        err = hs_scan(database, input, strlen(input), 0, scratch,
            eventHandler, NULL);
        if (err != HS_SUCCESS) {
            fprintf(stderr, "Error scanning input\n");
            hs_free_scratch(scratch);
            hs_free_database(database);
            return 1;
        }
        i++;
    }

    gettimeofday(&tv_end,NULL);
    float time_use = (tv_end.tv_sec-tv_begin.tv_sec)*1000000+(tv_end.tv_usec-tv_begin.tv_usec);
    printf("time use: %fn us", time_use);
    // Clean up
    hs_free_scratch(scratch);
    hs_free_database(database);

    return 0;
}

int main()
{
    test1();
}