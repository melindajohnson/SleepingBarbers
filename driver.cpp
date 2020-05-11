#include <iostream>    // cout
#include <sys/time.h> // gettimeofday
#include <unistd.h>
#include "Shop.h"

using namespace std;

void *barber( void * );
void *customer( void * );

int main( int argc, char *argv[] ) {

    int nBarbers = atoi( argv[1] );
    int nChairs = atoi( argv[2] );
    int nCustomers = atoi( argv[3] );
    int serviceTime = atoi( argv[4] );


    pthread_t barber_thread[nBarbers];
    pthread_t customer_threads[nCustomers];
    Shop shop(nBarbers, nChairs );


    //spawn nBarber number of threads
    for (int i = 0; i < nBarbers; i++) {
        Shop::ThreadParam *param = new Shop::ThreadParam(&shop, i, serviceTime);
        pthread_create(&barber_thread[i], NULL, barber, (void *) param);
    }

    //spawn nCustomer number of threads
    for (int i = 0; i < nCustomers; i++) {
        usleep(rand() % 1000);
        Shop::ThreadParam *param = new Shop::ThreadParam(&shop, i + 1,0);
        pthread_create(&customer_threads[i], NULL, customer, (void *) param);

    }
    //join all nCustomer threads
    for (int i = 0; i < nCustomers; i++) {
        pthread_join(customer_threads[i], NULL);
    }
    for (int i = 0; i < nBarbers; i++) {
        pthread_cancel(barber_thread[i]);
    }
    cout << "# customers who didn't receive a service = " << shop.nDropsOff
         << endl;

    return 0;
}

void *barber( void *arg ) {


    Shop::ThreadParam &param = *(Shop::ThreadParam *)arg;
    Shop &shop = *(param.shop);
    int id = param.id;
    int serviceTime = param.serviceTime;
    delete &param;

    while( true ) {

        shop.helloCustomer(id );
        usleep( serviceTime );
        shop.byeCustomer(id );
    }
}

void *customer( void *arg ) {

    Shop::ThreadParam &param = *(Shop::ThreadParam *)arg;
    Shop &shop = *(param.shop);
    int id = param.id;
    delete &param;
// if assigned to barber i then wait for service to finish
    // -1 means did not get barber
    int barber = -1;
    barber = shop.visitShop(id);
    if (barber != -1)
    {
        shop.leaveShop( id, barber ); // wait until my service is finished
    }
    pthread_exit(NULL);

}
