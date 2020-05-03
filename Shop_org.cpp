//
// Created by Melinda Johnson on 5/3/20.
//

#include <iostream> // cout
#include <sstream>  // stringstream
#include <string>   // string
#include "Shop_org.h"

void Shop_org::init( ) {
    pthread_mutex_init( &mutex, NULL );
    pthread_cond_init( &cond_customers_waiting, NULL );
    pthread_cond_init( &cond_customer_served, NULL );
    pthread_cond_init( &cond_barber_paid, NULL );
    pthread_cond_init( &cond_barber_sleeping, NULL );
}

string Shop_org::int2string( int i ) {
    stringstream out;
    out << i;
    return out.str( );
}

void Shop_org::print( int person, string message ) {
    cout << ( ( person != barber ) ? "customer[" : "barber  [" )
         << person << "]: " << message << endl;
}

bool Shop_org::visitShop( int id ) {
    pthread_mutex_lock( &mutex );   // lock

    if ( waiting_chairs.size( ) == max ) {  // waiting chairs are all occupied
        print( id,"leaves the shop because of no available waiting chairs.");
        ++nDropsOff;
        pthread_mutex_unlock( &mutex );
        return false;                 // leave the shop
    }
    if ( service_chair != 0 || !waiting_chairs.empty( ) ) {
        // someone is being served or transitting from a waiting to a service chair
        waiting_chairs.push( id );    // have a waiting chair

        print( id, "takes a waiting chair. # waiting seats available = "
                   + int2string( max - waiting_chairs.size( ) ) );
        pthread_cond_wait( &cond_customers_waiting, &mutex );
        waiting_chairs.pop( );        // stand up
    }

    print( id, "moves to the service chair. # waiting seats available = "
               + int2string( max - waiting_chairs.size( ) ) );
    service_chair = id;             // have the service chair
    in_service = true;

    // wake up the barber just in case if he is sleeping
    pthread_cond_signal( &cond_barber_sleeping );

    pthread_mutex_unlock( &mutex ); // unlock
    return true;
}

void Shop_org::leaveShop( int id ) {
    pthread_mutex_lock( &mutex );   // lock

    print( id, "wait for the hair-cut to be done" );
    while ( in_service == true )                           // while being served
        pthread_cond_wait( &cond_customer_served, &mutex );  // just sit.

    money_paid = true;
    pthread_cond_signal( &cond_barber_paid );

    print( id, "says good-bye to the barber." );
    pthread_mutex_unlock( &mutex ); // unlock
}

void Shop_org::helloCustomer( ) {
    pthread_mutex_lock( &mutex );   // lock

    if ( waiting_chairs.empty( ) && service_chair == 0 ) { // no customers
        print( barber, "sleeps because of no customers." );
        pthread_cond_wait( &cond_barber_sleeping, &mutex ); // then, let's sleep
    }

    if ( service_chair == 0 )               // check if the customer, sit down.
        pthread_cond_wait( &cond_barber_sleeping, &mutex );
    print( barber,
           "starts a hair-cut service for " + int2string( service_chair ) );

    pthread_mutex_unlock( &mutex );  // unlock
}

void Shop_org::byeCustomer( ) {
    pthread_mutex_lock( &mutex );    // lock

    in_service = false;
    print( barber, "says he's done with a hair-cut service for " +
                   int2string( service_chair ) );

    money_paid = false;
    pthread_cond_signal( &cond_customer_served );   // tell the customer "done"

    while ( money_paid == false )
        pthread_cond_wait( &cond_barber_paid, &mutex );

    service_chair = 0;
    print( barber, "calls in another customer" );
    pthread_cond_signal( &cond_customers_waiting ); // call in another one

    pthread_mutex_unlock( &mutex );  // unlock
}
