//
// Created by Melinda Johnson on 5/3/20.
//
#include <iostream>    // cout
#include <sys/time.h> // gettimeofday
#include "Shop_org.h"

using namespace std;

void *barber( void * );
void *customer( void * );

int main( int argc, char *argv[] ) {

    int nChairs = atoi( argv[1] );
    int nCustomers = atoi( argv[2] );
    int serviceTime = atoi( argv[3] );

    pthread_t barber_thread;
    pthread_t customer_threads[nCustomers];
    Shop_org shop( nChairs );

    void *barber_param[2] = { (void *)&shop, (void *)&serviceTime };
    pthread_create( &barber_thread, NULL, barber, barber_param );
    for ( int i = 0; i < nCustomers; i++ ) {
        usleep( rand( ) % 1000 );
        int id = i + 1;
        void *customer_param[2] = { (void *)&shop, (void *)&id };
        pthread_create( &customer_threads[i], NULL, customer, customer_param );
    }

    for ( int i = 0; i < nCustomers; i++ )
        pthread_join( customer_threads[i], NULL );

    pthread_cancel( barber_thread );
    cout << "# customers who didn't receive a service = " << shop.nDropsOff
         << endl;

    return 0;
}

void *barber( void *arg ) {

    void **args = ( void **)arg;
    Shop_org &shop = *(Shop_org *)args[0];
    int serviceTime = *(int *)args[1];

    while( true ) {
        shop.helloCustomer( );
        usleep( serviceTime );
        shop.byeCustomer( );
    }
}

void *customer( void *arg ) {

    void **args = ( void **)arg;
    Shop_org &shop = *(Shop_org *)args[0];
    int id = *(int *)args[1];

    if ( shop.visitShop( id ) == true )
        shop.leaveShop( id );
}
