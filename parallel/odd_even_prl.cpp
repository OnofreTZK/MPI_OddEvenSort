#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <random>
#include <iomanip>

// ----------------------------------------------------------------------------------------------------------------
// Generating the same random numbers( pseudo-random )
// ----------------------------------------------------------------------------------------------------------------
long int gen_numbers( long int range, long int seed )
{ 
      std::mt19937 gen( seed );

      std::uniform_int_distribution<long int> distr(0, range);

      return distr( gen );
}


// ----------------------------------------------------------------------------------------------------------------
// Generating array with random numbers
// ----------------------------------------------------------------------------------------------------------------
long int * gen_array( long int size )
{
    long int * temp_arr = new long int[size];

    for ( int i = 0; i < size; i++ )
    {
        temp_arr[i] = gen_numbers(size, i);
    }
    
    return temp_arr;
}


// ----------------------------------------------------------------------------------------------------------------
// Swaping values
// ----------------------------------------------------------------------------------------------------------------
void swap( long int * a, long int * b)
{

    long int temp = *a;
    *a = *b;
    *b = temp;

}


// ----------------------------------------------------------------------------------------------------------------
// Sort function
// ----------------------------------------------------------------------------------------------------------------
void odd_even_sort( long int * arr, long int size )
{
    long int phase, itr; // Kayo suggestions.

    for( phase = 0; phase < size; phase++ ) // Sorting loop
    {
        // Even
        if( phase % 2 == 0 )
        {
            for( itr = 1; itr < size; itr += 2 )
            {
                if( arr[itr - 1] > arr[itr] ) // iterator with iterator - 1
                {
                    swap( &arr[itr - 1], &arr[itr] );   
                }
            }
        }
        else // Odd
        {
            for( itr = 1; itr < size - 1; itr += 2 )
            {
                if( arr[itr] > arr[itr + 1] ) // iterator with iterator + 1
                {
                    swap( &arr[itr], &arr[itr + 1] );   
                }
            }
            
        }

    }
} 


// ----------------------------------------------------------------------------------------------------------------
// Printing array
// ----------------------------------------------------------------------------------------------------------------
void print( long int * arr, long int size )
{
    for( long int i = 0; i < size; i++ )
    {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

}


// ----------------------------------------------------------------------------------------------------------------
// OddEven phases control
// ----------------------------------------------------------------------------------------------------------------
void PHASE( long int SEND_RANK, long int RCV_RANK, long int * arr, int size, MPI_Comm COMM )
{
    // Get the current proc.
    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);

    // temporary list to make possible the "merge"
    long int * temp_arr = new long int[size];

    // List with both proc values.
    long int * aux_arr = new long int[size*2];


    if( current_rank == SEND_RANK )
    {
        // Send with block
        // Will send the local list of the current send rank
        // This proccess will sleep until receive the sorted list.
        MPI_Send( arr, size, MPI_LONG, RCV_RANK, 0, COMM );

        // Receiving sorted list.
        MPI_Recv( arr, size, MPI_LONG, RCV_RANK, 1, COMM, MPI_STATUS_IGNORE );    
    }
    else
    {
        // Receiving from send rank his local list
        // Put the list in temporary list.
        MPI_Recv( temp_arr, size, MPI_LONG, SEND_RANK, 0, COMM, MPI_STATUS_IGNORE );

        
        // MERGE ZONE
        // =============================================
        // Merge to aux_arr with order.
        long int itr_one = 0;
        long int itr_two = 0;
        
        for( int i = 0; i < size*2; i++ )
        {
            if( temp_arr[itr_one] < arr[itr_two] )
            {
                aux_arr[i] = temp_arr[itr_one++]; 
            }
            else
            {
                aux_arr[i] = arr[itr_two++];
            }
        }
        // =============================================
        

        // DIVIDE ZONE
        // =============================================
        int itr = size;

        for( int i = 0; i < size; i++ )
        {
            arr[i] = aux_arr[i];

            temp_arr[itr] = aux_arr[itr];

            itr++;    
        }
        // =============================================
        
        delete[] aux_arr;

        // Sending back the list 
        MPI_Send( temp_arr, size, MPI_LONG, SEND_RANK, 1, COMM ); 

        delete[] temp_arr;

    }



}


// ----------------------------------------------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------------------------------------------
int main ( int argc, char *argv[] )
{

    // Processing args.
    if( argc != 2 )
    {
        std::cout << "Please, give the array size!\n";

        return EXIT_FAILURE;
    }

    if( argc == 2 and atol(argv[1]) < 1 )
    {
        std::cout << "Invalid size!\n";

        return EXIT_FAILURE;
    }
    // ------------------------------------------------------

    // MPI Vars
    int my_rank, comm_sz;

    // Starting MPI ---------------------------------------------------------------------------------------
    MPI_Init(NULL, NULL); 

    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    
    // Root vars ----------------------------------------------------
    long int size = atol(argv[1]); // Array size.

    long int * arr = gen_array(size); // main array
    // --------------------------------------------------------------
    

    // Local vars ---------------------------------------------------
    int local_size = size/comm_sz; // local list size
    
    long int * local_arr = new long int[local_size]; // local list
    // --------------------------------------------------------------


    if( my_rank == 0 )
    {  
        print( arr, size ); // printing.
    }



    // START
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Sending parts of the array to all procs.
    MPI_Scatter( arr, local_size, MPI_LONG, local_arr, local_size, MPI_LONG, 0, MPI_COMM_WORLD ); 
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    delete[] arr;


    // LOCAL SORT
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Sorting in local and starting timer.
    std::chrono::steady_clock::time_point START = std::chrono::steady_clock::now();
    odd_even_sort( local_arr, local_size ); // sorting.
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // PHASES
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    for( int proc_itr = 1; proc_itr <= comm_sz; proc_itr++ )
    {
         if( my_rank % 2 != 0 ) // Odd
         {
            if( my_rank < comm_sz - 1 )
            {
                PHASE( my_rank, my_rank + 1, local_arr, local_size, MPI_COMM_WORLD );
            }  
         }
         else if( my_rank > 0 ) // Even
         {
            PHASE( my_rank - 1, my_rank, local_arr, local_size, MPI_COMM_WORLD );  
         }
    }

    // Stoping timer.
    std::chrono::steady_clock::time_point STOP = std::chrono::steady_clock::now();
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    auto timer = (STOP - START);

    double local_time = std::chrono::duration< double > (timer).count();


    long int * final_arr = new long int[size];

    // GATHERING
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MPI_Gather( local_arr, local_size, MPI_LONG, final_arr, local_size, MPI_LONG, 0, MPI_COMM_WORLD );
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    
    if( my_rank == 0 )
    {
        /*
        // Getting final exec time
        double final_time = local_time;

        for( int proc = 1; proc < comm_sz; proc++ )
        {
            MPI_Recv( &local_time, sizeof(local_time), MPI_DOUBLE, proc, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

            if( final_time < local_time )
            {
                final_time = local_time;
            }
        }*/

        
        print( final_arr, size ); // printing.

        std::cout << std::fixed
                  << std::setprecision(3) 
                  << local_time << std::endl;

    }
    else
    {/*
        // Sending local time to root.
        MPI_Send( &local_time, sizeof(local_time), MPI_DOUBLE, 0, 3, MPI_COMM_WORLD );*/
    }

    delete[] final_arr; // freeing memory.

    MPI_Finalize();
    
    return EXIT_SUCCESS;
}
