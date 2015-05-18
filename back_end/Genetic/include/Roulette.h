//-----------------------------------------------------------
//  FORGE
//-----------------------------------------------------------
//
//      roulette.h     v1.00
//
//      Defines the Roulette Wheel index class
//
//-----------------------------------------------------------
//  Copyright 1995 by Scott Robert Ladd. All rights reserved.
//-----------------------------------------------------------

#ifndef FORGE_ROULETTE_H
#define FORGE_ROULETTE_H

#include "RandomNumbers.h"

class RW_Except { };

template < class FType >
    class RouletteWheel
        {
        public:
            // creation constructor
            RouletteWheel
                (
                size_t sz,
                FType * weights = NULL,
		int seed = 0
                );

            // copy constructor
            RouletteWheel
                (
                const RouletteWheel & rw
                );

            // assignment operator
            void operator =
                (
                const RouletteWheel & rw
                );

            // destructor
            ~RouletteWheel();

            // change the weight of an entry
            FType Change
                (
                size_t i,
                FType weight
                );

            // interrogation function
            size_t GetSize() { return N; }

            FType GetWeight
                (
                size_t i
                );

            // retrieve a random index
            size_t GetIndex();

        protected:
            // array of weights
            size_t  N;
            FType * W;

            // total weight of all indexes
            FType T;

            // shared random deviate generator
            UniformRandom G;

        private:
            // internal absolute value of a float
            FType AbsVal
                (
                FType f
                )
                {
                if (f < FType(0.0))
                    return -f;
                else
                    return f;
                }

            // internal copy function
            void Copy
                (
                const RouletteWheel & rw
                );
        };

// creation constructor
template < class FType>
    RouletteWheel<FType>::RouletteWheel
        (
        size_t  sz,
        FType * weights,
	int seed
        ) 
        {
	  
	if( seed != 0 )
	  G = UniformRandom(seed);

        size_t i;

        N = sz;
        T = FType(0);
        W = new FType [N];

        //if (W == NULL) throw RW_Except();

        if (weights == NULL)
            {
            for (i = 0; i < N; ++i)
                {
                W[i] = FType(1);
                T   += FType(1);
                }
            }
        else
            {
            for (i = 0; i < N; ++i)
                {
                W[i] = AbsVal(weights[i]);
                T   += AbsVal(weights[i]);
                }
            }
        }

template < class FType>
    void RouletteWheel<FType>::Copy
        (
        const RouletteWheel & rw
        )
        {
        T = rw.T;
        N = rw.N;
        W = new FType [N];

        //if (W == NULL) throw RW_Except();

        memcpy(W,rw.W,sizeof(FType) * N);
        }

// copy constructor
template < class FType >
    inline RouletteWheel<FType>::RouletteWheel
        (
        const RouletteWheel & rw
        )
        {
        Copy(rw);
        }

// assignment operator
template < class FType >
    inline void RouletteWheel<FType>::operator =
        (
        const RouletteWheel & rw
        )
        {
        delete [] W;
        Copy(rw);
        }

// destructor
template < class FType >
    RouletteWheel<FType>::~RouletteWheel()
        {
        delete [] W;
        }

// change the weight of an entry
template < class FType >
    FType RouletteWheel<FType>::Change
        (
        size_t i,
        FType weight
        )
        {
        if (i >= N)
            return FType(-1);

        T -= W[i];
        T += weight;

        FType res = W[i];
        W[i] = weight;
        return res;
        }

// interrogator
template < class FType >
    inline FType RouletteWheel<FType>::GetWeight
        (
        size_t i
        )
        {
        return (i < N) ? W[i] : FType(-1);
        }

// retrieve a random index
template < class FType >
    size_t RouletteWheel<FType>::GetIndex()
        {
        FType s  = FType(G.rand() * T);
        size_t i = 0;

        while ((i < N) && (s > W[i]))
            {
            s -= W[i];
            ++i;
            }

        return i;
        }

#endif

