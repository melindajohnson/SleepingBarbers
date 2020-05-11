#include <iostream> // cout
#include <sstream>  // stringstream
#include <string>   // string
#include "Shop.h"

void Shop::init( ) {
    service_chair = new int[barber];          // indicate the current customer thread id
    in_service = new bool[barber];
    money_paid = new bool[barber];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_customers_waiting, NULL);
    cond_customer_served = new pthread_cond_t[barber];
    cond_barber_paid = new pthread_cond_t[barber];
    cond_barber_sleeping = new pthread_cond_t[barber];


    for (int i = 0; i < barber; i++) {
        service_chair[i] = 0;
        in_service[i] = false;
        money_paid[i] = false;
        pthread_cond_init(&cond_customer_served[i], NULL);
        pthread_cond_init(&cond_barber_paid[i], NULL);
        pthread_cond_init(&cond_barber_sleeping[i], NULL);
    }
}
Shop::~Shop(){

    delete [] service_chair;
    service_chair = NULL;
    delete [] in_service;
    in_service = NULL;
    delete [] money_paid;
    money_paid = NULL;
    delete [] cond_customer_served;
    cond_customer_served = NULL;
    delete [] cond_barber_paid;
    cond_barber_paid = NULL;
    delete [] cond_barber_sleeping;
    cond_barber_sleeping = NULL;

}
string Shop::int2string( int i ) {
    stringstream out;
    out << i;
    return out.str( );
}

void Shop::print( int person, string message ) {
    cout << ( ( person > 0 ) ? "customer[" : "barber  [" )
         << abs(person) << "]: " << message << endl;
}

int Shop::visitShop( int customerID ) {
    pthread_mutex_lock( &mutex );   // lock
    if ( waiting_chairs.size( ) == max ) {  // waiting chairs are all occupied
        print( customerID,"leaves the shop because of no available waiting chairs.");
        ++nDropsOff;
        pthread_mutex_unlock( &mutex );
        return -1;                 // leave the shop
    }
    if ( hasServiceChair() == -1 || !waiting_chairs.empty( ) ) {
        // someone is being served or transitting from a waiting to a service chair
        waiting_chairs.push( customerID );    // have a waiting chair

        print( customerID, "takes a waiting chair. # waiting seats available = "
                   + int2string( max - waiting_chairs.size( ) ) );
        pthread_cond_wait( &cond_customers_waiting, &mutex );
        waiting_chairs.pop( );        // stand up
    }

    print( customerID, "moves to the service chair. # waiting seats available = "
               + int2string( max - waiting_chairs.size( ) ) );


    int availableServiceChair = hasServiceChair();
    if ( service_chair[availableServiceChair] == 0) {
        service_chair[availableServiceChair] = customerID;             // have the service chair
        in_service[availableServiceChair] = true;
    }

    // wake up the barber just in case if he is sleeping based on service chair
    pthread_cond_signal( &cond_barber_sleeping[availableServiceChair] );

    pthread_mutex_unlock( &mutex ); // unlock
    return availableServiceChair;
}

void Shop::leaveShop(int customerId, int barberId ) {
    pthread_mutex_lock( &mutex );   // lock
    print( customerId,"wait for barber[" + int2string( barberId ) + "] to be done with haircut" );
    while ( in_service[barberId] == true )                           // while being served
        pthread_cond_wait( &cond_customer_served[barberId], &mutex );  // just sit.
    money_paid[barberId] = true;
    pthread_cond_signal( &cond_barber_paid[barberId] );

    print( customerId, "says good-bye to the barber." );
    pthread_mutex_unlock( &mutex ); // unlock

}

void Shop::helloCustomer(int barberId ) {
    pthread_mutex_lock( &mutex );   // lock

    if ( waiting_chairs.empty( ) && service_chair[barberId] == 0 ) { // no customers
        print( -barberId, "sleeps because of no customers." );
        pthread_cond_wait( &cond_barber_sleeping[barberId], &mutex ); // then, let's sleep
    }

    if ( service_chair[barberId] == 0 ) {       // check if the customer, sit down.
        pthread_cond_wait(&cond_barber_sleeping[barberId], &mutex);
    }

    print( -barberId,"starts a hair-cut service for customer[" + int2string( service_chair[barberId] ) + "]" );

    pthread_mutex_unlock( &mutex );  // unlock
}

void Shop::byeCustomer(int barberId ) {
    pthread_mutex_lock( &mutex );    // lock
    in_service[barberId] = false;
    print( -barberId, "says he's done with a hair-cut service for " +
                   int2string( service_chair[barberId] ) );

    money_paid[barberId] = false;
    pthread_cond_signal( &cond_customer_served[barberId] );   // tell the customer "done"

    while ( money_paid[barberId] == false )
        pthread_cond_wait( &cond_barber_paid[barberId], &mutex );

    service_chair[barberId] = 0;
    print( -barberId, "calls in another customer" );
    pthread_cond_signal( &cond_customers_waiting ); // call in another one

    pthread_mutex_unlock( &mutex );  // unlock
}

int Shop::hasServiceChair(){
    for(int i=0; i< barber; i++){
        if(service_chair[i]==0){
            return i;
        }
    }
    return -1;
}