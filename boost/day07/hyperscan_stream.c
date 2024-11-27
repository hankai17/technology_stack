#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <hs/hs.h>

int global_hit = 0;

int eventHandler(unsigned int id, unsigned long long from,
    unsigned long long to, unsigned int flags, void *ctx)
{
    printf("Match found at from %llu to %llu id %u\n", from, to, id);
    global_hit++;
    return 0;
}

int test1()
{
    hs_database_t *database;
    hs_compile_error_t *compile_err;
    hs_error_t err;

    // Compile the regular expression
    const char *regex = "hello|world";;

    err = hs_compile(regex, HS_FLAG_CASELESS, HS_MODE_STREAM,
        NULL, &database, &compile_err);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error compiling pattern: %s\n",
            compile_err->message);
        hs_free_compile_error(compile_err);
        return 1;
    }

    hs_stream_t *stream = NULL;
    err = hs_open_stream(database, 0, &stream);
    if (err != HS_SUCCESS) {
        printf("err: %d\n", err);
        return 1;
    }

    hs_scratch_t *scratch = NULL;
    err = hs_alloc_scratch(database, &scratch);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error allocating scratch space\n");
        hs_free_database(database);
        return 1;
    }

    //const char *input = "hello";
    const char *input = "world";

    // Scan the input
    size_t matchCount = 0;
    err = hs_scan_stream(stream, input, strlen(input), 0, scratch,
        eventHandler, &matchCount);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error scanning input\n");
        hs_free_scratch(scratch);
        hs_free_database(database);
        return 1;
    }

    // Clean up
    hs_free_scratch(scratch);
    hs_free_database(database);

    return 0;
}

int test2()
{
    hs_database_t *database;
    hs_stream_t *stream;
    hs_error_t err;

    // Compile regular expressions
    const char *patterns[] = {"hello"};
    const unsigned int ids[] = {1};
    unsigned int num_patterns = sizeof(patterns) / sizeof(const char *);
    hs_compile_error_t *compile_err;
    err = hs_compile_multi(patterns, NULL, ids, num_patterns, HS_MODE_STREAM,
        NULL, &database, &compile_err);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error compiling pattern: %s\n", compile_err->message);
        hs_free_compile_error(compile_err);
        return -1;
    }

    hs_scratch_t *scratch = NULL;
    err = hs_alloc_scratch(database, &scratch);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error allocating scratch space\n");
        hs_free_database(database);
        return 1;
    }

    // Create stream
    err = hs_open_stream(database, 0, &stream);
    if (err != HS_SUCCESS) {
        fprintf(stderr, "Error creating stream: %d\n", err);
        hs_free_database(database);
        return -1;
    }

    const char *input = "world";
    // Scan input string
    {
        err = hs_scan_stream(stream, input, strlen(input), 0, scratch, eventHandler, NULL);
        if (err != HS_SUCCESS) {
            fprintf(stderr, "Error scanning stream: %d\n", err);
			return 0;
        }
    }

    // Close stream and free resources
    hs_close_stream(stream, NULL, NULL, NULL);
    hs_free_database(database);

    return 0;
}

int main()
{
    test1();
    //test2();
    printf("global_hit: %d\n", global_hit);
}
