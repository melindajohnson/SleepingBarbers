//
// Created by Melinda Johnson on 5/3/20.
//

#ifndef _SHOP_ORG_H_
#define _SHOP_ORG_H_
#include <pthread.h>
#include <queue>

using namespace std;

#define DEFAULT_CHAIRS 3

class Shop_org {
public:
    Shop_org( int nChairs ) :
            max( ( nChairs > 0 ) ? nChairs : DEFAULT_CHAIRS ), service_chair( 0 ),
            in_service( false ), money_paid( false ), nDropsOff( 0 )
    { init( ); };
    Shop_org( ) : max( DEFAULT_CHAIRS ), service_chair( 0 ), in_service( false ),
                  money_paid( false ), nDropsOff( 0 )
    { init( ); };

    bool visitShop( int id );   // return true only when a cluster got a service
    void leaveShop( int id );
    void helloCustomer( );
    void byeCustomer( );
    int nDropsOff;              // the number of customers dropped off

private:
    const int max;              // the max number of threads that can wait
    int service_chair;          // indicate the current customer thread id
    bool in_service;
    bool money_paid;
    queue<int> waiting_chairs;  // includes the ids of all waiting threads

    pthread_mutex_t mutex;
    pthread_cond_t  cond_customers_waiting;
    pthread_cond_t  cond_customer_served;
    pthread_cond_t  cond_barber_paid;
    pthread_cond_t  cond_barber_sleeping;

    static const int barber = 0; // the id of the barber thread

    void init( );
    string int2string( int i );
    void print( int person, string message );
};

#endif
