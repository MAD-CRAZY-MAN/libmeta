#include "main.h"

int main()
{
    xvd_metadata_query_init();
    xvd_metadata_query_release();

    uint64_t unix_time_stamp;

    while(1)
    {
        unix_time_stamp = xvd_metadata_query_timestamp();
        printf("unix_time_stamp: %ld", unix_time_stamp);
        sleep(1);
    }
}