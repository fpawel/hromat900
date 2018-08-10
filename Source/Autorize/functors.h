#ifndef FUNCTORS_INCLUDED
#define FUNCTORS_INCLUDED

namespace functors {
    
struct die
{
    template <class T>
    void operator()( T* p ) {
        if ( p )
            delete p;
    };
};


#define EQUAL_BY_( FieldType, fieldAccessor, MacrosNumber ) struct EqualBy_##MacrosNumber\
{   \
    EqualBy_##MacrosNumber( const FieldType &the_value ) { \
        m_field_value = the_value; \
    } \
    template <class T> \
    bool operator()( T &r ) { \
        return ( m_field_value == r fieldAccessor ); \
    }\
private:\
    FieldType m_field_value;\
}

#define EQUAL_BY( FieldType, fieldAccessor ) EQUAL_BY_( FieldType, fieldAccessor, 0 )
#define EQUAL_BY_2( FieldType, fieldAccessor ) EQUAL_BY_( FieldType, fieldAccessor, 1 )
#define EQUAL_BY_3( FieldType, fieldAccessor ) EQUAL_BY_( FieldType, fieldAccessor, 2 )
#define EQUAL_BY_4( FieldType, fieldAccessor ) EQUAL_BY_( FieldType, fieldAccessor, 3 )
#define EQUAL_BY_5( FieldType, fieldAccessor ) EQUAL_BY_( FieldType, fieldAccessor, 4 )

#define LESS_BY( fieldAccessor ) struct\
{ \
    template<typename T> \
    bool operator()( T &l, T &r ) { \
        return l fieldAccessor < r fieldAccessor; \
    } \
}

#define GREATER_BY( fieldAccessor ) struct\
{ \
    template<typename T> \
    bool operator()( T &l, T &r ) { \
        return l fieldAccessor > r fieldAccessor; \
    } \
}


};

typedef EQUAL_BY( std::string, .name ) EqualByName;

#endif