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
    // -------------------------------------------------

    long int size = atol(argv[1]); // Array size.

    long int * arr = gen_array(size); // Generating a random array.

    //print( arr, size ); // printing.

    std::chrono::steady_clock::time_point START = std::chrono::steady_clock::now();
    odd_even_sort( arr, size ); // sorting.
    std::chrono::steady_clock::time_point STOP = std::chrono::steady_clock::now();

    auto timer = (STOP - START);

    double final_time = std::chrono::duration< double > (timer).count();

    //print( arr, size ); // printing after sort.

    delete[] arr; // freeing memory.

    std::cout << std::fixed
              << std::setprecision(3) 
              << final_time << std::endl;
    
    return EXIT_SUCCESS;
}



