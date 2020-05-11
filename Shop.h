#ifndef _SHOP_ORG_H_
#define _SHOP_ORG_H_
#include <pthread.h>
#include <queue>

using namespace std;

#define DEFAULT_CHAIRS 3 	// the default number of chairs for waiting = 3
#define DEFAULT_BARBERS 1 	// the default number of barbers = 1

class Shop {
public:
    Shop(int nBarbers, int nChairs ) :barber( ( nBarbers > 0 ) ? nBarbers : DEFAULT_BARBERS ),
            max( ( nChairs > 0 ) ? nChairs : DEFAULT_CHAIRS ), service_chair( 0 ), nDropsOff( 0 )
    { init( ); };

    Shop( ) : barber(DEFAULT_BARBERS),max( DEFAULT_CHAIRS ), service_chair( 0 ), nDropsOff( 0 )
    { init( ); };

   virtual  ~ Shop();

    int visitShop(int customerID );   // return true only when a cluster got a service
    void leaveShop(int customerId, int barberId);
    void helloCustomer(int barberId );
    void byeCustomer(int barberId );
    int nDropsOff;              // the number of customers dropped off

    class ThreadParam {
    public:
        ThreadParam( Shop *shop, int id, int serviceTime ) : shop( shop ), id( id ), serviceTime( serviceTime ) { };
        Shop *shop; // a pointer to the Shop object
        int id; // a thread identifier
        int serviceTime;// service time (usec) for barber; 0 for customer
    };

private:
    const int max;              // the max number of threads that can wait
    const int barber; // the id of the barber thread

    int *service_chair;          // indicate the current customer thread id
    bool *in_service;
    bool *money_paid;
    pthread_cond_t  *cond_customer_served;
    pthread_cond_t  *cond_barber_paid;
    pthread_cond_t  *cond_barber_sleeping;

    queue<int> waiting_chairs;  // includes the ids of all waiting threads

    pthread_mutex_t mutex;
    pthread_cond_t  cond_customers_waiting;

    void init( );
    string int2string( int i );
    void print( int person, string message );
    int hasServiceChair();
};

#endif
