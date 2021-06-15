#ifndef USER_INTERFACE_
#define USER_INTERFACE_



class Xvd_Metadata
{
    public:
        void xvd_metadata_query_init();
        void xvd_metadata_query_release();

        void xvd_metadata_query_timestamp();
        void xvd_metadata_query_platform_attitude();
        void xvd_metadata_query_sensor_position();
        void xvd_metadata_query_sensor_attitude();
        void xvd_metadata_query_fov();
        uint64_t time_unix_usec = 0;
        
    private:
        

};


#endif